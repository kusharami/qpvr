#-------------------------------------------------
#
# Project created by QtCreator 2017-05-19T12:52:37
#
#-------------------------------------------------

QT       -= core gui

TARGET = PVRAssets
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
        -Wno-unknown-pragmas \
        -Wno-sometimes-uninitialized \
        -Wno-unused-function \
        -Wno-unused-local-typedef \
        -Wno-switch

} else {
    win32 {
        QMAKE_CXXFLAGS_WARN_OFF -= -W0
        QMAKE_CXXFLAGS += -W3
        DEFINES += _CRT_SECURE_NO_WARNINGS _MBCS
        DEFINES -= UNICODE
        DEFINES -= _UNICODE
    }
}

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets \
    ../thirdparty/PowerVR_Native_SDK/Builds/Include

HEADERS += \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Asset.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/AssetIncludes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/AssetReader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/AssetWriter.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/BoundingBox.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Effect.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Effect_2.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Geometry.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Helper.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/PixelFormat.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/PVRAssets.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/PVRShamanSemantics.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Shader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/ShaderDescription.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/ShadowVolume.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/SkipGraph.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Animation.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Camera.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Light.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Mesh.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/MetaData.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/PVRTDecompress.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/Texture.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureDefines.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureFormats.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureHeader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureHeaderWithMetaData.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesBMP.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesDDS.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesKTX.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesPVR.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesTGA.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/FileDefinesXNB.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PaletteExpander.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PFXParser.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PFXReader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PODDefines.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PODReader.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderBMP.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderDDS.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderKTX.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderPVR.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderTGA.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderXNB.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterDDS.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterKTX.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterLegacyPVR.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterPVR.h

SOURCES += \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Geometry.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/PixelFormat.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/ShadowVolume.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Animation.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Camera.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Light.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Mesh.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Model.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/MetaData.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/PVRTDecompress.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/Texture.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureHeader.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Texture/TextureHeaderWithMetaData.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PaletteExpander.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PFXParser.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PFXReader.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/PODReader.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderBMP.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderDDS.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderKTX.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderPVR.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderTGA.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureReaderXNB.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterDDS.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterKTX.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterLegacyPVR.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/FileIO/TextureWriterPVR.cpp
