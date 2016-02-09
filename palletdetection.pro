#-------------------------------------------------
#
# Project created by QtCreator 2015-04-06T12:49:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = LMS111
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS += RCPRG_laser_drivers-hydro-devel/liblms1xx/libLMS1xx/LMS1xx.h

LIBS += -L/usr/local/lib \
        -L/usr/lib \
        -L/home/kunal/work/wmsAGV/boost_1_55_0/stage/lib/ \
        -lmlpack \
        -lboost_thread \
        -lboost_system \
        -lboost_filesystem

SOURCES += main.cpp \
          RCPRG_laser_drivers-hydro-devel/liblms1xx/libLMS1xx/LMS1xx.cpp

INCLUDEPATH += RCPRG_laser_drivers-hydro-devel/liblms1xx/libLMS1xx \
               /usr/local/include

#QMAKE_CXXFLAGS += -Wc++0x-compact
