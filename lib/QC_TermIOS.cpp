/*
  QC_TermIOS.cpp
  
  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols

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
   self->setPrivate(CID_TERMIOS, new QoreTermIOS);
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
   s->set_lflag((int)HARD_QORE_INT(params, 0));
   return 0;
}

static AbstractQoreNode *TERMIOS_setCFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->set_cflag((int)HARD_QORE_INT(params, 0));
   return 0;
}

static AbstractQoreNode *TERMIOS_setOFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->set_oflag((int)HARD_QORE_INT(params, 0));
   return 0;
}

static AbstractQoreNode *TERMIOS_setIFlag(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->set_iflag((int)HARD_QORE_INT(params, 0));
   return 0;
}

static AbstractQoreNode *TERMIOS_getCC(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   int64 rc = s->get_cc(HARD_QORE_INT(params, 0), xsink);
   return *xsink ? 0 : new QoreBigIntNode(rc);
}

// nothing TermIOS::setCC(softint $offset = 0, softint $value = 0)  
static AbstractQoreNode *TERMIOS_setCC(QoreObject *self, QoreTermIOS *s, const QoreListNode *params, ExceptionSink *xsink) {
   s->set_cc(HARD_QORE_INT(params, 0), (cc_t)HARD_QORE_INT(params, 1), xsink);
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

   QoreHashNode *rv = new QoreHashNode;
   rv->setKeyValue("rows", new QoreBigIntNode(rows), xsink);
   rv->setKeyValue("columns", new QoreBigIntNode(columns), xsink);
   return rv;
}

QoreClass *initTermIOSClass() {
   QORE_TRACE("initTermIOSClass()");

   // note that this class does not block therefore has no QDOM_THREAD
   QoreClass *QC_TERMIOS = new QoreClass("TermIOS", QDOM_TERMINAL_IO);
   CID_TERMIOS = QC_TERMIOS->getID();

   QC_TERMIOS->setConstructorExtended(TERMIOS_constructor);

   QC_TERMIOS->setCopy((q_copy_t)TERMIOS_copy);

   QC_TERMIOS->addMethodExtended("getLFlag", (q_method_t)TERMIOS_getLFlag, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_TERMIOS->addMethodExtended("getCFlag", (q_method_t)TERMIOS_getCFlag, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_TERMIOS->addMethodExtended("getOFlag", (q_method_t)TERMIOS_getOFlag, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);
   QC_TERMIOS->addMethodExtended("getIFlag", (q_method_t)TERMIOS_getIFlag, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // nothing TermIOS::setLFlag(softint $flag = 0)  
   QC_TERMIOS->addMethodExtended("setLFlag", (q_method_t)TERMIOS_setLFlag, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // nothing TermIOS::setCFlag(softint $flag = 0)  
   QC_TERMIOS->addMethodExtended("setCFlag", (q_method_t)TERMIOS_setCFlag, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // nothing TermIOS::setOFlag(softint $flag = 0)  
   QC_TERMIOS->addMethodExtended("setOFlag", (q_method_t)TERMIOS_setOFlag, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // nothing TermIOS::setIFlag(softint $flag = 0)  
   QC_TERMIOS->addMethodExtended("setIFlag", (q_method_t)TERMIOS_setIFlag, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, zero());

   // int TermIOS::getCC(softint $offset = 0)  
   QC_TERMIOS->addMethodExtended("getCC",    (q_method_t)TERMIOS_getCC, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo, 1, softBigIntTypeInfo, zero());

   // nothing TermIOS::setCC(softint $offset = 0, softint $value = 0)  
   QC_TERMIOS->addMethodExtended("setCC",    (q_method_t)TERMIOS_setCC, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, softBigIntTypeInfo, zero(), softBigIntTypeInfo, zero());

   QC_TERMIOS->addMethodExtended("isEqual",  (q_method_t)TERMIOS_isEqual, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, QC_TERMIOS->getTypeInfo(), QORE_PARAM_NO_ARG);

   // static methods
   QC_TERMIOS->addStaticMethodExtended("getWindowSize", f_TERMIOS_getWindowSize, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, hashTypeInfo);

   return QC_TERMIOS;
}
