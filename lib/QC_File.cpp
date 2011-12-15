/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_File.qpp

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
#include <qore/intern/QC_File.h>
#include <qore/intern/QC_Queue.h>
#ifdef HAVE_TERMIOS_H
#include <qore/intern/QC_TermIOS.h>
#endif

#ifndef TCSANOW
#define Q_TCSANOW 0
#else
#define Q_TCSANOW TCSANOW
#endif

static int check_terminal_io(QoreObject *self, const char *m, ExceptionSink *xsink) {
   // check for no-terminal-io at runtime with system objecs
   if (self->isSystemObject() && (getProgram()->getParseOptions64() & PO_NO_TERMINAL_IO)) {
      xsink->raiseException("ILLEGAL-EXPRESSION", "File::%s() cannot be called with a system constant object when 'no-terminal-io' is set", m);
      return -1;
   }
   return 0;
}

static void FILE_system_constructor(QoreObject *self, int fd, va_list args) {
   //printd(5, "FILE_system_constructor() self=%08p, descriptor=%d\n", self, fd);
   File *f = new File(QCS_DEFAULT);
   f->makeSpecial(fd);
   self->setPrivate(CID_FILE, f);
}

#ifdef HAVE_STRUCT_FLOCK
static int lock_intern(struct flock &fl, const QoreListNode *args, ExceptionSink *xsink) {
   fl.l_type = (short)HARD_QORE_INT(args, 0);
   fl.l_start = (off_t)HARD_QORE_INT(args, 1);
   fl.l_len = (off_t)HARD_QORE_INT(args, 2);
   if (fl.l_len < 0) {
      xsink->raiseException("FILE-LOCK-ERROR", "length of locked area cannot be negative (value passed=%d)", fl.l_len);
      return -1;
   }
   fl.l_whence = (short)HARD_QORE_INT(args, 3);
   return 0;
}
#else
static int file_lock_error(const char *f, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "this platform does not support UNIX-style file locking with fnctl(), therefore the File::%s() method is not available; for maximum portability, check Option::HAVE_FILE_LOCKING before calling this method", f);
   return 0;
}
#ifndef F_RDLCK
#define F_RDLCK -1
#endif
#endif

AbstractQoreNode *missing_method_error(const char *meth, const char *feat, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "the %s() method is not available on this build; for maximum portability, check Option::HAVE_%s before calling this method", meth, feat);
   return 0;
}

/* Qore class File */

qore_classid_t CID_FILE;
QoreClass *QC_FILE;

// nothing File::chown(softint uid, softint gid = -1) {}
static AbstractQoreNode* File_chown(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 uid = HARD_QORE_INT(args, 0);
   int64 gid = HARD_QORE_INT(args, 1);
#ifdef HAVE_CHOWN
   if (check_terminal_io(self, "chown", xsink))
      return 0;

   f->chown( (uid_t)uid, (gid_t)gid, xsink);
   return 0;
#else
   return missing_method_error("File::chown", "UNIX_FILEMGT", xsink);
#endif
   return 0;
}

// int File::close() {}
static int64 File_close(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "close", xsink))
      return 0;

   return f->close();
}

// File::constructor(*string encoding) {}
static void File_constructor(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* encoding = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(0));
   const QoreEncoding *qe = encoding ? QEM.findCreate(encoding) : QCS_DEFAULT;
   self->setPrivate(CID_FILE, new File(qe));
}

// File::copy() {}
static void File_copy(QoreObject* self, QoreObject* old, File* f, ExceptionSink* xsink) {
   self->setPrivate(CID_FILE, new File(f->getEncoding()));
}

// int File::f_printf(string fmt, ...) {}
static int64 File_f_printf(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* fmt = HARD_QORE_STRING(args, 0);
   if (check_terminal_io(self, "f_printf", xsink))
      return 0;

   QoreStringNodeHolder str(q_sprintf(args, 1, 0, xsink));
   if (!str)
      return 0;

   return f->write(*str, xsink);
}

