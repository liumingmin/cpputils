/*
 * Copyright (C) 1997-2005 by Objective Systems, Inc.
 *
 * This software is furnished under an open source license and may be 
 * used and copied only in accordance with the terms of this license. 
 * The text of the license may generally be found in the root 
 * directory of this installation in the COPYING file.  It 
 * can also be viewed online at the following URL:
 *
 *   http://www.obj-sys.com/open/license.html
 *
 * Any redistributions of this file including modified versions must 
 * maintain this copyright notice.
 *
 *****************************************************************************/

#include "sys_socket.h"
#if defined(_WIN32_WCE)
static int inited = 0;
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS 0
#elif defined (_WIN32)
// modify by bluecat 2011.10.14
// reason: windows also supply all these operations

//static LPFN_SEND send;
//static LPFN_SOCKET socket;
//static LPFN_SETSOCKOPT setsockopt;
//static LPFN_BIND bind;
//static LPFN_HTONL htonl;
//static LPFN_HTONS htons;
//static LPFN_CONNECT connect;
//static LPFN_INET_ADDR inet_addr;
//static LPFN_LISTEN listen;
//static LPFN_ACCEPT accept;
//static LPFN_NTOHL ntohl;
//static LPFN_NTOHS ntohs;
//static LPFN_RECV recv;
//static LPFN_SHUTDOWN shutdown;
//
//
//static LPFN_IOCTLSOCKET ioctlsocket;
//static LPFN_SENDTO sendto;
//static LPFN_INET_NTOA inet_ntoa;
//static LPFN_RECVFROM recvfrom;
//static LPFN_SELECT select;
//static LPFN_GETHOSTNAME gethostname;
//static LPFN_GETHOSTBYNAME gethostbyname;
//static LPFN_WSAGETLASTERROR  WSAGetLastError;
//static LPFN_WSACLEANUP WSACleanup;
//static LPFN_CLOSESOCKET closesocket;
//static LPFN_GETSOCKNAME getsockname;
static HMODULE ws32 = 0;
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS SD_BOTH
#else
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS SHUT_RDWR
#define closesocket close
#endif



