TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    sql_interface.cpp \
    mmysql.cpp \
    mmytable.cpp \
    mmyrecord.cpp \
    mmyset.cpp \
    mmytable_functions.cpp \
    mmyshunting.cpp \
    parse_tree.cpp \
    sethelper.cpp \


HEADERS += \
    sql_interface.h \
    mmysql.h \
    mmytable.h \
    mmyrecord.h \
    mmyset.h \
    mmytable_functions.h \
    mmyshunting.h \
    parse_tree.h \
    sethelper.h \


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../state_machine/release/ -lstate_machine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../state_machine/debug/ -lstate_machine
else:unix: LIBS += -L$$PWD/../state_machine/ -lstate_machine

INCLUDEPATH += $$PWD/../state_machine/debug
DEPENDPATH += $$PWD/../state_machine/debug

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../state_machine/release/libstate_machine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../state_machine/debug/libstate_machine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../state_machine/release/state_machine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../state_machine/debug/state_machine.lib
else:unix: PRE_TARGETDEPS += $$PWD/../state_machine/libstate_machine.a

DISTFILES += \
    ../test.bin \
    test.bin \
    resultsfile.bin
