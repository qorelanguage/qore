/* -*- indent-tabs-mode: nil -*- */
/*
  QoreURL.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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
#include <qore/QoreURL.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <string>

struct qore_url_private {
private:
   DLLLOCAL void invalidate() {
      if (host) {
         host->deref();
         host = 0;
      }
      if (path) {
         path->deref();
         path = 0;
      }
   }

   DLLLOCAL void parse_intern(const char* buf, bool keep_brackets, ExceptionSink* xsink) {
      if (!buf || !buf[0])
         return;

      printd(5, "QoreURL::parse_intern(%s)\n", buf);

      // buf is continuously shrinked depending on the part of the string
      // that remains to be processed
      std::string sbuf(buf);

      // look for the protocol, move 'pos' after the protocol specification
      size_t protocol_separator = sbuf.find("://");
      if (protocol_separator != std::string::npos) {
         protocol = new QoreStringNode(sbuf.c_str(), protocol_separator);
         // convert to lower case
         protocol->tolwr();
         printd(5, "QoreURL::parse_intern protocol=%s\n", protocol->getBuffer());
         sbuf = sbuf.substr(protocol_separator + 3);
      }

      // see if the rest of the URL is a windows path
      if (sbuf.size() >= 2 &&
          ((isalpha(sbuf[0]) && sbuf[1] == ':')
           || (sbuf[0] == '\\' && sbuf[1] == '\\'))
          && sbuf.find('@') == std::string::npos) {
         path = new QoreStringNode(sbuf.c_str());
         return;
      }

      // find end of hostname
      size_t first_slash = sbuf.find('/');
      if (first_slash != std::string::npos) {
          // get pathname if not at EOS
          path = new QoreStringNode(sbuf.c_str() + first_slash);
          printd(5, "QoreURL::parse_intern path: '%s'\n", path->getBuffer());
          // get copy of hostname string for localized searching and invasive parsing
          sbuf = sbuf.substr(0, first_slash);
          //printd(5, "QoreURL::nbuf: '%s' size: %d\n", nbuf.c_str(), nbuf.size());
      }

      // see if there's a username
      // note that nbuf here has already had the path removed so we can safely do a reverse search for the '@' sign
      size_t username_end = sbuf.rfind('@');
      if (username_end != std::string::npos) {
          // see if there's a password
          size_t pw_start = sbuf.find(':');
          if (pw_start < username_end && pw_start != std::string::npos) {
              printd(5, "QoreURL::parse_intern password: '%s'\n", sbuf.c_str() + pw_start + 1);
              password = new QoreStringNode(sbuf.c_str() + pw_start + 1, username_end - (pw_start + 1));
              // set username
              username = new QoreStringNode(sbuf.c_str(), pw_start);
          }
          else {
              username = new QoreStringNode(sbuf.c_str(), username_end);
          }
          sbuf = sbuf.substr(username_end + 1);
      }
      // else no username, keep processing sbuf

      // see if the "hostname" is enclosed in square brackets, denoting an ipv6 address
      if (!sbuf.empty() && sbuf[0] == '[') {
         size_t right_bracket = sbuf.find(']');
         if (right_bracket != std::string::npos) {
             host = new QoreStringNode(sbuf.c_str() + (keep_brackets ? 0 : 1),
                                       right_bracket - (keep_brackets ? -1 : 1));
             sbuf = sbuf.substr(right_bracket + 1);
         }
      }

      bool has_port = false;
      // see if there's a port
      size_t port_start = sbuf.rfind(':');
      if (port_start != std::string::npos) {
         // see if it's IPv6 localhost (::)
         if (port_start != 1 || sbuf[0] != ':') {
            // find the end of port data
            if (port_start + 1 == sbuf.size()) {
                if (xsink)
                    xsink->raiseException("PARSE-URL-ERROR", "URL '%s' has an invalid empty port specification", buf);
                invalidate();
                return;
            }
            for (size_t i = port_start + 1; i < sbuf.size(); ++i) {
               if (!isdigit(sbuf[i])) {
                  if (xsink)
                     xsink->raiseException("PARSE-URL-ERROR", "URL '%s' has an invalid non-numeric character in the port specification", buf);
                  invalidate();
                  return;
               }
            }

            port = atoi(sbuf.c_str() + port_start + 1);
            sbuf = sbuf.substr(0, port_start);
            has_port = true;
            printd(5, "QoreURL::parse_intern port=%d\n", port);
         }
      }

      // there is no hostname if there is no port specification and
      // no protocol, username, or password -- just a relative path
      if (!host) {
         if (!has_port && !protocol && !username && !password && path) {
            path->replace(0, 0, buf);
         }
         else if (!sbuf.empty()) {
            // set hostname
            printd(5, "QoreURL::parse_intern host=%s\n", sbuf.c_str());

            // see if the hostname is in the form "socket=xxxx" in which case we interpret as a UNIX domain socket
            if (!strncasecmp(sbuf.c_str(), "socket=", 7)) {
               host = new QoreStringNode();
               host->concatDecodeUrl(sbuf.c_str() + 7);
            }
            else
               host = new QoreStringNode(sbuf.c_str());
         }
      }
   }

public:
   QoreStringNode* protocol, *path, *username, *password, *host;
   int port;

   DLLLOCAL qore_url_private() {
      zero();
   }

   DLLLOCAL ~qore_url_private() {
      reset();
   }

   DLLLOCAL void zero() {
      protocol = path = username = password = host = 0;
      port = 0;
   }

   DLLLOCAL void reset() {
      if (protocol)
         protocol->deref();
      if (path)
         path->deref();
      if (username)
         username->deref();
      if (password)
         password->deref();
      if (host)
         host->deref();
   }

   DLLLOCAL int parse(const char* url, bool keep_brackets = false, ExceptionSink* xsink = 0) {
      reset();
      zero();
      parse_intern(url, keep_brackets, xsink);
      if (xsink && !*xsink && !isValid())
         xsink->raiseException("PARSE-URL-ERROR", "URL '%s' cannot be parsed", url);
      return isValid() ? 0 : -1;
   }

   DLLLOCAL bool isValid() const {
      return (host && host->strlen()) || (path && path->strlen());
   }

   // destructive
   DLLLOCAL QoreHashNode* getHash() {
      QoreHashNode* h = new QoreHashNode;
      if (protocol) {
         h->setKeyValue("protocol", protocol, 0);
         protocol = 0;
      }
      if (path) {
         h->setKeyValue("path", path, 0);
         path = 0;
      }
      if (username) {
         h->setKeyValue("username", username, 0);
         username = 0;
      }
      if (password) {
         h->setKeyValue("password", password, 0);
         password = 0;
      }
      if (host) {
         h->setKeyValue("host", host, 0);
         host = 0;
      }
      if (port)
         h->setKeyValue("port", new QoreBigIntNode(port), 0);

      return h;
   }
};

QoreURL::QoreURL() : priv(new qore_url_private) {
}

QoreURL::QoreURL(const char* url) : priv(new qore_url_private) {
   parse(url);
}

QoreURL::QoreURL(const QoreString* url) : priv(new qore_url_private) {
   parse(url->getBuffer());
}

QoreURL::QoreURL(const char* url, bool keep_brackets) : priv(new qore_url_private) {
   parse(url, keep_brackets);
}

QoreURL::QoreURL(const QoreString* url, bool keep_brackets) : priv(new qore_url_private) {
   parse(url->getBuffer(), keep_brackets);
}

QoreURL::QoreURL(const QoreString* url, bool keep_brackets, ExceptionSink* xsink) : priv(new qore_url_private) {
   parse(url, keep_brackets, xsink);
}

QoreURL::~QoreURL() {
   delete priv;
}

int QoreURL::parse(const char* url) {
   return priv->parse(url);
}

int QoreURL::parse(const QoreString* url) {
   return priv->parse(url->getBuffer());
}

int QoreURL::parse(const char* url, bool keep_brackets) {
   return priv->parse(url, keep_brackets);
}

int QoreURL::parse(const QoreString* url, bool keep_brackets) {
   return priv->parse(url->getBuffer(), keep_brackets);
}

int QoreURL::parse(const QoreString* url, bool keep_brackets, ExceptionSink* xsink) {
   TempEncodingHelper tmp(url, QCS_UTF8, xsink);
   if (*xsink)
      return -1;
   return priv->parse(tmp->c_str(), keep_brackets, xsink);
}

bool QoreURL::isValid() const {
   return (priv->host && priv->host->strlen()) || (priv->path && priv->path->strlen());
}

const QoreString* QoreURL::getProtocol() const {
   return priv->protocol;
}

const QoreString* QoreURL::getUserName() const {
   return priv->username;
}

const QoreString* QoreURL::getPassword() const {
   return priv->password;
}

const QoreString* QoreURL::getPath() const {
   return priv->path;
}

const QoreString* QoreURL::getHost() const {
   return priv->host;
}

int QoreURL::getPort() const {
   return priv->port;
}

// destructive
QoreHashNode* QoreURL::getHash() {
   return priv->getHash();
}

char* QoreURL::take_path() {
   return priv->path ? priv->path->giveBuffer() : 0;
}

char* QoreURL::take_username() {
   return priv->username ? priv->username->giveBuffer() : 0;
}

char* QoreURL::take_password() {
   return priv->password ? priv->password->giveBuffer() : 0;
}

char* QoreURL::take_host() {
   return priv->host ? priv->host->giveBuffer() : 0;
}
