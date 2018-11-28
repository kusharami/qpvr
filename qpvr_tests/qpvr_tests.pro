#-------------------------------------------------
#
# Project created by QtCreator 2017-05-25T16:01:02
#
#-------------------------------------------------

QT       += testlib

QT       += gui

TARGET = QPVRTests
CONFIG   += console

TEMPLATE = app

THIRDPARTY_PATH = $$_PRO_FILE_PWD_/../thirdparty

macx {
    DYNAMIC_LIBS.path = "Contents/lib"
    QMAKE_BUNDLE_DATA += DYNAMIC_LIBS
}

include($$THIRDPARTY_PATH/qzstream/QZStream.pri)
include(../pvrdepend.pri)

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    Tests.cpp

HEADERS += \
    Tests.h
