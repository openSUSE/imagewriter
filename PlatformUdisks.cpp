/*
 *  Copyright (c) 2009 Novell, Inc.
 *  All Rights Reserved.
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, contact Novell, Inc.
 *  
 *  To contact Novell about this file by physical or electronic mail,
 *  you may find current contact information at www.novell.com
 *  
 *  Author: Matt Barringer <mbarringer@suse.de>
 *
 */

#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <QtCore>
#include <QtGui>
#include <QRegExp>
#include <QDir>
#include <QProgressDialog>
#include <QMessageBox>

#include "DeviceItem.h"
#include "PlatformUdisks.h"

PlatformUdisks::PlatformUdisks(bool kioskMode, bool unsafe)
    : Platform(kioskMode, unsafe)
{
}

bool
PlatformUdisks::udiskEnabled()
{
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message, reply;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DaemonVersion";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", "/org/freedesktop/UDisks", "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    reply = connection.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qDebug() << "Failure: " <<  reply;
        return false;
    }

    return true;
}

// Figure out which devices we should allow a user to write to.
void
PlatformUdisks::findDevices()
{
    int i = 0;
    if (!udiskEnabled())
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("You don't have UDisks support."));
        msgBox.exec();
        return;
    }

    // First get the list of disks
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", "/org/freedesktop/UDisks", "org.freedesktop.UDisks", "EnumerateDevices");

    QDBusReply<QList<QDBusObjectPath> > reply = connection.call(message);
    if (!reply.isValid())
    {
        qDebug() << "Failure: " <<  reply.error();
        return;
    }

    QList<QDBusObjectPath> list = reply.value();
    QStringList devList;
    QRegExp reg("[0-9]+$");

    // Ignore partition slices
    for (i = 0; i < list.size(); ++i)
        if (!list.at(i).path().contains(reg))
            devList << list.at(i).path();

    QStringList diskList;
    // Safe mode (the default) only handles USB devices
    if (!mUnsafe)
    {
        for (i = 0; i < devList.size(); ++i)
            if (isUSB(devList.at(i)))
                diskList << devList.at(i);
    }
    else
    {
        diskList = devList;
    }

    for (i = 0; i < diskList.size(); ++i)
    {
        getNewDevice(diskList.at(i));
    }

    return;
}

bool
PlatformUdisks::isUSB(const QString &udiskPath)
{
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DriveConnectionInterface";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", udiskPath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return false;

    if (reply.value().variant().toString() == "usb")
        return true;
    else
        return false;
}

DeviceItem *
PlatformUdisks::getNewDevice(QString devicePath)
{
    QString path, model, vendor;

    DeviceItem *devItem = new DeviceItem;
    path = getPath(devicePath);
    if (path == "")
        return(NULL);

    if (!getIsDrive(devicePath))
        return(NULL);

    model = getModel(devicePath);
    vendor = getVendor(devicePath);

    devItem->setUDI(devicePath);
    devItem->setPath(path);
    devItem->setIsRemovable(getIsRemovable(devicePath));
    devItem->setSize(getSize(devicePath));
    devItem->setModelString(model);
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

QString
PlatformUdisks::getPath(const QString &devicePath)
{
    QString ret = "";
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DeviceFile";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", devicePath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(ret);

    ret = reply.value().variant().toString();
    return(ret);
}

QString
PlatformUdisks::getModel(const QString &devicePath)
{
    QString ret = "";
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DriveModel";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", devicePath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(ret);

    ret = reply.value().variant().toString();
    return(ret);
}

QString
PlatformUdisks::getVendor(const QString &devicePath)
{
    QString ret = "";
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DriveVendor";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", devicePath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(ret);

    ret = reply.value().variant().toString();
    if (ret == "0000")
        ret = "";
    return(ret);
}

bool
PlatformUdisks::getIsRemovable(const QString &devicePath)
{
    bool ret;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DriveCanDetach";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", devicePath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(false);

    ret = reply.value().variant().toBool();
    return(ret);
}

long long
PlatformUdisks::getSize(const QString &devicePath)
{
    long long ret;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DeviceSize";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", devicePath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(0);

    ret = reply.value().variant().toLongLong();
    return(ret);
}


bool
PlatformUdisks::isMounted(QString path)
{
    bool ret = false;
    QStringList partitionList = getPartitionList(path);

    for (int i = 0; i < partitionList.size(); ++i)
    {
        ret = isPartitionMounted(partitionList.at(i));
        if (ret)
            break;
    }

    return(ret);
}

bool
PlatformUdisks::unmountDevice (QString path)
{
    bool ret = true;
    QStringList partitionList = getPartitionList(path);

    for (int i = 0; i < partitionList.size(); ++i)
    {
        if (isPartitionMounted(partitionList.at(i)))
        {
            ret = performUnmount(partitionList.at(i));
            if (!ret)
                break;
        }
    }

    return(ret);
}

bool
PlatformUdisks::isPartitionMounted(const QString &partitionPath)
{
    bool ret;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DeviceIsMounted";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", partitionPath, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(false);

    ret = reply.value().variant().toBool();
    qDebug() << "DeviceIsMounted for " << partitionPath << ": " << ret;
    return(ret);
}

QStringList
PlatformUdisks::getPartitionList(const QString &devicePath)
{
    QStringList devList;
    // First get the list of disks
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", "/org/freedesktop/UDisks", "org.freedesktop.UDisks", "EnumerateDevices");

    QDBusReply<QList<QDBusObjectPath> > reply = connection.call(message);
    if (!reply.isValid())
    {
        qDebug() << "Failure: " <<  reply.error();
        return(devList);
    }

    QList<QDBusObjectPath> list = reply.value();
    QRegExp reg(QString("%1[0-9]+$").arg(devicePath));
    for (int i = 0; i < list.size(); ++i)
        if (list.at(i).path().contains(reg))
            devList << list.at(i).path();

    return(devList);
}

bool
PlatformUdisks::getIsDrive(const QString &path)
{
    bool ret;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message;
    QList<QVariant> args;
    args << "org.freedesktop.UDisks";
    args << "DeviceIsDrive";

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", path, "org.freedesktop.DBus.Properties", "Get");
    message.setArguments(args);

    QDBusReply<QDBusVariant> reply = connection.call(message);
    if (!reply.isValid())
        return(false);

    ret = reply.value().variant().toBool();
    return(ret);
}

bool
PlatformUdisks::performUnmount(QString udi)
{
    bool ret = true;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message, reply;

    message = QDBusMessage::createMethodCall("org.freedesktop.UDisks", udi, "org.freedesktop.UDisks.Device", "FilesystemUnmount");
    message << QStringList();
    reply = connection.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("DBUS error (%1): %2").arg(udi).arg(reply.errorMessage()));
        msgBox.exec();
        ret = false;
    }

    return ret;
}

