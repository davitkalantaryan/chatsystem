
// omxclient_qtapplication.hpp
// 2017 Aug 06

#ifndef OMXCLIENT_QTAPPLICATION_HPP
#define OMXCLIENT_QTAPPLICATION_HPP

#include <stdint.h>
#include <QApplication>
#include <thread>
#include <vector>
#include <string>
#include <QPushButton>
#include <QUrl>
#include "omxclient_commondefs.hpp"
#include <QTableWidget>
#include <queue>
#include <mutex>
#include <common/common_unnamedsemaphorelite.hpp>
#include <utility>
#include <cpp11+/thread_cpp11.hpp>
#include <common/common_servertcp.hpp>

namespace chatsystem { namespace simple{

enum class TaskType:uint32_t{Connect,Disconnect,AcceptConnect,Write,ReceivError};
struct SConnection{
    ::std::string hostName; int port; TaskType type;
    SConnection( std::string&& a_hostName, int a_port, TaskType a_type):hostName(std::move(a_hostName)),port(a_port),type(a_type){}
};

class QtApplication : public QApplication
{
    Q_OBJECT
public:
    QtApplication(int &argc, char **argv);
    ~QtApplication();

    // Other API
    void ConnectAnyThread(std::string&& pairName, int pairPort);
    void DisconnectAnyThread();
    void SendMessageAnyThread(std::string&& a_message);

    uint64_t isConnected()const;
    void SetServerPortAnyThread(int port);

    void execGui();
    int getServerPort()const;

public:
signals:
    void ConnectionTryFinishedSignal(bool);
    void DisconnectedSignal(void);
    void WriteDoneSignal(void);
    void IncomingConnectionSignal( ::std::string );
    void ServerPortChangedSignal( int );
    void MessageReceivedSignal( ::std::string );

private:
    void DeviceThreadFunction(void);
    void ServerAndReceiveThread(void);
    void ReceiveDataPrivate(int a_nSocket);

    void StopThreads();

private:
    volatile uint64_t               m_nConnected : 1;
    volatile uint64_t               m_nWork : 1;
    volatile uint64_t               m_nServerBinded : 1;
    STDN::thread                    m_threadDev;
    STDN::thread                    m_threadServer;
    ::std::queue< SConnection >     m_fifoTasks;
    ::common::UnnamedSemaphoreLite  m_semaForDeviceThread;
    ::std::mutex                    m_mutexForFifo;
    common::ServerTCP               m_server;
    int                             m_nServerPort;
    int                             m_nSocket;

};

}}  // namespace chatsystem { namespace simple{

//#define THIS_APP()    ((QtApplication*)(QCoreApplication::instance()))
#define THIS_APP()      (  static_cast<QtApplication*>(QCoreApplication::instance())  )


#endif // OMXCLIENT_QTAPPLICATION_HPP
