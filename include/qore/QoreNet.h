/*
  QoreNet.h

  Network functions and macros

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef HAVE_GETHOSTBYNAME_R
extern class LockedObject lck_gethostbyname;
#endif
#ifndef HAVE_GETHOSTBYADDR_R
extern class LockedObject lck_gethostbyaddr;
#endif

#define UNIX_PATH_MAX    108
#define NET_BUFSIZE      1024

// thread-safe gethostbyname (0 = success, !0 = error)
DLLEXPORT int q_gethostbyname(const char *host, struct in_addr *sin_addr);
// thread-safe gethostbyname (0/NULL = error)
DLLEXPORT class Hash *q_gethostbyname_to_hash(const char *host);

// thread-safe gethostbyaddr (string returned must be freed)
DLLEXPORT char *q_gethostbyaddr(const char *addr, int len, int type);
// thread-safe gethostbyaddr (0/NULL = error)
DLLEXPORT class Hash *q_gethostbyaddr_to_hash(const char *addr, int type = AF_INET);

#endif // _QORE_QORENET_H
