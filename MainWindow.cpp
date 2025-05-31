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


#include <QtGui>
#include <QFileDialog>
#include <QProgressDialog>
#include <QtDBus>
#include <QFile>
#include <QDebug>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QPushButton>
#include <QScreen>
#include <QMessageBox>
#include <QApplication>

#include <unistd.h>
#include <sys/types.h>

#ifdef USEUDISKS2
#include "udisks2_interface.h"
#endif


#include "MainWindow.h"

MainWindow::MainWindow (Platform *platform,
                        const char *cmddevice,
                        const char *cmdfile,
                        bool unsafe,
                        bool maximized,
                        QWidget *parent)
 : QWidget(parent)
{
    pPlatform = platform;
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    file = QString();
    mMaximized = maximized;
    mUnsafe = unsafe;
    fileSize = new QLabel("      ");
    fileLabel = new QLabel("     ");

// The "new" UI won't compile on 10.3 or SLE10, so fallback in that case to the older, uglier one
#if (QT_VERSION >= 0x040400)
    useNewUI();
#else
    useOldUI();
#endif

    setWindowTitle(QString("SUSE Studio Imagewriter %1").arg(APP_VERSION));
    reloadDeviceList(cmddevice);

    if (cmdfile != NULL)
    {
        if(QFile(cmdfile).exists())
        {
          setFile(cmdfile);
          setSizeLabel(cmdfile);
        }
    }

#ifdef USEHAL
    // Hook into DBUS insertion and removal notifications
    dbusConnection.connect("",
                           "/org/freedesktop/Hal/Manager",
                           "org.freedesktop.Hal.Manager",
                           "DeviceAdded",
                           this,
                           SLOT(deviceInserted(QDBusMessage)));

    dbusConnection.connect("",
                           "/org/freedesktop/Hal/Manager",
                           "org.freedesktop.Hal.Manager",
                           "DeviceRemoved",
                           this,
                           SLOT(deviceRemoved(QDBusMessage)));
#else // USEUDISKS2
    qDebug() << "Using udisks2";
    org::freedesktop::DBus::ObjectManager manager("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", QDBusConnection::systemBus());
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded",
                                         this, SLOT(deviceInserted(QDBusObjectPath,QVariantMapMap)));
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved",
                                         this, SLOT(deviceRemoved(QDBusObjectPath,QStringList)));
#endif
    if (!mMaximized)
        centerWindow();
}

void
MainWindow::reloadDeviceList(const char *cmddevice)
{
    int dev = -1;
    QList<DeviceItem *> list = pPlatform->getDeviceList();
    QList<DeviceItem *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        if (!(*i)->getPath().isEmpty())
            if (deviceComboBox->findText((*i)->getDisplayString()) == -1)
                addMenuItem((*i)->getDisplayString());

        if (cmddevice != NULL)
            if ((*i)->getPath().compare(cmddevice) == 0)
                dev = deviceComboBox->findText((*i)->getDisplayString(), Qt::MatchExactly);
    }

    if (dev != -1)
        deviceComboBox->setCurrentIndex(dev);
}

void
MainWindow::useNewUI()
{
#if (QT_VERSION >= 0x040400)
    QVBoxLayout *mainLayout;
    QStackedLayout *logoLayout;
    QGridLayout *bottomLayout;

    QHBoxLayout *pathSizeLayout;
    QPushButton *writeButton;

    imageLabel = new CustomLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    imageLabel->setScaledContents(false);
    QImage image(":logo-empty.png");
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->setAlignment(Qt::AlignCenter);

    directive = new CustomLabel(this);
    directive->setText(tr("Drag disk image here\n or click to select."));
    directive->setAlignment(Qt::AlignCenter);
    deviceComboBox = new QComboBox;
    deviceComboBox->addItem(DROPDOWN_DIRECTIVE);
    writeButton = new QPushButton(tr("Write"));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(write()));

    // These layouts are kind of a mess
    logoLayout = new QStackedLayout;
    logoLayout->setStackingMode(QStackedLayout::StackAll);
    logoLayout->addWidget(directive);
    logoLayout->addWidget(imageLabel);

    pathSizeLayout = new QHBoxLayout;
    pathSizeLayout->addWidget(fileLabel, Qt::AlignLeft);
    pathSizeLayout->addWidget(fileSize, Qt::AlignLeft);

    bottomLayout = new QGridLayout;
    bottomLayout->addLayout(pathSizeLayout, 0, 0);

    mainLayout = new QVBoxLayout;
    AboutLabel *aboutLabel = new AboutLabel();
    aboutLabel->setText("<u>About</u>");

    mainLayout->addWidget(aboutLabel,0,Qt::AlignTop|Qt::AlignLeft);
    mainLayout->addLayout(logoLayout, Qt::AlignHCenter);

    QGridLayout *comboLayout = new QGridLayout;
    comboLayout->addLayout(bottomLayout, 0, 0, Qt::AlignBottom);
    comboLayout->addWidget(deviceComboBox, 1,0);
    comboLayout->addWidget(writeButton, 1, 1, Qt::AlignRight);
    mainLayout->addLayout(comboLayout);

    setLayout(mainLayout);
    if (!mMaximized)
        resize(600, 400);

    setAcceptDrops(true);
