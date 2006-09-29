/*
  QoreString.cc

  QoreString Class Definition

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
#include <qore/QoreString.h>
#include <qore/BinaryObject.h>

#include <iconv.h>

// needed for platforms where the input buffer is defined as "const char"
template<typename T>
static inline size_t iconv_adapter (size_t (*iconv_f) (iconv_t, T, size_t *, char **, size_t *), iconv_t handle, char **inbuf, size_t *inavail, char **outbuf, size_t *outavail)
{
   return (*iconv_f) (handle, (T) inbuf, inavail, outbuf, outavail);
}

class QoreString *QoreString::convertEncoding(class QoreEncoding *nccs, ExceptionSink *xsink)
{
   if (nccs == charset || !len)
   {
      QoreString *str = copy();
      str->charset = nccs;
      return str;
   }

   printd(5, "QoreString::convertEncoding() from \"%s\" to \"%s\"\n", charset->code, nccs->code);

#ifdef NEED_ICONV_TRANSLIT
   QoreString to_code((char *)nccs->code);
   to_code.concat("//TRANSLIT");
   iconv_t c = iconv_open(to_code.getBuffer(), charset->code);
#else
   iconv_t c = iconv_open(nccs->code, charset->code);
#endif
   if (c == (iconv_t)-1)
   {
      if (errno == EINVAL)
	 xsink->raiseException("ENCODING-CONVERSION-ERROR", 
			       "cannot convert from \"%s\" to \"%s\"", 
			       charset->code, nccs->code);
      else
	 xsink->raiseException("ENCODING-CONVERSION-ERROR", 
			"unknown error converting from \"%s\" to \"%s\": %s", 
			charset->code, nccs->code, strerror(errno));
      return NULL;
   }
   
   // now convert value
   int al = len + STR_CLASS_BLOCK;
   char *nbuf = (char *)malloc(sizeof(char) * (al + 1));
   while (1)
   {
      size_t ilen = len;
      size_t olen = al;
      char *ib = buf;
      char *ob = nbuf;
      size_t rc = iconv_adapter(iconv, c, &ib, &ilen, &ob, &olen);
      if (rc == (size_t)-1)
	 switch (errno)
	 {
	    case EINVAL:
	    case EILSEQ:
	    {
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", 
				     "illegal character sequence found in input type \"%s\" (while converting to \"%s\")",
				     charset->code, nccs->code);
	       free(nbuf);
	       iconv_close(c);
	       return NULL;
	    }
	    case E2BIG:
	       al += STR_CLASS_BLOCK;
	       nbuf = (char *)realloc(nbuf, sizeof(char) * (al + 1));
	       break;
	    default:
	    {
	       xsink->raiseException("ENCODING-CONVERSION-ERROR", 
				     "error converting from \"%s\" to \"%s\": %s", 
				     charset->code, nccs->code,
				     strerror(errno));
	       free(nbuf);
	       iconv_close(c);
	       return NULL;
	    }
	 }
      else
      {
	 // terminate string
	 nbuf[al - olen] = '\0';
	 break;
      }
   }
   iconv_close(c);
   QoreString *str = new QoreString();
   str->take(nbuf, nccs);
   return str;
}

// endian-agnostic binary object -> base64 string function
// NOTE: not very high-performance - high-performance versions
//       would likely be endian-aware and operate directly on 32-bit words
void QoreString::concatBase64(char *buf, int size)
{
   //printf("buf=%08p, size=%d\n", buf, size);
   if (!size)
      return;

   unsigned char *p = (unsigned char *)buf;
   unsigned char *endbuf = p + size;
   while (p < endbuf)
   {
      // get 6 bits of data
      unsigned char c = p[0] >> 2;

      // byte 1: concat 1st 6-bit value
      concat(table64[c]);

      // byte 1: use remaining 2 bits in low order position
      c = (p[0] & 3) << 4;

      // check end
      if ((endbuf - p) == 1)
      {
	 concat(table64[c]);
	 concat("==");
	 break;
      }

      // byte 2: get 4 bits to make next 6-bit unit
      c |= p[1] >> 4;

      // concat 2nd 6-bit value
      concat(table64[c]);

      // byte 2: get 4 low bits
      c = (p[1] & 15) << 2;

      if ((endbuf - p) == 2)
      {
	 concat(table64[c]);
	 concat('=');
	 break;
      }

      // byte 3: get high 2 bits to make next 6-bit unit
      c |= p[2] >> 6;

      // concat 3rd 6-bit value
      concat(table64[c]);

      // byte 3: concat final 6 bits
      concat(table64[p[2] & 63]);
      p += 3;
   }
}

#define DO_HEX_CHAR(b) ((b) + (((b) > 9) ? 87 : 48))

void QoreString::concatHex(char *buf, int size)
{
   //printf("buf=%08p, size=%d\n", buf, size);
   if (!size)
      return;

   unsigned char *p = (unsigned char *)buf;
   unsigned char *endbuf = p + size;
   while (p < endbuf)
   {
     char c = (*p & 0xf0) >> 4;
     concat(DO_HEX_CHAR(c));
     c = *p & 0x0f;
     concat(DO_HEX_CHAR(c));
     p++;
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLEncode(QoreString *str, class ExceptionSink *xsink)
{
   // if it's not a null string
   if (str && str->len)
   {
      QoreString *cstr = str;
      if (charset != str->charset)
      {
	 cstr = str->convertEncoding(charset, xsink);
	 if (xsink->isEvent())
	    return;
      }

      for (int i = 0; i < cstr->len; i++)
      {
	 // concatenate translated character
	 int j;
	 for (j = 0; j < (int)NUM_HTML_CODES; j++)
	    if (cstr->buf[i] == html_codes[j].symbol)
	    {
	       concat(html_codes[j].code);
	       break;
	    }
	 // otherwise concatenate untranslated symbol
	 if (j == NUM_HTML_CODES)
	    concat(cstr->buf[i]);
      }

      if (cstr != str)
	 delete cstr;
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLEncode(char *str)
{
   // if it's not a null string
   if (str)
   {
      int i = 0;
      // iterate through new string
      while (str[i])
      {
	 // concatenate translated character
	 int j;
	 for (j = 0; j < (int)NUM_HTML_CODES; j++)
	    if (str[i] == html_codes[j].symbol)
	    {
	       concat(html_codes[j].code);
	       break;
	    }
	 // otherwise concatenate untranslated symbol
	 if (j == NUM_HTML_CODES)
	    concat(str[i]);
	 i++;
      }
      /*
      // see if buffer needs to be resized for '\0'
      check_char(len);
      // terminate string
      buf[len] = '\0';
      */
   }
}