int sys_SocketsInit ()
{
#if defined(_WIN32_WCE)
   WORD wVersionRequested;
   WSADATA wsaData;
   int err;

   if (inited) return SYS_OK; 

   wVersionRequested = MAKEWORD( 1, 1 );
    
   err = WSAStartup (wVersionRequested, &wsaData);
   if ( err != 0 ) {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.   */
      return SYS_E_NOTINIT;
   }
   inited = 1;

#elif defined (_WIN32)
// modify by bluecat 2011.10.14
// reason: windows also supply all these operations

//   LPFN_WSASTARTUP wsaStartup = NULL;
//   WSADATA wsaData;
//
//   if (ws32 != 0) return SYS_OK;
//
////   ws32 = LoadLibrary ("WSOCK32.DLL");
//  ws32 = LoadLibrary ("WS2_32.DLL");
//   if (ws32 == NULL) return SYS_E_NOTINIT;
//   
//   wsaStartup = (LPFN_WSASTARTUP) GetProcAddress (ws32, "WSAStartup");
//   if (wsaStartup == NULL) return SYS_E_NOTINIT;
//   
//   send = (LPFN_SEND) GetProcAddress (ws32, "send");
//   if (send == NULL) return SYS_E_NOTINIT;
//   
//   socket = (LPFN_SOCKET) GetProcAddress (ws32, "socket");
//   if (socket == NULL) return SYS_E_NOTINIT;
//   
//   setsockopt = (LPFN_SETSOCKOPT) GetProcAddress (ws32, "setsockopt");
//   if (setsockopt == NULL) return SYS_E_NOTINIT;
//   
//   bind = (LPFN_BIND) GetProcAddress (ws32, "bind");
//   if (bind == NULL) return SYS_E_NOTINIT;
//   
//   htonl = (LPFN_HTONL) GetProcAddress (ws32, "htonl");
//   if (htonl == NULL) return SYS_E_NOTINIT;
//   
//   htons = (LPFN_HTONS) GetProcAddress (ws32, "htons");
//   if (htons == NULL) return SYS_E_NOTINIT;
//   
//   connect = (LPFN_CONNECT) GetProcAddress (ws32, "connect");
//   if (connect == NULL) return SYS_E_NOTINIT;
//   
//   listen = (LPFN_LISTEN) GetProcAddress (ws32, "listen");
//   if (listen == NULL) return SYS_E_NOTINIT;
//   
//   accept = (LPFN_ACCEPT) GetProcAddress (ws32, "accept");
//   if (accept == NULL) return SYS_E_NOTINIT;
//   
//   inet_addr = (LPFN_INET_ADDR) GetProcAddress (ws32, "inet_addr");
//   if (inet_addr == NULL) return SYS_E_NOTINIT;
//   
//   ntohl = (LPFN_NTOHL) GetProcAddress (ws32, "ntohl");
//   if (ntohl == NULL) return SYS_E_NOTINIT;
//   
//   ntohs = (LPFN_NTOHS) GetProcAddress (ws32, "ntohs");
//   if (ntohs == NULL) return SYS_E_NOTINIT;
//   
//   recv = (LPFN_RECV) GetProcAddress (ws32, "recv");
//   if (recv == NULL) return SYS_E_NOTINIT;
//   
//   shutdown = (LPFN_SHUTDOWN) GetProcAddress (ws32, "shutdown");
//   if (shutdown == NULL) return SYS_E_NOTINIT;
//   
//   closesocket = (LPFN_CLOSESOCKET) GetProcAddress (ws32, "closesocket");
//   if (closesocket == NULL) return SYS_E_NOTINIT;
//
//   getsockname = (LPFN_GETSOCKNAME) GetProcAddress (ws32, "getsockname");
//   if (getsockname == NULL) return SYS_E_NOTINIT;
//   
//   ioctlsocket = (LPFN_IOCTLSOCKET) GetProcAddress(ws32, "ioctlsocket");
//   if(ioctlsocket == NULL) return SYS_E_NOTINIT;
//
//   sendto = (LPFN_SENDTO) GetProcAddress (ws32, "sendto");
//   if (sendto == NULL) return SYS_E_NOTINIT;
//
//   inet_ntoa = (LPFN_INET_NTOA) GetProcAddress (ws32, "inet_ntoa");
//   if (inet_ntoa == NULL) return SYS_E_NOTINIT;
//
//   recvfrom = (LPFN_RECVFROM) GetProcAddress (ws32, "recvfrom");
//   if (recvfrom == NULL) return SYS_E_NOTINIT;
//
//   select = (LPFN_SELECT) GetProcAddress (ws32, "select");
//   if (select == NULL) return SYS_E_NOTINIT;
//
//   gethostname = (LPFN_GETHOSTNAME) GetProcAddress (ws32, "gethostname");
//   if (gethostname == NULL) return SYS_E_NOTINIT;
//
//   gethostbyname = (LPFN_GETHOSTBYNAME) GetProcAddress (ws32, "gethostbyname");
//   if (gethostbyname == NULL) return SYS_E_NOTINIT;
//   
//   WSAGetLastError = (LPFN_WSAGETLASTERROR) GetProcAddress (ws32, 
//                                                           "WSAGetLastError");
//   if (WSAGetLastError == NULL) return SYS_E_NOTINIT;
//
//   WSACleanup = (LPFN_WSACLEANUP) GetProcAddress (ws32, "WSACleanup");
//   if (WSACleanup == NULL) return SYS_E_NOTINIT;
//   
//      
//   if (wsaStartup (MAKEWORD(1, 1), &wsaData) == -1) return SYS_E_NOTINIT;

	WSADATA wsaData;
	if (WSAStartup (MAKEWORD(1, 1), &wsaData) == -1) 
	{
		return SYS_E_NOTINIT;
	}

#endif
   return SYS_OK;
}

#if defined (_WIN32) || \
defined(_HP_UX) || defined(__hpux) || defined(_HPUX_SOURCE)
typedef int SYSSOCKLEN;
#else
typedef size_t SYSSOCKLEN;
#endif

