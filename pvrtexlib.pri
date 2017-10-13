isEmpty(PVRTEXLIB_PATH) {
    PVRTEXLIB_PATH = $$THIRDPARTY_PATH/PVRTexLib
}

macx {
    PVRTEXLIB_PATH = $$PVRTEXLIB_PATH/OSX_x86/Dynamic
}

win32 {
    contains(QMAKE_HOST.arch, x86_64) {
        PVRTEXLIB_PATH = $$PVRTEXLIB_PATH/Windows_x86_64/Dynamic
    } else {
        PVRTEXLIB_PATH = $$PVRTEXLIB_PATH/Windows_x86_32/Dynamic
    }

    DEFINES += _WINDLL_IMPORT
}
