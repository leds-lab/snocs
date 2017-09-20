#-------------------------------------------------
#
# Project created by QtCreator 2016-10-11T15:47:28
#
#-------------------------------------------------

include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_SoCIN_Torus

SOURCES += SoCIN_Torus.cpp \
    ../PluginManager/PluginManager.cpp \
    ../TrafficMeter/TrafficMeter.cpp

HEADERS += SoCIN_Torus.h \
    ../PluginManager/PluginManager.h \
    ../TrafficMeter/TrafficMeter.h
