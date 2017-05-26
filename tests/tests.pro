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

include(../thirdparty/qzstream/QZStream.pri)

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    Tests.cpp

HEADERS += \
    Tests.h
