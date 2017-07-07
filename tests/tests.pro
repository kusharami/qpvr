#-------------------------------------------------
#
# Project created by QtCreator 2017-05-25T16:01:02
#
#-------------------------------------------------

QT       += testlib

QT       += gui

TARGET = QPVRTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

THIRDPARTY_PATH = $$_PRO_FILE_PWD_/../thirdparty

include($$THIRDPARTY_PATH/qzstream/QZStream.pri)
include(../deps.pri)

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    Tests.cpp

HEADERS += \
    Tests.h
