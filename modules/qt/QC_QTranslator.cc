/*
 QC_QTranslator.cc
 
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

#include "QC_QTranslator.h"
#include "QC_QObject.h"

#include "qore-qt.h"

int CID_QTRANSLATOR;
class QoreClass *QC_QTranslator = 0;

//QTranslator ( QObject * parent = 0 )
static void QTRANSLATOR_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTRANSLATOR, new QoreQTranslator(self, parent ? parent->getQObject() : 0));
   return;
}

static void QTRANSLATOR_copy(class QoreObject *self, class QoreObject *old, class QoreQTranslator *qt, ExceptionSink *xsink)
{
   xsink->raiseException("QTRANSLATOR-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual bool isEmpty () const
static QoreNode *QTRANSLATOR_isEmpty(QoreObject *self, QoreQTranslator *qt, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qt->qobj->isEmpty());
}

//bool load ( const QString & filename, const QString & directory = QString(), const QString & search_delimiters = QString(), const QString & suffix = QString() )
static QoreNode *QTRANSLATOR_load(QoreObject *self, QoreQTranslator *qt, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString filename;
   if (get_qstring(p, filename, xsink))
      return 0;
   p = get_param(params, 1);
   QString directory;
   if (get_qstring(p, directory, xsink, true))
      directory = QString();
   p = get_param(params, 2);
   QString search_delimiters;
   if (get_qstring(p, search_delimiters, xsink, true))
      search_delimiters = QString();
   p = get_param(params, 3);
   QString suffix;
   if (get_qstring(p, suffix, xsink, true))
      suffix = QString();
   return new QoreNode(qt->qobj->load(filename, directory, search_delimiters, suffix));
}

//virtual QString translate ( const char * context, const char * sourceText, const char * comment = 0 ) const
//QString translate ( const char * context, const char * sourceText, const char * comment, int n ) const
static QoreNode *QTRANSLATOR_translate(QoreObject *self, QoreQTranslator *qt, const QoreList *params, ExceptionSink *xsink)
{
   QoreStringNode *p = test_string_param(params, 0);
   if (!p) {
      xsink->raiseException("QTRANSLATOR-TRANSLATE-PARAM-ERROR", "expecting a string as first argument to QTranslator::translate()");
      return 0;
   }
   const char *context = p->getBuffer();

   p = test_string_param(params, 1);
   if (!p) {
      xsink->raiseException("QTRANSLATOR-TRANSLATE-PARAM-ERROR", "expecting a string as second argument to QTranslator::translate()");
      return 0;
   }
   const char *sourceText = p->getBuffer();

   p = test_string_param(params, 2);
   const char *comment = p ? p->getBuffer() : 0;

   if (num_params(params) < 4) 
      return new QoreStringNode(qt->qobj->translate(context, sourceText, comment).toUtf8().data(), QCS_UTF8);

   QoreNode *pn = get_param(params, 3);
   int n = pn ? pn->getAsInt() : 0;
   return new QoreStringNode(qt->qobj->translate(context, sourceText, comment, n).toUtf8().data(), QCS_UTF8);
}

QoreClass *initQTranslatorClass(QoreClass *qobject)
{
   QC_QTranslator = new QoreClass("QTranslator", QDOM_GUI);
   CID_QTRANSLATOR = QC_QTranslator->getID();

   QC_QTranslator->addBuiltinVirtualBaseClass(qobject);

   QC_QTranslator->setConstructor(QTRANSLATOR_constructor);
   QC_QTranslator->setCopy((q_copy_t)QTRANSLATOR_copy);

   QC_QTranslator->addMethod("isEmpty",                     (q_method_t)QTRANSLATOR_isEmpty);
   QC_QTranslator->addMethod("load",                        (q_method_t)QTRANSLATOR_load);
   QC_QTranslator->addMethod("translate",                   (q_method_t)QTRANSLATOR_translate);

   return QC_QTranslator;
}
