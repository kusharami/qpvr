#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T12:04:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = PVRCore
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11 warn_off

unix|win32-g++ {
    QMAKE_CXXFLAGS_WARN_OFF -= -w
    QMAKE_CXXFLAGS += -Wall
}

macx {
    DEFINES += "TARGET_OS_MAC=1"
}

win32 {
    QMAKE_CXXFLAGS_WARN_OFF -= -W0
    QMAKE_CXXFLAGS += -W3
    DEFINES += _CRT_SECURE_NO_WARNINGS _MBCS
    DEFINES -= UNICODE
    DEFINES -= _UNICODE
}

HEADERS += \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Assert_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/AxisAlignedBox.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/BufferStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ComplexTypes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ConsoleMessenger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ContiguousMap.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/CoreIncludes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Defines.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FilePath.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FileStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FileWrapStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Fixed.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ForwardDecApiObjects.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FreeValue.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/HalfFloat.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Hash_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IAssetProvider.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IGraphicsContext.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IInputHandler.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IndexedArray.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IPlatformContext.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ListOfInterfaces.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Log.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Maths.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Messenger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/MultiObject.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/NativeLibrary.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/NormalisedInteger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/OSManager.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Plane.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/PVRCore.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Rectangle.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/RefCounted.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ReinterpretBits.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/RingBuffer.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/SizedPointer.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/SortedArray.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/StackTrace.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Stream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/StringFunctions.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/StringHash.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Threading.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Time_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Types.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/UnicodeConverter.h \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugiconfig.hpp \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugixml.hpp

SOURCES += \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/BufferStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/ConsoleMessenger.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FilePath.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FileStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/FileWrapStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Log.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/NativeLibrary.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/StringFunctions.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Time.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/UnicodeConverter.cpp \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugixml.cpp

macx {
    SOURCES += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/NativeLibrary.mm
}

win32 {
    HEADERS += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/WindowsResourceStream.h

    SOURCES += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/StackTraceWindows.cpp \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/WindowsResourceStream.cpp
}

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore \
    ../thirdparty/PowerVR_Native_SDK/Builds/Include
