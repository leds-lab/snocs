# if depends on socin definitions - add link library options
LIBS += -L$$PWD/plugins/ -lparameters
INCLUDEPATH += $$PWD/Parameters
DEPENDPATH += $$PWD/Parameters

# qmake version 5.7.0
#QMAKE_RPATHDIR += $ORIGIN
# Previous qmake version compatibily
unix {
    QMAKE_LFLAGS += -Wl,-rpath=\$$ORIGIN
}