int sys_SocketCreate (SYS_SOCKET* psocket) 
{
   int on;

   struct linger linger;
   SYS_SOCKET sock = socket (AF_INET,
                             SOCK_STREAM,
                             0);
  
   if (sock == SYS_SOCKET_INVALID){
      _SYS_TRACE("Error:Failed to create TCP socket\n");
      return SYS_E_INVSOCKET;
   }

   on = 1;
   if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, 
                   (const char* ) &on, sizeof (on)) == -1)
   {
      _SYS_TRACE("Error:Failed to set socket option SO_REUSEADDR\n");
      return SYS_E_INVSOCKET;
   }
   linger.l_onoff = 1;
   linger.l_linger = 0;
   if (setsockopt (sock, SOL_SOCKET, SO_LINGER, 
                   (const char* ) &linger, sizeof (linger)) == -1)
   {
      _SYS_TRACE("Error:Failed to set socket option linger\n");
      return SYS_E_INVSOCKET;
   }
   *psocket = sock;
   return SYS_OK;
}

int sys_SocketCreateUDP (SYS_SOCKET* psocket) 
{
   int on;
   struct linger linger;

   SYS_SOCKET sock = socket (AF_INET,
                             SOCK_DGRAM,
                             0);

   if (sock == SYS_SOCKET_INVALID){
      _SYS_TRACE("Error:Failed to create UDP socket\n");
      return SYS_E_INVSOCKET;
   }

   on = 1;
   if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, 
                   (const char* ) &on, sizeof (on)) == -1)
   {
      _SYS_TRACE("Error:Failed to set socket option SO_REUSEADDR\n");
      return SYS_E_INVSOCKET;
   }
   linger.l_onoff = 1;
   linger.l_linger = 0;
   /*if (setsockopt (sock, SOL_SOCKET, SO_LINGER, 
                 (const char* ) &linger, sizeof (linger)) == -1)
      return SYS_E_INVSOCKET;
   */
   *psocket = sock;
   return SYS_OK;
}

int sys_SocketClose (SYS_SOCKET socket)
{
   shutdown (socket, SHUTDOWN_FLAGS);
   if (closesocket (socket) == -1)
      return SYS_E_INVSOCKET;
   return SYS_OK;
}

int sys_SocketBind (SYS_SOCKET socket, SYS_IPADDR addr, int port) 
{
   struct sockaddr_in m_addr;

   if (socket == SYS_SOCKET_INVALID)
   { 
      _SYS_TRACE("Error:Invalid socket passed to bind\n");
      return SYS_E_INVSOCKET;
   }

   memset (&m_addr, 0, sizeof (m_addr));
   m_addr.sin_family = AF_INET;
   m_addr.sin_addr.s_addr = (addr == 0) ? INADDR_ANY : htonl (addr);
   m_addr.sin_port = htons ((unsigned short)port);

   if (bind (socket, (struct sockaddr *) (void*) &m_addr,
                     sizeof (m_addr)) == -1)
   {
      perror ("bind");
      _SYS_TRACE("Error:Bind failed\n");
      return SYS_E_INVSOCKET;
   }

   return SYS_OK;
}


int sys_SocketGetSockName(SYS_SOCKET socket, struct sockaddr_in *name, int *size)
{
   int ret;
   ret = getsockname(socket, (struct sockaddr*)name, size);
   if(ret == 0)
      return SYS_OK;
   else{
      _SYS_TRACE("Error:sys_SocketGetSockName - getsockname\n");
      return SYS_E_INVSOCKET;
   }
}

int sys_SocketGetIpAndPort(SYS_SOCKET socket, char *ip, int len, int *port)
{
   int ret=SYS_OK, size;
   struct sockaddr_in addr;
   char *host=NULL;

   size = sizeof(addr);

   ret = sys_SocketGetSockName(socket, &addr, &size);
   if(ret != 0)
      return SYS_E_INVSOCKET;

   host = inet_ntoa(addr.sin_addr);

   if(host && strlen(host) < (unsigned)len)
      strcpy(ip, host);   
   else{
     _SYS_TRACE("Error:Insufficient buffer for ip address - "
                 "sys_SocketGetIpAndPort\n");
      return -1;
   }
   
   *port = addr.sin_port;

   return SYS_OK;
}

