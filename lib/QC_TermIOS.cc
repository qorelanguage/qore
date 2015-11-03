/*
  QC_TermIOS.cc
  
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
#include <qore/intern/QC_TermIOS.h>

qore_classid_t CID_TERMIOS;

static void TERMIOS_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_TERMIOS, new QoreTermIOS());
}

static void TERMIOS_copy(QoreObject *self, QoreObject *old, QoreTermIOS *s, ExceptionSink *xsink) {
   self->setPrivate(CID_TERMIOS, new QoreTermIOS(*s));
}

static AbstractQoreNode *TERMIOS_getLFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->get_lflag());
}

static AbstractQoreNode *TERMIOS_getCFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->get_cflag());
}

static AbstractQoreNode *TERMIOS_getOFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->get_oflag());
}

static AbstractQoreNode *TERMIOS_getIFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->get_iflag());
}

static AbstractQoreNode *TERMIOS_setLFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   s->set_lflag(p ? p->getAsBigInt() : 0);
   return 0;
}

static AbstractQoreNode *TERMIOS_setCFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   s->set_cflag(p ? p->getAsBigInt() : 0);
   return 0;
}

static AbstractQoreNode *TERMIOS_setOFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   s->set_oflag(p ? p->getAsBigInt() : 0);
   return 0;
}

static AbstractQoreNode *TERMIOS_setIFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);
   s->set_iflag(p ? p->getAsBigInt() : 0);
   return 0;
}

static AbstractQoreNode *TERMIOS_getCC(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p = get_param(params, 0);

   int64 rc = s->get_cc(p ? p->getAsBigInt() : 0, xsink);
   if (*xsink)
      return 0;
   return new QoreBigIntNode(rc);
}

// setCC(offset, value)
static AbstractQoreNode *TERMIOS_setCC(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   const AbstractQoreNode *p0 = get_param(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);

   s->set_cc(p0 ? p0->getAsBigInt() : 0, p1 ? p1->getAsInt() : 0, xsink);
   return 0;
}

static AbstractQoreNode *TERMIOS_isEqual(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   QoreObject *p0 = test_object_param(params, 0);
   QoreTermIOS *ios = p0 ? (QoreTermIOS *)p0->getReferencedPrivateData(CID_TERMIOS, xsink) : 0;
   if (!ios) {
      if (!*xsink)
         xsink->raiseException("TERMIOS-ISEQUAL-ERROR", "expecting a TermIOS object as argument to TermIOS::isEqual()");
      return 0;
   }
   ReferenceHolder<QoreTermIOS> holder(ios, xsink);
   return get_bool_node(s->is_equal(ios));
}

static AbstractQoreNode *f_TERMIOS_getWindowSize(const QoreListNode *params, ExceptionSink *xsink) {
   int rows, columns;

   if (QoreTermIOS::getWindowSize(rows, columns, xsink))
      return 0;

   QoreHashNode *rv = new QoreHashNode();
   rv->setKeyValue("rows", new QoreBigIntNode(rows), xsink);
   rv->setKeyValue("columns", new QoreBigIntNode(columns), xsink);
   return rv;
}

QoreClass *initTermIOSClass() {
   QORE_TRACE("initTermIOSClass()");

   // note that this class does not block therefore has no QDOM_THREAD
   QoreClass *QC_TERMIOS = new QoreClass("TermIOS", QDOM_TERMINAL_IO);
   CID_TERMIOS = QC_TERMIOS->getID();
   QC_TERMIOS->setConstructor(TERMIOS_constructor);
   QC_TERMIOS->setCopy((q_copy_t)TERMIOS_copy);

   QC_TERMIOS->addMethod("getLFlag", (q_method_t)TERMIOS_getLFlag);
   QC_TERMIOS->addMethod("getCFlag", (q_method_t)TERMIOS_getCFlag);
   QC_TERMIOS->addMethod("getOFlag", (q_method_t)TERMIOS_getOFlag);
   QC_TERMIOS->addMethod("getIFlag", (q_method_t)TERMIOS_getIFlag);
   QC_TERMIOS->addMethod("setLFlag", (q_method_t)TERMIOS_setLFlag);
   QC_TERMIOS->addMethod("setCFlag", (q_method_t)TERMIOS_setCFlag);
   QC_TERMIOS->addMethod("setOFlag", (q_method_t)TERMIOS_setOFlag);
   QC_TERMIOS->addMethod("setIFlag", (q_method_t)TERMIOS_setIFlag);
   QC_TERMIOS->addMethod("getCC",    (q_method_t)TERMIOS_getCC);
   QC_TERMIOS->addMethod("setCC",    (q_method_t)TERMIOS_setCC);
   QC_TERMIOS->addMethod("isEqual",  (q_method_t)TERMIOS_isEqual);

   // static methods
   QC_TERMIOS->addStaticMethod("getWindowSize", f_TERMIOS_getWindowSize);

   return QC_TERMIOS;
}
