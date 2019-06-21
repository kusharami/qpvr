VERSION = 1.2.0

TARGET = qpvr

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++11 warn_off

msvc {
    QMAKE_CXXFLAGS_WARN_OFF -= -W0
    QMAKE_CXXFLAGS += -W3
    QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT
} else {
    clang|gcc {
        QMAKE_CXXFLAGS_WARN_OFF -= -w
        QMAKE_CXXFLAGS += -Wall
        QMAKE_CXXFLAGS += \
            -Wno-deprecated-declarations \
            -Wno-unknown-pragmas
    }
}

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

HEADERS += \
    PVRTextureWrapper.h \
    QPVRPlugin.h \
    QPVRHandler.h \
    QIODevicePVRAdapter.h

SOURCES += \
    PVRTextureWrapper.cpp \
    QPVRPlugin.cpp \
    QPVRHandler.cpp \
    QIODevicePVRAdapter.cpp

THIRDPARTY_PATH = $$_PRO_FILE_PWD_/../thirdparty

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

BUILD_LIBS_DIR = $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR

msvc {
    PRE_TARGETDEPS += \
        $$PVRTEXLIB_PATH/PVRTexLib.lib \
        $$BUILD_LIBS_DIR/PVRCore.lib \
        $$BUILD_LIBS_DIR/PVRAssets.lib
} else {
    equals(PVRTEXLIB_STATIC, 1) {
        linux|macx:PRE_TARGETDEPS += $$PVRTEXLIB_PATH/libPVRTexLib.a
    }
    
    clang|gcc:PRE_TARGETDEPS += \
        $$BUILD_LIBS_DIR/libPVRCore.a \
        $$BUILD_LIBS_DIR/libPVRAssets.a
}

!win32-g++ {
    LIBS += -L$$PVRTEXLIB_PATH
    LIBS += -lPVRTexLib
    INCLUDEPATH += ../thirdparty/PVRTexLib/Include
}

LIBS += -L$$_PRO_FILE_PWD_/../build/$$CONFIG_DIR
LIBS += -lPVRCore -lPVRAssets 

OTHER_FILES += pvr.json

INCLUDEPATH += ../thirdparty/PowerVR_Native_SDK/Framework

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = QPVRPlugin

DISTFILES += \
    CHANGELOG
