/*
  QC_Sequence.cc
  
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
#include <qore/thread.h>
#include <qore/QC_Sequence.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_SEQUENCE;

static inline void *getSequence(void *obj)
{
   ((QoreSequence *)obj)->ROreference();
   return obj;
}

static class QoreNode *SEQUENCE_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int start;
   class QoreNode *p0 = get_param(params, 0);
   if (p0)
      start = p0->getAsInt();
   else
      start = 0;
   self->setPrivate(CID_SEQUENCE, new QoreSequence(start), getSequence);
   return NULL;
}

static class QoreNode *SEQUENCE_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreSequence *s = (QoreSequence *)self->getAndClearPrivateData(CID_SEQUENCE);
   if (s)
      s->deref();
   return NULL;
}

static class QoreNode *SEQUENCE_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreSequence *s = (QoreSequence *)get_param(params, 0)->val.object->getReferencedPrivateData(CID_SEQUENCE);
   if (s)
   {
      self->setPrivate(CID_SEQUENCE, new QoreSequence(s->getCurrent()), getSequence);
      s->deref();
   }
   else
      alreadyDeleted(xsink, "Sequence::copy");

   return NULL;
}

static class QoreNode *SEQUENCE_next(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreSequence *s = (QoreSequence *)self->getReferencedPrivateData(CID_SEQUENCE);
   class QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(NT_INT, s->next());
      s->deref();
   }
   else
   {
      alreadyDeleted(xsink, "Sequence::next");
      rv = NULL;
   }
   return rv;
}

static class QoreNode *SEQUENCE_getCurrent(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreSequence *s = (QoreSequence *)self->getReferencedPrivateData(CID_SEQUENCE);
   QoreNode *rv;
   if (s)
   {
      rv = new QoreNode(NT_INT, s->getCurrent()); 
      s->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Sequence::getCurrent");
   }
   return rv;
}

class QoreClass *initSequenceClass()
{
   tracein("initSequenceClass()");

   class QoreClass *QC_SEQUENCE = new QoreClass(strdup("Sequence"));
   CID_SEQUENCE = QC_SEQUENCE->getID();
   QC_SEQUENCE->addMethod("constructor",   SEQUENCE_constructor);
   QC_SEQUENCE->addMethod("destructor",    SEQUENCE_destructor);
   QC_SEQUENCE->addMethod("copy",          SEQUENCE_copy);
   QC_SEQUENCE->addMethod("next",          SEQUENCE_next);
   QC_SEQUENCE->addMethod("getCurrent",    SEQUENCE_getCurrent);

   traceout("initSequenceClass()");
   return QC_SEQUENCE;
}
