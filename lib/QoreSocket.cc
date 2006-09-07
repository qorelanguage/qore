/*
  QoreSocket.cc

  IPv4 Socket Class
  
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
#include <qore/QoreSocket.h>
#include <qore/QoreLib.h>

#include <ctype.h>
#include <stdlib.h>
#include <strings.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

// QoreSocket::accept()
// returns a new socket
int QoreSocket::acceptInternal(class SocketSource *source)
{
   if (!sock)
      return -2;

   int rc;
   if (type == AF_UNIX)
   {
      struct sockaddr_un addr_un;

      socklen_t size = sizeof(struct sockaddr_un);
      rc = ::accept(sock, (struct sockaddr *)&addr_un, &size);
      //printd(1, "QoreSocket::accept() %d bytes returned\n", size);
      
      if (rc > 0 && source)
      {
	 class QoreString *addr = new QoreString(charsetid);
	 addr->sprintf("UNIX socket: %s", socketname);
	 source->setAddress(addr);
	 source->setHostName("localhost");
      }
   }
   else if (type == AF_INET)
   {
      struct sockaddr_in addr_in;
      unsigned size = sizeof(struct sockaddr_in);

      rc = ::accept(sock, (struct sockaddr *)&addr_in, (socklen_t *)&size);
      //printd(1, "QoreSocket::accept() %d bytes returned\n", size);

      if (rc > 0 && source)
      {
	 char *host;
	 if ((host = q_gethostbyaddr((const char *)&addr_in.sin_addr.s_addr, sizeof(addr_in.sin_addr.s_addr), AF_INET)))
	 {
	    class QoreString *hostname = new QoreString(charsetid);
	    hostname->take(host);
	    source->setHostName(hostname);
	 }

	 // get IP address
	 char ifname[80];
	 if (inet_ntop(AF_INET, &addr_in.sin_addr, ifname, sizeof(ifname)))
	    source->setAddress(ifname);
      }
   }
   else
      rc = -1;
   return rc;
}

// hardcoded to SOCK_STREAM (tcp only)
int QoreSocket::connectINET(char *host, int prt, class ExceptionSink *xsink)
{
   tracein("QoreSocket::connectINET()");

   // close socket if already open
   close();

   printd(5, "QoreSocket::connectINET(%s:%d)\n", host, prt);

   struct sockaddr_in addr_p;

   addr_p.sin_family = AF_INET;
   addr_p.sin_port = htons(prt);

   if (q_gethostbyname(host, &addr_p.sin_addr))
   {
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", "cannot resolve hostname '%s'", host);
      traceout("QoreSocket::connectINET()");
      return -1;
   }

   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("QoreSocket::connectINET()");
      return -1;
   }

   if ((::connect(sock, (const sockaddr *)&addr_p, sizeof(struct sockaddr_in))) == -1)
   {
      ::close(sock);
      sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("QoreSocket::connectINET()");
      return -1;
   }
   type = AF_INET;
   port = prt;
   printd(5, "QoreSocket::connectINET(this=%08p, host='%s', port=%d) success, sock=%d\n", this, host, port, sock);
   traceout("QoreSocket::connectINET()");
   return 0;
}

int QoreSocket::connectUNIX(char *p, class ExceptionSink *xsink)
{
   tracein("connectUNIX()");

   // close socket if already open
   close();

   printd(5, "QoreSocket::connectUNIX(%s)\n", p);

   struct sockaddr_un addr;

   addr.sun_family = AF_UNIX;
   // copy path and terminate if necessary
   strncpy(addr.sun_path, p, UNIX_PATH_MAX - 1);
   addr.sun_path[UNIX_PATH_MAX - 1] = '\0';
   if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("connectUNIX()");
      return -1;
   }
   if ((::connect(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un))) == -1)
   {
      ::close(sock);
      sock = 0;
      if (xsink)
	 xsink->raiseException("SOCKET-CONNECT-ERROR", strerror(errno));
      traceout("connectUNIX()");
      return -1;
   }
   // save file name for deleting when socket is closed
   socketname = strdup(addr.sun_path);
   type = AF_UNIX;
   traceout("connectUNIX()");
   return 0;
}

// currently hardcoded to SOCK_STREAM (tcp-only)
// if there is no port specifier, opens UNIX domain socket (if necessary)
// and binds to a local UNIX socket file
// for UNIX domain sockets (AF_UNIX)
// * bind("filename");
// for INET sockets (AF_INET)
// * bind("interface:port");
int QoreSocket::bind(char *name, bool reuseaddr)
{
   //printd(5, "QoreSocket::bind(%s)\n", name);
   // see if there is a port specifier
   char *p = strchr(name, ':');
   if (p)
   {
      int prt = atoi(p + 1);
      //printd(5, "QoreSocket::bind() port=%d\n", prt);
      if (prt < 0)
	 return -1;
      class QoreString host(name);
      host.terminate(p - name);

      return bind(host.getBuffer(), prt, reuseaddr);
   }
   else // bind to UNIX domain socket file
   {
      // close if it's already been opened as an INET socket
      if (sock && type != AF_UNIX)
	 close();

      // try to open socket if necessary
      if (!sock && openUNIX())
	 return -1;

      // set SO_REUSEADDR option if necessary
      reuse(reuseaddr);

      struct sockaddr_un addr;
      addr.sun_family = AF_UNIX;
      // copy path and terminate if necessary
      strncpy(addr.sun_path, name, UNIX_PATH_MAX - 1);
      addr.sun_path[UNIX_PATH_MAX - 1] = '\0';
      if ((::bind(sock, (const sockaddr *)&addr, sizeof(struct sockaddr_un))) == -1)
      {
	 close();
	 return -1;
      }
      // save socket file name for deleting on close
      socketname = strdup(addr.sun_path);
      // delete UNIX domain socket on close
      del = true;
   }
   return 0;
}

int QoreSocket::sendi1(char i)
{
   if (!sock)
      return -1;

   int rc = send(&i, 1);

   if (!rc || rc < 0)
      return -1;

   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi2(short i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = htons(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 2 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 2)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi4(int i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = htonl(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 4 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 4)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi8(int64 i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = i8MSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 8 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 8)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi2LSB(short i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = i2LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 2 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 2)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);

   return 0;
}

int QoreSocket::sendi4LSB(int i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = i4LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 4 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 4)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

int QoreSocket::sendi8LSB(int64 i)
{
   if (!sock)
      return -1;

   // convert to network byte order
   i = i8LSB(i);
   char *buf = (char *)&i;
   int bs = 0;
   while (1)
   {
      int rc = send(buf + bs, 8 - bs);
      if (!rc || rc < 0)
	 return -1;
      bs += rc;
      if (bs >= 8)
	 break;
   }
   //printd(5, "QoreSocket::send() sent %d byte(s)\n", bs);
   return 0;
}

// receive integer values and convert from network byte order
int QoreSocket::recvi1(int timeout, char *val)
{
   if (!sock)
      return -1;

   return recv(val, 1, 0, timeout);
}

int QoreSocket::recvi2(int timeout, short *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 2)
	 break;
   }

   *val = ntohs(*val);
   return 2;
}

int QoreSocket::recvi4(int timeout, int *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 4)
	 break;
   }

   *val = ntohl(*val);
   return 4;
}

int QoreSocket::recvi8(int timeout, int64 *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 8 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 8)
	 break;
   }

   *val = MSBi8(*val);
   return 8;
}

int QoreSocket::recvi2LSB(int timeout, short *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 2 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 2)
	 break;
   }

   *val = LSBi2(*val);
   return 2;
}

int QoreSocket::recvi4LSB(int timeout, int *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 4 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 4)
	 break;
   }

   *val = LSBi4(*val);
   return 4;
}

int QoreSocket::recvi8LSB(int timeout, int64 *val)
{
   if (!sock)
      return -1;

   char *buf = (char *)val;

   int br = 0;
   while (true)
   {
      int rc = recv(buf + br, 8 - br, 0, timeout);
      if (rc <= 0)
	 return rc;

      br += rc;

      if (br >= 8)
	 break;
   }

   *val = LSBi8(*val);
   return 4;
}

int QoreSocket::send(int fd, int size)
{
   if (!sock || !size)
   {
      printd(5, "QoreSocket::send() ERROR: sock=%d size=%d\n", sock, size);
      return -1;
   }

   char *buf = (char *)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);

   int rc = 0;
   int bs = 0;
   while (true)
   {
      // calculate bytes needed
      int bn;
      if (size < 0)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else
      {
	 bn = size - bs;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }
      rc = read(fd, buf, bn);
      if (!rc)
	 break;
      if (rc < 0)
      {
	 printd(5, "QoreSocket::send() read error: %s\n", strerror(errno));
	 break;
      }

      // send buffer
      rc = send(buf, rc);
      if (rc < 0)
      {
	 printd(5, "QoreSocket::send() send error: %s\n", strerror(errno));
	 break;
      }
      bs += rc;
      if (bs >= size)
      {
	 rc = 0;
	 break;
      }
   }
   free(buf);
   return rc;
}

class BinaryObject *QoreSocket::recvBinary(int bufsize, int timeout, int *rc)
{
   if (!sock)
      return NULL;

   int bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

   class BinaryObject *b = new BinaryObject();

   char *buf = (char *)malloc(sizeof(char) * bs);
   int br = 0; // bytes received
   while (true)
   {
      *rc = recv(buf, bs, 0, timeout);
      if ((*rc) <= 0)
      {
	 delete b;
	 b = NULL;
	 break;
      }
      b->append(buf, *rc);
      br += *rc;

      if (bufsize > 0)
      {
	 if (bufsize - br < bs)
	    bs = bufsize - br;
	 if (br >= bufsize)
	    break;
      }
   }
   free(buf);
   return b;
}

class QoreString *QoreSocket::recv(int bufsize, int timeout, int *rc)
{
   if (!sock)
   {
      *rc = -3;
      return NULL;
   }

   int bs = bufsize > 0 && bufsize < DEFAULT_SOCKET_BUFSIZE ? bufsize : DEFAULT_SOCKET_BUFSIZE;

   class QoreString *str = new QoreString(charsetid);

   char *buf = (char *)malloc(sizeof(char) * bs);

   int br = 0; // bytes received
   while (true)
   {
      *rc = recv(buf, bs, 0, timeout);
      if ((*rc) <= 0)
      {
	 //printd(0, "QoreSocket::recv(%d, %d) bs=%d, br=%d, rc=%d, errno=%d (%s)\n", bufsize, timeout, bs, br, *rc, errno, strerror(errno));

	 delete str;
	 str = NULL;
	 break;
      }
      str->concat(buf, *rc);
      br += *rc;

      if (bufsize > 0)
      {
	 if (br >= bufsize)
	    break;
	 if (bufsize - br < bs)
	    bs = bufsize - br;
      }
   }
   //printd(5, "QoreSocket::recv() received %d byte(s), strlen=%d\n", br, str->strlen());
   free(buf);
   return str;
}

class QoreString *QoreSocket::recv(int timeout, int *rc)
{
   if (!sock)
   {
      *rc = -3;
      return NULL;
   }

   char *buf = (char *)malloc(sizeof(char) * (DEFAULT_SOCKET_BUFSIZE + 1));
   *rc = recv(buf, DEFAULT_SOCKET_BUFSIZE, 0, timeout);
   if ((*rc) <= 0)
   {
      free(buf);
      return NULL;
   }

   buf[*rc] = '\0';
   QoreString *msg = new QoreString(charsetid);
   msg->take(buf);
   return msg;
}

// receive data and write to file descriptor
int QoreSocket::recv(int fd, int size, int timeout)
{
   if (!sock || !size)
      return -1;

   char *buf = (char *)malloc(sizeof(char) * DEFAULT_SOCKET_BUFSIZE);
   int br = 0;
   int rc;
   while (1)
   {
      // calculate bytes needed
      int bn;
      if (size == -1)
	 bn = DEFAULT_SOCKET_BUFSIZE;
      else
      {
	 bn = size - br;
	 if (bn > DEFAULT_SOCKET_BUFSIZE)
	    bn = DEFAULT_SOCKET_BUFSIZE;
      }

      rc = recv(buf, bn, 0, timeout);
      if (rc <= 0)
	 break;
      br += rc;

      // write buffer to file descriptor
      rc = write(fd, buf, rc);
      if (rc <= 0)
	 break;

      if (size > 0 && br >= size)
      {
	 rc = 0;
	 break;
      }
   }
   free(buf);
   return rc;
}

// returns 0 for success
int QoreSocket::sendHTTPMessage(char *method, char *path, char *http_version, class Hash *headers, void *data, int size)
{
   // prepare header string
   QoreString hdr(charsetid);

   hdr.sprintf("%s %s HTTP/%s\r\n", method, path, http_version);
   if (headers)
   {
      class HashIterator hi(headers);

      while (hi.next())
      {
	 class QoreNode *v = hi.getValue();
	 if (v)
	 {
	    if (v->type == NT_STRING)
	       hdr.sprintf("%s: %s\r\n", hi.getKey(), v->val.String->getBuffer());
	    else if (v->type == NT_INT)
	       hdr.sprintf("%s: %lld\r\n", hi.getKey(), v->val.intval);
	    else if (v->type == NT_FLOAT)
	       hdr.sprintf("%s: %f\r\n", hi.getKey(), v->val.floatval);
	 }
      }
   }
   // add data and content-length header if necessary
   if (size && data)
      hdr.sprintf("Content-Length: %d\r\n", size);

   hdr.concat("\r\n");
   int rc;
   if ((rc = send(hdr.getBuffer(), hdr.strlen())))
      return rc;

   if (size && data)
      return send((char *)data, size);

   return 0;
}

// returns 0 for success
int QoreSocket::sendHTTPResponse(int code, char *desc, char *http_version, class Hash *headers, void *data, int size)
{
   // prepare header string
   QoreString hdr(charsetid);

   hdr.sprintf("HTTP/%s %03d %s\r\n", http_version, code, desc);
   if (headers)
   {
      class HashIterator hi(headers);

      while (hi.next())
      {
	 class QoreNode *v = hi.getValue();
	 if (v)
	 {
	    if (v->type == NT_STRING)
	       hdr.sprintf("%s: %s\r\n", hi.getKey(), v->val.String->getBuffer());
	    else if (v->type == NT_INT)
	       hdr.sprintf("%s: %lld\r\n", hi.getKey(), v->val.intval);
	    else if (v->type == NT_FLOAT)
	       hdr.sprintf("%s: %f\r\n", hi.getKey(), v->val.floatval);
	 }
      }
   }
   // add data and content-length header if necessary
   if (size && data)
      hdr.sprintf("Content-Length: %d\r\n", size);

   hdr.concat("\r\n");
   int rc;
   if ((rc = send(hdr.getBuffer(), hdr.strlen())))
      return rc;

   if (size && data)
      return send((char *)data, size);

   return 0;
}

// FIXME: implement a maximum header size - otherwise a malicious message could fill up all memory
// rc is:
//    0 for remote end shutdown
//   -1 for socket error
//   -2 for socket not open
//   -3 for timeout
class QoreNode *QoreSocket::readHTTPHeader(int timeout, int *rc)
{
   if (!sock)
   {
      *rc = -2;
      return NULL;
   }

   // read in HHTP header until \r\n\r\n or \n\n from socket

   // state:
   //   0 = '\r' received
   //   1 = '\r\n' received
   //   2 = '\r\n\r' received
   //   3 = '\n' received
   int state = -1;

   QoreString *hdr = new QoreString(charsetid);
   while (true)
   {
      char c;
      *rc = recv(&c, 1, 0, timeout); // = read(sock, &c, 1);
      //printd(0, "read char: %c (%03d) (old state: %d)\n", c > 30 ? c : '?', c, state);
      if ((*rc) <= 0)
      {
	 //printd(0, "QoreSocket::readHTTPHeader(timeout=%d) hdr->strlen()=%d, rc=%d, errno=%d (%s)\n", timeout, hdr->strlen(), *rc, errno, strerror(errno));

	 delete hdr;
	 return NULL;
      }
      // check if we can progress to the next state
      if (state == -1 && c == '\n')
      {
	 state = 3;
	 continue;
      }
      else if (state == -1 && c == '\r')
      {
	 state = 0;
	 continue;
      }
      else if (state > 0 && c == '\n')
	 break;
      if (!state && c == '\n')
      {
	 state = 1;
	 continue;
      }
      else if (state == 1 && c == '\r')
      {
	 state = 2;
	 continue;
      }
      else
      {
	 if (!state)
	    hdr->concat('\r');
	 else if (state == 1)
	    hdr->concat("\r\n");
	 else if (state == 2)
	    hdr->concat("\r\n\r");
	 else if (state == 3)
	    hdr->concat('\n');
	 state = -1;
	 hdr->concat(c);
      }
   }
   hdr->concat("\n\0");

   char *buf = hdr->getBuffer();
   //printd(0, "HTTP header=%s", buf);

   char *p;
   if ((p = strstr(buf, "\r\n")))
   {
     *p = '\0';
      p += 2;
   }
   else if ((p = strchr(buf, '\n')))
   {
     *p = '\0';
      p++;
   }
   else
   {
      //printd(5, "can't find first EOL marker\n");
      return new QoreNode(hdr);
   }
   char *t1;
   if (!(t1 = strstr(buf, "HTTP/1.")))
      return new QoreNode(hdr);

   Hash *h = new Hash();

#if 0
   h->setKeyValue("dbg_hdr", new QoreNode(buf), NULL);
#endif

   // get version
   h->setKeyValue("http_version", new QoreNode(new QoreString(t1 + 5, 3, charsetid)), NULL);

   // if we are getting a response
   if (t1 == buf)
   {
      char *t2 = strchr(buf + 8, ' ');
      if (t2)
      {
	 t2++;
	 if (isdigit(*(t2)))
	 {
	    h->setKeyValue("status_code", new QoreNode((int64)atoi(t2)), NULL);
	    if (strlen(t2) > 4)
	       h->setKeyValue("status_message", new QoreNode(t2 + 4), NULL);
	 }
      }
   }
   else // get method and path
   {
      char *t2 = strchr(buf, ' ');
      if (t2)
      {
	 *t2 = '\0';
	 h->setKeyValue("method", new QoreNode(buf), NULL);
	 t2++;
	 t1 = strchr(t2, ' ');
	 if (t1)
	 {
	    *t1 = '\0';
	    h->setKeyValue("path", new QoreNode(t2), NULL);
	 }
      }
   }
   while (*p)
   {
      buf = p;
      
      if ((p = strstr(buf, "\r\n")))
      {
	 *p = '\0';
	 p += 2;
      }
      else if ((p = strchr(buf, '\n')))
      {
	 *p = '\0';
	 p++;
      }
      else
	 break;
      t1 = strchr(buf, ':');
      if (!t1)
	 break;
      *t1 = '\0';
      t1++;
      while (t1 && isblank(*t1))
	 t1++;
      strtolower(buf);
      h->setKeyValue(buf, new QoreNode(t1), NULL);
   }
   delete hdr;
   return new QoreNode(h);
}

bool QoreSocket::isDataAvailable(int timeout)
{
   if (!sock)
      return false;

   fd_set sfs;

   struct timeval tv;
   tv.tv_sec  = timeout / 1000;
   tv.tv_usec = (timeout % 1000) * 1000;

   FD_ZERO(&sfs);
   FD_SET(sock, &sfs);
   return select(sock + 1, &sfs, NULL, NULL, &tv);

#if 0
   struct pollfd pfd;
   pfd.fd = sock;
   pfd.events = POLLIN|POLLPRI;
   pfd.revents = 0;

   return poll(&pfd, 1, timeout);
#endif
}
