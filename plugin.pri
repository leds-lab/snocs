TEMPLATE = lib
CONFIG -= qt
CONFIG -= app_bundle

DESTDIR = ../plugins
QMAKE_LFLAGS += -fPIC
DEFINES += PLUGIN

unix {
    target.path = /usr/lib/socin
    INSTALLS += target
}
