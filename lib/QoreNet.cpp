/*
  QoreNet.cpp

  Network functions

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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

int q_addr_to_string(int family, const char* addr, QoreString& str) {
   family = q_get_af(family);

   char buf[QORE_NET_ADDR_BUF_LEN];
   if (!inet_ntop(family, addr, buf, QORE_NET_ADDR_BUF_LEN))
      return -1;
   str.concat(buf);
   return 0;
}

QoreStringNode* q_addr_to_string(int family, const char* addr) {
   family = q_get_af(family);

   char buf[QORE_NET_ADDR_BUF_LEN];
   return inet_ntop(family, addr, buf, QORE_NET_ADDR_BUF_LEN) ? new QoreStringNode(buf) : 0;
}

int q_addr_to_string2(const struct sockaddr* ai_addr, QoreString& str) {
   size_t slen = str.strlen();

   const void* addr;
   if (ai_addr->sa_family == AF_INET) {
      struct sockaddr_in* ipv4 = (struct sockaddr_in*)ai_addr;
      addr = &(ipv4->sin_addr);
      str.reserve(slen + INET_ADDRSTRLEN + 1);
   }
   else if (ai_addr->sa_family == AF_INET6) {
      struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ai_addr;
      addr = &(ipv6->sin6_addr);
      str.reserve(slen + INET6_ADDRSTRLEN + 1);
   }
#ifdef HAVE_SYS_UN_H
   // windows does not support UNIX sockets, for example
   else if (ai_addr->sa_family == AF_UNIX) {
      struct sockaddr_un* un = (struct sockaddr_un*)ai_addr;
      str.concat(un->sun_path);
      return 0;
   }
#endif
   else
      return -1;

   if (!inet_ntop(ai_addr->sa_family, addr, (char*)(str.getBuffer() + slen), str.capacity() - slen))
      return -1;

   str.terminate(slen + strlen(str.getBuffer() + slen));
   return 0;
}


QoreStringNode* q_addr_to_string2(const struct sockaddr* ai_addr) {
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode);

   return q_addr_to_string2(ai_addr, **str) ? 0 : str.release();
}

int q_get_port_from_addr(const struct sockaddr* ai_addr) {
   if (ai_addr->sa_family == AF_INET) {
      const struct sockaddr_in* ipv4 = (struct sockaddr_in*)ai_addr;
      return ntohs(ipv4->sin_port);
   }
   else if (ai_addr->sa_family == AF_INET6) {
      const struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ai_addr;
      return ntohs(ipv6->sin6_port);
   }

   return -1;
}

//! Get host's IP address (struct in_addr*) from name.
int q_gethostbyname(const char* host, struct in_addr* sin_addr) {
   QORE_TRACE("q_gethostbyname()");

   ExceptionSink xsink;
   QoreAddrInfo qai;
   qai.getInfo(&xsink, host, NULL);
   if (xsink) {
      xsink.clear();
      return -1;
   }

   struct addrinfo* addrInfo = qai.getAddrInfo();
   while (addrInfo) {
      if (addrInfo->ai_addr->sa_family == AF_INET) {
         memcpy(sin_addr, &(reinterpret_cast<struct sockaddr_in*>(addrInfo->ai_addr)->sin_addr), sizeof(struct in_addr));
         return 0;
      }
      addrInfo = addrInfo->ai_next;
   }
   return -1;
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

static QoreHashNode* he_to_hash(struct hostent& he) {
   QoreHashNode* h = new QoreHashNode;

   if (he.h_name && he.h_name[0])
      h->setKeyValue("name", new QoreStringNode(he.h_name), 0); // official host name
   if (he.h_aliases) {
      QoreListNode* l = new QoreListNode;
      char** a = he.h_aliases;
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

      QoreListNode* l = new QoreListNode();
      char** a = he.h_addr_list;
      while (*a) {
	 if (inet_ntop(he.h_addrtype, *(a++), buf, QORE_NET_ADDR_BUF_LEN))
	    l->push(new QoreStringNode(buf));
      }
      h->setKeyValue("addresses", l, 0);
   }

   return h;
}

//! Get host's IP addresses and info from name.
QoreHashNode* q_gethostbyname_to_hash(const char* host) {
   ExceptionSink xsink;
   QoreAddrInfo qai;
   qai.getInfo(&xsink, host, NULL, Q_AF_UNSPEC, AI_CANONNAME);
   ReferenceHolder<QoreHashNode> result(new QoreHashNode, &xsink);
   ReferenceHolder<QoreListNode> addresses(new QoreListNode, &xsink);
   if (xsink || !result || !addresses) {
      xsink.clear();
      return nullptr;
   }

   struct addrinfo* addrInfo = qai.getAddrInfo();
   result->setKeyValue("name", new QoreStringNode(addrInfo->ai_canonname), &xsink);
   result->setKeyValue("aliases", new QoreListNode, &xsink);
   int ai_family = addrInfo->ai_family;
   switch (ai_family) {
      case AF_INET:
         result->setKeyValue("typename", new QoreStringNode("ipv4"), &xsink);
         result->setKeyValue("type", new QoreBigIntNode(AF_INET), &xsink);
         result->setKeyValue("len", new QoreBigIntNode(4), &xsink);
         break;
      case AF_INET6:
         result->setKeyValue("typename", new QoreStringNode("ipv6"), &xsink);
         result->setKeyValue("type", new QoreBigIntNode(AF_INET6), &xsink);
         result->setKeyValue("len", new QoreBigIntNode(16), &xsink);
         break;
      default:
         result->setKeyValue("typename", new QoreStringNode("unknown"), &xsink);
         break;
   }

   while (addrInfo && !xsink) {
      if (ai_family == addrInfo->ai_family) {
         SimpleRefHolder<QoreStringNode> addr(new QoreStringNode);
         if (q_addr_to_string2(addrInfo->ai_addr, **addr))
            return nullptr;
         addresses->push(addr.release());
      }
      addrInfo = addrInfo->ai_next;
   }
   result->setKeyValue("addresses", addresses.release(), &xsink);
   if (xsink) {
      xsink.clear();
      return nullptr;
   }

   return result.release();
}

//! Get host's IP address from name.
QoreStringNode* q_gethostbyname_to_string(const char* host) {
   ExceptionSink xsink;
   QoreAddrInfo qai;
   qai.getInfo(&xsink, host, NULL);
   if (xsink) {
      xsink.clear();
      return nullptr;
   }

   SimpleRefHolder<QoreStringNode> addr(new QoreStringNode);
   if (q_addr_to_string2(qai.getAddrInfo()->ai_addr, **addr))
      return nullptr;

   return addr.release();
}

//! Get host's name from IP address.
char* q_gethostbyaddr(const char* addr, int len, int type) {
   sockaddr_in in;
   sockaddr_in6 in6;
   char buf[512];
   int rc;

   type = q_get_af(type);
   if (type == AF_INET) {
      memset(&in, 0, sizeof(sockaddr_in));
      in.sin_family = AF_INET;
      in.sin_addr = *((struct in_addr*)addr);
      rc = getnameinfo(reinterpret_cast<sockaddr*>(&in), sizeof(sockaddr_in), buf, sizeof buf, nullptr, 0, NI_NAMEREQD);
   }
   else if (type == AF_INET6) {
      memset(&in6, 0, sizeof(sockaddr_in6));
      in6.sin6_family = AF_INET6;
      in6.sin6_addr = *((struct in6_addr*)addr);
      rc = getnameinfo(reinterpret_cast<sockaddr*>(&in6), sizeof(sockaddr_in6), buf, sizeof buf, nullptr, 0, NI_NAMEREQD);
   }
   else {
      return nullptr;
   }

   return rc ? nullptr : strdup(buf);
}

// thread-safe gethostbyaddr
// FIXME: check err?
QoreHashNode* q_gethostbyaddr_to_hash(ExceptionSink* xsink, const char* addr, int type) {
   in_addr sin_addr;
   in6_addr sin6_addr;
   void* dst;
   int len;

   type = q_get_af(type);

   if (type == AF_INET) {
      dst = (void*)&sin_addr;
      len = sizeof(sin_addr);
   }
   else if (type == AF_INET6) {
      dst = (void*)&sin6_addr;
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
   if (!gethostbyaddr_r((char*)dst, len, type, &he, buf, NET_BUFSIZE, &err))
      return 0;
# else // assume glibc2-style gethostbyaddr_r
   struct hostent* p;

   rc = gethostbyaddr_r(dst, len, type, &he, buf, NET_BUFSIZE, &p, &err);
   if (rc || !p)
      return 0;
# endif // HAVE_SOLARIS_STYLE_GETHOST

   return he_to_hash(he);

#else  // else if !HAVE_GETHOSTBYADDR_R
   AutoLocker al(&lck_gethostbyaddr);
   struct hostent* he;
   if (!(he = gethostbyaddr((char*)dst, len, type)))
      return 0;

   return he_to_hash(*he);
#endif // HAVE_GETHOSTBYADDR_R
}

//! Get host's name from IP address.
QoreStringNode* q_gethostbyaddr_to_string(ExceptionSink* xsink, const char* addr, int type) {
   sockaddr_in in;
   sockaddr_in6 in6;
   char buf[512];
   void* dst;

   type = q_get_af(type);
   if (type == AF_INET) {
      memset(&in, 0, sizeof(sockaddr_in));
      in.sin_family = AF_INET;
      dst = (void*)&in.sin_addr;
   }
   else if (type == AF_INET6) {
      memset(&in6, 0, sizeof(sockaddr_in6));
      in6.sin6_family = AF_INET6;
      dst = (void*)&in6.sin6_addr;
   }
   else {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "%d is an invalid address type (valid types are AF_INET=%d, AF_INET6=%d", type, AF_INET, AF_INET6);
      return nullptr;
   }

   int rc = inet_pton(type, addr, dst);
   if (rc == 0) {
      xsink->raiseException("GETHOSTBYADDR-ERROR", "'%s' is not a valid address for %s addresses", addr, type == AF_INET ? "AF_INET (IPv4)" : "AF_INET6 (IPv6)");
      return nullptr;
   }
   if (rc < 0)
      return nullptr;

   if (type == AF_INET) {
      rc = getnameinfo(reinterpret_cast<sockaddr*>(&in), sizeof(sockaddr_in), buf, sizeof buf, nullptr, 0, NI_NAMEREQD);
   }
   else {
      rc = getnameinfo(reinterpret_cast<sockaddr*>(&in6), sizeof(sockaddr_in6), buf, sizeof buf, nullptr, 0, NI_NAMEREQD);
   }

   if (rc)
      return nullptr;

   return new QoreStringNode(buf);
}

QoreListNode* q_getaddrinfo_to_list(ExceptionSink* xsink, const char* node, const char* service, int family, int flags, int socktype) {
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

int QoreAddrInfo::getInfo(ExceptionSink* xsink, const char* node, const char* service, int family, int flags, int socktype, int protocol) {
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

QoreListNode* QoreAddrInfo::getList() const {
   if (!ai)
      return 0;

   QoreListNode* l = new QoreListNode;

   for (struct addrinfo* p = ai; p; p = p->ai_next) {
      QoreHashNode* h = new QoreHashNode;

      const char* family = q_af_to_str(p->ai_family);

      if (p->ai_canonname && *p->ai_canonname)
	 h->setKeyValue("canonname", new QoreStringNode(p->ai_canonname), 0);

      QoreStringNode* addr = q_addr_to_string2(p->ai_addr);
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

const char* QoreAddrInfo::getFamilyName(int family) {
   return q_af_to_str(q_get_af(family));
}

QoreStringNode* QoreAddrInfo::getAddressDesc(int family, const char* addr) {
   family = q_get_af(family);

   QoreStringNode* str = new QoreStringNode;
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