// FIXME: this is slow, each concatenated character gets terminated as well
void QoreString::concatAndHTMLDecode(QoreString *str)
{
   // if it's not a null string
   if (str && str->len)
   {
      int i = 0;
      while (str->buf[i])
      {
	 // concatenate translated character
	 int j;
	 for (j = 0; j < (int)NUM_HTML_CODES; j++)
	    if ((str->len - i) >= html_codes[j].len
		&& !strncmp(html_codes[j].code, &str->buf[i], html_codes[j].len))
	    {
	       concat(html_codes[j].symbol);
	       i += html_codes[j].len;
	       continue;
	    }
	 // otherwise concatenate untranslated symbol
	 concat(str->buf[i++]);
      }
      /*
      // see if buffer needs to be resized for '\0'
      check_char(len);
      // terminate string
      buf[len] = '\0';
      */
   }
}

// return 0 for success
int QoreString::vsprintf(const char *fmt, va_list args)
{
   int fmtlen = ::strlen(fmt);
   // ensure minimum space is free
   if ((allocated - len - fmtlen) < MIN_SPRINTF_BUFSIZE)
   {
      allocated += fmtlen + MIN_SPRINTF_BUFSIZE;
      // resize buffer
      buf = (char *)realloc(buf, allocated * sizeof(char));
   }
   // set free buffer size
   int free = allocated - len;

   // copy formatted string to buffer
   int i = ::vsnprintf(buf + len, free, fmt, args);

   if (i >= free)
   {
      //printd(5, "vsnprintf() failed: i=%d allocated=%d len=%d buf=%08p fmtlen=%d (new=i+%d = %d)\n", i, allocated, len, buf, fmtlen, STR_CLASS_EXTRA, i + STR_CLASS_EXTRA);
      // resize buffer
      allocated = len + i + STR_CLASS_EXTRA;
      buf = (char *)realloc(buf, sizeof(char) * allocated);
      *(buf + len) = '\0';
      return -1;
   }

   len += i;
   return 0;
}

