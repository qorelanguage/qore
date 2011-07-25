/*
  QC_File.cpp

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

qore_classid_t CID_FILE;
QoreClass *QC_FILE = 0;

static void FILE_system_constructor(QoreObject *self, int fd, va_list args) {
   //printd(5, "FILE_system_constructor() self=%08p, descriptor=%d\n", self, fd);
   File *f = new File(QCS_DEFAULT);
   f->makeSpecial(fd);
   self->setPrivate(CID_FILE, f);
}

static void FILE_constructor(QoreObject *self, const QoreListNode *args, ExceptionSink *xsink) {
   // get encoding name if available
   const QoreEncoding *cs = get_encoding_param(args, 0);
   self->setPrivate(CID_FILE, new File(cs));
}

static void FILE_copy(QoreObject *self, QoreObject *old, File *f, ExceptionSink *xsink) {
   self->setPrivate(CID_FILE, new File(f->getEncoding()));
}

// open(string filename, [flags, mode, charset])
static AbstractQoreNode *FILE_open(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);

   int flags = get_int_param_with_default(args, 1, O_RDONLY);
   int mode = get_int_param_with_default(args, 2, 0666);
   const QoreEncoding *charset = get_encoding_param(args, 3, f->getEncoding());

   return new QoreBigIntNode(f->open(p0->getBuffer(), flags, mode, charset));
}

// open2(string filename, [flags, mode, charset])
// throws an exception if there is an error
static AbstractQoreNode *FILE_open2(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(args, 0);

   int flags = get_int_param_with_default(args, 1, O_RDONLY);
   int mode = get_int_param_with_default(args, 2, 0666);
   const QoreEncoding *charset = get_encoding_param(args, 3, f->getEncoding());
   
   f->open2(xsink, p0->getBuffer(), flags, mode, charset);
   return 0;
}

static AbstractQoreNode *FILE_close(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->close());
}

static AbstractQoreNode *FILE_sync(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->sync());
}

// *string File::read(softint $size, timeout $timeout_ms = -1)  
static AbstractQoreNode *FILE_read(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 size = HARD_QORE_INT(args, 0);
   if (!size) {
      xsink->raiseException("FILE-READ-PARAMETER-ERROR", "expecting size as first parameter of File::read()");
      return 0;
   }

   // get timeout
   int timeout_ms = (int)HARD_QORE_INT(args, 1);

   return f->read((qore_offset_t)size, timeout_ms, xsink);
}

static AbstractQoreNode *FILE_readu1(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   unsigned char c;
   if (f->readu1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

static AbstractQoreNode *FILE_readu2(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   unsigned short s;
   if (f->readu2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readu4(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   unsigned int i;
   if (f->readu4(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readu2LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   unsigned short s;
   if (f->readu2LSB(&s, xsink))
      return 0;
   
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readu4LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   unsigned int i;
   if (f->readu4LSB(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi1(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   char c;
   if (f->readi1(&c, xsink))
      return 0;
   return new QoreBigIntNode(c);
}

static AbstractQoreNode *FILE_readi2(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   short s;
   if (f->readi2(&s, xsink))
      return 0;
   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readi4(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int i;
   if (f->readi4(&i, xsink))
      return 0;

   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi8(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 i;
   if (f->readi8(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi2LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   short s;
   if (f->readi2LSB(&s, xsink))
      return 0;

   return new QoreBigIntNode(s);
}

static AbstractQoreNode *FILE_readi4LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int i;
   if (f->readi4LSB(&i, xsink))
      return 0;

   return new QoreBigIntNode(i);
}

static AbstractQoreNode *FILE_readi8LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 i;
   if (f->readi8LSB(&i, xsink))
      return 0;
   
   return new QoreBigIntNode(i);
}

// *binary File::readBinary(softint $size, timeout $timeout_ms = -1)  
static AbstractQoreNode *FILE_readBinary(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 size = HARD_QORE_INT(args, 0);
   if (!size) {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return 0;
   }

   // get timeout
   int timeout_ms = (int)HARD_QORE_INT(args, 1);

   return f->readBinary((qore_offset_t)size, timeout_ms, xsink);
}

static AbstractQoreNode *FILE_write_bin(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(bin, const BinaryNode, args, 0);

   int rc = f->write(bin, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_write_str(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(str, const QoreStringNode, args, 0);

   int rc = f->write(str, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei1(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   char c = (char)HARD_QORE_INT(args, 0);
   int rc = f->writei1(c, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei2(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   short s = (short)HARD_QORE_INT(args, 0);
   int rc = f->writei2(s, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei4(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int i = (int)HARD_QORE_INT(args, 0);
   int rc = f->writei4(i, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei8(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   int rc = f->writei8(i, xsink);
   if (xsink->isEvent())
      return 0;
   
   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei2LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   short s = (short)HARD_QORE_INT(args, 0);
   int rc = f->writei2LSB(s, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei4LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int i = (int)HARD_QORE_INT(args, 0);
   int rc = f->writei4LSB(i, xsink);
   if (xsink->isEvent())
      return 0;

   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_writei8LSB(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int64 i = HARD_QORE_INT(args, 0);
   int rc = f->writei8LSB(i, xsink);
   if (xsink->isEvent())
      return 0;
   
   return new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_printf(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   QoreStringNodeHolder str(q_sprintf(args, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_vprintf(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   QoreStringNodeHolder str(q_vsprintf(args, 0, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_f_printf(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   QoreStringNodeHolder str(q_sprintf(args, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_f_vprintf(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   QoreStringNodeHolder str(q_vsprintf(args, 1, 0, xsink));
   if (!str)
      return 0;

   int rc = f->write(*str, xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

static AbstractQoreNode *FILE_readLine(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->readLine(xsink);
}

static AbstractQoreNode *FILE_setCharset(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   const QoreEncoding *charset = get_encoding_param(args, 0);
   f->setEncoding(charset);
   return 0;
}

static AbstractQoreNode *FILE_getCharset(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreStringNode(f->getEncoding()->getCode());
}

static AbstractQoreNode *FILE_setPos(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int pos = (int)HARD_QORE_INT(args, 0);
   return new QoreBigIntNode(f->setPos(pos));
}

/*
static AbstractQoreNode *FILE_setPosFromEnd(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return 0;
}

static AbstractQoreNode *FILE_setPosFromCurrent(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return 0;
}
*/