// int File::f_vprintf(string fmt, any fmt_args) {}
static int64 File_f_vprintf(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* fmt = HARD_QORE_STRING(args, 0);
   const AbstractQoreNode* fmt_args = get_param(args, 1);
   if (check_terminal_io(self, "f_vprintf", xsink))
      return 0;

   QoreStringNodeHolder str(q_vsprintf(args, 1, 0, xsink));
   if (!str)
      return 0;

   return f->write(*str, xsink);
}

// string File::getCharset() {}
static AbstractQoreNode* File_getCharset(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   return new QoreStringNode(f->getEncoding()->getCode());
}

// string File::getEncoding() {}
static AbstractQoreNode* File_getEncoding(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   return new QoreStringNode(f->getEncoding()->getCode());
}

// hash File::getLockInfo() {}
static AbstractQoreNode* File_getLockInfo(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
#ifdef HAVE_STRUCT_FLOCK
   if (check_terminal_io(self, "getLockInfo", xsink))
      return 0;

   struct flock fl;
   if (f->getLockInfo(fl, xsink))
      return 0;

   QoreHashNode *h = new QoreHashNode;
   h->setKeyValue("start", new QoreBigIntNode(fl.l_start), xsink);
   h->setKeyValue("len", new QoreBigIntNode(fl.l_len), xsink);
   h->setKeyValue("pid", new QoreBigIntNode(fl.l_pid), xsink);
   h->setKeyValue("type", new QoreBigIntNode(fl.l_type), xsink);
   h->setKeyValue("whence", new QoreBigIntNode(fl.l_whence), xsink);

   return h;
#else
   return file_lock_error("getLockInfo", xsink);
#endif
   return 0;
}

// int File::getPos() {}
static int64 File_getPos(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "getPos", xsink))
      return 0;

   return f->getPos();
}

// nothing File::getTerminalAttributes(TermIOS termios) {}
static AbstractQoreNode* File_getTerminalAttributes(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   HARD_QORE_OBJ_DATA(termios, QoreTermIOS, args, 0, CID_TERMIOS, "File::getTerminalAttributes()", "TermIOS", xsink);
   if (*xsink)
      return 0;
#ifdef HAVE_TERMIOS_H
   if (check_terminal_io(self, "getTerminalAttributes", xsink))
      return 0;

   ReferenceHolder<QoreTermIOS> holder(termios, xsink);
   f->getTerminalAttributes(termios, xsink);
   return 0;
#else
   return missing_method_error("File::getTerminalAttributes", "TERMIOS", xsink);
#endif
   return 0;
}

// *string File::getchar() {}
static AbstractQoreNode* File_getchar(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "getchar", xsink))
      return 0;

   return f->getchar(xsink);
}

// hash File::hstat() {}
static AbstractQoreNode* File_hstat(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "hstat", xsink))
      return 0;

   return f->hstat(xsink);
}

// bool File::isDataAvailable(timeout timeout_ms = 0) {}
static bool File_isDataAvailable(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 timeout_ms = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "isDataAvailable", xsink))
      return 0;

   return f->isDataAvailable(timeout_ms, xsink);
}

// int File::lock(softint type = F_RDLCK, softint start = 0, softint len = 0, softint whence = SEEK_SET) {}
static int64 File_lock(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 type = HARD_QORE_INT(args, 0);
   int64 start = HARD_QORE_INT(args, 1);
   int64 len = HARD_QORE_INT(args, 2);
   int64 whence = HARD_QORE_INT(args, 3);
#ifdef HAVE_STRUCT_FLOCK
   if (check_terminal_io(self, "lock", xsink))
      return 0;

   struct flock fl;
   if (lock_intern(fl, args, xsink))
      return 0;
   return f->lock(fl, xsink);
#else
   return file_lock_error("lock", xsink);
#endif
   return 0;
}

