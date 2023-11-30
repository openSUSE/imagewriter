#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <QtCore>
#include <QtGui>
#include <QRegExp>
#include <QDir>
#include <QProgressDialog>
#include <QMessageBox>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingReply>


#include "DeviceItem.h"
#include "PlatformUdisks2.h"
#include "udisks2_interface.h"
#include "udisks2_mountpoints_interface.h"


PlatformUdisks2::PlatformUdisks2(bool kioskMode, bool unsafe)
    : Platform(kioskMode, unsafe)
{
    qRegisterMetaType<DBUSManagerStruct>("DBUSManagerStruct");
    qDBusRegisterMetaType<DBUSManagerStruct>();

    qRegisterMetaType<QVariantMapMap>("QVariantMapMap");
    qDBusRegisterMetaType<QVariantMapMap>();

    qRegisterMetaType<ByteArrayList>("ByteArrayList");
    qDBusRegisterMetaType<ByteArrayList>();
}

bool
PlatformUdisks2::udisk2Enabled()
{
    QDBusInterface remoteApp("org.freedesktop.UDisks2",
                             "/org/freedesktop/UDisks2/Manager",
                             "org.freedesktop.UDisks2.Manager",
                             QDBusConnection::systemBus());
    QVariant reply = remoteApp.property("Version");
    if (reply.isNull())
        return false;
    return true;
}

void
PlatformUdisks2::findDevices()
{
    QRegExp reg("[1-9]+$");

    if (!udisk2Enabled())
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("You don't have UDisks2 support."));
        msgBox.exec();
        return;
    }

    org::freedesktop::DBus::ObjectManager manager("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", QDBusConnection::systemBus());
    QDBusPendingReply<DBUSManagerStruct> reply = manager.GetManagedObjects();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Failure: " <<  reply.error();
        exit(0);
    }

    Q_FOREACH(const QDBusObjectPath &path, reply.value().keys()) {
        const QString udi = path.path();
        if (!udi.startsWith("/org/freedesktop/UDisks2/block_devices"))
            continue;

        // Skip disk slices
        if (udi.contains(reg))
            continue;

        QVariantMap blockProperties = getBlockDeviceProperties(udi);
        QString drivePath = blockProperties.value("drivePath").toString();
        qlonglong size = blockProperties.value("size").toLongLong();
        QString devicePath = blockProperties.value("path").toString();

        if (drivePath.isEmpty() || size == 0 || devicePath.isEmpty())
            continue;


        QVariantMap driveProperties = getDriveProperties(drivePath);
        bool isUSB = driveProperties.value("isUSB").toBool();

        // "Safe mode" only lists USB devices
        if (!mUnsafe)
        {
            if (!isUSB)
                continue;
        }

        buildDevice(blockProperties, driveProperties);
    }
}

QVariantMap
PlatformUdisks2::getBlockDeviceProperties(const QString &blockDevice)
{
    QVariantMap properties;

    QDBusInterface remoteApp("org.freedesktop.UDisks2",
                             blockDevice,
                             "org.freedesktop.UDisks2.Block",
                             QDBusConnection::systemBus());
    QDBusObjectPath objectPath = qvariant_cast<QDBusObjectPath>(remoteApp.property("Drive"));
    QString path = objectPath.path();
    properties.insert("drivePath", path);
    properties.insert("path", QString(remoteApp.property("Device").toByteArray()));
    properties.insert("size", remoteApp.property("Size"));
    return properties;
}

QVariantMap
PlatformUdisks2::getDriveProperties(const QString &drivePath)
{
    QVariantMap properties;
    QDBusInterface remoteApp("org.freedesktop.UDisks2",
                             drivePath,
                             "org.freedesktop.UDisks2.Drive",
                             QDBusConnection::systemBus());
    properties.insert("removable", remoteApp.property("Removable"));
    properties.insert("vendor", remoteApp.property("Vendor"));
    properties.insert("serial", remoteApp.property("Serial"));
    properties.insert("model", remoteApp.property("Model"));

    if (remoteApp.property("ConnectionBus").toString().toLower() == "usb")
        properties.insert("isUSB", true);
    else
        properties.insert("isUSB", false);


    return properties;
}

DeviceItem *
PlatformUdisks2::buildDevice(QVariantMap &blockProperties, QVariantMap &driveProperties) {
    DeviceItem *devItem = new DeviceItem;

    QString path = blockProperties.value("path").toString();
    devItem->setUDI(path.mid(path.lastIndexOf("/") + 1));
    qDebug() << devItem->getUDI();
    isMounted(devItem->getUDI());
    devItem->setPath(path);
    devItem->setIsRemovable(driveProperties.value("removable").toBool());
    devItem->setSize(blockProperties.value("size").toLongLong());
    devItem->setModelString(driveProperties.value("model").toString());
    QString vendor = driveProperties.value("vendor").toString();

    if (vendor == "")
    {
        if (mKioskMode)
            devItem->setVendorString("SUSE Studio USB Key");
        else
            devItem->setVendorString("Unknown Device");
    }
    else
    {
        devItem->setVendorString(vendor);
    }
    QString newDisplayString = QString("%1 %2 - %3 (%4 MB)")
                                      .arg(devItem->getVendorString())
                                      .arg(devItem->getModelString())
                                      .arg(devItem->getPath())
                                      .arg(devItem->getSize() / 1048576);
    devItem->setDisplayString(newDisplayString);

    if (mKioskMode)
    {
        if((devItem->getSize() / 1048576) > 200000)
        {
            delete devItem;
            return(NULL);
        }
    }

    // If a device is 0 megs we might as well just not list it
    if ((devItem->getSize() / 1048576) > 0)
    {
        itemList << devItem;
    }
    else
    {
        delete devItem;
        devItem = NULL;
    }

    return(devItem);
}