int sys_SocketListen (SYS_SOCKET socket, int maxConnection) 
{
   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;

   if (listen (socket, maxConnection) == -1)
      return SYS_E_INVSOCKET;

   return SYS_OK;
}

int sys_SocketAccept (SYS_SOCKET socket, SYS_SOCKET *pNewSocket, 
                    SYS_IPADDR* destAddr, int* destPort) 
{
   struct sockaddr_in m_addr;
   SYSSOCKLEN addr_length = sizeof (m_addr);

   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;
   if (pNewSocket == 0) return SYS_E_INVPARAM;

   *pNewSocket = accept (socket, (struct sockaddr *) (void*) &m_addr, 
                         &addr_length);
   if (*pNewSocket <= 0) return SYS_E_INVSOCKET;

   if (destAddr != 0) 
      *destAddr = ntohl (m_addr.sin_addr.s_addr);
   if (destPort != 0)
      *destPort = ntohs (m_addr.sin_port);

   return SYS_OK;
}

int sys_SocketConnect (SYS_SOCKET socket, const char* host, int port) 
{
   struct sockaddr_in m_addr;

   if (socket == SYS_SOCKET_INVALID)
   { 
      return SYS_E_INVSOCKET;
   }
   
   memset (&m_addr, 0, sizeof (m_addr));

   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons ((unsigned short)port);
   m_addr.sin_addr.s_addr = inet_addr (host);

   if (connect (socket, (struct sockaddr *) (void*) &m_addr, 
                sizeof (struct sockaddr_in)) == -1)
   {
      return SYS_E_INVSOCKET;
   }
   return SYS_OK;
}
/*
// **Need to add check whether complete data was sent by checking the return
// **value of send and if complete data is not sent then add mechanism to 
// **send remaining bytes. This will make sys_SocketSend call atomic.
*/
int sys_SocketSend (SYS_SOCKET socket, const SYSOCTET* pdata, SYSUINT size)
{
   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;
   
   if (send (socket, (const char*) pdata, size, SEND_FLAGS) == -1)
      return SYS_E_INVSOCKET;
   return SYS_OK;
}

int sys_SocketSendTo(SYS_SOCKET socket, const SYSOCTET* pdata, SYSUINT size,
                     const char* host, int port)
{
   struct sockaddr_in m_addr;
   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;
   
   memset (&m_addr, 0, sizeof (m_addr));

   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons ((unsigned short)port);
   m_addr.sin_addr.s_addr = inet_addr (host);
   if (sendto (socket, (const char*) pdata, size, SEND_FLAGS, 
                                    (const struct sockaddr*)&m_addr, 
                                    sizeof(m_addr)) == -1)
      return SYS_E_INVSOCKET;
   return SYS_OK;
}

int sys_SocketRecvPeek(SYS_SOCKET socket, SYSOCTET* pbuf, SYSUINT bufsize)
{
   int len;
   int flags = MSG_PEEK;

   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;

   if ((len = recv (socket, (char*) pbuf, bufsize, flags)) == -1)
      return SYS_E_INVSOCKET;
   return len;
}

int sys_SocketRecv (SYS_SOCKET socket, SYSOCTET* pbuf, SYSUINT bufsize)
{
   int len;
   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;

   if ((len = recv (socket, (char*) pbuf, bufsize, 0)) == -1)
      return SYS_E_INVSOCKET;
   return len;
}

int sys_SocketRecvFrom (SYS_SOCKET socket, SYSOCTET* pbuf, SYSUINT bufsize,
                      char* remotehost, SYSUINT hostBufLen, int * remoteport)
{
   struct sockaddr_in m_addr;
   int len, addrlen;
   char * host=NULL;
   if (socket == SYS_SOCKET_INVALID) return SYS_E_INVSOCKET;
   addrlen = sizeof(m_addr);

   memset (&m_addr, 0, sizeof (m_addr));
      
   if ((len = recvfrom (socket, (char*) pbuf, bufsize, 0, 
                        (struct sockaddr*)&m_addr, &addrlen)) == -1)
      return SYS_E_INVSOCKET;

   if(remoteport)
      *remoteport = ntohs(m_addr.sin_port);
   if(remotehost)
   {
      host = inet_ntoa(m_addr.sin_addr);
      if(strlen(host) < (hostBufLen-1))
         strcpy(remotehost, host);
      else
         return -1;
   }
   return len;
}

