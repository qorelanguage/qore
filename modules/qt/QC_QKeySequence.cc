/*
 QC_QKeySequence.cc
 
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
#include "QC_QKeySequence.h"

int CID_QKEYSEQUENCE;
QoreClass *QC_QKeySequence = 0;

static void QKEYSEQUENCE_constructor(class QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreQKeySequence *qks;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qks = new QoreQKeySequence();
   else if (p->type == NT_STRING)
      qks = new QoreQKeySequence((reinterpret_cast<QoreStringNode *>(p))->getBuffer());
   else {
      int i = p->getAsInt();

      p = get_param(params, 1);
      if (is_nothing(p))
	 qks = new QoreQKeySequence((QKeySequence::StandardKey)i);
      else {
	 int k2 = p->getAsInt();
	 p = get_param(params, 2);
	 int k3 = p ? p->getAsInt() : 0;
	 p = get_param(params, 3);
	 int k4 = p ? p->getAsInt() : 0;
	 qks = new QoreQKeySequence(i, k2, k3, k4);
      }
   }

   self->setPrivate(CID_QKEYSEQUENCE, qks);
}

static void QKEYSEQUENCE_copy(class QoreObject *self, class QoreObject *old, class QoreQKeySequence *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QKEYSEQUENCE, new QoreQKeySequence(*qf));
}

//uint count () const
static QoreNode *QKEYSEQUENCE_count(QoreObject *self, QoreQKeySequence *qks, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qks->count());
}

//DataPtr & data_ptr ()
//static QoreNode *QKEYSEQUENCE_data_ptr(QoreObject *self, QoreQKeySequence *qks, const QoreList *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qks->data_ptr());
//}

//bool isEmpty () const
static QoreNode *QKEYSEQUENCE_isEmpty(QoreObject *self, QoreQKeySequence *qks, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qks->isEmpty());
}

//SequenceMatch matches ( const QKeySequence & seq ) const
static QoreNode *QKEYSEQUENCE_matches(QoreObject *self, QoreQKeySequence *qks, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQKeySequence *seq = (p && p->type == NT_OBJECT) ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   if (!seq) {
      if (!xsink->isException())
         xsink->raiseException("QKEYSEQUENCE-MATCHES-PARAM-ERROR", "expecting a QKeySequence object as first argument to QKeySequence::matches()");
      return 0;
   }
   ReferenceHolder<QoreQKeySequence> holder(seq, xsink);
   return new QoreNode((int64)qks->matches(*(static_cast<QKeySequence *>(seq))));
}

//QString toString ( SequenceFormat format = PortableText ) const
static QoreNode *QKEYSEQUENCE_toString(QoreObject *self, QoreQKeySequence *qks, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QKeySequence::SequenceFormat format = (QKeySequence::SequenceFormat)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qks->toString(format).toUtf8().data(), QCS_UTF8);
}

class QoreClass *initQKeySequenceClass()
{
   tracein("initQKeySequenceClass()");
   
   QC_QKeySequence = new QoreClass("QKeySequence", QDOM_GUI);
   CID_QKEYSEQUENCE = QC_QKeySequence->getID();
   QC_QKeySequence->setConstructor(QKEYSEQUENCE_constructor);
   QC_QKeySequence->setCopy((q_copy_t)QKEYSEQUENCE_copy);

   QC_QKeySequence->addMethod("count",                       (q_method_t)QKEYSEQUENCE_count);
   //QC_QKeySequence->addMethod("data_ptr",                    (q_method_t)QKEYSEQUENCE_data_ptr);
   QC_QKeySequence->addMethod("isEmpty",                     (q_method_t)QKEYSEQUENCE_isEmpty);
   QC_QKeySequence->addMethod("matches",                     (q_method_t)QKEYSEQUENCE_matches);
   QC_QKeySequence->addMethod("toString",                    (q_method_t)QKEYSEQUENCE_toString);

   traceout("initQKeySequenceClass()");
   return QC_QKeySequence;
}
