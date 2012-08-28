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

#ifndef __DEVICE_ITEM_H__
#define __DEVICE_ITEM_H__

#include <QWidget>
#include <QDebug>

// This class represents the devices we find
class DeviceItem
{
public:
    DeviceItem() {}

    QString getPath() { return mPath; }
    QString getVendorString() { return mVendorString; }
    QString getModelString() { return mModelString; }
    QString getDisplayString() { return mDisplayString; }
    QString getUDI() { return mUDI; }
    qint64 getSize() { return mSize; }
    bool isRemovable() { return mIsRemovable; }

    void setPath(QString path) { mPath = path; }
    void setVendorString(QString vendor) { mVendorString = vendor; }
    void setModelString(QString modelString) { mModelString = modelString; }
    void setDisplayString(QString str) { mDisplayString = str; }
    void setSize(qint64 size) { mSize = size; }
    void setUDI(QString UDI) { mUDI = UDI; }
    void setIsRemovable(bool removable) { mIsRemovable = removable; }

private:
    QString mPath, // Path to the device (example: /dev/sdb)
            mUDI, // UDI for HAL
            mVendorString, // The vendor 
            mModelString, // the model string
            mDisplayString; // The string used in the pulldown device selection display
    qint64 mSize;
    bool mIsRemovable;
};

#endif
