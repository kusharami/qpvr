VERSION = 1.0.0

TARGET = qpvr

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11 warn_off

unix|win32-g++ {
    QMAKE_CXXFLAGS_WARN_OFF -= -w
    QMAKE_CXXFLAGS += -Wall
} else {
    win32 {
        QMAKE_CXXFLAGS_WARN_OFF -= -W0
        QMAKE_CXXFLAGS += -W3
    }
}

target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

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

CONFIG(debug, debug|release) {
    CONFIG_NAME = debug
    win32 {
        TARGET_EXT = d.dll
    }

    macx {
        TARGET_EXT = d.dylib
    }

} else {
    CONFIG_NAME = release
    win32 {
        TARGET_EXT = .dll
    }

    macx {
        TARGET_EXT = .dylib
    }
}

LIBS += -L$$OUT_PWD/../thirdparty/qzstream/lib/$$CONFIG_NAME
LIBS += -L$$OUT_PWD/../PVRAssets/$$CONFIG_NAME
LIBS += -L$$OUT_PWD/../PVRCore/$$CONFIG_NAME
LIBS += -lPVRCore -lPVRAssets -lPVRTexLib -lQZStream

OTHER_FILES += pvr.json

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PVRTexLib/Include \
    ../thirdparty/qzstream/lib

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QPVRPlugin