static AbstractQoreNode *FILE_getPos(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(f->getPos());
}

// *string File::getchar()  
static AbstractQoreNode *FILE_getchar(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->getchar();
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
static AbstractQoreNode *file_lock_error(const char *f, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "this platform does not support UNIX-style file locking with fnctl(), therefore the File::%s() method is not available; for maximum portability, check Option::HAVE_FILE_LOCKING before calling this method", f);
   return 0;
}
#ifndef F_RDLCK
#define F_RDLCK -1
#endif
#endif

AbstractQoreNode *missing_method_error(const char *meth, const char *feat, ExceptionSink *xsink) {
   xsink->raiseException("MISSING-FEATURE-ERROR", "the %s() method is not available on this build; for maximum portability, check Option::%s before calling this method", meth, feat);
   return 0;
}

static AbstractQoreNode *FILE_lock(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_STRUCT_FLOCK
   struct flock fl;

   if (lock_intern(fl, args, xsink))
      return 0;

   int rc = f->lock(fl, xsink);
   if (*xsink)
      return 0;
   return new QoreBigIntNode(rc);
#else
   return file_lock_error("lock", xsink);
#endif
}

static AbstractQoreNode *FILE_lockBlocking(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_STRUCT_FLOCK
   struct flock fl;

   if (lock_intern(fl, args, xsink))
      return 0;

   f->lockBlocking(fl, xsink);
   return 0;
#else
   return file_lock_error("lockBlocking", xsink);
#endif
}

static AbstractQoreNode *FILE_getLockInfo(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_STRUCT_FLOCK
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
}

