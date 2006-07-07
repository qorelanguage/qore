/*
  QoreFile.cc

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
#include <qore/Qore.h>
#include <qore/QoreFile.h>
#include <qore/support.h>

char *QoreFile::readBlock(int &size)
{
   int bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
   int br = 0;
   char *buf = (char *)malloc(sizeof(char) * bs);
   char *bbuf = NULL;

   while (true)
   {
      int rc = ::read(fd, buf, bs);
      if (rc <= 0)
	 break;
      // enlarge bbuf (ensure buffer is 1 byte bigger than needed)
      bbuf = (char *)realloc(bbuf, br + rc + 1);
      // append buffer to bbuf
      memcpy(bbuf + br, buf, rc);
      br += rc;

      if (size > 0)
      {
	 if (size - br < bs)
	    bs = size - br;
	 if (br >= size)
	    break;
      }
   }
   free(buf);
   if (!br)
   {
      if (bbuf)
	 free(bbuf);
      return NULL;
   }
   size = br;
   return bbuf;
}