// nothing File::lockBlocking(softint type = F_RDLCK, softint start = 0, softint len = 0, softint whence = SEEK_SET) {}
static AbstractQoreNode* File_lockBlocking(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 type = HARD_QORE_INT(args, 0);
   int64 start = HARD_QORE_INT(args, 1);
   int64 len = HARD_QORE_INT(args, 2);
   int64 whence = HARD_QORE_INT(args, 3);
#ifdef HAVE_STRUCT_FLOCK
   if (check_terminal_io(self, "lockBlocking", xsink))
      return 0;

   struct flock fl;
   if (lock_intern(fl, args, xsink))
      return 0;

   f->lockBlocking(fl, xsink);
   return 0;
#else
   return file_lock_error("lockBlocking", xsink);
#endif
   return 0;
}

// int File::open(string filename, softint flags = O_RDONLY, softint mode = 0666, *string encoding) {}
static int64 File_open(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* filename = HARD_QORE_STRING(args, 0);
   int64 flags = HARD_QORE_INT(args, 1);
   int64 mode = HARD_QORE_INT(args, 2);
   const QoreStringNode* encoding = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(3));
   if (check_terminal_io(self, "open", xsink))
      return 0;

   const QoreEncoding *qe = encoding ? QEM.findCreate(encoding) : QCS_DEFAULT;
   return f->open(filename->getBuffer(), flags, mode, qe);
}

// nothing File::open2(string filename, softint flags = O_RDONLY, softint mode = 0666, *string encoding) {}
static AbstractQoreNode* File_open2(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* filename = HARD_QORE_STRING(args, 0);
   int64 flags = HARD_QORE_INT(args, 1);
   int64 mode = HARD_QORE_INT(args, 2);
   const QoreStringNode* encoding = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(3));
   if (check_terminal_io(self, "open2", xsink))
      return 0;

   const QoreEncoding *qe = encoding ? QEM.findCreate(encoding) : QCS_DEFAULT;
   f->open2(xsink, filename->getBuffer(), flags, mode, qe);
   return 0;
}

// int File::printf(string fmt, ...) {}
static int64 File_printf(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* fmt = HARD_QORE_STRING(args, 0);
   if (check_terminal_io(self, "printf", xsink))
      return 0;

   QoreStringNodeHolder str(q_sprintf(args, 0, 0, xsink));
   if (!str)
      return 0;

   return f->write(*str, xsink);
}

// int File::printf() {}
static int64 File_printf_1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   return 0;
}

// *string File::read(softint size, timeout timeout_ms = -1) {}
static AbstractQoreNode* File_read(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 size = HARD_QORE_INT(args, 0);
   int64 timeout_ms = HARD_QORE_INT(args, 1);
   if (check_terminal_io(self, "read", xsink))
      return 0;

   if (!size) {
      xsink->raiseException("FILE-READ-PARAMETER-ERROR", "expecting non-zero size as first parameter of File::read()");
      return 0;
   }

   return f->read((qore_offset_t)size, timeout_ms, xsink);
}

// *binary File::readBinary(softint size, timeout timeout_ms = -1) {}
static AbstractQoreNode* File_readBinary(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 size = HARD_QORE_INT(args, 0);
   int64 timeout_ms = HARD_QORE_INT(args, 1);
   if (check_terminal_io(self, "readBinary", xsink))
      return 0;

   if (!size) {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return 0;
   }

   return f->readBinary((qore_offset_t)size, timeout_ms, xsink);
}

// *string File::readLine() {}
static AbstractQoreNode* File_readLine(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readLine", xsink))
      return 0;

   return f->readLine(xsink);
}

