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

#include <string.h>
#include <ctype.h>

class Hash *parseURL(class QoreString *url)
{
   if (!url || !url->strlen())
      return NULL;

   printd(5, "parseURL(%s)\n", url->getBuffer());

   Hash *h = new Hash();
   char *buf = url->getBuffer();
   char *p = strstr(buf, "://");
   char *pos;

   // get protocol
   if (p)
   {
      QoreString *prot = url->substr(0, p - buf);
      // convert to lower case
      char *c = prot->getBuffer();
      while (*c)
      {
	 *c = tolower(*c);
	 c++;
      }
      printd(5, "parseURL() protocol=%s\n", prot->getBuffer());
      h->setKeyValue("protocol", new QoreNode(prot), NULL);
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
	 class QoreString *path = new QoreString(p + 1);
	 printd(5, "parseURL() path=%s\n", path->getBuffer());
	 h->setKeyValue("path", new QoreNode(path), NULL);
      }
      // get copy of hostname string for localized searching and invasive parsing
      nbuf = (char *)malloc(sizeof(char) * (p - pos + 1));
      strncpy(nbuf, pos, p - pos);
      nbuf[p - pos] = '\0';
   }
   else
      nbuf = strdup(pos);

   // see if there's a username
   if ((p = strchr(nbuf, '@')))
   {
      pos = p + 1;
      *p = '\0';
      // see if there's a password
      if ((p = strchr(nbuf, ':')))
      {
	 printd(5, "parseURL() password=%s\n", p + 1);
	 h->setKeyValue("password", new QoreNode(p + 1), NULL);
	 *p = '\0';
      }
      // set username
      printd(5, "parseURL() username=%s\n", nbuf);
      h->setKeyValue("username", new QoreNode(nbuf), NULL);
   }
   else
      pos = nbuf;

   // see if there's a port
   if ((p = strchr(pos, ':')))
   {
      *p = '\0';
      int port = atoi(p + 1);
      printd(5, "parseURL() port=%d\n", port);
      h->setKeyValue("port", new QoreNode(NT_INT, port), NULL);
   }
   // set hostname
   printd(5, "parseURL() host=%s\n", pos);
   h->setKeyValue("host", new QoreNode(pos), NULL);
   free(nbuf);

   return h;
}

