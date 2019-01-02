
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

namespace chatsystem { namespace simple{

enum class TaskType{Connect,Disconnect,AcceptConnect};
struct SConnection{ ::std::string hostName; int port, type; SConnection( std::string&& a_hostName, int a_port, int a_type):hostName(std::move(a_hostName)),port(a_port),type(a_type){} };

class QtApplication : public QApplication
{
    Q_OBJECT
public:
    QtApplication(int &argc, char **argv);
    ~QtApplication();

    // Other API
    void ConnectAnyThread(std::string&& pairName, int pairPort=DEFAULT_CHAT_PORT);
    void DisconnectAnyThread();

public:
signals:
    void ConnectionTryFinishedSignal(bool);
    void DisconnectedSignal(void);

private:
    void DeviceThreadFunction(void);

private:
    uint64_t                        m_nConnected : 1;
    uint64_t                        m_nWork : 1;
    STDN::thread                    m_threadDev;
    ::std::queue< SConnection >     m_fifoTasks;
    ::common::UnnamedSemaphoreLite  m_semaForDeviceThread;
    ::std::mutex                    m_mutexForFifo;

};

}}  // namespace chatsystem { namespace simple{

#define THIS_APP    ((QtApplication*)(QCoreApplication::instance()))
#define m_pActiveButtonMacro    THIS_APP->m_pActiveButton


#endif // OMXCLIENT_QTAPPLICATION_HPP
