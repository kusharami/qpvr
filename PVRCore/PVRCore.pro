#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T12:04:44
#
#-------------------------------------------------

QT       -= core gui

TARGET = PVRCore
TEMPLATE = lib
CONFIG += staticlib

CONFIG(debug, debug|release) {
    CONFIG_DIR = Debug
} else {
    CONFIG_DIR = Release
}

DESTDIR = $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR

CONFIG += c++11 warn_off

macx {
    DEFINES += "TARGET_OS_MAC=1"
}

unix|win32-g++ {
    QMAKE_CXXFLAGS_WARN_OFF -= -w
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += \
        -Wno-unknown-pragmas
} else {
    win32 {
        QMAKE_CXXFLAGS_WARN_OFF -= -W0
        QMAKE_CXXFLAGS += -W3
        DEFINES += _CRT_SECURE_NO_WARNINGS _MBCS
        DEFINES -= UNICODE
        DEFINES -= _UNICODE
    }
}

HEADERS += \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugiconfig.hpp \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugixml.hpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/ContiguousMap.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/DynamicArray.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/FreeValue.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/IndexedArray.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/ListOfInterfaces.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/MultiObject.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/RingBuffer.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures/SortedArray.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Assert_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/ComplexTypes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Defines.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Fixed.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/HalfFloat.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Hash_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/NativeLibrary.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/NormalisedInteger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/RefCounted.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/ReinterpretBits.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/SizedPointer.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/StackTrace.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Time_.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Types.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/ForwardDecApiObjects.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/IAssetProvider.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/IGraphicsContext.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/IInputHandler.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/IPlatformContext.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces/OSManager.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/Asset.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/AssetReader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/AssetWriter.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/BufferStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FilePath.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FileStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FileWrapStream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Logging/ConsoleMessenger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Logging/Messenger.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Math/AxisAlignedBox.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Math/BoundingSphere.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Math/Plane.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Math/Rectangle.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/CoreIncludes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Strings/StringHash.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Strings/UnicodeConverter.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesBMP.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesDDS.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesKTX.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesPVR.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesTGA.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/FileDefinesXNB.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/MetaData.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/PVRTDecompress.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/TextureDefines.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/TextureHeader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/CoreIncludes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/DataStructures.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Interfaces.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Log.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Maths.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/PixelFormat.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/PVRCore.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Stream.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/StringFunctions.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Threading.h

SOURCES += \
    ../thirdparty/PowerVR_Native_SDK/External/pugixml/pugixml.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/PixelFormat.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/Time.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/BufferStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FilePath.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FileStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/IO/FileWrapStream.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Logging/ConsoleMessenger.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Logging/Log.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Strings/StringFunctions.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Strings/UnicodeConverter.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/MetaData.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/PVRTDecompress.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/Texture.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Texture/TextureHeader.cpp

macx {
    SOURCES += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/NativeLibrary.mm
}

win32 {
    HEADERS += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/WindowsResourceStream.h

    SOURCES += \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Base/NativeLibrary.cpp \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/StackTraceWindows.cpp \
        ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore/Windows/WindowsResourceStream.cpp
}

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRCore \
    ../thirdparty/PowerVR_Native_SDK/Builds/Include
