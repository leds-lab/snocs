#-------------------------------------------------
#
# Project created by QtCreator 2016-09-24T16:08:32
#
#-------------------------------------------------

include(../NoC/NoC.pro)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = noc_SoCIN

HEADERS += \
    SoCIN.h \
    ../TrafficMeter/TrafficMeter.h \
    ../PluginManager/PluginManager.h

SOURCES += \
    SoCIN.cpp \
    ../TrafficMeter/TrafficMeter.cpp \
    ../PluginManager/PluginManager.cpp
