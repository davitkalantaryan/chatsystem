# File remote_plot_spectrum.pro
# File created : 04 Jul 2017
# Created by : Davit Kalantaryan (davit.kalantaryan@desy.de)
# This file can be used to produce Makefile for daqadcreceiver application
# for PITZ
# CONFIG += TEST
# For making test: '$qmake "CONFIG+=TEST" daqadcreceiver.pro' , then '$make'
#include(../../common/common_qt/sys_common.pri)
include(../../common/common_qt/gui_with_mainwnd.pri)

#CONFIG += release

greaterThan(QT_MAJOR_VERSION, 4){
DEFINES += MARGINES_OK
}else{
greaterThan(QT_MAJOR_VERSION, 3){
greaterThan(QT_MINOR_VERSION,7): DEFINES += MARGINES_OK
}
}

#QT += webkitwidgets
#DEFINES += webkitwidgets_defined

#DEFINES += DEBUG_APP_NON_RPI
#DEFINES += LOCAL_HOST_NAME=""192.168.0.100""
#QT += network

INCLUDEPATH += ../../../include
INCLUDEPATH += ../../../contrib/cpp-raft/include
#INCLUDEPATH += ../../../src/client
SOURCES += \
    ../../../src/ui/main_simple_chat.cpp \
    ../../../src/ui/chatsystem_simple_qtapplication.cpp \
    ../../../contrib/cpp-raft/common/common_iodevice.cpp \
    ../../../contrib/cpp-raft/common/common_servertcp.cpp \
    ../../../contrib/cpp-raft/common/common_socketbase.cpp \
    ../../../contrib/cpp-raft/common/common_sockettcp.cpp \
    ../../../src/ui/chatsystem_simple_centralwidget.cpp \
    ../../../src/ui/chatsystem_simple_mainwindow.cpp

HEADERS += \
    ../../../src/ui/chatsystem_simple_qtapplication.hpp \
    ../../../src/ui/chatsystem_simple_centralwidget.hpp \
    ../../../src/ui/chatsystem_simple_mainwindow.hpp \
    ../../../contrib/cpp-raft/include/common/common_servertcp.hpp
