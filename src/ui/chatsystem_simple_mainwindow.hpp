#ifndef CHATSYSTEM_SIMPLE_MAINWINDOW_HPP
#define CHATSYSTEM_SIMPLE_MAINWINDOW_HPP

#include "common_ui_qt_mainwindow.hpp"
#include "chatsystem_simple_centralwidget.hpp"

namespace chatsystem { namespace simple{

class MainWindow : public common::ui::qt::MainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private slots:
    void ServerPortChangedSlot( int port );

private:
    chatsystem::simple::CentralWidget       m_centralWidget;
};


}} // namespace chatsystem { namespace simple{

#endif // CHATSYSTEM_SIMPLE_MAINWINDOW_HPP
