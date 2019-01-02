//#include "main_chatsystem.h"
// 2017 Jul 12
// main_chatsystem.cpp

#include <QApplication>
#include "common_ui_qt_mainwindow.hpp"
#include "chatsystem_simple_qtapplication.hpp"
#include "chatsystem_simple_centralwidget.hpp"
#include <stdio.h>
#include <QUrl>
#include <signal.h>
#ifdef ANDROID
#include <QtWebView/QtWebView>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


int main(int argc, char* argv[])
{
    //freopen( "/dev/null", "w", stderr);

    chatsystem::simple::QtApplication myApp(argc, argv);
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QUrl>("QUrl");
    //qRegisterMetaType<omxclient::SDevReturn>("omxclient::SDevReturn");

    common::ui::qt::MainWindow aMain;
    chatsystem::simple::CentralWidget aWidget;

    //printf("!!!!!!!!!!!!!!!!!!!!! \n");

    aMain.setCentralWidget(&aWidget);
    aMain.show();


    myApp.exec();
    return 0;
}
