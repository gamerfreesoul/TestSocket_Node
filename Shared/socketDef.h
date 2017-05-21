#ifndef _SOCKET_DEF_H_
#define _SOCKET_DEF_H_

#ifdef WIN32
	#include <winsock2.h>
	#define GETERROR			WSAGetLastError()
	#define CLOSESOCKET(s)		closesocket(s)
	#define IOCTLSOCKET(s,c,a)  ioctlsocket(s,c,a)
	#define CONN_INPRROGRESS	WSAEWOULDBLOCK
	#ifndef socklen_t			// python already define socklen_t...	
	typedef int socklen_t;
	#endif

	#pragma comment(lib, "ws2_32.lib")
#else
	#include <sys/time.h>
	#include <stddef.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <sys/wait.h>
	#include <sys/poll.h>

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
	#undef __USE_GNU
	#include <netdb.h>
	#define __USE_GNU
	#include <sys/errno.h>
	#include <arpa/inet.h>

	typedef int SOCKET;
	typedef sockaddr_in			SOCKADDR_IN;
	typedef sockaddr			SOCKADDR;
	#define INVALID_SOCKET	    -1
	#define SOCKET_ERROR        -1
	#define GETERROR			errno
	#define WSAEWOULDBLOCK		EWOULDBLOCK
	#define CLOSESOCKET(s)		close(s)
	#define IOCTLSOCKET(s,c,a)  ioctl(s,c,a)
	#define CONN_INPRROGRESS	EINPROGRESS
#endif


#endif //_SOCKET_DEF_H_
