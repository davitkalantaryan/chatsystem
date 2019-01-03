#ifndef CHATSYSTEM_SIMPLE_CENTRALWIDGET_HPP
#define CHATSYSTEM_SIMPLE_CENTRALWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QGridLayout>
//#include <QHBoxLayout>

namespace chatsystem { namespace simple{

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    CentralWidget();
    ~CentralWidget();

private slots:
    void ConnectDisconnectClickedSlot(void);
    void ConnectionTryFinishedSlot(bool);
    void HostNameChangedSlot(const QString &);
    void IncomingConnectionSlot( ::std::string );
    void MessageReceivedSlot(::std::string);
    void DisconnectedSlot(void);
    void SendMessageSlot(void);
    void WriteDoneSlot(void);

private:
    QGridLayout     m_layoutMain;
    QLineEdit       m_editForHost;
    QPushButton     m_btnConnectDisconnect;
    QTextEdit       m_conversationText;
    QLineEdit       m_messageText;
    QPushButton     m_btnSendMsg;

    uint64_t        m_nLastHostUnableToConnect : 1;
};

}} // namespace chatsystem { namespace simple{


#endif // CHATSYSTEM_SIMPLE_CENTRALWIDGET_HPP
