/*
 QC_QStyle.cc
 
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

#include "QC_QStyle.h"
#include "QC_QStyleOption.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

int CID_QSTYLE;
class QoreClass *QC_QStyle = 0;

//QStyle ()
static void QSTYLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLE, new QoreQStyle(self));
   return;
}

static void QSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQStyle *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//int combinedLayoutSpacing ( QSizePolicy::ControlTypes controls1, QSizePolicy::ControlTypes controls2, Qt::Orientation orientation, QStyleOption * option = 0, QWidget * widget = 0 ) const
static AbstractQoreNode *QSTYLE_combinedLayoutSpacing(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QSizePolicy::ControlTypes controls1 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlTypes controls2 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 3);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);

   o = test_object_param(params, 4);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreBigIntNode(qs->getQStyle()->combinedLayoutSpacing(controls1, controls2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_drawComplexControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_drawControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::ControlElement element = (QStyle::ControlElement)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawControl(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static AbstractQoreNode *QSTYLE_drawItemPixmap(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);

   o = test_object_param(params, 1);
   QoreQRect *rectangle = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);

   const AbstractQoreNode *p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;

   o = test_object_param(params, 3);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QPixmap object as fourth argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qs->drawItemPixmap(painter->getQPainter(), *(static_cast<QRect *>(rectangle)), alignment, *(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const
static AbstractQoreNode *QSTYLE_drawItemText(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   QoreQRect *rectangle = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);

   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;

   p = get_param(params, 3);
   QoreQPalette *palette = o ? (QoreQPalette *)o->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPalette object as fourth argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);

   p = get_param(params, 4);
   bool enabled = p ? p->getAsBool() : false;

   p = get_param(params, 5);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   p = get_param(params, 6);
   QPalette::ColorRole textRole = !is_nothing(p) ? (QPalette::ColorRole)p->getAsInt() : QPalette::NoRole;
   qs->drawItemText(painter->getQPainter(), *(static_cast<QRect *>(rectangle)), alignment, *(palette->getQPalette()), enabled, text, textRole);
   return 0;
}

//virtual void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_drawPrimitive(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::PrimitiveElement element = (QStyle::PrimitiveElement)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawPrimitive(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static AbstractQoreNode *QSTYLE_generatedIconPixmap(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);

   o = test_object_param(params, 2);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QStyleOption object as third argument to QStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qs->generatedIconPixmap(iconMode, *(static_cast<QPixmap *>(pixmap)), static_cast<QStyleOption *>(option)));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_hitTestComplexControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQPoint *position = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qs->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static AbstractQoreNode *QSTYLE_itemPixmapRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQRect *rectangle = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMPIXMAPRECT-PARAM-ERROR", "expecting a QRect object as first argument to QStyle::itemPixmapRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);

   const AbstractQoreNode *p = get_param(params, 1);
   int alignment = p ? p->getAsInt() : 0;

   o = test_object_param(params, 2);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMPIXMAPRECT-PARAM-ERROR", "expecting a QPixmap object as third argument to QStyle::itemPixmapRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->itemPixmapRect(*(static_cast<QRect *>(rectangle)), alignment, *(static_cast<QPixmap *>(pixmap))));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const
static AbstractQoreNode *QSTYLE_itemTextRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQFontMetrics *metrics = o ? (QoreQFontMetrics *)o->getReferencedPrivateData(CID_QFONTMETRICS, xsink) : 0;
   if (!metrics) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMTEXTRECT-PARAM-ERROR", "expecting a QFontMetrics object as first argument to QStyle::itemTextRect()");
      return 0;
   }
   ReferenceHolder<QoreQFontMetrics> metricsHolder(metrics, xsink);

   o = test_object_param(params, 1);
   QoreQRect *rectangle = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMTEXTRECT-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::itemTextRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);

   const AbstractQoreNode *p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;

   p = get_param(params, 3);
   bool enabled = p ? p->getAsBool() : false;

   p = get_param(params, 4);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->itemTextRect(*(static_cast<QFontMetrics *>(metrics)), *(static_cast<QRect *>(rectangle)), alignment, enabled, text));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//int layoutSpacing ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static AbstractQoreNode *QSTYLE_layoutSpacing(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);

   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);

   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 3);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qs->getQStyle()->layoutSpacing(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_pixelMetric(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::PixelMetric metric = (QStyle::PixelMetric)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qs->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual void polish ( QWidget * widget )
//virtual void polish ( QApplication * application )
//virtual void polish ( QPalette & palette )
static AbstractQoreNode *QSTYLE_polish(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   if (!o) {
      xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() was expecting a QApplication, QPalette, or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)o->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQPalette *palette = (QoreQPalette *)o->getReferencedPrivateData(CID_QPALETTE, xsink);
      if (!palette) {
	 QoreQWidget *widget = (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink);
	 if (!widget) {
	    if (!xsink->isException())
	       xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
	    return 0;
	 }
	 ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
	 qs->polish(static_cast<QWidget *>(widget->getQWidget()));
	 return 0;
      }
      ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);
      qs->polish(*(palette->getQPalette()));
      return 0;
   }
   ReferenceHolder<QoreQApplication> applicationHolder(application, xsink);
   qs->polish(static_cast<QApplication *>(application->qobj));
   return 0;
}

//virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_sizeFromContents(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::ContentsType type = (QStyle::ContentsType)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQSize *contentsSize = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qs->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//QIcon standardIcon ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static AbstractQoreNode *QSTYLE_standardIcon(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);

   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qs->getQStyle()->standardIcon(standardIcon, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//virtual QPalette standardPalette () const
static AbstractQoreNode *QSTYLE_standardPalette(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qs->standardPalette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static AbstractQoreNode *QSTYLE_styleHint(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::StyleHint hint = (QStyle::StyleHint)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
//   p = get_param(params, 3);
//   ??? QStyleHintReturn* returnData = p;
   return new QoreBigIntNode(qs->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_subControlRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   p = get_param(params, 2);
   QStyle::SubControl subControl = (QStyle::SubControl)(p ? p->getAsInt() : 0);

   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QSTYLE_subElementRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::SubElement element = (QStyle::SubElement)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);

   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->subElementRect(element, static_cast<QStyleOption *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual void unpolish ( QWidget * widget )
//virtual void unpolish ( QApplication * application )
static AbstractQoreNode *QSTYLE_unpolish(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   if (!o) {
      xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() was expecting a QApplication or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)o->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQWidget *widget = (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
	 if (!xsink->isException())
	    xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
	 return 0;
      }
      ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
      qs->unpolish(static_cast<QWidget *>(widget->getQWidget()));
      return 0;
   }
   ReferenceHolder<QoreQApplication> applicationHolder(application, xsink);
   qs->unpolish(static_cast<QApplication *>(application->qobj));
   return 0;
}

//int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static AbstractQoreNode *QSTYLE_layoutSpacingImplementation(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 3);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);

   o = test_object_param(params, 4);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreBigIntNode(qs->layoutSpacingImplementation(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//QIcon standardIconImplementation ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static AbstractQoreNode *QSTYLE_standardIconImplementation(QoreObject *self, QoreAbstractQStyle *qs, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);

   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);

   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qs->standardIconImplementation(standardIcon, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

static QoreClass *initQStyleClass(QoreClass *qobject)
{
   QC_QStyle = new QoreClass("QStyle", QDOM_GUI);
   CID_QSTYLE = QC_QStyle->getID();

   QC_QStyle->addBuiltinVirtualBaseClass(qobject);

   QC_QStyle->setConstructor(QSTYLE_constructor);
   QC_QStyle->setCopy((q_copy_t)QSTYLE_copy);

   QC_QStyle->addMethod("combinedLayoutSpacing",       (q_method_t)QSTYLE_combinedLayoutSpacing);
   QC_QStyle->addMethod("drawComplexControl",          (q_method_t)QSTYLE_drawComplexControl);
   QC_QStyle->addMethod("drawControl",                 (q_method_t)QSTYLE_drawControl);
   QC_QStyle->addMethod("drawItemPixmap",              (q_method_t)QSTYLE_drawItemPixmap);
   QC_QStyle->addMethod("drawItemText",                (q_method_t)QSTYLE_drawItemText);
   QC_QStyle->addMethod("drawPrimitive",               (q_method_t)QSTYLE_drawPrimitive);
   QC_QStyle->addMethod("generatedIconPixmap",         (q_method_t)QSTYLE_generatedIconPixmap);
   QC_QStyle->addMethod("hitTestComplexControl",       (q_method_t)QSTYLE_hitTestComplexControl);
   QC_QStyle->addMethod("itemPixmapRect",              (q_method_t)QSTYLE_itemPixmapRect);
   QC_QStyle->addMethod("itemTextRect",                (q_method_t)QSTYLE_itemTextRect);
   QC_QStyle->addMethod("layoutSpacing",               (q_method_t)QSTYLE_layoutSpacing);
   QC_QStyle->addMethod("pixelMetric",                 (q_method_t)QSTYLE_pixelMetric);
   QC_QStyle->addMethod("polish",                      (q_method_t)QSTYLE_polish);
   QC_QStyle->addMethod("sizeFromContents",            (q_method_t)QSTYLE_sizeFromContents);
   QC_QStyle->addMethod("standardIcon",                (q_method_t)QSTYLE_standardIcon);
   QC_QStyle->addMethod("standardPalette",             (q_method_t)QSTYLE_standardPalette);
   QC_QStyle->addMethod("styleHint",                   (q_method_t)QSTYLE_styleHint);
   QC_QStyle->addMethod("subControlRect",              (q_method_t)QSTYLE_subControlRect);
   QC_QStyle->addMethod("subElementRect",              (q_method_t)QSTYLE_subElementRect);
   QC_QStyle->addMethod("unpolish",                    (q_method_t)QSTYLE_unpolish);

   QC_QStyle->addMethod("layoutSpacingImplementation", (q_method_t)QSTYLE_layoutSpacingImplementation, true);
   QC_QStyle->addMethod("standardIconImplementation",  (q_method_t)QSTYLE_standardIconImplementation, true);

   return QC_QStyle;
}

QoreNamespace *initQStyleNS(QoreClass *qobject)
{ 
   QoreNamespace *qstyle_ns = new QoreNamespace("QStyle");

   QoreClass *qstyle, *qmotifstyle, *qwindowsstyle;

   qstyle_ns->addSystemClass((qstyle = initQStyleClass(qobject)));
   qstyle_ns->addSystemClass((qmotifstyle = initQMotifStyleClass(qstyle)));
   qstyle_ns->addSystemClass(initQCDEStyleClass(qmotifstyle));
   qstyle_ns->addSystemClass((qwindowsstyle = initQWindowsStyleClass(qstyle)));
   qstyle_ns->addSystemClass(initQCleanlooksStyleClass(qwindowsstyle));
   qstyle_ns->addSystemClass(initQPlastiqueStyleClass(qwindowsstyle));
#ifdef DARWIN
   qstyle_ns->addSystemClass(initQMacStyleClass(qwindowsstyle));
#endif
#ifdef WINDOWS
   qstyle_ns->addSystemClass(initQWindowsXPStyleClass(qwindowsstyle));
#endif

   // PrimitiveElement enum
   qstyle_ns->addConstant("PE_Q3CheckListController", new QoreBigIntNode(QStyle::PE_Q3CheckListController));
   qstyle_ns->addConstant("PE_Q3CheckListExclusiveIndicator", new QoreBigIntNode(QStyle::PE_Q3CheckListExclusiveIndicator));
   qstyle_ns->addConstant("PE_Q3CheckListIndicator",  new QoreBigIntNode(QStyle::PE_Q3CheckListIndicator));
   qstyle_ns->addConstant("PE_Q3DockWindowSeparator", new QoreBigIntNode(QStyle::PE_Q3DockWindowSeparator));
   qstyle_ns->addConstant("PE_Q3Separator",           new QoreBigIntNode(QStyle::PE_Q3Separator));
   qstyle_ns->addConstant("PE_Frame",                 new QoreBigIntNode(QStyle::PE_Frame));
   qstyle_ns->addConstant("PE_FrameDefaultButton",    new QoreBigIntNode(QStyle::PE_FrameDefaultButton));
   qstyle_ns->addConstant("PE_FrameDockWidget",       new QoreBigIntNode(QStyle::PE_FrameDockWidget));
   qstyle_ns->addConstant("PE_FrameFocusRect",        new QoreBigIntNode(QStyle::PE_FrameFocusRect));
   qstyle_ns->addConstant("PE_FrameGroupBox",         new QoreBigIntNode(QStyle::PE_FrameGroupBox));
   qstyle_ns->addConstant("PE_FrameLineEdit",         new QoreBigIntNode(QStyle::PE_FrameLineEdit));
   qstyle_ns->addConstant("PE_FrameMenu",             new QoreBigIntNode(QStyle::PE_FrameMenu));
   qstyle_ns->addConstant("PE_FrameStatusBar",        new QoreBigIntNode(QStyle::PE_FrameStatusBar));
   qstyle_ns->addConstant("PE_FrameTabWidget",        new QoreBigIntNode(QStyle::PE_FrameTabWidget));
   qstyle_ns->addConstant("PE_FrameWindow",           new QoreBigIntNode(QStyle::PE_FrameWindow));
   qstyle_ns->addConstant("PE_FrameButtonBevel",      new QoreBigIntNode(QStyle::PE_FrameButtonBevel));
   qstyle_ns->addConstant("PE_FrameButtonTool",       new QoreBigIntNode(QStyle::PE_FrameButtonTool));
   qstyle_ns->addConstant("PE_FrameTabBarBase",       new QoreBigIntNode(QStyle::PE_FrameTabBarBase));
   qstyle_ns->addConstant("PE_PanelButtonCommand",    new QoreBigIntNode(QStyle::PE_PanelButtonCommand));
   qstyle_ns->addConstant("PE_PanelButtonBevel",      new QoreBigIntNode(QStyle::PE_PanelButtonBevel));
   qstyle_ns->addConstant("PE_PanelButtonTool",       new QoreBigIntNode(QStyle::PE_PanelButtonTool));
   qstyle_ns->addConstant("PE_PanelMenuBar",          new QoreBigIntNode(QStyle::PE_PanelMenuBar));
   qstyle_ns->addConstant("PE_PanelToolBar",          new QoreBigIntNode(QStyle::PE_PanelToolBar));
   qstyle_ns->addConstant("PE_PanelLineEdit",         new QoreBigIntNode(QStyle::PE_PanelLineEdit));
   qstyle_ns->addConstant("PE_IndicatorArrowDown",    new QoreBigIntNode(QStyle::PE_IndicatorArrowDown));
   qstyle_ns->addConstant("PE_IndicatorArrowLeft",    new QoreBigIntNode(QStyle::PE_IndicatorArrowLeft));
   qstyle_ns->addConstant("PE_IndicatorArrowRight",   new QoreBigIntNode(QStyle::PE_IndicatorArrowRight));
   qstyle_ns->addConstant("PE_IndicatorArrowUp",      new QoreBigIntNode(QStyle::PE_IndicatorArrowUp));
   qstyle_ns->addConstant("PE_IndicatorBranch",       new QoreBigIntNode(QStyle::PE_IndicatorBranch));
   qstyle_ns->addConstant("PE_IndicatorButtonDropDown", new QoreBigIntNode(QStyle::PE_IndicatorButtonDropDown));
   qstyle_ns->addConstant("PE_IndicatorViewItemCheck", new QoreBigIntNode(QStyle::PE_IndicatorViewItemCheck));
   qstyle_ns->addConstant("PE_IndicatorCheckBox",     new QoreBigIntNode(QStyle::PE_IndicatorCheckBox));
   qstyle_ns->addConstant("PE_IndicatorDockWidgetResizeHandle", new QoreBigIntNode(QStyle::PE_IndicatorDockWidgetResizeHandle));
   qstyle_ns->addConstant("PE_IndicatorHeaderArrow",  new QoreBigIntNode(QStyle::PE_IndicatorHeaderArrow));
   qstyle_ns->addConstant("PE_IndicatorMenuCheckMark", new QoreBigIntNode(QStyle::PE_IndicatorMenuCheckMark));
   qstyle_ns->addConstant("PE_IndicatorProgressChunk", new QoreBigIntNode(QStyle::PE_IndicatorProgressChunk));
   qstyle_ns->addConstant("PE_IndicatorRadioButton",  new QoreBigIntNode(QStyle::PE_IndicatorRadioButton));
   qstyle_ns->addConstant("PE_IndicatorSpinDown",     new QoreBigIntNode(QStyle::PE_IndicatorSpinDown));
   qstyle_ns->addConstant("PE_IndicatorSpinMinus",    new QoreBigIntNode(QStyle::PE_IndicatorSpinMinus));
   qstyle_ns->addConstant("PE_IndicatorSpinPlus",     new QoreBigIntNode(QStyle::PE_IndicatorSpinPlus));
   qstyle_ns->addConstant("PE_IndicatorSpinUp",       new QoreBigIntNode(QStyle::PE_IndicatorSpinUp));
   qstyle_ns->addConstant("PE_IndicatorToolBarHandle", new QoreBigIntNode(QStyle::PE_IndicatorToolBarHandle));
   qstyle_ns->addConstant("PE_IndicatorToolBarSeparator", new QoreBigIntNode(QStyle::PE_IndicatorToolBarSeparator));
   qstyle_ns->addConstant("PE_PanelTipLabel",         new QoreBigIntNode(QStyle::PE_PanelTipLabel));
   qstyle_ns->addConstant("PE_IndicatorTabTear",      new QoreBigIntNode(QStyle::PE_IndicatorTabTear));
   qstyle_ns->addConstant("PE_PanelScrollAreaCorner", new QoreBigIntNode(QStyle::PE_PanelScrollAreaCorner));
   qstyle_ns->addConstant("PE_Widget",                new QoreBigIntNode(QStyle::PE_Widget));
   qstyle_ns->addConstant("PE_IndicatorColumnViewArrow", new QoreBigIntNode(QStyle::PE_IndicatorColumnViewArrow));
   qstyle_ns->addConstant("PE_CustomBase",            new QoreBigIntNode(QStyle::PE_CustomBase));

   // PixelMetric enum
   qstyle_ns->addConstant("PM_ButtonMargin",          new QoreBigIntNode(QStyle::PM_ButtonMargin));
   qstyle_ns->addConstant("PM_ButtonDefaultIndicator", new QoreBigIntNode(QStyle::PM_ButtonDefaultIndicator));
   qstyle_ns->addConstant("PM_MenuButtonIndicator",   new QoreBigIntNode(QStyle::PM_MenuButtonIndicator));
   qstyle_ns->addConstant("PM_ButtonShiftHorizontal", new QoreBigIntNode(QStyle::PM_ButtonShiftHorizontal));
   qstyle_ns->addConstant("PM_ButtonShiftVertical",   new QoreBigIntNode(QStyle::PM_ButtonShiftVertical));
   qstyle_ns->addConstant("PM_DefaultFrameWidth",     new QoreBigIntNode(QStyle::PM_DefaultFrameWidth));
   qstyle_ns->addConstant("PM_SpinBoxFrameWidth",     new QoreBigIntNode(QStyle::PM_SpinBoxFrameWidth));
   qstyle_ns->addConstant("PM_ComboBoxFrameWidth",    new QoreBigIntNode(QStyle::PM_ComboBoxFrameWidth));
   qstyle_ns->addConstant("PM_MaximumDragDistance",   new QoreBigIntNode(QStyle::PM_MaximumDragDistance));
   qstyle_ns->addConstant("PM_ScrollBarExtent",       new QoreBigIntNode(QStyle::PM_ScrollBarExtent));
   qstyle_ns->addConstant("PM_ScrollBarSliderMin",    new QoreBigIntNode(QStyle::PM_ScrollBarSliderMin));
   qstyle_ns->addConstant("PM_SliderThickness",       new QoreBigIntNode(QStyle::PM_SliderThickness));
   qstyle_ns->addConstant("PM_SliderControlThickness", new QoreBigIntNode(QStyle::PM_SliderControlThickness));
   qstyle_ns->addConstant("PM_SliderLength",          new QoreBigIntNode(QStyle::PM_SliderLength));
   qstyle_ns->addConstant("PM_SliderTickmarkOffset",  new QoreBigIntNode(QStyle::PM_SliderTickmarkOffset));
   qstyle_ns->addConstant("PM_SliderSpaceAvailable",  new QoreBigIntNode(QStyle::PM_SliderSpaceAvailable));
   qstyle_ns->addConstant("PM_DockWidgetSeparatorExtent", new QoreBigIntNode(QStyle::PM_DockWidgetSeparatorExtent));
   qstyle_ns->addConstant("PM_DockWidgetHandleExtent", new QoreBigIntNode(QStyle::PM_DockWidgetHandleExtent));
   qstyle_ns->addConstant("PM_DockWidgetFrameWidth",  new QoreBigIntNode(QStyle::PM_DockWidgetFrameWidth));
   qstyle_ns->addConstant("PM_TabBarTabOverlap",      new QoreBigIntNode(QStyle::PM_TabBarTabOverlap));
   qstyle_ns->addConstant("PM_TabBarTabHSpace",       new QoreBigIntNode(QStyle::PM_TabBarTabHSpace));
   qstyle_ns->addConstant("PM_TabBarTabVSpace",       new QoreBigIntNode(QStyle::PM_TabBarTabVSpace));
   qstyle_ns->addConstant("PM_TabBarBaseHeight",      new QoreBigIntNode(QStyle::PM_TabBarBaseHeight));
   qstyle_ns->addConstant("PM_TabBarBaseOverlap",     new QoreBigIntNode(QStyle::PM_TabBarBaseOverlap));
   qstyle_ns->addConstant("PM_ProgressBarChunkWidth", new QoreBigIntNode(QStyle::PM_ProgressBarChunkWidth));
   qstyle_ns->addConstant("PM_SplitterWidth",         new QoreBigIntNode(QStyle::PM_SplitterWidth));
   qstyle_ns->addConstant("PM_TitleBarHeight",        new QoreBigIntNode(QStyle::PM_TitleBarHeight));
   qstyle_ns->addConstant("PM_MenuScrollerHeight",    new QoreBigIntNode(QStyle::PM_MenuScrollerHeight));
   qstyle_ns->addConstant("PM_MenuHMargin",           new QoreBigIntNode(QStyle::PM_MenuHMargin));
   qstyle_ns->addConstant("PM_MenuVMargin",           new QoreBigIntNode(QStyle::PM_MenuVMargin));
   qstyle_ns->addConstant("PM_MenuPanelWidth",        new QoreBigIntNode(QStyle::PM_MenuPanelWidth));
   qstyle_ns->addConstant("PM_MenuTearoffHeight",     new QoreBigIntNode(QStyle::PM_MenuTearoffHeight));
   qstyle_ns->addConstant("PM_MenuDesktopFrameWidth", new QoreBigIntNode(QStyle::PM_MenuDesktopFrameWidth));
   qstyle_ns->addConstant("PM_MenuBarPanelWidth",     new QoreBigIntNode(QStyle::PM_MenuBarPanelWidth));
   qstyle_ns->addConstant("PM_MenuBarItemSpacing",    new QoreBigIntNode(QStyle::PM_MenuBarItemSpacing));
   qstyle_ns->addConstant("PM_MenuBarVMargin",        new QoreBigIntNode(QStyle::PM_MenuBarVMargin));
   qstyle_ns->addConstant("PM_MenuBarHMargin",        new QoreBigIntNode(QStyle::PM_MenuBarHMargin));
   qstyle_ns->addConstant("PM_IndicatorWidth",        new QoreBigIntNode(QStyle::PM_IndicatorWidth));
   qstyle_ns->addConstant("PM_IndicatorHeight",       new QoreBigIntNode(QStyle::PM_IndicatorHeight));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorWidth", new QoreBigIntNode(QStyle::PM_ExclusiveIndicatorWidth));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorHeight", new QoreBigIntNode(QStyle::PM_ExclusiveIndicatorHeight));
   qstyle_ns->addConstant("PM_CheckListButtonSize",   new QoreBigIntNode(QStyle::PM_CheckListButtonSize));
   qstyle_ns->addConstant("PM_CheckListControllerSize", new QoreBigIntNode(QStyle::PM_CheckListControllerSize));
   qstyle_ns->addConstant("PM_DialogButtonsSeparator", new QoreBigIntNode(QStyle::PM_DialogButtonsSeparator));
   qstyle_ns->addConstant("PM_DialogButtonsButtonWidth", new QoreBigIntNode(QStyle::PM_DialogButtonsButtonWidth));
   qstyle_ns->addConstant("PM_DialogButtonsButtonHeight", new QoreBigIntNode(QStyle::PM_DialogButtonsButtonHeight));
   qstyle_ns->addConstant("PM_MdiSubWindowFrameWidth", new QoreBigIntNode(QStyle::PM_MdiSubWindowFrameWidth));
   qstyle_ns->addConstant("PM_MDIFrameWidth",         new QoreBigIntNode(QStyle::PM_MDIFrameWidth));
   qstyle_ns->addConstant("PM_MdiSubWindowMinimizedWidth", new QoreBigIntNode(QStyle::PM_MdiSubWindowMinimizedWidth));
   qstyle_ns->addConstant("PM_MDIMinimizedWidth",     new QoreBigIntNode(QStyle::PM_MDIMinimizedWidth));
   qstyle_ns->addConstant("PM_HeaderMargin",          new QoreBigIntNode(QStyle::PM_HeaderMargin));
   qstyle_ns->addConstant("PM_HeaderMarkSize",        new QoreBigIntNode(QStyle::PM_HeaderMarkSize));
   qstyle_ns->addConstant("PM_HeaderGripMargin",      new QoreBigIntNode(QStyle::PM_HeaderGripMargin));
   qstyle_ns->addConstant("PM_TabBarTabShiftHorizontal", new QoreBigIntNode(QStyle::PM_TabBarTabShiftHorizontal));
   qstyle_ns->addConstant("PM_TabBarTabShiftVertical", new QoreBigIntNode(QStyle::PM_TabBarTabShiftVertical));
   qstyle_ns->addConstant("PM_TabBarScrollButtonWidth", new QoreBigIntNode(QStyle::PM_TabBarScrollButtonWidth));
   qstyle_ns->addConstant("PM_ToolBarFrameWidth",     new QoreBigIntNode(QStyle::PM_ToolBarFrameWidth));
   qstyle_ns->addConstant("PM_ToolBarHandleExtent",   new QoreBigIntNode(QStyle::PM_ToolBarHandleExtent));
   qstyle_ns->addConstant("PM_ToolBarItemSpacing",    new QoreBigIntNode(QStyle::PM_ToolBarItemSpacing));
   qstyle_ns->addConstant("PM_ToolBarItemMargin",     new QoreBigIntNode(QStyle::PM_ToolBarItemMargin));
   qstyle_ns->addConstant("PM_ToolBarSeparatorExtent", new QoreBigIntNode(QStyle::PM_ToolBarSeparatorExtent));
   qstyle_ns->addConstant("PM_ToolBarExtensionExtent", new QoreBigIntNode(QStyle::PM_ToolBarExtensionExtent));
   qstyle_ns->addConstant("PM_SpinBoxSliderHeight",   new QoreBigIntNode(QStyle::PM_SpinBoxSliderHeight));
   qstyle_ns->addConstant("PM_DefaultTopLevelMargin", new QoreBigIntNode(QStyle::PM_DefaultTopLevelMargin));
   qstyle_ns->addConstant("PM_DefaultChildMargin",    new QoreBigIntNode(QStyle::PM_DefaultChildMargin));
   qstyle_ns->addConstant("PM_DefaultLayoutSpacing",  new QoreBigIntNode(QStyle::PM_DefaultLayoutSpacing));
   qstyle_ns->addConstant("PM_ToolBarIconSize",       new QoreBigIntNode(QStyle::PM_ToolBarIconSize));
   qstyle_ns->addConstant("PM_ListViewIconSize",      new QoreBigIntNode(QStyle::PM_ListViewIconSize));
   qstyle_ns->addConstant("PM_IconViewIconSize",      new QoreBigIntNode(QStyle::PM_IconViewIconSize));
   qstyle_ns->addConstant("PM_SmallIconSize",         new QoreBigIntNode(QStyle::PM_SmallIconSize));
   qstyle_ns->addConstant("PM_LargeIconSize",         new QoreBigIntNode(QStyle::PM_LargeIconSize));
   qstyle_ns->addConstant("PM_FocusFrameVMargin",     new QoreBigIntNode(QStyle::PM_FocusFrameVMargin));
   qstyle_ns->addConstant("PM_FocusFrameHMargin",     new QoreBigIntNode(QStyle::PM_FocusFrameHMargin));
   qstyle_ns->addConstant("PM_ToolTipLabelFrameWidth", new QoreBigIntNode(QStyle::PM_ToolTipLabelFrameWidth));
   qstyle_ns->addConstant("PM_CheckBoxLabelSpacing",  new QoreBigIntNode(QStyle::PM_CheckBoxLabelSpacing));
   qstyle_ns->addConstant("PM_TabBarIconSize",        new QoreBigIntNode(QStyle::PM_TabBarIconSize));
   qstyle_ns->addConstant("PM_SizeGripSize",          new QoreBigIntNode(QStyle::PM_SizeGripSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleMargin", new QoreBigIntNode(QStyle::PM_DockWidgetTitleMargin));
   qstyle_ns->addConstant("PM_MessageBoxIconSize",    new QoreBigIntNode(QStyle::PM_MessageBoxIconSize));
   qstyle_ns->addConstant("PM_ButtonIconSize",        new QoreBigIntNode(QStyle::PM_ButtonIconSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleBarButtonMargin", new QoreBigIntNode(QStyle::PM_DockWidgetTitleBarButtonMargin));
   qstyle_ns->addConstant("PM_RadioButtonLabelSpacing", new QoreBigIntNode(QStyle::PM_RadioButtonLabelSpacing));
   qstyle_ns->addConstant("PM_LayoutLeftMargin",      new QoreBigIntNode(QStyle::PM_LayoutLeftMargin));
   qstyle_ns->addConstant("PM_LayoutTopMargin",       new QoreBigIntNode(QStyle::PM_LayoutTopMargin));
   qstyle_ns->addConstant("PM_LayoutRightMargin",     new QoreBigIntNode(QStyle::PM_LayoutRightMargin));
   qstyle_ns->addConstant("PM_LayoutBottomMargin",    new QoreBigIntNode(QStyle::PM_LayoutBottomMargin));
   qstyle_ns->addConstant("PM_LayoutHorizontalSpacing", new QoreBigIntNode(QStyle::PM_LayoutHorizontalSpacing));
   qstyle_ns->addConstant("PM_LayoutVerticalSpacing", new QoreBigIntNode(QStyle::PM_LayoutVerticalSpacing));
   qstyle_ns->addConstant("PM_CustomBase",            new QoreBigIntNode(QStyle::PM_CustomBase));

   // StateFlag enum
   qstyle_ns->addConstant("State_None",               new QoreBigIntNode(QStyle::State_None));
   qstyle_ns->addConstant("State_Enabled",            new QoreBigIntNode(QStyle::State_Enabled));
   qstyle_ns->addConstant("State_Raised",             new QoreBigIntNode(QStyle::State_Raised));
   qstyle_ns->addConstant("State_Sunken",             new QoreBigIntNode(QStyle::State_Sunken));
   qstyle_ns->addConstant("State_Off",                new QoreBigIntNode(QStyle::State_Off));
   qstyle_ns->addConstant("State_NoChange",           new QoreBigIntNode(QStyle::State_NoChange));
   qstyle_ns->addConstant("State_On",                 new QoreBigIntNode(QStyle::State_On));
   qstyle_ns->addConstant("State_DownArrow",          new QoreBigIntNode(QStyle::State_DownArrow));
   qstyle_ns->addConstant("State_Horizontal",         new QoreBigIntNode(QStyle::State_Horizontal));
   qstyle_ns->addConstant("State_HasFocus",           new QoreBigIntNode(QStyle::State_HasFocus));
   qstyle_ns->addConstant("State_Top",                new QoreBigIntNode(QStyle::State_Top));
   qstyle_ns->addConstant("State_Bottom",             new QoreBigIntNode(QStyle::State_Bottom));
   qstyle_ns->addConstant("State_FocusAtBorder",      new QoreBigIntNode(QStyle::State_FocusAtBorder));
   qstyle_ns->addConstant("State_AutoRaise",          new QoreBigIntNode(QStyle::State_AutoRaise));
   qstyle_ns->addConstant("State_MouseOver",          new QoreBigIntNode(QStyle::State_MouseOver));
   qstyle_ns->addConstant("State_UpArrow",            new QoreBigIntNode(QStyle::State_UpArrow));
   qstyle_ns->addConstant("State_Selected",           new QoreBigIntNode(QStyle::State_Selected));
   qstyle_ns->addConstant("State_Active",             new QoreBigIntNode(QStyle::State_Active));
   qstyle_ns->addConstant("State_Window",             new QoreBigIntNode(QStyle::State_Window));
   qstyle_ns->addConstant("State_Open",               new QoreBigIntNode(QStyle::State_Open));
   qstyle_ns->addConstant("State_Children",           new QoreBigIntNode(QStyle::State_Children));
   qstyle_ns->addConstant("State_Item",               new QoreBigIntNode(QStyle::State_Item));
   qstyle_ns->addConstant("State_Sibling",            new QoreBigIntNode(QStyle::State_Sibling));
   qstyle_ns->addConstant("State_Editing",            new QoreBigIntNode(QStyle::State_Editing));
   qstyle_ns->addConstant("State_KeyboardFocusChange", new QoreBigIntNode(QStyle::State_KeyboardFocusChange));
#ifdef QT_KEYPAD_NAVIGATION
   qstyle_ns->addConstant("State_HasEditFocus",       new QoreBigIntNode(QStyle::State_HasEditFocus));
#endif
   qstyle_ns->addConstant("State_ReadOnly",           new QoreBigIntNode(QStyle::State_ReadOnly));
   qstyle_ns->addConstant("State_Small",              new QoreBigIntNode(QStyle::State_Small));
   qstyle_ns->addConstant("State_Mini",               new QoreBigIntNode(QStyle::State_Mini));
   
   // ControlElement enum
   qstyle_ns->addConstant("CE_PushButton",            new QoreBigIntNode(QStyle::CE_PushButton));
   qstyle_ns->addConstant("CE_PushButtonBevel",       new QoreBigIntNode(QStyle::CE_PushButtonBevel));
   qstyle_ns->addConstant("CE_PushButtonLabel",       new QoreBigIntNode(QStyle::CE_PushButtonLabel));
   qstyle_ns->addConstant("CE_CheckBox",              new QoreBigIntNode(QStyle::CE_CheckBox));
   qstyle_ns->addConstant("CE_CheckBoxLabel",         new QoreBigIntNode(QStyle::CE_CheckBoxLabel));
   qstyle_ns->addConstant("CE_RadioButton",           new QoreBigIntNode(QStyle::CE_RadioButton));
   qstyle_ns->addConstant("CE_RadioButtonLabel",      new QoreBigIntNode(QStyle::CE_RadioButtonLabel));
   qstyle_ns->addConstant("CE_TabBarTab",             new QoreBigIntNode(QStyle::CE_TabBarTab));
   qstyle_ns->addConstant("CE_TabBarTabShape",        new QoreBigIntNode(QStyle::CE_TabBarTabShape));
   qstyle_ns->addConstant("CE_TabBarTabLabel",        new QoreBigIntNode(QStyle::CE_TabBarTabLabel));
   qstyle_ns->addConstant("CE_ProgressBar",           new QoreBigIntNode(QStyle::CE_ProgressBar));
   qstyle_ns->addConstant("CE_ProgressBarGroove",     new QoreBigIntNode(QStyle::CE_ProgressBarGroove));
   qstyle_ns->addConstant("CE_ProgressBarContents",   new QoreBigIntNode(QStyle::CE_ProgressBarContents));
   qstyle_ns->addConstant("CE_ProgressBarLabel",      new QoreBigIntNode(QStyle::CE_ProgressBarLabel));
   qstyle_ns->addConstant("CE_MenuItem",              new QoreBigIntNode(QStyle::CE_MenuItem));
   qstyle_ns->addConstant("CE_MenuScroller",          new QoreBigIntNode(QStyle::CE_MenuScroller));
   qstyle_ns->addConstant("CE_MenuVMargin",           new QoreBigIntNode(QStyle::CE_MenuVMargin));
   qstyle_ns->addConstant("CE_MenuHMargin",           new QoreBigIntNode(QStyle::CE_MenuHMargin));
   qstyle_ns->addConstant("CE_MenuTearoff",           new QoreBigIntNode(QStyle::CE_MenuTearoff));
   qstyle_ns->addConstant("CE_MenuEmptyArea",         new QoreBigIntNode(QStyle::CE_MenuEmptyArea));
   qstyle_ns->addConstant("CE_MenuBarItem",           new QoreBigIntNode(QStyle::CE_MenuBarItem));
   qstyle_ns->addConstant("CE_MenuBarEmptyArea",      new QoreBigIntNode(QStyle::CE_MenuBarEmptyArea));
   qstyle_ns->addConstant("CE_ToolButtonLabel",       new QoreBigIntNode(QStyle::CE_ToolButtonLabel));
   qstyle_ns->addConstant("CE_Header",                new QoreBigIntNode(QStyle::CE_Header));
   qstyle_ns->addConstant("CE_HeaderSection",         new QoreBigIntNode(QStyle::CE_HeaderSection));
   qstyle_ns->addConstant("CE_HeaderLabel",           new QoreBigIntNode(QStyle::CE_HeaderLabel));
   qstyle_ns->addConstant("CE_Q3DockWindowEmptyArea", new QoreBigIntNode(QStyle::CE_Q3DockWindowEmptyArea));
   qstyle_ns->addConstant("CE_ToolBoxTab",            new QoreBigIntNode(QStyle::CE_ToolBoxTab));
   qstyle_ns->addConstant("CE_SizeGrip",              new QoreBigIntNode(QStyle::CE_SizeGrip));
   qstyle_ns->addConstant("CE_Splitter",              new QoreBigIntNode(QStyle::CE_Splitter));
   qstyle_ns->addConstant("CE_RubberBand",            new QoreBigIntNode(QStyle::CE_RubberBand));
   qstyle_ns->addConstant("CE_DockWidgetTitle",       new QoreBigIntNode(QStyle::CE_DockWidgetTitle));
   qstyle_ns->addConstant("CE_ScrollBarAddLine",      new QoreBigIntNode(QStyle::CE_ScrollBarAddLine));
   qstyle_ns->addConstant("CE_ScrollBarSubLine",      new QoreBigIntNode(QStyle::CE_ScrollBarSubLine));
   qstyle_ns->addConstant("CE_ScrollBarAddPage",      new QoreBigIntNode(QStyle::CE_ScrollBarAddPage));
   qstyle_ns->addConstant("CE_ScrollBarSubPage",      new QoreBigIntNode(QStyle::CE_ScrollBarSubPage));
   qstyle_ns->addConstant("CE_ScrollBarSlider",       new QoreBigIntNode(QStyle::CE_ScrollBarSlider));
   qstyle_ns->addConstant("CE_ScrollBarFirst",        new QoreBigIntNode(QStyle::CE_ScrollBarFirst));
   qstyle_ns->addConstant("CE_ScrollBarLast",         new QoreBigIntNode(QStyle::CE_ScrollBarLast));
   qstyle_ns->addConstant("CE_FocusFrame",            new QoreBigIntNode(QStyle::CE_FocusFrame));
   qstyle_ns->addConstant("CE_ComboBoxLabel",         new QoreBigIntNode(QStyle::CE_ComboBoxLabel));
   qstyle_ns->addConstant("CE_ToolBar",               new QoreBigIntNode(QStyle::CE_ToolBar));
   qstyle_ns->addConstant("CE_ToolBoxTabShape",       new QoreBigIntNode(QStyle::CE_ToolBoxTabShape));
   qstyle_ns->addConstant("CE_ToolBoxTabLabel",       new QoreBigIntNode(QStyle::CE_ToolBoxTabLabel));
   qstyle_ns->addConstant("CE_HeaderEmptyArea",       new QoreBigIntNode(QStyle::CE_HeaderEmptyArea));
   qstyle_ns->addConstant("CE_ColumnViewGrip",        new QoreBigIntNode(QStyle::CE_ColumnViewGrip));
   qstyle_ns->addConstant("CE_CustomBase",            new QoreBigIntNode(QStyle::CE_CustomBase));

   // SubElement enum
   qstyle_ns->addConstant("SE_PushButtonContents",    new QoreBigIntNode(QStyle::SE_PushButtonContents));
   qstyle_ns->addConstant("SE_PushButtonFocusRect",   new QoreBigIntNode(QStyle::SE_PushButtonFocusRect));
   qstyle_ns->addConstant("SE_CheckBoxIndicator",     new QoreBigIntNode(QStyle::SE_CheckBoxIndicator));
   qstyle_ns->addConstant("SE_CheckBoxContents",      new QoreBigIntNode(QStyle::SE_CheckBoxContents));
   qstyle_ns->addConstant("SE_CheckBoxFocusRect",     new QoreBigIntNode(QStyle::SE_CheckBoxFocusRect));
   qstyle_ns->addConstant("SE_CheckBoxClickRect",     new QoreBigIntNode(QStyle::SE_CheckBoxClickRect));
   qstyle_ns->addConstant("SE_RadioButtonIndicator",  new QoreBigIntNode(QStyle::SE_RadioButtonIndicator));
   qstyle_ns->addConstant("SE_RadioButtonContents",   new QoreBigIntNode(QStyle::SE_RadioButtonContents));
   qstyle_ns->addConstant("SE_RadioButtonFocusRect",  new QoreBigIntNode(QStyle::SE_RadioButtonFocusRect));
   qstyle_ns->addConstant("SE_RadioButtonClickRect",  new QoreBigIntNode(QStyle::SE_RadioButtonClickRect));
   qstyle_ns->addConstant("SE_ComboBoxFocusRect",     new QoreBigIntNode(QStyle::SE_ComboBoxFocusRect));
   qstyle_ns->addConstant("SE_SliderFocusRect",       new QoreBigIntNode(QStyle::SE_SliderFocusRect));
   qstyle_ns->addConstant("SE_Q3DockWindowHandleRect", new QoreBigIntNode(QStyle::SE_Q3DockWindowHandleRect));
   qstyle_ns->addConstant("SE_ProgressBarGroove",     new QoreBigIntNode(QStyle::SE_ProgressBarGroove));
   qstyle_ns->addConstant("SE_ProgressBarContents",   new QoreBigIntNode(QStyle::SE_ProgressBarContents));
   qstyle_ns->addConstant("SE_ProgressBarLabel",      new QoreBigIntNode(QStyle::SE_ProgressBarLabel));
   qstyle_ns->addConstant("SE_DialogButtonAccept",    new QoreBigIntNode(QStyle::SE_DialogButtonAccept));
   qstyle_ns->addConstant("SE_DialogButtonReject",    new QoreBigIntNode(QStyle::SE_DialogButtonReject));
   qstyle_ns->addConstant("SE_DialogButtonApply",     new QoreBigIntNode(QStyle::SE_DialogButtonApply));
   qstyle_ns->addConstant("SE_DialogButtonHelp",      new QoreBigIntNode(QStyle::SE_DialogButtonHelp));
   qstyle_ns->addConstant("SE_DialogButtonAll",       new QoreBigIntNode(QStyle::SE_DialogButtonAll));
   qstyle_ns->addConstant("SE_DialogButtonAbort",     new QoreBigIntNode(QStyle::SE_DialogButtonAbort));
   qstyle_ns->addConstant("SE_DialogButtonIgnore",    new QoreBigIntNode(QStyle::SE_DialogButtonIgnore));
   qstyle_ns->addConstant("SE_DialogButtonRetry",     new QoreBigIntNode(QStyle::SE_DialogButtonRetry));
   qstyle_ns->addConstant("SE_DialogButtonCustom",    new QoreBigIntNode(QStyle::SE_DialogButtonCustom));
   qstyle_ns->addConstant("SE_ToolBoxTabContents",    new QoreBigIntNode(QStyle::SE_ToolBoxTabContents));
   qstyle_ns->addConstant("SE_HeaderLabel",           new QoreBigIntNode(QStyle::SE_HeaderLabel));
   qstyle_ns->addConstant("SE_HeaderArrow",           new QoreBigIntNode(QStyle::SE_HeaderArrow));
   qstyle_ns->addConstant("SE_TabWidgetTabBar",       new QoreBigIntNode(QStyle::SE_TabWidgetTabBar));
   qstyle_ns->addConstant("SE_TabWidgetTabPane",      new QoreBigIntNode(QStyle::SE_TabWidgetTabPane));
   qstyle_ns->addConstant("SE_TabWidgetTabContents",  new QoreBigIntNode(QStyle::SE_TabWidgetTabContents));
   qstyle_ns->addConstant("SE_TabWidgetLeftCorner",   new QoreBigIntNode(QStyle::SE_TabWidgetLeftCorner));
   qstyle_ns->addConstant("SE_TabWidgetRightCorner",  new QoreBigIntNode(QStyle::SE_TabWidgetRightCorner));
   qstyle_ns->addConstant("SE_ViewItemCheckIndicator", new QoreBigIntNode(QStyle::SE_ViewItemCheckIndicator));
   qstyle_ns->addConstant("SE_TabBarTearIndicator",   new QoreBigIntNode(QStyle::SE_TabBarTearIndicator));
   qstyle_ns->addConstant("SE_TreeViewDisclosureItem", new QoreBigIntNode(QStyle::SE_TreeViewDisclosureItem));
   qstyle_ns->addConstant("SE_LineEditContents",      new QoreBigIntNode(QStyle::SE_LineEditContents));
   qstyle_ns->addConstant("SE_FrameContents",         new QoreBigIntNode(QStyle::SE_FrameContents));
   qstyle_ns->addConstant("SE_DockWidgetCloseButton", new QoreBigIntNode(QStyle::SE_DockWidgetCloseButton));
   qstyle_ns->addConstant("SE_DockWidgetFloatButton", new QoreBigIntNode(QStyle::SE_DockWidgetFloatButton));
   qstyle_ns->addConstant("SE_DockWidgetTitleBarText", new QoreBigIntNode(QStyle::SE_DockWidgetTitleBarText));
   qstyle_ns->addConstant("SE_DockWidgetIcon",        new QoreBigIntNode(QStyle::SE_DockWidgetIcon));
   qstyle_ns->addConstant("SE_CheckBoxLayoutItem",    new QoreBigIntNode(QStyle::SE_CheckBoxLayoutItem));
   qstyle_ns->addConstant("SE_ComboBoxLayoutItem",    new QoreBigIntNode(QStyle::SE_ComboBoxLayoutItem));
   qstyle_ns->addConstant("SE_DateTimeEditLayoutItem", new QoreBigIntNode(QStyle::SE_DateTimeEditLayoutItem));
   qstyle_ns->addConstant("SE_DialogButtonBoxLayoutItem", new QoreBigIntNode(QStyle::SE_DialogButtonBoxLayoutItem));
   qstyle_ns->addConstant("SE_LabelLayoutItem",       new QoreBigIntNode(QStyle::SE_LabelLayoutItem));
   qstyle_ns->addConstant("SE_ProgressBarLayoutItem", new QoreBigIntNode(QStyle::SE_ProgressBarLayoutItem));
   qstyle_ns->addConstant("SE_PushButtonLayoutItem",  new QoreBigIntNode(QStyle::SE_PushButtonLayoutItem));
   qstyle_ns->addConstant("SE_RadioButtonLayoutItem", new QoreBigIntNode(QStyle::SE_RadioButtonLayoutItem));
   qstyle_ns->addConstant("SE_SliderLayoutItem",      new QoreBigIntNode(QStyle::SE_SliderLayoutItem));
   qstyle_ns->addConstant("SE_SpinBoxLayoutItem",     new QoreBigIntNode(QStyle::SE_SpinBoxLayoutItem));
   qstyle_ns->addConstant("SE_ToolButtonLayoutItem",  new QoreBigIntNode(QStyle::SE_ToolButtonLayoutItem));
   qstyle_ns->addConstant("SE_FrameLayoutItem",       new QoreBigIntNode(QStyle::SE_FrameLayoutItem));
   qstyle_ns->addConstant("SE_GroupBoxLayoutItem",    new QoreBigIntNode(QStyle::SE_GroupBoxLayoutItem));
   qstyle_ns->addConstant("SE_TabWidgetLayoutItem",   new QoreBigIntNode(QStyle::SE_TabWidgetLayoutItem));
   qstyle_ns->addConstant("SE_CustomBase",            new QoreBigIntNode(QStyle::SE_CustomBase));

   // ComplexControl enum
   qstyle_ns->addConstant("CC_SpinBox",               new QoreBigIntNode(QStyle::CC_SpinBox));
   qstyle_ns->addConstant("CC_ComboBox",              new QoreBigIntNode(QStyle::CC_ComboBox));
   qstyle_ns->addConstant("CC_ScrollBar",             new QoreBigIntNode(QStyle::CC_ScrollBar));
   qstyle_ns->addConstant("CC_Slider",                new QoreBigIntNode(QStyle::CC_Slider));
   qstyle_ns->addConstant("CC_ToolButton",            new QoreBigIntNode(QStyle::CC_ToolButton));
   qstyle_ns->addConstant("CC_TitleBar",              new QoreBigIntNode(QStyle::CC_TitleBar));
   qstyle_ns->addConstant("CC_Q3ListView",            new QoreBigIntNode(QStyle::CC_Q3ListView));
   qstyle_ns->addConstant("CC_Dial",                  new QoreBigIntNode(QStyle::CC_Dial));
   qstyle_ns->addConstant("CC_GroupBox",              new QoreBigIntNode(QStyle::CC_GroupBox));
   qstyle_ns->addConstant("CC_MdiControls",           new QoreBigIntNode(QStyle::CC_MdiControls));
   qstyle_ns->addConstant("CC_CustomBase",            new QoreBigIntNode(QStyle::CC_CustomBase));
   
   // SubControl enum
   qstyle_ns->addConstant("SC_None",                  new QoreBigIntNode(QStyle::SC_None));
   qstyle_ns->addConstant("SC_ScrollBarAddLine",      new QoreBigIntNode(QStyle::SC_ScrollBarAddLine));
   qstyle_ns->addConstant("SC_ScrollBarSubLine",      new QoreBigIntNode(QStyle::SC_ScrollBarSubLine));
   qstyle_ns->addConstant("SC_ScrollBarAddPage",      new QoreBigIntNode(QStyle::SC_ScrollBarAddPage));
   qstyle_ns->addConstant("SC_ScrollBarSubPage",      new QoreBigIntNode(QStyle::SC_ScrollBarSubPage));
   qstyle_ns->addConstant("SC_ScrollBarFirst",        new QoreBigIntNode(QStyle::SC_ScrollBarFirst));
   qstyle_ns->addConstant("SC_ScrollBarLast",         new QoreBigIntNode(QStyle::SC_ScrollBarLast));
   qstyle_ns->addConstant("SC_ScrollBarSlider",       new QoreBigIntNode(QStyle::SC_ScrollBarSlider));
   qstyle_ns->addConstant("SC_ScrollBarGroove",       new QoreBigIntNode(QStyle::SC_ScrollBarGroove));
   qstyle_ns->addConstant("SC_SpinBoxUp",             new QoreBigIntNode(QStyle::SC_SpinBoxUp));
   qstyle_ns->addConstant("SC_SpinBoxDown",           new QoreBigIntNode(QStyle::SC_SpinBoxDown));
   qstyle_ns->addConstant("SC_SpinBoxFrame",          new QoreBigIntNode(QStyle::SC_SpinBoxFrame));
   qstyle_ns->addConstant("SC_SpinBoxEditField",      new QoreBigIntNode(QStyle::SC_SpinBoxEditField));
   qstyle_ns->addConstant("SC_ComboBoxFrame",         new QoreBigIntNode(QStyle::SC_ComboBoxFrame));
   qstyle_ns->addConstant("SC_ComboBoxEditField",     new QoreBigIntNode(QStyle::SC_ComboBoxEditField));
   qstyle_ns->addConstant("SC_ComboBoxArrow",         new QoreBigIntNode(QStyle::SC_ComboBoxArrow));
   qstyle_ns->addConstant("SC_ComboBoxListBoxPopup",  new QoreBigIntNode(QStyle::SC_ComboBoxListBoxPopup));
   qstyle_ns->addConstant("SC_SliderGroove",          new QoreBigIntNode(QStyle::SC_SliderGroove));
   qstyle_ns->addConstant("SC_SliderHandle",          new QoreBigIntNode(QStyle::SC_SliderHandle));
   qstyle_ns->addConstant("SC_SliderTickmarks",       new QoreBigIntNode(QStyle::SC_SliderTickmarks));
   qstyle_ns->addConstant("SC_ToolButton",            new QoreBigIntNode(QStyle::SC_ToolButton));
   qstyle_ns->addConstant("SC_ToolButtonMenu",        new QoreBigIntNode(QStyle::SC_ToolButtonMenu));
   qstyle_ns->addConstant("SC_TitleBarSysMenu",       new QoreBigIntNode(QStyle::SC_TitleBarSysMenu));
   qstyle_ns->addConstant("SC_TitleBarMinButton",     new QoreBigIntNode(QStyle::SC_TitleBarMinButton));
   qstyle_ns->addConstant("SC_TitleBarMaxButton",     new QoreBigIntNode(QStyle::SC_TitleBarMaxButton));
   qstyle_ns->addConstant("SC_TitleBarCloseButton",   new QoreBigIntNode(QStyle::SC_TitleBarCloseButton));
   qstyle_ns->addConstant("SC_TitleBarNormalButton",  new QoreBigIntNode(QStyle::SC_TitleBarNormalButton));
   qstyle_ns->addConstant("SC_TitleBarShadeButton",   new QoreBigIntNode(QStyle::SC_TitleBarShadeButton));
   qstyle_ns->addConstant("SC_TitleBarUnshadeButton", new QoreBigIntNode(QStyle::SC_TitleBarUnshadeButton));
   qstyle_ns->addConstant("SC_TitleBarContextHelpButton", new QoreBigIntNode(QStyle::SC_TitleBarContextHelpButton));
   qstyle_ns->addConstant("SC_TitleBarLabel",         new QoreBigIntNode(QStyle::SC_TitleBarLabel));
   qstyle_ns->addConstant("SC_Q3ListView",            new QoreBigIntNode(QStyle::SC_Q3ListView));
   qstyle_ns->addConstant("SC_Q3ListViewBranch",      new QoreBigIntNode(QStyle::SC_Q3ListViewBranch));
   qstyle_ns->addConstant("SC_Q3ListViewExpand",      new QoreBigIntNode(QStyle::SC_Q3ListViewExpand));
   qstyle_ns->addConstant("SC_DialGroove",            new QoreBigIntNode(QStyle::SC_DialGroove));
   qstyle_ns->addConstant("SC_DialHandle",            new QoreBigIntNode(QStyle::SC_DialHandle));
   qstyle_ns->addConstant("SC_DialTickmarks",         new QoreBigIntNode(QStyle::SC_DialTickmarks));
   qstyle_ns->addConstant("SC_GroupBoxCheckBox",      new QoreBigIntNode(QStyle::SC_GroupBoxCheckBox));
   qstyle_ns->addConstant("SC_GroupBoxLabel",         new QoreBigIntNode(QStyle::SC_GroupBoxLabel));
   qstyle_ns->addConstant("SC_GroupBoxContents",      new QoreBigIntNode(QStyle::SC_GroupBoxContents));
   qstyle_ns->addConstant("SC_GroupBoxFrame",         new QoreBigIntNode(QStyle::SC_GroupBoxFrame));
   qstyle_ns->addConstant("SC_MdiMinButton",          new QoreBigIntNode(QStyle::SC_MdiMinButton));
   qstyle_ns->addConstant("SC_MdiNormalButton",       new QoreBigIntNode(QStyle::SC_MdiNormalButton));
   qstyle_ns->addConstant("SC_MdiCloseButton",        new QoreBigIntNode(QStyle::SC_MdiCloseButton));
   qstyle_ns->addConstant("SC_All",                   new QoreBigIntNode(QStyle::SC_All));

   // ContentsType enum
   qstyle_ns->addConstant("CT_PushButton",            new QoreBigIntNode(QStyle::CT_PushButton));
   qstyle_ns->addConstant("CT_CheckBox",              new QoreBigIntNode(QStyle::CT_CheckBox));
   qstyle_ns->addConstant("CT_RadioButton",           new QoreBigIntNode(QStyle::CT_RadioButton));
   qstyle_ns->addConstant("CT_ToolButton",            new QoreBigIntNode(QStyle::CT_ToolButton));
   qstyle_ns->addConstant("CT_ComboBox",              new QoreBigIntNode(QStyle::CT_ComboBox));
   qstyle_ns->addConstant("CT_Splitter",              new QoreBigIntNode(QStyle::CT_Splitter));
   qstyle_ns->addConstant("CT_Q3DockWindow",          new QoreBigIntNode(QStyle::CT_Q3DockWindow));
   qstyle_ns->addConstant("CT_ProgressBar",           new QoreBigIntNode(QStyle::CT_ProgressBar));
   qstyle_ns->addConstant("CT_MenuItem",              new QoreBigIntNode(QStyle::CT_MenuItem));
   qstyle_ns->addConstant("CT_MenuBarItem",           new QoreBigIntNode(QStyle::CT_MenuBarItem));
   qstyle_ns->addConstant("CT_MenuBar",               new QoreBigIntNode(QStyle::CT_MenuBar));
   qstyle_ns->addConstant("CT_Menu",                  new QoreBigIntNode(QStyle::CT_Menu));
   qstyle_ns->addConstant("CT_TabBarTab",             new QoreBigIntNode(QStyle::CT_TabBarTab));
   qstyle_ns->addConstant("CT_Slider",                new QoreBigIntNode(QStyle::CT_Slider));
   qstyle_ns->addConstant("CT_ScrollBar",             new QoreBigIntNode(QStyle::CT_ScrollBar));
   qstyle_ns->addConstant("CT_Q3Header",              new QoreBigIntNode(QStyle::CT_Q3Header));
   qstyle_ns->addConstant("CT_LineEdit",              new QoreBigIntNode(QStyle::CT_LineEdit));
   qstyle_ns->addConstant("CT_SpinBox",               new QoreBigIntNode(QStyle::CT_SpinBox));
   qstyle_ns->addConstant("CT_SizeGrip",              new QoreBigIntNode(QStyle::CT_SizeGrip));
   qstyle_ns->addConstant("CT_TabWidget",             new QoreBigIntNode(QStyle::CT_TabWidget));
   qstyle_ns->addConstant("CT_DialogButtons",         new QoreBigIntNode(QStyle::CT_DialogButtons));
   qstyle_ns->addConstant("CT_HeaderSection",         new QoreBigIntNode(QStyle::CT_HeaderSection));
   qstyle_ns->addConstant("CT_GroupBox",              new QoreBigIntNode(QStyle::CT_GroupBox));
   qstyle_ns->addConstant("CT_MdiControls",           new QoreBigIntNode(QStyle::CT_MdiControls));
   qstyle_ns->addConstant("CT_CustomBase",            new QoreBigIntNode(QStyle::CT_CustomBase));

   // StyleHint enum
   qstyle_ns->addConstant("SH_EtchDisabledText",      new QoreBigIntNode(QStyle::SH_EtchDisabledText));
   qstyle_ns->addConstant("SH_DitherDisabledText",    new QoreBigIntNode(QStyle::SH_DitherDisabledText));
   qstyle_ns->addConstant("SH_ScrollBar_MiddleClickAbsolutePosition", new QoreBigIntNode(QStyle::SH_ScrollBar_MiddleClickAbsolutePosition));
   qstyle_ns->addConstant("SH_ScrollBar_ScrollWhenPointerLeavesControl", new QoreBigIntNode(QStyle::SH_ScrollBar_ScrollWhenPointerLeavesControl));
   qstyle_ns->addConstant("SH_TabBar_SelectMouseType", new QoreBigIntNode(QStyle::SH_TabBar_SelectMouseType));
   qstyle_ns->addConstant("SH_TabBar_Alignment",      new QoreBigIntNode(QStyle::SH_TabBar_Alignment));
   qstyle_ns->addConstant("SH_Header_ArrowAlignment", new QoreBigIntNode(QStyle::SH_Header_ArrowAlignment));
   qstyle_ns->addConstant("SH_Slider_SnapToValue",    new QoreBigIntNode(QStyle::SH_Slider_SnapToValue));
   qstyle_ns->addConstant("SH_Slider_SloppyKeyEvents", new QoreBigIntNode(QStyle::SH_Slider_SloppyKeyEvents));
   qstyle_ns->addConstant("SH_ProgressDialog_CenterCancelButton", new QoreBigIntNode(QStyle::SH_ProgressDialog_CenterCancelButton));
   qstyle_ns->addConstant("SH_ProgressDialog_TextLabelAlignment", new QoreBigIntNode(QStyle::SH_ProgressDialog_TextLabelAlignment));
   qstyle_ns->addConstant("SH_PrintDialog_RightAlignButtons", new QoreBigIntNode(QStyle::SH_PrintDialog_RightAlignButtons));
   qstyle_ns->addConstant("SH_MainWindow_SpaceBelowMenuBar", new QoreBigIntNode(QStyle::SH_MainWindow_SpaceBelowMenuBar));
   qstyle_ns->addConstant("SH_FontDialog_SelectAssociatedText", new QoreBigIntNode(QStyle::SH_FontDialog_SelectAssociatedText));
   qstyle_ns->addConstant("SH_Menu_AllowActiveAndDisabled", new QoreBigIntNode(QStyle::SH_Menu_AllowActiveAndDisabled));
   qstyle_ns->addConstant("SH_Menu_SpaceActivatesItem", new QoreBigIntNode(QStyle::SH_Menu_SpaceActivatesItem));
   qstyle_ns->addConstant("SH_Menu_SubMenuPopupDelay", new QoreBigIntNode(QStyle::SH_Menu_SubMenuPopupDelay));
   qstyle_ns->addConstant("SH_ScrollView_FrameOnlyAroundContents", new QoreBigIntNode(QStyle::SH_ScrollView_FrameOnlyAroundContents));
   qstyle_ns->addConstant("SH_MenuBar_AltKeyNavigation", new QoreBigIntNode(QStyle::SH_MenuBar_AltKeyNavigation));
   qstyle_ns->addConstant("SH_ComboBox_ListMouseTracking", new QoreBigIntNode(QStyle::SH_ComboBox_ListMouseTracking));
   qstyle_ns->addConstant("SH_Menu_MouseTracking",    new QoreBigIntNode(QStyle::SH_Menu_MouseTracking));
   qstyle_ns->addConstant("SH_MenuBar_MouseTracking", new QoreBigIntNode(QStyle::SH_MenuBar_MouseTracking));
   qstyle_ns->addConstant("SH_ItemView_ChangeHighlightOnFocus", new QoreBigIntNode(QStyle::SH_ItemView_ChangeHighlightOnFocus));
   qstyle_ns->addConstant("SH_Widget_ShareActivation", new QoreBigIntNode(QStyle::SH_Widget_ShareActivation));
   qstyle_ns->addConstant("SH_Workspace_FillSpaceOnMaximize", new QoreBigIntNode(QStyle::SH_Workspace_FillSpaceOnMaximize));
   qstyle_ns->addConstant("SH_ComboBox_Popup",        new QoreBigIntNode(QStyle::SH_ComboBox_Popup));
   qstyle_ns->addConstant("SH_TitleBar_NoBorder",     new QoreBigIntNode(QStyle::SH_TitleBar_NoBorder));
   qstyle_ns->addConstant("SH_Slider_StopMouseOverSlider", new QoreBigIntNode(QStyle::SH_Slider_StopMouseOverSlider));
   qstyle_ns->addConstant("SH_ScrollBar_StopMouseOverSlider", new QoreBigIntNode(QStyle::SH_ScrollBar_StopMouseOverSlider));
   qstyle_ns->addConstant("SH_BlinkCursorWhenTextSelected", new QoreBigIntNode(QStyle::SH_BlinkCursorWhenTextSelected));
   qstyle_ns->addConstant("SH_RichText_FullWidthSelection", new QoreBigIntNode(QStyle::SH_RichText_FullWidthSelection));
   qstyle_ns->addConstant("SH_Menu_Scrollable",       new QoreBigIntNode(QStyle::SH_Menu_Scrollable));
   qstyle_ns->addConstant("SH_GroupBox_TextLabelVerticalAlignment", new QoreBigIntNode(QStyle::SH_GroupBox_TextLabelVerticalAlignment));
   qstyle_ns->addConstant("SH_GroupBox_TextLabelColor", new QoreBigIntNode(QStyle::SH_GroupBox_TextLabelColor));
   qstyle_ns->addConstant("SH_Menu_SloppySubMenus",   new QoreBigIntNode(QStyle::SH_Menu_SloppySubMenus));
   qstyle_ns->addConstant("SH_Table_GridLineColor",   new QoreBigIntNode(QStyle::SH_Table_GridLineColor));
   qstyle_ns->addConstant("SH_LineEdit_PasswordCharacter", new QoreBigIntNode(QStyle::SH_LineEdit_PasswordCharacter));
   qstyle_ns->addConstant("SH_DialogButtons_DefaultButton", new QoreBigIntNode(QStyle::SH_DialogButtons_DefaultButton));
   qstyle_ns->addConstant("SH_ToolBox_SelectedPageTitleBold", new QoreBigIntNode(QStyle::SH_ToolBox_SelectedPageTitleBold));
   qstyle_ns->addConstant("SH_TabBar_PreferNoArrows", new QoreBigIntNode(QStyle::SH_TabBar_PreferNoArrows));
   qstyle_ns->addConstant("SH_ScrollBar_LeftClickAbsolutePosition", new QoreBigIntNode(QStyle::SH_ScrollBar_LeftClickAbsolutePosition));
   qstyle_ns->addConstant("SH_Q3ListViewExpand_SelectMouseType", new QoreBigIntNode(QStyle::SH_Q3ListViewExpand_SelectMouseType));
   qstyle_ns->addConstant("SH_UnderlineShortcut",     new QoreBigIntNode(QStyle::SH_UnderlineShortcut));
   qstyle_ns->addConstant("SH_SpinBox_AnimateButton", new QoreBigIntNode(QStyle::SH_SpinBox_AnimateButton));
   qstyle_ns->addConstant("SH_SpinBox_KeyPressAutoRepeatRate", new QoreBigIntNode(QStyle::SH_SpinBox_KeyPressAutoRepeatRate));
   qstyle_ns->addConstant("SH_SpinBox_ClickAutoRepeatRate", new QoreBigIntNode(QStyle::SH_SpinBox_ClickAutoRepeatRate));
   qstyle_ns->addConstant("SH_Menu_FillScreenWithScroll", new QoreBigIntNode(QStyle::SH_Menu_FillScreenWithScroll));
   qstyle_ns->addConstant("SH_ToolTipLabel_Opacity",  new QoreBigIntNode(QStyle::SH_ToolTipLabel_Opacity));
   qstyle_ns->addConstant("SH_DrawMenuBarSeparator",  new QoreBigIntNode(QStyle::SH_DrawMenuBarSeparator));
   qstyle_ns->addConstant("SH_TitleBar_ModifyNotification", new QoreBigIntNode(QStyle::SH_TitleBar_ModifyNotification));
   qstyle_ns->addConstant("SH_Button_FocusPolicy",    new QoreBigIntNode(QStyle::SH_Button_FocusPolicy));
   qstyle_ns->addConstant("SH_MenuBar_DismissOnSecondClick", new QoreBigIntNode(QStyle::SH_MenuBar_DismissOnSecondClick));
   qstyle_ns->addConstant("SH_MessageBox_UseBorderForButtonSpacing", new QoreBigIntNode(QStyle::SH_MessageBox_UseBorderForButtonSpacing));
   qstyle_ns->addConstant("SH_TitleBar_AutoRaise",    new QoreBigIntNode(QStyle::SH_TitleBar_AutoRaise));
   qstyle_ns->addConstant("SH_ToolButton_PopupDelay", new QoreBigIntNode(QStyle::SH_ToolButton_PopupDelay));
   qstyle_ns->addConstant("SH_FocusFrame_Mask",       new QoreBigIntNode(QStyle::SH_FocusFrame_Mask));
   qstyle_ns->addConstant("SH_RubberBand_Mask",       new QoreBigIntNode(QStyle::SH_RubberBand_Mask));
   qstyle_ns->addConstant("SH_WindowFrame_Mask",      new QoreBigIntNode(QStyle::SH_WindowFrame_Mask));
   qstyle_ns->addConstant("SH_SpinControls_DisableOnBounds", new QoreBigIntNode(QStyle::SH_SpinControls_DisableOnBounds));
   qstyle_ns->addConstant("SH_Dial_BackgroundRole",   new QoreBigIntNode(QStyle::SH_Dial_BackgroundRole));
   qstyle_ns->addConstant("SH_ComboBox_LayoutDirection", new QoreBigIntNode(QStyle::SH_ComboBox_LayoutDirection));
   qstyle_ns->addConstant("SH_ItemView_EllipsisLocation", new QoreBigIntNode(QStyle::SH_ItemView_EllipsisLocation));
   qstyle_ns->addConstant("SH_ItemView_ShowDecorationSelected", new QoreBigIntNode(QStyle::SH_ItemView_ShowDecorationSelected));
   qstyle_ns->addConstant("SH_ItemView_ActivateItemOnSingleClick", new QoreBigIntNode(QStyle::SH_ItemView_ActivateItemOnSingleClick));
   qstyle_ns->addConstant("SH_ScrollBar_ContextMenu", new QoreBigIntNode(QStyle::SH_ScrollBar_ContextMenu));
   qstyle_ns->addConstant("SH_ScrollBar_RollBetweenButtons", new QoreBigIntNode(QStyle::SH_ScrollBar_RollBetweenButtons));
   qstyle_ns->addConstant("SH_Slider_AbsoluteSetButtons", new QoreBigIntNode(QStyle::SH_Slider_AbsoluteSetButtons));
   qstyle_ns->addConstant("SH_Slider_PageSetButtons", new QoreBigIntNode(QStyle::SH_Slider_PageSetButtons));
   qstyle_ns->addConstant("SH_Menu_KeyboardSearch",   new QoreBigIntNode(QStyle::SH_Menu_KeyboardSearch));
   qstyle_ns->addConstant("SH_TabBar_ElideMode",      new QoreBigIntNode(QStyle::SH_TabBar_ElideMode));
   qstyle_ns->addConstant("SH_DialogButtonLayout",    new QoreBigIntNode(QStyle::SH_DialogButtonLayout));
   qstyle_ns->addConstant("SH_ComboBox_PopupFrameStyle", new QoreBigIntNode(QStyle::SH_ComboBox_PopupFrameStyle));
   qstyle_ns->addConstant("SH_MessageBox_TextInteractionFlags", new QoreBigIntNode(QStyle::SH_MessageBox_TextInteractionFlags));
   qstyle_ns->addConstant("SH_DialogButtonBox_ButtonsHaveIcons", new QoreBigIntNode(QStyle::SH_DialogButtonBox_ButtonsHaveIcons));
   qstyle_ns->addConstant("SH_SpellCheckUnderlineStyle", new QoreBigIntNode(QStyle::SH_SpellCheckUnderlineStyle));
   qstyle_ns->addConstant("SH_MessageBox_CenterButtons", new QoreBigIntNode(QStyle::SH_MessageBox_CenterButtons));
   qstyle_ns->addConstant("SH_Menu_SelectionWrap",    new QoreBigIntNode(QStyle::SH_Menu_SelectionWrap));
   qstyle_ns->addConstant("SH_ItemView_MovementWithoutUpdatingSelection", new QoreBigIntNode(QStyle::SH_ItemView_MovementWithoutUpdatingSelection));
   qstyle_ns->addConstant("SH_ToolTip_Mask",          new QoreBigIntNode(QStyle::SH_ToolTip_Mask));
   qstyle_ns->addConstant("SH_FocusFrame_AboveWidget", new QoreBigIntNode(QStyle::SH_FocusFrame_AboveWidget));
   qstyle_ns->addConstant("SH_TextControl_FocusIndicatorTextCharFormat", new QoreBigIntNode(QStyle::SH_TextControl_FocusIndicatorTextCharFormat));
   qstyle_ns->addConstant("SH_WizardStyle",           new QoreBigIntNode(QStyle::SH_WizardStyle));
   qstyle_ns->addConstant("SH_ItemView_ArrowKeysNavigateIntoChildren", new QoreBigIntNode(QStyle::SH_ItemView_ArrowKeysNavigateIntoChildren));
   qstyle_ns->addConstant("SH_CustomBase",            new QoreBigIntNode(QStyle::SH_CustomBase));

   // StandardPixmap enum
   qstyle_ns->addConstant("SP_TitleBarMenuButton",    new QoreBigIntNode(QStyle::SP_TitleBarMenuButton));
   qstyle_ns->addConstant("SP_TitleBarMinButton",     new QoreBigIntNode(QStyle::SP_TitleBarMinButton));
   qstyle_ns->addConstant("SP_TitleBarMaxButton",     new QoreBigIntNode(QStyle::SP_TitleBarMaxButton));
   qstyle_ns->addConstant("SP_TitleBarCloseButton",   new QoreBigIntNode(QStyle::SP_TitleBarCloseButton));
   qstyle_ns->addConstant("SP_TitleBarNormalButton",  new QoreBigIntNode(QStyle::SP_TitleBarNormalButton));
   qstyle_ns->addConstant("SP_TitleBarShadeButton",   new QoreBigIntNode(QStyle::SP_TitleBarShadeButton));
   qstyle_ns->addConstant("SP_TitleBarUnshadeButton", new QoreBigIntNode(QStyle::SP_TitleBarUnshadeButton));
   qstyle_ns->addConstant("SP_TitleBarContextHelpButton", new QoreBigIntNode(QStyle::SP_TitleBarContextHelpButton));
   qstyle_ns->addConstant("SP_DockWidgetCloseButton", new QoreBigIntNode(QStyle::SP_DockWidgetCloseButton));
   qstyle_ns->addConstant("SP_MessageBoxInformation", new QoreBigIntNode(QStyle::SP_MessageBoxInformation));
   qstyle_ns->addConstant("SP_MessageBoxWarning",     new QoreBigIntNode(QStyle::SP_MessageBoxWarning));
   qstyle_ns->addConstant("SP_MessageBoxCritical",    new QoreBigIntNode(QStyle::SP_MessageBoxCritical));
   qstyle_ns->addConstant("SP_MessageBoxQuestion",    new QoreBigIntNode(QStyle::SP_MessageBoxQuestion));
   qstyle_ns->addConstant("SP_DesktopIcon",           new QoreBigIntNode(QStyle::SP_DesktopIcon));
   qstyle_ns->addConstant("SP_TrashIcon",             new QoreBigIntNode(QStyle::SP_TrashIcon));
   qstyle_ns->addConstant("SP_ComputerIcon",          new QoreBigIntNode(QStyle::SP_ComputerIcon));
   qstyle_ns->addConstant("SP_DriveFDIcon",           new QoreBigIntNode(QStyle::SP_DriveFDIcon));
   qstyle_ns->addConstant("SP_DriveHDIcon",           new QoreBigIntNode(QStyle::SP_DriveHDIcon));
   qstyle_ns->addConstant("SP_DriveCDIcon",           new QoreBigIntNode(QStyle::SP_DriveCDIcon));
   qstyle_ns->addConstant("SP_DriveDVDIcon",          new QoreBigIntNode(QStyle::SP_DriveDVDIcon));
   qstyle_ns->addConstant("SP_DriveNetIcon",          new QoreBigIntNode(QStyle::SP_DriveNetIcon));
   qstyle_ns->addConstant("SP_DirOpenIcon",           new QoreBigIntNode(QStyle::SP_DirOpenIcon));
   qstyle_ns->addConstant("SP_DirClosedIcon",         new QoreBigIntNode(QStyle::SP_DirClosedIcon));
   qstyle_ns->addConstant("SP_DirLinkIcon",           new QoreBigIntNode(QStyle::SP_DirLinkIcon));
   qstyle_ns->addConstant("SP_FileIcon",              new QoreBigIntNode(QStyle::SP_FileIcon));
   qstyle_ns->addConstant("SP_FileLinkIcon",          new QoreBigIntNode(QStyle::SP_FileLinkIcon));
   qstyle_ns->addConstant("SP_ToolBarHorizontalExtensionButton", new QoreBigIntNode(QStyle::SP_ToolBarHorizontalExtensionButton));
   qstyle_ns->addConstant("SP_ToolBarVerticalExtensionButton", new QoreBigIntNode(QStyle::SP_ToolBarVerticalExtensionButton));
   qstyle_ns->addConstant("SP_FileDialogStart",       new QoreBigIntNode(QStyle::SP_FileDialogStart));
   qstyle_ns->addConstant("SP_FileDialogEnd",         new QoreBigIntNode(QStyle::SP_FileDialogEnd));
   qstyle_ns->addConstant("SP_FileDialogToParent",    new QoreBigIntNode(QStyle::SP_FileDialogToParent));
   qstyle_ns->addConstant("SP_FileDialogNewFolder",   new QoreBigIntNode(QStyle::SP_FileDialogNewFolder));
   qstyle_ns->addConstant("SP_FileDialogDetailedView", new QoreBigIntNode(QStyle::SP_FileDialogDetailedView));
   qstyle_ns->addConstant("SP_FileDialogInfoView",    new QoreBigIntNode(QStyle::SP_FileDialogInfoView));
   qstyle_ns->addConstant("SP_FileDialogContentsView", new QoreBigIntNode(QStyle::SP_FileDialogContentsView));
   qstyle_ns->addConstant("SP_FileDialogListView",    new QoreBigIntNode(QStyle::SP_FileDialogListView));
   qstyle_ns->addConstant("SP_FileDialogBack",        new QoreBigIntNode(QStyle::SP_FileDialogBack));
   qstyle_ns->addConstant("SP_DirIcon",               new QoreBigIntNode(QStyle::SP_DirIcon));
   qstyle_ns->addConstant("SP_DialogOkButton",        new QoreBigIntNode(QStyle::SP_DialogOkButton));
   qstyle_ns->addConstant("SP_DialogCancelButton",    new QoreBigIntNode(QStyle::SP_DialogCancelButton));
   qstyle_ns->addConstant("SP_DialogHelpButton",      new QoreBigIntNode(QStyle::SP_DialogHelpButton));
   qstyle_ns->addConstant("SP_DialogOpenButton",      new QoreBigIntNode(QStyle::SP_DialogOpenButton));
   qstyle_ns->addConstant("SP_DialogSaveButton",      new QoreBigIntNode(QStyle::SP_DialogSaveButton));
   qstyle_ns->addConstant("SP_DialogCloseButton",     new QoreBigIntNode(QStyle::SP_DialogCloseButton));
   qstyle_ns->addConstant("SP_DialogApplyButton",     new QoreBigIntNode(QStyle::SP_DialogApplyButton));
   qstyle_ns->addConstant("SP_DialogResetButton",     new QoreBigIntNode(QStyle::SP_DialogResetButton));
   qstyle_ns->addConstant("SP_DialogDiscardButton",   new QoreBigIntNode(QStyle::SP_DialogDiscardButton));
   qstyle_ns->addConstant("SP_DialogYesButton",       new QoreBigIntNode(QStyle::SP_DialogYesButton));
   qstyle_ns->addConstant("SP_DialogNoButton",        new QoreBigIntNode(QStyle::SP_DialogNoButton));
   qstyle_ns->addConstant("SP_ArrowUp",               new QoreBigIntNode(QStyle::SP_ArrowUp));
   qstyle_ns->addConstant("SP_ArrowDown",             new QoreBigIntNode(QStyle::SP_ArrowDown));
   qstyle_ns->addConstant("SP_ArrowLeft",             new QoreBigIntNode(QStyle::SP_ArrowLeft));
   qstyle_ns->addConstant("SP_ArrowRight",            new QoreBigIntNode(QStyle::SP_ArrowRight));
   qstyle_ns->addConstant("SP_ArrowBack",             new QoreBigIntNode(QStyle::SP_ArrowBack));
   qstyle_ns->addConstant("SP_ArrowForward",          new QoreBigIntNode(QStyle::SP_ArrowForward));
   qstyle_ns->addConstant("SP_DirHomeIcon",           new QoreBigIntNode(QStyle::SP_DirHomeIcon));
   qstyle_ns->addConstant("SP_CommandLink",           new QoreBigIntNode(QStyle::SP_CommandLink));
   qstyle_ns->addConstant("SP_VistaShield",           new QoreBigIntNode(QStyle::SP_VistaShield));
   qstyle_ns->addConstant("SP_CustomBase",            new QoreBigIntNode(QStyle::SP_CustomBase));


   return qstyle_ns;
}
