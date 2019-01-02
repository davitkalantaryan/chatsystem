
#include "chatsystem_simple_centralwidget.hpp"

#define TEXT_FOR_CONNECT    "Connect"

chatsystem::simple::CentralWidget::CentralWidget()
{
    m_btnSendMsg.setText("Send");
    m_btnConnectDisconnect.setText(TEXT_FOR_CONNECT);
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
}



chatsystem::simple::CentralWidget::~CentralWidget()
{
    m_layoutMain.removeWidget(&m_btnSendMsg);
    m_layoutMain.removeWidget(&m_messageText);
    m_layoutMain.removeWidget(&m_conversationText);
    m_layoutMain.removeWidget(&m_btnConnectDisconnect);
    m_layoutMain.removeWidget(&m_editForHost);
}


