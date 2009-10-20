/*
  QoreFile.cc

  Network functions

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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
#include <qore/intern/QC_Queue.h>

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/file.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <string>

#ifndef DEFAULT_FILE_BUFSIZE
#define DEFAULT_FILE_BUFSIZE 4096
#endif

struct qore_qf_private {
   int fd;
   bool is_open;
   bool special_file;
   const QoreEncoding *charset;
   std::string filename;
   mutable QoreThreadLock m;
   Queue *cb_queue;

   DLLLOCAL qore_qf_private(const QoreEncoding *cs) : is_open(false),
						      special_file(false),
						      charset(cs), 
						      cb_queue(0) {
   }

   DLLLOCAL ~qore_qf_private() {
      close_intern();

      // must be dereferenced and removed before deleting
      assert(!cb_queue);
   }

   DLLLOCAL int close_intern() {
      filename.clear();

      int rc;
      if (is_open) {
	 if (special_file)
	    rc = -1;
	 else {	    
	    rc = ::close(fd);
	    is_open = false;
	    do_close_event();
	 }
      }
      else
	 rc = 0;
      return rc;
   }

   DLLLOCAL int open_intern(const char *fn, int flags, int mode, const QoreEncoding *cs) {
      close_intern();

      if (!flags)
	 flags = O_RDONLY;

      do_open_event(fn, flags, mode, cs);

      fd = ::open(fn, flags, mode);
      if (fd < 0)
	 return fd;

      do_opened_event(fn, flags, mode, cs);

      filename = fn;
      if (cs)
	 charset = cs;
      is_open = true;
      return 0;
   }

   DLLLOCAL int open(const char *fn, int flags, int mode, const QoreEncoding *cs) {
      if (!fn || special_file)
	 return -1;

      AutoLocker al(m);
      return open_intern(fn, flags, mode, cs);
   }

   // returns -1 for exception
   DLLLOCAL int check_read_open(ExceptionSink *xsink) const {
      if (is_open)
	 return 0;
   
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   // returns -1 for exception
   DLLLOCAL int check_write_open(ExceptionSink *xsink) const {
      if (is_open)
	 return 0;
      
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   // returns -1 for exception
   DLLLOCAL int check_open(ExceptionSink *xsink) const {
      if (is_open)
	 return 0;
      
      xsink->raiseException("FILE-OPERATION-ERROR", "file has not been opened");
      return -1;
   }

   DLLLOCAL bool isDataAvailable(int timeout_ms, ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (check_read_open(xsink))
	 return false;
      
      return isDataAvailableIntern(timeout_ms);
   }

   // assumes lock is held and file is open
   DLLLOCAL bool isDataAvailableIntern(int timeout_ms) const {
      fd_set sfs;
      
      FD_ZERO(&sfs);
      FD_SET(fd, &sfs);

      struct timeval tv;
      int rc;
      while (true) {
	 tv.tv_sec  = timeout_ms / 1000;
	 tv.tv_usec = (timeout_ms % 1000) * 1000;
      
	 rc = select(fd + 1, &sfs, 0, 0, &tv);   
	 // retry if we were interrupted by a signal
	 if (rc >= 0 || errno != EINTR)
	    break;
      }
      return rc;
   }

   DLLLOCAL int setTerminalAttributes(int action, QoreTermIOS *ios, ExceptionSink *xsink) const {
      AutoLocker al(m);
      
      if (check_open(xsink))
	 return -1;

      return ios->set(fd, action, xsink);
   }

   DLLLOCAL int getTerminalAttributes(QoreTermIOS *ios, ExceptionSink *xsink) const {
      AutoLocker al(m);
      
      if (check_open(xsink))
	 return -1;

      return ios->get(fd, xsink);
   }

   // unlocked, assumes file is open
   DLLLOCAL qore_size_t read(void *buf, qore_size_t bs) const {
      qore_offset_t rc;
      while (true) {
	 rc = ::read(fd, buf, bs);
	 // try again if we were interrupted by a signal
	 if (rc >= 0 || errno != EINTR)
	    break;
      }

      if (rc > 0)
	 do_read_event(rc, rc, bs);

      return rc;
   }

   // unlocked, assumes file is open
   DLLLOCAL qore_size_t write(const void *buf, qore_size_t len) const {
      qore_offset_t rc;
      while (true) {
	 rc = ::write(fd, buf, len);
	 // try again if we are interrupted by a signal
	 if (rc >= 0 || errno != EINTR)
	    break;
      }

      if (rc > 0)
	 do_write_event(rc, rc, len);

      return rc;
   }

   // private function, unlocked
   DLLLOCAL int readChar() const {
      unsigned char ch = 0;
      if (read(&ch, 1) != 1)
	 return -1;
      return (int)ch;
   }

   DLLLOCAL char *readBlock(qore_offset_t &size, int timeout_ms, ExceptionSink *xsink) {
      qore_size_t bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
      qore_size_t br = 0;
      char *buf = (char *)malloc(sizeof(char) * bs);
      char *bbuf = 0;

      while (true) {
	 // wait for data
	 if (timeout_ms >= 0 && !isDataAvailableIntern(timeout_ms)) {
	    xsink->raiseException("FILE-READ-TIMEOUT", "timeout limit exceeded (%d ms) reading file block", timeout_ms);
	    br = 0;
	    break;
	 }

	 qore_offset_t rc;
	 while (true) {
	    rc = ::read(fd, buf, bs);
	    // try again if we were interrupted by a signal
	    if (rc >= 0 || errno != EINTR)
	       break;
	 }
	 //printd(0, "read(%d, %p, %d) rc=%d\n", fd, buf, bs, rc);
	 if (rc <= 0)
	    break;

	 // enlarge bbuf (ensure buffer is 1 byte bigger than needed)
	 bbuf = (char *)realloc(bbuf, br + rc + 1);
	 // append buffer to bbuf
	 memcpy(bbuf + br, buf, rc);
	 br += rc;

	 do_read_event(rc, br, size);
      
	 if (size > 0) {
	    if (size - br < bs)
	       bs = size - br;
	    if (br >= (qore_size_t)size)
	       break;
	 }
      }
      free(buf);
      if (!br) {
	 if (bbuf)
	    free(bbuf);
	 return 0;
      }
      size = br;
      return bbuf;
   }

   DLLLOCAL void setEventQueue(Queue *cbq, ExceptionSink *xsink) {
      if (cb_queue)
	 cb_queue->deref(xsink);
      cb_queue = cbq;
   }

   DLLLOCAL void cleanup(ExceptionSink *xsink) {
      if (cb_queue) {
	 
	 // close the file before the delete message is put on the queue
	 // the file would be closed anyway in the destructor
	 close_intern();
	 
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DELETED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->push_and_take_ref(h);
	 
	 // deref and remove event queue
	 cb_queue->deref(xsink);
	 cb_queue = 0;
      }
   }

   DLLLOCAL void do_open_event(const char *fn, int flags, int mode, const QoreEncoding *enc) const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_OPEN_FILE), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("filename", new QoreStringNode(fn), 0);
	 h->setKeyValue("flags", new QoreBigIntNode(flags), 0);
	 h->setKeyValue("mode", new QoreBigIntNode(mode), 0);
	 h->setKeyValue("encoding", new QoreStringNode(enc->getCode()), 0);
	 cb_queue->push_and_take_ref(h);
      }
   }

   DLLLOCAL void do_opened_event(const char *fn, int flags, int mode, const QoreEncoding *enc) const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_FILE_OPENED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("filename", new QoreStringNode(fn), 0);
	 h->setKeyValue("flags", new QoreBigIntNode(flags), 0);
	 h->setKeyValue("mode", new QoreBigIntNode(mode), 0);
	 h->setKeyValue("encoding", new QoreStringNode(enc->getCode()), 0);
	 cb_queue->push_and_take_ref(h);
      }
   }

   DLLLOCAL void do_close_event() const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CHANNEL_CLOSED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->push_and_take_ref(h);
      }
   }

   DLLLOCAL void do_read_event(int bytes_read, int total_read, int bufsize) const {
      // post bytes read on event queue, if any
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DATA_READ), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("read", new QoreBigIntNode(bytes_read), 0);
	 h->setKeyValue("total_read", new QoreBigIntNode(total_read), 0);
	 h->setKeyValue("total_to_read", new QoreBigIntNode(bufsize), 0);
	 cb_queue->push_and_take_ref(h);
      }
   }

   DLLLOCAL void do_write_event(int bytes_written, int total_written, int bufsize) const {
      // post bytes sent on event queue, if any
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DATA_WRITTEN), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("written", new QoreBigIntNode(bytes_written), 0);
	 h->setKeyValue("total_written", new QoreBigIntNode(total_written), 0);
	 h->setKeyValue("total_to_write", new QoreBigIntNode(bufsize), 0);
	 cb_queue->push_and_take_ref(h);
      }
   }
};

QoreFile::QoreFile(const QoreEncoding *cs) : priv(new qore_qf_private(cs)) {
}

QoreFile::~QoreFile() {
   delete priv;
}

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
   if (rc)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl() failed: %s", strerror(errno));
   return rc;
}

//! perform a file lock operation, does not block
int QoreFile::lock(const struct flock &fl, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_SETLK, &fl);
   if (rc && rc != EACCES)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl() failed: %s", strerror(errno));

   return rc;
}

//! get lock info operation, does not block
int QoreFile::getLockInfo(struct flock &fl, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-LOCK-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_GETLK, &fl);
   if (rc)
      xsink->raiseException("FILE-LOCK-ERROR", "the call to fcntl(F_GETLK) failed: %s", strerror(errno));
   return rc;
}

int QoreFile::chown(uid_t owner, gid_t group, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-CHOWN-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fchown(priv->fd, owner, group);
   if (rc)
      xsink->raiseException("FILE-CHOWN-ERROR", "the chown(%d, %d) operation failed: %s", owner, group, strerror(errno));

   return rc;
}

#if 0
int QoreFile::preallocate(fstore_t &fs, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-PREALLOCATE-ERROR", "the file has not been opened");
      return -1;
   }

   int rc = fcntl(priv->fd, F_PREALLOCATE, &fs);
   if (rc)
      xsink->raiseException("FILE-PREALLOCATE-ERROR", "the call to fcntl(F_PREALLOCATE) failed (%d bytes allocated): %s", fs.fst_bytesalloc, strerror(errno));
   return rc;
}
#endif

QoreStringNode *QoreFile::getFileName() const { 
   AutoLocker al(priv->m);

   return priv->filename.empty() ? 0 : new QoreStringNode(priv->filename.c_str()); 
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
      xsink->raiseException("FILE-OPEN2-ERROR", "cannot open '%s': %s", fn, strerror(errno));
      return -1;
   }

   return 0;
}

QoreStringNode *QoreFile::readLine(ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (!priv->is_open) {
      xsink->raiseException("FILE-READLINE-ERROR", "file has not been opened");
      return 0;
   }
   
   int ch;

   QoreStringNodeHolder str(new QoreStringNode(priv->charset));
   while ((ch = priv->readChar()) >= 0) {
      char c = ch;
      str->concat((char)c);
      if (c == '\n')
	 break;
   }
   if (!str->strlen())
      return 0;

   return str.release();
}

qore_size_t QoreFile::setPos(qore_size_t pos) {
   AutoLocker al(priv->m);

   if (!priv->is_open)
      return -1;
   
   return lseek(priv->fd, pos, SEEK_SET);
}

qore_size_t QoreFile::getPos() {
   AutoLocker al(priv->m);

   if (!priv->is_open)
      return -1;
   
   return lseek(priv->fd, 0, SEEK_CUR);
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

   QoreStringNode *str = new QoreStringNode(priv->charset);
   str->concat((char)c);
   return str;
}

int QoreFile::write(const void *data, qore_size_t len, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   if (!len)
      return 0;
   
   return priv->write(data, len);
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
   
   return priv->write(wstr->getBuffer(), wstr->strlen());
}

int QoreFile::write(const BinaryNode *b, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   if (!b)
      return 0;
   
   return priv->write(b->getPtr(), b->size());
}

QoreStringNode *QoreFile::read(qore_offset_t size, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);
   
      if (priv->check_read_open(xsink))
	 return 0;
   
      buf = priv->readBlock(size, -1, xsink);
   }
   if (!buf)
      return 0;

   QoreStringNode *str = new QoreStringNode(buf, size, size, priv->charset);
   //str->terminate(buf[size - 1] ? size : size - 1);
   str->terminate(size);
   return str;
}

BinaryNode *QoreFile::readBinary(qore_offset_t size, ExceptionSink *xsink) {
   if (!size)
      return 0;

   char *buf;
   {
      AutoLocker al(priv->m);

      if (priv->check_read_open(xsink))
	 return 0;
   
      buf = priv->readBlock(size, -1, xsink);
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
   
      buf = priv->readBlock(size, timeout_ms, xsink);
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
   
      buf = priv->readBlock(size, timeout_ms, xsink);
   }
   if (!buf)
      return 0;
   
   return new BinaryNode(buf, size);
}

int QoreFile::writei1(char i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   return priv->write((char *)&i, 1);   
}

int QoreFile::writei2(short i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;

   i = htons(i);
   return priv->write((char *)&i, 2);   
}

int QoreFile::writei4(int i, ExceptionSink *xsink) {
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   i = htonl(i);
   return priv->write((char *)&i, 4);
}

int QoreFile::writei8(int64 i, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   i = i8MSB(i);
   return priv->write((char *)&i, 4);
}

int QoreFile::writei2LSB(short i, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   i = i2LSB(i);
   return priv->write((char *)&i, 2);   
}

int QoreFile::writei4LSB(int i, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   i = i4LSB(i);
   return priv->write((char *)&i, 4);
}

int QoreFile::writei8LSB(int64 i, ExceptionSink *xsink)
{
   AutoLocker al(priv->m);

   if (priv->check_write_open(xsink))
      return -1;
   
   i = i8LSB(i);
   return priv->write((char *)&i, 4);
}

int QoreFile::readu1(unsigned char *val, ExceptionSink *xsink)
{
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

bool QoreFile::isDataAvailable(int timeout_ms, ExceptionSink *xsink) const {
   return priv->isDataAvailable(timeout_ms, xsink);
}

int QoreFile::getFD() const {
   return priv->fd;
}

int QoreFile::setTerminalAttributes(int action, QoreTermIOS *ios, ExceptionSink *xsink) const {
   return priv->setTerminalAttributes(action, ios, xsink);
}

int QoreFile::getTerminalAttributes(QoreTermIOS *ios, ExceptionSink *xsink) const {
   return priv->getTerminalAttributes(ios, xsink);
}

void QoreFile::setEventQueue(Queue *cbq, ExceptionSink *xsink) {
   priv->setEventQueue(cbq, xsink);
}

void QoreFile::cleanup(ExceptionSink *xsink) {
   priv->cleanup(xsink);
}
