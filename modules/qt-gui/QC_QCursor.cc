/*
 QC_QCursor.cc
 
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

#include "QC_QCursor.h"
#include "QC_QPixmap.h"
#include "QC_QBitmap.h"
#include "QC_QPoint.h"

qore_classid_t CID_QCURSOR;
QoreClass *QC_QCursor = 0;

//QCursor ()
//QCursor ( Qt::CursorShape shape )
//QCursor ( const QBitmap & bitmap, const QBitmap & mask, int hotX = -1, int hotY = -1 )
//QCursor ( const QPixmap & pixmap, int hotX = -1, int hotY = -1 )
//QCursor ( const QCursor & c )
//QCursor ( HCURSOR cursor )
//QCursor ( Qt::HANDLE handle )
static void QCURSOR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QCURSOR, new QoreQCursor());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (!pixmap) {
         QoreQBitmap *bitmap = (QoreQBitmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QBITMAP, xsink);
         if (!bitmap) {
            if (!xsink->isException())
               xsink->raiseException("QCURSOR-CONSTRUCTOR-PARAM-ERROR", "QCursor::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return;
         }
         ReferenceHolder<AbstractPrivateData> bitmapHolder(static_cast<AbstractPrivateData *>(bitmap), xsink);
         p = get_param(params, 1);
         QoreQBitmap *mask = (p && p->getType() == NT_OBJECT) ? (QoreQBitmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QBITMAP, xsink) : 0;
         if (!mask) {
            if (!xsink->isException())
               xsink->raiseException("QCURSOR-CONSTRUCTOR-PARAM-ERROR", "this version of QCursor::constructor() expects an object derived from QBitmap as the second argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
            return;
         }
         ReferenceHolder<AbstractPrivateData> maskHolder(static_cast<AbstractPrivateData *>(mask), xsink);
	 p = get_param(params, 2);
	 int hotX = !is_nothing(p) ? p->getAsInt() : -1;
	 p = get_param(params, 3);
	 int hotY = !is_nothing(p) ? p->getAsInt() : -1;
	 self->setPrivate(CID_QCURSOR, new QoreQCursor(*(static_cast<QBitmap *>(bitmap)), *(static_cast<QBitmap *>(mask)), hotX, hotY));
	 return;
      }
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 1);
      int hotX = !is_nothing(p) ? p->getAsInt() : -1;
      p = get_param(params, 2);
      int hotY = !is_nothing(p) ? p->getAsInt() : -1;
      self->setPrivate(CID_QCURSOR, new QoreQCursor(*(static_cast<QPixmap *>(pixmap)), hotX, hotY));
      return;
   }
   Qt::CursorShape shape = (Qt::CursorShape)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QCURSOR, new QoreQCursor(shape));
   return;
}

static void QCURSOR_copy(QoreObject *self, QoreObject *old, QoreQCursor *qc, ExceptionSink *xsink)
{
   xsink->raiseException("QCURSOR-COPY-ERROR", "objects of this class cannot be copied");
}

//const QBitmap * bitmap () const
static AbstractQoreNode *QCURSOR_bitmap(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QBitmap, new QoreQBitmap(*qc->bitmap()));
}

/*
//HCURSOR_or_HANDLE handle () const
static AbstractQoreNode *QCURSOR_handle(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   ??? return new QoreBigIntNode(qc->handle());
}
*/

//QPoint hotSpot () const
static AbstractQoreNode *QCURSOR_hotSpot(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(qc->hotSpot()));
}

//const QBitmap * mask () const
static AbstractQoreNode *QCURSOR_mask(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QBitmap, new QoreQBitmap(*qc->mask()));
}

//QPixmap pixmap () const
static AbstractQoreNode *QCURSOR_pixmap(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPixmap, new QoreQPixmap(qc->pixmap()));
}

//void setShape ( Qt::CursorShape shape )
static AbstractQoreNode *QCURSOR_setShape(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::CursorShape shape = (Qt::CursorShape)(p ? p->getAsInt() : 0);
   qc->setShape(shape);
   return 0;
}

//Qt::CursorShape shape () const
static AbstractQoreNode *QCURSOR_shape(QoreObject *self, QoreQCursor *qc, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qc->shape());
}

QoreClass *initQCursorClass()
{
   QC_QCursor = new QoreClass("QCursor", QDOM_GUI);
   CID_QCURSOR = QC_QCursor->getID();

   QC_QCursor->setConstructor(QCURSOR_constructor);
   QC_QCursor->setCopy((q_copy_t)QCURSOR_copy);

   QC_QCursor->addMethod("bitmap",                      (q_method_t)QCURSOR_bitmap);
   //QC_QCursor->addMethod("handle",                      (q_method_t)QCURSOR_handle);
   QC_QCursor->addMethod("hotSpot",                     (q_method_t)QCURSOR_hotSpot);
   QC_QCursor->addMethod("mask",                        (q_method_t)QCURSOR_mask);
   QC_QCursor->addMethod("pixmap",                      (q_method_t)QCURSOR_pixmap);
   QC_QCursor->addMethod("setShape",                    (q_method_t)QCURSOR_setShape);
   QC_QCursor->addMethod("shape",                       (q_method_t)QCURSOR_shape);

   return QC_QCursor;
}

//QPoint pos ()
static AbstractQoreNode *f_QCursor_pos(const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPoint, new QoreQPoint(QCursor::pos()));
}

//void setPos ( int x, int y )
//void setPos ( const QPoint & p )
static AbstractQoreNode *f_QCursor_setPos(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QCURSOR-SETPOS-PARAM-ERROR", "QCursor::setPos() does not know how to handle arguments of class '%s' as passed as the first argument", reinterpret_cast<const QoreObject *>(p)->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QCursor::setPos(*(static_cast<QPoint *>(point)));
      return 0;
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   QCursor::setPos(x, y);
   return 0;
}

void initQCursorStaticFunctions()
{
   builtinFunctions.add("QCursor_pos",     f_QCursor_pos);
   builtinFunctions.add("QCursor_setPos",  f_QCursor_setPos);
}
