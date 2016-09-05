TEMPLATE = lib
DESTDIR = ../plugins
QMAKE_LFLAGS += -fPIC
DEFINES += _EXP_SYMBOLS

unix {
    target.path = /usr/lib/socin
    INSTALLS += target
}
