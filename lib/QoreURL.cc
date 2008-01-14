/*
 QoreURL.cc
  
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
#include <qore/QoreURL.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct qore_url_private {
      class QoreStringNode *protocol, *path, *username, *password, *host;
      int port;

      DLLLOCAL qore_url_private()
      {
	 zero();
      }

      DLLLOCAL ~qore_url_private()
      {
	 reset();
      }

      DLLLOCAL void zero()
      {
	 protocol = path = username = password = host = NULL;
	 port = 0;
      }
      
      DLLLOCAL void reset()
      {
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

      DLLLOCAL void parseIntern(const char *buf)
      {
	 if (!buf || !buf[0])
	    return;
   
	 printd(5, "QoreURL::parseIntern(%s)\n", buf);
   
	 char *p = (char *)strstr(buf, "://");
	 const char *pos;
   
	 // get protocol
	 if (p)
	 {
	    protocol = new QoreStringNode(buf, p - buf);
	    // convert to lower case
	    protocol->tolwr();
	    printd(5, "QoreURL::parseIntern protocol=%s\n", protocol->getBuffer());
	    pos = p + 3;
	 }
	 else
	    pos = buf;
   
	 char *nbuf;
   
	 // find end of hostname
	 if ((p = (char *)strchr(pos, '/')))
	 {
	    // get pathname if not at EOS
	    if (p[1] != '\0')
	    {
	       path = new QoreStringNode(p);
	       printd(5, "QoreURL::parseIntern path=%s\n", path->getBuffer());
	    }
	    // get copy of hostname string for localized searching and invasive parsing
	    nbuf = (char *)malloc(sizeof(char) * (p - pos + 1));
	    strncpy(nbuf, pos, p - pos);
	    nbuf[p - pos] = '\0';
	 }
	 else
	    nbuf = strdup(pos);
   
	 // see if there's a username
	 // note that nbuf here has already had the path removed so we can safely do a reverse search for the '@' sign
	 if ((p = strrchr(nbuf, '@')))
	 {
	    pos = p + 1;
	    *p = '\0';
	    // see if there's a password
	    if ((p = strchr(nbuf, ':')))
	    {
	       printd(5, "QoreURL::parseIntern password=%s\n", p + 1);
	       password = new QoreStringNode(p + 1);
	       *p = '\0';
	    }
	    // set username
	    printd(5, "QoreURL::parseIntern username=%s\n", nbuf);
	    username = new QoreStringNode(nbuf);
	 }
	 else
	    pos = nbuf;
   
	 // see if there's a port
	 if ((p = (char *)strchr(pos, ':')))
	 {
	    *p = '\0';
	    port = atoi(p + 1);
	    printd(5, "QoreURL::parseIntern port=%d\n", port);
	 }
	 // set hostname
	 printd(5, "QoreURL::parseIntern host=%s\n", pos);
	 host = new QoreStringNode(pos);
	 free(nbuf);
      }
};

QoreURL::QoreURL() : priv(new qore_url_private)
{
}

QoreURL::QoreURL(const char *url) : priv(new qore_url_private)
{
   parse(url);
}

QoreURL::QoreURL(const QoreString *url) : priv(new qore_url_private)
{
   parse(url->getBuffer());
}

QoreURL::~QoreURL()
{
   delete priv;
}

int QoreURL::parse(const char *url)
{
   priv->reset();
   priv->zero();
   priv->parseIntern(url);
   return isValid() ? 0 : -1;
}

int QoreURL::parse(const QoreString *url)
{
   priv->reset();
   priv->zero();
   priv->parseIntern(url->getBuffer());
   return isValid() ? 0 : -1;
}

bool QoreURL::isValid() const
{
   return priv->host && priv->host->strlen();
}

const QoreString *QoreURL::getProtocol() const
{
   return priv->protocol;
}

const QoreString *QoreURL::getUserName() const
{
   return priv->username;
}

const QoreString *QoreURL::getPassword() const
{
   return priv->password;
}

const QoreString *QoreURL::getPath() const
{
   return priv->path;
}

const QoreString *QoreURL::getHost() const
{
   return priv->host;
}

int QoreURL::getPort() const
{
   return priv->port;
}

class QoreHash *QoreURL::getHash()
{
   class QoreHash *h = new QoreHash();
   if (priv->protocol)
   {
      h->setKeyValue("protocol", priv->protocol, NULL);
      priv->protocol = NULL;
   }
   if (priv->path)
   {
      h->setKeyValue("path", priv->path, NULL);
      priv->path = NULL;
   }
   if (priv->username)
   {
      h->setKeyValue("username", priv->username, NULL);
      priv->username = NULL;
   }
   if (priv->password)
   {
      h->setKeyValue("password", priv->password, NULL);
      priv->password = NULL;
   }
   if (priv->host)
   {
      h->setKeyValue("host", priv->host, NULL);
      priv->host = NULL;
   }
   if (priv->port)
      h->setKeyValue("port", new QoreNode((int64)priv->port), NULL);
   
   return h;
}

char *QoreURL::take_path()
{
   return priv->path ? priv->path->giveBuffer() : NULL;
}

char *QoreURL::take_username()
{
   return priv->username ? priv->username->giveBuffer() : NULL;
}

char *QoreURL::take_password()
{
   return priv->password ? priv->password->giveBuffer() : NULL;
}

char *QoreURL::take_host()
{
   return priv->host ? priv->host->giveBuffer() : NULL;
}
