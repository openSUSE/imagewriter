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

#include <QApplication>

#ifndef Q_OS_LINUX
#error "Only linux is supported at the moment"
#endif

#include <unistd.h>
#include <sys/types.h>
#include "MainWindow.h"
#include "PlatformHal.h"
#include "PlatformUdisks.h"
#include "PlatformUdisks2.h"
#include "DeviceItem.h"


int
main (int argc, char *argv[])
{
    int c;
    char *device = NULL;
    char *file = NULL;
    bool unsafe = false;
    bool maximized = false;
    bool listMode = false;
    bool kioskMode = false;
    qDebug() << "Starting up...";
#if defined(Q_OS_UNIX) 
#if !defined(KIOSKHACK) && !defined(USEUDISKS2)
    if (getuid() != 0)
        qFatal("You must run this program as the root user.");
#endif
#endif

    while ((c = getopt (argc, argv, "mlkvuhd:f:")) != -1)
    {
        switch (c)
        {
            case 'h':
                fprintf(stdout, "Usage:\t%s [-d <device>] [-f <raw file>] [-u] [-l] [-v]\n", argv[0]);
                fprintf(stdout, "Flashes a raw disk file to a device\n\n");
                fprintf(stdout, "-d <device>\t\tSpecify a device, for example: /dev/sdc\n");
                fprintf(stdout, "-f <raw file>\t\tSpecify the file to write\n");
                fprintf(stdout, "-k\t\t\tOperate in \"kiosk mode\", only listing disks smaller than 200GB\n");
                fprintf(stdout, "-l\t\t\tList valid USB devices\n");
                fprintf(stdout, "-m\t\t\tMaximize the window\n");
                fprintf(stdout, "-u\t\t\tOperate in unsafe mode, listing all disks, not just removable ones\n");
                fprintf(stdout, "-v\t\t\tVersion and author information\n");
                exit(0);
            case 'u':
                unsafe = true;
                break;
            case 'd':
                device = strdup(optarg);
                break;
            case 'f':
                file = strdup(optarg);
                break;
            case 'l':
                listMode = true;
                break;
            case 'k':
                kioskMode = true;
                break;
            case 'v':
                fprintf(stdout, "SUSE Studio Imagewriter %s\nWritten by Matt Barringer <mbarringer@suse.de>\n", APP_VERSION);
                exit(0);
                break;
            case 'm':
                 maximized = true;
                 break;
            default:
                break;
        }
    }

    QApplication app(argc, argv);
#ifdef USEHAL
    PlatformHal *platform = new PlatformHal(kioskMode, unsafe);
#elif USEUDISKS2
    PlatformUdisks2 *platform = new PlatformUdisks2(kioskMode, unsafe);
#else
    PlatformUdisks *platform = new PlatformUdisks(kioskMode, unsafe);
#endif
    platform->findDevices();

    if (listMode)
    {
        QList<DeviceItem *> list = platform->getDeviceList();
        QList<DeviceItem *>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
        {
            if (!(*i)->getPath().isEmpty())
                fprintf(stdout, "%s\n", (*i)->getPath().toLatin1().data());
        }
        exit(0);
    }
    
    MainWindow window(platform, device, file, unsafe, maximized);
    if (maximized)
    {
        window.showMaximized();
    }
    else
    {
        window.show();
    }
    return app.exec();
}