void QoreString::concat(char *str)
{
   // if it's not a null string
   if (str)
   {
      int i = 0;
      // iterate through new string
      while (str[i])
      {
	 // if buffer needs to be resized
	 check_char(len);
	 // concatenate one character at a time
	 buf[len++] = str[i++];
      }
      // see if buffer needs to be resized for '\0'
      check_char(len);
      // terminate string
      buf[len] = '\0';
   }
}

void QoreString::concat(char *str, int size)
{
   check_char(len + size);
   memcpy(buf + len, str, size);
   len += size;
   buf[len] = '\0';
}

void QoreString::concat(QoreString *str)
{
   // if it's not a null string
   if (str && str->len)
   {
      // if buffer needs to be resized
      check_char(str->len + len);
      // concatenate new string
      memcpy(buf + len, str->buf, str->len);
      len += str->len;
      buf[len] = '\0';
   }
}

/*
void QoreString::concat(QoreString *str, int size)
{
   // if it's not a null string
   if (str && str->len)
   {
      // if buffer needs to be resized
      check_char(str->len + size);
      // concatenate new string
      memcpy(buf + len, str->buf, size);
      len += size;
      buf[len] = '\0';
   }
}
*/

void QoreString::concat(QoreString *str, class ExceptionSink *xsink)
{
   // if it's not a null string
   if (str && str->len)
   {
      QoreString *cstr = str;
      if (charset != str->charset)
      {
	 cstr = str->convertEncoding(charset, xsink);
	 if (xsink->isEvent())
	    return;
      }

      // if buffer needs to be resized
      check_char(cstr->len + len);
      // concatenate new string
      memcpy(buf + len, cstr->buf, cstr->len);
      len += cstr->len;
      buf[len] = '\0';

      if (cstr != str)
	 delete cstr;
   }
}

void QoreString::concat(QoreString *str, int size, class ExceptionSink *xsink)
{
   // if it's not a null string
   if (str && str->len)
   {
      QoreString *cstr = str;
      if (charset != str->charset)
      {
	 cstr = str->convertEncoding(charset, xsink);
	 if (xsink->isEvent())
	    return;
      }

      // adjust size for number of characters if this is a multi-byte character set
      if (charset->isMultiByte())
	 size = charset->getByteLen(cstr->buf, size);

      // if buffer needs to be resized
      check_char(cstr->len + size);
      // concatenate new string
      memcpy(buf + len, cstr->buf, size);
      len += size;
      buf[len] = '\0';

      if (cstr != str)
	 delete cstr;
   }
}

void QoreString::concat(char c)
{
   if (allocated)
   {
      buf[len] = c;
      check_char(++len);
      buf[len] = '\0';
      return;
   }
   // allocate new string buffer
   allocated = STR_CLASS_BLOCK;
   len = 1;
   buf = (char *)malloc(sizeof(char) * allocated);
   buf[0] = c;
   buf[1] = '\0';
}

int QoreString::vsnprintf(int size, const char *fmt, va_list args)
{
   // ensure minimum space is free
   if ((allocated - len) < size)
   {
      allocated += (size + STR_CLASS_EXTRA);
      // resize buffer
      buf = (char *)realloc(buf, allocated * sizeof(char));
   }
   // copy formatted string to buffer
   int i = ::vsnprintf(buf + len, size, fmt, args);
   len += i;
   return i;
}

// returns 0 for success
int QoreString::sprintf(const char *fmt, ...)
{
   va_list args;
   while (true)
   {
      va_start(args, fmt);
      int rc = vsprintf(fmt, args);
      va_end(args);
      if (!rc)
	 break;
   }
   return 0;
}

int QoreString::snprintf(int size, const char *fmt, ...)
{
   va_list args;
   va_start(args, fmt);
   int i = vsnprintf(size, fmt, args);
   va_end(args);
   return i;
}

