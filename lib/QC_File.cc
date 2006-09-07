/*
  QC_File.cc

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
#include <qore/common.h>
#include <qore/QC_File.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>
#include <qore/charset.h>

int CID_FILE;

static void getFile(void *obj)
{
   ((File *)obj)->ROreference();
}

static void releaseFile(void *obj)
{
   ((File *)obj)->deref();
}

static void FILE_system_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   //printd(5, "FILE_constructor() self=%08p, params=%08p\n", self, params);
   self->setPrivate(CID_FILE, new File(QCS_DEFAULT), getFile, releaseFile);
}

static void FILE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get character set name if available
   class QoreEncoding *cs;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
   {
      cs = QEM.findCreate(p0->val.String);
      //printd(0, "FILE_constructor() str=%s, cs=%08p\n", p0->val.String->getBuffer(), cs);
   }
   else
      cs = QCS_DEFAULT;

   self->setPrivate(CID_FILE, new File(cs), getFile, releaseFile);
}

static void FILE_destructor(class Object *self, class File *f, ExceptionSink *xsink)
{
   //printd(5, "FILE_destructor() self=%08p, f=%08p\n", self, f);
   f->deref();
}

static void FILE_copy(class Object *self, class Object *old, class File *f, class ExceptionSink *xsink)
{
   self->setPrivate(CID_FILE, new File(f->getEncoding()), getFile, releaseFile);
}

// open(filename, [flags, mode, charset])
static class QoreNode *FILE_open(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p;
   int flags, mode;
   class QoreEncoding *charset;
   p0 = test_param(params, NT_STRING, 0);
   if (!p0)
   {
      xsink->raiseException("FILE-OPEN-PARAMETER-ERROR", "expecting string filename as first argument of File::open()");
      return NULL;
   }
   p = get_param(params, 1);
   if (p)
      flags = p->getAsInt();
   else
      flags = 0;
   p = get_param(params, 2);
   if (p)
      mode = p->getAsInt();
   else
      mode = 0666;
   p = test_param(params, NT_STRING, 3);
   if (p)
      charset = QEM.findCreate(p->val.String);
   else
      charset = QCS_DEFAULT;

   return new QoreNode((int64)f->open(p0->val.String->getBuffer(), flags, mode, charset));
}

static class QoreNode *FILE_close(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)f->close());
}

static class QoreNode *FILE_sync(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)f->sync());
}

static class QoreNode *FILE_read(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   int size;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      size = p0->getAsInt();
   else
      size = 0;

   if (!size)
   {
      xsink->raiseException("FILE-READ-PARAMETER-ERROR", "expecting size as first parameter of File::read()");
      return NULL;
   }

   QoreString *str = f->read(size, xsink);
   if (str)
      return new QoreNode(str);

   return NULL;
}

static class QoreNode *FILE_readi1(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   char c;
   if (f->readi1(&c, xsink))
      return NULL;
   return new QoreNode((int64)c);
}

static class QoreNode *FILE_readi2(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2(&s, xsink))
      return NULL;
   return new QoreNode((int64)s);
}

static class QoreNode *FILE_readi4(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4(&i, xsink))
      return NULL;

   return new QoreNode((int64)i);
}

static class QoreNode *FILE_readi2LSB(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   short s;
   if (f->readi2LSB(&s, xsink))
      return NULL;

   return new QoreNode((int64)s);
}

static class QoreNode *FILE_readi4LSB(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   int i;
   if (f->readi4LSB(&i, xsink))
      return NULL;

   return new QoreNode((int64)i);
}

static class QoreNode *FILE_readBinary(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);

   int size = p0 ? p0->getAsInt() : 0;
   if (!size)
   {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return NULL;
   }

   BinaryObject *b = f->readBinary(size, xsink);
   if (b)
      return new QoreNode(b);

   return NULL;
}

static class QoreNode *FILE_write(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("FILE-WRITE-PARAMETER-ERROR", "expecting string or binary object to write as first parameter of File::write()");
      return NULL;
   }

   int rc;
   if (p0->type == NT_STRING)
      rc = f->write(p0->val.String, xsink);
   else
      rc = f->write(p0->val.bin, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_writei1(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   char c;
   if (!p0)
      c = 0;
   else
      c = p0->getAsInt();

   int rc = f->writei1(c, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_writei2(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2(s, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_writei4(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4(i, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_writei2LSB(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   int rc = f->writei2LSB(s, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_writei4LSB(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   int rc = f->writei4LSB(i, xsink);
   if (xsink->isEvent())
      return NULL;

   return new QoreNode((int64)rc);
}

static class QoreNode *FILE_printf(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (!str)
      return NULL;

   int rc = f->write(str, xsink);

   class QoreNode *rv;
   if (xsink->isEvent())
      rv = NULL;
   else
      rv = new QoreNode((int64)rc);
   delete str;

   return rv;
}

static class QoreNode *FILE_vprintf(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_vsprintf(params, 0, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;

   int rc = f->write(str, xsink);
   if (xsink->isEvent())
      rv = NULL;
   else
      rv = new QoreNode((int64)rc);
   delete str;

   return rv;
}

static class QoreNode *FILE_f_printf(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 1, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   int rc = f->write(str, xsink);
   if (xsink->isEvent())
      rv = NULL;
   else
      rv = new QoreNode((int64)rc);
   delete str;

   return rv;
}

static class QoreNode *FILE_f_vprintf(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_vsprintf(params, 1, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   int rc = f->write(str, xsink);
   if (xsink->isEvent())
      rv = NULL;
   else
      rv = new QoreNode((int64)rc);
   delete str;
   return rv;
}

static class QoreNode *FILE_readLine(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreString *str = f->readLine(xsink);
   if (str)
      return new QoreNode(str);

   return NULL;
}

static class QoreNode *FILE_setCharset(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreEncoding *charset;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
      charset = QEM.findCreate(p0->val.String);
   else
      charset = QCS_DEFAULT;

   f->setEncoding(charset);
   return NULL;
}

static class QoreNode *FILE_getCharset(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(f->getEncoding()->code);
}

static class QoreNode *FILE_setPos(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   int pos;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      pos = p0->getAsInt();
   else
      pos = 0;

   return new QoreNode((int64)f->setPos(pos));
}

/*
static class QoreNode *FILE_setPosFromEnd(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   //f->open();
   return NULL;
}

static class QoreNode *FILE_setPosFromCurrent(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   //f->open();
   return NULL;
}
*/

