/*
 QC_QRegExpValidator.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

#include "QC_QRegExpValidator.h"
#include "QC_QObject.h"
#include "QC_QRegExp.h"

#include "qore-qt.h"

int CID_QREGEXPVALIDATOR;
class QoreClass *QC_QRegExpValidator = 0;

//QRegExpValidator ( QObject * parent )
//QRegExpValidator ( const QRegExp & rx, QObject * parent )
static void QREGEXPVALIDATOR_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQRegExp *rx = (QoreQRegExp *)p->val.object->getReferencedPrivateData(CID_QREGEXP, xsink);
      if (!rx) {
         QoreAbstractQObject *parent = (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink);
         if (!parent) {
            if (!xsink->isException())
               xsink->raiseException("QREGEXPVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "QRegExpValidator::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return;
         }
         ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
         self->setPrivate(CID_QREGEXPVALIDATOR, new QoreQRegExpValidator(self, parent->getQObject()));
         return;
      }
      ReferenceHolder<AbstractPrivateData> rxHolder(static_cast<AbstractPrivateData *>(rx), xsink);
      p = get_param(params, 1);
      QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (!parent) {
         if (!xsink->isException())
            xsink->raiseException("QREGEXPVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "this version of QRegExpValidator::constructor() expects an object derived from QObject as the second argument", p->val.object->getClass()->getName());
         return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QREGEXPVALIDATOR, new QoreQRegExpValidator(self, *(static_cast<QRegExp *>(rx)), parent->getQObject()));
      return;
   }
}

static void QREGEXPVALIDATOR_copy(class QoreObject *self, class QoreObject *old, class QoreQRegExpValidator *qrev, ExceptionSink *xsink)
{
   xsink->raiseException("QREGEXPVALIDATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//const QRegExp & regExp () const
static QoreNode *QREGEXPVALIDATOR_regExp(QoreObject *self, QoreQRegExpValidator *qrev, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qre = new QoreObject(QC_QRegExp, getProgram());
   QoreQRegExp *q_qre = new QoreQRegExp(qrev->qobj->regExp());
   o_qre->setPrivate(CID_QREGEXP, q_qre);
   return new QoreNode(o_qre);
}

//void setRegExp ( const QRegExp & rx )
static QoreNode *QREGEXPVALIDATOR_setRegExp(QoreObject *self, QoreQRegExpValidator *qrev, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRegExp *rx = (p && p->type == NT_OBJECT) ? (QoreQRegExp *)p->val.object->getReferencedPrivateData(CID_QREGEXP, xsink) : 0;
   if (!rx) {
      if (!xsink->isException())
         xsink->raiseException("QREGEXPVALIDATOR-SETREGEXP-PARAM-ERROR", "expecting a QRegExp object as first argument to QRegExpValidator::setRegExp()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rxHolder(static_cast<AbstractPrivateData *>(rx), xsink);
   qrev->qobj->setRegExp(*(static_cast<QRegExp *>(rx)));
   return 0;
}

QoreClass *initQRegExpValidatorClass(QoreClass *qvalidator)
{
   QC_QRegExpValidator = new QoreClass("QRegExpValidator", QDOM_GUI);
   CID_QREGEXPVALIDATOR = QC_QRegExpValidator->getID();

   QC_QRegExpValidator->addBuiltinVirtualBaseClass(qvalidator);

   QC_QRegExpValidator->setConstructor(QREGEXPVALIDATOR_constructor);
   QC_QRegExpValidator->setCopy((q_copy_t)QREGEXPVALIDATOR_copy);

   QC_QRegExpValidator->addMethod("regExp",                      (q_method_t)QREGEXPVALIDATOR_regExp);
   QC_QRegExpValidator->addMethod("setRegExp",                   (q_method_t)QREGEXPVALIDATOR_setRegExp);

   return QC_QRegExpValidator;
}