class QoreString *QoreString::substr_simple(int offset, int length)
{
   tracein("QoreString::substr_simple(offset, length)");
   printd(5, "QoreString::substr_simple(offset=%d, length=%d) string=\"%s\" (this=%08p len=%d)\n", 
	  offset, length, buf, this, len);
   if (offset < 0)
      offset = len + offset;
   if ((offset < 0) || (offset >= len))  // if offset outside of string, return nothing
   {
      traceout("QoreString::substr_simple(offset, length)");
      return NULL;
   }
   if (length < 0)
   {
      length = len - offset + length;
      if (length < 0)
	 length = 0;
   }
   else if (length > (len - offset))
      length = len - offset;
   QoreString *ns = new QoreString(charset);
   ns->concat(buf + offset, length);
   traceout("QoreString::substr_simple(offset, length)");
   return ns;
}

class QoreString *QoreString::substr_simple(int offset)
{
   tracein("QoreString::substr_simple(offset)");
   printd(5, "QoreString::substr_simple(offset=%d) string=\"%s\" (this=%08p len=%d)\n", 
	  offset, buf, this, len);
   if (offset < 0)
      offset = len + offset;
   if ((offset < 0) || (offset >= len))  // if offset outside of string, return nothing
   {
      traceout("QoreString::substr_simple(offset, length)");
      return NULL;
   }
   QoreString *ns = new QoreString(charset);
   ns->concat(buf + offset);
   traceout("QoreString::substr_simple(offset)");
   return ns;
}

class QoreString *QoreString::substr_complex(int offset, int length)
{
   tracein("QoreString::substr_complex(offset, length)");
   printd(5, "QoreString::substr_complex(offset=%d, length=%d) string=\"%s\" (this=%08p len=%d)\n", 
	  offset, length, buf, this, len);

   if (offset < 0)
   {
      int clength = charset->getLength(buf);
      offset = clength + offset;

      if ((offset < 0) || (offset >= clength))  // if offset outside of string, return nothing
	 return NULL;
   }

   int start = charset->getByteLen(buf, offset);
   if (start == len)
      return NULL;

   if (length < 0)
   {
      length = charset->getLength(buf + start) + length;
      if (length < 0)
	 length = 0;
   }
   int end = charset->getByteLen(buf + start, length);

   QoreString *ns = new QoreString(charset);

   ns->concat(buf + start, end);
   return ns;
}

class QoreString *QoreString::substr_complex(int offset)
{
   //printd(5, "QoreString::substr_complex(offset=%d) string=\"%s\" (this=%08p len=%d)\n", offset, buf, this, len);
   if (offset < 0)
   {
      int clength = charset->getLength(buf);
      offset = clength + offset;

      if ((offset < 0) || (offset >= clength))  // if offset outside of string, return nothing
      {
	 //printd(5, "this=%08p, len=%d, offset=%d, clength=%d, buf=%s\n", this, len, offset, clength, buf);
	 return NULL;
      }
   }

   int start = charset->getByteLen(buf, offset);
   //printd(5, "offset=%d, start=%d\n", offset, start);
   if (start == len)
   {
      //printd(5, "this=%08p, len=%d, offset=%d, buf=%08p, start=d, %s\n", this, len, offset, buf, start, buf);
      return NULL;
   }

   // calculate byte offset
   QoreString *ns = new QoreString(charset);
   ns->concat(buf + start);
   return ns;
}

void QoreString::splice_simple(int offset, int num, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, num=%d, len=%d)\n", offset, num, len);
   int end;
   if (num > (len - offset))
   {
      end = len;
      num = len - offset;
   }
   else
      end = offset + num;

   // move down entries if necessary
   if (end != len)
      memmove(buf + offset, buf + end, sizeof(char) * (len - end));

   // calculate new length
   len -= num;
   // set last entry to NULL
   buf[len] = '\0';
}