// *int File::readi1() {}
static AbstractQoreNode* File_readi1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi1", xsink))
      return 0;

   char c;
   if (f->readi1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

// *int File::readi2() {}
static AbstractQoreNode* File_readi2(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi2", xsink))
      return 0;

   short s;
   if (f->readi2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

// *int File::readi2LSB() {}
static AbstractQoreNode* File_readi2LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi2LSB", xsink))
      return 0;

   short s;
   if (f->readi2LSB(&s, xsink))
      return 0;

   return new QoreBigIntNode(s);
}

// *int File::readi4() {}
static AbstractQoreNode* File_readi4(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi4", xsink))
      return 0;

   int i;
   if (f->readi4(&i, xsink))
      return 0;
   return new QoreBigIntNode(i);
}

// *int File::readi4LSB() {}
static AbstractQoreNode* File_readi4LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi4LSB", xsink))
      return 0;

   int i;
   if (f->readi4LSB(&i, xsink))
      return 0;
   return new QoreBigIntNode(i);
}

// *int File::readi4LSB() {}
static AbstractQoreNode* File_readi4LSB_1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi8LSB", xsink))
      return 0;

   int64 i;
   if (f->readi8LSB(&i, xsink))
      return 0;   
   return new QoreBigIntNode(i);
}

// *int File::readi8() {}
static AbstractQoreNode* File_readi8(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readi8", xsink))
      return 0;

   int64 i;
   if (f->readi8(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

// *int File::readu1() {}
static AbstractQoreNode* File_readu1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readu1", xsink))
      return 0;

   unsigned char c;
   if (f->readu1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

// *int File::readu2() {}
static AbstractQoreNode* File_readu2(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readu2", xsink))
      return 0;

   unsigned short s;
   if (f->readu2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

// *int File::readu2LSB() {}
static AbstractQoreNode* File_readu2LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readu2LSB", xsink))
      return 0;

   unsigned short s;
   if (f->readu2LSB(&s, xsink))
      return 0;
   
   return new QoreBigIntNode(s);
}

// *int File::readu4() {}
static AbstractQoreNode* File_readu4(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readu4", xsink))
      return 0;

   unsigned int i;
   if (f->readu4(&i, xsink))
      return 0;
   return new QoreBigIntNode(i);
}

// *int File::readu4LSB() {}
static AbstractQoreNode* File_readu4LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "readu4LSB", xsink))
      return 0;

   unsigned int i;
   if (f->readu4LSB(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

// nothing File::setCharset(*string encoding) {}
static AbstractQoreNode* File_setCharset(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* encoding = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(0));
   if (check_terminal_io(self, "setCharset", xsink))
      return 0;

   const QoreEncoding *qe = encoding ? QEM.findCreate(encoding) : QCS_DEFAULT;
   f->setEncoding(qe);
   return 0;
}

// nothing File::setEncoding(*string encoding) {}
static AbstractQoreNode* File_setEncoding(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* encoding = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(0));
   if (check_terminal_io(self, "setEncoding", xsink))
      return 0;

   const QoreEncoding *qe = encoding ? QEM.findCreate(encoding) : QCS_DEFAULT;
   f->setEncoding(qe);
   return 0;
}

// nothing File::setEventQueue(Queue queue) {}
static AbstractQoreNode* File_setEventQueue(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   HARD_QORE_OBJ_DATA(queue, Queue, args, 0, CID_QUEUE, "File::setEventQueue()", "Queue", xsink);
   if (*xsink)
      return 0;
   if (check_terminal_io(self, "setEventQueue", xsink))
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to function
   f->setEventQueue(queue, xsink);
   return 0;
}

// nothing File::setEventQueue() {}
static AbstractQoreNode* File_setEventQueue_1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "setEventQueue", xsink))
      return 0;

   f->setEventQueue(0, xsink);
   return 0;
}

// int File::setPos(int pos = 0) {}
static int64 File_setPos(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 pos = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "setPos", xsink))
      return 0;

   return f->setPos(pos);
}

