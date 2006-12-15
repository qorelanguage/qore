/*
 QoreURL.cc
  
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
#include <qore/common.h>
#include <qore/QoreURL.h>
#include <qore/QoreString.h>
#include <qore/Hash.h>
#include <qore/QoreNode.h>

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void QoreURL::zero()
{
   protocol = path = username = password = host = NULL;
   port = 0;
}

void QoreURL::reset()
{
   if (protocol)
      delete protocol;
   if (path)
      delete path;
   if (username)
      delete username;
   if (password)
      delete password;
   if (host)
      delete host;
}

void QoreURL::parseIntern(char *buf)
{
   if (!buf || !buf[0])
      return;
   
   printd(5, "QoreURL::parseIntern(%s)\n", buf);
   
   char *p = strstr(buf, "://");
   char *pos;
   
   // get protocol
   if (p)
   {
      protocol = new QoreString(buf, p - buf);
      // convert to lower case
      protocol->tolwr();
      printd(5, "QoreURL::parseIntern protocol=%s\n", protocol->getBuffer());
      pos = p + 3;
   }
   else
      pos = buf;
   
   char *nbuf;
   
   // find end of hostname
   if ((p = strchr(pos, '/')))
   {
      // get pathname if not at EOS
      if (p[1] != '\0')
      {
	 path = new QoreString(p + 1);
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
	 password = new QoreString(p + 1);
	 *p = '\0';
      }
      // set username
      printd(5, "QoreURL::parseIntern username=%s\n", nbuf);
      username = new QoreString(nbuf);
   }
   else
      pos = nbuf;
   
   // see if there's a port
   if ((p = strchr(pos, ':')))
   {
      *p = '\0';
      port = atoi(p + 1);
      printd(5, "QoreURL::parseIntern port=%d\n", port);
   }
   // set hostname
   printd(5, "QoreURL::parseIntern host=%s\n", pos);
   host = new QoreString(pos);
   free(nbuf);
}

QoreURL::QoreURL() 
{
   zero();
}

QoreURL::QoreURL(char *url)
{
   zero();
   parse(url);
}

QoreURL::QoreURL(class QoreString *url)
{
   zero();
   parse(url->getBuffer());
}

QoreURL::~QoreURL()
{
   reset();
}

int QoreURL::parse(char *url)
{
   reset();
   zero();
   parseIntern(url);
   return isValid() ? 0 : -1;
}

int QoreURL::parse(class QoreString *url)
{
   reset();
   zero();
   parseIntern(url->getBuffer());
   return isValid() ? 0 : -1;
}

bool QoreURL::isValid() const
{
   return host && host->strlen();
}

class QoreString *QoreURL::getProtocol() const
{
   return protocol;
}

int QoreURL::getPort() const
{
   return port;
}

class Hash *QoreURL::getHash()
{
   class Hash *h = new Hash();
   if (protocol)
   {
      h->setKeyValue("protocol", new QoreNode(protocol), NULL);
      protocol = NULL;
   }
   if (path)
   {
      h->setKeyValue("path", new QoreNode(path), NULL);
      path = NULL;
   }
   if (username)
   {
      h->setKeyValue("username", new QoreNode(username), NULL);
      username = NULL;
   }
   if (password)
   {
      h->setKeyValue("password", new QoreNode(password), NULL);
      password = NULL;
   }
   if (host)
   {
      h->setKeyValue("host", new QoreNode(host), NULL);
      host = NULL;
   }
   if (port)
      h->setKeyValue("port", new QoreNode((int64)port), NULL);
   
   return h;
}

char *QoreURL::take_path()
{
   return path ? path->giveBuffer() : NULL;
}

char *QoreURL::take_username()
{
   return username ? username->giveBuffer() : NULL;
}

char *QoreURL::take_password()
{
   return password ? password->giveBuffer() : NULL;
}

char *QoreURL::take_host()
{
   return host ? host->giveBuffer() : NULL;
}
