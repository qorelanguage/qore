/*
  QoreFile.cpp

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
#include <qore/QoreFile.h>
#include <qore/intern/qore_qf_private.h>

QoreFile::QoreFile(const QoreEncoding *cs) : priv(new qore_qf_private(cs)) {
}

QoreFile::~QoreFile() {
   delete priv;
}

#ifdef HAVE_STRUCT_FLOCK
int QoreFile::lockBlocking(struct flock &fl, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc;
   while (true) {
      rc = fcntl(priv->fd, F_SETLKW, &fl);
      // try again if we are interrupted by a signal
      if (rc != -1 || errno != EINTR)
	 break;
   }
   if (rc == -1)
      xsink->raiseErrnoException("FILE-LOCK-ERROR", errno, "the call to fcntl(F_SETLKW) failed");

   return rc;
}

//! perform a file lock operation, does not block
int QoreFile::lock(const struct flock &fl, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc;
   while (true) {
      rc = fcntl(priv->fd, F_SETLK, &fl);
      // repeat if interrupted
      if (!rc || (rc == -1 && errno != EINTR))
	 break;
   }
   // only raise an exception if the lock failed for a reason other than
   // that it is already locked by someone else
   if (rc == -1 && errno != EACCES && errno != EAGAIN)
      xsink->raiseErrnoException("FILE-LOCK-ERROR", errno, "the call to fcntl(F_SETLK) failed");

   return rc;
}

//! get lock info operation, does not block
int QoreFile::getLockInfo(struct flock &fl, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc;
   while (true) {
      rc = fcntl(priv->fd, F_GETLK, &fl);
      // repeat if interrupted
      if (!rc || (rc == -1 && errno != EINTR))
	 break;
   }
   if (rc)
      xsink->raiseErrnoException("FILE-LOCK-ERROR", errno, "the call to fcntl(F_GETLK) failed");

   return rc;
}
#endif

#ifdef HAVE_PWD_H
int QoreFile::chown(uid_t owner, gid_t group, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-CHOWN-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fchown(priv->fd, owner, group);
   if (rc)
      xsink->raiseErrnoException("FILE-CHOWN-ERROR", errno, "the chown(%d, %d) operation failed", owner, group);

   return rc;
}
#endif

#if 0
int QoreFile::preallocate(fstore_t &fs, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-PREALLOCATE-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_PREALLOCATE, &fs);
   if (rc)
      xsink->raiseErrnoException("FILE-PREALLOCATE-ERROR", errno, "the call to fcntl(F_PREALLOCATE) failed (%d bytes allocated)", fs.fst_bytesalloc);

   return rc;
}
#endif

QoreStringNode *QoreFile::getFileName() const {
   AutoLocker al(priv->m);

   return priv->filename.empty() ? 0 : new QoreStringNode(priv->filename.c_str());
}

std::string QoreFile::getFileNameStr() const {
   AutoLocker al(priv->m);

   return priv->filename;
}

int QoreFile::close() {
   AutoLocker al(priv->m);

   return priv->close_intern();
}

void QoreFile::setEncoding(const QoreEncoding *cs)
{
   priv->charset = cs;
}

const QoreEncoding *QoreFile::getEncoding() const {
   return priv->charset;
}

#ifndef HAVE_FSYNC
/* Emulate fsync on platforms which lack it, primarily Windows and
   cross-compilers like MinGW.

   This is derived from sqlite3 sources and is in the public domain.

   Written by Richard W.M. Jones <rjones.at.redhat.com>
*/
#ifdef _Q_WINDOWS
int fsync (int fd) {
   HANDLE h = (HANDLE) _get_osfhandle (fd);
   DWORD err;

   if (h == INVALID_HANDLE_VALUE) {
      errno = EBADF;
      return -1;
   }

   if (!FlushFileBuffers (h)) {
      /* Translate some Windows errors into rough approximations of Unix
       * errors.  MSDN is useless as usual - in this case it doesn't
       * document the full range of errors.
       */
      err = GetLastError();
      switch (err) {
	 /* eg. Trying to fsync a tty. */
	 case ERROR_INVALID_HANDLE:
	    errno = EINVAL;
	    break;

	 default:
	    errno = EIO;
      }
      return -1;
   }
   return 0;
}
#else // windows
#error no fsync() on this platform
#endif
#endif // HAVE_FSYNC

int QoreFile::sync() {
   AutoLocker al(priv->m);

   if (priv->is_open)
      return ::fsync(priv->fd);
   return -1;
}

void QoreFile::makeSpecial(int sfd) {
   priv->is_open = true;
   priv->filename.clear();
   priv->charset = QCS_DEFAULT;
   priv->special_file = true;
   priv->fd = sfd;
}

int QoreFile::open(const char *fn, int flags, int mode, const QoreEncoding *cs) {
   return priv->open(fn, flags, mode, cs);
}

