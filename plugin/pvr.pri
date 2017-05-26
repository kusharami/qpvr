THIRDPARTY = $$_PRO_FILE_PWD_/../thirdparty
PVR_PATH = $$THIRDPARTY/PowerVR_Native_SDK/Framework
PREBUILT = $$THIRDPARTY/prebuilt

macx {
    PLATFORM_LIB_DIR = mac
}

win32 {
    PLATFORM_LIB_DIR = win32
}

CONFIG(debug, debug|release) {
    PREBUILT = $$PREBUILT/$$PLATFORM_LIB_DIR/debug
    DEFINES += "DEBUG=1"
} else {
    PREBUILT = $$PREBUILT/$$PLATFORM_LIB_DIR/release
}

macx {
    DEFINES += "TARGET_OS_MAC=1"
    LIBS += -lz
}

LIBS += -L$$PREBUILT
LIBS += -lPVRCore -lPVRAssets

INCLUDEPATH += $$PVR_PATH
