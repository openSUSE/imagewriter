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

#ifdef USEHAL

#include <QtCore>
#include <QtGui>
#include <QDir>
#include <QProgressDialog>

#include "DeviceItem.h"
#include "PlatformHal.h"

PlatformHal::PlatformHal(bool kioskMode, bool unsafe)
    : Platform(kioskMode, unsafe)
{
}

// Figure out which devices we should allow a user to write to.
void
PlatformHal::findDevices()
{
    char **drives;
    int drive_count, i;
    LibHalContext *context;

    DeviceItem *devItem = NULL;

    if ((context = initHal()) == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Could not initialize HAL."));
        msgBox.exec();
        return;
    }

    // We want to only write to USB drives, unless the user specifies
    // the unsafe flag on the command line
    if (mUnsafe)
        drives = libhal_manager_find_device_string_match(context,
                                                        "storage.drive_type",
                                                        "disk",
                                                        &drive_count,
                                                        NULL);
    else
        drives = libhal_manager_find_device_string_match(context,
                                                        "storage.bus",
                                                        "usb",
                                                        &drive_count,
                                                        NULL);

    for(i = 0; i < drive_count; i++)
    {
        devItem = getNewDevice(drives[i], context);
    }

    libhal_free_string_array(drives);
    libhal_ctx_shutdown(context, NULL);
    libhal_ctx_free(context);
    return;
}

DeviceItem *
PlatformHal::getNewDevice(QString devicePath)
{
    return getNewDevice(devicePath, NULL);
}

DeviceItem *
PlatformHal::getNewDevice(QString devicePath, LibHalContext *context)
{
    char *device, *product, *vendor;
    long long size;
    bool isRemovable = true;
    DeviceItem *devItem = NULL;
    LibHalContext *localContext = context;

    // context / localContext is NULL when getNewDevice is called from the main window
    // when a USB device is inserted
    if (localContext == NULL)
    {
        if ((localContext = initHal()) == NULL)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Could not initialize HAL."));
            msgBox.exec();
            return(NULL);
        }
    }

    device = libhal_device_get_property_string(localContext,
                                               devicePath.toAscii(),
                                               "block.device",
                                               NULL);
    if (device != NULL)
    {

        product = libhal_device_get_property_string(localContext,
                                                    devicePath.toAscii(),
                                                    "info.product",
                                                    NULL);

        vendor = libhal_device_get_property_string(localContext,
                                                   devicePath.toAscii(),
                                                   "info.vendor",
                                                   NULL);
        size = libhal_device_get_property_uint64(localContext,
                                                 devicePath.toAscii(),
                                                 "storage.removable.media_size",
                                                 NULL);

        isRemovable = libhal_device_get_property_bool(localContext,
                                                      devicePath.toAscii(),
                                                      "storage.removable",
                                                      NULL);

        devItem = new DeviceItem;
        devItem->setUDI(devicePath);
        devItem->setPath(device);
        devItem->setIsRemovable(isRemovable);
        devItem->setSize(size);

        if (!strcmp(product, ""))
            devItem->setModelString("");
        else
            devItem->setModelString(product);

        if (!strcmp(vendor, ""))
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
        QString newDisplayString = QString("%1 %2 - %3 (%4 MB)").arg(devItem->getVendorString()).arg(devItem->getModelString()).arg(devItem->getPath()).arg(devItem->getSize() / 1048576);
        devItem->setDisplayString(newDisplayString);

        if (mKioskMode)
        {
            if((devItem->getSize() / 1048576) > 200000)
            {
                delete devItem;
                libhal_free_string(device);
                libhal_free_string(product);
                libhal_free_string(vendor);
                if (context == NULL)
                {
                    libhal_ctx_shutdown(localContext, NULL);
                    libhal_ctx_free(localContext);
                }

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
        
        libhal_free_string(device);
        libhal_free_string(product);
        libhal_free_string(vendor);
    }

    if (context == NULL)
    {
        libhal_ctx_shutdown(localContext, NULL);
        libhal_ctx_free(localContext);
    }

    return(devItem);
}

LibHalContext *
PlatformHal::initHal()
{
    DBusError error;
    DBusConnection *dbus_connection;
    LibHalContext *context;
    char **devices;
    int device_count;

    if ((context = libhal_ctx_new()) == NULL)
        return(NULL);

    dbus_error_init(&error);
    dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if(dbus_error_is_set(&error))
    {
            dbus_error_free(&error);
            libhal_ctx_free(context);
            return(NULL);
    }

    libhal_ctx_set_dbus_connection(context, dbus_connection);
    if(!libhal_ctx_init(context, &error))
    {
            dbus_error_free(&error);
            libhal_ctx_free(context);
            return(NULL);
    }

    devices = libhal_get_all_devices(context, &device_count, NULL);
    if(devices == NULL)
    {
            libhal_ctx_shutdown(context, NULL);
            libhal_ctx_free(context);
            context = NULL;
            return(NULL);
    }

    libhal_free_string_array(devices);
    return(context);
}

bool
PlatformHal::isMounted(QString path)
{
    LibHalContext *context;
    LibHalVolume *halVolume;
    bool ret = false;
    char **volumes;
    int volumeCount, i;
    
    if ((context = initHal()) == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Could not initialize HAL."));
        msgBox.exec();
        return false;
    }
    
    volumes = libhal_manager_find_device_string_match(context,
                                                      "info.parent",
                                                      path.toLatin1().data(),
                                                      &volumeCount,
                                                      NULL);
    for(i = 0; i < volumeCount; i++)
    {
        halVolume = libhal_volume_from_udi(context, volumes[i]);
        // I don't really know if this is better than just looking for the volume.is_mounted property,
        // might as well be on the safe side.
        if (libhal_volume_is_mounted(halVolume))
            ret = true;
        
        libhal_volume_free(halVolume);
    }

    libhal_free_string_array(volumes);
    libhal_ctx_shutdown(context, NULL);
    libhal_ctx_free(context);
    return ret;
}

bool
PlatformHal::unmountDevice (QString path)
{
    LibHalContext *context;
    bool ret = true;
    char **volumes;
    int volumeCount, i;
    
    if ((context = initHal()) == NULL)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Could not initialize HAL."));
        msgBox.exec();
        return false;
    }
    
    volumes = libhal_manager_find_device_string_match(context,
                                                      "info.parent",
                                                      path.toLatin1().data(),
                                                      &volumeCount,
                                                      NULL);
    for(i = 0; i < volumeCount; i++)
    {
        if (!performUnmount(volumes[i]))
            ret = false;
    }

    libhal_free_string_array(volumes);
    libhal_ctx_shutdown(context, NULL);
    libhal_ctx_free(context);
    return ret;
}


bool
PlatformHal::performUnmount(QString udi)
{
    bool ret = true;
    QDBusConnection connection = QDBusConnection::systemBus();
    QDBusMessage message, reply;
    QList<QVariant> options;

    message = QDBusMessage::createMethodCall("org.freedesktop.Hal", udi, "org.freedesktop.Hal.Device.Volume", "Unmount");
    message << QStringList();
    reply = connection.call(message);

    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qDebug() << "Failure: " <<  reply;
        ret = false;
    }

    return ret;
}
#endif
