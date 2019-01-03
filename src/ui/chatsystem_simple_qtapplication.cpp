
// omxclient_qtapplication.cpp
// 2017 Aug 06

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "chatsystem_simple_qtapplication.hpp"
#include <common/common_sockettcp.hpp>
#include <common/newlockguards.hpp>
//#include <iostream>


chatsystem::simple::QtApplication::QtApplication(int &a_argc, char **a_argv)
    :
      QApplication(a_argc,a_argv),
      m_nServerPort(DEFAULT_CHAT_PORT)
{
    m_nSocket = -1;
    m_nServerBinded=m_nConnected = 0;
    m_nWork = 1;
    m_threadDev = STDN::thread(&QtApplication::DeviceThreadFunction,this);
    m_threadServer = ::STDN::thread(&QtApplication::ServerAndReceiveThread,this);
}


chatsystem::simple::QtApplication::~QtApplication()
{
    StopThreads();
}


void chatsystem::simple::QtApplication::SendMessageAnyThread(std::string&& a_message)
{
    const int nMessageLen(static_cast<int>(a_message.size()));
    m_mutexForFifo.lock();
    m_fifoTasks.push(  SConnection(std::move(a_message),nMessageLen,TaskType::Write)  );
    m_mutexForFifo.unlock();
    m_semaForDeviceThread.post();
}


void chatsystem::simple::QtApplication::ConnectAnyThread( std::string&& a_pairName, int a_pairPort)
{
    m_mutexForFifo.lock();
    m_fifoTasks.push(  SConnection(std::move(a_pairName),a_pairPort,TaskType::Connect)  );
    m_mutexForFifo.unlock();
    m_semaForDeviceThread.post();
}


void chatsystem::simple::QtApplication::DisconnectAnyThread()
{
    m_mutexForFifo.lock();
    m_fifoTasks.push(  SConnection( std::string(" "),-1,TaskType::Disconnect)  );
    m_mutexForFifo.unlock();
    m_semaForDeviceThread.post();
}


uint64_t chatsystem::simple::QtApplication::isConnected()const
{
    return m_nConnected;
}


void chatsystem::simple::QtApplication::SetServerPortAnyThread(int a_port)
{
    m_nServerPort = a_port;
}


void chatsystem::simple::QtApplication::execGui()
{
    exec();
    StopThreads();
}


int chatsystem::simple::QtApplication::getServerPort()const
{
    return m_nServerPort;
}


void chatsystem::simple::QtApplication::StopThreads()
{
    if(!m_nWork){return;}
    m_nWork=0;
    m_server.StopServer();
    m_semaForDeviceThread.post();
    m_threadServer.join();
    m_threadDev.join();
}


void chatsystem::simple::QtApplication::ReceiveDataPrivate(int a_nSocket)
{
    ::std::string strForInform;
    common::SocketTCP clientSock;
    int nSndRcv;
    uint32_t unStrLen;

    clientSock.SetNewSocketDescriptor(a_nSocket);
    while(m_nWork){

        nSndRcv = clientSock.readC(&unStrLen,4);
        if(nSndRcv!=4){break;}

        strForInform.resize(unStrLen);
        nSndRcv = clientSock.readC(const_cast<char*>(strForInform.c_str()),static_cast<int>(unStrLen));
        if(nSndRcv!=static_cast<int>(unStrLen)){break;}

        emit MessageReceivedSignal(strForInform);
    }

    if(m_nWork){
        m_mutexForFifo.lock();
        m_fifoTasks.push(  SConnection( std::string(" "),-1,TaskType::ReceivError)  );
        m_mutexForFifo.unlock();
        m_semaForDeviceThread.post();
        //emit DisconnectedSignal();
        //common::socketN::SleepN()
        SleepIntr(10);
    }

    //a_clientSock.closeC();
}