int QoreFile::open2(ExceptionSink *xsink, const char *fn, int flags, int mode, const QoreEncoding *cs) {
   if (!fn) {
      xsink->raiseException("FILE-OPEN2-ERROR", "no file name given");
      return -1;
   }

   if (priv->special_file) {
      xsink->raiseException("FILE-OPEN2-ERROR", "system files cannot be reopened");
      return -1;
   }

   int rc;
   {
      AutoLocker al(priv->m);

      rc = priv->open_intern(fn, flags, mode, cs);
   }

   if (rc) {
      xsink->raiseErrnoException("FILE-OPEN2-ERROR", errno, "cannot open '%s'", fn);
      return -1;
   }

   return 0;
}

int QoreFile::readLine(QoreString &str) {
   return priv->readLine(str);
}

QoreStringNode* QoreFile::readLine(ExceptionSink* xsink) {
   return priv->readLine(true, xsink);
}

QoreStringNode* QoreFile::readLine(bool incl_eol, ExceptionSink* xsink) {
   return priv->readLine(incl_eol, xsink);
}

int QoreFile::readLine(QoreString &str, bool incl_eol) {
   return priv->readLine(str, incl_eol);
}

QoreStringNode* QoreFile::readUntil(const char* bytes, bool incl_bytes, ExceptionSink* xsink) {
   return priv->readUntil(bytes, incl_bytes, xsink);
}

int QoreFile::readUntil(char byte, QoreString& str, bool incl_byte) {
   return priv->readUntil(byte, str, incl_byte);
}

int QoreFile::readUntil(const char* bytes, QoreString& str, bool incl_bytes) {
   return priv->readUntil(bytes, str, incl_bytes);
}

qore_size_t QoreFile::setPos(qore_size_t pos) {
   AutoLocker al(priv->m);

   if (!priv->is_open)
      return -1;

   return lseek(priv->fd, pos, SEEK_SET);
}

// FIXME: deleteme
qore_size_t QoreFile::getPos() {
   return priv->getPos();
}

qore_size_t QoreFile::getPos() const {
   return priv->getPos();
}

QoreStringNode *QoreFile::getchar(ExceptionSink *xsink) {
   SimpleRefHolder<QoreStringNode> str(new QoreStringNode(priv->charset));

   int c;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;

      c = priv->readChar();
      if (c < 0)
	 return 0;

      str->concat((char)c);
      if (priv->charset->isMultiByte())
	 return str.release();

      // read in more characters for multi-byte chars if needed
      qore_offset_t rc = priv->charset->getCharLen(str->getBuffer(), 1);
      // rc < 0: invalid character
      if (!rc) {
	 xsink->raiseException("FILE-GETCHAR-ERROR", "invalid multi-byte character received: initial byte 0x%x is an invalid initial character for '%s' character encoding", c, priv->charset->getCode());
	 return 0;
      }

      // rc == 1: we have a valid character already with the single byte
      if (rc == 1)
	 return str.release();

      assert(rc < 0);
      rc = -rc;
      while (rc--) {
	 c = priv->readChar();
	 if (c < 0) {
	    xsink->raiseException("FILE-GETCHAR-ERROR", "invalid multi-byte character received: EOF encountered after %d byte%s read of a %d byte %s character", str->strlen(), str->strlen() == 1 ? "" : "s", str->strlen() + rc + 1, priv->charset->getCode());
	    return 0;
	 }

	 str->concat((char)c);
      }
   }

   return str.release();
}

QoreStringNode *QoreFile::getchar() {
   int c;
   {
      AutoLocker al(priv->m);

      if (!priv->is_open)
	 return 0;

      c = priv->readChar();
   }

   if (c < 0)
      return 0;

   QoreStringNode * str = new QoreStringNode(priv->charset);
   str->concat((char)c);
   return str;
}

int QoreFile::write(const void *data, qore_size_t len, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   if (!len)
      return 0;

   return priv->write(data, len, xsink);
}

int QoreFile::write(const QoreString *str, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   if (!str)
      return 0;

   TempEncodingHelper wstr(str, priv->charset, xsink);
   if (*xsink)
      return -1;

   //printd(0, "QoreFile::write() str priv->charset=%s, priv->charset=%s\n", str->getEncoding()->code, priv->charset->code);

   return priv->write(wstr->getBuffer(), wstr->strlen(), xsink);
}

int QoreFile::write(const BinaryNode *b, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   if (!b)
      return 0;

   return priv->write(b->getPtr(), b->size(), xsink);
}

int QoreFile::read(QoreString &str, qore_offset_t size, ExceptionSink *xsink) {
   str.clear();

   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return -1;

      buf = priv->readBlock(size, -1, "read", xsink);
   }
   if (!buf)
      return -1;

   str.takeAndTerminate(buf, size, priv->charset);
   return 0;
}

QoreStringNode *QoreFile::read(qore_offset_t size, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;

      buf = priv->readBlock(size, -1, "read", xsink);
   }
   if (!buf)
      return 0;

   QoreStringNode* str;
   if (size) {
      str = new QoreStringNode(buf, size, size, priv->charset);
      str->terminate(size);
   }
   else
      str = new QoreStringNode(priv->charset);
   return str;
}

int QoreFile::readBinary(BinaryNode &b, qore_offset_t size, ExceptionSink *xsink) {
   b.clear();

   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return -1;

      buf = priv->readBlock(size, -1, "readBinary", xsink);
   }
   if (!buf)
      return -1;

   if (size)
      b.append(buf, size);
   free(buf);
   return 0;
}

