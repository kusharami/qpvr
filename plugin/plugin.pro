VERSION = 1.0.0

TARGET = qpvr

TEMPLATE = lib
CONFIG += plugin

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

CONFIG(debug, debug|release) {
    CONFIG_NAME = debug
} else {
    CONFIG_NAME = release
}

LIBS += -L$$OUT_PWD/../thirdparty/qzstream/lib/$$CONFIG_NAME
LIBS += -L$$OUT_PWD/../PVRAssets/$$CONFIG_NAME
LIBS += -L$$OUT_PWD/../PVRCore/$$CONFIG_NAME
LIBS += -lPVRCore -lPVRAssets -lQZStream
OTHER_FILES += pvr.json
INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/qzstream/lib
