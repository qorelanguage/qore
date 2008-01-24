/*
 QC_QDragMoveEvent.cc
 
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

#include "QC_QDragMoveEvent.h"
#include "QC_QPoint.h"
#include "QC_QMimeData.h"
#include "QC_QRect.h"

#include "qore-qt.h"

int CID_QDRAGMOVEEVENT;
class QoreClass *QC_QDragMoveEvent = 0;

//QDragMoveEvent ( const QPoint & pos, Qt::DropActions actions, const QMimeData * data, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Type type = DragMove )
static void QDRAGMOVEEVENT_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGMOVEEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QPoint object as first argument to QDragMoveEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   p = get_param(params, 1);
   Qt::DropActions actions = (Qt::DropActions)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   QoreQMimeData *data = (p && p->type == NT_OBJECT) ? (QoreQMimeData *)p->val.object->getReferencedPrivateData(CID_QMIMEDATA, xsink) : 0;
   if (!data) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGMOVEEVENT-CONSTRUCTOR-PARAM-ERROR", "expecting a QMimeData object as third argument to QDragMoveEvent::constructor()");
      return;
   }
   ReferenceHolder<QoreQMimeData> dataHolder(data, xsink);
   p = get_param(params, 3);
   Qt::MouseButtons buttons = (Qt::MouseButtons)(p ? p->getAsInt() : 0);
   p = get_param(params, 4);
   Qt::KeyboardModifiers modifiers = (Qt::KeyboardModifiers)(p ? p->getAsInt() : 0);
   p = get_param(params, 5);
   QDragMoveEvent::Type type = (QDragMoveEvent::Type)(!is_nothing(p) ? p->getAsInt() : QDragMoveEvent::DragMove);
   self->setPrivate(CID_QDRAGMOVEEVENT, new QoreQDragMoveEvent(*(static_cast<QPoint *>(pos)), actions, static_cast<QMimeData *>(data->qobj), buttons, modifiers, type));
   return;
}

static void QDRAGMOVEEVENT_copy(class QoreObject *self, class QoreObject *old, class QoreQDragMoveEvent *qdme, ExceptionSink *xsink)
{
   xsink->raiseException("QDRAGMOVEEVENT-COPY-ERROR", "objects of this class cannot be copied");
}

//void accept ( const QRect & rectangle )
//void accept ()
static QoreNode *QDRAGMOVEEVENT_accept(QoreObject *self, QoreQDragMoveEvent *qdme, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qdme->accept();
      return 0;
   }
   QoreQRect *rectangle = p ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGMOVEEVENT-ACCEPT-PARAM-ERROR", "this version of QDragMoveEvent::accept() expects an object derived from QRect as the first argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   qdme->accept(*(static_cast<QRect *>(rectangle)));
   return 0;
}

//QRect answerRect () const
static QoreNode *QDRAGMOVEEVENT_answerRect(QoreObject *self, QoreQDragMoveEvent *qdme, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qdme->answerRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void ignore ( const QRect & rectangle )
//void ignore ()
static QoreNode *QDRAGMOVEEVENT_ignore(QoreObject *self, QoreQDragMoveEvent *qdme, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      qdme->ignore();
      return 0;
   }
   QoreQRect *rectangle = p ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QDRAGMOVEEVENT-IGNORE-PARAM-ERROR", "this version of QDragMoveEvent::ignore() expects an object derived from QRect as the first argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   qdme->ignore(*(static_cast<QRect *>(rectangle)));
   return 0;
}

QoreClass *initQDragMoveEventClass(QoreClass *qdropevent)
{
   QC_QDragMoveEvent = new QoreClass("QDragMoveEvent", QDOM_GUI);
   CID_QDRAGMOVEEVENT = QC_QDragMoveEvent->getID();

   QC_QDragMoveEvent->addBuiltinVirtualBaseClass(qdropevent);

   QC_QDragMoveEvent->setConstructor(QDRAGMOVEEVENT_constructor);
   QC_QDragMoveEvent->setCopy((q_copy_t)QDRAGMOVEEVENT_copy);

   QC_QDragMoveEvent->addMethod("accept",                      (q_method_t)QDRAGMOVEEVENT_accept);
   QC_QDragMoveEvent->addMethod("answerRect",                  (q_method_t)QDRAGMOVEEVENT_answerRect);
   QC_QDragMoveEvent->addMethod("ignore",                      (q_method_t)QDRAGMOVEEVENT_ignore);

   return QC_QDragMoveEvent;
}
