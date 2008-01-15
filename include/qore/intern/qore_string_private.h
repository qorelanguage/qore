/*
  qore_string_private.h

  QoreString private implementation

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

#ifndef QORE_QORE_STRING_PRIVATE_H
#define QORE_QORE_STRING_PRIVATE_H 

#define MAX_INT_STRING_LEN    48
#define MAX_BIGINT_STRING_LEN 48
#define MAX_FLOAT_STRING_LEN  48
#define STR_CLASS_BLOCK 80
#define STR_CLASS_EXTRA 40

#define MIN_SPRINTF_BUFSIZE 120

struct qore_string_private {
      int len;
      unsigned allocated;
      char *buf;
      const QoreEncoding *charset;

      DLLLOCAL qore_string_private()
      {
      }
      DLLLOCAL qore_string_private(const qore_string_private &p)
      {
	 allocated = p.len + STR_CLASS_EXTRA;
	 buf = (char *)malloc(sizeof(char) * allocated);
	 len = p.len;
	 if (len)
	    memcpy(buf, p.buf, len);
	 buf[len] = '\0';
	 charset = p.charset;
      }
      DLLLOCAL ~qore_string_private()
      {
	 if (buf)
	    free(buf);
      }

      DLLLOCAL void check_char(unsigned i)
      {
	 if (i >= allocated)
	 {
	    int d = i >> 2;
	    allocated = i + (d < STR_CLASS_BLOCK ? STR_CLASS_BLOCK : d);
	    //allocated = i + STR_CLASS_BLOCK;
	    allocated = (allocated / 16 + 1) * 16; // use complete cache line
	    buf = (char *)realloc(buf, allocated * sizeof(char));
	 }
      }
      DLLLOCAL void check_offset(int &offset)
      {
	 if (offset < 0)
	 {
	    offset = len + offset;
	    if (offset < 0)
	       offset = 0;
	 }
	 else if (offset > len)
	    offset = len;
      }

      DLLLOCAL void check_offset(int &offset, int &num)
      {
	 check_offset(offset);
	 if (num < 0)
	 {
	    num = len + num - offset;
	    if (num < 0)
	       num = 0;
	 }
      }

};

#endif
