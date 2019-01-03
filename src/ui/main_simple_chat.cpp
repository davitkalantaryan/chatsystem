//#include "main_chatsystem.h"
// 2017 Jul 12
// main_chatsystem.cpp

#include <QApplication>
#include "chatsystem_simple_qtapplication.hpp"
#include "chatsystem_simple_mainwindow.hpp"
#include <stdio.h>
#include <QUrl>
#include <signal.h>
#include <string>
#ifdef ANDROID
#include <QtWebView/QtWebView>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


extern char* g_pcArgv0;
char* g_pcArgv0;


int main(int argc, char* argv[])
{
    //freopen( "/dev/null", "w", stderr);

    g_pcArgv0 = argv[0];

    common::socketN::Initialize();

    chatsystem::simple::QtApplication myApp(argc, argv);
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QUrl>("QUrl");
    qRegisterMetaType< ::std::string >("::std::string");
    //qRegisterMetaType<omxclient::SDevReturn>("omxclient::SDevReturn");

    chatsystem::simple::MainWindow aMain;

    aMain.show();
    myApp.execGui();
    common::socketN::Cleanup();

    return 0;
}
