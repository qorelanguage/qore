/*
  QoreNet.cc

  Network functions

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/QoreNet.h>
#include <qore/support.h>

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

// FIXME: check err?
int q_gethostbyname(char *host, struct in_addr *sin_addr)
{
   tracein("q_gethostbyname()");
   
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   if (gethostbyname_r(host, &he, buf, BUFSIZE, &p, &err))
   {
      traceout("q_gethostbyname()");
      return -1;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, BUFSIZE, &err))
   {
      printd(5, "q_gethostbyname() Solaris gethostbyname_r() returned NULL");
      traceout("q_gethostbyname()");
      return -1;
   }
# endif // HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   memcpy((char *)sin_addr, (char *)he.h_addr, he.h_length);
#else  // else if !HAVE_GETHOSTBYNAME_R
   struct hostent *he;
   lck_gethostbyname.lock();
   if (!(he = gethostbyname(host)))
   {
      //herror("q_gethostbyname()");
      lck_gethostbyname.unlock();
      traceout("q_gethostbyname()");
      return -1;
   }
   memcpy((char *)sin_addr, (char *)he->h_addr, he->h_length);
   lck_gethostbyname.unlock();
#endif
   traceout("q_gethostbyname()");
   return 0;
}

// thread-safe gethostbyaddr (string returned must be freed)
// FIXME: check err?
char *q_gethostbyaddr(const char *addr, int len, int type)
{
   char *host;
   
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (gethostbyaddr_r(addr, len, type, &he, buf, BUFSIZE, &err))
      host = strdup(he.h_name);
   else
      host = NULL;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;
   
   if (!gethostbyaddr_r(addr, len, type, &he, buf, BUFSIZE, &p, &err))
      host = strdup(he.h_name);
   else
      host = NULL;
# endif // HAVE_SOLARIS_STYLE_GETHOST
#else  // else if !HAVE_GETHOSTBYADDR_R
   lck_gethostbyaddr.lock();
   struct hostent *he;
   if ((he = gethostbyaddr(addr, len, type)))
      host = strdup(he->h_name);
   else
      host = NULL;
   lck_gethostbyaddr.unlock();
#endif // HAVE_GETHOSTBYADDR_R
   return host;
}

