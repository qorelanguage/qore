/*
  QC_TermIOS.cc
  
  Qore Programming Language
  
  Copyright 2003 - 2008 David Nichols

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

QoreClass *initTermIOSClass() {
   QORE_TRACE("initTermIOSClass()");

   // note that this class does not block therefore has no QDOM_THREAD
   QoreClass *QC_TERMIOS = new QoreClass("TermIOS");
   CID_TERMIOS = QC_TERMIOS->getID();
   QC_TERMIOS->setConstructor(TERMIOS_constructor);
   QC_TERMIOS->setCopy((q_copy_t)TERMIOS_copy);

   QC_TERMIOS->addMethod("getLFLag", (q_method_t)TERMIOS_getLFlag);
   QC_TERMIOS->addMethod("getCFLag", (q_method_t)TERMIOS_getCFlag);
   QC_TERMIOS->addMethod("getOFLag", (q_method_t)TERMIOS_getOFlag);
   QC_TERMIOS->addMethod("getIFLag", (q_method_t)TERMIOS_getIFlag);
   QC_TERMIOS->addMethod("setLFLag", (q_method_t)TERMIOS_setLFlag);
   QC_TERMIOS->addMethod("setCFLag", (q_method_t)TERMIOS_setCFlag);
   QC_TERMIOS->addMethod("setOFLag", (q_method_t)TERMIOS_setOFlag);
   QC_TERMIOS->addMethod("setIFLag", (q_method_t)TERMIOS_setIFlag);

   return QC_TERMIOS;
}
