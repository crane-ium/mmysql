TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    sql_interface.cpp \
    mmysql.cpp \
    mmytable.cpp \
    mmyrecord.cpp

HEADERS += \
    sql_interface.h \
    mmysql.h \
    mmytable.h \
    mmyrecord.h
