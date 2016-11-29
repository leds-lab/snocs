#-------------------------------------------------
#
# Project created by QtCreator 2016-11-28T19:09:33
#
#-------------------------------------------------

include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_SoCIN_3D

SOURCES += SoCIN_3D.cpp \
    ../PluginManager/PluginManager.cpp


HEADERS += SoCIN_3D.h \
    ../PluginManager/PluginManager.h
