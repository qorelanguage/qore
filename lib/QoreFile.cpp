/*
  QoreFile.cpp

  Network functions

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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
#include <qore/intern/QC_Queue.h>
#ifdef HAVE_TERMIOS_H
#include <qore/intern/QC_TermIOS.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/file.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include <string>

#ifndef DEFAULT_FILE_BUFSIZE
#define DEFAULT_FILE_BUFSIZE 16384
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
	    do_close_event_unlocked();
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

#ifdef _Q_WINDOWS
      // open files in binary mode by default on Windows
      if (!(flags & O_TEXT))
	 flags |= O_BINARY;
#endif

      do_open_event_unlocked(fn, flags, mode, cs);

      fd = ::open(fn, flags, mode);
      if (fd < 0)
	 return fd;

      do_opened_event_unlocked(fn, flags, mode, cs);

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

   DLLLOCAL bool isOpen() const {
      return is_open;
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

#ifdef HAVE_TERMIOS_H
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
#endif

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
	 do_read_event_unlocked(rc, rc, bs);

      return rc;
   }

   // unlocked, assumes file is open
   DLLLOCAL qore_size_t write(const void* buf, qore_size_t len, ExceptionSink* xsink = 0) const {
      qore_offset_t rc;
      while (true) {
	 rc = ::write(fd, buf, len);
	 // try again if we are interrupted by a signal
	 if (rc >= 0 || errno != EINTR)
	    break;
      }

      if (rc > 0)
	 do_write_event_unlocked(rc, rc, len);
      else if (xsink && rc < 0)
         xsink->raiseErrnoException("FILE-WRITE-ERROR", errno, "failed writing "QLLD" byte%s to File", len, len == 1 ? "" : "s");

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
	 //printd(5, "readBlock(fd: %d, buf: %p, bs: %d) rc: %d\n", fd, buf, bs, rc);
	 if (rc <= 0)
	    break;

	 // enlarge bbuf (ensure buffer is 1 byte bigger than needed)
	 bbuf = (char *)realloc(bbuf, br + rc + 1);
	 // append buffer to bbuf
	 memcpy(bbuf + br, buf, rc);
	 br += rc;

	 do_read_event_unlocked(rc, br, size);
      
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

   DLLLOCAL QoreStringNode* readLine(bool incl_eol, ExceptionSink* xsink) {
      QoreStringNodeHolder str(new QoreStringNode(charset));

      int rc = readLine(**str, incl_eol);

      if (rc == -2) {
         xsink->raiseException("FILE-READLINE-ERROR", "file has not been opened");
         return 0;
      }

      return rc == -1 ? 0 : str.release();
   }

   DLLLOCAL int readLine(QoreString& str, bool incl_eol = true) {
      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      bool tty = (bool)isatty(fd);

      int ch, rc = -1;

      while ((ch = readChar()) >= 0) {
         str.concat((char)ch);
         if (rc == -1)
            rc = 0;

         if (ch == '\r') {
            // see if next byte is \n' if we're not connected to a terminal device
            if (!tty) {
               ch = readChar();
               if (ch >= 0) {
                  if (ch == '\n') {
                     if (incl_eol)
                        str.concat((char)ch);
                  }
                  else {
                     // reset file to previous byte position
                     lseek(fd, -1, SEEK_CUR);
                  }
               }
            }
            if (!incl_eol)
               str.terminate(str.strlen() - 1);
            break;
         }

         if (ch == '\n') {
            if (!incl_eol)
               str.terminate(str.strlen() - 1);
            break;
         }
      }

      return rc;
   }

   DLLLOCAL int readUntil(char byte, QoreString& str, bool incl_byte = true) {
      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      int ch, rc = -1;

      while ((ch = readChar()) >= 0) {
         char c = ch;
         str.concat(c);
         if (rc == -1)
            rc = 0;
         if (c == byte) {
            if (!incl_byte)
               str.terminate(str.strlen() - 1);
            break;
         }
      }

      return rc;
   }

   DLLLOCAL QoreStringNode* readUntil(const char* bytes, bool incl_bytes, ExceptionSink* xsink) {
      QoreStringNodeHolder str(new QoreStringNode(charset));

      int rc = readUntil(bytes, **str, incl_bytes);

      if (rc == -2) {
         xsink->raiseException("FILE-READLINE-ERROR", "file has not been opened");
         return 0;
      }

      return rc == -1 ? 0 : str.release();
   }

   // not the most efficient search algorithm, restarts the search the byte after it fails for multi-byte patterns
   DLLLOCAL int readUntil(const char* bytes, QoreString& str, bool incl_bytes) {
      if (!bytes[1])
         return readUntil(bytes[0], str, incl_bytes);

      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      // offset in bytes
      unsigned pos = 0;

      int ch, rc = -1;

      while ((ch = readChar()) >= 0) {
         char c = ch;
         str.concat(c);
         if (rc == -1)
            rc = 0;

         if (c == bytes[pos]) {
            ++pos;
            if (!bytes[pos]) {
               if (!incl_bytes)
                  str.terminate(str.strlen() - pos);
               break;
            }
         }
         else if (pos) {
            // bytes=aaac read=aaaac str=aaa pos=3
            //          ^         ^
            // restart search with characters already added to the string if more than 1 character was matched previously
            if (pos > 1) {
               unsigned ps = 1;
               while (ps < pos) {
                  if (!strncmp(str.getBuffer() + ps, bytes, pos - ps)) {
                     pos -= ps;
                     break;
                  }
                  ++ps;
               }
               if (pos == ps)
                  pos = 0;
            }
            else {
               // restart search if failed
               pos = 0;
            }
         }
      }

      return rc;
   }

   DLLLOCAL bool isTty() const {
      AutoLocker al(m);

      if (!is_open)
         return false;

      return (bool)isatty(fd);
   }

   DLLLOCAL qore_size_t getPos() const {
      AutoLocker al(m);

      if (!is_open)
         return -1;

      return lseek(fd, 0, SEEK_CUR);
   }

   DLLLOCAL void setEventQueue(Queue *cbq, ExceptionSink *xsink) {
      AutoLocker al(m);
      if (cb_queue)
	 cb_queue->deref(xsink);
      cb_queue = cbq;
   }

   DLLLOCAL void cleanup(ExceptionSink *xsink) {
      AutoLocker al(m);
      if (cb_queue) {
	 // close the file before the delete message is put on the queue
	 // the file would be closed anyway in the destructor
	 close_intern();
	 
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DELETED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
	 
	 // deref and remove event queue
	 cb_queue->deref(xsink);
	 cb_queue = 0;
      }
   }

   DLLLOCAL void do_open_event_unlocked(const char *fn, int flags, int mode, const QoreEncoding *enc) const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_OPEN_FILE), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("filename", new QoreStringNode(fn), 0);
	 h->setKeyValue("flags", new QoreBigIntNode(flags), 0);
	 h->setKeyValue("mode", new QoreBigIntNode(mode), 0);
	 h->setKeyValue("encoding", new QoreStringNode(enc->getCode()), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_opened_event_unlocked(const char *fn, int flags, int mode, const QoreEncoding *enc) const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_FILE_OPENED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("filename", new QoreStringNode(fn), 0);
	 h->setKeyValue("flags", new QoreBigIntNode(flags), 0);
	 h->setKeyValue("mode", new QoreBigIntNode(mode), 0);
	 h->setKeyValue("encoding", new QoreStringNode(enc->getCode()), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_close_event_unlocked() const {
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CHANNEL_CLOSED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_read_event_unlocked(int bytes_read, int total_read, int bufsize) const {
      // post bytes read on event queue, if any
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DATA_READ), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("read", new QoreBigIntNode(bytes_read), 0);
	 h->setKeyValue("total_read", new QoreBigIntNode(total_read), 0);
	 h->setKeyValue("total_to_read", new QoreBigIntNode(bufsize), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_write_event_unlocked(int bytes_written, int total_written, int bufsize) const {
      // post bytes sent on event queue, if any
      if (cb_queue) {
	 QoreHashNode *h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DATA_WRITTEN), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("written", new QoreBigIntNode(bytes_written), 0);
	 h->setKeyValue("total_written", new QoreBigIntNode(total_written), 0);
	 h->setKeyValue("total_to_write", new QoreBigIntNode(bufsize), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL QoreListNode *stat(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (check_read_open(xsink))
	 return 0;
   
      struct stat sbuf;
      if (fstat(fd, &sbuf)) {
	 xsink->raiseErrnoException("FILE-STAT-ERROR", errno, "fstat() call failed");
	 return 0;
      }

      return stat_to_list(sbuf);
   }

   DLLLOCAL QoreHashNode *hstat(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (check_read_open(xsink))
	 return 0;
   
      struct stat sbuf;
      if (fstat(fd, &sbuf)) {
	 xsink->raiseErrnoException("FILE-HSTAT-ERROR", errno, "fstat() call failed");
	 return 0;
      }

      return stat_to_hash(sbuf);
   }

#ifdef HAVE_SYS_STATVFS_H
   DLLLOCAL QoreHashNode *statvfs(ExceptionSink *xsink) const {
      AutoLocker al(m);

      if (check_read_open(xsink))
	 return 0;
   
      struct statvfs vfs;
      if (fstatvfs(fd, &vfs)) {
	 xsink->raiseErrnoException("FILE-STATVFS-ERROR", errno, "fstatvfs() call failed");
	 return 0;
      }

      return statvfs_to_hash(vfs);
   }
#endif
};

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
      qore_size_t rc = priv->charset->getCharLen(str->getBuffer(), 1);
      // rc == 0: invalid character; but we can't throw an exception here - anyway I think this can't happen with UTF-8 currently
      //          which is the only multi-byte encoding we currently support
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

      buf = priv->readBlock(size, -1, xsink);
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
   
      buf = priv->readBlock(size, -1, xsink);
   }
   if (!buf)
      return 0;

   QoreStringNode *str = new QoreStringNode(buf, size, size, priv->charset);
   //str->terminate(buf[size - 1] ? size : size - 1);
   str->terminate(size);
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

      buf = priv->readBlock(size, -1, xsink);
   }
   if (!buf)
      return -1;

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

#ifdef HAVE_SYS_STATVFS_H
QoreHashNode *QoreFile::statvfs(ExceptionSink *xsink) const {
   return priv->statvfs(xsink);
}
#endif

bool QoreFile::isTty() const {
   return priv->isTty();
}