static AbstractQoreNode *FILE_chown(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_CHOWN
   uid_t owner = (uid_t)HARD_QORE_INT(args, 0);
   gid_t group = (gid_t)HARD_QORE_INT(args, 1);
   f->chown(owner, group, xsink);
   return 0;
#else
   return missing_method_error("File::chown", "CHOWN", xsink);
#endif
}

// bool File::isDataAvailable(timeout $timeout = 0)  
static AbstractQoreNode *FILE_isDataAvailable(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int timeout_ms = (int)HARD_QORE_INT(args, 0);
   bool rc = f->isDataAvailable(timeout_ms, xsink);
   return *xsink ? 0 : get_bool_node(rc);
}

#ifdef HAVE_TERMIOS_H
static AbstractQoreNode *FILE_getTerminalAttributes(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(ios, QoreTermIOS, args, 0, CID_TERMIOS, "TermIOS", "File::getTerminalAttributes", xsink);
   if (*xsink)
      return 0;
   assert(ios);
   ReferenceHolder<QoreTermIOS> holder(ios, xsink);
   f->getTerminalAttributes(ios, xsink);
   return 0;
}

static AbstractQoreNode *FILE_setTerminalAttributes(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   int action = (int)HARD_QORE_INT(args, 0);
   HARD_QORE_OBJ_DATA(ios, QoreTermIOS, args, 1, CID_TERMIOS, "TermIOS", "File::setTerminalAttributes", xsink);
   if (*xsink)
      return 0;

   ReferenceHolder<QoreTermIOS> holder(ios, xsink);
   f->setTerminalAttributes(action, ios, xsink);
   return 0;
}
#endif

static AbstractQoreNode *FILE_setEventQueue_queue(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(q, Queue, args, 0, CID_QUEUE, "Queue", "File::setEventQueue", xsink);
   if (*xsink)
      return 0;

   // pass reference from QoreObject::getReferencedPrivateData() to function
   f->setEventQueue(q, xsink);
   return 0;
}

static AbstractQoreNode *FILE_setEventQueue_nothing(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   f->setEventQueue(0, xsink);
   return 0;
}

static AbstractQoreNode *FILE_stat(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->stat(xsink);
}

static AbstractQoreNode *FILE_hstat(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
   return f->hstat(xsink);
}

static AbstractQoreNode *FILE_statvfs(QoreObject *self, File *f, const QoreListNode *args, ExceptionSink *xsink) {
#ifdef HAVE_SYS_STATVFS_H
   return f->statvfs(xsink);
#else
   return missing_method_error("File::statvfs", "STATVFS", xsink);
#endif
}

// static methods
static AbstractQoreNode *f_FILE_stat(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   struct stat sbuf;
   if (stat(p0->getBuffer(), &sbuf)) {
      xsink->raiseErrnoException("FILE-STAT-ERROR", errno, "stat() command failed");
      return 0;
   }

   return stat_to_list(sbuf);
}

static AbstractQoreNode *f_FILE_lstat(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(p0->getBuffer(), &sbuf)) {
#else
   if (stat(p0->getBuffer(), &sbuf)) {
#endif
      xsink->raiseErrnoException("FILE-LSTAT-ERROR", errno, "lstat() command failed");
      return 0;
   }

   return stat_to_list(sbuf);
}

static AbstractQoreNode *f_FILE_hstat(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   struct stat sbuf;
   if (stat(p0->getBuffer(), &sbuf)) {
      xsink->raiseErrnoException("FILE-HSTAT-ERROR", errno, "stat() command failed");
      return 0;
   }

   return stat_to_hash(sbuf);
}

