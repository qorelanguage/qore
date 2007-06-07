/*
  QoreNet.cc

  Network functions

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

#include <qore/Qore.h>

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>

// FIXME: check err?
int q_gethostbyname(const char *host, struct in_addr *sin_addr)
{
   tracein("q_gethostbyname()");
   
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[NET_BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   if (gethostbyname_r(host, &he, buf, NET_BUFSIZE, &p, &err))
   {
      // NOTE: ERANGE means that the buffer was too small
      //printd(5, "gethostbyname_r() host=%s (bs=%d) error=%d: %d: %s\n", host, NET_BUFSIZE, err, errno, strerror(errno));
      traceout("q_gethostbyname()");
      return -1;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, NET_BUFSIZE, &err))
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

#define NET_BUF_LEN 80
static class Hash *he_to_hash(struct hostent &he)
{
   class Hash *h = new Hash();
   
   if (he.h_name && he.h_name[0])
      h->setKeyValue("name", new QoreNode(he.h_name), 0); // official host name
   if (he.h_aliases)
   {
      class List *l = new List();
      char **a = he.h_aliases;
      while (*a)
	 l->push(new QoreNode(*(a++)));
      h->setKeyValue("aliases", new QoreNode(l), 0);
   }
   switch (he.h_addrtype)
   {
      case AF_INET:
	 h->setKeyValue("typename", new QoreNode("ipv4"), 0);
	 h->setKeyValue("type", new QoreNode((int64)AF_INET), 0);
	 break;

      case AF_INET6:
	 h->setKeyValue("typename", new QoreNode("ipv6"), 0);
	 h->setKeyValue("type", new QoreNode((int64)AF_INET6), 0);
	 break;

      default:
	 h->setKeyValue("typename", new QoreNode("unknown"), 0);
   }
   h->setKeyValue("len", new QoreNode((int64)he.h_length), 0);

   if (he.h_addr_list)
   {
      char buf[NET_BUF_LEN];

      class List *l = new List();
      char **a = he.h_addr_list;
      while (*a)
      {
	 if (inet_ntop(he.h_addrtype, *(a++), buf, NET_BUF_LEN))
	    l->push(new QoreNode(buf));
      }
      h->setKeyValue("addresses", new QoreNode(l), 0);
   }

   return h;
}

class Hash *q_gethostbyname_to_hash(const char *host)
{  
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[NET_BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   if (gethostbyname_r(host, &he, buf, NET_BUFSIZE, &p, &err))
   {
      // NOTE: ERANGE means that the buffer was too small
      //printd(5, "gethostbyname_r() host=%s (bs=%d) error=%d: %d: %s\n", host, NET_BUFSIZE, err, errno, strerror(errno));
      return 0;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, NET_BUFSIZE, &err))
   {
      printd(5, "q_gethostbyname() Solaris gethostbyname_r() returned NULL");
      return 0;
   }
# endif // HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   return he_to_hash(he);
#else  // else if !HAVE_GETHOSTBYNAME_R
   struct hostent *he;
   AutoLocker al(&lck_gethostbyname);
   if (!(he = gethostbyname(host)))
   {
      //herror("q_gethostbyname()");
      lck_gethostbyname.unlock();
      return 0;
   }
   return he_to_hash(*he);
#endif
}

// thread-safe gethostbyaddr (string returned must be freed)
// FIXME: check err?
char *q_gethostbyaddr(const char *addr, int len, int type)
{
   char *host;
   
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[NET_BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (gethostbyaddr_r(addr, len, type, &he, buf, NET_BUFSIZE, &err))
      host = strdup(he.h_name);
   else
      host = NULL;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;
   
   if (!gethostbyaddr_r(addr, len, type, &he, buf, NET_BUFSIZE, &p, &err))
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

// thread-safe gethostbyaddr
// FIXME: check err?
class Hash *q_gethostbyaddr_to_hash(const char *addr, int type)
{
   in_addr sin_addr;
   in6_addr sin6_addr;
   void *dst;
   int len;

   if (type == AF_INET)
   {
      dst = (void *)&sin_addr;
      len = sizeof(sin_addr);
   }
   else if (type == PF_INET)
   {
      dst = (void *)&sin6_addr;
      len = sizeof(sin6_addr);
   }
   else
      return 0;

   if (inet_pton(type, addr, dst) <= 0)
      return 0;
   
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[NET_BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (!gethostbyaddr_r(dst, len, type, &he, buf, NET_BUFSIZE, &err))
      return 0;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;
   
   if (gethostbyaddr_r(dst, len, type, &he, buf, NET_BUFSIZE, &p, &err))
      return 0;
# endif // HAVE_SOLARIS_STYLE_GETHOST

   return he_to_hash(he);

#else  // else if !HAVE_GETHOSTBYADDR_R
   AutoLocker al(&lck_gethostbyaddr);
   struct hostent *he;
   if (!(he = gethostbyaddr(dst, len, type)))
      return 0;

   return he_to_hash(*he);
#endif // HAVE_GETHOSTBYADDR_R
}


