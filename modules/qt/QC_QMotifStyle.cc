/*
 QC_QMotifStyle.cc
 
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

#include "QC_QMotifStyle.h"

int CID_QMOTIFSTYLE;
class QoreClass *QC_QMotifStyle = 0;

//QMotifStyle ( bool useHighlightCols = false )
static void QMOTIFSTYLE_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool useHighlightCols = p ? p->getAsBool() : false;
   self->setPrivate(CID_QMOTIFSTYLE, new QoreQMotifStyle(self, useHighlightCols));
   return;
}

static void QMOTIFSTYLE_copy(class Object *self, class Object *old, class QoreQMotifStyle *qms, ExceptionSink *xsink)
{
   xsink->raiseException("QMOTIFSTYLE-COPY-ERROR", "objects of this class cannot be copied");
}

//void setUseHighlightColors ( bool arg )
static QoreNode *QMOTIFSTYLE_setUseHighlightColors(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool arg = p ? p->getAsBool() : false;
   qms->getQMotifStyle()->setUseHighlightColors(arg);
   return 0;
}

//bool useHighlightColors () const
static QoreNode *QMOTIFSTYLE_useHighlightColors(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qms->getQMotifStyle()->useHighlightColors());
}

//virtual void drawComplexControl ( ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_drawComplexControl(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::ComplexControl control = (QMotifStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QMotifStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QMotifStyle::drawComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qms->drawComplexControl(control, static_cast<QStyleOptionComplex *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawControl ( ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_drawControl(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::ControlElement element = (QMotifStyle::ControlElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QStyleOption object as second argument to QMotifStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWCONTROL-PARAM-ERROR", "expecting a QPainter object as third argument to QMotifStyle::drawControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qms->drawControl(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual void drawPrimitive ( PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_drawPrimitive(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::PrimitiveElement element = (QMotifStyle::PrimitiveElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QStyleOption object as second argument to QMotifStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPainter *painter = (p && p->type == NT_OBJECT) ? (QoreQPainter *)p->val.object->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-DRAWPRIMITIVE-PARAM-ERROR", "expecting a QPainter object as third argument to QMotifStyle::drawPrimitive()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   qms->drawPrimitive(element, static_cast<QStyleOption *>(option), painter->getQPainter(), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0);
   return 0;
}

//virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0
static QoreNode *QMOTIFSTYLE_generatedIconPixmap(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QIcon::Mode iconMode = (QIcon::Mode)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->type == NT_OBJECT) ? (QoreQPixmap *)p->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QPixmap object as second argument to QMotifStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-GENERATEDICONPIXMAP-PARAM-ERROR", "expecting a QStyleOption object as third argument to QMotifStyle::generatedIconPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   Object *o_qp = new Object(QC_QPixmap, getProgram());
   QoreQPixmap *q_qp = new QoreQPixmap(qms->generatedIconPixmap(iconMode, *(static_cast<QPixmap *>(pixmap)), static_cast<QStyleOption *>(option)));
   o_qp->setPrivate(CID_QPIXMAP, q_qp);
   return new QoreNode(o_qp);
}

//virtual SubControl hitTestComplexControl ( ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_hitTestComplexControl(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::ComplexControl control = (QMotifStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QMotifStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQPoint *position = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!position) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-HITTESTCOMPLEXCONTROL-PARAM-ERROR", "expecting a QPoint object as third argument to QMotifStyle::hitTestComplexControl()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> positionHolder(static_cast<AbstractPrivateData *>(position), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return new QoreNode((int64)qms->hitTestComplexControl(control, static_cast<QStyleOptionComplex *>(option), *(static_cast<QPoint *>(position)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_pixelMetric(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::PixelMetric metric = (QMotifStyle::PixelMetric)(p ? p->getAsInt() : 0);
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
   return new QoreNode((int64)qms->pixelMetric(metric, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QSize sizeFromContents ( ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_sizeFromContents(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::ContentsType type = (QMotifStyle::ContentsType)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QStyleOption object as second argument to QMotifStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQSize *contentsSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!contentsSize) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-SIZEFROMCONTENTS-PARAM-ERROR", "expecting a QSize object as third argument to QMotifStyle::sizeFromContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> contentsSizeHolder(static_cast<AbstractPrivateData *>(contentsSize), xsink);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qms->sizeFromContents(type, static_cast<QStyleOption *>(option), *(static_cast<QSize *>(contentsSize)), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//virtual int styleHint ( StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_styleHint(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::StyleHint hint = (QMotifStyle::StyleHint)(p ? p->getAsInt() : 0);
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
   return new QoreNode((int64)qms->styleHint(hint, option ? static_cast<QStyleOption *>(option) : 0, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
}

//virtual QRect subControlRect ( ComplexControl control, const QStyleOptionComplex * option, SubControl subControl, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_subControlRect(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::ComplexControl control = (QMotifStyle::ComplexControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOptionComplex *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionComplex *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTIONCOMPLEX, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-SUBCONTROLRECT-PARAM-ERROR", "expecting a QStyleOptionComplex object as second argument to QMotifStyle::subControlRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QMotifStyle::SubControl subControl = (QMotifStyle::SubControl)(p ? p->getAsInt() : 0);
   p = get_param(params, 3);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qms->subControlRect(control, static_cast<QStyleOptionComplex *>(option), subControl, widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//virtual QRect subElementRect ( SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0
static QoreNode *QMOTIFSTYLE_subElementRect(Object *self, QoreAbstractQMotifStyle *qms, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMotifStyle::SubElement element = (QMotifStyle::SubElement)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQStyleOption *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOption *)p->val.object->getReferencedPrivateData(CID_QSTYLEOPTION, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMOTIFSTYLE-SUBELEMENTRECT-PARAM-ERROR", "expecting a QStyleOption object as second argument to QMotifStyle::subElementRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 2);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qms->subElementRect(element, static_cast<QStyleOption *>(option), widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

QoreClass *initQMotifStyleClass(QoreClass *qstyle)
{
   QC_QMotifStyle = new QoreClass("QMotifStyle", QDOM_GUI);
   CID_QMOTIFSTYLE = QC_QMotifStyle->getID();

   QC_QMotifStyle->addBuiltinVirtualBaseClass(qstyle);

   QC_QMotifStyle->setConstructor(QMOTIFSTYLE_constructor);
   QC_QMotifStyle->setCopy((q_copy_t)QMOTIFSTYLE_copy);

   QC_QMotifStyle->addMethod("setUseHighlightColors",       (q_method_t)QMOTIFSTYLE_setUseHighlightColors);
   QC_QMotifStyle->addMethod("useHighlightColors",          (q_method_t)QMOTIFSTYLE_useHighlightColors);
   QC_QMotifStyle->addMethod("drawComplexControl",          (q_method_t)QMOTIFSTYLE_drawComplexControl);
   QC_QMotifStyle->addMethod("drawControl",                 (q_method_t)QMOTIFSTYLE_drawControl);
   QC_QMotifStyle->addMethod("drawPrimitive",               (q_method_t)QMOTIFSTYLE_drawPrimitive);
   QC_QMotifStyle->addMethod("generatedIconPixmap",         (q_method_t)QMOTIFSTYLE_generatedIconPixmap);
   QC_QMotifStyle->addMethod("hitTestComplexControl",       (q_method_t)QMOTIFSTYLE_hitTestComplexControl);
   QC_QMotifStyle->addMethod("pixelMetric",                 (q_method_t)QMOTIFSTYLE_pixelMetric);
   QC_QMotifStyle->addMethod("sizeFromContents",            (q_method_t)QMOTIFSTYLE_sizeFromContents);
   QC_QMotifStyle->addMethod("styleHint",                   (q_method_t)QMOTIFSTYLE_styleHint);
   QC_QMotifStyle->addMethod("subControlRect",              (q_method_t)QMOTIFSTYLE_subControlRect);
   QC_QMotifStyle->addMethod("subElementRect",              (q_method_t)QMOTIFSTYLE_subElementRect);

   return QC_QMotifStyle;
}
