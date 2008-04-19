/*
  QoreFile.cc

  Network functions

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
#include <qore/QoreFile.h>

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/file.h>

#include <string>

struct qore_qf_private {
      int fd;
      bool is_open;
      bool special_file;
      const QoreEncoding *charset;
      std::string filename;

      DLLLOCAL qore_qf_private(const QoreEncoding *cs)
      {
	 is_open = false;
	 charset = cs;
	 special_file = false;
      }

      DLLLOCAL ~qore_qf_private()
      {
      }
};

// returns -1 for exception
int QoreFile::check_read_open(ExceptionSink *xsink)
{
   if (priv->is_open)
      return 0;
   
   xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
   return -1;
}

// returns -1 for exception
int QoreFile::check_write_open(ExceptionSink *xsink)
{
   if (priv->is_open)
      return 0;
   
   xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
   return -1;
}

QoreFile::QoreFile(const QoreEncoding *cs) : priv(new qore_qf_private(cs))
{
}

QoreFile::~QoreFile()
{
   close();
   delete priv;
}

int QoreFile::lockBlocking(struct flock &fl, ExceptionSink *xsink)
{
   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_SETLKW, &fl);
   if (rc)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl() failed: %s", strerror(errno));
   return rc;
}

//! perform a file lock operation, does not block
int QoreFile::lock(const struct flock &fl, ExceptionSink *xsink)
{
   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_SETLK, &fl);
   if (rc)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl() failed: %s", strerror(errno));
   return rc;
}

//! get lock info operation, does not block
int QoreFile::getLockInfo(struct flock &fl, ExceptionSink *xsink)
{
   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_GETLK, &fl);
   if (rc)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl(F_GETLK) failed: %s", strerror(errno));
   return rc;
}

int QoreFile::chown(uid_t owner, gid_t group, ExceptionSink *xsink)
{
   if (!priv->is_open) {
      xsink->raiseException("FILE-CHOWN-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fchown(priv->fd, owner, group);
   if (rc)
      xsink->raiseException("FILE-CHOWN-ERROR", "the chown(%d, %d) operation failed: %s", owner, group, strerror(errno));

   return rc;
}

int QoreFile::preallocate(fstore_t &fs, ExceptionSink *xsink)
{
   if (!priv->is_open) {
      xsink->raiseException("FILE-PREALLOCATE-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_PREALLOCATE, &fs);
   if (rc)
      xsink->raiseException("FILE-PREALLOCATE-ERROR", "the call to fcntl(F_PREALLOCATE) failed (%d bytes allocated): %s", fs.fst_bytesalloc, strerror(errno));
   return rc;
}

const char *QoreFile::getFileName() const
{ 
   return priv->filename.c_str(); 
}

int QoreFile::close()
{
   priv->filename.clear();

   int rc;
   if (priv->is_open)
   {
      if (priv->special_file)
	 rc = -1;
      else
      {
	 rc = ::close(priv->fd);
	 priv->is_open = false;
      }
   }
   else
      rc = 0;
   return rc;
}

void QoreFile::setEncoding(const QoreEncoding *cs)
{
   priv->charset = cs;
}

const QoreEncoding *QoreFile::getEncoding() const
{
   return priv->charset;
}

int QoreFile::sync()
{
   int rc;
   if (priv->is_open)
      rc = ::fsync(priv->fd);
   else
      rc = -1;
   return rc;
}

void QoreFile::makeSpecial(int sfd)
{
   priv->is_open = true;
   priv->filename.clear();
   priv->charset = QCS_DEFAULT;
   priv->special_file = true;
   priv->fd = sfd;
}

int QoreFile::open(const char *fn, int flags, int mode, const QoreEncoding *cs)
{
   if (!fn || priv->special_file)
      return -1;
   close();
   if (!flags)
      flags = O_RDONLY;
   priv->fd = ::open(fn, flags, mode);
   if (priv->fd < 0)
      return priv->fd;
   priv->filename = fn;
   if (cs)
      priv->charset = cs;
   priv->is_open = true;
   return 0;
}

int QoreFile::open2(ExceptionSink *xsink, const char *fn, int flags, int mode, const QoreEncoding *cs)
{
   if (!fn)
   {
      xsink->raiseException("FILE-OPEN2-ERROR", "no file name given");
      return -1;
   }
   if (priv->special_file)
   {
      xsink->raiseException("FILE-OPEN2-ERROR", "system files cannot be reopened");
      return -1;
   }
   close();
   if (!flags)
      flags = O_RDONLY;
   priv->fd = ::open(fn, flags, mode);
   if (priv->fd < 0) {
      xsink->raiseException("FILE-OPEN2-ERROR", "cannot open '%s': %s", fn, strerror(errno));
      return -1;
   }
   priv->filename = fn;
   if (cs)
      priv->charset = cs;
   priv->is_open = true;
   return 0;
}

// private function
int QoreFile::readChar()
{
   unsigned char ch = 0;
   if (::read(priv->fd, &ch, 1) != 1)
      return -1;
   return (int)ch;
}

QoreStringNode *QoreFile::readLine(ExceptionSink *xsink)
{
   if (!priv->is_open)
   {
      xsink->raiseException("FILE-READLINE-ERROR", "file has not been opened");
      return 0;
   }
   
   int ch;

   QoreStringNodeHolder str(new QoreStringNode(priv->charset));
   while ((ch = readChar()) >= 0)
   {
      char c = ch;
      str->concat((char)c);
      if (c == '\n')
	 break;
   }
   if (!str->strlen())
      return 0;

   return str.release();
}

int QoreFile::setPos(int pos)
{
   if (!priv->is_open)
      return -1;
   
   return lseek(priv->fd, pos, SEEK_SET);
}

int QoreFile::getPos()
{
   if (!priv->is_open)
      return -1;
   
   return lseek(priv->fd, 0, SEEK_CUR);
}

QoreStringNode *QoreFile::getchar()
{
   if (!priv->is_open)
      return 0;
   
   int c = readChar();
   if (c < 0)
      return 0;
   QoreStringNode *str = new QoreStringNode(priv->charset);
   str->concat((char)c);
   return str;
}

int QoreFile::write(const void *data, unsigned len, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   if (!len)
      return 0;
   
   return ::write(priv->fd, data, len);
}

int QoreFile::write(const QoreString *str, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   if (!str)
      return 0;

   TempEncodingHelper wstr(str, priv->charset, xsink);
   if (*xsink)
      return -1;

   //printd(0, "QoreFile::write() str priv->charset=%s, priv->charset=%s\n", str->getEncoding()->code, priv->charset->code);
   
   return ::write(priv->fd, wstr->getBuffer(), wstr->strlen());
}

int QoreFile::write(const BinaryNode *b, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   if (!b)
      return 0;
   
   return ::write(priv->fd, b->getPtr(), b->size());
}

#ifndef DEFAULT_FILE_BUFSIZE
#define DEFAULT_FILE_BUFSIZE 4096
#endif

QoreStringNode *QoreFile::read(int size, ExceptionSink *xsink)
{
   if (!size)
      return 0;
   
   if (check_read_open(xsink))
      return 0;
   
   char *buf = readBlock(size);
   if (!buf)
      return 0;

   QoreStringNode *str = new QoreStringNode(buf, size, size, priv->charset);
   str->terminate(buf[size - 1] ? size : size - 1);
   return str;
}

class BinaryNode *QoreFile::readBinary(int size, ExceptionSink *xsink)
{
   if (!size)
      return 0;
   
   if (check_read_open(xsink))
      return 0;
   
   char *buf = readBlock(size);
   if (!buf)
      return 0;
   
   return new BinaryNode(buf, size);
}

int QoreFile::writei1(char i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;

   return ::write(priv->fd, (char *)&i, 1);   
}

int QoreFile::writei2(short i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;

   i = htons(i);
   return ::write(priv->fd, (char *)&i, 2);   
}

int QoreFile::writei4(int i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   i = htonl(i);
   return ::write(priv->fd, (char *)&i, 4);
}

int QoreFile::writei8(int64 i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   i = i8MSB(i);
   return ::write(priv->fd, (char *)&i, 4);
}

int QoreFile::writei2LSB(short i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   i = i2LSB(i);
   return ::write(priv->fd, (char *)&i, 2);   
}

int QoreFile::writei4LSB(int i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   i = i4LSB(i);
   return ::write(priv->fd, (char *)&i, 4);
}

int QoreFile::writei8LSB(int64 i, ExceptionSink *xsink)
{
   if (check_write_open(xsink))
      return -1;
   
   i = i8LSB(i);
   return ::write(priv->fd, (char *)&i, 4);
}

int QoreFile::readu1(unsigned char *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 1);
   if (rc <= 0)
      return -1;
   return 0;
}

int QoreFile::readu2(unsigned short *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = ntohs(*val);
   return 0;
}

int QoreFile::readu4(unsigned int *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = ntohl(*val);
   return 0;
}

int QoreFile::readu2LSB(unsigned short *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = LSBi2(*val);
   return 0;
}

int QoreFile::readu4LSB(unsigned int *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = LSBi4(*val);
   return 0;
}

int QoreFile::readi1(char *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
      
   int rc = ::read(priv->fd, val, 1);
   if (rc <= 0)
      return -1;
   return 0;
}

int QoreFile::readi2(short *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
      
   int rc = ::read(priv->fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = ntohs(*val);
   return 0;
}

int QoreFile::readi4(int *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = ntohl(*val);
   return 0;
}

int QoreFile::readi8(int64 *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 8);
   if (rc <= 0)
      return -1;
   
   *val = MSBi8(*val);
   return 0;
}

int QoreFile::readi2LSB(short *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 2);
   if (rc <= 0)
      return -1;
   
   *val = LSBi2(*val);
   return 0;
}

int QoreFile::readi4LSB(int *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 4);
   if (rc <= 0)
      return -1;
   
   *val = LSBi4(*val);
   return 0;
}

int QoreFile::readi8LSB(int64 *val, ExceptionSink *xsink)
{
   if (check_read_open(xsink))
      return -1;
   
   int rc = ::read(priv->fd, val, 8);
   if (rc <= 0)
      return -1;
   
   *val = LSBi8(*val);
   return 0;
}

char *QoreFile::readBlock(int &size)
{
   int bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
   int br = 0;
   char *buf = (char *)malloc(sizeof(char) * bs);
   char *bbuf = 0;

   while (true)
   {
      int rc = ::read(priv->fd, buf, bs);
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
      return 0;
   }
   size = br;
   return bbuf;
}
