/*
 QC_QDragEnterEvent.cc
 
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

#include "QC_QDragEnterEvent.h"
#include "QC_QPoint.h"
#include "QC_QMimeData.h"

#include "qore-qt-gui.h"

qore_classid_t CID_QDRAGENTEREVENT;
class QoreClass *QC_QDragEnterEvent = 0;

//QDragEnterEvent ( const QPoint & point, Qt::DropActions actions, const QMimeData * data, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers )
static void QDRAGENTEREVENT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o = test_object_param(params, 0);
   QoreQPoint *point = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGENTEREVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as first argument to QDragEnterEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> pointHolder(point, xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);

   o = test_object_param(params, 2);
   QoreQMimeData *data = o ? (QoreQMimeData *)o->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
   if (!data) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGENTEREVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QMimeData object as third argument to QDragEnterEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQMimeData> dataHolder(data, xsink);
   p = get_param(params, 3);
   Qt::MouseButtons buttons = (Qt::MouseButtons)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QDRAGENTEREVENT, new QoreQDragEnterEvent(*(static_cast<QPoint *>(point)), actions, static_cast<QMimeData *>(data->qobj), buttons, modifiers));
   return;
}

static void QDRAGENTEREVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQDragEnterEvent *qdee, ExceptionSink *xsink)
{
   xsink->raiseException("QDRAGENTEREVENT-COPY-ERROR", "objects of this class cannot be copied");
}

QoreClass *initQDragEnterEventClass(QoreClass *qdragmoveevent)
{
   QC_QDragEnterEvent = new QoreClass("QDragEnterEvent", QDOM_GUI);
   CID_QDRAGENTEREVENT = QC_QDragEnterEvent->getID();

   QC_QDragEnterEvent->addBuiltinVirtualBaseClass(qdragmoveevent);

   QC_QDragEnterEvent->setConstructor(QDRAGENTEREVENT_constructor);
   QC_QDragEnterEvent->setCopy((q_copy_t)QDRAGENTEREVENT_copy);


   return QC_QDragEnterEvent;
}
