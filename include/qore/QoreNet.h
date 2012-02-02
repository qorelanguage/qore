/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNet.h

  Network functions and macros

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _QORE_QORENET_H

#define _QORE_QORENET_H

#include <sys/types.h>

//! thread-safe gethostbyname (0 = success, !0 = error)
/** FIXME: should be const struct in_addr
 */
DLLEXPORT int q_gethostbyname(const char *host, struct in_addr *sin_addr);

//! thread-safe gethostbyname (0/NULL = error)
DLLEXPORT QoreHashNode *q_gethostbyname_to_hash(const char *host);

//! thread-safe gethostbyname (0/NULL = error)
DLLEXPORT QoreStringNode *q_gethostbyname_to_string(const char *host);

//! thread-safe gethostbyaddr (string returned must be freed)
DLLEXPORT char *q_gethostbyaddr(const char *addr, int len, int type);

//! thread-safe gethostbyaddr (0/NULL = error)
DLLEXPORT QoreHashNode *q_gethostbyaddr_to_hash(ExceptionSink *xsink, const char *addr, int type = Q_AF_INET);

//! thread-safe gethostbyaddr (0/NULL = error)
DLLEXPORT QoreStringNode *q_gethostbyaddr_to_string(ExceptionSink *xsink, const char *addr, int type = Q_AF_INET);

//! converts a network address in network byte order to a string (address_family = AF_INET or AF_INET6), returns 0 on error
/** @see q_addr_to_string2()
 */
DLLEXPORT QoreStringNode *q_addr_to_string(int address_family, const char *addr);

//! converts a network address in network byte order to a string (address_family = AF_INET or AF_INET6), returns 0 on error
DLLEXPORT QoreStringNode *q_addr_to_string2(const struct sockaddr *ai_addr);

//! get port from struct sockaddr, returns -1 if port cannot be determined
DLLEXPORT int q_get_port_from_addr(const struct sockaddr *ai_addr);

//! returns address info as a hash
DLLEXPORT QoreListNode *q_getaddrinfo_to_list(ExceptionSink *xsink, const char *node, const char *service, int family = Q_AF_UNSPEC, int flags = 0, int socktype = Q_SOCK_STREAM);

//! provides an interface to getaddrinfo
class QoreAddrInfo {
protected:
   struct addrinfo *ai;
   bool has_svc;

public:
   //! create an empty structure
   DLLEXPORT QoreAddrInfo();

   //! destroy the object
   DLLEXPORT ~QoreAddrInfo();

   //! clears the current results, if any
   DLLEXPORT void clear();

   //! get address info with the given parameters, if any errors occur, a Qore-language exception is thrown
   /** @param xsink if any errors occur, Qore-language exception info is added to this object
       @param node the node name for the lookup
       @param service the service name (from /etc/services, for example) or port number
       @param family a hint for the address family, AF_UNSPEC means any family
       @param flags hint flags as per the getaddrinfo() call: AI_ADDRCONFIG, AI_ALL, AI_CANONNAME, AI_NUMERICHOST, AI_NUMERICSERV, AI_PASSIVE, AI_V4MAPPED
       @param socktype a hint for the type of socket; 0 = any socket type
       @param protocol a hint for the protocol number; 0 = the default protocol
    */
   DLLEXPORT int getInfo(ExceptionSink *xsink, const char *node, const char *service, int family = Q_AF_UNSPEC, int flags = 0, int socktype = Q_SOCK_STREAM, int protocol = 0);

   //! returns the struct addrinfo * being managed (may by 0)
   DLLLOCAL struct addrinfo *getAddrInfo() const {
      return ai;
   }
   
   //! returns a list of hashes of address info, if an addrinfo structure is being managed
   DLLEXPORT QoreListNode *getList() const;   

   //! returns the name of the address family as a string (ie AF_INET = "ipv4", etc)
   DLLEXPORT static const char *getFamilyName(int address_family);

   //! returns a descriptive string for the address family and an address string (ie AF_INET6, "::1" = "ipv6[::1]")
   DLLEXPORT static QoreStringNode *getAddressDesc(int address_family, const char *addr);
};

#endif // _QORE_QORENET_H
