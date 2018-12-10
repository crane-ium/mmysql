TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    mmysql.cpp \
    mmytable.cpp \
    mmyshunting.cpp \
    sethelper.cpp \
    parse_tree_functions.cpp \
    mmystate.cpp \
    mmy_helper_functions.cpp \


HEADERS += \
    mmysql.h \
    mmytable.h \
    mmyrecord.h \
    mmyshunting.h \
    sethelper.h \
    parse_tree_functions.h \
    mmyenums.h \
    mmy_helper_functions.h \
    mmy_constants.h \


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
    resultsfile.bin \
    work_report.txt \
    returnfile.bin \
    sample_inputs.txt