void chatsystem::simple::QtApplication::ServerAndReceiveThread(void)
{
    sockaddr_in bufForAddr;
    int nIncomingSocket;
    uint32_t unStrLen;
    std::string strForInform;

enterTryPoint:
    try{

        while((!m_nServerBinded)&& m_server.InitServer(m_nServerPort,false)){emit ServerPortChangedSignal(++m_nServerPort);}
        m_nServerBinded = 1;

        while(m_nWork){

            while((!m_nConnected)&&m_nWork){
                nIncomingSocket = m_server.WaitForConnection(1000,&bufForAddr);
                if(nIncomingSocket<0){m_nServerBinded=0;goto enterTryPoint;}
                else if(nIncomingSocket>0){  // we have new connection
                    strForInform = ::common::socketN::GetIPAddress(&bufForAddr);
                    strForInform += "|";
                    unStrLen = static_cast<uint32_t>(strForInform.size());
                    strForInform.resize(unStrLen+512);
                    ::common::socketN::GetHostName(&bufForAddr,const_cast<char*>(strForInform.c_str())+unStrLen,511);

                    emit IncomingConnectionSignal(strForInform);

                    m_mutexForFifo.lock();
                    m_fifoTasks.push(  SConnection(std::move(strForInform),nIncomingSocket,TaskType::AcceptConnect)  );
                    m_mutexForFifo.unlock();
                    m_semaForDeviceThread.post();

                    ReceiveDataPrivate(nIncomingSocket);
                }
                // else // we have timeout
            }

            if(m_nConnected&&(m_nSocket>=0)){ReceiveDataPrivate(m_nSocket);}
        }
    }
    catch(...){
        m_server.closeC();
        goto enterTryPoint;
    }
    m_server.closeC();
}


void chatsystem::simple::QtApplication::DeviceThreadFunction(void)
{
    common::NewLockGuard< std::mutex > aGuard;
    common::SocketTCP aSocket, aSocketToAccept;
    ::std::string strTaskStr;
    int nPortOrSocketOrLen;
    int nSndRcv;
    TaskType nType;

enterTryPoint:
    try{
        while(m_nWork){
            m_semaForDeviceThread.wait();

            aGuard.SetAndLockMutex(&m_mutexForFifo);

            if(m_fifoTasks.size()){
                strTaskStr = std::move(m_fifoTasks.front().hostName);
                nPortOrSocketOrLen = m_fifoTasks.front().port;
                nType = static_cast<TaskType>(m_fifoTasks.front().type);
                m_fifoTasks.pop();
            }else{
                aGuard.UnsetAndUnlockMutex();
                continue;
            }

            aGuard.UnsetAndUnlockMutex();

            m_nSocket = static_cast<int>(aSocket);

            switch(nType){
            case TaskType::Connect:
                if(m_nConnected){
                    aSocket.closeC();
                    m_nConnected=0;
                }
                nSndRcv=aSocket.connectC(strTaskStr.c_str(),nPortOrSocketOrLen,-1);
                m_nSocket = static_cast<int>(aSocket);
                if(!nSndRcv){
                    m_nConnected=1;
                    emit ConnectionTryFinishedSignal(true);
                }
                else{ emit ConnectionTryFinishedSignal(false); }
                break;
            case TaskType::Disconnect:
                if(m_nConnected){
                    aSocket.closeC();
                    m_nConnected=0;
                    emit DisconnectedSignal();
                }
                break;
            case TaskType::Write:
                if(m_nConnected){
                    nSndRcv=aSocket.writeC(&nPortOrSocketOrLen,4);
                    if(nSndRcv!=4){
                        emit DisconnectedSignal();
                        aSocket.closeC();
                        m_nSocket = -1;
                        m_nConnected = 0;
                        goto enterTryPoint;
                    }

                    nSndRcv=aSocket.writeC(strTaskStr.c_str(),nPortOrSocketOrLen);
                    if(nSndRcv!=nPortOrSocketOrLen){
                        emit DisconnectedSignal();
                        aSocket.closeC();
                        m_nSocket = -1;
                        m_nConnected = 0;
                        goto enterTryPoint;
                    }

                    emit WriteDoneSignal();
                }
                break;
            case TaskType::ReceivError:
                if(m_nConnected){
                    emit DisconnectedSignal();
                    aSocket.closeC();
                    m_nSocket = -1;
                    m_nConnected = 0;
                    goto enterTryPoint;
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
                    m_nSocket = nPortOrSocketOrLen;
                    aSocket.SetNewSocketDescriptor(nPortOrSocketOrLen);
                    m_nConnected=1;
                }
                break;
            }

        }  // while(m_nWork){
    }
    catch(...){
        aGuard.UnsetAndUnlockMutex();
        aSocket.closeC();
        m_nConnected=0;
        goto enterTryPoint;
    }

    aSocket.closeC();
}