int sys_SocketSelect(int nfds, fd_set *readfds, fd_set *writefds, 
                     fd_set *exceptfds, struct timeval * timeout)
{
   int ret;   
#if defined (_WIN32)
  ret = select(nfds, readfds, writefds, exceptfds, 
             (const struct timeval *) timeout);
#else
   ret = select(nfds, readfds, writefds, exceptfds, timeout);
#endif
   return ret;
}

int sysGetLocalIPAddress(char * pIPAddrs)
{
   int ret;
   struct hostent *phost;
   struct in_addr addr;
   char hostname[100];

   if(pIPAddrs == NULL)
      return -1; /* Need to find suitable return value */
   ret = gethostname(hostname, 100);
   if(ret == 0)
   {
      phost = gethostbyname(hostname);
      if(phost == NULL)
         return -1; /* Need to define a return value if made part of rtsrc */
      memcpy(&addr, phost->h_addr_list[0], sizeof(struct in_addr));
      strcpy(pIPAddrs, inet_ntoa(addr));
     
   }
   else{
      return -1;
   }
   return SYS_OK;
}

int sys_SocketStrToAddr (const char* pIPAddrStr, SYS_IPADDR* pIPAddr) 
{
   int b1, b2, b3, b4;
   int rv = sscanf (pIPAddrStr, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
   if (rv != 4 ||
      (b1 < 0 || b1 > 256) || (b2 < 0 || b2 > 256) ||
      (b3 < 0 || b3 > 256) || (b4 < 0 || b4 > 256))
      return SYS_E_INVPARAM;
   *pIPAddr = ((b1 & 0xFF) << 24) | ((b2 & 0xFF) << 16) | 
              ((b3 & 0xFF) << 8) | (b4 & 0xFF);
   return SYS_OK;
}

int sys_SocketConvertIpToNwAddr(char *inetIp, char *netIp)
{

   struct sockaddr_in sin = {0};
#ifdef _WIN32
   sin.sin_addr.s_addr = inet_addr(inetIp);
   if(sin.sin_addr.s_addr == INADDR_NONE)
   {
      _SYS_TRACE("Error:Failed to convert address\n");
      return -1;
   }
#else
   if(!inet_aton(inetIp, &sin.sin_addr))
   {
      _SYS_TRACE("Error:Failed to convert address\n");
      return -1;
   }
  
#endif
   
   memcpy(netIp, (char*)&sin.sin_addr.s_addr, sizeof(unsigned long));
   return SYS_OK;
}

int sys_SocketAddrToStr (SYS_IPADDR ipAddr, char* pbuf, int bufsize)
{
   char buf1[5], buf2[5], buf3[5], buf4[5];
   int cnt = 0;

   if (bufsize < 8) 
      return SYS_E_BUFOVFLW;

   cnt += sprintf (buf1, "%lu", (ipAddr >> 24) & 0xFF);
   cnt += sprintf (buf2, "%lu", (ipAddr >> 16) & 0xFF);
   cnt += sprintf (buf3, "%lu", (ipAddr >> 8) & 0xFF);
   cnt += sprintf (buf4, "%lu", ipAddr & 0xFF);
   if (bufsize < cnt + 4)
      return SYS_E_BUFOVFLW;
   sprintf (pbuf, "%s.%s.%s.%s", buf1, buf2, buf3, buf4);
   return SYS_OK;
}

int sys_SocketsCleanup (void)
{
#ifdef _WIN32
   int ret = WSACleanup();
   if(ret == 0)
      return SYS_OK;
   else
      return ret;
#endif
   return SYS_OK;
}

long sys_SocketHTONL(long val)
{
   return htonl(val);
}

short sys_SocketHTONS(short val)
{
   return htons(val);
}