static class QoreNode *FILE_getPos(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)f->getPos());
}

static class QoreNode *FILE_getchar(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   QoreString *str = f->getchar();
   if (str)
      return new QoreNode(str);

   return NULL;
}

#ifdef DEBUG
static class QoreNode *FILE_getFD(class Object *self, class File *f, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)f->getFD());
}
#endif

class QoreClass *initFileClass()
{
   tracein("initFileClass()");

   class QoreClass *QC_FILE = new QoreClass(strdup("File"));
   CID_FILE = QC_FILE->getID();
   QC_FILE->setSystemConstructor(FILE_system_constructor);
   QC_FILE->setConstructor(FILE_constructor);
   QC_FILE->setDestructor((q_destructor_t)FILE_destructor);
   QC_FILE->setCopy((q_copy_t)FILE_copy);
   QC_FILE->addMethod("open",              (q_method_t)FILE_open);
   QC_FILE->addMethod("close",             (q_method_t)FILE_close);
   QC_FILE->addMethod("sync",              (q_method_t)FILE_sync);
   QC_FILE->addMethod("read",              (q_method_t)FILE_read);
   QC_FILE->addMethod("readi1",            (q_method_t)FILE_readi1);
   QC_FILE->addMethod("readi2",            (q_method_t)FILE_readi2);
   QC_FILE->addMethod("readi4",            (q_method_t)FILE_readi4);
   QC_FILE->addMethod("readi2LSB",         (q_method_t)FILE_readi2LSB);
   QC_FILE->addMethod("readi4LSB",         (q_method_t)FILE_readi4LSB);
   QC_FILE->addMethod("readBinary",        (q_method_t)FILE_readBinary);
   QC_FILE->addMethod("write",             (q_method_t)FILE_write);
   QC_FILE->addMethod("writei1",           (q_method_t)FILE_writei1);
   QC_FILE->addMethod("writei2",           (q_method_t)FILE_writei2);
   QC_FILE->addMethod("writei4",           (q_method_t)FILE_writei4);
   QC_FILE->addMethod("writei2LSB",        (q_method_t)FILE_writei2LSB);
   QC_FILE->addMethod("writei4LSB",        (q_method_t)FILE_writei4LSB);
   QC_FILE->addMethod("readLine",          (q_method_t)FILE_readLine);
   QC_FILE->addMethod("setCharset",        (q_method_t)FILE_setCharset);
   QC_FILE->addMethod("getCharset",        (q_method_t)FILE_getCharset);
   QC_FILE->addMethod("setPos",            (q_method_t)FILE_setPos);
   //QC_FILE->addMethod("setPosFromEnd",     (q_method_t)FILE_setPosFromEnd);
   //QC_FILE->addMethod("setPosFromCurrent", (q_method_t)FILE_setPosFromCurrent);
   QC_FILE->addMethod("getPos",            (q_method_t)FILE_getPos);
   QC_FILE->addMethod("getchar",           (q_method_t)FILE_getchar);
   QC_FILE->addMethod("printf",            (q_method_t)FILE_printf);
   QC_FILE->addMethod("vprintf",           (q_method_t)FILE_vprintf);
   QC_FILE->addMethod("f_printf",          (q_method_t)FILE_f_printf);
   QC_FILE->addMethod("f_vprintf",         (q_method_t)FILE_f_vprintf);
#ifdef DEBUG
   QC_FILE->addMethod("getFD",             (q_method_t)FILE_getFD);
#endif
   traceout("initFileClass()");
   return QC_FILE;
}