#endif

    return;
}

void
MainWindow::useOldUI()
{
#if (QT_VERSION < 0x040400)
    QGridLayout *mainLayout;
    QHBoxLayout *fileSelectLayout, *buttonLayout;

    QLabel *file, *device, *version;
    QPushButton *fileSelectButton, *exitButton, *writeButton;

    fileLine = new QLineEdit;
    deviceComboBox = new QComboBox;

    fileSelectButton = new QPushButton(tr("Select"));
    connect(fileSelectButton, SIGNAL(clicked()), this, SLOT(selectImage()));

    exitButton = new QPushButton(tr("Exit"));
    connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    writeButton = new QPushButton(tr("Write"));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(write()));

    file = new QLabel(tr("File"));
    device = new QLabel(tr("Device"));
    version = new QLabel(QString("SUSE Studio Imagewriter %1").arg(APP_VERSION));

    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(exitButton);
    buttonLayout->addWidget(writeButton);

    fileSelectLayout = new QHBoxLayout;
    fileSelectLayout->addWidget(fileLine);
    fileSelectLayout->addWidget(fileSelectButton);

    mainLayout = new QGridLayout;
    mainLayout->addWidget(file, 0, 0);
    mainLayout->addLayout(fileSelectLayout, 0, 1);
    mainLayout->addWidget(device, 1, 0);
    mainLayout->addWidget(deviceComboBox, 1, 1);

    mainLayout->addLayout(buttonLayout, 2, 0, Qt::AlignRight);
    setLayout(mainLayout);
    if (!mMaximized)
        resize(600,170);
#endif

    return;
}

void
MainWindow::centerWindow()
{
    QScreen *screen = QApplication::primaryScreen();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = screen->geometry().width();
    screenHeight = screen->geometry().height();

    windowSize = size();
    width = windowSize.width();
    height = windowSize.height();

    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

    move ( x, y );
}

void
MainWindow::selectImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Open Image"),
                        QDir::currentPath(),
                        tr("Image Files (*.raw *.iso *.img)"));
    if (!fileName.isEmpty())
    {
        setFile(fileName);
        setSizeLabel(fileName);
        if (fileName.endsWith(".iso"))
            checkIso(fileName);
    }

    return;
}

bool
MainWindow::checkIso(const QString &fileName)
{
    // For now we only support writing hybrid ISOs, so we need to check if there's an MBR signature.
    // It'll be in the last two bytes of the boot record.
    QByteArray mbr;
    QFile mbrTest(fileName);
    mbrTest.open(QIODevice::ReadOnly);
    mbrTest.seek(510);
    mbr = mbrTest.read(2);
    mbrTest.close();
    // I think it's safe to assume that we'll only be encountering little-endian boot images for a while.
    // If that changes, we'll need to test for 0xAA55
    if (mbr.toHex() != "55aa")
    {
        setFile("");
        setSizeLabel("");
        directive->setText(tr("Drag disk image here\n or click to select."));
        QMessageBox msgBox;
        msgBox.setText(tr("Sorry, I can't write this ISO.  You need to use another program to write it to a DVD."));
        msgBox.exec();
        return(false);
    }

    return(true);
}

