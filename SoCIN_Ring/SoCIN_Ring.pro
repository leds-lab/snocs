#-------------------------------------------------
#
# Project created by QtCreator 2016-10-13T14:15:09
#
#-------------------------------------------------

include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_SoCIN_Ring

SOURCES += SoCIN_Ring.cpp \
    ../PluginManager/PluginManager.cpp \
    ../TrafficMeter/TrafficMeter.cpp

HEADERS += SoCIN_Ring.h \
    ../PluginManager/PluginManager.h \
    ../TrafficMeter/TrafficMeter.h
