TARGET = PVRAssets

include(../PVRLib.pri)

INCLUDEPATH += ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets

HEADERS += \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/AssetIncludes.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/BoundingBox.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Effect.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Effect_2.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Geometry.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Helper.h \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model.h \
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
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/ShadowVolume.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Animation.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Camera.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Light.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Mesh.cpp \
    ../thirdparty/PowerVR_Native_SDK/Framework/PVRAssets/Model/Model.cpp \
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