// UDisks2 insertion handler
void MainWindow::deviceInserted(const QDBusObjectPath &object_path,
                                const QVariantMapMap &interfaces_and_properties)
{
    Q_UNUSED(interfaces_and_properties);

    QRegularExpression reg("[0-9]+$");
    QString path = object_path.path();

    if (!path.startsWith("/org/freedesktop/UDisks2/block_devices"))
        return;

    if (path.contains(reg))
        return;

    DeviceItem *device = pPlatform->getNewDevice(path);
    if (device != NULL)
        if (deviceComboBox->findText(device->getDisplayString()) == -1)
            addMenuItem(device->getDisplayString());

}

// UDisks2 removal handler
void MainWindow::deviceRemoved(const QDBusObjectPath &object_path,
                               const QStringList &interfaces)
{
    Q_UNUSED(interfaces);

    QRegularExpression reg("[0-9]+$");
    QString path = object_path.path();

    if (!path.startsWith("/org/freedesktop/UDisks2/block_devices"))
        return;

    if (path.contains(reg))
        return;

    QString udi = path.mid(path.lastIndexOf("/") + 1);
    QList<DeviceItem *> list = pPlatform->getDeviceList();
    QList<DeviceItem *>::iterator i;
    for (i = list.begin(); i != list.end(); ++i)
    {
        if ((*i)->getUDI() == udi)
        {
            if (removeMenuItem((*i)->getDisplayString()) != -1)
            {
                pPlatform->removeDeviceFromList(path);
                break;
            }
        }
    }

}

// UDisks & HAL insertion handler
void
MainWindow::deviceInserted(QDBusMessage message)
{
    QString devicePath;
#ifdef USEHAL
    devicePath = message.arguments().at(0).toString();
    if (devicePath.startsWith("/org/freedesktop/Hal/devices/storage_serial"))
#else
    QDBusObjectPath path = message.arguments().at(0).value<QDBusObjectPath>();
    devicePath = path.path();
    if (devicePath.startsWith("/org/freedesktop/UDisks/devices/"))
#endif
    {
        DeviceItem *device = pPlatform->getNewDevice(devicePath);
        if (device != NULL)
            if (deviceComboBox->findText(device->getDisplayString()) == -1)
                addMenuItem(device->getDisplayString());
    }
}

// UDisks & HAL removal handler
void
MainWindow::deviceRemoved(QDBusMessage message)
{
    QString devicePath;
#ifdef USEHAL
    devicePath = message.arguments().at(0).toString();
    if (devicePath.startsWith("/org/freedesktop/Hal/devices/storage_serial"))
#else
    QDBusObjectPath path = message.arguments().at(0).value<QDBusObjectPath>();
    devicePath = path.path();
    if (devicePath.startsWith("/org/freedesktop/UDisks/devices/"))
#endif
    {
        QList<DeviceItem *> list = pPlatform->getDeviceList();
        QList<DeviceItem *>::iterator i;
        for (i = list.begin(); i != list.end(); ++i)
        {
            if ((*i)->getUDI() == devicePath)
            {
                if (removeMenuItem((*i)->getDisplayString()) != -1)
                {
                    pPlatform->removeDeviceFromList(devicePath);
                    break;
                }
            }
        }
    }
}

void
MainWindow::addMenuItem(const QString &item)
{
    if (deviceComboBox->itemText(0) == DROPDOWN_DIRECTIVE)
        deviceComboBox->removeItem(0);
    deviceComboBox->addItem(item, 0);
}

int
MainWindow::removeMenuItem(const QString &item)
{
    int index = deviceComboBox->findText(item);
    if (index != -1)
    {
        deviceComboBox->removeItem(index);
        if (deviceComboBox->count() == 0)
            deviceComboBox->addItem(DROPDOWN_DIRECTIVE);
    }
    return(index);
}

