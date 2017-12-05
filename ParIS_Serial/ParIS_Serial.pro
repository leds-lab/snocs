#-------------------------------------------------
#
# Project created by Eduardo Alves @ 12/05/2017
#
#-------------------------------------------------

include(../common.pri)
include(../plugin.pri)
include(../socindefines.pri)

TARGET = router_ParIS_Serial

SOURCES += ParIS_Serial.cpp \
    ../src/PluginManager.cpp \
    Par2Ser.cpp \
    Ser2Par.cpp \
    Converter.cpp

HEADERS += ParIS_Serial.h \
    ../src/PluginManager.h \
    Par2Ser.h \
    Ser2Par.h \
    Converter.h

