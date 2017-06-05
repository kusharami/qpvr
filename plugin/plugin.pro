VERSION = 1.0.2

TARGET = qpvr

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11 warn_off

unix|win32-g++ {
    QMAKE_CXXFLAGS_WARN_OFF -= -w
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += \
        -Wno-unknown-pragmas
} else {
    win32 {
        QMAKE_CXXFLAGS_WARN_OFF -= -W0
        QMAKE_CXXFLAGS += -W3
        QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT
    }
}

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

HEADERS += \
    QPVRPlugin.h \
    QPVRHandler.h \
    QIODevicePVRAdapter.h

SOURCES += \
    QPVRPlugin.cpp \
    QPVRHandler.cpp \
    QIODevicePVRAdapter.cpp

include(../thirdparty/qzstream/QZStream.pri)

macx {
    LIBS += -L$$_PRO_FILE_PWD_/../thirdparty/PVRTexLib/OSX_x86/Static
}

win32 {
    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L$$_PRO_FILE_PWD_/../thirdparty/PVRTexLib/Windows_x86_64/Static
    } else {
        LIBS += -L$$_PRO_FILE_PWD_/../thirdparty/PVRTexLib/Windows_x86_32/Static
    }
}

win32 {
    CONFIG(debug, debug|release) {
        TARGET_EXT = d.dll
    } else {
        TARGET_EXT = .dll
    }
}

CONFIG(debug, debug|release) {
    CONFIG_DIR = Debug
} else {
    CONFIG_DIR = Release
}

LIBS += -L$$_PRO_FILE_PWD_/../thirdparty/qzstream/build/$$CONFIG_DIR
LIBS += -L$$_PRO_FILE_PWD_/../build/$$CONFIG_DIR
LIBS += -L$$_PRO_FILE_PWD_/../build/$$CONFIG_DIR
LIBS += -lPVRCore -lPVRAssets -lPVRTexLib -lQZStream

OTHER_FILES += pvr.json

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PVRTexLib/Include \
    ../thirdparty/qzstream/lib

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QPVRPlugin
