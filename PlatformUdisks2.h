#ifndef __PLATFORMUDISKS2_H__
#define __PLATFORMUDISKS2_H__

#include "PlatformUdisks.h"

class PlatformUdisks2 : public Platform
{
public:
    PlatformUdisks2(bool kioskMode = false, bool unsafe = false);
    void findDevices();
    DeviceItem *getNewDevice(QString newPath);
    bool isMounted(QString path);
    bool unmountDevice(QString path);

private:
    bool udisk2Enabled();
    QString getDrivePath(const QString &blockDevice);
    QStringList getPartitionList(const QString &devicePath);
    bool isPartitionMounted(const QString &partitionPath);
    bool doUnmount(const QString &partitionPath);
    QVariantMap getBlockDeviceProperties(const QString &blockDevice);
    QVariantMap getDriveProperties(const QString &drivePath);
    DeviceItem *buildDevice(QVariantMap &blockProperties, QVariantMap &driveProperties);
};

#endif
