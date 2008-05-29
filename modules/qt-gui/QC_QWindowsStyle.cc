/*
 QC_QWindowsStyle.cc
 
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

#include "QC_QWindowsStyle.h"
#include "QC_QStyleOption.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

qore_classid_t CID_QWINDOWSSTYLE;
class QoreClass *QC_QWindowsStyle = 0;

//QWindowsStyle ()
static void QWINDOWSSTYLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QWINDOWSSTYLE, new QoreQWindowsStyle(self));
   return;
}

static void QWINDOWSSTYLE_copy(class QoreObject *self, class QoreObject *old, class QoreQWindowsStyle *qws, ExceptionSink *xsink)
{
   xsink->raiseException("QWINDOWSSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_drawComplexControl(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_drawControl(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::ControlElement element = (QWindowsStyle::ControlElement)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->drawControl(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_drawPrimitive(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::PrimitiveElement element = (QWindowsStyle::PrimitiveElement)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQPainter *painter = o ? (QoreQPainter *)o->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->drawPrimitive(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_generatedIconPixmap(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQPixmap *pixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QWindowsStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   o = test_object_param(params, 2);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QStyleOption object as third argument to QWindowsStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   QoreObject *o_qp = new QoreObject(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qws->generatedIconPixmap(iconMode, *(static_cast<QPixmap *>(pixmap)), static_cast<QStyleOption *>(option)));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return o_qp;
}

//virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_hitTestComplexControl(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQPoint *position = o ? (QoreQPoint *)o->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QWindowsStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreBigIntNode(qws->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_pixelMetric(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::PixelMetric metric = (QWindowsStyle::PixelMetric)(p ? p->getAsInt() : 0);
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
   return new QoreBigIntNode(qws->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_sizeFromContents(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::ContentsType type = (QWindowsStyle::ContentsType)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQSize *contentsSize = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QWindowsStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qws->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_styleHint(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::StyleHint hint = (QWindowsStyle::StyleHint)(p ? p->getAsInt() : 0);
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
//   o = test_object_param(params, 3);
//   ??? QStyleHintReturn* returnData = p;
   return new QoreBigIntNode(qws->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_subControlRect(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOptionComplex *option = o ? (QoreQStyleOptionComplex *)o->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QWindowsStyle::SubControl subControl = (QWindowsStyle::SubControl)(p ? p->getAsInt() : 0);
   o = test_object_param(params, 3);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qws->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static AbstractQoreNode *QWINDOWSSTYLE_subElementRect(QoreObject *self, QoreAbstractQWindowsStyle *qws, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QWindowsStyle::SubElement element = (QWindowsStyle::SubElement)(p ? p->getAsInt() : 0);
   const QoreObject *o = test_object_param(params, 1);
   QoreQStyleOption *option = o ? (QoreQStyleOption *)o->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   o = test_object_param(params, 2);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qws->subElementRect(element, static_cast<QStyleOption *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

QoreClass *initQWindowsStyleClass(QoreClass *qstyle)
{
   QC_QWindowsStyle = new QoreClass("QWindowsStyle", QDOM_GUI);
   CID_QWINDOWSSTYLE = QC_QWindowsStyle->getID();

   QC_QWindowsStyle->addBuiltinVirtualBaseClass(qstyle);

   QC_QWindowsStyle->setConstructor(QWINDOWSSTYLE_constructor);
   QC_QWindowsStyle->setCopy((q_copy_t)QWINDOWSSTYLE_copy);

   QC_QWindowsStyle->addMethod("drawComplexControl",          (q_method_t)QWINDOWSSTYLE_drawComplexControl);
   QC_QWindowsStyle->addMethod("drawControl",                 (q_method_t)QWINDOWSSTYLE_drawControl);
   QC_QWindowsStyle->addMethod("drawPrimitive",               (q_method_t)QWINDOWSSTYLE_drawPrimitive);
   QC_QWindowsStyle->addMethod("generatedIconPixmap",         (q_method_t)QWINDOWSSTYLE_generatedIconPixmap);
   QC_QWindowsStyle->addMethod("hitTestComplexControl",       (q_method_t)QWINDOWSSTYLE_hitTestComplexControl);
   QC_QWindowsStyle->addMethod("pixelMetric",                 (q_method_t)QWINDOWSSTYLE_pixelMetric);
   QC_QWindowsStyle->addMethod("sizeFromContents",            (q_method_t)QWINDOWSSTYLE_sizeFromContents);
   QC_QWindowsStyle->addMethod("styleHint",                   (q_method_t)QWINDOWSSTYLE_styleHint);
   QC_QWindowsStyle->addMethod("subControlRect",              (q_method_t)QWINDOWSSTYLE_subControlRect);
   QC_QWindowsStyle->addMethod("subElementRect",              (q_method_t)QWINDOWSSTYLE_subElementRect);

   return QC_QWindowsStyle;
}
