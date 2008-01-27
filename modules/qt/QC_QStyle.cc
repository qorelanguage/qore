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
static void QSTYLE_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLE, new QoreQStyle(self));
   return;
}

static void QSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQStyle *qs, ExceptionSink *xsink)
{
   xsink->raiseException("QSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//int combinedLayoutSpacing ( QSizePolicy::ControlTypes controls1, QSizePolicy::ControlTypes controls2, Qt::Orientation orientation, QStyleOption * option = 0, QWidget * widget = 0 ) const
static QoreNode *QSTYLE_combinedLayoutSpacing(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlTypes controls1 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlTypes controls2 = (QSizePolicy::ControlTypes)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreNode((int64)qs->getQStyle()->combinedLayoutSpacing(controls1, controls2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_drawComplexControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_drawControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ControlElement element = (QStyle::ControlElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawControl(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static QoreNode *QSTYLE_drawItemPixmap(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMPIXMAP-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemPixmap()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static QoreNode *QSTYLE_drawItemText(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QPainter object as first argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQPainter> painterHolder(painter, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWITEMTEXT-PARAM-ERROR", "expecting a QRect object as second argument to QStyle::drawItemText()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 2);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
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
static QoreNode *QSTYLE_drawPrimitive(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::PrimitiveElement element = (QStyle::PrimitiveElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qs->drawPrimitive(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static QoreNode *QSTYLE_generatedIconPixmap(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
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
static QoreNode *QSTYLE_hitTestComplexControl(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const
static QoreNode *QSTYLE_itemPixmapRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rectangle) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMPIXMAPRECT-PARAM-ERROR", "expecting a QRect object as first argument to QStyle::itemPixmapRect()");
      return 0;
   }
   ReferenceHolder<QoreQRect> rectangleHolder(rectangle, xsink);
   p = get_param(params, 1);
   int alignment = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
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
static QoreNode *QSTYLE_itemTextRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFontMetrics *metrics = (p && p->type == NT_OBJECT) ? (QoreQFontMetrics *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QFONTMETRICS, xsink) : 0;
   if (!metrics) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-ITEMTEXTRECT-PARAM-ERROR", "expecting a QFontMetrics object as first argument to QStyle::itemTextRect()");
      return 0;
   }
   ReferenceHolder<QoreQFontMetrics> metricsHolder(metrics, xsink);
   p = get_param(params, 1);
   QoreQRect *rectangle = (p && p->type == NT_OBJECT) ? (QoreQRect *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink) : 0;
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
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->itemTextRect(*(static_cast<QFontMetrics *>(metrics)), *(static_cast<QRect *>(rectangle)), alignment, enabled, text));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//int layoutSpacing ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_layoutSpacing(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->getQStyle()->layoutSpacing(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_pixelMetric(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::PixelMetric metric = (QStyle::PixelMetric)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qs->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual void polish ( QWidget * widget )
//virtual void polish ( QApplication * application )
//virtual void polish ( QPalette & palette )
static QoreNode *QSTYLE_polish(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   if (!p) {
      xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() was expecting a QApplication, QPalette, or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQPalette *palette = (QoreQPalette *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPALETTE, xsink);
      if (!palette) {
	 QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
	 if (!widget) {
	    if (!xsink->isException())
	       xsink->raiseException("QSTYLE-POLISH-PARAM-ERROR", "QStyle::polish() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
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
static QoreNode *QSTYLE_sizeFromContents(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ContentsType type = (QStyle::ContentsType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQSize *contentsSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qs->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//QIcon standardIcon ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_standardIcon(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qs->getQStyle()->standardIcon(standardIcon, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//virtual QPalette standardPalette () const
static QoreNode *QSTYLE_standardPalette(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(qs->standardPalette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static QoreNode *QSTYLE_styleHint(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StyleHint hint = (QStyle::StyleHint)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
//   p = get_param(params, 3);
//   ??? QStyleHintReturn* returnData = p;
   return new QoreNode((int64)qs->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_subControlRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::ComplexControl control = (QStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QStyle::SubControl subControl = (QStyle::SubControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qs->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static QoreNode *QSTYLE_subElementRect(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::SubElement element = (QStyle::SubElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
static QoreNode *QSTYLE_unpolish(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   if (!p) {
      xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() was expecting a QApplication or a QWidget object as the sole argument");
      return 0;
   }

   QoreQApplication *application = (QoreQApplication *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QAPPLICATION, xsink);
   if (!application) {
      QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (!widget) {
	 if (!xsink->isException())
	    xsink->raiseException("QSTYLE-UNPOLISH-PARAM-ERROR", "QStyle::unpolish() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
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
static QoreNode *QSTYLE_layoutSpacingImplementation(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QSizePolicy::ControlType control1 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QSizePolicy::ControlType control2 = (QSizePolicy::ControlType)(p ? p->getAsInt() : 0);
   p = get_param(params, 2);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 4);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   return new QoreNode((int64)qs->layoutSpacingImplementation(control1, control2, orientation, option ? static_cast<QStyleOption *>(option) : 0, widget ? widget->getQWidget() : 0));
}

//QIcon standardIconImplementation ( StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
static QoreNode *QSTYLE_standardIconImplementation(QoreObject *self, QoreAbstractQStyle *qs, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::StandardPixmap standardIcon = (QStyle::StandardPixmap)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<QoreQStyleOption> optionHolder(option, xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
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
   qstyle_ns->addConstant("PE_Q3CheckListController", new QoreNode((int64)QStyle::PE_Q3CheckListController));
   qstyle_ns->addConstant("PE_Q3CheckListExclusiveIndicator", new QoreNode((int64)QStyle::PE_Q3CheckListExclusiveIndicator));
   qstyle_ns->addConstant("PE_Q3CheckListIndicator",  new QoreNode((int64)QStyle::PE_Q3CheckListIndicator));
   qstyle_ns->addConstant("PE_Q3DockWindowSeparator", new QoreNode((int64)QStyle::PE_Q3DockWindowSeparator));
   qstyle_ns->addConstant("PE_Q3Separator",           new QoreNode((int64)QStyle::PE_Q3Separator));
   qstyle_ns->addConstant("PE_Frame",                 new QoreNode((int64)QStyle::PE_Frame));
   qstyle_ns->addConstant("PE_FrameDefaultButton",    new QoreNode((int64)QStyle::PE_FrameDefaultButton));
   qstyle_ns->addConstant("PE_FrameDockWidget",       new QoreNode((int64)QStyle::PE_FrameDockWidget));
   qstyle_ns->addConstant("PE_FrameFocusRect",        new QoreNode((int64)QStyle::PE_FrameFocusRect));
   qstyle_ns->addConstant("PE_FrameGroupBox",         new QoreNode((int64)QStyle::PE_FrameGroupBox));
   qstyle_ns->addConstant("PE_FrameLineEdit",         new QoreNode((int64)QStyle::PE_FrameLineEdit));
   qstyle_ns->addConstant("PE_FrameMenu",             new QoreNode((int64)QStyle::PE_FrameMenu));
   qstyle_ns->addConstant("PE_FrameStatusBar",        new QoreNode((int64)QStyle::PE_FrameStatusBar));
   qstyle_ns->addConstant("PE_FrameTabWidget",        new QoreNode((int64)QStyle::PE_FrameTabWidget));
   qstyle_ns->addConstant("PE_FrameWindow",           new QoreNode((int64)QStyle::PE_FrameWindow));
   qstyle_ns->addConstant("PE_FrameButtonBevel",      new QoreNode((int64)QStyle::PE_FrameButtonBevel));
   qstyle_ns->addConstant("PE_FrameButtonTool",       new QoreNode((int64)QStyle::PE_FrameButtonTool));
   qstyle_ns->addConstant("PE_FrameTabBarBase",       new QoreNode((int64)QStyle::PE_FrameTabBarBase));
   qstyle_ns->addConstant("PE_PanelButtonCommand",    new QoreNode((int64)QStyle::PE_PanelButtonCommand));
   qstyle_ns->addConstant("PE_PanelButtonBevel",      new QoreNode((int64)QStyle::PE_PanelButtonBevel));
   qstyle_ns->addConstant("PE_PanelButtonTool",       new QoreNode((int64)QStyle::PE_PanelButtonTool));
   qstyle_ns->addConstant("PE_PanelMenuBar",          new QoreNode((int64)QStyle::PE_PanelMenuBar));
   qstyle_ns->addConstant("PE_PanelToolBar",          new QoreNode((int64)QStyle::PE_PanelToolBar));
   qstyle_ns->addConstant("PE_PanelLineEdit",         new QoreNode((int64)QStyle::PE_PanelLineEdit));
   qstyle_ns->addConstant("PE_IndicatorArrowDown",    new QoreNode((int64)QStyle::PE_IndicatorArrowDown));
   qstyle_ns->addConstant("PE_IndicatorArrowLeft",    new QoreNode((int64)QStyle::PE_IndicatorArrowLeft));
   qstyle_ns->addConstant("PE_IndicatorArrowRight",   new QoreNode((int64)QStyle::PE_IndicatorArrowRight));
   qstyle_ns->addConstant("PE_IndicatorArrowUp",      new QoreNode((int64)QStyle::PE_IndicatorArrowUp));
   qstyle_ns->addConstant("PE_IndicatorBranch",       new QoreNode((int64)QStyle::PE_IndicatorBranch));
   qstyle_ns->addConstant("PE_IndicatorButtonDropDown", new QoreNode((int64)QStyle::PE_IndicatorButtonDropDown));
   qstyle_ns->addConstant("PE_IndicatorViewItemCheck", new QoreNode((int64)QStyle::PE_IndicatorViewItemCheck));
   qstyle_ns->addConstant("PE_IndicatorCheckBox",     new QoreNode((int64)QStyle::PE_IndicatorCheckBox));
   qstyle_ns->addConstant("PE_IndicatorDockWidgetResizeHandle", new QoreNode((int64)QStyle::PE_IndicatorDockWidgetResizeHandle));
   qstyle_ns->addConstant("PE_IndicatorHeaderArrow",  new QoreNode((int64)QStyle::PE_IndicatorHeaderArrow));
   qstyle_ns->addConstant("PE_IndicatorMenuCheckMark", new QoreNode((int64)QStyle::PE_IndicatorMenuCheckMark));
   qstyle_ns->addConstant("PE_IndicatorProgressChunk", new QoreNode((int64)QStyle::PE_IndicatorProgressChunk));
   qstyle_ns->addConstant("PE_IndicatorRadioButton",  new QoreNode((int64)QStyle::PE_IndicatorRadioButton));
   qstyle_ns->addConstant("PE_IndicatorSpinDown",     new QoreNode((int64)QStyle::PE_IndicatorSpinDown));
   qstyle_ns->addConstant("PE_IndicatorSpinMinus",    new QoreNode((int64)QStyle::PE_IndicatorSpinMinus));
   qstyle_ns->addConstant("PE_IndicatorSpinPlus",     new QoreNode((int64)QStyle::PE_IndicatorSpinPlus));
   qstyle_ns->addConstant("PE_IndicatorSpinUp",       new QoreNode((int64)QStyle::PE_IndicatorSpinUp));
   qstyle_ns->addConstant("PE_IndicatorToolBarHandle", new QoreNode((int64)QStyle::PE_IndicatorToolBarHandle));
   qstyle_ns->addConstant("PE_IndicatorToolBarSeparator", new QoreNode((int64)QStyle::PE_IndicatorToolBarSeparator));
   qstyle_ns->addConstant("PE_PanelTipLabel",         new QoreNode((int64)QStyle::PE_PanelTipLabel));
   qstyle_ns->addConstant("PE_IndicatorTabTear",      new QoreNode((int64)QStyle::PE_IndicatorTabTear));
   qstyle_ns->addConstant("PE_PanelScrollAreaCorner", new QoreNode((int64)QStyle::PE_PanelScrollAreaCorner));
   qstyle_ns->addConstant("PE_Widget",                new QoreNode((int64)QStyle::PE_Widget));
   qstyle_ns->addConstant("PE_IndicatorColumnViewArrow", new QoreNode((int64)QStyle::PE_IndicatorColumnViewArrow));
   qstyle_ns->addConstant("PE_CustomBase",            new QoreNode((int64)QStyle::PE_CustomBase));

   // PixelMetric enum
   qstyle_ns->addConstant("PM_ButtonMargin",          new QoreNode((int64)QStyle::PM_ButtonMargin));
   qstyle_ns->addConstant("PM_ButtonDefaultIndicator", new QoreNode((int64)QStyle::PM_ButtonDefaultIndicator));
   qstyle_ns->addConstant("PM_MenuButtonIndicator",   new QoreNode((int64)QStyle::PM_MenuButtonIndicator));
   qstyle_ns->addConstant("PM_ButtonShiftHorizontal", new QoreNode((int64)QStyle::PM_ButtonShiftHorizontal));
   qstyle_ns->addConstant("PM_ButtonShiftVertical",   new QoreNode((int64)QStyle::PM_ButtonShiftVertical));
   qstyle_ns->addConstant("PM_DefaultFrameWidth",     new QoreNode((int64)QStyle::PM_DefaultFrameWidth));
   qstyle_ns->addConstant("PM_SpinBoxFrameWidth",     new QoreNode((int64)QStyle::PM_SpinBoxFrameWidth));
   qstyle_ns->addConstant("PM_ComboBoxFrameWidth",    new QoreNode((int64)QStyle::PM_ComboBoxFrameWidth));
   qstyle_ns->addConstant("PM_MaximumDragDistance",   new QoreNode((int64)QStyle::PM_MaximumDragDistance));
   qstyle_ns->addConstant("PM_ScrollBarExtent",       new QoreNode((int64)QStyle::PM_ScrollBarExtent));
   qstyle_ns->addConstant("PM_ScrollBarSliderMin",    new QoreNode((int64)QStyle::PM_ScrollBarSliderMin));
   qstyle_ns->addConstant("PM_SliderThickness",       new QoreNode((int64)QStyle::PM_SliderThickness));
   qstyle_ns->addConstant("PM_SliderControlThickness", new QoreNode((int64)QStyle::PM_SliderControlThickness));
   qstyle_ns->addConstant("PM_SliderLength",          new QoreNode((int64)QStyle::PM_SliderLength));
   qstyle_ns->addConstant("PM_SliderTickmarkOffset",  new QoreNode((int64)QStyle::PM_SliderTickmarkOffset));
   qstyle_ns->addConstant("PM_SliderSpaceAvailable",  new QoreNode((int64)QStyle::PM_SliderSpaceAvailable));
   qstyle_ns->addConstant("PM_DockWidgetSeparatorExtent", new QoreNode((int64)QStyle::PM_DockWidgetSeparatorExtent));
   qstyle_ns->addConstant("PM_DockWidgetHandleExtent", new QoreNode((int64)QStyle::PM_DockWidgetHandleExtent));
   qstyle_ns->addConstant("PM_DockWidgetFrameWidth",  new QoreNode((int64)QStyle::PM_DockWidgetFrameWidth));
   qstyle_ns->addConstant("PM_TabBarTabOverlap",      new QoreNode((int64)QStyle::PM_TabBarTabOverlap));
   qstyle_ns->addConstant("PM_TabBarTabHSpace",       new QoreNode((int64)QStyle::PM_TabBarTabHSpace));
   qstyle_ns->addConstant("PM_TabBarTabVSpace",       new QoreNode((int64)QStyle::PM_TabBarTabVSpace));
   qstyle_ns->addConstant("PM_TabBarBaseHeight",      new QoreNode((int64)QStyle::PM_TabBarBaseHeight));
   qstyle_ns->addConstant("PM_TabBarBaseOverlap",     new QoreNode((int64)QStyle::PM_TabBarBaseOverlap));
   qstyle_ns->addConstant("PM_ProgressBarChunkWidth", new QoreNode((int64)QStyle::PM_ProgressBarChunkWidth));
   qstyle_ns->addConstant("PM_SplitterWidth",         new QoreNode((int64)QStyle::PM_SplitterWidth));
   qstyle_ns->addConstant("PM_TitleBarHeight",        new QoreNode((int64)QStyle::PM_TitleBarHeight));
   qstyle_ns->addConstant("PM_MenuScrollerHeight",    new QoreNode((int64)QStyle::PM_MenuScrollerHeight));
   qstyle_ns->addConstant("PM_MenuHMargin",           new QoreNode((int64)QStyle::PM_MenuHMargin));
   qstyle_ns->addConstant("PM_MenuVMargin",           new QoreNode((int64)QStyle::PM_MenuVMargin));
   qstyle_ns->addConstant("PM_MenuPanelWidth",        new QoreNode((int64)QStyle::PM_MenuPanelWidth));
   qstyle_ns->addConstant("PM_MenuTearoffHeight",     new QoreNode((int64)QStyle::PM_MenuTearoffHeight));
   qstyle_ns->addConstant("PM_MenuDesktopFrameWidth", new QoreNode((int64)QStyle::PM_MenuDesktopFrameWidth));
   qstyle_ns->addConstant("PM_MenuBarPanelWidth",     new QoreNode((int64)QStyle::PM_MenuBarPanelWidth));
   qstyle_ns->addConstant("PM_MenuBarItemSpacing",    new QoreNode((int64)QStyle::PM_MenuBarItemSpacing));
   qstyle_ns->addConstant("PM_MenuBarVMargin",        new QoreNode((int64)QStyle::PM_MenuBarVMargin));
   qstyle_ns->addConstant("PM_MenuBarHMargin",        new QoreNode((int64)QStyle::PM_MenuBarHMargin));
   qstyle_ns->addConstant("PM_IndicatorWidth",        new QoreNode((int64)QStyle::PM_IndicatorWidth));
   qstyle_ns->addConstant("PM_IndicatorHeight",       new QoreNode((int64)QStyle::PM_IndicatorHeight));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorWidth", new QoreNode((int64)QStyle::PM_ExclusiveIndicatorWidth));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorHeight", new QoreNode((int64)QStyle::PM_ExclusiveIndicatorHeight));
   qstyle_ns->addConstant("PM_CheckListButtonSize",   new QoreNode((int64)QStyle::PM_CheckListButtonSize));
   qstyle_ns->addConstant("PM_CheckListControllerSize", new QoreNode((int64)QStyle::PM_CheckListControllerSize));
   qstyle_ns->addConstant("PM_DialogButtonsSeparator", new QoreNode((int64)QStyle::PM_DialogButtonsSeparator));
   qstyle_ns->addConstant("PM_DialogButtonsButtonWidth", new QoreNode((int64)QStyle::PM_DialogButtonsButtonWidth));
   qstyle_ns->addConstant("PM_DialogButtonsButtonHeight", new QoreNode((int64)QStyle::PM_DialogButtonsButtonHeight));
   qstyle_ns->addConstant("PM_MdiSubWindowFrameWidth", new QoreNode((int64)QStyle::PM_MdiSubWindowFrameWidth));
   qstyle_ns->addConstant("PM_MDIFrameWidth",         new QoreNode((int64)QStyle::PM_MDIFrameWidth));
   qstyle_ns->addConstant("PM_MdiSubWindowMinimizedWidth", new QoreNode((int64)QStyle::PM_MdiSubWindowMinimizedWidth));
   qstyle_ns->addConstant("PM_MDIMinimizedWidth",     new QoreNode((int64)QStyle::PM_MDIMinimizedWidth));
   qstyle_ns->addConstant("PM_HeaderMargin",          new QoreNode((int64)QStyle::PM_HeaderMargin));
   qstyle_ns->addConstant("PM_HeaderMarkSize",        new QoreNode((int64)QStyle::PM_HeaderMarkSize));
   qstyle_ns->addConstant("PM_HeaderGripMargin",      new QoreNode((int64)QStyle::PM_HeaderGripMargin));
   qstyle_ns->addConstant("PM_TabBarTabShiftHorizontal", new QoreNode((int64)QStyle::PM_TabBarTabShiftHorizontal));
   qstyle_ns->addConstant("PM_TabBarTabShiftVertical", new QoreNode((int64)QStyle::PM_TabBarTabShiftVertical));
   qstyle_ns->addConstant("PM_TabBarScrollButtonWidth", new QoreNode((int64)QStyle::PM_TabBarScrollButtonWidth));
   qstyle_ns->addConstant("PM_ToolBarFrameWidth",     new QoreNode((int64)QStyle::PM_ToolBarFrameWidth));
   qstyle_ns->addConstant("PM_ToolBarHandleExtent",   new QoreNode((int64)QStyle::PM_ToolBarHandleExtent));
   qstyle_ns->addConstant("PM_ToolBarItemSpacing",    new QoreNode((int64)QStyle::PM_ToolBarItemSpacing));
   qstyle_ns->addConstant("PM_ToolBarItemMargin",     new QoreNode((int64)QStyle::PM_ToolBarItemMargin));
   qstyle_ns->addConstant("PM_ToolBarSeparatorExtent", new QoreNode((int64)QStyle::PM_ToolBarSeparatorExtent));
   qstyle_ns->addConstant("PM_ToolBarExtensionExtent", new QoreNode((int64)QStyle::PM_ToolBarExtensionExtent));
   qstyle_ns->addConstant("PM_SpinBoxSliderHeight",   new QoreNode((int64)QStyle::PM_SpinBoxSliderHeight));
   qstyle_ns->addConstant("PM_DefaultTopLevelMargin", new QoreNode((int64)QStyle::PM_DefaultTopLevelMargin));
   qstyle_ns->addConstant("PM_DefaultChildMargin",    new QoreNode((int64)QStyle::PM_DefaultChildMargin));
   qstyle_ns->addConstant("PM_DefaultLayoutSpacing",  new QoreNode((int64)QStyle::PM_DefaultLayoutSpacing));
   qstyle_ns->addConstant("PM_ToolBarIconSize",       new QoreNode((int64)QStyle::PM_ToolBarIconSize));
   qstyle_ns->addConstant("PM_ListViewIconSize",      new QoreNode((int64)QStyle::PM_ListViewIconSize));
   qstyle_ns->addConstant("PM_IconViewIconSize",      new QoreNode((int64)QStyle::PM_IconViewIconSize));
   qstyle_ns->addConstant("PM_SmallIconSize",         new QoreNode((int64)QStyle::PM_SmallIconSize));
   qstyle_ns->addConstant("PM_LargeIconSize",         new QoreNode((int64)QStyle::PM_LargeIconSize));
   qstyle_ns->addConstant("PM_FocusFrameVMargin",     new QoreNode((int64)QStyle::PM_FocusFrameVMargin));
   qstyle_ns->addConstant("PM_FocusFrameHMargin",     new QoreNode((int64)QStyle::PM_FocusFrameHMargin));
   qstyle_ns->addConstant("PM_ToolTipLabelFrameWidth", new QoreNode((int64)QStyle::PM_ToolTipLabelFrameWidth));
   qstyle_ns->addConstant("PM_CheckBoxLabelSpacing",  new QoreNode((int64)QStyle::PM_CheckBoxLabelSpacing));
   qstyle_ns->addConstant("PM_TabBarIconSize",        new QoreNode((int64)QStyle::PM_TabBarIconSize));
   qstyle_ns->addConstant("PM_SizeGripSize",          new QoreNode((int64)QStyle::PM_SizeGripSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleMargin", new QoreNode((int64)QStyle::PM_DockWidgetTitleMargin));
   qstyle_ns->addConstant("PM_MessageBoxIconSize",    new QoreNode((int64)QStyle::PM_MessageBoxIconSize));
   qstyle_ns->addConstant("PM_ButtonIconSize",        new QoreNode((int64)QStyle::PM_ButtonIconSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleBarButtonMargin", new QoreNode((int64)QStyle::PM_DockWidgetTitleBarButtonMargin));
   qstyle_ns->addConstant("PM_RadioButtonLabelSpacing", new QoreNode((int64)QStyle::PM_RadioButtonLabelSpacing));
   qstyle_ns->addConstant("PM_LayoutLeftMargin",      new QoreNode((int64)QStyle::PM_LayoutLeftMargin));
   qstyle_ns->addConstant("PM_LayoutTopMargin",       new QoreNode((int64)QStyle::PM_LayoutTopMargin));
   qstyle_ns->addConstant("PM_LayoutRightMargin",     new QoreNode((int64)QStyle::PM_LayoutRightMargin));
   qstyle_ns->addConstant("PM_LayoutBottomMargin",    new QoreNode((int64)QStyle::PM_LayoutBottomMargin));
   qstyle_ns->addConstant("PM_LayoutHorizontalSpacing", new QoreNode((int64)QStyle::PM_LayoutHorizontalSpacing));
   qstyle_ns->addConstant("PM_LayoutVerticalSpacing", new QoreNode((int64)QStyle::PM_LayoutVerticalSpacing));
   qstyle_ns->addConstant("PM_CustomBase",            new QoreNode((int64)QStyle::PM_CustomBase));

   // StateFlag enum
   qstyle_ns->addConstant("State_None",               new QoreNode((int64)QStyle::State_None));
   qstyle_ns->addConstant("State_Enabled",            new QoreNode((int64)QStyle::State_Enabled));
   qstyle_ns->addConstant("State_Raised",             new QoreNode((int64)QStyle::State_Raised));
   qstyle_ns->addConstant("State_Sunken",             new QoreNode((int64)QStyle::State_Sunken));
   qstyle_ns->addConstant("State_Off",                new QoreNode((int64)QStyle::State_Off));
   qstyle_ns->addConstant("State_NoChange",           new QoreNode((int64)QStyle::State_NoChange));
   qstyle_ns->addConstant("State_On",                 new QoreNode((int64)QStyle::State_On));
   qstyle_ns->addConstant("State_DownArrow",          new QoreNode((int64)QStyle::State_DownArrow));
   qstyle_ns->addConstant("State_Horizontal",         new QoreNode((int64)QStyle::State_Horizontal));
   qstyle_ns->addConstant("State_HasFocus",           new QoreNode((int64)QStyle::State_HasFocus));
   qstyle_ns->addConstant("State_Top",                new QoreNode((int64)QStyle::State_Top));
   qstyle_ns->addConstant("State_Bottom",             new QoreNode((int64)QStyle::State_Bottom));
   qstyle_ns->addConstant("State_FocusAtBorder",      new QoreNode((int64)QStyle::State_FocusAtBorder));
   qstyle_ns->addConstant("State_AutoRaise",          new QoreNode((int64)QStyle::State_AutoRaise));
   qstyle_ns->addConstant("State_MouseOver",          new QoreNode((int64)QStyle::State_MouseOver));
   qstyle_ns->addConstant("State_UpArrow",            new QoreNode((int64)QStyle::State_UpArrow));
   qstyle_ns->addConstant("State_Selected",           new QoreNode((int64)QStyle::State_Selected));
   qstyle_ns->addConstant("State_Active",             new QoreNode((int64)QStyle::State_Active));
   qstyle_ns->addConstant("State_Window",             new QoreNode((int64)QStyle::State_Window));
   qstyle_ns->addConstant("State_Open",               new QoreNode((int64)QStyle::State_Open));
   qstyle_ns->addConstant("State_Children",           new QoreNode((int64)QStyle::State_Children));
   qstyle_ns->addConstant("State_Item",               new QoreNode((int64)QStyle::State_Item));
   qstyle_ns->addConstant("State_Sibling",            new QoreNode((int64)QStyle::State_Sibling));
   qstyle_ns->addConstant("State_Editing",            new QoreNode((int64)QStyle::State_Editing));
   qstyle_ns->addConstant("State_KeyboardFocusChange", new QoreNode((int64)QStyle::State_KeyboardFocusChange));
#ifdef QT_KEYPAD_NAVIGATION
   qstyle_ns->addConstant("State_HasEditFocus",       new QoreNode((int64)QStyle::State_HasEditFocus));
#endif
   qstyle_ns->addConstant("State_ReadOnly",           new QoreNode((int64)QStyle::State_ReadOnly));
   qstyle_ns->addConstant("State_Small",              new QoreNode((int64)QStyle::State_Small));
   qstyle_ns->addConstant("State_Mini",               new QoreNode((int64)QStyle::State_Mini));
   
   // ControlElement enum
   qstyle_ns->addConstant("CE_PushButton",            new QoreNode((int64)QStyle::CE_PushButton));
   qstyle_ns->addConstant("CE_PushButtonBevel",       new QoreNode((int64)QStyle::CE_PushButtonBevel));
   qstyle_ns->addConstant("CE_PushButtonLabel",       new QoreNode((int64)QStyle::CE_PushButtonLabel));
   qstyle_ns->addConstant("CE_CheckBox",              new QoreNode((int64)QStyle::CE_CheckBox));
   qstyle_ns->addConstant("CE_CheckBoxLabel",         new QoreNode((int64)QStyle::CE_CheckBoxLabel));
   qstyle_ns->addConstant("CE_RadioButton",           new QoreNode((int64)QStyle::CE_RadioButton));
   qstyle_ns->addConstant("CE_RadioButtonLabel",      new QoreNode((int64)QStyle::CE_RadioButtonLabel));
   qstyle_ns->addConstant("CE_TabBarTab",             new QoreNode((int64)QStyle::CE_TabBarTab));
   qstyle_ns->addConstant("CE_TabBarTabShape",        new QoreNode((int64)QStyle::CE_TabBarTabShape));
   qstyle_ns->addConstant("CE_TabBarTabLabel",        new QoreNode((int64)QStyle::CE_TabBarTabLabel));
   qstyle_ns->addConstant("CE_ProgressBar",           new QoreNode((int64)QStyle::CE_ProgressBar));
   qstyle_ns->addConstant("CE_ProgressBarGroove",     new QoreNode((int64)QStyle::CE_ProgressBarGroove));
   qstyle_ns->addConstant("CE_ProgressBarContents",   new QoreNode((int64)QStyle::CE_ProgressBarContents));
   qstyle_ns->addConstant("CE_ProgressBarLabel",      new QoreNode((int64)QStyle::CE_ProgressBarLabel));
   qstyle_ns->addConstant("CE_MenuItem",              new QoreNode((int64)QStyle::CE_MenuItem));
   qstyle_ns->addConstant("CE_MenuScroller",          new QoreNode((int64)QStyle::CE_MenuScroller));
   qstyle_ns->addConstant("CE_MenuVMargin",           new QoreNode((int64)QStyle::CE_MenuVMargin));
   qstyle_ns->addConstant("CE_MenuHMargin",           new QoreNode((int64)QStyle::CE_MenuHMargin));
   qstyle_ns->addConstant("CE_MenuTearoff",           new QoreNode((int64)QStyle::CE_MenuTearoff));
   qstyle_ns->addConstant("CE_MenuEmptyArea",         new QoreNode((int64)QStyle::CE_MenuEmptyArea));
   qstyle_ns->addConstant("CE_MenuBarItem",           new QoreNode((int64)QStyle::CE_MenuBarItem));
   qstyle_ns->addConstant("CE_MenuBarEmptyArea",      new QoreNode((int64)QStyle::CE_MenuBarEmptyArea));
   qstyle_ns->addConstant("CE_ToolButtonLabel",       new QoreNode((int64)QStyle::CE_ToolButtonLabel));
   qstyle_ns->addConstant("CE_Header",                new QoreNode((int64)QStyle::CE_Header));
   qstyle_ns->addConstant("CE_HeaderSection",         new QoreNode((int64)QStyle::CE_HeaderSection));
   qstyle_ns->addConstant("CE_HeaderLabel",           new QoreNode((int64)QStyle::CE_HeaderLabel));
   qstyle_ns->addConstant("CE_Q3DockWindowEmptyArea", new QoreNode((int64)QStyle::CE_Q3DockWindowEmptyArea));
   qstyle_ns->addConstant("CE_ToolBoxTab",            new QoreNode((int64)QStyle::CE_ToolBoxTab));
   qstyle_ns->addConstant("CE_SizeGrip",              new QoreNode((int64)QStyle::CE_SizeGrip));
   qstyle_ns->addConstant("CE_Splitter",              new QoreNode((int64)QStyle::CE_Splitter));
   qstyle_ns->addConstant("CE_RubberBand",            new QoreNode((int64)QStyle::CE_RubberBand));
   qstyle_ns->addConstant("CE_DockWidgetTitle",       new QoreNode((int64)QStyle::CE_DockWidgetTitle));
   qstyle_ns->addConstant("CE_ScrollBarAddLine",      new QoreNode((int64)QStyle::CE_ScrollBarAddLine));
   qstyle_ns->addConstant("CE_ScrollBarSubLine",      new QoreNode((int64)QStyle::CE_ScrollBarSubLine));
   qstyle_ns->addConstant("CE_ScrollBarAddPage",      new QoreNode((int64)QStyle::CE_ScrollBarAddPage));
   qstyle_ns->addConstant("CE_ScrollBarSubPage",      new QoreNode((int64)QStyle::CE_ScrollBarSubPage));
   qstyle_ns->addConstant("CE_ScrollBarSlider",       new QoreNode((int64)QStyle::CE_ScrollBarSlider));
   qstyle_ns->addConstant("CE_ScrollBarFirst",        new QoreNode((int64)QStyle::CE_ScrollBarFirst));
   qstyle_ns->addConstant("CE_ScrollBarLast",         new QoreNode((int64)QStyle::CE_ScrollBarLast));
   qstyle_ns->addConstant("CE_FocusFrame",            new QoreNode((int64)QStyle::CE_FocusFrame));
   qstyle_ns->addConstant("CE_ComboBoxLabel",         new QoreNode((int64)QStyle::CE_ComboBoxLabel));
   qstyle_ns->addConstant("CE_ToolBar",               new QoreNode((int64)QStyle::CE_ToolBar));
   qstyle_ns->addConstant("CE_ToolBoxTabShape",       new QoreNode((int64)QStyle::CE_ToolBoxTabShape));
   qstyle_ns->addConstant("CE_ToolBoxTabLabel",       new QoreNode((int64)QStyle::CE_ToolBoxTabLabel));
   qstyle_ns->addConstant("CE_HeaderEmptyArea",       new QoreNode((int64)QStyle::CE_HeaderEmptyArea));
   qstyle_ns->addConstant("CE_ColumnViewGrip",        new QoreNode((int64)QStyle::CE_ColumnViewGrip));
   qstyle_ns->addConstant("CE_CustomBase",            new QoreNode((int64)QStyle::CE_CustomBase));

   // SubElement enum
   qstyle_ns->addConstant("SE_PushButtonContents",    new QoreNode((int64)QStyle::SE_PushButtonContents));
   qstyle_ns->addConstant("SE_PushButtonFocusRect",   new QoreNode((int64)QStyle::SE_PushButtonFocusRect));
   qstyle_ns->addConstant("SE_CheckBoxIndicator",     new QoreNode((int64)QStyle::SE_CheckBoxIndicator));
   qstyle_ns->addConstant("SE_CheckBoxContents",      new QoreNode((int64)QStyle::SE_CheckBoxContents));
   qstyle_ns->addConstant("SE_CheckBoxFocusRect",     new QoreNode((int64)QStyle::SE_CheckBoxFocusRect));
   qstyle_ns->addConstant("SE_CheckBoxClickRect",     new QoreNode((int64)QStyle::SE_CheckBoxClickRect));
   qstyle_ns->addConstant("SE_RadioButtonIndicator",  new QoreNode((int64)QStyle::SE_RadioButtonIndicator));
   qstyle_ns->addConstant("SE_RadioButtonContents",   new QoreNode((int64)QStyle::SE_RadioButtonContents));
   qstyle_ns->addConstant("SE_RadioButtonFocusRect",  new QoreNode((int64)QStyle::SE_RadioButtonFocusRect));
   qstyle_ns->addConstant("SE_RadioButtonClickRect",  new QoreNode((int64)QStyle::SE_RadioButtonClickRect));
   qstyle_ns->addConstant("SE_ComboBoxFocusRect",     new QoreNode((int64)QStyle::SE_ComboBoxFocusRect));
   qstyle_ns->addConstant("SE_SliderFocusRect",       new QoreNode((int64)QStyle::SE_SliderFocusRect));
   qstyle_ns->addConstant("SE_Q3DockWindowHandleRect", new QoreNode((int64)QStyle::SE_Q3DockWindowHandleRect));
   qstyle_ns->addConstant("SE_ProgressBarGroove",     new QoreNode((int64)QStyle::SE_ProgressBarGroove));
   qstyle_ns->addConstant("SE_ProgressBarContents",   new QoreNode((int64)QStyle::SE_ProgressBarContents));
   qstyle_ns->addConstant("SE_ProgressBarLabel",      new QoreNode((int64)QStyle::SE_ProgressBarLabel));
   qstyle_ns->addConstant("SE_DialogButtonAccept",    new QoreNode((int64)QStyle::SE_DialogButtonAccept));
   qstyle_ns->addConstant("SE_DialogButtonReject",    new QoreNode((int64)QStyle::SE_DialogButtonReject));
   qstyle_ns->addConstant("SE_DialogButtonApply",     new QoreNode((int64)QStyle::SE_DialogButtonApply));
   qstyle_ns->addConstant("SE_DialogButtonHelp",      new QoreNode((int64)QStyle::SE_DialogButtonHelp));
   qstyle_ns->addConstant("SE_DialogButtonAll",       new QoreNode((int64)QStyle::SE_DialogButtonAll));
   qstyle_ns->addConstant("SE_DialogButtonAbort",     new QoreNode((int64)QStyle::SE_DialogButtonAbort));
   qstyle_ns->addConstant("SE_DialogButtonIgnore",    new QoreNode((int64)QStyle::SE_DialogButtonIgnore));
   qstyle_ns->addConstant("SE_DialogButtonRetry",     new QoreNode((int64)QStyle::SE_DialogButtonRetry));
   qstyle_ns->addConstant("SE_DialogButtonCustom",    new QoreNode((int64)QStyle::SE_DialogButtonCustom));
   qstyle_ns->addConstant("SE_ToolBoxTabContents",    new QoreNode((int64)QStyle::SE_ToolBoxTabContents));
   qstyle_ns->addConstant("SE_HeaderLabel",           new QoreNode((int64)QStyle::SE_HeaderLabel));
   qstyle_ns->addConstant("SE_HeaderArrow",           new QoreNode((int64)QStyle::SE_HeaderArrow));
   qstyle_ns->addConstant("SE_TabWidgetTabBar",       new QoreNode((int64)QStyle::SE_TabWidgetTabBar));
   qstyle_ns->addConstant("SE_TabWidgetTabPane",      new QoreNode((int64)QStyle::SE_TabWidgetTabPane));
   qstyle_ns->addConstant("SE_TabWidgetTabContents",  new QoreNode((int64)QStyle::SE_TabWidgetTabContents));
   qstyle_ns->addConstant("SE_TabWidgetLeftCorner",   new QoreNode((int64)QStyle::SE_TabWidgetLeftCorner));
   qstyle_ns->addConstant("SE_TabWidgetRightCorner",  new QoreNode((int64)QStyle::SE_TabWidgetRightCorner));
   qstyle_ns->addConstant("SE_ViewItemCheckIndicator", new QoreNode((int64)QStyle::SE_ViewItemCheckIndicator));
   qstyle_ns->addConstant("SE_TabBarTearIndicator",   new QoreNode((int64)QStyle::SE_TabBarTearIndicator));
   qstyle_ns->addConstant("SE_TreeViewDisclosureItem", new QoreNode((int64)QStyle::SE_TreeViewDisclosureItem));
   qstyle_ns->addConstant("SE_LineEditContents",      new QoreNode((int64)QStyle::SE_LineEditContents));
   qstyle_ns->addConstant("SE_FrameContents",         new QoreNode((int64)QStyle::SE_FrameContents));
   qstyle_ns->addConstant("SE_DockWidgetCloseButton", new QoreNode((int64)QStyle::SE_DockWidgetCloseButton));
   qstyle_ns->addConstant("SE_DockWidgetFloatButton", new QoreNode((int64)QStyle::SE_DockWidgetFloatButton));
   qstyle_ns->addConstant("SE_DockWidgetTitleBarText", new QoreNode((int64)QStyle::SE_DockWidgetTitleBarText));
   qstyle_ns->addConstant("SE_DockWidgetIcon",        new QoreNode((int64)QStyle::SE_DockWidgetIcon));
   qstyle_ns->addConstant("SE_CheckBoxLayoutItem",    new QoreNode((int64)QStyle::SE_CheckBoxLayoutItem));
   qstyle_ns->addConstant("SE_ComboBoxLayoutItem",    new QoreNode((int64)QStyle::SE_ComboBoxLayoutItem));
   qstyle_ns->addConstant("SE_DateTimeEditLayoutItem", new QoreNode((int64)QStyle::SE_DateTimeEditLayoutItem));
   qstyle_ns->addConstant("SE_DialogButtonBoxLayoutItem", new QoreNode((int64)QStyle::SE_DialogButtonBoxLayoutItem));
   qstyle_ns->addConstant("SE_LabelLayoutItem",       new QoreNode((int64)QStyle::SE_LabelLayoutItem));
   qstyle_ns->addConstant("SE_ProgressBarLayoutItem", new QoreNode((int64)QStyle::SE_ProgressBarLayoutItem));
   qstyle_ns->addConstant("SE_PushButtonLayoutItem",  new QoreNode((int64)QStyle::SE_PushButtonLayoutItem));
   qstyle_ns->addConstant("SE_RadioButtonLayoutItem", new QoreNode((int64)QStyle::SE_RadioButtonLayoutItem));
   qstyle_ns->addConstant("SE_SliderLayoutItem",      new QoreNode((int64)QStyle::SE_SliderLayoutItem));
   qstyle_ns->addConstant("SE_SpinBoxLayoutItem",     new QoreNode((int64)QStyle::SE_SpinBoxLayoutItem));
   qstyle_ns->addConstant("SE_ToolButtonLayoutItem",  new QoreNode((int64)QStyle::SE_ToolButtonLayoutItem));
   qstyle_ns->addConstant("SE_FrameLayoutItem",       new QoreNode((int64)QStyle::SE_FrameLayoutItem));
   qstyle_ns->addConstant("SE_GroupBoxLayoutItem",    new QoreNode((int64)QStyle::SE_GroupBoxLayoutItem));
   qstyle_ns->addConstant("SE_TabWidgetLayoutItem",   new QoreNode((int64)QStyle::SE_TabWidgetLayoutItem));
   qstyle_ns->addConstant("SE_CustomBase",            new QoreNode((int64)QStyle::SE_CustomBase));

   // ComplexControl enum
   qstyle_ns->addConstant("CC_SpinBox",               new QoreNode((int64)QStyle::CC_SpinBox));
   qstyle_ns->addConstant("CC_ComboBox",              new QoreNode((int64)QStyle::CC_ComboBox));
   qstyle_ns->addConstant("CC_ScrollBar",             new QoreNode((int64)QStyle::CC_ScrollBar));
   qstyle_ns->addConstant("CC_Slider",                new QoreNode((int64)QStyle::CC_Slider));
   qstyle_ns->addConstant("CC_ToolButton",            new QoreNode((int64)QStyle::CC_ToolButton));
   qstyle_ns->addConstant("CC_TitleBar",              new QoreNode((int64)QStyle::CC_TitleBar));
   qstyle_ns->addConstant("CC_Q3ListView",            new QoreNode((int64)QStyle::CC_Q3ListView));
   qstyle_ns->addConstant("CC_Dial",                  new QoreNode((int64)QStyle::CC_Dial));
   qstyle_ns->addConstant("CC_GroupBox",              new QoreNode((int64)QStyle::CC_GroupBox));
   qstyle_ns->addConstant("CC_MdiControls",           new QoreNode((int64)QStyle::CC_MdiControls));
   qstyle_ns->addConstant("CC_CustomBase",            new QoreNode((int64)QStyle::CC_CustomBase));
   
   // SubControl enum
   qstyle_ns->addConstant("SC_None",                  new QoreNode((int64)QStyle::SC_None));
   qstyle_ns->addConstant("SC_ScrollBarAddLine",      new QoreNode((int64)QStyle::SC_ScrollBarAddLine));
   qstyle_ns->addConstant("SC_ScrollBarSubLine",      new QoreNode((int64)QStyle::SC_ScrollBarSubLine));
   qstyle_ns->addConstant("SC_ScrollBarAddPage",      new QoreNode((int64)QStyle::SC_ScrollBarAddPage));
   qstyle_ns->addConstant("SC_ScrollBarSubPage",      new QoreNode((int64)QStyle::SC_ScrollBarSubPage));
   qstyle_ns->addConstant("SC_ScrollBarFirst",        new QoreNode((int64)QStyle::SC_ScrollBarFirst));
   qstyle_ns->addConstant("SC_ScrollBarLast",         new QoreNode((int64)QStyle::SC_ScrollBarLast));
   qstyle_ns->addConstant("SC_ScrollBarSlider",       new QoreNode((int64)QStyle::SC_ScrollBarSlider));
   qstyle_ns->addConstant("SC_ScrollBarGroove",       new QoreNode((int64)QStyle::SC_ScrollBarGroove));
   qstyle_ns->addConstant("SC_SpinBoxUp",             new QoreNode((int64)QStyle::SC_SpinBoxUp));
   qstyle_ns->addConstant("SC_SpinBoxDown",           new QoreNode((int64)QStyle::SC_SpinBoxDown));
   qstyle_ns->addConstant("SC_SpinBoxFrame",          new QoreNode((int64)QStyle::SC_SpinBoxFrame));
   qstyle_ns->addConstant("SC_SpinBoxEditField",      new QoreNode((int64)QStyle::SC_SpinBoxEditField));
   qstyle_ns->addConstant("SC_ComboBoxFrame",         new QoreNode((int64)QStyle::SC_ComboBoxFrame));
   qstyle_ns->addConstant("SC_ComboBoxEditField",     new QoreNode((int64)QStyle::SC_ComboBoxEditField));
   qstyle_ns->addConstant("SC_ComboBoxArrow",         new QoreNode((int64)QStyle::SC_ComboBoxArrow));
   qstyle_ns->addConstant("SC_ComboBoxListBoxPopup",  new QoreNode((int64)QStyle::SC_ComboBoxListBoxPopup));
   qstyle_ns->addConstant("SC_SliderGroove",          new QoreNode((int64)QStyle::SC_SliderGroove));
   qstyle_ns->addConstant("SC_SliderHandle",          new QoreNode((int64)QStyle::SC_SliderHandle));
   qstyle_ns->addConstant("SC_SliderTickmarks",       new QoreNode((int64)QStyle::SC_SliderTickmarks));
   qstyle_ns->addConstant("SC_ToolButton",            new QoreNode((int64)QStyle::SC_ToolButton));
   qstyle_ns->addConstant("SC_ToolButtonMenu",        new QoreNode((int64)QStyle::SC_ToolButtonMenu));
   qstyle_ns->addConstant("SC_TitleBarSysMenu",       new QoreNode((int64)QStyle::SC_TitleBarSysMenu));
   qstyle_ns->addConstant("SC_TitleBarMinButton",     new QoreNode((int64)QStyle::SC_TitleBarMinButton));
   qstyle_ns->addConstant("SC_TitleBarMaxButton",     new QoreNode((int64)QStyle::SC_TitleBarMaxButton));
   qstyle_ns->addConstant("SC_TitleBarCloseButton",   new QoreNode((int64)QStyle::SC_TitleBarCloseButton));
   qstyle_ns->addConstant("SC_TitleBarNormalButton",  new QoreNode((int64)QStyle::SC_TitleBarNormalButton));
   qstyle_ns->addConstant("SC_TitleBarShadeButton",   new QoreNode((int64)QStyle::SC_TitleBarShadeButton));
   qstyle_ns->addConstant("SC_TitleBarUnshadeButton", new QoreNode((int64)QStyle::SC_TitleBarUnshadeButton));
   qstyle_ns->addConstant("SC_TitleBarContextHelpButton", new QoreNode((int64)QStyle::SC_TitleBarContextHelpButton));
   qstyle_ns->addConstant("SC_TitleBarLabel",         new QoreNode((int64)QStyle::SC_TitleBarLabel));
   qstyle_ns->addConstant("SC_Q3ListView",            new QoreNode((int64)QStyle::SC_Q3ListView));
   qstyle_ns->addConstant("SC_Q3ListViewBranch",      new QoreNode((int64)QStyle::SC_Q3ListViewBranch));
   qstyle_ns->addConstant("SC_Q3ListViewExpand",      new QoreNode((int64)QStyle::SC_Q3ListViewExpand));
   qstyle_ns->addConstant("SC_DialGroove",            new QoreNode((int64)QStyle::SC_DialGroove));
   qstyle_ns->addConstant("SC_DialHandle",            new QoreNode((int64)QStyle::SC_DialHandle));
   qstyle_ns->addConstant("SC_DialTickmarks",         new QoreNode((int64)QStyle::SC_DialTickmarks));
   qstyle_ns->addConstant("SC_GroupBoxCheckBox",      new QoreNode((int64)QStyle::SC_GroupBoxCheckBox));
   qstyle_ns->addConstant("SC_GroupBoxLabel",         new QoreNode((int64)QStyle::SC_GroupBoxLabel));
   qstyle_ns->addConstant("SC_GroupBoxContents",      new QoreNode((int64)QStyle::SC_GroupBoxContents));
   qstyle_ns->addConstant("SC_GroupBoxFrame",         new QoreNode((int64)QStyle::SC_GroupBoxFrame));
   qstyle_ns->addConstant("SC_MdiMinButton",          new QoreNode((int64)QStyle::SC_MdiMinButton));
   qstyle_ns->addConstant("SC_MdiNormalButton",       new QoreNode((int64)QStyle::SC_MdiNormalButton));
   qstyle_ns->addConstant("SC_MdiCloseButton",        new QoreNode((int64)QStyle::SC_MdiCloseButton));
   qstyle_ns->addConstant("SC_All",                   new QoreNode((int64)QStyle::SC_All));

   // ContentsType enum
   qstyle_ns->addConstant("CT_PushButton",            new QoreNode((int64)QStyle::CT_PushButton));
   qstyle_ns->addConstant("CT_CheckBox",              new QoreNode((int64)QStyle::CT_CheckBox));
   qstyle_ns->addConstant("CT_RadioButton",           new QoreNode((int64)QStyle::CT_RadioButton));
   qstyle_ns->addConstant("CT_ToolButton",            new QoreNode((int64)QStyle::CT_ToolButton));
   qstyle_ns->addConstant("CT_ComboBox",              new QoreNode((int64)QStyle::CT_ComboBox));
   qstyle_ns->addConstant("CT_Splitter",              new QoreNode((int64)QStyle::CT_Splitter));
   qstyle_ns->addConstant("CT_Q3DockWindow",          new QoreNode((int64)QStyle::CT_Q3DockWindow));
   qstyle_ns->addConstant("CT_ProgressBar",           new QoreNode((int64)QStyle::CT_ProgressBar));
   qstyle_ns->addConstant("CT_MenuItem",              new QoreNode((int64)QStyle::CT_MenuItem));
   qstyle_ns->addConstant("CT_MenuBarItem",           new QoreNode((int64)QStyle::CT_MenuBarItem));
   qstyle_ns->addConstant("CT_MenuBar",               new QoreNode((int64)QStyle::CT_MenuBar));
   qstyle_ns->addConstant("CT_Menu",                  new QoreNode((int64)QStyle::CT_Menu));
   qstyle_ns->addConstant("CT_TabBarTab",             new QoreNode((int64)QStyle::CT_TabBarTab));
   qstyle_ns->addConstant("CT_Slider",                new QoreNode((int64)QStyle::CT_Slider));
   qstyle_ns->addConstant("CT_ScrollBar",             new QoreNode((int64)QStyle::CT_ScrollBar));
   qstyle_ns->addConstant("CT_Q3Header",              new QoreNode((int64)QStyle::CT_Q3Header));
   qstyle_ns->addConstant("CT_LineEdit",              new QoreNode((int64)QStyle::CT_LineEdit));
   qstyle_ns->addConstant("CT_SpinBox",               new QoreNode((int64)QStyle::CT_SpinBox));
   qstyle_ns->addConstant("CT_SizeGrip",              new QoreNode((int64)QStyle::CT_SizeGrip));
   qstyle_ns->addConstant("CT_TabWidget",             new QoreNode((int64)QStyle::CT_TabWidget));
   qstyle_ns->addConstant("CT_DialogButtons",         new QoreNode((int64)QStyle::CT_DialogButtons));
   qstyle_ns->addConstant("CT_HeaderSection",         new QoreNode((int64)QStyle::CT_HeaderSection));
   qstyle_ns->addConstant("CT_GroupBox",              new QoreNode((int64)QStyle::CT_GroupBox));
   qstyle_ns->addConstant("CT_MdiControls",           new QoreNode((int64)QStyle::CT_MdiControls));
   qstyle_ns->addConstant("CT_CustomBase",            new QoreNode((int64)QStyle::CT_CustomBase));

   // StyleHint enum
   qstyle_ns->addConstant("SH_EtchDisabledText",      new QoreNode((int64)QStyle::SH_EtchDisabledText));
   qstyle_ns->addConstant("SH_DitherDisabledText",    new QoreNode((int64)QStyle::SH_DitherDisabledText));
   qstyle_ns->addConstant("SH_ScrollBar_MiddleClickAbsolutePosition", new QoreNode((int64)QStyle::SH_ScrollBar_MiddleClickAbsolutePosition));
   qstyle_ns->addConstant("SH_ScrollBar_ScrollWhenPointerLeavesControl", new QoreNode((int64)QStyle::SH_ScrollBar_ScrollWhenPointerLeavesControl));
   qstyle_ns->addConstant("SH_TabBar_SelectMouseType", new QoreNode((int64)QStyle::SH_TabBar_SelectMouseType));
   qstyle_ns->addConstant("SH_TabBar_Alignment",      new QoreNode((int64)QStyle::SH_TabBar_Alignment));
   qstyle_ns->addConstant("SH_Header_ArrowAlignment", new QoreNode((int64)QStyle::SH_Header_ArrowAlignment));
   qstyle_ns->addConstant("SH_Slider_SnapToValue",    new QoreNode((int64)QStyle::SH_Slider_SnapToValue));
   qstyle_ns->addConstant("SH_Slider_SloppyKeyEvents", new QoreNode((int64)QStyle::SH_Slider_SloppyKeyEvents));
   qstyle_ns->addConstant("SH_ProgressDialog_CenterCancelButton", new QoreNode((int64)QStyle::SH_ProgressDialog_CenterCancelButton));
   qstyle_ns->addConstant("SH_ProgressDialog_TextLabelAlignment", new QoreNode((int64)QStyle::SH_ProgressDialog_TextLabelAlignment));
   qstyle_ns->addConstant("SH_PrintDialog_RightAlignButtons", new QoreNode((int64)QStyle::SH_PrintDialog_RightAlignButtons));
   qstyle_ns->addConstant("SH_MainWindow_SpaceBelowMenuBar", new QoreNode((int64)QStyle::SH_MainWindow_SpaceBelowMenuBar));
   qstyle_ns->addConstant("SH_FontDialog_SelectAssociatedText", new QoreNode((int64)QStyle::SH_FontDialog_SelectAssociatedText));
   qstyle_ns->addConstant("SH_Menu_AllowActiveAndDisabled", new QoreNode((int64)QStyle::SH_Menu_AllowActiveAndDisabled));
   qstyle_ns->addConstant("SH_Menu_SpaceActivatesItem", new QoreNode((int64)QStyle::SH_Menu_SpaceActivatesItem));
   qstyle_ns->addConstant("SH_Menu_SubMenuPopupDelay", new QoreNode((int64)QStyle::SH_Menu_SubMenuPopupDelay));
   qstyle_ns->addConstant("SH_ScrollView_FrameOnlyAroundContents", new QoreNode((int64)QStyle::SH_ScrollView_FrameOnlyAroundContents));
   qstyle_ns->addConstant("SH_MenuBar_AltKeyNavigation", new QoreNode((int64)QStyle::SH_MenuBar_AltKeyNavigation));
   qstyle_ns->addConstant("SH_ComboBox_ListMouseTracking", new QoreNode((int64)QStyle::SH_ComboBox_ListMouseTracking));
   qstyle_ns->addConstant("SH_Menu_MouseTracking",    new QoreNode((int64)QStyle::SH_Menu_MouseTracking));
   qstyle_ns->addConstant("SH_MenuBar_MouseTracking", new QoreNode((int64)QStyle::SH_MenuBar_MouseTracking));
   qstyle_ns->addConstant("SH_ItemView_ChangeHighlightOnFocus", new QoreNode((int64)QStyle::SH_ItemView_ChangeHighlightOnFocus));
   qstyle_ns->addConstant("SH_Widget_ShareActivation", new QoreNode((int64)QStyle::SH_Widget_ShareActivation));
   qstyle_ns->addConstant("SH_Workspace_FillSpaceOnMaximize", new QoreNode((int64)QStyle::SH_Workspace_FillSpaceOnMaximize));
   qstyle_ns->addConstant("SH_ComboBox_Popup",        new QoreNode((int64)QStyle::SH_ComboBox_Popup));
   qstyle_ns->addConstant("SH_TitleBar_NoBorder",     new QoreNode((int64)QStyle::SH_TitleBar_NoBorder));
   qstyle_ns->addConstant("SH_Slider_StopMouseOverSlider", new QoreNode((int64)QStyle::SH_Slider_StopMouseOverSlider));
   qstyle_ns->addConstant("SH_ScrollBar_StopMouseOverSlider", new QoreNode((int64)QStyle::SH_ScrollBar_StopMouseOverSlider));
   qstyle_ns->addConstant("SH_BlinkCursorWhenTextSelected", new QoreNode((int64)QStyle::SH_BlinkCursorWhenTextSelected));
   qstyle_ns->addConstant("SH_RichText_FullWidthSelection", new QoreNode((int64)QStyle::SH_RichText_FullWidthSelection));
   qstyle_ns->addConstant("SH_Menu_Scrollable",       new QoreNode((int64)QStyle::SH_Menu_Scrollable));
   qstyle_ns->addConstant("SH_GroupBox_TextLabelVerticalAlignment", new QoreNode((int64)QStyle::SH_GroupBox_TextLabelVerticalAlignment));
   qstyle_ns->addConstant("SH_GroupBox_TextLabelColor", new QoreNode((int64)QStyle::SH_GroupBox_TextLabelColor));
   qstyle_ns->addConstant("SH_Menu_SloppySubMenus",   new QoreNode((int64)QStyle::SH_Menu_SloppySubMenus));
   qstyle_ns->addConstant("SH_Table_GridLineColor",   new QoreNode((int64)QStyle::SH_Table_GridLineColor));
   qstyle_ns->addConstant("SH_LineEdit_PasswordCharacter", new QoreNode((int64)QStyle::SH_LineEdit_PasswordCharacter));
   qstyle_ns->addConstant("SH_DialogButtons_DefaultButton", new QoreNode((int64)QStyle::SH_DialogButtons_DefaultButton));
   qstyle_ns->addConstant("SH_ToolBox_SelectedPageTitleBold", new QoreNode((int64)QStyle::SH_ToolBox_SelectedPageTitleBold));
   qstyle_ns->addConstant("SH_TabBar_PreferNoArrows", new QoreNode((int64)QStyle::SH_TabBar_PreferNoArrows));
   qstyle_ns->addConstant("SH_ScrollBar_LeftClickAbsolutePosition", new QoreNode((int64)QStyle::SH_ScrollBar_LeftClickAbsolutePosition));
   qstyle_ns->addConstant("SH_Q3ListViewExpand_SelectMouseType", new QoreNode((int64)QStyle::SH_Q3ListViewExpand_SelectMouseType));
   qstyle_ns->addConstant("SH_UnderlineShortcut",     new QoreNode((int64)QStyle::SH_UnderlineShortcut));
   qstyle_ns->addConstant("SH_SpinBox_AnimateButton", new QoreNode((int64)QStyle::SH_SpinBox_AnimateButton));
   qstyle_ns->addConstant("SH_SpinBox_KeyPressAutoRepeatRate", new QoreNode((int64)QStyle::SH_SpinBox_KeyPressAutoRepeatRate));
   qstyle_ns->addConstant("SH_SpinBox_ClickAutoRepeatRate", new QoreNode((int64)QStyle::SH_SpinBox_ClickAutoRepeatRate));
   qstyle_ns->addConstant("SH_Menu_FillScreenWithScroll", new QoreNode((int64)QStyle::SH_Menu_FillScreenWithScroll));
   qstyle_ns->addConstant("SH_ToolTipLabel_Opacity",  new QoreNode((int64)QStyle::SH_ToolTipLabel_Opacity));
   qstyle_ns->addConstant("SH_DrawMenuBarSeparator",  new QoreNode((int64)QStyle::SH_DrawMenuBarSeparator));
   qstyle_ns->addConstant("SH_TitleBar_ModifyNotification", new QoreNode((int64)QStyle::SH_TitleBar_ModifyNotification));
   qstyle_ns->addConstant("SH_Button_FocusPolicy",    new QoreNode((int64)QStyle::SH_Button_FocusPolicy));
   qstyle_ns->addConstant("SH_MenuBar_DismissOnSecondClick", new QoreNode((int64)QStyle::SH_MenuBar_DismissOnSecondClick));
   qstyle_ns->addConstant("SH_MessageBox_UseBorderForButtonSpacing", new QoreNode((int64)QStyle::SH_MessageBox_UseBorderForButtonSpacing));
   qstyle_ns->addConstant("SH_TitleBar_AutoRaise",    new QoreNode((int64)QStyle::SH_TitleBar_AutoRaise));
   qstyle_ns->addConstant("SH_ToolButton_PopupDelay", new QoreNode((int64)QStyle::SH_ToolButton_PopupDelay));
   qstyle_ns->addConstant("SH_FocusFrame_Mask",       new QoreNode((int64)QStyle::SH_FocusFrame_Mask));
   qstyle_ns->addConstant("SH_RubberBand_Mask",       new QoreNode((int64)QStyle::SH_RubberBand_Mask));
   qstyle_ns->addConstant("SH_WindowFrame_Mask",      new QoreNode((int64)QStyle::SH_WindowFrame_Mask));
   qstyle_ns->addConstant("SH_SpinControls_DisableOnBounds", new QoreNode((int64)QStyle::SH_SpinControls_DisableOnBounds));
   qstyle_ns->addConstant("SH_Dial_BackgroundRole",   new QoreNode((int64)QStyle::SH_Dial_BackgroundRole));
   qstyle_ns->addConstant("SH_ComboBox_LayoutDirection", new QoreNode((int64)QStyle::SH_ComboBox_LayoutDirection));
   qstyle_ns->addConstant("SH_ItemView_EllipsisLocation", new QoreNode((int64)QStyle::SH_ItemView_EllipsisLocation));
   qstyle_ns->addConstant("SH_ItemView_ShowDecorationSelected", new QoreNode((int64)QStyle::SH_ItemView_ShowDecorationSelected));
   qstyle_ns->addConstant("SH_ItemView_ActivateItemOnSingleClick", new QoreNode((int64)QStyle::SH_ItemView_ActivateItemOnSingleClick));
   qstyle_ns->addConstant("SH_ScrollBar_ContextMenu", new QoreNode((int64)QStyle::SH_ScrollBar_ContextMenu));
   qstyle_ns->addConstant("SH_ScrollBar_RollBetweenButtons", new QoreNode((int64)QStyle::SH_ScrollBar_RollBetweenButtons));
   qstyle_ns->addConstant("SH_Slider_AbsoluteSetButtons", new QoreNode((int64)QStyle::SH_Slider_AbsoluteSetButtons));
   qstyle_ns->addConstant("SH_Slider_PageSetButtons", new QoreNode((int64)QStyle::SH_Slider_PageSetButtons));
   qstyle_ns->addConstant("SH_Menu_KeyboardSearch",   new QoreNode((int64)QStyle::SH_Menu_KeyboardSearch));
   qstyle_ns->addConstant("SH_TabBar_ElideMode",      new QoreNode((int64)QStyle::SH_TabBar_ElideMode));
   qstyle_ns->addConstant("SH_DialogButtonLayout",    new QoreNode((int64)QStyle::SH_DialogButtonLayout));
   qstyle_ns->addConstant("SH_ComboBox_PopupFrameStyle", new QoreNode((int64)QStyle::SH_ComboBox_PopupFrameStyle));
   qstyle_ns->addConstant("SH_MessageBox_TextInteractionFlags", new QoreNode((int64)QStyle::SH_MessageBox_TextInteractionFlags));
   qstyle_ns->addConstant("SH_DialogButtonBox_ButtonsHaveIcons", new QoreNode((int64)QStyle::SH_DialogButtonBox_ButtonsHaveIcons));
   qstyle_ns->addConstant("SH_SpellCheckUnderlineStyle", new QoreNode((int64)QStyle::SH_SpellCheckUnderlineStyle));
   qstyle_ns->addConstant("SH_MessageBox_CenterButtons", new QoreNode((int64)QStyle::SH_MessageBox_CenterButtons));
   qstyle_ns->addConstant("SH_Menu_SelectionWrap",    new QoreNode((int64)QStyle::SH_Menu_SelectionWrap));
   qstyle_ns->addConstant("SH_ItemView_MovementWithoutUpdatingSelection", new QoreNode((int64)QStyle::SH_ItemView_MovementWithoutUpdatingSelection));
   qstyle_ns->addConstant("SH_ToolTip_Mask",          new QoreNode((int64)QStyle::SH_ToolTip_Mask));
   qstyle_ns->addConstant("SH_FocusFrame_AboveWidget", new QoreNode((int64)QStyle::SH_FocusFrame_AboveWidget));
   qstyle_ns->addConstant("SH_TextControl_FocusIndicatorTextCharFormat", new QoreNode((int64)QStyle::SH_TextControl_FocusIndicatorTextCharFormat));
   qstyle_ns->addConstant("SH_WizardStyle",           new QoreNode((int64)QStyle::SH_WizardStyle));
   qstyle_ns->addConstant("SH_ItemView_ArrowKeysNavigateIntoChildren", new QoreNode((int64)QStyle::SH_ItemView_ArrowKeysNavigateIntoChildren));
   qstyle_ns->addConstant("SH_CustomBase",            new QoreNode((int64)QStyle::SH_CustomBase));

   // StandardPixmap enum
   qstyle_ns->addConstant("SP_TitleBarMenuButton",    new QoreNode((int64)QStyle::SP_TitleBarMenuButton));
   qstyle_ns->addConstant("SP_TitleBarMinButton",     new QoreNode((int64)QStyle::SP_TitleBarMinButton));
   qstyle_ns->addConstant("SP_TitleBarMaxButton",     new QoreNode((int64)QStyle::SP_TitleBarMaxButton));
   qstyle_ns->addConstant("SP_TitleBarCloseButton",   new QoreNode((int64)QStyle::SP_TitleBarCloseButton));
   qstyle_ns->addConstant("SP_TitleBarNormalButton",  new QoreNode((int64)QStyle::SP_TitleBarNormalButton));
   qstyle_ns->addConstant("SP_TitleBarShadeButton",   new QoreNode((int64)QStyle::SP_TitleBarShadeButton));
   qstyle_ns->addConstant("SP_TitleBarUnshadeButton", new QoreNode((int64)QStyle::SP_TitleBarUnshadeButton));
   qstyle_ns->addConstant("SP_TitleBarContextHelpButton", new QoreNode((int64)QStyle::SP_TitleBarContextHelpButton));
   qstyle_ns->addConstant("SP_DockWidgetCloseButton", new QoreNode((int64)QStyle::SP_DockWidgetCloseButton));
   qstyle_ns->addConstant("SP_MessageBoxInformation", new QoreNode((int64)QStyle::SP_MessageBoxInformation));
   qstyle_ns->addConstant("SP_MessageBoxWarning",     new QoreNode((int64)QStyle::SP_MessageBoxWarning));
   qstyle_ns->addConstant("SP_MessageBoxCritical",    new QoreNode((int64)QStyle::SP_MessageBoxCritical));
   qstyle_ns->addConstant("SP_MessageBoxQuestion",    new QoreNode((int64)QStyle::SP_MessageBoxQuestion));
   qstyle_ns->addConstant("SP_DesktopIcon",           new QoreNode((int64)QStyle::SP_DesktopIcon));
   qstyle_ns->addConstant("SP_TrashIcon",             new QoreNode((int64)QStyle::SP_TrashIcon));
   qstyle_ns->addConstant("SP_ComputerIcon",          new QoreNode((int64)QStyle::SP_ComputerIcon));
   qstyle_ns->addConstant("SP_DriveFDIcon",           new QoreNode((int64)QStyle::SP_DriveFDIcon));
   qstyle_ns->addConstant("SP_DriveHDIcon",           new QoreNode((int64)QStyle::SP_DriveHDIcon));
   qstyle_ns->addConstant("SP_DriveCDIcon",           new QoreNode((int64)QStyle::SP_DriveCDIcon));
   qstyle_ns->addConstant("SP_DriveDVDIcon",          new QoreNode((int64)QStyle::SP_DriveDVDIcon));
   qstyle_ns->addConstant("SP_DriveNetIcon",          new QoreNode((int64)QStyle::SP_DriveNetIcon));
   qstyle_ns->addConstant("SP_DirOpenIcon",           new QoreNode((int64)QStyle::SP_DirOpenIcon));
   qstyle_ns->addConstant("SP_DirClosedIcon",         new QoreNode((int64)QStyle::SP_DirClosedIcon));
   qstyle_ns->addConstant("SP_DirLinkIcon",           new QoreNode((int64)QStyle::SP_DirLinkIcon));
   qstyle_ns->addConstant("SP_FileIcon",              new QoreNode((int64)QStyle::SP_FileIcon));
   qstyle_ns->addConstant("SP_FileLinkIcon",          new QoreNode((int64)QStyle::SP_FileLinkIcon));
   qstyle_ns->addConstant("SP_ToolBarHorizontalExtensionButton", new QoreNode((int64)QStyle::SP_ToolBarHorizontalExtensionButton));
   qstyle_ns->addConstant("SP_ToolBarVerticalExtensionButton", new QoreNode((int64)QStyle::SP_ToolBarVerticalExtensionButton));
   qstyle_ns->addConstant("SP_FileDialogStart",       new QoreNode((int64)QStyle::SP_FileDialogStart));
   qstyle_ns->addConstant("SP_FileDialogEnd",         new QoreNode((int64)QStyle::SP_FileDialogEnd));
   qstyle_ns->addConstant("SP_FileDialogToParent",    new QoreNode((int64)QStyle::SP_FileDialogToParent));
   qstyle_ns->addConstant("SP_FileDialogNewFolder",   new QoreNode((int64)QStyle::SP_FileDialogNewFolder));
   qstyle_ns->addConstant("SP_FileDialogDetailedView", new QoreNode((int64)QStyle::SP_FileDialogDetailedView));
   qstyle_ns->addConstant("SP_FileDialogInfoView",    new QoreNode((int64)QStyle::SP_FileDialogInfoView));
   qstyle_ns->addConstant("SP_FileDialogContentsView", new QoreNode((int64)QStyle::SP_FileDialogContentsView));
   qstyle_ns->addConstant("SP_FileDialogListView",    new QoreNode((int64)QStyle::SP_FileDialogListView));
   qstyle_ns->addConstant("SP_FileDialogBack",        new QoreNode((int64)QStyle::SP_FileDialogBack));
   qstyle_ns->addConstant("SP_DirIcon",               new QoreNode((int64)QStyle::SP_DirIcon));
   qstyle_ns->addConstant("SP_DialogOkButton",        new QoreNode((int64)QStyle::SP_DialogOkButton));
   qstyle_ns->addConstant("SP_DialogCancelButton",    new QoreNode((int64)QStyle::SP_DialogCancelButton));
   qstyle_ns->addConstant("SP_DialogHelpButton",      new QoreNode((int64)QStyle::SP_DialogHelpButton));
   qstyle_ns->addConstant("SP_DialogOpenButton",      new QoreNode((int64)QStyle::SP_DialogOpenButton));
   qstyle_ns->addConstant("SP_DialogSaveButton",      new QoreNode((int64)QStyle::SP_DialogSaveButton));
   qstyle_ns->addConstant("SP_DialogCloseButton",     new QoreNode((int64)QStyle::SP_DialogCloseButton));
   qstyle_ns->addConstant("SP_DialogApplyButton",     new QoreNode((int64)QStyle::SP_DialogApplyButton));
   qstyle_ns->addConstant("SP_DialogResetButton",     new QoreNode((int64)QStyle::SP_DialogResetButton));
   qstyle_ns->addConstant("SP_DialogDiscardButton",   new QoreNode((int64)QStyle::SP_DialogDiscardButton));
   qstyle_ns->addConstant("SP_DialogYesButton",       new QoreNode((int64)QStyle::SP_DialogYesButton));
   qstyle_ns->addConstant("SP_DialogNoButton",        new QoreNode((int64)QStyle::SP_DialogNoButton));
   qstyle_ns->addConstant("SP_ArrowUp",               new QoreNode((int64)QStyle::SP_ArrowUp));
   qstyle_ns->addConstant("SP_ArrowDown",             new QoreNode((int64)QStyle::SP_ArrowDown));
   qstyle_ns->addConstant("SP_ArrowLeft",             new QoreNode((int64)QStyle::SP_ArrowLeft));
   qstyle_ns->addConstant("SP_ArrowRight",            new QoreNode((int64)QStyle::SP_ArrowRight));
   qstyle_ns->addConstant("SP_ArrowBack",             new QoreNode((int64)QStyle::SP_ArrowBack));
   qstyle_ns->addConstant("SP_ArrowForward",          new QoreNode((int64)QStyle::SP_ArrowForward));
   qstyle_ns->addConstant("SP_DirHomeIcon",           new QoreNode((int64)QStyle::SP_DirHomeIcon));
   qstyle_ns->addConstant("SP_CommandLink",           new QoreNode((int64)QStyle::SP_CommandLink));
   qstyle_ns->addConstant("SP_VistaShield",           new QoreNode((int64)QStyle::SP_VistaShield));
   qstyle_ns->addConstant("SP_CustomBase",            new QoreNode((int64)QStyle::SP_CustomBase));


   return qstyle_ns;
}
