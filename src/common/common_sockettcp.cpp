
// common_sockettcp.cpp
// 2017 Jul 06

#include "common_sockettcp.hpp"

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <memory.h>
#endif


common::SocketTCP::~SocketTCP()
{
}

#ifdef _MSC_VER
#if(_MSC_VER >= 1400)
//#define		_WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#endif
#endif

int common::SocketTCP::connectC(const char *a_svrName, int a_port, int a_connectionTimeoutMs)
{	
	const char *host = NULL;
	fd_set rfds;
	int rtn = -1;
	int maxsd = 0;
	char l_host[MAX_HOSTNAME_LENGTH];

	closeHard();

	m_socket = (int)socket(AF_INET, SOCK_STREAM, 0);
#ifdef	WIN32
	if (m_socket == INVALID_SOCKET)
#else
	if (m_socket < 0)
#endif
	{
		m_socket = -1;
		return E_NO_SOCKET;
	}

	//nRet = ConnectToServer(a_szName, a_nPort, a_lnTimeout);
	

	host = a_svrName;

	if (host == NULL || *host == '\0')
	{

		if (gethostname(l_host, MAX_HOSTNAME_LENGTH) < 0)
		{
			return E_UNKNOWN_HOST;
		}

		host = l_host;
	}

	unsigned long ha;
	struct sockaddr_in addr;
	memset((char *)&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
#ifdef	WIN32
	addr.sin_port = htons((u_short)a_port);
#else
	addr.sin_port = htons(a_nPort);
#endif

	if ((ha = inet_addr(a_svrName)) == INADDR_NONE)
	{
		struct hostent* hostent_ptr = gethostbyname(a_svrName);

		if (!hostent_ptr)
		{
			return E_UNKNOWN_HOST;
		}

		a_svrName = inet_ntoa(*(struct in_addr *)hostent_ptr->h_addr_list[0]);

		if ((ha = inet_addr(a_svrName)) == INADDR_NONE)
		{
			return E_UNKNOWN_HOST;
		}
	}

	memcpy((char *)&addr.sin_addr, (char *)&ha, sizeof(ha));

#ifdef	WIN32
	unsigned long on = 1;
	ioctlsocket(m_socket, FIONBIO, &on);
#else  /* #ifdef	WIN32 */
	int status;
	if ((status = fcntl(m_socket, F_GETFL, 0)) != -1)
	{
		status |= O_NONBLOCK;
		fcntl(m_socket, F_SETFL, status);
	}
#endif  /* #ifdef	WIN32 */

	int addr_len = sizeof(addr);
	rtn = ::connect(m_socket, (struct sockaddr *) &addr, addr_len);

	if (rtn != 0)
	{
		int nErrno2 = errno;///?
		if (!SOCKET_INPROGRESS(nErrno2))
			return E_NO_CONNECT;
	}


	//////////////////////////////////////////////////////////////////////////
	FD_ZERO(&rfds);
	FD_SET((unsigned int)m_socket, &rfds);
	maxsd = (int)(m_socket + 1);

	struct timeval		aTimeout2;
	struct timeval* pTimeout;

	if (a_connectionTimeoutMs >= 0)
	{
#ifdef WIN32
		aTimeout2.tv_sec = a_connectionTimeoutMs / 1000L;
		aTimeout2.tv_usec = (a_connectionTimeoutMs % 1000L) * 1000L;
#else
		aTimeout2.tv_sec = (time_t)(a_connectionTimeoutMs / 1000L);
		aTimeout2.tv_usec = (suseconds_t)((a_connectionTimeoutMs % 1000L) * 1000L);
#endif
		pTimeout = &aTimeout2;
	}
	else
	{
		pTimeout = NULL;
	}

	rtn = select(maxsd, (fd_set *)0, &rfds, (fd_set *)0, pTimeout);

	switch (rtn)
	{
	case 0:	/* time out */
		return _SOCKET_TIMEOUT_;
#ifdef	WIN32
	case SOCKET_ERROR:
#else
	case -1:
#endif
		if (errno == EINTR)
		{
			/* interrupted by signal */
			return _EINTR_ERROR_;
		}

		return E_SELECT;
	default:
		break;
	}

	if (!FD_ISSET(m_socket, &rfds))
	{
		return E_FATAL; // Should be handled
	}
	///////////////////////////////////////////////////////////////////////////////

	return 0;

}


#ifndef MSEC
#include <sys/timeb.h>
#define MSEC(finish, start)	( (long)( (finish).millitm - (start).millitm ) + \
							(long)( (finish).time - (start).time ) * 1000 )
#endif


