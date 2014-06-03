/*
  QoreNet.cpp

  Network functions

  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

#define QORE_NET_ADDR_BUF_LEN 80

int q_get_af(int type) {
   if (type >= 0)
      return type;

   switch (type) {
      case Q_AF_UNSPEC:
	 return AF_UNSPEC;
      case Q_AF_INET6:
	 return AF_INET6;
   }

   return AF_INET;
}

int q_get_sock_type(int t) {
   if (t >= 0)
      return t;

   return SOCK_STREAM;
}

int q_addr_to_string(int family, const char *addr, QoreString& str) {
   family = q_get_af(family);

   char buf[QORE_NET_ADDR_BUF_LEN];
   if (!inet_ntop(family, addr, buf, QORE_NET_ADDR_BUF_LEN))
      return -1;
   str.concat(buf);
   return 0;
}

QoreStringNode *q_addr_to_string(int family, const char *addr) {
   family = q_get_af(family);

   char buf[QORE_NET_ADDR_BUF_LEN];
   return inet_ntop(family, addr, buf, QORE_NET_ADDR_BUF_LEN) ? new QoreStringNode(buf) : 0;
}

int q_addr_to_string2(const struct sockaddr* ai_addr, QoreString& str) {
   size_t slen = str.strlen();

   const void *addr;
   if (ai_addr->sa_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)ai_addr;
      addr = &(ipv4->sin_addr);
      str.reserve(slen + INET_ADDRSTRLEN + 1);
   }
   else if (ai_addr->sa_family == AF_INET6) {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai_addr;
      addr = &(ipv6->sin6_addr);
      str.reserve(slen + INET6_ADDRSTRLEN + 1);
   }
#if HAVE_SYS_UN_H
   // windows does not support UNIX sockets, for example
   else if (ai_addr->sa_family == AF_UNIX) {
      struct sockaddr_un* un = (struct sockaddr_un*)ai_addr;
      str.concat(un->sun_path);
      return 0;
   }
#endif
   else
      return -1;

   if (!inet_ntop(ai_addr->sa_family, addr, (char *)(str.getBuffer() + slen), str.capacity() - slen))
      return -1;

   str.terminate(slen + strlen(str.getBuffer() + slen));
   return 0;
}


QoreStringNode *q_addr_to_string2(const struct sockaddr *ai_addr) {
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode);

   return q_addr_to_string2(ai_addr, **str) ? 0 : str.release();
}

int q_get_port_from_addr(const struct sockaddr *ai_addr) {
   if (ai_addr->sa_family == AF_INET) {
      const struct sockaddr_in *ipv4 = (struct sockaddr_in *)ai_addr;
      return ntohs(ipv4->sin_port);
   }
   else if (ai_addr->sa_family == AF_INET6) {
      const struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai_addr;
      return ntohs(ipv6->sin6_port);
   }

   return -1;
}

// FIXME: check err?
int q_gethostbyname(const char *host, struct in_addr *sin_addr) {
   QORE_TRACE("q_gethostbyname()");
   
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[NET_BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   int rc = gethostbyname_r(host, &he, buf, NET_BUFSIZE, &p, &err);
   if (!p || rc) {
      // NOTE: ERANGE means that the buffer was too small
       //printd(0, "gethostbyname_r() host=%s (bs=%d) error=%d: %d: %s\n", host, NET_BUFSIZE, err, errno, strerror(errno));

      return -1;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, NET_BUFSIZE, &err)) {
      printd(5, "q_gethostbyname() Solaris gethostbyname_r() returned NULL");

      return -1;
   }
# endif // HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   memcpy((char *)sin_addr, (char *)he.h_addr, he.h_length);
#else  // else if !HAVE_GETHOSTBYNAME_R
   struct hostent *he;
   lck_gethostbyname.lock();
   if (!(he = gethostbyname(host))) {
      //herror("q_gethostbyname()");
      lck_gethostbyname.unlock();

      return -1;
   }
   memcpy((char *)sin_addr, (char *)he->h_addr, he->h_length);
   lck_gethostbyname.unlock();
#endif

   return 0;
}

static const char* q_af_to_str(int af) {
   switch (af) {
      case AF_INET:
         return "ipv4";
      case AF_INET6:
         return "ipv6";
      case AF_UNIX:
         return "unix";
   }
   return "unknown";
}

void q_af_to_hash(int af, QoreHashNode& h, ExceptionSink* xsink) {
   h.setKeyValue("type", new QoreBigIntNode(af), xsink);
   h.setKeyValue("typename", new QoreStringNode(q_af_to_str(af)), xsink);
}

static QoreHashNode *he_to_hash(struct hostent &he) {
   QoreHashNode *h = new QoreHashNode;
   
   if (he.h_name && he.h_name[0])
      h->setKeyValue("name", new QoreStringNode(he.h_name), 0); // official host name
   if (he.h_aliases) {
      QoreListNode *l = new QoreListNode;
      char **a = he.h_aliases;
      while (*a)
	 l->push(new QoreStringNode(*(a++)));
      h->setKeyValue("aliases", l, 0);
   }
   switch (he.h_addrtype) {
      case AF_INET:
	 h->setKeyValue("typename", new QoreStringNode("ipv4"), 0);
	 h->setKeyValue("type", new QoreBigIntNode(AF_INET), 0);
	 break;

      case AF_INET6:
	 h->setKeyValue("typename", new QoreStringNode("ipv6"), 0);
	 h->setKeyValue("type", new QoreBigIntNode(AF_INET6), 0);
	 break;

      default:
	 h->setKeyValue("typename", new QoreStringNode("unknown"), 0);
	 // no break
   }
   h->setKeyValue("len", new QoreBigIntNode(he.h_length), 0);

   if (he.h_addr_list) {
      char buf[QORE_NET_ADDR_BUF_LEN];

      QoreListNode *l = new QoreListNode();
      char **a = he.h_addr_list;
      while (*a) {
	 if (inet_ntop(he.h_addrtype, *(a++), buf, QORE_NET_ADDR_BUF_LEN))
	    l->push(new QoreStringNode(buf));
      }
      h->setKeyValue("addresses", l, 0);
   }

   return h;
}

static QoreStringNode *hename_string(struct hostent &he) {
   if (he.h_name && he.h_name[0])
      return new QoreStringNode(he.h_name);

   return new QoreStringNode;
}

static QoreStringNode *headdr_string(struct hostent &he) {
   if (he.h_addr_list && he.h_addr_list[0]) {
      char buf[QORE_NET_ADDR_BUF_LEN];
      if (inet_ntop(he.h_addrtype, he.h_addr_list[0], buf, QORE_NET_ADDR_BUF_LEN))
	 return new QoreStringNode(buf);
   }

   return new QoreStringNode;
}

QoreHashNode *q_gethostbyname_to_hash(const char *host) {  
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[NET_BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   int rc = gethostbyname_r(host, &he, buf, NET_BUFSIZE, &p, &err);

   if (!p || rc) {
      // NOTE: ERANGE means that the buffer was too small
      //printd(5, "gethostbyname_r() host=%s (bs=%d) error=%d: %d: %s\n", host, NET_BUFSIZE, err, errno, strerror(errno));
      return 0;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, NET_BUFSIZE, &err)) {
      printd(5, "q_gethostbyname() Solaris gethostbyname_r() returned NULL");
      return 0;
   }
# endif // HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   return he_to_hash(he);
#else  // else if !HAVE_GETHOSTBYNAME_R
   struct hostent *he;
   AutoLocker al(&lck_gethostbyname);
   if (!(he = gethostbyname(host))) {
      //herror("q_gethostbyname()");
      lck_gethostbyname.unlock();
      return 0;
   }
   return he_to_hash(*he);
#endif
}

QoreStringNode *q_gethostbyname_to_string(const char *host) {  
#ifdef HAVE_GETHOSTBYNAME_R
   struct hostent he;
   int err;
   char buf[NET_BUFSIZE];
# ifdef HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   struct hostent *p;
   
   int rc = gethostbyname_r(host, &he, buf, NET_BUFSIZE, &p, &err);
   if (!p || rc) {
      // NOTE: ERANGE means that the buffer was too small
      //printd(5, "gethostbyname_r() host=%s (bs=%d) error=%d: %d: %s\n", host, NET_BUFSIZE, err, errno, strerror(errno));
      return 0;
   }
# else // assume Solaris-style gethostbyname_r
   if (!gethostbyname_r(host, &he, buf, NET_BUFSIZE, &err)) {
      printd(5, "q_gethostbyname() Solaris gethostbyname_r() returned NULL");
      return 0;
   }
# endif // HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE
   return headdr_string(he);
#else  // else if !HAVE_GETHOSTBYNAME_R
   struct hostent *he;
   AutoLocker al(&lck_gethostbyname);
   if (!(he = gethostbyname(host))) {
      //herror("q_gethostbyname()");
      lck_gethostbyname.unlock();
      return 0;
   }
   return headdr_string(*he);
#endif
}

// thread-safe gethostbyaddr (string returned must be freed)
// FIXME: check err?
char *q_gethostbyaddr(const char *addr, int len, int type) {
   char *host;

   type = q_get_af(type);
    
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[NET_BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (gethostbyaddr_r(addr, len, type, &he, buf, NET_BUFSIZE, &err))
      host = strdup(he.h_name);
   else
      host = 0;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;

   int rc = gethostbyaddr_r(addr, len, type, &he, buf, NET_BUFSIZE, &p, &err);
   host = !rc && p ? strdup(he.h_name) : 0;
# endif // HAVE_SOLARIS_STYLE_GETHOST
#else  // else if !HAVE_GETHOSTBYADDR_R
   lck_gethostbyaddr.lock();
   struct hostent *he;
   if ((he = gethostbyaddr(addr, len, type)))
      host = strdup(he->h_name);
   else
      host = 0;
   lck_gethostbyaddr.unlock();
#endif // HAVE_GETHOSTBYADDR_R
   return host;
}

// thread-safe gethostbyaddr
// FIXME: check err?
QoreHashNode *q_gethostbyaddr_to_hash(ExceptionSink *xsink, const char *addr, int type) {
   in_addr sin_addr;
   in6_addr sin6_addr;
   void *dst;
   int len;

   type = q_get_af(type);

   if (type == AF_INET) {
      dst = (void *)&sin_addr;
      len = sizeof(sin_addr);
   }
   else if (type == AF_INET6) {
      dst = (void *)&sin6_addr;
      len = sizeof(sin6_addr);
   }
   else {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "%d is an invalid address type (valid types are AF_INET=%d, AF_INET6=%d", type, AF_INET, AF_INET6);
      return 0;
   }

   int rc = inet_pton(type, addr, dst);
   if (rc == 0) {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "'%s' is not a valid address for %s addresses", addr, type == AF_INET ? "AF_INET (IPv4)" : "AF_INET6 (IPv6)");
      return 0;
   }
   if (rc < 0)
      return 0;
   
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[NET_BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (!gethostbyaddr_r((char *)dst, len, type, &he, buf, NET_BUFSIZE, &err))
      return 0;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;
   
   rc = gethostbyaddr_r(dst, len, type, &he, buf, NET_BUFSIZE, &p, &err);
   if (rc || !p)
      return 0;
# endif // HAVE_SOLARIS_STYLE_GETHOST

   return he_to_hash(he);

#else  // else if !HAVE_GETHOSTBYADDR_R
   AutoLocker al(&lck_gethostbyaddr);
   struct hostent *he;
   if (!(he = gethostbyaddr((char *)dst, len, type)))
      return 0;

   return he_to_hash(*he);
#endif // HAVE_GETHOSTBYADDR_R
}

// thread-safe gethostbyaddr
// FIXME: check err?
QoreStringNode *q_gethostbyaddr_to_string(ExceptionSink *xsink, const char *addr, int type) {
   in_addr sin_addr;
   in6_addr sin6_addr;
   void *dst;
   int len;

   type = q_get_af(type);

   if (type == AF_INET) {
      dst = (void *)&sin_addr;
      len = sizeof(sin_addr);
   }
   else if (type == AF_INET6) {
      dst = (void *)&sin6_addr;
      len = sizeof(sin6_addr);
   }
   else {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "%d is an invalid address type (valid types are AF_INET=%d, AF_INET6=%d", type, AF_INET, AF_INET6);
      return 0;
   }

   int rc = inet_pton(type, addr, dst);
   if (rc == 0) {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "'%s' is not a valid address for %s addresses", addr, type == AF_INET ? "AF_INET (IPv4)" : "AF_INET6 (IPv6)");
      return 0;
   }
   if (rc < 0)
      return 0;
   
#ifdef HAVE_GETHOSTBYADDR_R
   struct hostent he;
   char buf[NET_BUFSIZE];
   int err;
# ifdef HAVE_SOLARIS_STYLE_GETHOST
   if (!gethostbyaddr_r((char *)dst, len, type, &he, buf, NET_BUFSIZE, &err))
      return 0;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent *p;
   
   rc = gethostbyaddr_r(dst, len, type, &he, buf, NET_BUFSIZE, &p, &err);
   if (rc || !p)
      return 0;
# endif // HAVE_SOLARIS_STYLE_GETHOST

   return hename_string(he);

#else  // else if !HAVE_GETHOSTBYADDR_R
   AutoLocker al(&lck_gethostbyaddr);
   struct hostent *he;
   if (!(he = gethostbyaddr((char *)dst, len, type)))
      return 0;

   return hename_string(*he);
#endif // HAVE_GETHOSTBYADDR_R
}

QoreListNode *q_getaddrinfo_to_list(ExceptionSink *xsink, const char *node, const char *service, int family, int flags, int socktype) {
   QoreAddrInfo ai;
   if (ai.getInfo(xsink, node, service, family, flags, socktype))
      return 0;

   return ai.getList();
}

QoreAddrInfo::QoreAddrInfo() : ai(0), has_svc(false) {
}

QoreAddrInfo::~QoreAddrInfo() {
   clear();
}

void QoreAddrInfo::clear() {
   if (ai) {
      freeaddrinfo(ai);
      ai = 0;
      has_svc = false;
   }
}

int QoreAddrInfo::getInfo(ExceptionSink *xsink, const char *node, const char *service, int family, int flags, int socktype, int protocol) {
   family = q_get_af(family);
   socktype = q_get_sock_type(socktype);

   if (ai)
      clear();

   struct addrinfo hints;
   memset(&hints, 0, sizeof hints); // make sure the struct is empty

   hints.ai_family = family;
   hints.ai_flags = flags;
   hints.ai_socktype = socktype;
   hints.ai_protocol = protocol;

   int status = getaddrinfo(node, service, &hints, &ai);
   if (status) {
      if (xsink)
	 xsink->raiseException("QOREADDRINFO-GETINFO-ERROR", "getaddrinfo(node: '%s', service: '%s', address_family: %d='%s', flags: %d) error: %s", node ? node : "", service ? service : "", family, q_af_to_str(family), flags, gai_strerror(status));
      return -1;
   }

   if (service)
      has_svc = true;
   return 0;   
}

QoreListNode *QoreAddrInfo::getList() const {
   if (!ai)
      return 0;

   QoreListNode *l = new QoreListNode;

   for (struct addrinfo *p = ai; p; p = p->ai_next) {
      QoreHashNode *h = new QoreHashNode;

      const char *family = q_af_to_str(p->ai_family);

      if (p->ai_canonname && *p->ai_canonname)
	 h->setKeyValue("canonname", new QoreStringNode(p->ai_canonname), 0);

      QoreStringNode *addr = q_addr_to_string2(p->ai_addr);
      if (addr) {
	 h->setKeyValue("address", addr, 0);
	 h->setKeyValue("address_desc", getAddressDesc(p->ai_family, addr->getBuffer()), 0);
      }

      h->setKeyValue("family", new QoreBigIntNode(p->ai_family), 0);
      h->setKeyValue("familystr", new QoreStringNode(family), 0);
      h->setKeyValue("addrlen", new QoreBigIntNode(p->ai_addrlen), 0);
      if (has_svc) {
	 int port = q_get_port_from_addr(p->ai_addr);
	 if (port != -1)
	    h->setKeyValue("port", new QoreBigIntNode(port), 0);
      }

      l->push(h);
   }

   return l;
}

const char *QoreAddrInfo::getFamilyName(int family) {
   return q_af_to_str(q_get_af(family));
}

QoreStringNode *QoreAddrInfo::getAddressDesc(int family, const char *addr) {
   family = q_get_af(family);

   QoreStringNode *str = new QoreStringNode;
   switch (family) {
      case AF_INET:
	 str->sprintf("ipv4(%s)", addr);
	 break;
      case AF_INET6:
	 str->sprintf("ipv6[%s]", addr);
	 break;
      default:
	 str->sprintf("%s:%s", getFamilyName(family), addr);
	 break;
   }
   return str;
}
