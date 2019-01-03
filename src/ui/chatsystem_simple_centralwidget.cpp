
#include "chatsystem_simple_centralwidget.hpp"
#include "chatsystem_simple_qtapplication.hpp"
#include <string.h>
#include <stdlib.h>

#define TEXT_FOR_CONNECT    "Connect"
#define TEXT_FOR_DISCONNECT    "Disconnect"

extern char* g_pcArgv0;

chatsystem::simple::CentralWidget::CentralWidget()
{
#if 0
    size_t unRead;
    char vcBuffer[8192];
    FILE* fpPipe;

    //snprintf(vcBuffer,1023,"ldd %s",g_pcArgv0);
    //fpPipe=_popen(vcBuffer,"rt");
    fpPipe=_popen("echo %Path%","rt");

    if(fpPipe){

        //while(fgets(vcBuffer, 128, pPipe)){
        //    printf(psBuffer);
        //}

        unRead=fread(vcBuffer, 1,8191, fpPipe);
        vcBuffer[unRead]=0;
        m_conversationText.setText(vcBuffer);
        _pclose(fpPipe);
    }

    //popen
#endif

    m_nLastHostUnableToConnect = 0;
    m_btnSendMsg.setText("Send");
    m_btnConnectDisconnect.setText(TEXT_FOR_CONNECT);
    m_conversationText.setReadOnly(true);
    m_messageText.setReadOnly(true);
    m_editForHost.setStyleSheet("background-color:rgb(255,255,255);color:rgb(0,0,0);");

    //m_messageText.resize(m_messageText.width(),2*m_messageText.height());
#if 0
    //m_layoutMain.addWidget(&m_editForHost,0,0,-1,1);//m_btnConnectDisconnect
    m_layoutMain.addWidget(&m_editForHost,0,0,2,0);//m_btnConnectDisconnect
    m_layoutMain.addWidget(&m_btnConnectDisconnect,1,1);//
    m_layoutMain.addWidget(&m_messageText,2,0,2,4);
    m_layoutMain.addWidget(&m_btnSendMsg,1,1);
#else
    m_layoutMain.addWidget(&m_editForHost,0,0);//m_btnConnectDisconnect
    m_layoutMain.addWidget(&m_btnConnectDisconnect,0,1);//m_conversationText
    m_layoutMain.addWidget(&m_conversationText,1,0);//m_conversationText
    m_layoutMain.addWidget(&m_messageText,2,0,1,1);
    m_layoutMain.addWidget(&m_btnSendMsg,2,1);
#endif
    setLayout(&m_layoutMain);

    connect(&m_btnConnectDisconnect,SIGNAL(clicked()),this,SLOT(ConnectDisconnectClickedSlot()));
    connect(&m_btnSendMsg,SIGNAL(clicked()),this,SLOT(SendMessageSlot()));
    connect(&m_editForHost,SIGNAL(textChanged(const QString&)),this,SLOT(HostNameChangedSlot(const QString&)));
    connect(THIS_APP(),SIGNAL(ConnectionTryFinishedSignal(bool)),this,SLOT(ConnectionTryFinishedSlot(bool)));
    connect(THIS_APP(),SIGNAL(IncomingConnectionSignal( ::std::string )),this,SLOT(IncomingConnectionSlot(::std::string)));
    connect(THIS_APP(),SIGNAL(MessageReceivedSignal( ::std::string )),this,SLOT(MessageReceivedSlot(::std::string)));
    connect(THIS_APP(),SIGNAL(DisconnectedSignal(void)),this,SLOT(DisconnectedSlot(void)));
    connect(THIS_APP(),SIGNAL(WriteDoneSignal(void)),this,SLOT(WriteDoneSlot(void)));
}



