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

int CID_QSTYLE;
class QoreClass *QC_QStyle = 0;

//QStyle ()
static void QSTYLE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLE, new QoreQStyle(self));
   return;
}

static void QSTYLE_copy(class Object *self, class Object *old, class QoreQStyle *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//int combinedLayoutSpacing ( QSizePolicy::ControlTypes controls1, QSizePolicy::ControlTypes controls2, Qt::Orientation orientation, QStyleOption * option = 0, QWidget * widget = 0 ) const
static QoreNode *QSTYLE_combinedLayoutSpacing(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlTypes controls1 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlTypes controls2 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreNode((int64)qs->getQStyle()->combinedLayoutSpacing(controls1, controls2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_drawComplexControl(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_drawControl(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ControlElement element = (QStyle::ControlElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawControl(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static QoreNode *QSTYLE_drawItemPixmap(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static QoreNode *QSTYLE_drawItemText(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
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
   qs->drawItemText(painter->getQPainter(), *(static_cast<QRect *>(rectangle)), alignment, *(static_cast<QPalette *>(palette)), enabled, text, textRole);
   return 0;
}

//virtual void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_drawPrimitive(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::PrimitiveElement element = (QStyle::PrimitiveElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawPrimitive(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static QoreNode *QSTYLE_generatedIconPixmap(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QStyleOption object as third argument to QStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qs->generatedIconPixmap(iconMode, *(static_cast<QPixmap *>(pixmap)), static_cast<QStyleOption *>(option)));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_hitTestComplexControl(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static QoreNode *QSTYLE_itemPixmapRect(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMPIXMAPRECT-PARAM-ERROR", "expecting a QRect object as first argument to QStyle::itemPixmapRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 1);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMPIXMAPRECT-PARAM-ERROR", "expecting a QPixmap object as third argument to QStyle::itemPixmapRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->itemPixmapRect(*(static_cast<QRect *>(rectangle)), alignment, *(static_cast<QPixmap *>(pixmap))));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const
static QoreNode *QSTYLE_itemTextRect(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFontMetrics *metrics = (p && p->type == NT_OBJECT) ? (QoreQFontMetrics *)p->val.object->getReferencedPrivateData(CID_QFONTMETRICS, xsink) : 0;
   if (!metrics) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMTEXTRECT-PARAM-ERROR", "expecting a QFontMetrics object as first argument to QStyle::itemTextRect()");
      return 0;
   }
   ReferenceHolder<QoreQFontMetrics> metricsHolder(metrics, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMTEXTRECT-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::itemTextRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   bool enabled = p ? p->getAsBool() : false;
   p = get_param(params, 4);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->itemTextRect(*(static_cast<QFontMetrics *>(metrics)), *(static_cast<QRect *>(rectangle)), alignment, enabled, text));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//int layoutSpacing ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_layoutSpacing(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->getQStyle()->layoutSpacing(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_pixelMetric(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::PixelMetric metric = (QStyle::PixelMetric)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual void polish ( QWidget * widget )
//virtual void polish ( QApplication * application )
//virtual void polish ( QPalette & palette )
static QoreNode *QSTYLE_polish(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   if (!p) {
      xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() was expecting a QApplication, QPalette, or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)p->val.object->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQPalette *palette = (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink);
      if (!palette) {
	 QoreQWidget *widget = (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
	 if (!widget) {
	    if (!xsink->isException())
	       xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
	    return 0;
	 }
	 ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
	 qs->polish(static_cast<QWidget *>(widget->getQWidget()));
	 return 0;
      }
      ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);
      qs->polish(*(static_cast<QPalette *>(palette)));
      return 0;
   }
   ReferenceHolder<QoreQApplication> applicationHolder(application, xsink);
   qs->polish(static_cast<QApplication *>(application->qobj));
   return 0;
}

//virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_sizeFromContents(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ContentsType type = (QStyle::ContentsType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQSize *contentsSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qs->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QIcon standardIcon ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_standardIcon(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qs->getQStyle()->standardIcon(standardIcon, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//virtual QPalette standardPalette () const
static QoreNode *QSTYLE_standardPalette(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qs->standardPalette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static QoreNode *QSTYLE_styleHint(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StyleHint hint = (QStyle::StyleHint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
//   p = get_param(params, 3);
//   ??? QStyleHintReturn* returnData = p;
   return new QoreNode((int64)qs->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_subControlRect(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QStyle::SubControl subControl = (QStyle::SubControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_subElementRect(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::SubElement element = (QStyle::SubElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->subElementRect(element, static_cast<QStyleOption *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual void unpolish ( QWidget * widget )
//virtual void unpolish ( QApplication * application )
static QoreNode *QSTYLE_unpolish(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   if (!p) {
      xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() was expecting a QApplication or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)p->val.object->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQWidget *widget = (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
	 if (!xsink->isException())
	    xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
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
static QoreNode *QSTYLE_layoutSpacingImplementation(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreNode((int64)qs->layoutSpacingImplementation(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//QIcon standardIconImplementation ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_standardIconImplementation(Object *self, QoreAbstractQStyle *qs, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qs->standardIconImplementation(standardIcon, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

QoreClass *initQStyleClass(QoreClass *qobject)
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
