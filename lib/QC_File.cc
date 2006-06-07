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

static void *getFile(void *obj)
{
   ((File *)obj)->ROreference();
   return obj;
}

static class QoreNode *FILE_system_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get character set name if available
   class QoreEncoding *cs;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
   {
      cs = QEM.findCreate(p0->val.String);
      //printd(0, "FILE_constructor() str=%s, cs=%08x\n", p0->val.String->getBuffer(), cs);
   }
   else
      cs = QCS_DEFAULT;

   self->setPrivate(CID_FILE, new File(cs), getFile);
   return NULL;
}

static class QoreNode *FILE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   // get character set name if available
   class QoreEncoding *cs;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
   {
      cs = QEM.findCreate(p0->val.String);
      //printd(0, "FILE_constructor() str=%s, cs=%08x\n", p0->val.String->getBuffer(), cs);
   }
   else
      cs = QCS_DEFAULT;

   self->setPrivate(CID_FILE, new File(cs), getFile);
   return NULL;
}

static class QoreNode *FILE_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getAndClearPrivateData(CID_FILE);
   if (f)
      f->deref();
   return NULL;
}

static class QoreNode *FILE_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)get_param(params, 0)->val.object->getReferencedPrivateData(CID_FILE);

   if (f)
   {
      self->setPrivate(CID_FILE, new File(f->getEncoding()), getFile);
      f->deref();
   }
   else
      alreadyDeleted(xsink, "File::copy");

   return NULL;
}

