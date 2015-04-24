/*
  qore_qf_private.h

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

#ifndef _QORE_INTERN_QORE_QF_PRIVATE_H
#define _QORE_INTERN_QORE_QF_PRIVATE_H

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
   const QoreEncoding* charset;
   std::string filename;
   mutable QoreThreadLock m;
   Queue* cb_queue;

   DLLLOCAL qore_qf_private(const QoreEncoding* cs) : is_open(false),
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

   DLLLOCAL int open_intern(const char* fn, int flags, int mode, const QoreEncoding* cs) {
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

   DLLLOCAL int open(const char* fn, int flags, int mode, const QoreEncoding* cs) {
      if (!fn || special_file)
	 return -1;

      AutoLocker al(m);
      return open_intern(fn, flags, mode, cs);
   }

   // returns -1 for exception
   DLLLOCAL int check_read_open(ExceptionSink* xsink) const {
      if (is_open)
	 return 0;
   
      xsink->raiseException("FILE-READ-ERROR", "file has not been opened");
      return -1;
   }

   // returns -1 for exception
   DLLLOCAL int check_write_open(ExceptionSink* xsink) const {
      if (is_open)
	 return 0;
      
      xsink->raiseException("FILE-WRITE-ERROR", "file has not been opened");
      return -1;
   }

   // returns -1 for exception
   DLLLOCAL int check_open(ExceptionSink* xsink) const {
      if (is_open)
	 return 0;
      
      xsink->raiseException("FILE-OPERATION-ERROR", "file has not been opened");
      return -1;
   }

   DLLLOCAL bool isOpen() const {
      return is_open;
   }

   DLLLOCAL bool isDataAvailable(int timeout_ms, ExceptionSink* xsink) const {
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
   DLLLOCAL int setTerminalAttributes(int action, QoreTermIOS* ios, ExceptionSink* xsink) const {
      AutoLocker al(m);
      
      if (check_open(xsink))
	 return -1;

      return ios->set(fd, action, xsink);
   }

   DLLLOCAL int getTerminalAttributes(QoreTermIOS* ios, ExceptionSink* xsink) const {
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

   // private function, unlocked
   DLLLOCAL int readUnicode(int* n_len = 0) const {
      char buf[charset->getMaxCharWidth()];
      if (read(buf, 1) != 1)
	 return -1;

      int len = (int)charset->getCharLen(buf, 1);
      if (len < 0) {
	 len = -len;
	 for (int i = 1; i < len; ++i) {
	    if (read(&buf[i], 1) != 1)
	       return -1;
	 }
      }

      if (n_len)
	 *n_len = len;
      
      return charset->getUnicode(buf);
   }

   DLLLOCAL char* readBlock(qore_offset_t &size, int timeout_ms, ExceptionSink* xsink) {
      qore_size_t bs = size > 0 && size < DEFAULT_FILE_BUFSIZE ? size : DEFAULT_FILE_BUFSIZE;
      qore_size_t br = 0;
      char* buf = (char* )malloc(sizeof(char) * bs);
      char* bbuf = 0;

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
	 bbuf = (char* )realloc(bbuf, br + rc + 1);
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

   DLLLOCAL int readLineUnicode(QoreString& str, bool incl_eol = true) {
      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      bool tty = (bool)isatty(fd);

      int ch, rc = -1;

      while ((ch = readUnicode()) >= 0) {
	 // skip BOM
	 if (ch == 0xfeff)
	    continue;
	 else if (ch == 0xfffe && charset == QCS_UTF16 && str.empty()) {
	    charset = QCS_UTF16LE;
	    continue;
	 }
	 
	 str.concatUnicode(ch);
	 
         if (rc == -1)
	    rc = 0;

	 char c = str[str.size() - 1];
	 
         if (c == '\r') {
            // see if next byte is \n' if we're not connected to a terminal device
            if (!tty) {
	       int len = 0;
               ch = readUnicode(&len);
               if (ch >= 0) {
                  if (ch == '\n') {
                     if (incl_eol)
                        str.concatUnicode(ch);
                  }
                  else {
                     // reset file to previous byte position
                     lseek(fd, -len, SEEK_CUR);
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
   
   DLLLOCAL int readUntilUnicode(char byte, QoreString& str, bool incl_byte = true) {
      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      int ch, rc = -1;

      while ((ch = readUnicode()) >= 0) {
	 // skip BOM
	 if (ch == 0xfeff)
	    continue;
	 else if (ch == 0xfffe && charset == QCS_UTF16 && str.empty()) {
	    charset = QCS_UTF16LE;
	    continue;
	 }

	 str.concatUnicode(ch);
	 
         if (rc == -1)
            rc = 0;
         if (ch == byte) {
            if (!incl_byte)
               str.terminate(str.strlen() - 1);
            break;
         }
      }

      return rc;
   }
   DLLLOCAL int readUntilUnicode(const char* bytes, QoreString& str, bool incl_bytes) {
      str.clear();

      AutoLocker al(m);

      if (!is_open)
         return -2;

      // offset in bytes
      unsigned pos = 0;

      int ch, rc = -1;

      while ((ch = readUnicode()) >= 0) {
	 // skip BOM
	 if (ch == 0xfeff)
	    continue;
	 else if (ch == 0xfffe && charset == QCS_UTF16 && str.empty()) {
	    charset = QCS_UTF16LE;
	    continue;
	 }

	 str.concatUnicode(ch);

	 if (rc == -1)
            rc = 0;

         if (ch == bytes[pos]) {
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

   DLLLOCAL void setEventQueue(Queue* cbq, ExceptionSink* xsink) {
      AutoLocker al(m);
      if (cb_queue)
	 cb_queue->deref(xsink);
      cb_queue = cbq;
   }

   DLLLOCAL void cleanup(ExceptionSink* xsink) {
      AutoLocker al(m);
      if (cb_queue) {
	 // close the file before the delete message is put on the queue
	 // the file would be closed anyway in the destructor
	 close_intern();
	 
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DELETED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
	 
	 // deref and remove event queue
	 cb_queue->deref(xsink);
	 cb_queue = 0;
      }
   }

   DLLLOCAL void do_open_event_unlocked(const char* fn, int flags, int mode, const QoreEncoding* enc) const {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
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

   DLLLOCAL void do_opened_event_unlocked(const char* fn, int flags, int mode, const QoreEncoding* enc) const {
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
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
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_CHANNEL_CLOSED), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL void do_read_event_unlocked(int bytes_read, int total_read, int bufsize) const {
      // post bytes read on event queue, if any
      if (cb_queue) {
	 QoreHashNode* h = new QoreHashNode;
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
	 QoreHashNode* h = new QoreHashNode;
	 h->setKeyValue("event", new QoreBigIntNode(QORE_EVENT_DATA_WRITTEN), 0);
	 h->setKeyValue("source", new QoreBigIntNode(QORE_SOURCE_FILE), 0);
	 h->setKeyValue("id", new QoreBigIntNode((int64)this), 0);
	 h->setKeyValue("written", new QoreBigIntNode(bytes_written), 0);
	 h->setKeyValue("total_written", new QoreBigIntNode(total_written), 0);
	 h->setKeyValue("total_to_write", new QoreBigIntNode(bufsize), 0);
	 cb_queue->pushAndTakeRef(h);
      }
   }

   DLLLOCAL QoreListNode* stat(ExceptionSink* xsink) const {
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

   DLLLOCAL QoreHashNode* hstat(ExceptionSink* xsink) const {
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
   DLLLOCAL QoreHashNode* statvfs(ExceptionSink* xsink) const {
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

#endif
