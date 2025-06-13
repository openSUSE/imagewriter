# print deprecation warning
message("qmake support is deprecated and will be removed in future release. Use CMake instead.")

unix:isEmpty(PREFIX):PREFIX = /usr/local
TEMPLATE = app
TARGET =  imagewriter
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR += .
VERSION=1.10
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
QMAKE_CXXFLAGS_RELEASE += "-fvisibility=hidden -fvisibility-inlines-hidden"
QT  += dbus gui widgets

# Input
HEADERS += DeviceItem.h \
    MainWindow.h \
    Platform.h \
    PlatformHal.h \
    PlatformUdisks.h \
    PlatformUdisks2.h \
    udisks2_interface.h \
    udisks2_mountpoints_interface.h \
    MetaTypes.h
SOURCES += main.cpp \
    MainWindow.cpp \
    PlatformHal.cpp \
    PlatformUdisks.cpp \
    Platform.cpp \
    PlatformUdisks2.cpp \
    udisks2_interface.cpp \
    udisks2_mountpoints_interface.cpp

CONFIG += c++11 link_pkgconfig


exists("/usr/include/hal/libhal.h") { 
    PKGCONFIG += hal \
        hal-storage
    DEFINES += USEHAL
    DEFINES -= USEUDISKS2
    HEADERS -= PlatformUdisks2.h udisks2_interface.h udisks2_mountpoints_interface.h MetaTypes.h
    SOURCES -= PlatformUdisks2.cpp udisks2_interface.cpp udisks2_mountpoints_interface.cpp
}

QMAKE_EXTRA_TARGETS += distfile
DISTFILE_MAKEDIR = .tmp/imagewriter-$$VERSION
DISTFILE_EXTRAFILES = $$RESOURCES \
    COPYING \
    imagewriter.1 \
    imagewriter.pro \
    README.md \
    INSTALL \
    *.png \
    imagewriter.qrc \
    imagewriter.desktop \
    imagewriter.spec \
    icons/ 

distfile.commands = mkdir \
    -p \
    $$DISTFILE_MAKEDIR \
    && \
    cp \
    -r \
    -f \
    --parent \
    $$SOURCES \
    $$HEADERS \
    $$FORMS \
    $$DISTFILE_EXTRAFILES \
    $$DISTFILE_MAKEDIR \
    && \
    cd \
    .tmp \
    && \
    tar \
    cvzf \
    imagewriter-$$VERSION\.tar.gz \
    imagewriter-$$VERSION \
    && \
    mv \
    imagewriter-$$VERSION\.tar.gz \
    .. \
    && \
    cd \
    ..

INSTALLS += target \
    icon32 \
    icon64 \
    icon128 \
    desktop \
    manpage
target.path = $$PREFIX/bin
desktop.path = $$PREFIX/share/applications/
desktop.files += imagewriter.desktop
icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps
icon32.files += icons/32x32/imagewriter.png
icon64.path = $$PREFIX/share/icons/hicolor/64x64/apps
icon64.files += icons/64x64/imagewriter.png
icon128.path = $$PREFIX/share/icons/hicolor/128x128/apps
icon128.files += icons/128x128/imagewriter.png
manpage.path =$$PREFIX/share/man/man1
manpage.files += imagewriter.1
RESOURCES += imagewriter.qrc