// open(filename, [flags, mode, charset])
static class QoreNode *FILE_open(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->open(p0->val.String->getBuffer(), flags, mode, charset);
      f->deref();
      rv = new QoreNode(NT_INT, rc);
   }
   else
   {
      alreadyDeleted(xsink, "File::open");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_close(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->close();
      f->deref();
      rv = new QoreNode(NT_INT, rc);
   }
   else
   {
      alreadyDeleted(xsink, "File::close");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_sync(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->sync();
      f->deref();
      rv = new QoreNode(NT_INT, rc);
   }
   else
   {
      alreadyDeleted(xsink, "File::sync");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_read(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      QoreString *str = f->read(size, xsink);
      if (str)
	 rv = new QoreNode(str);
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::read");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readi1(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);

   QoreNode *rv;
   if (f)
   {
      char c;
      if (f->readi1(&c, xsink))
	 rv = NULL;
      else
	 rv = new QoreNode((int64)c);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readi1");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readi2(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);

   QoreNode *rv;
   if (f)
   {
      short s;
      if (f->readi2(&s, xsink))
	 rv = NULL;
      else
	 rv = new QoreNode((int64)s);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readi2");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readi4(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);

   QoreNode *rv;
   if (f)
   {
      int i;
      if (f->readi4(&i, xsink))
	 rv = NULL;
      else
	 rv = new QoreNode((int64)i);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readi4");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readi2LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);

   QoreNode *rv;
   if (f)
   {
      short s;
      if (f->readi2LSB(&s, xsink))
	 rv = NULL;
      else
	 rv = new QoreNode((int64)s);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readi2LSB");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readi4LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);

   QoreNode *rv;
   if (f)
   {
      int i;
      if (f->readi4LSB(&i, xsink))
	 rv = NULL;
      else
	 rv = new QoreNode((int64)i);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readi4LSB");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_readBinary(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int size;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      size = p0->getAsInt();
   else
      size = 0;

   if (!size)
   {
      xsink->raiseException("FILE-READ-BINARY-PARAMETER-ERROR", "expecting size as first parameter of File::readBinary()");
      return NULL;
   }

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      BinaryObject *b = f->readBinary(size, xsink);
      if (b)
	 rv = new QoreNode(b);
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readBinary");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_write(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0 || (p0->type != NT_STRING && p0->type != NT_BINARY))
   {
      xsink->raiseException("FILE-WRITE-PARAMETER-ERROR", "expecting string or binary object to write as first parameter of File::write()");
      return NULL;
   }

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc;
      if (p0->type == NT_STRING)
	 rc = f->write(p0->val.String, xsink);
      else
	 rc = f->write(p0->val.bin, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::write");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_writei1(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   char c;
   if (!p0)
      c = 0;
   else
      c = p0->getAsInt();

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->writei1(c, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode((int64)rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::writei1");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_writei2(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->writei2(s, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode((int64)rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::writei2");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_writei4(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->writei4(i, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode((int64)rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::writei4");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_writei2LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   short s;
   if (!p0)
      s = 0;
   else
      s = p0->getAsInt();

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->writei2LSB(s, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode((int64)rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::writei2LSB");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_writei4LSB(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   int i;
   if (!p0)
      i = 0;
   else
      i = p0->getAsInt();

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->writei4LSB(i, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode((int64)rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::writei4LSB");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_printf(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->write(str, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::printf");
      rv = NULL;
   }
   delete str;
   return rv;
}

static class QoreNode *FILE_vprintf(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_vsprintf(params, 0, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->write(str, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::vprintf");
      rv = NULL;
   }
   delete str;
   return rv;
}

static class QoreNode *FILE_f_printf(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 1, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->write(str, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::v_printf");
      rv = NULL;
   }
   delete str;
   return rv;
}

static class QoreNode *FILE_f_vprintf(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreString *str = q_vsprintf(params, 1, 0, xsink);
   if (!str)
      return NULL;

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int rc = f->write(str, xsink);
      if (xsink->isEvent())
	 rv = NULL;
      else
	 rv = new QoreNode(NT_INT, rc);
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::f_vprintf");
      rv = NULL;
   }
   delete str;
   return rv;
}

static class QoreNode *FILE_readLine(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      QoreString *str = f->readLine(xsink);
      if (str)
	 rv = new QoreNode(str);
      else
	 rv = NULL;
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::readLine");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_setCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreEncoding *charset;
   QoreNode *p0 = test_param(params, NT_STRING, 0);
   if (p0)
      charset = QEM.findCreate(p0->val.String);
   else
      charset = QCS_DEFAULT;

   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      f->setEncoding(charset);
      f->deref();
   }
   else
      alreadyDeleted(xsink, "File::setCharset");
   return NULL;
}

static class QoreNode *FILE_getCharset(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      class QoreEncoding *ccs = f->getEncoding();
      f->deref();
      rv = new QoreNode(ccs ? ccs->code : "(unknown)");
   }
   else
   {
      alreadyDeleted(xsink, "File::getCharset");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_setPos(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int pos;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      pos = p0->getAsInt();
   else
      pos = 0;

   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      rv = new QoreNode(NT_INT, f->setPos(pos));
      f->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "File::setPos");
   }
   return rv;
}

/*
static class QoreNode *FILE_setPosFromEnd(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      //f->open();
      f->deref();
   }
   else
      alreadyDeleted(xsink, "File::setPosFromEnd");
   return NULL;
}

static class QoreNode *FILE_setPosFromCurrent(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      //f->open();
      f->deref();
   }
   else
      alreadyDeleted(xsink, "File::setPosFromCurrent");
   return NULL;
}
*/

static class QoreNode *FILE_getPos(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      int p = f->getPos();
      f->deref();
      rv = new QoreNode(NT_INT, p);
   }
   else
   {
      alreadyDeleted(xsink, "File::getPos");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *FILE_getchar(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      QoreString *str = f->getchar();
      if (str)
	 rv = new QoreNode(str);
      else 
	 rv = NULL;
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::getchar");
      rv = NULL;
   }
   return rv;
}

#ifdef DEBUG
static class QoreNode *FILE_getFD(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *rv;
   File *f = (File *)self->getReferencedPrivateData(CID_FILE);
   if (f)
   {
      rv = new QoreNode(NT_INT, f->getFD());
      f->deref();
   }
   else
   {
      alreadyDeleted(xsink, "File::getFD");
      rv = NULL;
   }
   return rv;
}
#endif

class QoreClass *initFileClass()
{
   tracein("initFileClass()");

   class QoreClass *QC_FILE = new QoreClass(strdup("File"));
   CID_FILE = QC_FILE->getID();
   QC_FILE->setSystemConstructor(FILE_system_constructor);
   QC_FILE->addMethod("constructor",       FILE_constructor);
   QC_FILE->addMethod("destructor",        FILE_destructor);
   QC_FILE->addMethod("copy",              FILE_copy);
   QC_FILE->addMethod("open",              FILE_open);
   QC_FILE->addMethod("close",             FILE_close);
   QC_FILE->addMethod("sync",              FILE_sync);
   QC_FILE->addMethod("read",              FILE_read);
   QC_FILE->addMethod("readi1",            FILE_readi1);
   QC_FILE->addMethod("readi2",            FILE_readi2);
   QC_FILE->addMethod("readi4",            FILE_readi4);
   QC_FILE->addMethod("readi2LSB",         FILE_readi2LSB);
   QC_FILE->addMethod("readi4LSB",         FILE_readi4LSB);
   QC_FILE->addMethod("readBinary",        FILE_readBinary);
   QC_FILE->addMethod("write",             FILE_write);
   QC_FILE->addMethod("writei1",           FILE_writei1);
   QC_FILE->addMethod("writei2",           FILE_writei2);
   QC_FILE->addMethod("writei4",           FILE_writei4);
   QC_FILE->addMethod("writei2LSB",        FILE_writei2LSB);
   QC_FILE->addMethod("writei4LSB",        FILE_writei4LSB);
   QC_FILE->addMethod("readLine",          FILE_readLine);
   QC_FILE->addMethod("setCharset",        FILE_setCharset);
   QC_FILE->addMethod("getCharset",        FILE_getCharset);
   QC_FILE->addMethod("setPos",            FILE_setPos);
   //QC_FILE->addMethod("setPosFromEnd",     FILE_setPosFromEnd);
   //QC_FILE->addMethod("setPosFromCurrent", FILE_setPosFromCurrent);
   QC_FILE->addMethod("getPos",            FILE_getPos);
   QC_FILE->addMethod("getchar",           FILE_getchar);
   QC_FILE->addMethod("printf",            FILE_printf);
   QC_FILE->addMethod("vprintf",           FILE_vprintf);
   QC_FILE->addMethod("f_printf",          FILE_f_printf);
   QC_FILE->addMethod("f_vprintf",         FILE_f_vprintf);
#ifdef DEBUG
   QC_FILE->addMethod("getFD",             FILE_getFD);
#endif
   traceout("initFileClass()");
   return QC_FILE;
}
