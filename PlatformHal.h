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

#ifndef __PLATFORMHAL_H__
#define __PLATFORMHAL_H__

#ifdef USEHAL
#include <QtCore>
#include <hal/libhal.h>
#include <hal/libhal-storage.h>

#include <QtDBus>

#include "DeviceItem.h"
#include "Platform.h"

class PlatformHal : public Platform
{

public:
    PlatformHal(bool kioskMode = false, bool unsafe = false);
    void findDevices();
    bool isMounted(QString path);
    bool unmountDevice(QString path);
    DeviceItem *getNewDevice(QString devicePath);

private:
    bool performUnmount(QString udi);
    DeviceItem *getNewDevice(QString devicePath, LibHalContext *context = NULL);
    LibHalContext *initHal();

};
#endif

#endif
