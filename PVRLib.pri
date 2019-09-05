QT       -= core gui

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

msvc {
    QMAKE_CXXFLAGS_WARN_OFF -= -W0
    QMAKE_CXXFLAGS += -W3
    DEFINES += _CRT_SECURE_NO_WARNINGS _MBCS
} else {
    clang|gcc {
        QMAKE_CXXFLAGS_WARN_OFF -= -w
        QMAKE_CXXFLAGS += -Wall -fpermissive
        QMAKE_CXXFLAGS += \
            -Wno-unknown-pragmas \
            -Wno-unused-function \
            -Wno-deprecated-declarations \
            -Wno-unused-local-typedefs \
            -Wno-switch \
    
        clang:QMAKE_CXXFLAGS += \
            -Wno-sometimes-uninitialized
        else:gcc:QMAKE_CXXFLAGS += \
            -Wno-maybe-uninitialized
    
        win32 {
            DEFINES += _WIN32_WINNT=0x600
            !equals(QT_ARCH, x86_64):QMAKE_CXXFLAGS += -msse2
        }
    }
}

win32 {
    DEFINES -= UNICODE
    DEFINES -= _UNICODE
}

INCLUDEPATH += \
    ../thirdparty/PowerVR_Native_SDK/Framework \
    ../thirdparty/PowerVR_Native_SDK/Builds/Include