chatsystem::simple::CentralWidget::~CentralWidget()
{
    m_layoutMain.removeWidget(&m_btnSendMsg);
    m_layoutMain.removeWidget(&m_messageText);
    m_layoutMain.removeWidget(&m_conversationText);
    m_layoutMain.removeWidget(&m_btnConnectDisconnect);
    m_layoutMain.removeWidget(&m_editForHost);
}



void chatsystem::simple::CentralWidget::ConnectDisconnectClickedSlot(void)
{
    if(THIS_APP()->isConnected()){
        THIS_APP()->DisconnectAnyThread();
        m_editForHost.setReadOnly(false);
    }
    else{
        int nPort(DEFAULT_CHAT_PORT);
        QString hostName = m_editForHost.text();
        std::string strHostName( hostName.toStdString() );
        char* pcPort = strchr(const_cast<char*>(strHostName.c_str()),':');

        if(pcPort){
            nPort = atoi(pcPort+1);
            *pcPort = 0;
        }

        pcPort = strchr(const_cast<char*>(strHostName.c_str()),'|');
        if(pcPort){*pcPort = 0; }

        m_editForHost.setReadOnly(true);
        THIS_APP()->ConnectAnyThread( std::move(strHostName),nPort);
    }
}



void chatsystem::simple::CentralWidget::ConnectionTryFinishedSlot(bool a_isSuccess)
{
    if(a_isSuccess){

        m_conversationText.append("+++ ");
        m_conversationText.insertPlainText(m_editForHost.text());
        m_conversationText.insertPlainText(" connected +++");

        m_nLastHostUnableToConnect = 0;
        m_btnConnectDisconnect.setText(TEXT_FOR_DISCONNECT);
        m_messageText.setReadOnly(false);
        m_editForHost.setStyleSheet("background-color:rgb(0,255,0);color:blue;");
    }
    else{
        m_nLastHostUnableToConnect = 1;
        m_editForHost.setStyleSheet("background-color:rgb(255,0,0);color:rgb(255,255,255);");
        m_editForHost.setReadOnly(false);
        m_messageText.setReadOnly(true);
    }
}


void chatsystem::simple::CentralWidget::HostNameChangedSlot(const QString &)
{
    if(m_nLastHostUnableToConnect){
        m_nLastHostUnableToConnect = 0;
        m_editForHost.setStyleSheet("background-color:rgb(255,255,255);color:rgb(0,0,0);");
    }
}



void chatsystem::simple::CentralWidget::IncomingConnectionSlot( ::std::string a_hostName )
{
    m_conversationText.append("+++ ");
    m_conversationText.insertPlainText(a_hostName.c_str());
    m_conversationText.insertPlainText(" connected +++");

    m_nLastHostUnableToConnect = 0;
    m_editForHost.setText(a_hostName.c_str());

    m_editForHost.setReadOnly(true);
    m_editForHost.setStyleSheet("background-color:rgb(0,255,0);");

    m_btnConnectDisconnect.setText(TEXT_FOR_DISCONNECT);
    m_messageText.setReadOnly(false);
}


void chatsystem::simple::CentralWidget::MessageReceivedSlot(::std::string a_message )
{
    m_conversationText.append( (std::string("ot: ")+a_message).c_str());
}


void chatsystem::simple::CentralWidget::DisconnectedSlot(void)
{
    m_btnConnectDisconnect.setText(TEXT_FOR_CONNECT);
    m_messageText.setReadOnly(true);
    m_editForHost.setReadOnly(false);
    m_editForHost.setStyleSheet("background-color:rgb(255,255,255);color:rgb(0,0,0);");

    m_conversationText.append("--- disconnected ---");
}


void chatsystem::simple::CentralWidget::SendMessageSlot(void)
{
    THIS_APP()->SendMessageAnyThread(m_messageText.text().toStdString());
}


void chatsystem::simple::CentralWidget::WriteDoneSlot(void)
{
    //chatsystem::simple::CentralWidget::
    m_conversationText.append( QString("me: ")+m_messageText.text() );
    m_messageText.setText("");
}