DeviceItem *
PlatformUdisks2::getNewDevice(QString newPath)
{
    QVariantMap blockProperties = getBlockDeviceProperties(newPath);
    QString drivePath = blockProperties.value("drivePath").toString();
    qlonglong size = blockProperties.value("size").toLongLong();
    QString devicePath = blockProperties.value("path").toString();

    if (drivePath.isEmpty() || size == 0 || devicePath.isEmpty())
        return NULL;

    QVariantMap driveProperties = getDriveProperties(drivePath);
    bool isUSB = driveProperties.value("isUSB").toBool();

    if (!mUnsafe)
    {
        if (!isUSB)
            return NULL;
    }

    return buildDevice(blockProperties, driveProperties);
}

bool
PlatformUdisks2::isMounted(QString path)
{
    bool mounted = false;
    QStringList partitions = getPartitionList(path);
    foreach(QString partition, partitions)
    {
        if (isPartitionMounted(partition))
        {
            mounted = true;
            break;
        }
    }
    if (!mounted) {
	mounted = isPartitionMounted(QString("/org/freedesktop/UDisks2/block_devices/") + path);
    }

    return mounted;
}

QStringList
PlatformUdisks2::getPartitionList(const QString &devicePath)
{
    QStringList partitionList;
    org::freedesktop::DBus::ObjectManager manager("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", QDBusConnection::systemBus());
    QDBusPendingReply<DBUSManagerStruct> reply = manager.GetManagedObjects();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Failure: " <<  reply.error();
        exit(0);
    }

    QRegExp reg(QString("%1[0-9]+$").arg(devicePath));

    Q_FOREACH(const QDBusObjectPath &path, reply.value().keys()) {
        const QString udi = path.path();
        if (!udi.startsWith("/org/freedesktop/UDisks2/block_devices"))
            continue;

        if (!udi.contains(reg))
            continue;
        partitionList << udi;
    }

    return partitionList;
}

bool
PlatformUdisks2::isPartitionMounted(const QString &partitionPath)
{
    org::freedesktop::UDisks2::Filesystem manager("org.freedesktop.UDisks2", partitionPath, QDBusConnection::systemBus());
    ByteArrayList reply = manager.mountPoints();
    if (reply.isEmpty())
    {
        qDebug() << partitionPath << "not mounted";
        return false;
    }

    return true;
}

bool
PlatformUdisks2::unmountDevice(QString path)
{
    bool res = true;
    QStringList partitions = getPartitionList(path);
    if (partitions.empty())
    {
        res = doUnmount(QString("/org/freedesktop/UDisks2/block_devices/") + path);
    }
    else
    {
        foreach(QString partition, partitions)
        {
            if (isPartitionMounted(partition) && !doUnmount(partition))
            {
                res = false;
                break;
            }
        }
    }

    return(res);
}

bool
PlatformUdisks2::doUnmount(const QString &partitionPath)
{
    bool ret = true;
    QDBusConnection connection = QDBusConnection::systemBus();
    QList<QVariant> args;
    QVariantMap map;
    args << map;

    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", partitionPath, "org.freedesktop.UDisks2.Filesystem", "Unmount");
    message.setArguments(args);
    QDBusMessage reply = connection.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("DBUS error (%1): %2").arg(partitionPath).arg(reply.errorMessage()));
        msgBox.exec();
        ret = false;
    }
    return(ret);
}

int
PlatformUdisks2::open(DeviceItem* item)
{
    int ret = -1;

    QDBusConnection connection = QDBusConnection::systemBus();
    QList<QVariant> args;
    QVariantMap map;
    args << map;

    QString udi = QString("/org/freedesktop/UDisks2/block_devices/%1").arg(item->getUDI());
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", udi, "org.freedesktop.UDisks2.Block", "OpenForRestore");
    message.setArguments(args);
    QDBusMessage reply = connection.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("DBUS error (%1): %2").arg(item->getUDI()).arg(reply.errorMessage()));
        msgBox.exec();
        ret = -1;
    }
    else if (reply.arguments().empty() || reply.arguments()[0].userType() != qMetaTypeId<QDBusUnixFileDescriptor>())
    {
        QMessageBox msgBox;
        msgBox.setText(QString("got empty or invalid reply!?"));
        msgBox.exec();
	errno = EINVAL;
    }
    else
    {
	QDBusUnixFileDescriptor fd(qvariant_cast<QDBusUnixFileDescriptor>(reply.arguments()[0]));
	if (fd.isValid())
	{
	    ret = ::dup(fd.fileDescriptor());
	}
    }

    qDebug() << "ret" << ret;

    return ret;
}

