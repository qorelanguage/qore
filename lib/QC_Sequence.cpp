/*
  QC_Sequence.cpp
  
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
#include <qore/intern/QC_Sequence.h>

qore_classid_t CID_SEQUENCE;

static void SEQUENCE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_SEQUENCE, new QoreSequence);
}

static void SEQUENCE_constructor_int(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_SEQUENCE, new QoreSequence((int)HARD_QORE_INT(params, 0)));
}

static void SEQUENCE_copy(QoreObject *self, QoreObject *old, QoreSequence *s, ExceptionSink *xsink) {
   self->setPrivate(CID_SEQUENCE, new QoreSequence(s->getCurrent()));
}

static AbstractQoreNode *SEQUENCE_next(QoreObject *self, QoreSequence *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->next());
}

static AbstractQoreNode *SEQUENCE_getCurrent(QoreObject *self, QoreSequence *s, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(s->getCurrent()); 
}

QoreClass *initSequenceClass() {
   QORE_TRACE("initSequenceClass()");

   // note that this does not block therefore has no QDOM_THREAD
   QoreClass *QC_SEQUENCE = new QoreClass("Sequence");
   CID_SEQUENCE = QC_SEQUENCE->getID();

   QC_SEQUENCE->setConstructorExtended(SEQUENCE_constructor);
   QC_SEQUENCE->setConstructorExtended(SEQUENCE_constructor_int, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_SEQUENCE->setCopy((q_copy_t)SEQUENCE_copy);

   QC_SEQUENCE->addMethodExtended("next",        (q_method_t)SEQUENCE_next, false, QC_NO_FLAGS, QDOM_DEFAULT, bigIntTypeInfo);
   QC_SEQUENCE->addMethodExtended("getCurrent",  (q_method_t)SEQUENCE_getCurrent, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   return QC_SEQUENCE;
}
