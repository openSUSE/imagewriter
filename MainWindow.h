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

#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtGui>
#include <QWidget>
#include <QDBusMessage>

#include "Platform.h"
#include "DeviceItem.h"
#define DROPDOWN_DIRECTIVE "Insert a USB device"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(Platform *platform,
               const char *cmddevice,
               const char *cmdfile,
               bool unsafe = false,
               bool maximized = false,
               QWidget *parent = 0);

public slots:
    void selectImage();
    void deviceInserted(QDBusMessage message);
    void deviceRemoved(QDBusMessage message);

private slots:
    void write();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void setSizeLabel(const QString &fileName);

private:
    void addMenuItem(const QString &item);
    int removeMenuItem(const QString &item);
    void setFile(const QString &newFile);
    void divineMeaning(const QString &path);
    void divineFurther(DeviceItem *item);
    bool isMounted(const QString &path);
    void writeData(const QString &path);
    bool checkIso(const QString &fileName);
    void centerWindow();
    void useNewUI();
    void useOldUI();
    void reloadDeviceList(const char *cmddevice);

#if (QT_VERSION < 0x040400)
    QLineEdit* fileLine;
#endif

    QLabel *imageLabel, *directive;
    QString file;
    QLabel *fileSize, *fileLabel;
    QComboBox *deviceComboBox;
    Platform *pPlatform;
    bool mMaximized;
    bool mUnsafe;
};

// Rather than grabbing a mouse click for the entire window, just grab it for the part
// that contains the graphics
class CustomLabel : public QLabel
{
public:
    CustomLabel(QWidget* parent);

protected:
    void mousePressEvent(QMouseEvent *event);
};

// About box "link"
class AboutLabel : public QLabel
{
public:
    AboutLabel(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};
#endif