void QoreString::splice_simple(int offset, int num, class QoreString *str, class ExceptionSink *xsink)
{
   //printd(5, "splice_intern(offset=%d, num=%d, len=%d)\n", offset, num, len);

   int end;
   if (num > (len - offset))
   {
      end = len;
      num = len - offset;
   }
   else
      end = offset + num;

   // get number of entries to insert
   if (str->len > num) // make bigger
   {
      int ol = len;
      check_char(len - num + str->len);
      // move trailing entries forward if necessary
      if (end != ol)
         memmove(buf + (end - num + str->len), buf + end, sizeof(char) * (ol - end));
   }
   else if (num > str->len) // make list smaller
      memmove(buf + offset + str->len, buf + offset + num, sizeof(char) * (len - offset - str->len));

   memcpy(buf + offset, str->buf, str->len);

   // calculate new length
   len = len - num + str->len;
   // set last entry to NULL
   buf[len] = '\0';
}

void QoreString::splice_complex(int offset, class ExceptionSink *xsink)
{
   // get length in chars
   int clen = charset->getLength(buf);
   //printd(0, "splice_complex(offset=%d) clen=%d\n", offset, clen);
   if (offset >= clen)
      return;
   if (offset < 0)
   {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }
   // calculate byte offset
   if (offset)
      offset = charset->getByteLen(buf, offset);

   // truncate string at offset
   len = offset;
   buf[len] = '\0';
}

void QoreString::splice_complex(int offset, int num, class ExceptionSink *xsink)
{
   //printd(5, "splice_complex(offset=%d, num=%d, len=%d)\n", offset, num, len);
   // get length in chars
   int clen = charset->getLength(buf);
   if (offset >= clen)
      return;
   if (offset < 0)
   {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }

   if (num < 0)
   {
      num = clen + num - offset;
      if (num < 0)
	 num = 0;
   }

   int end;
   if (num > (clen - offset))
   {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   offset = charset->getByteLen(buf, offset);
   end = charset->getByteLen(buf, end);
   num = charset->getByteLen(buf + offset, num);

   // move down entries if necessary
   if (end != len)
      memmove(buf + offset, buf + end, sizeof(char) * (len - end));

   // calculate new length
   len -= num;
   // set last entry to NULL
   buf[len] = '\0';
}

void QoreString::splice_complex(int offset, int num, class QoreString *str, class ExceptionSink *xsink)
{
   // get length in chars
   int clen = charset->getLength(buf);
   //printd(5, "splice_complex(offset=%d, num=%d, str='%s', len=%d) clen=%d buf='%s'\n", offset, num, str->getBuffer(), len, clen, buf);

   if (offset >= clen)
      offset = clen;
   else if (offset < 0)
   {
      offset = clen + offset;
      if (offset < 0)
	 offset = 0;
   }

   if (num < 0)
   {
      num = clen + num - offset;
      if (num < 0)
	 num = 0;
   }

   int end;
   if (num > (clen - offset))
   {
      end = clen;
      num = clen - offset;
   }
   else
      end = offset + num;

   // get character positions
   offset = charset->getByteLen(buf, offset);
   end = charset->getByteLen(buf, end);
   num = charset->getByteLen(buf + offset, num);

   //printd(5, "offset=%d, end=%d, num=%d\n", offset, end, num);
   // get number of entries to insert
   if (str->len > num) // make bigger
   {
      int ol = len;
      check_char(len - num + str->len);
      // move trailing entries forward if necessary
      //printd(5, "buf='%s'(%d), str='%s'(%d), end=%d, num=%d, newlen=%d\n", buf, ol, str->buf, str->len, end, num, len);
      if (end != ol)
         memmove(buf + (end - num + str->len), buf + end, ol - end);
   }
   else if (num > str->len) // make string smaller
      memmove(buf + offset + str->len, buf + offset + num, sizeof(char) * (len - offset - str->len));

   memcpy(buf + offset, str->buf, str->len);

   // calculate new length
   len = len - num + str->len;
   // set last entry to NULL
   buf[len] = '\0';
}

// NULL values sorted at end
int QoreString::compareSoft(QoreString *str, class ExceptionSink *xsink)
{
   if (!buf)
      if (!str->buf)
	 return 0;
      else
	 return 1;
   // convert charsets if necessary
   if (charset != str->charset)
   {
      class QoreString *t = str->convertEncoding(charset, xsink);
      if (xsink->isEvent())
	 return 1;
      int rc = strcmp(buf, t->buf);
      delete t;
      return rc;
   }

   return strcmp(buf, str->buf);
}
