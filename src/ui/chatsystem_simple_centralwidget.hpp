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

private:
    QGridLayout  m_layoutMain;
    QLineEdit   m_editForHost;
    QPushButton m_btnConnectDisconnect;
    QTextEdit   m_conversationText;
    QLineEdit   m_messageText;
    QPushButton m_btnSendMsg;
};

}} // namespace chatsystem { namespace simple{


#endif // CHATSYSTEM_SIMPLE_CENTRALWIDGET_HPP
