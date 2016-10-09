#-------------------------------------------------
#
# Project created by QtCreator 2016-10-07T14:34:16
#
#-------------------------------------------------


include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_crossbar

SOURCES += SoCIN_Crossbar.cpp \
    ../PluginManager/PluginManager.cpp

HEADERS += SoCIN_Crossbar.h \
    ../PluginManager/PluginManager.h