// nothing File::setTerminalAttributes(softint action = TCSANOW, TermIOS termios) {}
static AbstractQoreNode* File_setTerminalAttributes(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 action = HARD_QORE_INT(args, 0);
   HARD_QORE_OBJ_DATA(termios, QoreTermIOS, args, 0, CID_TERMIOS, "File::setTerminalAttributes()", "TermIOS", xsink);
   if (*xsink)
      return 0;
#ifdef HAVE_TERMIOS_H
   if (check_terminal_io(self, "setTerminalAttributes", xsink))
      return 0;

   ReferenceHolder<QoreTermIOS> holder(termios, xsink);
   f->setTerminalAttributes(action, termios, xsink);
   return 0;
#else
   return missing_method_error("File::setTerminalAttributes", "TERMIOS", xsink);
#endif
   return 0;
}

// list File::stat() {}
static AbstractQoreNode* File_stat(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "stat", xsink))
      return 0;

   return f->stat(xsink);
}

// hash File::statvfs() {}
static AbstractQoreNode* File_statvfs(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
#ifdef HAVE_SYS_STATVFS_H
   if (check_terminal_io(self, "statvfs", xsink))
      return 0;

   return f->statvfs(xsink);
#else
   return missing_method_error("File::statvfs", "STATVFS", xsink);
#endif
   return 0;
}

// int File::sync() {}
static int64 File_sync(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   if (check_terminal_io(self, "sync", xsink))
      return 0;

   return f->sync();
}

// int File::vprintf(string fmt, any fmt_args) {}
static int64 File_vprintf(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* fmt = HARD_QORE_STRING(args, 0);
   const AbstractQoreNode* fmt_args = get_param(args, 1);
   if (check_terminal_io(self, "vprintf", xsink))
      return 0;

   QoreStringNodeHolder str(q_vsprintf(args, 0, 0, xsink));
   if (!str)
      return 0;

   return f->write(*str, xsink);
}

// int File::write(binary data) {}
static int64 File_write(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const BinaryNode* data = HARD_QORE_BINARY(args, 0);
   if (check_terminal_io(self, "write", xsink))
      return 0;

   return f->write(data, xsink);
}

// int File::write(string data) {}
static int64 File_write_1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* data = HARD_QORE_STRING(args, 0);
   if (check_terminal_io(self, "write", xsink))
      return 0;

   return f->write(data, xsink);
}

// int File::writei1(int c) {}
static int64 File_writei1(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 c = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei1", xsink))
      return 0;

   return f->writei1(c, xsink);
}

// int File::writei2(int s) {}
static int64 File_writei2(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 s = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei2", xsink))
      return 0;

   return f->writei2(s, xsink);
}

// int File::writei2LSB(int s) {}
static int64 File_writei2LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 s = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei2LSB", xsink))
      return 0;

   return f->writei2LSB(s, xsink);
}

// int File::writei4(int i) {}
static int64 File_writei4(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei4", xsink))
      return 0;

   return f->writei4(i, xsink);
}

// int File::writei4LSB(int i) {}
static int64 File_writei4LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei4LSB", xsink))
      return 0;

   return f->writei4LSB(i, xsink);
}

// int File::writei8(int i) {}
static int64 File_writei8(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei8", xsink))
      return 0;

   return f->writei8(i, xsink);
}

// int File::writei8LSB(int i) {}
static int64 File_writei8LSB(QoreObject* self, File* f, const QoreListNode* args, ExceptionSink* xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   if (check_terminal_io(self, "writei8LSB", xsink))
      return 0;

   return f->writei8LSB(i, xsink);
}

// static hash File::hlstat(string path) {}
static AbstractQoreNode* static_File_hlstat(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* path = HARD_QORE_STRING(args, 0);
   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(path->getBuffer(), &sbuf))
#else
   if (stat(path->getBuffer(), &sbuf))
#endif
   {
      xsink->raiseErrnoException("FILE-HLSTAT-ERROR", errno, "lstat() command failed");
      return 0;
   }

   return stat_to_hash(sbuf);
}

// static hash File::hstat(string path) {}
static AbstractQoreNode* static_File_hstat(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* path = HARD_QORE_STRING(args, 0);
   struct stat sbuf;
   if (stat(path->getBuffer(), &sbuf)) {
      xsink->raiseErrnoException("FILE-HSTAT-ERROR", errno, "stat() command failed");
      return 0;
   }

   return stat_to_hash(sbuf);
}