void
MainWindow::setSizeLabel(const QString &fileName)
{
    if (fileName != "")
    {
        QFile filecheck(fileName);
        if(filecheck.exists())
        {
            int size = filecheck.size() / (1024*1024);
            fileSize->setText("(<b>" + QString::number(size) + " MB</b>)" );
        }
    }
    else
    {
        fileSize->setText("");
    }
    return;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
#if 0
    qDebug() << event->mimeData()->text();
    qDebug() << event->mimeData()->formats();
    qDebug() << event->mimeData()->urls();
#endif
    if (event->mimeData()->hasFormat("text/uri-list"))
        if ((event->mimeData()->urls()[0].toString().endsWith("iso")) || (event->mimeData()->urls()[0].toString().endsWith("raw")))
            event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QString file = event->mimeData()->urls()[0].toLocalFile();
    setFile(file);
    setSizeLabel(file);
    if (file.endsWith("iso"))
        checkIso(file);

}

void MainWindow::setFile(const QString &newFile)
{
    file = newFile;
    if (newFile != "")
    {
    #if (QT_VERSION >= 0x040400)
        QImage image(":logo-mini.png");
        imageLabel->setPixmap(QPixmap::fromImage(image));
        directive->setText("");
    #else
        fileLine->setText(file);
    #endif

        fileLabel->setText("<b>Selected:</b> " + file);
    }
    else
    {
        fileLabel->setText("");
    #if (QT_VERSION >= 0x040400)
        QImage image(":logo-empty.png");
        imageLabel->setPixmap(QPixmap::fromImage(image));
        directive->setText("");
    #else
        fileLine->setText("");
    #endif
    }
}

void
MainWindow::write()
{
    if (file.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Please select an image to use."));
        msgBox.exec();
        return;
    }

    DeviceItem *item = pPlatform->findDeviceInList(deviceComboBox->currentText());

    if (item != NULL)
    {
        if (pPlatform->isMounted(item->getUDI()))
        {
            // We won't let them nuke a mounted device
            QMessageBox msgBox;
            msgBox.setText(tr("This device is already mounted, and I will not write to a mounted device.  Would you like me to attempt to unmount it?"));
#if (QT_VERSION >= 0x040400)
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
#else
            msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
            msgBox.setButtonText(QMessageBox::No, tr("No"));
#endif

            switch (msgBox.exec())
            {
                case QMessageBox::Yes:
                {
                    if (!pPlatform->unmountDevice(item->getUDI()))
                    {
                        QMessageBox failedBox;
                        failedBox.setText(tr("Unmount failed.  I will not write to this device."));
                        failedBox.exec();
                        return;
                    }
                    break;
                }
                case QMessageBox::No:
                    return;
                default:
                    break;
            }
        }

        QMessageBox msgBox;
        QString messageString;
        if (item->isRemovable())
            messageString = tr("This will overwrite the contents of ") + item->getPath() + tr(".  Are you sure you want to continue?");
        else
            messageString = item->getPath() + tr(" is a non-removable hard drive, and this will overwrite the contents.  Are you <b>sure</b> you want to continue?");
        msgBox.setText(messageString);

#if (QT_VERSION >= 0x040400)
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Cancel);
#else
        msgBox.setButtonText(QMessageBox::Cancel, tr("Cancel"));
        msgBox.setButtonText(QMessageBox::Ok, tr("Ok"));
#endif


        switch (msgBox.exec())
        {
            case QMessageBox::Ok:
            {
                pPlatform->writeData(item, file);
                break;
            }
            default:
                break;
        }
    }
}

CustomLabel::CustomLabel(QWidget* parent)
 : QLabel(parent)
{
}

void CustomLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        MainWindow *window = (MainWindow *) parentWidget();
        window->selectImage();
    }
}

AboutLabel::AboutLabel(QWidget *parent)
 : QLabel(parent)
{
    setStyleSheet("color: grey");
}

void
AboutLabel::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QMessageBox about(QMessageBox::Information, "About SUSE Studio Imagewriter",
                   "The <b>SUSE Studio Imagewriter</b> is (C) 2012, SUSE Linux Products GmbH<br><br>\
                   It is cheerfully released under the GPL v2 license.  You can find the source code on github: https://github.com/mbarringer/imagewriter<br><br>\
                   It was written by Matt Barringer &lt;matt@incoherent.de&gt;.  Please send complaints directly to him.");
    about.exec();
}

void
AboutLabel::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::PointingHandCursor);
}

void
AboutLabel::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::ArrowCursor);
}

