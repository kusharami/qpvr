VERSION = 1.0.7

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

THIRDPARTY_PATH = $$_PRO_FILE_PWD_/../thirdparty

include($$THIRDPARTY_PATH/qzstream/QZStream.pri)

include(../pvrtexlib.pri)

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

win32:PRE_TARGETDEPS += \
    $$THIRDPARTY_PATH/qzstream/build/$$CONFIG_DIR/QZStream.lib \
    $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR/PVRCore.lib \
    $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR/PVRAssets.lib

macx:PRE_TARGETDEPS += \
    $$THIRDPARTY_PATH/qzstream/build/$$CONFIG_DIR/libQZStream.a \
    $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR/libPVRCore.a \
    $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR/libPVRAssets.a

LIBS += -L$$PVRTEXLIB_PATH
LIBS += -L$$THIRDPARTY_PATH/qzstream/build/$$CONFIG_DIR
LIBS += -L$$_PRO_FILE_PWD_/../build/$$CONFIG_DIR
LIBS += -lPVRCore -lPVRAssets -lPVRTexLib -lQZStream

OTHER_FILES += pvr.json

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PVRTexLib/Include \
    ../thirdparty/qzstream/lib

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QPVRPlugin

DISTFILES += \
    CHANGELOG
