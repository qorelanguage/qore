/*
 QC_QValidator.cc
 
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

#include "QC_QValidator.h"

int CID_QVALIDATOR;
class QoreClass *QC_QValidator = 0;

// static class variables for local variable instantiation
char QoreQValidatorExtension::fixup_str[] = "format";
char QoreQValidatorExtension::validate_str[] = "validate";

//QValidator ( QObject * parent )
static void QVALIDATOR_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QVALIDATOR-CONSTRUCTOR-PARAM-ERROR", "expecting a QObject object as first argument to QValidator::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QVALIDATOR, new QoreQValidator(self, parent->getQObject()));
   return;
}

static void QVALIDATOR_copy(class Object *self, class Object *old, class QoreQValidator *qv, ExceptionSink *xsink)
{
   xsink->raiseException("QVALIDATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual void fixup ( QString & input ) const
/*
static QoreNode *QVALIDATOR_fixup(Object *self, QoreQValidator *qv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString input;
   if (get_qstring(p, input, xsink))
      return 0;
   qv->fixup(input);
   return 0;
}
*/

//QLocale locale () const
static QoreNode *QVALIDATOR_locale(Object *self, QoreQValidator *qv, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_ql = new Object(QC_QLocale, getProgram());
   QoreQLocale *q_ql = new QoreQLocale(qv->locale());
   o_ql->setPrivate(CID_QLOCALE, q_ql);
   return new QoreNode(o_ql);
}

//void setLocale ( const QLocale & locale )
static QoreNode *QVALIDATOR_setLocale(Object *self, QoreQValidator *qv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQLocale *locale = (p && p->type == NT_OBJECT) ? (QoreQLocale *)p->val.object->getReferencedPrivateData(CID_QLOCALE, xsink) : 0;
   if (!locale) {
      if (!xsink->isException())
         xsink->raiseException("QVALIDATOR-SETLOCALE-PARAM-ERROR", "expecting a QLocale object as first argument to QValidator::setLocale()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> localeHolder(static_cast<AbstractPrivateData *>(locale), xsink);
   qv->setLocale(*(static_cast<QLocale *>(locale)));
   return 0;
}

//virtual State validate ( QString & input, int & pos ) const = 0
/*
static QoreNode *QVALIDATOR_validate(Object *self, QoreQValidator *qv, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString input;
   if (get_qstring(p, input, xsink))
      return 0;
//   p = get_param(params, 1);
//   int pos = p ? p->getAsInt() : 0;
//   ??? return new QoreNode((int64)qv->validate(input, pos));
}
*/

QoreClass *initQValidatorClass()
{
   QC_QValidator = new QoreClass("QValidator", QDOM_GUI);
   CID_QVALIDATOR = QC_QValidator->getID();

   QC_QValidator->setConstructor(QVALIDATOR_constructor);
   QC_QValidator->setCopy((q_copy_t)QVALIDATOR_copy);

   //QC_QValidator->addMethod("fixup",                       (q_method_t)QVALIDATOR_fixup);
   QC_QValidator->addMethod("locale",                      (q_method_t)QVALIDATOR_locale);
   QC_QValidator->addMethod("setLocale",                   (q_method_t)QVALIDATOR_setLocale);
   //QC_QValidator->addMethod("validate",                    (q_method_t)QVALIDATOR_validate);

   return QC_QValidator;
}
