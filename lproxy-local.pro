TEMPLATE = app
TARGET = lproxy
#VERSION = 0.2.1

QT += qml quick core websockets webchannel
#webkitwidgets webchannel
CONFIG += c++11 console

!osx:qtHaveModule(webengine) {
        QT += webengine
        DEFINES += QT_WEBVIEW_WEBENGINE_BACKEND
}

SOURCES += main.cpp \
    websocketchannel/websocketclientwrapper.cpp \
    websocketchannel/websockettransport.cpp \
    webchannelobject/webchannelobject.cpp \
    lproxy/src/core/boost_build/boost_1_57_0__libs__filesystem__src__path.cpp \
    lproxy/src/core/boost_build/dateprebuild.cpp \
    lproxy/src/core/boost_build/fsprebuild.cpp \
    lproxy/src/core/boost_build/sysprebuild.cpp \
    lproxy/src/core/boost_build/tprebuild.cpp \
    lproxy/src/core/log/log_types.cpp \
    lproxy/src/core/log/logging.cpp \
    lproxy/src/core/log/loglevel.cpp \
    lproxy/src/core/log/logoutput_if.cpp \
    lproxy/src/core/crypto/aes_crypto.cpp \
    lproxy/src/core/crypto/encryptor.cpp \
    lproxy/src/core/crypto/md5_crypto.cpp \
    lproxy/src/core/crypto/rsa_crypto.cpp \
    local/log.cpp \
    lproxy/src/core/lss/config_local.cpp \
    lproxy/src/core/lss/session_local.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

DISTFILES += \
    lproxy/src/core/log/Makefile \
    lproxy/src/core/log/Makefile.win32

HEADERS += \
    websocketchannel/websocketclientwrapper.h \
    websocketchannel/websockettransport.h \
    webchannelobject/webchannelobject.h \
    local/log.h

INCLUDEPATH += \
        $$PWD/lproxy/contrib/boost/boost_1_57_0 \
        $$PWD/lproxy/contrib/cryptopp \
        $$PWD/lproxy/src/core/ \
        $$PWD/lproxy/src/core/boost_build

DEFINES += \
        LOG_LOCKFREE_QUEUE _REENTRANT BOOST_REGEX_NO_LIB \
        BOOST_DATE_TIME_SOURCE BOOST_SYSTEM_NO_LIB

android {
        equals(ANDROID_TARGET_ARCH, x86) {
            LIBS += -L"$$_PRO_FILE_PWD_/libs/Androidx86" -lcryptopp -lpthread -lrt
        } else: equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
            LIBS += -L"$$_PRO_FILE_PWD_/libs/Androidarmeabiv7a" -lcryptopp -lpthread -lrt
        } else {
            LIBS += -L"$$_PRO_FILE_PWD_/libs/Android" -lcryptopp -lpthread -lrt
        }
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
}
win32 {
        QMAKE_CXXFLAGS += -include winsock2.h
        LIBS += -L"$$_PRO_FILE_PWD_/libs/MinGW32bit" -lcryptopp -lws2_32 -lmswsock
        RC_ICONS = $$PWD/icon/lp64x64.ico
}

