#-------------------------------------------------
#
# Project created by QtCreator 2016-08-26T12:02:25
#
#-------------------------------------------------

CONFIG -= qt
CONFIG -= app_bundle

include(../Memory/Memory.pro)

TEMPLATE = lib

TARGET = memfifo

SOURCES += Fifo.cpp

HEADERS += Fifo.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
