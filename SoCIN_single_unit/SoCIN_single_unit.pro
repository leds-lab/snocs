#-------------------------------------------------
#
# Project created by QtCreator 2016-10-07T14:34:16
#
#-------------------------------------------------


include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_unit

SOURCES += \
    ../PluginManager/PluginManager.cpp \
    SoCIN_Unit.cpp

HEADERS += \
    ../PluginManager/PluginManager.h \
    SoCIN_Unit.h
