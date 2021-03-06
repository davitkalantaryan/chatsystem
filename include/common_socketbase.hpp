
// common_socketbase.hpp
// 2017 Jul 06

#ifndef __common_socketbase_hpp__
#define __common_socketbase_hpp__

#include <common_iodevice.hpp>

#if !defined(E_FATAL) & !defined(E_NO_BIND)

#include <errno.h>

#define E_FATAL			-1	/* fatal error */
#define E_NO_BIND		-7	/* can not bind address to port */
#define E_NO_CONNECT	-8	/* can not connect to server */
#define E_NO_LISTEN		-14	/* can not listen */
#define E_NO_SOCKET		-18	/* no socket generated */
#define E_RECEIVE		-25	/* error by receive */
#define E_SELECT		-28	/* error by select */
#define E_SEND			-29	/* error by send */
#define E_UNKNOWN_HOST		-38	/* can not find host */
#define E_WINSOCK_VERSION	-42	/* WINSOCK DLL version not requested */
#define	E_CONN_CLOSED		-46	/* connecttion closed by peer */

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif

#ifdef	WIN32
//#include <Windows.h>
#ifndef SOCKET_INPROGRESS
#define	SOCKET_INPROGRESS(e)	(WSAGetLastError() == WSAEWOULDBLOCK)
#endif
#else
#include <netinet/in.h>
#if defined(EALREADY) && defined(EAGAIN)
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EALREADY || e == EAGAIN)
#else
#ifdef  EALREADY
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EALREADY)
#else
#ifdef  EAGAIN
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS || e == EAGAIN)
#else
#define	SOCKET_INPROGRESS(e)	(e == EINPROGRESS)
#endif
#endif
#endif
#endif

#ifdef	WIN32
#define	SOCKET_WOULDBLOCK(e)	(WSAGetLastError() == WSAEWOULDBLOCK)
#else
#if defined(EAGAIN) && defined(EWOULDBLOCK)
#define	SOCKET_WOULDBLOCK(e)	(e == EAGAIN || e == EWOULDBLOCK)
#else
#ifdef	EWOULDBLOCK
#define	SOCKET_WOULDBLOCK(e)	(e == EWOULDBLOCK)
#else
#define	SOCKET_WOULDBLOCK(e)	(e == EAGAIN)
#endif
#endif
#endif


#define	_SOCKET_TIMEOUT_		-2001
#define	_NEGATIVE_ERROR_(x)		(((x)>0) ? -(x) : (x))
#define	_EINTR_ERROR_			_NEGATIVE_ERROR_(EINTR)


#define MAX_HOSTNAME_LENGTH     64

#ifdef WIN32
#ifndef socklen_t
#define	socklen_t	int
#endif/* #ifndef socklen_t */
#ifndef snprintf
//#define snprintf _snprintf
#endif /*#ifndef snprintf*/
#else
#endif

#endif/* #if !defined(E_FATAL) & !defined(E_NO_BIND) */

#define sockaddr_in_using_ok
struct sockaddr_in;

namespace common{

class SocketBase : public IODevice
{
public:
	SocketBase();
	virtual ~SocketBase();

	virtual void closeC(void);
	common::SocketBase& operator=(const common::SocketBase& a_nSocket);

	operator const int&()const;
	void SetSockDescriptor(int rawSocket);	
	int DublicateSocket(int procID, void* protInfo)const;

	static int  Initialize();
	static void Cleanup();
	long int SleepN(long long int nanoSec);
#ifdef sockaddr_in_using_ok
	static const char*	GetIPAddress(const sockaddr_in* addr, char* buffer, int buffLen);
	static const char*	GetHostName (const sockaddr_in* addr, char* buffer, int buffLen);
	static int  GetPort(const sockaddr_in* a_addr);
#endif  // #ifdef sockaddr_in_using_ok

protected:
	virtual void closeHard(void);

protected:
	int	   m_socket;
};

}


#endif  // #ifndef __common_socketbase_hpp__
