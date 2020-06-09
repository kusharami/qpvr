CONFIG(debug, debug|release) {
    CONFIG_DIR = Debug
} else {
    CONFIG_DIR = Release
}

isEmpty(BIN_DIR) {
    BIN_DIR = $$_PRO_FILE_PWD_/../build/$$CONFIG_DIR
    DESTDIR = $$BIN_DIR
}

include(pvrtexlib.pri)

equals(PVRTEXLIB_CONFIG, "Dynamic") {
    win32 {
        QMAKE_POST_LINK += \
            copy /y \"$$shell_path($$PVRTEXLIB_PATH/PVRTexLib.dll)\" \
            \"$$shell_path($$BIN_DIR)\" $$escape_expand(\n\t)
    }

    macx {
        DYNAMIC_LIBS.files += $$PVRTEXLIB_PATH/libPVRTexLib.dylib
    }
}
