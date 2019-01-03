
#include "chatsystem_simple_mainwindow.hpp"
#include "chatsystem_simple_qtapplication.hpp"

chatsystem::simple::MainWindow::MainWindow()
{
    setCentralWidget(&m_centralWidget);
    ServerPortChangedSlot(THIS_APP()->getServerPort());
    connect(THIS_APP(),SIGNAL(ServerPortChangedSignal( int )),this,SLOT(ServerPortChangedSlot(int)));
}


chatsystem::simple::MainWindow::~MainWindow()
{
    //
}


void chatsystem::simple::MainWindow::ServerPortChangedSlot( int a_nPort )
{
    char vcBuffer[512];
    snprintf(vcBuffer,511,"simple_chat; serverPort:%d",a_nPort);
    setWindowTitle(vcBuffer);
}
