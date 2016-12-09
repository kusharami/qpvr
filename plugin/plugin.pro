VERSION = 1.0.0

TARGET = qpvr

TEMPLATE = lib
CONFIG += plugin

include(pvr.pri)

OTHER_FILES += pvr.json

target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

HEADERS += \
    QPVRPlugin.h \
    QPVRHandler.h

SOURCES += \
    QPVRPlugin.cpp \
    QPVRHandler.cpp
