# if depends on socin definitions - add link library options
LIBS += -L$$PWD/plugins/ -lparameters
INCLUDEPATH += $$PWD/Parameters
DEPENDPATH += $$PWD/Parameters

greaterThan(QT_VERSION, 5.7) {
    unix {
        message("Using RPATH")
        QMAKE_RPATHDIR += $ORIGIN
    }
} else {
    unix:!mac {
        message("Using LFLAGS")
        QMAKE_LFLAGS += -Wl,-rpath=\$$ORIGIN
    }
}
