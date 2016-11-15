#-------------------------------------------------
#
# Project created by QtCreator 2016-11-07T15:10:01
#
#-------------------------------------------------

include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_SoCIN_Spider

SOURCES += SoCIN_Spider.cpp \
    ../PluginManager/PluginManager.cpp

HEADERS += SoCIN_Spider.h \
    ../PluginManager/PluginManager.h

