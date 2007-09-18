/*
 QC_QWindowsStyle.cc
 
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

#include "QC_QWindowsStyle.h"

int CID_QWINDOWSSTYLE;
class QoreClass *QC_QWindowsStyle = 0;

//QWindowsStyle ()
static void QWINDOWSSTYLE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QWINDOWSSTYLE, new QoreQWindowsStyle(self));
   return;
}

static void QWINDOWSSTYLE_copy(class Object *self, class Object *old, class QoreQWindowsStyle *qws, ExceptionSink *xsink)
{
   xsink->raiseException("QWINDOWSSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_drawComplexControl(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->getQWindowsStyle()->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), static_cast<QPainter *>(painter), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_drawControl(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::ControlElement element = (QWindowsStyle::ControlElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->getQWindowsStyle()->drawControl(element, static_cast<QStyleOption *>(option), static_cast<QPainter *>(painter), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_drawPrimitive(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::PrimitiveElement element = (QWindowsStyle::PrimitiveElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QWindowsStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qws->getQWindowsStyle()->drawPrimitive(element, static_cast<QStyleOption *>(option), static_cast<QPainter *>(painter), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static QoreNode *QWINDOWSSTYLE_generatedIconPixmap(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QWindowsStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QStyleOption object as third argument to QWindowsStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qws->getQWindowsStyle()->generatedIconPixmap(iconMode, *(static_cast<QPixmap *>(pixmap)), static_cast<QStyleOption *>(option)));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_hitTestComplexControl(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QWindowsStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qws->getQWindowsStyle()->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_pixelMetric(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::PixelMetric metric = (QWindowsStyle::PixelMetric)(p ? p->getAsInt() : 0);
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
   return new QoreNode((int64)qws->getQWindowsStyle()->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_sizeFromContents(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::ContentsType type = (QWindowsStyle::ContentsType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQSize *contentsSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QWindowsStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qws->getQWindowsStyle()->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_styleHint(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::StyleHint hint = (QWindowsStyle::StyleHint)(p ? p->getAsInt() : 0);
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
   return new QoreNode((int64)qws->getQWindowsStyle()->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_subControlRect(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::ComplexControl control = (QWindowsStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QWindowsStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QWindowsStyle::SubControl subControl = (QWindowsStyle::SubControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qws->getQWindowsStyle()->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static QoreNode *QWINDOWSSTYLE_subElementRect(Object *self, QoreAbstractQWindowsStyle *qws, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QWindowsStyle::SubElement element = (QWindowsStyle::SubElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QWINDOWSSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QWindowsStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qws->getQWindowsStyle()->subElementRect(element, static_cast<QStyleOption *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
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
