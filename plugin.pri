TEMPLATE = lib
CONFIG -= qt
CONFIG -= app_bundle

TARGET_EXT = .dll

DESTDIR = ../plugins
QMAKE_LFLAGS += -fPIC
DEFINES += PLUGIN

include(resources.pri)

unix {
    target.path = /usr/lib/socin
    INSTALLS += target
}

