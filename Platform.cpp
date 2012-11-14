#include "Platform.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>
#include <errno.h>

#define BLOCKSIZE 1048576
#define _GNU_SOURCE

Platform::Platform(bool kioskMode, bool unsafe)
{
     mKioskMode = kioskMode;
     mUnsafe = unsafe;
}

bool
Platform::removeDeviceFromList(const QString &displayName)
{
    DeviceItem *item = NULL;
    QLinkedList<DeviceItem *>::iterator i;
    for (i = itemList.begin(); i != itemList.end(); ++i)
    {
        if ((*i)->getDisplayString() == displayName)
        {
            item = (*i);
            itemList.erase(i);
        }
    }

    if (item == NULL)
        return(false);

    delete item;
    return(true);
}

DeviceItem *
Platform::findDeviceInList(const QString &displayName)
{
    DeviceItem *retItem = NULL;
    QLinkedList<DeviceItem *>::iterator i;
    for (i = itemList.begin(); i != itemList.end(); ++i)
    {
        if ((*i)->getDisplayString() == displayName)
            retItem = (*i);
    }

    return(retItem);
}

// TODO make this routine not be shit
void
Platform::writeData(QString path, QString fileName, qint64 deviceSize)
{
    QFileInfo info(fileName);
    qint64 realSize = info.size();

    if (realSize > deviceSize)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("The image you are trying to write is larger than your USB stick."));
        msgBox.exec();
        return;
    }

    qint64 i = 0;
    char *buffer = (char *) malloc(BLOCKSIZE);
    qint64 read = 0;
    qint64 written = 0;

    int ofd = -1;
    int ifd = -1;

    int percentWritten, megsWritten, megsTotal;
    megsTotal = realSize / 1048576;

    // Open the file to read from
    if ((ifd = ::open(fileName.toLocal8Bit().data(), O_RDONLY|O_LARGEFILE)) == -1)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Couldn't open ") + fileName + ": " + strerror(errno));
        msgBox.exec();
        return;
    }

    if ((ofd = ::open(path.toLocal8Bit().data(), O_WRONLY|O_SYNC|O_LARGEFILE)) == -1)
    {
        QMessageBox msgBox;
        msgBox.setText(QObject::tr("Couldn't open ") + path + ": " + strerror(errno));
        msgBox.exec();
        ::close(ifd);
        return;
    }

    QProgressDialog progress(" ", "Cancel", 0, 100);
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setValue(100);
    progress.setWindowTitle(QObject::tr("Writing"));

    for (i = 0; i <= realSize; i++)
    {
        if ((read = ::read(ifd, buffer, BLOCKSIZE)) == -1)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Read failure"));
            msgBox.exec();
            break;
        }

        written = ::write(ofd, buffer, read);
        if (written == -1)
        {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Write failure"));
            msgBox.exec();
            break;
        }

        i += written;
        percentWritten = (i*100)/realSize;
        megsWritten = i / 1048576;
        progress.setValue(percentWritten);
        progress.setLabelText(QObject::tr("Written %1MB out of %2MB").arg(megsWritten).arg(megsTotal));
        qApp->processEvents();

        if (progress.wasCanceled())
             break;
     }
    ::close(ofd);
    ::close(ifd);
    free(buffer);
    progress.setValue(100);
}