static AbstractQoreNode *f_FILE_hlstat(const QoreListNode *args, ExceptionSink *xsink) {
   HARD_QORE_PARAM(p0, const QoreStringNode, args, 0);

   struct stat sbuf;
#ifdef HAVE_LSTAT
   if (lstat(p0->getBuffer(), &sbuf)) {
#else
   if (stat(p0->getBuffer(), &sbuf)) {
#endif
      xsink->raiseErrnoException("FILE-HLSTAT-ERROR", errno, "lstat() command failed");
      return 0;
   }

   return stat_to_hash(sbuf);
}

static AbstractQoreNode *f_FILE_statvfs(const QoreListNode *args, ExceptionSink *xsink) {
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
}


QoreClass *initFileClass() {
   QORE_TRACE("initFileClass()");

   QC_FILE = new QoreClass("File", QDOM_FILESYSTEM);
   CID_FILE = QC_FILE->getID();

   QC_FILE->setSystemConstructor(FILE_system_constructor);

   QC_FILE->setConstructorExtended(FILE_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT);
   QC_FILE->setConstructorExtended(FILE_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->setCopy((q_copy_t)FILE_copy);

   QC_FILE->addMethodExtended("open",              (q_method_t)FILE_open, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666));
   QC_FILE->addMethodExtended("open",              (q_method_t)FILE_open, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 4, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666), stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("open2",             (q_method_t)FILE_open2, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 3, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666));
   QC_FILE->addMethodExtended("open2",             (q_method_t)FILE_open2, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 4, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, new QoreBigIntNode(O_RDONLY), softBigIntTypeInfo, new QoreBigIntNode(0666), stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("close",             (q_method_t)FILE_close, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FILE->addMethodExtended("sync",              (q_method_t)FILE_sync, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);

   // *string File::read(softint $size, timeout $timeout_ms = -1)  
   QC_FILE->addMethodExtended("read",              (q_method_t)FILE_read, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1));

   // *int File::readu1()  
   QC_FILE->addMethodExtended("readu1",            (q_method_t)FILE_readu1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readu2",            (q_method_t)FILE_readu2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readu4",            (q_method_t)FILE_readu4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readu2LSB",         (q_method_t)FILE_readu2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readu4LSB",         (q_method_t)FILE_readu4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi1",            (q_method_t)FILE_readi1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi2",            (q_method_t)FILE_readi2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi4",            (q_method_t)FILE_readi4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi8",            (q_method_t)FILE_readi8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi2LSB",         (q_method_t)FILE_readi2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi4LSB",         (q_method_t)FILE_readi4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);
   QC_FILE->addMethodExtended("readi8LSB",         (q_method_t)FILE_readi8LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntOrNothingTypeInfo);

   // *binary File::readBinary(softint $size, timeout $timeout_ms = -1)  
   QC_FILE->addMethodExtended("readBinary",        (q_method_t)FILE_readBinary, false, QC_NO_FLAGS, QDOM_DEFAULT, binaryOrNothingTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, timeoutTypeInfo, new QoreBigIntNode(-1));

   // overloaded write method
   QC_FILE->addMethodExtended("write",             (q_method_t)FILE_write_bin, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, binaryTypeInfo, QORE_PARAM_NO_ARG);
   QC_FILE->addMethodExtended("write",             (q_method_t)FILE_write_str, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   
   QC_FILE->addMethodExtended("writei1",           (q_method_t)FILE_writei1, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei2",           (q_method_t)FILE_writei2, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei4",           (q_method_t)FILE_writei4, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei8",           (q_method_t)FILE_writei8, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei2LSB",        (q_method_t)FILE_writei2LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei4LSB",        (q_method_t)FILE_writei4LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());
   QC_FILE->addMethodExtended("writei8LSB",        (q_method_t)FILE_writei8LSB, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());

   // *string File::readLine()  
   QC_FILE->addMethodExtended("readLine",          (q_method_t)FILE_readLine, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo);

   QC_FILE->addMethodExtended("setCharset",        (q_method_t)FILE_setCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   QC_FILE->addMethodExtended("setCharset",        (q_method_t)FILE_setCharset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("getCharset",        (q_method_t)FILE_getCharset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   QC_FILE->addMethodExtended("setPos",            (q_method_t)FILE_setPos, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());

   //QC_FILE->addMethod("setPosFromEnd",     (q_method_t)FILE_setPosFromEnd);
   //QC_FILE->addMethod("setPosFromCurrent", (q_method_t)FILE_setPosFromCurrent);

   QC_FILE->addMethodExtended("getPos",            (q_method_t)FILE_getPos, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // *string File::getchar()  
   QC_FILE->addMethodExtended("getchar",           (q_method_t)FILE_getchar, false, QC_NO_FLAGS, QDOM_DEFAULT, stringOrNothingTypeInfo);

   // add an alias for write(string)
   QC_FILE->addMethodExtended("print",             (q_method_t)FILE_write_str, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("printf",            (q_method_t)class_int_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FILE->addMethodExtended("printf",            (q_method_t)FILE_printf, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("vprintf",           (q_method_t)class_int_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FILE->addMethodExtended("vprintf",           (q_method_t)FILE_vprintf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("f_printf",          (q_method_t)class_int_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FILE->addMethodExtended("f_printf",          (q_method_t)FILE_f_printf, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, bigIntTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("f_vprintf",         (q_method_t)class_int_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, bigIntTypeInfo);
   QC_FILE->addMethodExtended("f_vprintf",         (q_method_t)FILE_f_vprintf, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, anyTypeInfo, QORE_PARAM_NO_ARG);

   QC_FILE->addMethodExtended("lock",              (q_method_t)FILE_lock, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo, 4, softBigIntTypeInfo, new QoreBigIntNode(F_RDLCK), softBigIntTypeInfo, zero(), softBigIntTypeInfo, zero(), softBigIntTypeInfo, new QoreBigIntNode(SEEK_SET));
   QC_FILE->addMethodExtended("lockBlocking",      (q_method_t)FILE_lockBlocking, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 4, softBigIntTypeInfo, new QoreBigIntNode(F_RDLCK), softBigIntTypeInfo, zero(), softBigIntTypeInfo, zero(), softBigIntTypeInfo, new QoreBigIntNode(SEEK_SET));

   QC_FILE->addMethodExtended("getLockInfo",       (q_method_t)FILE_getLockInfo, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   QC_FILE->addMethodExtended("chown",             (q_method_t)FILE_chown, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, softBigIntTypeInfo, new QoreBigIntNode(-1), softBigIntTypeInfo, new QoreBigIntNode(-1));

   // bool File::isDataAvailable(timeout $timeout = 0)  
   QC_FILE->addMethodExtended("isDataAvailable",        (q_method_t)FILE_isDataAvailable, false, QC_NO_FLAGS, QDOM_DEFAULT, boolTypeInfo, 1, timeoutTypeInfo, zero());

#ifdef HAVE_TERMIOS_H
   QC_FILE->addMethodExtended("getTerminalAttributes",  (q_method_t)FILE_getTerminalAttributes, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_TERMIOS->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_FILE->addMethodExtended("setTerminalAttributes",  (q_method_t)FILE_setTerminalAttributes, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, softBigIntTypeInfo, new QoreBigIntNode(TCSANOW), QC_TERMIOS->getTypeInfo(), QORE_PARAM_NO_ARG);
#endif

   // overloaded setEventQueue() method
   QC_FILE->addMethodExtended("setEventQueue",          (q_method_t)FILE_setEventQueue_queue, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_QUEUE->getTypeInfo(), QORE_PARAM_NO_ARG);
   QC_FILE->addMethodExtended("setEventQueue",          (q_method_t)FILE_setEventQueue_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   QC_FILE->addMethodExtended("stat",                   (q_method_t)FILE_stat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo);
   QC_FILE->addMethodExtended("hstat",                  (q_method_t)FILE_hstat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);
   QC_FILE->addMethodExtended("statvfs",                (q_method_t)FILE_statvfs, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   // static methods
   QC_FILE->addStaticMethodExtended("stat",     f_FILE_stat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FILE->addStaticMethodExtended("lstat",    f_FILE_lstat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FILE->addStaticMethodExtended("hstat",    f_FILE_hstat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FILE->addStaticMethodExtended("hlstat",   f_FILE_hlstat, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_FILE->addStaticMethodExtended("statvfs",  f_FILE_statvfs, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_FILE;
}