BinaryNode *QoreFile::readBinary(qore_offset_t size, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;

      buf = priv->readBlock(size, -1, "readBinary", xsink);
   }
   if (!buf)
      return 0;

   return new BinaryNode(buf, size);
}

QoreStringNode *QoreFile::read(qore_offset_t size, int timeout_ms, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;

      buf = priv->readBlock(size, timeout_ms, "read", xsink);
   }
   if (!buf)
      return 0;

   QoreStringNode *str = new QoreStringNode(buf, size, size, priv->charset);
   //str->terminate(buf[size - 1] ? size : size - 1);
   str->terminate(size);
   return str;
}

BinaryNode *QoreFile::readBinary(qore_offset_t size, int timeout_ms, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;

      buf = priv->readBlock(size, timeout_ms, "readBinary", xsink);
   }
   if (!buf)
      return 0;

   return new BinaryNode(buf, size);
}

int QoreFile::writei1(char i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   return priv->write((char *)&i, 1, xsink);
}

int QoreFile::writei2(short i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = htons(i);
   return priv->write((char *)&i, 2, xsink);
}

int QoreFile::writei4(int i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = htonl(i);
   return priv->write((char *)&i, 4, xsink);
}

int QoreFile::writei8(int64 i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = i8MSB(i);
   return priv->write((char *)&i, 4, xsink);
}

int QoreFile::writei2LSB(short i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = i2LSB(i);
   return priv->write((char *)&i, 2, xsink);
}

int QoreFile::writei4LSB(int i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = i4LSB(i);
   return priv->write((char *)&i, 4, xsink);
}

int QoreFile::writei8LSB(int64 i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = i8LSB(i);
   return priv->write((char *)&i, 4, xsink);
}

int QoreFile::readu1(unsigned char *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 1);
   if (rc <= 0)
      return -1;
   return 0;
}

int QoreFile::readu2(unsigned short *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 2);
   if (rc <= 0)
      return -1;

   *val = ntohs(*val);
   return 0;
}

int QoreFile::readu4(unsigned int *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 4);
   if (rc <= 0)
      return -1;

   *val = ntohl(*val);
   return 0;
}

int QoreFile::readu2LSB(unsigned short *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 2);
   if (rc <= 0)
      return -1;

   *val = LSBi2(*val);
   return 0;
}

int QoreFile::readu4LSB(unsigned int *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 4);
   if (rc <= 0)
      return -1;

   *val = LSBi4(*val);
   return 0;
}

int QoreFile::readi1(char *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 1);
   if (rc <= 0)
      return -1;
   return 0;
}

int QoreFile::readi2(short *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 2);
   if (rc <= 0)
      return -1;

   *val = ntohs(*val);
   return 0;
}

int QoreFile::readi4(int *val, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 4);
   if (rc <= 0)
      return -1;

   *val = ntohl(*val);
   return 0;
}

int QoreFile::readi8(int64 *val, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 8);
   if (rc <= 0)
      return -1;

   *val = MSBi8(*val);
   return 0;
}

int QoreFile::readi2LSB(short *val, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 2);
   if (rc <= 0)
      return -1;

   *val = LSBi2(*val);
   return 0;
}

int QoreFile::readi4LSB(int *val, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 4);
   if (rc <= 0)
      return -1;

   *val = LSBi4(*val);
   return 0;
}

int QoreFile::readi8LSB(int64 *val, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_read_open(xsink))
      return -1;

   int rc = priv->read(val, 8);
   if (rc <= 0)
      return -1;

   *val = LSBi8(*val);
   return 0;
}

bool QoreFile::isOpen() const {
   return priv->isOpen();
}

bool QoreFile::isDataAvailable(int timeout_ms, ExceptionSink *xsink) const {
   return priv->isDataAvailable(timeout_ms, xsink);
}

int QoreFile::getFD() const {
   return priv->fd;
}

#ifdef HAVE_TERMIOS_H
int QoreFile::setTerminalAttributes(int action, QoreTermIOS *ios, ExceptionSink *xsink) const {
   return priv->setTerminalAttributes(action, ios, xsink);
}

int QoreFile::getTerminalAttributes(QoreTermIOS *ios, ExceptionSink *xsink) const {
   return priv->getTerminalAttributes(ios, xsink);
}
#endif

void QoreFile::setEventQueue(Queue *cbq, ExceptionSink *xsink) {
   priv->setEventQueue(cbq, xsink);
}

void QoreFile::cleanup(ExceptionSink *xsink) {
   priv->cleanup(xsink);
}

QoreListNode *QoreFile::stat(ExceptionSink *xsink) const {
   return priv->stat(xsink);
}

QoreHashNode *QoreFile::hstat(ExceptionSink *xsink) const {
   return priv->hstat(xsink);
}

#ifdef Q_HAVE_STATVFS
QoreHashNode *QoreFile::statvfs(ExceptionSink *xsink) const {
   return priv->statvfs(xsink);
}
#endif

bool QoreFile::isTty() const {
   return priv->isTty();
}
