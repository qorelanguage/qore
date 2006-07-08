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
   if ((p = strchr(nbuf, '@')))
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
