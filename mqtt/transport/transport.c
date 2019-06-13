/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *    Sergio R. Caprile - "commonalization" from prior samples and/or documentation extension
 *******************************************************************************/
 
 /**********************************************************************************************************
** 文件名		:transport.c
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-08-08
** 描述			:mqtt通信接口文件，串口移植请修改此文件
************************************************************************************************************/

//#include <sys/types.h>

#if !defined(SOCKET_ERROR)
	/** error in socket operation */
	#define SOCKET_ERROR -1
#endif

//#if defined(WIN32)
///* default on Windows is 64 - increase to make Linux and Windows the same */
//#define FD_SETSIZE 1024
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#define MAXHOSTNAMELEN 256
//#define EAGAIN WSAEWOULDBLOCK
//#define EINTR WSAEINTR
//#define EINVAL WSAEINVAL
//#define EINPROGRESS WSAEINPROGRESS
//#define EWOULDBLOCK WSAEWOULDBLOCK
//#define ENOTCONN WSAENOTCONN
//#define ECONNRESET WSAECONNRESET
//#define ioctl ioctlsocket
//#define socklen_t int
//#else
#define INVALID_SOCKET SOCKET_ERROR
//#include <socket.h>
//#include <sys/param.h>
//#include <sys/time.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
//#include <unistd.h>
#include <errno.h>
//#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
//#endif

//#if defined(WIN32)
//#include <Iphlpapi.h>
//#else
//#include <sys/ioctl.h>
//#include <net/if.h>
//#endif

#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "malloc.h"
#include "delay.h"
#include "usart.h"  
#include <stdio.h>
#include "ucos_ii.h" 
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dns.h"

#include "transport.h"

/**
This simple low-level implementation assumes a single connection for a single thread. Thus, a static
variable is used for that connection.
On other scenarios, the user must solve this by taking into account that the current implementation of
MQTTPacket_read() has a function pointer for a function call to get the data to a buffer, but no provisions
to know the caller or other indicator (the socket id): int (*getfn)(unsigned char*, int)
*/
static int mysock = INVALID_SOCKET;


int transport_sendPacketBuffer(int sock, unsigned char* buf, int buflen)
{
	int rc = 0;
	rc = write(sock, buf, buflen);
	return rc;
}


int transport_getdata(unsigned char* buf, int count)
{
	int rc = recv(mysock, buf, count, 0);
	//printf("received %d bytes count %d\n", rc, (int)count);
	return rc;
}

int transport_getdatanb(void *sck, unsigned char* buf, int count)
{
	int sock = *((int *)sck); 	/* sck: pointer to whatever the system may use to identify the transport */
	/* this call will return after the timeout set on initialization if no bytes;
	   in your system you will use whatever you use to get whichever outstanding
	   bytes your socket equivalent has ready to be extracted right now, if any,
	   or return immediately */
	int rc = recv(sock, buf, count, 0);	
	if (rc == -1) {
		/* check error conditions from your system here, and return -1 */
		return 0;
	}
	return rc;
}

/**
return >=0 for a socket descriptor, <0 for an error code
@todo Basically moved from the sample without changes, should accomodate same usage for 'sock' for clarity,
removing indirections
*/
//int transport_open(char* addr, int port)
//{
//int* sock = &mysock;
//	int type = SOCK_STREAM;
//	struct sockaddr_in address;
//#if defined(AF_INET6)
//	struct sockaddr_in6 address6;
//#endif
//	int rc = -1;
//#if defined(WIN32)
//	short family;
//#else
//	sa_family_t family = AF_INET;
//#endif
//	struct addrinfo *result = NULL;
//	struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};
//	static struct timeval tv;

//	*sock = -1;
//	if (addr[0] == '[')
//	  ++addr;

//	if ((rc = getaddrinfo(addr, NULL, &hints, &result)) == 0)
//	{
//		struct addrinfo* res = result;

//		/* prefer ip4 addresses */
//		while (res)
//		{
//			if (res->ai_family == AF_INET)
//			{
//				result = res;
//				break;
//			}
//			res = res->ai_next;
//		}

//#if defined(AF_INET6)
//		if (result->ai_family == AF_INET6)
//		{
//			address6.sin6_port = htons(port);
//			address6.sin6_family = family = AF_INET6;
//			address6.sin6_addr = ((struct sockaddr_in6*)(result->ai_addr))->sin6_addr;
//		}
//		else
//#endif
//		if (result->ai_family == AF_INET)
//		{
//			address.sin_port = htons(port);
//			address.sin_family = family = AF_INET;
//			address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
//		}
//		else
//			rc = -1;

//		freeaddrinfo(result);
//	}

//	if (rc == 0)
//	{
//		*sock =	socket(family, type, 0);
//		if (*sock != -1)
//		{
//#if defined(NOSIGPIPE)
//			int opt = 1;

//			if (setsockopt(*sock, SOL_SOCKET, SO_NOSIGPIPE, (void*)&opt, sizeof(opt)) != 0)
//				Log(TRACE_MIN, -1, "Could not set SO_NOSIGPIPE for socket %d", *sock);
//#endif

//			if (family == AF_INET)
//				rc = connect(*sock, (struct sockaddr*)&address, sizeof(address));
//	#if defined(AF_INET6)
//			else
//				rc = connect(*sock, (struct sockaddr*)&address6, sizeof(address6));
//	#endif
//		}
//	}
//	if (mysock == INVALID_SOCKET)
//		return rc;

//	tv.tv_sec = 1;  /* 1 second Timeout */
//	tv.tv_usec = 0;  
//	setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
//	return mysock;
//}

int transport_open(char* addr, int port)
{
	int* sock = &mysock;
	struct hostent *server;
	struct sockaddr_in serv_addr;
	static struct  timeval tv;
	int timeout = 1000;
	fd_set readset;
	fd_set writeset;
	*sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*sock < 0)
		printf("[ERROR] Create socket failed\n");
	
	server = gethostbyname(addr);
	if(server == NULL)
		printf("[ERROR] Get host ip failed\n");
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
	if(connect(*sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("[ERROR] connect failed\n");
        return -1;
	}
	tv.tv_sec = 10;  /* 1 second Timeout */
	tv.tv_usec = 0; 
	setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,sizeof(timeout));

    return mysock;
}

int transport_close(int sock)
{
	int rc;

	rc = shutdown(sock, SHUT_WR);
	rc = recv(sock, NULL, (size_t)0, 0);
	rc = close(sock);

	return rc;
}
