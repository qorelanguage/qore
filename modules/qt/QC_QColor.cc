/*
 QC_QColor.cc
 
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
#include "QC_QColor.h"

int CID_QCOLOR;
QoreClass *QC_QColor = 0;

static void QCOLOR_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QoreQColor *qc;
   if (is_nothing(p))
      qc = new QoreQColor();
   else if (p->type == NT_STRING)
      qc = new QoreQColor(p->val.String->getBuffer());
   else {
      int f = p->getAsInt();
      p = get_param(params, 1);
      if (!is_nothing(p))
      {
	 int g = p->getAsInt();
	 p = get_param(params, 2);
	 int b = p ? p->getAsInt() : 0;
	 p = get_param(params, 3);
	 int a = is_nothing(p) ? 255 : p->getAsInt();
	 qc = new QoreQColor(f, g, b, a);
      }
      else
	 qc = new QoreQColor((QRgb)f);
   }

   self->setPrivate(CID_QCOLOR, qc);
}

static void QCOLOR_copy(class Object *self, class Object *old, class QoreQColor *qf, ExceptionSink *xsink)
{
   xsink->raiseException("QCOLOR-COPY-ERROR", "objects of this class cannot be copied");
}

//int alpha () const
static QoreNode *QCOLOR_alpha(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->alpha());
}

//qreal alphaF () const
static QoreNode *QCOLOR_alphaF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->alphaF());
}

//int black () const
static QoreNode *QCOLOR_black(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->black());
}

//qreal blackF () const
static QoreNode *QCOLOR_blackF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->blackF());
}

//int blue () const
static QoreNode *QCOLOR_blue(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->blue());
}

//qreal blueF () const
static QoreNode *QCOLOR_blueF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->blueF());
}

//QColor convertTo ( Spec colorSpec ) const
static QoreNode *QCOLOR_convertTo(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   
   QColor::Spec colorSpec = (QColor::Spec)(p ? p->getAsInt() : 0);
   QoreQColor *n_qc = new QoreQColor(qc->convertTo(colorSpec));
   Object *nqc = new Object(self->getClass(CID_QCOLOR), getProgram());
   nqc->setPrivate(CID_QCOLOR, n_qc);

   return new QoreNode(nqc);
}

//int cyan () const
static QoreNode *QCOLOR_cyan(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->cyan());
}

//qreal cyanF () const
static QoreNode *QCOLOR_cyanF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->cyanF());
}

//QColor darker ( int factor = 200 ) const
//static QoreNode *QCOLOR_darker(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   int factor = !is_nothing(p) ? p->getAsInt() : 200;
//   ??? return new QoreNode((int64)qc->darker(factor));
//}

//void getCmyk ( int * c, int * m, int * y, int * k, int * a = 0 )
//static QoreNode *QCOLOR_getCmyk(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* c = p;
//   p = get_param(params, 1);
//   ??? int* m = p;
//   p = get_param(params, 2);
//   ??? int* y = p;
//   p = get_param(params, 3);
//   ??? int* k = p;
//   p = get_param(params, 4);
//   ??? int* a = p;
//   qc->getCmyk(c, m, y, k, a);
//   return 0;
//}

//void getCmykF ( qreal * c, qreal * m, qreal * y, qreal * k, qreal * a = 0 )
//static QoreNode *QCOLOR_getCmykF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? qreal* c = p;
//   p = get_param(params, 1);
//   ??? qreal* m = p;
//   p = get_param(params, 2);
//   ??? qreal* y = p;
//   p = get_param(params, 3);
//   ??? qreal* k = p;
//   p = get_param(params, 4);
//   ??? qreal* a = p;
//   qc->getCmykF(c, m, y, k, a);
//   return 0;
//}

//void getHsv ( int * h, int * s, int * v, int * a = 0 ) const
//static QoreNode *QCOLOR_getHsv(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* h = p;
//   p = get_param(params, 1);
//   ??? int* s = p;
//   p = get_param(params, 2);
//   ??? int* v = p;
//   p = get_param(params, 3);
//   ??? int* a = p;
//   qc->getHsv(h, s, v, a);
//   return 0;
//}

//void getHsvF ( qreal * h, qreal * s, qreal * v, qreal * a = 0 ) const
//static QoreNode *QCOLOR_getHsvF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? qreal* h = p;
//   p = get_param(params, 1);
//   ??? qreal* s = p;
//   p = get_param(params, 2);
//   ??? qreal* v = p;
//   p = get_param(params, 3);
//   ??? qreal* a = p;
//   qc->getHsvF(h, s, v, a);
//   return 0;
//}

//void getRgb ( int * r, int * g, int * b, int * a = 0 ) const
//static QoreNode *QCOLOR_getRgb(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* r = p;
//   p = get_param(params, 1);
//   ??? int* g = p;
//   p = get_param(params, 2);
//   ??? int* b = p;
//   p = get_param(params, 3);
//   ??? int* a = p;
//   qc->getRgb(r, g, b, a);
//   return 0;
//}

//void getRgbF ( qreal * r, qreal * g, qreal * b, qreal * a = 0 ) const
//static QoreNode *QCOLOR_getRgbF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? qreal* r = p;
//   p = get_param(params, 1);
//   ??? qreal* g = p;
//   p = get_param(params, 2);
//   ??? qreal* b = p;
//   p = get_param(params, 3);
//   ??? qreal* a = p;
//   qc->getRgbF(r, g, b, a);
//   return 0;
//}

//int green () const
static QoreNode *QCOLOR_green(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->green());
}

//qreal greenF () const
static QoreNode *QCOLOR_greenF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->greenF());
}

//int hue () const
static QoreNode *QCOLOR_hue(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->hue());
}

//qreal hueF () const
static QoreNode *QCOLOR_hueF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->hueF());
}

//bool isValid () const
static QoreNode *QCOLOR_isValid(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->isValid());
}

//int magenta () const
static QoreNode *QCOLOR_magenta(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->magenta());
}

//qreal magentaF () const
static QoreNode *QCOLOR_magentaF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->magentaF());
}

//QString name () const
static QoreNode *QCOLOR_name(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qc->name().toUtf8().data(), QCS_UTF8));
}

//int red () const
static QoreNode *QCOLOR_red(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->red());
}

//qreal redF () const
static QoreNode *QCOLOR_redF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->redF());
}

//QRgb rgb () const
static QoreNode *QCOLOR_rgb(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->rgb());
}

//QRgb rgba () const
static QoreNode *QCOLOR_rgba(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->rgba());
}

//int saturation () const
static QoreNode *QCOLOR_saturation(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->saturation());
}

//qreal saturationF () const
static QoreNode *QCOLOR_saturationF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->saturationF());
}

//void setAlpha ( int alpha )
static QoreNode *QCOLOR_setAlpha(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int alpha = p ? p->getAsInt() : 0;
   qc->setAlpha(alpha);
   return 0;
}

//void setAlphaF ( qreal alpha )
static QoreNode *QCOLOR_setAlphaF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float alpha = p ? p->getAsFloat() : 0;
   qc->setAlphaF(alpha);
   return 0;
}

//void setBlue ( int blue )
static QoreNode *QCOLOR_setBlue(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int blue = p ? p->getAsInt() : 0;
   qc->setBlue(blue);
   return 0;
}

//void setBlueF ( qreal blue )
static QoreNode *QCOLOR_setBlueF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float blue = p ? p->getAsFloat() : 0;
   qc->setBlueF(blue);
   return 0;
}

//void setCmyk ( int c, int m, int y, int k, int a = 255 )
static QoreNode *QCOLOR_setCmyk(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int c = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int m = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int y = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int k = p ? p->getAsInt() : 0;
   p = get_param(params, 4);
   int a = !is_nothing(p) ? p->getAsInt() : 255;
   qc->setCmyk(c, m, y, k, a);
   return 0;
}

//void setCmykF ( qreal c, qreal m, qreal y, qreal k, qreal a = 1.0 )
static QoreNode *QCOLOR_setCmykF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float c = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float m = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float y = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float k = p ? p->getAsFloat() : 0;
   p = get_param(params, 4);
   float a = p ? p->getAsFloat() : 0;
   qc->setCmykF(c, m, y, k, a);
   return 0;
}

//void setGreen ( int green )
static QoreNode *QCOLOR_setGreen(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int green = p ? p->getAsInt() : 0;
   qc->setGreen(green);
   return 0;
}

//void setGreenF ( qreal green )
static QoreNode *QCOLOR_setGreenF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float green = p ? p->getAsFloat() : 0;
   qc->setGreenF(green);
   return 0;
}

//void setHsv ( int h, int s, int v, int a = 255 )
static QoreNode *QCOLOR_setHsv(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int s = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int v = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int a = !is_nothing(p) ? p->getAsInt() : 255;
   qc->setHsv(h, s, v, a);
   return 0;
}

//void setHsvF ( qreal h, qreal s, qreal v, qreal a = 1.0 )
static QoreNode *QCOLOR_setHsvF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float h = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float s = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float v = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float a = p ? p->getAsFloat() : 0;
   qc->setHsvF(h, s, v, a);
   return 0;
}

//void setNamedColor ( const QString & name )
static QoreNode *QCOLOR_setNamedColor(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   qc->setNamedColor(name);
   return 0;
}

//void setRed ( int red )
static QoreNode *QCOLOR_setRed(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int red = p ? p->getAsInt() : 0;
   qc->setRed(red);
   return 0;
}

//void setRedF ( qreal red )
static QoreNode *QCOLOR_setRedF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float red = p ? p->getAsFloat() : 0;
   qc->setRedF(red);
   return 0;
}

//void setRgb ( int r, int g, int b, int a = 255 )
//void setRgb ( QRgb rgb )
static QoreNode *QCOLOR_setRgb(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int r = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   if (is_nothing(p))
      qc->setRgb((QRgb)r);
   else
   {
      int g = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int b = p ? p->getAsInt() : 0;
      p = get_param(params, 3);
      int a = !is_nothing(p) ? p->getAsInt() : 255;
      qc->setRgb(r, g, b, a);
   }
   return 0;
}

//void setRgbF ( qreal r, qreal g, qreal b, qreal a = 1.0 )
static QoreNode *QCOLOR_setRgbF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   float r = p ? p->getAsFloat() : 0;
   p = get_param(params, 1);
   float g = p ? p->getAsFloat() : 0;
   p = get_param(params, 2);
   float b = p ? p->getAsFloat() : 0;
   p = get_param(params, 3);
   float a = p ? p->getAsFloat() : 0;
   qc->setRgbF(r, g, b, a);
   return 0;
}

//void setRgba ( QRgb rgba )
static QoreNode *QCOLOR_setRgba(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int rgba = p ? p->getAsInt() : 0;
   qc->setRgba(rgba);
   return 0;
}

//Spec spec () const
//static QoreNode *QCOLOR_spec(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qc->spec());
//}

//QColor toCmyk () const
//static QoreNode *QCOLOR_toCmyk(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qc->toCmyk());
//}

//QColor toHsv () const
//static QoreNode *QCOLOR_toHsv(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qc->toHsv());
//}

//QColor toRgb () const
//static QoreNode *QCOLOR_toRgb(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qc->toRgb());
//}

//int value () const
static QoreNode *QCOLOR_value(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->value());
}

//qreal valueF () const
static QoreNode *QCOLOR_valueF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->valueF());
}

//int yellow () const
static QoreNode *QCOLOR_yellow(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qc->yellow());
}

//qreal yellowF () const
static QoreNode *QCOLOR_yellowF(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qc->yellowF());
}

//QColor light (int f = 150) const
static QoreNode *QCOLOR_light(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int f = !is_nothing(p) ? p->getAsInt() : 150;
   Object *o_qc = new Object(self->getClass(CID_QCOLOR), getProgram());
   QoreQColor *q_qc = new QoreQColor(qc->light(f));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//QColor lighter (int f = 150) const
static QoreNode *QCOLOR_lighter(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int f = !is_nothing(p) ? p->getAsInt() : 150;
   Object *o_qc = new Object(self->getClass(CID_QCOLOR), getProgram());
   QoreQColor *q_qc = new QoreQColor(qc->lighter(f));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//QColor dark (int f = 200) const
static QoreNode *QCOLOR_dark(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int f = !is_nothing(p) ? p->getAsInt() : 200;
   Object *o_qc = new Object(self->getClass(CID_QCOLOR), getProgram());
   QoreQColor *q_qc = new QoreQColor(qc->dark(f));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

//QColor darker (int f = 200) const
static QoreNode *QCOLOR_darker(Object *self, QoreQColor *qc, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int f = !is_nothing(p) ? p->getAsInt() : 200;
   Object *o_qc = new Object(self->getClass(CID_QCOLOR), getProgram());
   QoreQColor *q_qc = new QoreQColor(qc->darker(f));
   o_qc->setPrivate(CID_QCOLOR, q_qc);
   return new QoreNode(o_qc);
}

class QoreClass *initQColorClass()
{
   tracein("initQColorClass()");
   
   QC_QColor = new QoreClass("QColor", QDOM_GUI);
   CID_QCOLOR = QC_QColor->getID();
   QC_QColor->setConstructor(QCOLOR_constructor);
   QC_QColor->setCopy((q_copy_t)QCOLOR_copy);

   QC_QColor->addMethod("alpha",                       (q_method_t)QCOLOR_alpha);
   QC_QColor->addMethod("alphaF",                      (q_method_t)QCOLOR_alphaF);
   QC_QColor->addMethod("black",                       (q_method_t)QCOLOR_black);
   QC_QColor->addMethod("blackF",                      (q_method_t)QCOLOR_blackF);
   QC_QColor->addMethod("blue",                        (q_method_t)QCOLOR_blue);
   QC_QColor->addMethod("blueF",                       (q_method_t)QCOLOR_blueF);
   QC_QColor->addMethod("convertTo",                   (q_method_t)QCOLOR_convertTo);
   QC_QColor->addMethod("cyan",                        (q_method_t)QCOLOR_cyan);
   QC_QColor->addMethod("cyanF",                       (q_method_t)QCOLOR_cyanF);
   //QC_QColor->addMethod("darker",                      (q_method_t)QCOLOR_darker);
   //QC_QColor->addMethod("getCmyk",                     (q_method_t)QCOLOR_getCmyk);
   //QC_QColor->addMethod("getCmykF",                    (q_method_t)QCOLOR_getCmykF);
   //QC_QColor->addMethod("getHsv",                      (q_method_t)QCOLOR_getHsv);
   //QC_QColor->addMethod("getHsvF",                     (q_method_t)QCOLOR_getHsvF);
   //QC_QColor->addMethod("getRgb",                      (q_method_t)QCOLOR_getRgb);
   //QC_QColor->addMethod("getRgbF",                     (q_method_t)QCOLOR_getRgbF);
   QC_QColor->addMethod("green",                       (q_method_t)QCOLOR_green);
   QC_QColor->addMethod("greenF",                      (q_method_t)QCOLOR_greenF);
   QC_QColor->addMethod("hue",                         (q_method_t)QCOLOR_hue);
   QC_QColor->addMethod("hueF",                        (q_method_t)QCOLOR_hueF);
   QC_QColor->addMethod("isValid",                     (q_method_t)QCOLOR_isValid);
   QC_QColor->addMethod("magenta",                     (q_method_t)QCOLOR_magenta);
   QC_QColor->addMethod("magentaF",                    (q_method_t)QCOLOR_magentaF);
   QC_QColor->addMethod("name",                        (q_method_t)QCOLOR_name);
   QC_QColor->addMethod("red",                         (q_method_t)QCOLOR_red);
   QC_QColor->addMethod("redF",                        (q_method_t)QCOLOR_redF);
   QC_QColor->addMethod("rgb",                         (q_method_t)QCOLOR_rgb);
   QC_QColor->addMethod("rgba",                        (q_method_t)QCOLOR_rgba);
   QC_QColor->addMethod("saturation",                  (q_method_t)QCOLOR_saturation);
   QC_QColor->addMethod("saturationF",                 (q_method_t)QCOLOR_saturationF);
   QC_QColor->addMethod("setAlpha",                    (q_method_t)QCOLOR_setAlpha);
   QC_QColor->addMethod("setAlphaF",                   (q_method_t)QCOLOR_setAlphaF);
   QC_QColor->addMethod("setBlue",                     (q_method_t)QCOLOR_setBlue);
   QC_QColor->addMethod("setBlueF",                    (q_method_t)QCOLOR_setBlueF);
   QC_QColor->addMethod("setCmyk",                     (q_method_t)QCOLOR_setCmyk);
   QC_QColor->addMethod("setCmykF",                    (q_method_t)QCOLOR_setCmykF);
   QC_QColor->addMethod("setGreen",                    (q_method_t)QCOLOR_setGreen);
   QC_QColor->addMethod("setGreenF",                   (q_method_t)QCOLOR_setGreenF);
   QC_QColor->addMethod("setHsv",                      (q_method_t)QCOLOR_setHsv);
   QC_QColor->addMethod("setHsvF",                     (q_method_t)QCOLOR_setHsvF);
   QC_QColor->addMethod("setNamedColor",               (q_method_t)QCOLOR_setNamedColor);
   QC_QColor->addMethod("setRed",                      (q_method_t)QCOLOR_setRed);
   QC_QColor->addMethod("setRedF",                     (q_method_t)QCOLOR_setRedF);
   QC_QColor->addMethod("setRgb",                      (q_method_t)QCOLOR_setRgb);
   QC_QColor->addMethod("setRgb",                      (q_method_t)QCOLOR_setRgb);
   QC_QColor->addMethod("setRgbF",                     (q_method_t)QCOLOR_setRgbF);
   QC_QColor->addMethod("setRgba",                     (q_method_t)QCOLOR_setRgba);
   //QC_QColor->addMethod("spec",                        (q_method_t)QCOLOR_spec);
   //QC_QColor->addMethod("toCmyk",                      (q_method_t)QCOLOR_toCmyk);
   //QC_QColor->addMethod("toHsv",                       (q_method_t)QCOLOR_toHsv);
   //QC_QColor->addMethod("toRgb",                       (q_method_t)QCOLOR_toRgb);
   QC_QColor->addMethod("value",                       (q_method_t)QCOLOR_value);
   QC_QColor->addMethod("valueF",                      (q_method_t)QCOLOR_valueF);
   QC_QColor->addMethod("yellow",                      (q_method_t)QCOLOR_yellow);
   QC_QColor->addMethod("yellowF",                     (q_method_t)QCOLOR_yellowF);

   QC_QColor->addMethod("light",                       (q_method_t)QCOLOR_light);
   QC_QColor->addMethod("lighter",                     (q_method_t)QCOLOR_lighter);
   QC_QColor->addMethod("dark",                        (q_method_t)QCOLOR_dark);
   QC_QColor->addMethod("darker",                      (q_method_t)QCOLOR_darker);

   traceout("initQColorClass()");
   return QC_QColor;
}