// static list File::lstat(string path) {}
static AbstractQoreNode* static_File_lstat(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* path = HARD_QORE_STRING(args, 0);
   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(path->getBuffer(), &sbuf))
#else
   if (stat(path->getBuffer(), &sbuf))
#endif
   {
      xsink->raiseErrnoException("FILE-LSTAT-ERROR", errno, "lstat() command failed");
      return 0;
   }

   return stat_to_list(sbuf);
}

// static list File::stat(*string path) {}
static AbstractQoreNode* static_File_stat(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* path = reinterpret_cast<const QoreStringNode*>(args->retrieve_entry(0));
   struct stat sbuf;
   if (stat(path->getBuffer(), &sbuf)) {
      xsink->raiseErrnoException("FILE-STAT-ERROR", errno, "stat() command failed");
      return 0;
   }

   return stat_to_list(sbuf);
}

// static hash File::statvfs(string path) {}
static AbstractQoreNode* static_File_statvfs(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* path = HARD_QORE_STRING(args, 0);
#ifdef HAVE_SYS_STATVFS_H
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   struct statvfs vfs;
   if (statvfs(p0->getBuffer(), &vfs)) {
      xsink->raiseErrnoException("FILE-STATVFS-ERROR", errno, "statvfs() call failed");
      return 0;
   }

   return statvfs_to_hash(vfs);
#else
   return file_lock_error("statvfs", xsink);
#endif
   return 0;
}

