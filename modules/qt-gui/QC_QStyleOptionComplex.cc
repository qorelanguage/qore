/*
 QC_QStyleOptionComplex.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QStyleOptionComplex.h"
#include "QC_QStyleOption.h"

qore_classid_t CID_QSTYLEOPTIONCOMPLEX;
QoreClass *QC_QStyleOptionComplex = 0;

int QStyleOptionComplex_Notification(QoreObject *obj, QStyleOptionComplex *qsoc, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "activeSubControls")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyle::SubControls activeSubControls = (QStyle::SubControls)(p ? p->getAsInt() : 0);
      qsoc->activeSubControls = activeSubControls;
      return 0;
   }

   if (!strcmp(mem, "subControls")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyle::SubControls subControls = (QStyle::SubControls)(p ? p->getAsInt() : 0);
      qsoc->subControls = subControls;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOptionComplex_MemberGate(QStyleOptionComplex *qsoc, const char *mem)
{
   if (!strcmp(mem, "activeSubControls"))
      return new QoreBigIntNode(qsoc->activeSubControls);

   if (!strcmp(mem, "subControls"))
      return new QoreBigIntNode(qsoc->subControls);

   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONCOMPLEX_memberNotification(QoreObject *self, QoreQStyleOptionComplex *qsoc, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   if (!QStyleOptionComplex_Notification(self, qsoc, member, xsink) || *xsink)
      return 0;

   QStyleOption_Notification(self, qsoc, member, xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTIONCOMPLEX_memberGate(QoreObject *self, QoreQStyleOptionComplex *qsoc, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOptionComplex_MemberGate(qsoc, member);
   if (rv)
      return rv;

   return QStyleOption_MemberGate(qsoc, member);
}

//QStyleOptionComplex ( int version = QStyleOptionComplex::Version, int type = SO_Complex )
//QStyleOptionComplex ( const QStyleOptionComplex & other )
static void QSTYLEOPTIONCOMPLEX_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int version = !is_nothing(p) ? p->getAsInt() : QStyleOptionComplex::Version;
   p = get_param(params, 1);
   int type = !is_nothing(p) ? p->getAsInt() : QStyleOption::SO_Complex;
   self->setPrivate(CID_QSTYLEOPTIONCOMPLEX, new QoreQStyleOptionComplex(version, type));
   return;
}

static void QSTYLEOPTIONCOMPLEX_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOptionComplex *qsoc, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTIONCOMPLEX, new QoreQStyleOptionComplex(*qsoc));
}

QoreClass *initQStyleOptionComplexClass(QoreClass *qstyleoption)
{
   QC_QStyleOptionComplex = new QoreClass("QStyleOptionComplex", QDOM_GUI);
   CID_QSTYLEOPTIONCOMPLEX = QC_QStyleOptionComplex->getID();

   QC_QStyleOptionComplex->addBuiltinVirtualBaseClass(qstyleoption);

   QC_QStyleOptionComplex->setConstructor(QSTYLEOPTIONCOMPLEX_constructor);
   QC_QStyleOptionComplex->setCopy((q_copy_t)QSTYLEOPTIONCOMPLEX_copy);

   // special methods
   QC_QStyleOptionComplex->addMethod("memberNotification",  (q_method_t)QSTYLEOPTIONCOMPLEX_memberNotification);
   QC_QStyleOptionComplex->addMethod("memberGate",          (q_method_t)QSTYLEOPTIONCOMPLEX_memberGate);

   return QC_QStyleOptionComplex;
}
