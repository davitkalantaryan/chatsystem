
// omxclient_qtapplication.cpp
// 2017 Aug 06

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "chatsystem_simple_qtapplication.hpp"
#include <common/common_sockettcp.hpp>


chatsystem::simple::QtApplication::QtApplication(int &a_argc, char **a_argv)
    :
      QApplication(a_argc,a_argv)
{
    m_nConnected = 0;
    m_nWork = 1;
    m_threadDev = STDN::thread(&QtApplication::DeviceThreadFunction,this);
}


chatsystem::simple::QtApplication::~QtApplication()
{
    m_nWork=0;
    m_semaForDeviceThread.post();
    m_threadDev.join();
}


void chatsystem::simple::QtApplication::ConnectAnyThread( std::string&& a_pairName, int a_pairPort)
{
    m_mutexForFifo.lock();
    m_fifoTasks.push(  SConnection(std::move(a_pairName),a_pairPort,(int)TaskType::Connect)  );
    m_mutexForFifo.unlock();
    m_semaForDeviceThread.post();
}


void chatsystem::simple::QtApplication::DisconnectAnyThread()
{
    //
}


void chatsystem::simple::QtApplication::DeviceThreadFunction(void)
{
    common::SocketTCP aSocket, aSocketToAccept;
    ::std::string strTaskStr;
    int nPortOrSocketOrLen;
    int nSndRcv;
    TaskType nType;

    while(m_nWork){
        m_semaForDeviceThread.wait();

        m_mutexForFifo.lock();

        if(m_fifoTasks.size()){
            strTaskStr = std::move(m_fifoTasks.front().hostName);
            nPortOrSocketOrLen = m_fifoTasks.front().port;
            nType = (TaskType)m_fifoTasks.front().type;
            m_fifoTasks.pop();
        }else{
            m_mutexForFifo.unlock();
            continue;
        }

        m_mutexForFifo.unlock();

        switch(nType){
        case TaskType::Connect:
            if(m_nConnected){
                aSocket.closeC();
                m_nConnected=0;
            }
            nSndRcv=aSocket.connectC(strTaskStr.c_str(),nPortOrSocketOrLen);
            if(!nSndRcv){
                m_nConnected=1;
                ConnectionTryFinishedSignal(true);
            }
            else{ConnectionTryFinishedSignal(false);}
            break;
        case TaskType::Disconnect:
            if(m_nConnected){
                aSocket.closeC();
                m_nConnected=0;
            }
            break;
        //case TaskType::AcceptConnect:
            //break;
        default:
            if(m_nConnected){
                aSocketToAccept.SetNewSocketDescriptor(nPortOrSocketOrLen);
                aSocketToAccept.closeC();
            }
            else{
                aSocket.SetNewSocketDescriptor(nPortOrSocketOrLen);
                m_nConnected=1;
            }
            break;
        }

    }  // while(m_nWork){
}
