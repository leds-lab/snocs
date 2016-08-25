#-------------------------------------------------
#
# Project created by QtCreator 2016-08-25T15:53:40
#
#-------------------------------------------------

CONFIG -= qt
CONFIG -= app_bundle

include(../FlowControl/FlowControl.pro)

TEMPLATE = lib

TARGET = fccreditbased

SOURCES += FC_CreditBased.cpp

HEADERS += FC_CreditBased.h

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}
