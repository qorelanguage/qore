/*
  QoreFile.h

  File object

  Qore Programming Language

  Copyright (C) 2005 David Nichols
  
  FIXME: normalize methods to raise exceptions when file operations are attempted on a closed file

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

#ifndef _QORE_QOREFILE_H

#define _QORE_QOREFILE_H

#include <qore/config.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

extern class QoreEncoding *QCS_DEFAULT;

class QoreFile {
   private:
      int fd;
      bool is_open;
      bool special_file;
      int flags;
      class QoreEncoding *charset;
      char *filename;

      inline int readChar();
     
   public:
      inline QoreFile(class QoreEncoding *cs)
      {
	 is_open = false;
	 filename = NULL;
	 charset = cs;
	 special_file = false;
      }
      inline ~QoreFile()
      {
	 close();
      }

      // NOTE: this can only be called right after the constructor by the system itself
      inline void makeSpecial(int sfd);

      inline int open(char *fn, int flags = O_RDONLY, int mode = 0777, class QoreEncoding *cs = QCS_DEFAULT);
      
      inline int close()
      {
	 if (filename)
	 {
	    free(filename);
	    filename = NULL;
	 }

	 int rc;
	 if (is_open)
	 {
	    if (special_file)
	       rc = -1;
	    else
	    {
	       rc = ::close(fd);
	       is_open = false;
	    }
	 }
	 else
	    rc = 0;
	 return rc;
      }

      inline void setEncoding(class QoreEncoding *cs)
      {
	 charset = cs;
      }
      inline class QoreEncoding *getEncoding()
      {
	 return charset;
      }
      inline int sync()
      {
	 int rc;
	 if (is_open)
	    rc = ::fsync(fd);
	 else
	    rc = -1;
	 return rc;
      }

      inline class QoreString *readLine(class ExceptionSink *xsink);
      inline int write(class QoreString *str, class ExceptionSink *xsink);
      inline int write(class BinaryObject *b, class ExceptionSink *xsink);
      inline int writei1(char i, class ExceptionSink *xsink);
      inline int writei2(short i, class ExceptionSink *xsink);
      inline int writei4(int i, class ExceptionSink *xsink);
      inline int writei8(int64 i, class ExceptionSink *xsink);
      inline int writei2LSB(short i, class ExceptionSink *xsink);
      inline int writei4LSB(int i, class ExceptionSink *xsink);
      inline int writei8LSB(int64 i, class ExceptionSink *xsink);
      inline int readi1(char *val, class ExceptionSink *xsink);
      inline int readi2(short *val, class ExceptionSink *xsink);
      inline int readi4(int *val, class ExceptionSink *xsink);
      inline int readi8(int64 *val, class ExceptionSink *xsink);
      inline int readi2LSB(short *val, class ExceptionSink *xsink);
      inline int readi4LSB(int *val, class ExceptionSink *xsink);
      inline int readi8LSB(int64 *val, class ExceptionSink *xsink);
      inline class QoreString *read(int size, class ExceptionSink *xsink);
      inline class BinaryObject *readBinary(int size, class ExceptionSink *xsink);
      inline int setPos(int pos);
      inline int getPos();
      inline class QoreString *getchar();
      inline char *getFileName() { return filename; }
#ifdef DEBUG
      inline int getFD() { return is_open ? fd : -1; }
#endif
};

#include <qore/charset.h>
#include <qore/QoreString.h>
#include <qore/BinaryObject.h>
#include <qore/QoreLib.h>

inline void QoreFile::makeSpecial(int sfd)
{
   is_open = true;
   filename = NULL;
   charset = QCS_DEFAULT;
   special_file = true;
   fd = sfd;
}

inline int QoreFile::open(char *fn, int flags, int mode, class QoreEncoding *cs)
{
   if (!fn || special_file)
      return -1;
   close();
   if (!flags)
      flags = O_RDONLY;
   if (!mode)
      mode = 0777;   
   fd = ::open(fn, flags, mode);
   filename = strdup(fn);
   if (fd < 0)
      return fd;
   if (cs)
      charset = cs;
   is_open = true;
   return 0;
}

// private function
inline int QoreFile::readChar()
{
   unsigned char ch = 0;
   if (::read(fd, &ch, 1) != 1)
      return -1;
   return (int)ch;
}

inline class QoreString *QoreFile::readLine(class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READLINE-ERROR", "file has not been opened");
      return NULL;
   }

   int ch;
   class QoreString *str = new QoreString(charset, NULL);
   while ((ch = readChar()) >= 0)
   {
      char c = ch;
      str->concat((char)c);
      if (c == '\n')
	 break;
   }
   if (!str->strlen())
   {
      delete str;
      str = NULL;
   }
   return str;
}

inline int QoreFile::setPos(int pos)
{
   if (!is_open)
      return -1;

   return lseek(fd, pos, SEEK_SET);
}

inline int QoreFile::getPos()
{
   if (!is_open)
      return -1;

   return lseek(fd, 0, SEEK_CUR);
}

inline class QoreString *QoreFile::getchar()
{
   if (!is_open)
      return NULL;

   int c = readChar();
   if (c < 0)
      return NULL;
   QoreString *str = new QoreString(charset, NULL);
   str->concat((char)c);
   return str;
}

inline int QoreFile::write(class QoreString *str, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   if (!str)
      return 0;

   QoreString *wstr;
   if (str->getEncoding() != charset && charset && str->getEncoding())
   {
      wstr = str->convertEncoding(charset, xsink);
      if (xsink->isEvent())
	 return -1;
   }
   else 
      wstr = str;
   //printd(0, "QoreFile::write() str charset=%s, charset=%s\n", str->getEncoding()->code, charset->code);

   int rc = ::write(fd, wstr->getBuffer(), wstr->strlen());
   if (str != wstr)
      delete wstr;
   return rc;
}

inline int QoreFile::write(class BinaryObject *b, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   if (!b)
      return 0;

   return ::write(fd, b->getPtr(), b->size());
}

#ifndef DEFAULT_FILE_BUFSIZE
#define DEFAULT_FILE_BUFSIZE 4096
#endif
inline class QoreString *QoreFile::read(int size, class ExceptionSink *xsink)
{
   if (!size)
      return NULL;

   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return NULL;
   }

   int bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
   int br = 0;
   char *buf = (char *)malloc(sizeof(char) * bs);
   QoreString *str = new QoreString(charset, "");

   while (true)
   {
      int rc = ::read(fd, buf, bs);
      if (rc <= 0)
	 break;
      str->concat(buf, rc);
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
      delete str;
      return NULL;
   }
   return str;
}

inline class BinaryObject *QoreFile::readBinary(int size, class ExceptionSink *xsink)
{
   if (!size)
      return NULL;

   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return NULL;
   }

   int bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
   int br = 0;
   char *buf = (char *)malloc(sizeof(char) * bs);
   char *bbuf = NULL;

   while (true)
   {
      int rc = ::read(fd, buf, bs);
      if (rc <= 0)
	 break;
      // enlarge bbuf
      bbuf = (char *)realloc(bbuf, br + rc);
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
   return new BinaryObject(bbuf, br);
}

inline int QoreFile::writei1(char i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   return ::write(fd, (char *)&i, 1);   
}

inline int QoreFile::writei2(short i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = htons(i);
   return ::write(fd, (char *)&i, 2);   
}

inline int QoreFile::writei4(int i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = htonl(i);
   return ::write(fd, (char *)&i, 4);
}

inline int QoreFile::writei8(int64 i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = i8MSB(i);
   return ::write(fd, (char *)&i, 4);
}

inline int QoreFile::writei2LSB(short i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = i2LSB(i);
   return ::write(fd, (char *)&i, 2);   
}

inline int QoreFile::writei4LSB(int i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = i4LSB(i);
   return ::write(fd, (char *)&i, 4);
}

inline int QoreFile::writei8LSB(int64 i, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   i = i8LSB(i);
   return ::write(fd, (char *)&i, 4);
}

inline int QoreFile::readi1(char *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 1);
   if (rc <= 0)
      return -1;
   return 0;
}

inline int QoreFile::readi2(short *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = ntohs(*val);
   return 0;
}

inline int QoreFile::readi4(int *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = ntohl(*val);
   return 0;
}

inline int QoreFile::readi8(int64 *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 8);
   if (rc <= 0)
      return -1;
   
   *val = MSBi8(*val);
   return 0;
}

inline int QoreFile::readi2LSB(short *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = LSBi2(*val);
   return 0;
}

inline int QoreFile::readi4LSB(int *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = LSBi4(*val);
   return 0;
}

inline int QoreFile::readi8LSB(int64 *val, class ExceptionSink *xsink)
{
   if (!is_open)
   {
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   int rc = ::read(fd, val, 8);
   if (rc <= 0)
      return -1;
   
   *val = LSBi8(*val);
   return 0;
}

#endif  // _QORE_QOREFILE_H