int common::SocketTCP::readC(void* a_pBuffer, int a_nSize, int a_lnSelectTm)
{
	long a_nRcvTmout = 10; // should be removed

	char* pcBuffer = (char*)a_pBuffer;

	fd_set rfds;
	char *cp = NULL;
	int maxsd = 0;
	int nTry(0);

	FD_ZERO(&rfds);
	FD_SET((unsigned int)m_socket, &rfds);
	maxsd = m_socket + 1;

	// In not windows cases pselect instead of select can be considered
	// The reason that in pselect m_Timout remains constant
	// But here we choose other solution
	struct timeval		aTimeout2;
	struct timeval*		pTimeout;

	if (a_lnSelectTm >= 0)
	{
		aTimeout2.tv_sec = a_lnSelectTm / 1000;
		aTimeout2.tv_usec = (a_lnSelectTm % 1000) * 1000;
		pTimeout = &aTimeout2;
	}
	else
	{
		pTimeout = NULL;
	}


	nTry = select(maxsd, &rfds, (fd_set *)0, (fd_set *)0, pTimeout);

	switch (nTry)
	{
	case 0:	/* time out */
		return _SOCKET_TIMEOUT_;
#ifdef	WIN32
	case SOCKET_ERROR:
#else
	case -1:
#endif
		if (errno == EINTR)
		{
			/* interrupted by signal */
			return _EINTR_ERROR_;
		}

		return E_SELECT;
	default:
		break;
	}

	if (!FD_ISSET(m_socket, &rfds))
	{
		return E_FATAL;
	}


	int nReceived = 0;
	nTry = 0;
	cp = pcBuffer;
	int nRemained = a_nSize;


	timeb				aSettingTime;
	timeb				aCurrentTime;

	ftime(&aSettingTime);

	int nTryReadZero(0);
	long lnReceiveTime;
	int nIteration(0);


	while (nRemained > 0)
	{

		nTry = recv(m_socket, cp, nRemained, 0);

		if (!nIteration++)
		{
			if (!nTry)
			{
				//printf("===============E_CONN_CLOSED\n");
				//exit(1);
				break;
			}
		}

#ifdef	WIN32
		if (nTry == SOCKET_ERROR)
#else
		if (nTry < 0)
#endif
		{
			if (SOCKET_WOULDBLOCK(errno))
			{
				common::SocketBase::SleepN(1);
			}
			else
			{
				return E_RECEIVE;
			}
		}
		else if (nTry == 0)
		{
			if (a_nRcvTmout < 0 && nTryReadZero++ == 2)
				break;
			common::SocketBase::SleepN(1);
		}
		else
		{
			cp += nTry;
			nRemained -= nTry;
			nReceived += nTry;
			//			break;
		}


		ftime(&aCurrentTime);
		lnReceiveTime = MSEC(aCurrentTime, aSettingTime);
		if (a_nRcvTmout >= 0 && lnReceiveTime >= a_nRcvTmout)
		{

			//printf("nReceived = %d instead of %d\n",nReceived,a_nSize);
			break;
			//if( nTry == 0 && nReceived == 0 && a_nSize )return E_CONN_CLOSED;
			//return _SOCKET_TIMEOUT_;
		}
	}

	if (nTry == 0 && nReceived == 0 && a_nSize)
	{
		return E_CONN_CLOSED;
	}

	return nReceived;

}


int common::SocketTCP::writeC(const void* a_cpBuffer, int a_nSize)
{
	const char* pcBuffer = (const char*)a_cpBuffer;
	const char *cp = NULL;
	int len_to_write = 0;
	int len_wrote = 0;
	int n = 0;

#ifdef DEBUG_SOCKET_FUNCTIONS
	static int nRectIter = 0;
	printf("%.2d -> fl:\"%s\",ln:%d,fnc:\"%s(%d)\" => %.2d\n",
		++s_nRecvAndSend, __MY_FILE__, __LINE__, __FUNCTION__, a_nSize, ++nRectIter);
#endif // #ifdef DEBUG_SOCKET_FUNCTIONS

	if (a_nSize <= 0)
		return 0;

	len_to_write = a_nSize;
	cp = pcBuffer;
	while (len_to_write > 0)
	{
		n = send(m_socket, cp, len_to_write, 0);
#ifdef	WIN32
		if (n == SOCKET_ERROR)
#else
		if (n < 0)
#endif
		{
			if (SOCKET_WOULDBLOCK(errno))
			{
				//				break;
				continue;
			}
			else
			{
				return E_SEND;
			}
		}
		else
		{
			cp += n;
			len_to_write -= n;
			len_wrote += n;
		}
	}

	return len_wrote;
}


common::IODevice* common::SocketTCP::Clone()const
{
	return new SocketTCP(*this);
}