QoreClass* initFileClass() {
   QC_FILE = new QoreClass("File", QDOM_FILESYSTEM);
   CID_FILE = QC_FILE->getID();

   // set system constructor
   QC_FILE->setSystemConstructor(FILE_system_constructor);

   // nothing File::chown(softint uid, softint gid = -1) {}
   QC_FILE->addMethodExtended("chown", (q_method_t)File_chown, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, softBigIntTypeInfo, NULL, softBigIntTypeInfo, new QoreBigIntNode(-1));

   // int File::close() {}
   QC_FILE->addMethodExtended("close", (q_method_int64_t)File_close, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // File::constructor(*string encoding) {}
   QC_FILE->setConstructorExtended(File_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringOrNothingTypeInfo, NULL);

   // File::copy() {}
   QC_FILE->setCopy((q_copy_t)File_copy);

   // int File::f_printf(string fmt, ...) {}
   QC_FILE->addMethodExtended("f_printf", (q_method_int64_t)File_f_printf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, NULL);

   // int File::f_vprintf(string fmt, any fmt_args) {}
   QC_FILE->addMethodExtended("f_vprintf", (q_method_int64_t)File_f_vprintf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, NULL, anyTypeInfo, NULL);

   // string File::getCharset() {}
   QC_FILE->addMethodExtended("getCharset", (q_method_t)File_getCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo);

   // string File::getEncoding() {}
   QC_FILE->addMethodExtended("getEncoding", (q_method_t)File_getEncoding, false, QC_NO_FLAGS, QDOM_DEFAULT, stringTypeInfo);

   // hash File::getLockInfo() {}
   QC_FILE->addMethodExtended("getLockInfo", (q_method_t)File_getLockInfo, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);

   // int File::getPos() {}
   QC_FILE->addMethodExtended("getPos", (q_method_int64_t)File_getPos, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // nothing File::getTerminalAttributes(TermIOS termios) {}
   QC_FILE->addMethodExtended("getTerminalAttributes", (q_method_t)File_getTerminalAttributes, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_TERMIOS->getTypeInfo(), NULL);

   // *string File::getchar() {}
   QC_FILE->addMethodExtended("getchar", (q_method_t)File_getchar, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // hash File::hstat() {}
   QC_FILE->addMethodExtended("hstat", (q_method_t)File_hstat, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);

   // bool File::isDataAvailable(timeout timeout_ms = 0) {}
   QC_FILE->addMethodExtended("isDataAvailable", (q_method_bool_t)File_isDataAvailable, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, timeoutTypeInfo, zero());

   // int File::lock(softint type = F_RDLCK, softint start = 0, softint len = 0, softint whence = SEEK_SET) {}
   QC_FILE->addMethodExtended("lock", (q_method_int64_t)File_lock, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 4, softBigIntTypeInfo, new QoreBigIntNode(F_RDLCK), softBigIntTypeInfo, zero(), softBigIntTypeInfo, zero(), softBigIntTypeInfo, new QoreBigIntNode(SEEK_SET));

   // nothing File::lockBlocking(softint type = F_RDLCK, softint start = 0, softint len = 0, softint whence = SEEK_SET) {}
   QC_FILE->addMethodExtended("lockBlocking", (q_method_t)File_lockBlocking, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 4, softBigIntTypeInfo, new QoreBigIntNode(F_RDLCK), softBigIntTypeInfo, zero(), softBigIntTypeInfo, zero(), softBigIntTypeInfo, new QoreBigIntNode(SEEK_SET));

   // int File::open(string filename, softint flags = O_RDONLY, softint mode = 0666, *string encoding) {}
   QC_FILE->addMethodExtended("open", (q_method_int64_t)File_open, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 4, stringTypeInfo, NULL, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666), stringOrNothingTypeInfo, NULL);

   // nothing File::open2(string filename, softint flags = O_RDONLY, softint mode = 0666, *string encoding) {}
   QC_FILE->addMethodExtended("open2", (q_method_t)File_open2, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 4, stringTypeInfo, NULL, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666), stringOrNothingTypeInfo, NULL);

   // int File::printf(string fmt, ...) {}
   QC_FILE->addMethodExtended("printf", (q_method_int64_t)File_printf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, NULL);

   // int File::printf() {}
   QC_FILE->addMethodExtended("printf", (q_method_int64_t)File_printf_1, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);

   // *string File::read(softint size, timeout timeout_ms = -1) {}
   QC_FILE->addMethodExtended("read", (q_method_t)File_read, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo, 2, softBigIntTypeInfo, NULL, timeoutTypeInfo, new QoreBigIntNode(-1));

   // *binary File::readBinary(softint size, timeout timeout_ms = -1) {}
   QC_FILE->addMethodExtended("readBinary", (q_method_t)File_readBinary, false, QC_NO_FLAGS, QDOM_DEFAULT, binaryOrNothingTypeInfo, 2, softBigIntTypeInfo, NULL, timeoutTypeInfo, new QoreBigIntNode(-1));

   // *string File::readLine() {}
   QC_FILE->addMethodExtended("readLine", (q_method_t)File_readLine, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // *int File::readi1() {}
   QC_FILE->addMethodExtended("readi1", (q_method_t)File_readi1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi2() {}
   QC_FILE->addMethodExtended("readi2", (q_method_t)File_readi2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi2LSB() {}
   QC_FILE->addMethodExtended("readi2LSB", (q_method_t)File_readi2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi4() {}
   QC_FILE->addMethodExtended("readi4", (q_method_t)File_readi4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi4LSB() {}
   QC_FILE->addMethodExtended("readi4LSB", (q_method_t)File_readi4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi4LSB() {}
   QC_FILE->addMethodExtended("readi4LSB", (q_method_t)File_readi4LSB_1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readi8() {}
   QC_FILE->addMethodExtended("readi8", (q_method_t)File_readi8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readu1() {}
   QC_FILE->addMethodExtended("readu1", (q_method_t)File_readu1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readu2() {}
   QC_FILE->addMethodExtended("readu2", (q_method_t)File_readu2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readu2LSB() {}
   QC_FILE->addMethodExtended("readu2LSB", (q_method_t)File_readu2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readu4() {}
   QC_FILE->addMethodExtended("readu4", (q_method_t)File_readu4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *int File::readu4LSB() {}
   QC_FILE->addMethodExtended("readu4LSB", (q_method_t)File_readu4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // nothing File::setCharset(*string encoding) {}
   QC_FILE->addMethodExtended("setCharset", (q_method_t)File_setCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringOrNothingTypeInfo, NULL);

   // nothing File::setEncoding(*string encoding) {}
   QC_FILE->addMethodExtended("setEncoding", (q_method_t)File_setEncoding, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringOrNothingTypeInfo, NULL);

   // nothing File::setEventQueue(Queue queue) {}
   QC_FILE->addMethodExtended("setEventQueue", (q_method_t)File_setEventQueue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), NULL);

   // nothing File::setEventQueue() {}
   QC_FILE->addMethodExtended("setEventQueue", (q_method_t)File_setEventQueue_1, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // int File::setPos(int pos = 0) {}
   QC_FILE->addMethodExtended("setPos", (q_method_int64_t)File_setPos, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, zero());

   // nothing File::setTerminalAttributes(softint action = TCSANOW, TermIOS termios) {}
   QC_FILE->addMethodExtended("setTerminalAttributes", (q_method_t)File_setTerminalAttributes, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, softBigIntTypeInfo, new QoreBigIntNode(TCSANOW), QC_TERMIOS->getTypeInfo(), NULL);

   // list File::stat() {}
   QC_FILE->addMethodExtended("stat", (q_method_t)File_stat, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo);

   // hash File::statvfs() {}
   QC_FILE->addMethodExtended("statvfs", (q_method_t)File_statvfs, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo);

   // int File::sync() {}
   QC_FILE->addMethodExtended("sync", (q_method_int64_t)File_sync, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // int File::vprintf(string fmt, any fmt_args) {}
   QC_FILE->addMethodExtended("vprintf", (q_method_int64_t)File_vprintf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, NULL, anyTypeInfo, NULL);

   // int File::write(binary data) {}
   QC_FILE->addMethodExtended("write", (q_method_int64_t)File_write, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, binaryTypeInfo, NULL);

   // int File::write(string data) {}
   QC_FILE->addMethodExtended("write", (q_method_int64_t)File_write_1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, NULL);

   // int File::writei1(int c) {}
   QC_FILE->addMethodExtended("writei1", (q_method_int64_t)File_writei1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei2(int s) {}
   QC_FILE->addMethodExtended("writei2", (q_method_int64_t)File_writei2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei2LSB(int s) {}
   QC_FILE->addMethodExtended("writei2LSB", (q_method_int64_t)File_writei2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei4(int i) {}
   QC_FILE->addMethodExtended("writei4", (q_method_int64_t)File_writei4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei4LSB(int i) {}
   QC_FILE->addMethodExtended("writei4LSB", (q_method_int64_t)File_writei4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei8(int i) {}
   QC_FILE->addMethodExtended("writei8", (q_method_int64_t)File_writei8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // int File::writei8LSB(int i) {}
   QC_FILE->addMethodExtended("writei8LSB", (q_method_int64_t)File_writei8LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, bigIntTypeInfo, NULL);

   // static hash File::hlstat(string path) {}
   QC_FILE->addStaticMethodExtended("hlstat", static_File_hlstat, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, NULL);

   // static hash File::hstat(string path) {}
   QC_FILE->addStaticMethodExtended("hstat", static_File_hstat, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, NULL);

   // static list File::lstat(string path) {}
   QC_FILE->addStaticMethodExtended("lstat", static_File_lstat, false, QC_NO_FLAGS, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, NULL);

   // static list File::stat(*string path) {}
   QC_FILE->addStaticMethodExtended("stat", static_File_stat, false, QC_NO_FLAGS, QDOM_FILESYSTEM, listTypeInfo, 1, stringOrNothingTypeInfo, NULL);

   // static hash File::statvfs(string path) {}
   QC_FILE->addStaticMethodExtended("statvfs", static_File_statvfs, false, QC_NO_FLAGS, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, NULL);

   return QC_FILE;
}
