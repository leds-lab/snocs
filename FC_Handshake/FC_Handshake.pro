#-------------------------------------------------
#
# Project created by QtCreator 2016-08-25T11:03:32
#
#-------------------------------------------------

CONFIG -= qt
CONFIG -= app_bundle

include(../FlowControl/FlowControl.pro)

TEMPLATE = lib

TARGET = fchandshake

SOURCES += FC_Handshake.cpp

HEADERS += FC_Handshake.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
