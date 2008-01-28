/*
 QC_QCleanlooksStyle.cc
 
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

#include "QC_QCleanlooksStyle.h"

int CID_QCLEANLOOKSSTYLE;
class QoreClass *QC_QCleanlooksStyle = 0;

//QCleanlooksStyle ()
static void QCLEANLOOKSSTYLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QCLEANLOOKSSTYLE, new QoreQCleanlooksStyle(self));
   return;
}

static void QCLEANLOOKSSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQCleanlooksStyle *qcs, ExceptionSink *xsink)
{
   xsink->raiseException("QCLEANLOOKSSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const
static QoreNode *QCLEANLOOKSSTYLE_drawItemText(QoreObject *self, QoreAbstractQCleanlooksStyle *qcs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QCLEANLOOKSSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPainter object as first argument to QCleanlooksStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QCLEANLOOKSSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QRect object as second argument to QCleanlooksStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectangleHolder(static_cast<AbstractPrivateData *>(rectangle), xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QCLEANLOOKSSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPalette object as fourth argument to QCleanlooksStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> paletteHolder(static_cast<AbstractPrivateData *>(palette), xsink);
   p = get_param(params, 4);
   bool enabled = p ? p->getAsBool() : false;
   p = get_param(params, 5);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   p = get_param(params, 6);
   QPalette::ColorRole textRole = !is_nothing(p) ? (QPalette::ColorRole)p->getAsInt() : QPalette::NoRole;
   qcs->drawItemText(painter->getQPainter(), *(static_cast<QRect *>(rectangle)), alignment, *(palette->getQPalette()), enabled, text, textRole);
   return 0;
}

QoreClass *initQCleanlooksStyleClass(QoreClass *qwindowsstyle)
{
   QC_QCleanlooksStyle = new QoreClass("QCleanlooksStyle", QDOM_GUI);
   CID_QCLEANLOOKSSTYLE = QC_QCleanlooksStyle->getID();

   QC_QCleanlooksStyle->addBuiltinVirtualBaseClass(qwindowsstyle);

   QC_QCleanlooksStyle->setConstructor(QCLEANLOOKSSTYLE_constructor);
   QC_QCleanlooksStyle->setCopy((q_copy_t)QCLEANLOOKSSTYLE_copy);

   QC_QCleanlooksStyle->addMethod("drawItemText",                (q_method_t)QCLEANLOOKSSTYLE_drawItemText);

   return QC_QCleanlooksStyle;
}
