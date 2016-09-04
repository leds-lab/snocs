TEMPLATE = lib
DESTDIR = ../plugins
QMAKE_LFLAGS += -fPIC
DEFINES += _SHARED

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
