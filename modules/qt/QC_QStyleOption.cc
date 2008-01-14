/*
 QC_QStyleOption.cc
 
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

#include "QC_QStyleOption.h"
#include "QC_QWidget.h"
#include "QC_QRect.h"
#include "QC_QPalette.h"
#include "QC_QFontMetrics.h"

#include "qore-qt.h"

int CID_QSTYLEOPTION;
class QoreClass *QC_QStyleOption = 0;

//QStyleOption ( int version = QStyleOption::Version, int type = SO_Default )
//QStyleOption ( const QStyleOption & other )
static void QSTYLEOPTION_constructor(QoreObject *self, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSTYLEOPTION, new QoreQStyleOption());
      return;
   }
   int version = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int type = p ? p->getAsInt() : 0;
   self->setPrivate(CID_QSTYLEOPTION, new QoreQStyleOption(version, type));
   return;
}

static void QSTYLEOPTION_copy(class QoreObject *self, class QoreObject *old, class QoreQStyleOption *qso, ExceptionSink *xsink)
{
   self->setPrivate(CID_QSTYLEOPTION, new QoreQStyleOption(*qso));
}

//void initFrom ( const QWidget * widget )
static QoreNode *QSTYLEOPTION_initFrom(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-INITFROM-PARAM-ERROR", "expecting a QWidget object as first argument to QStyleOption::initFrom()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   qso->initFrom(widget->getQWidget());
   return 0;
}

//void setRect ( const QRect & rect )
static QoreNode *QSTYLEOPTION_setRect(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQRect *rect = (p && p->type == NT_OBJECT) ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!rect) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-SETRECT-PARAM-ERROR", "expecting a QRect object as first argument to QStyleOption::setRect()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
   qso->rect = *(static_cast<QRect *>(rect));
   return 0;
}

//QRect rect ( ) const
static QoreNode *QSTYLEOPTION_rect(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qso->rect);
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//void setPalette ( const QPalette & palette )
static QoreNode *QSTYLEOPTION_setPalette(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as first argument to QStyleOption::setPalette()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> paletteHolder(static_cast<AbstractPrivateData *>(palette), xsink);
   qso->palette = *(palette->getQPalette());
   return 0;
}

//QPalette palette ( ) const
static QoreNode *QSTYLEOPTION_palette(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(&qso->palette);
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
}

//void setFontMetrics ( const QFontMetrics & fontMetrics )
static QoreNode *QSTYLEOPTION_setFontMetrics(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFontMetrics *fontMetrics = (p && p->type == NT_OBJECT) ? (QoreQFontMetrics *)p->val.object->getReferencedPrivateData(CID_QFONTMETRICS, xsink) : 0;
   if (!fontMetrics) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-SETFONTMETRICS-PARAM-ERROR", "expecting a QFontMetrics object as first argument to QStyleOption::setFontMetrics()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> fontMetricsHolder(static_cast<AbstractPrivateData *>(fontMetrics), xsink);
   qso->fontMetrics = *(static_cast<QFontMetrics *>(fontMetrics));
   return 0;
}

//QFontMetrics fontMetrics ( ) const
static QoreNode *QSTYLEOPTION_fontMetrics(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qfm = new QoreObject(QC_QFontMetrics, getProgram());
   QoreQFontMetrics *q_qfm = new QoreQFontMetrics(qso->fontMetrics);
   o_qfm->setPrivate(CID_QFONTMETRICS, q_qfm);
   return new QoreNode(o_qfm);
}

//void setState ( QStyle::State state )
static QoreNode *QSTYLEOPTION_setState(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QStyle::State state = (QStyle::State)(p ? p->getAsInt() : 0);
   qso->state = state;
   return 0;
}

//QStyle::State state ( ) const
static QoreNode *QSTYLEOPTION_state(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qso->state);
}

//void setDirection ( Qt::LayoutDirection direction )
static QoreNode *QSTYLEOPTION_setDirection(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
   qso->direction = direction;
   return 0;
}

//Qt::LayoutDirection direction ( ) const
static QoreNode *QSTYLEOPTION_direction(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qso->direction);
}

//void setType ( int type )
static QoreNode *QSTYLEOPTION_setType(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int type = p ? p->getAsInt() : 0;
   qso->type = type;
   return 0;
}

//int type ( ) const
static QoreNode *QSTYLEOPTION_type(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qso->type);
}

//void setVersion ( int version )
static QoreNode *QSTYLEOPTION_setVersion(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int version = p ? p->getAsInt() : 0;
   qso->version = version;
   return 0;
}

//int version ( ) const
static QoreNode *QSTYLEOPTION_version(QoreObject *self, QoreQStyleOption *qso, const QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qso->version);
}

QoreClass *initQStyleOptionClass()
{
   QC_QStyleOption = new QoreClass("QStyleOption", QDOM_GUI);
   CID_QSTYLEOPTION = QC_QStyleOption->getID();

   QC_QStyleOption->setConstructor(QSTYLEOPTION_constructor);
   QC_QStyleOption->setCopy((q_copy_t)QSTYLEOPTION_copy);

   QC_QStyleOption->addMethod("initFrom",                    (q_method_t)QSTYLEOPTION_initFrom);

   QC_QStyleOption->addMethod("setRect",                     (q_method_t)QSTYLEOPTION_setRect);
   QC_QStyleOption->addMethod("rect",                        (q_method_t)QSTYLEOPTION_rect);
   QC_QStyleOption->addMethod("setPalette",                  (q_method_t)QSTYLEOPTION_setPalette);
   QC_QStyleOption->addMethod("palette",                     (q_method_t)QSTYLEOPTION_palette);
   QC_QStyleOption->addMethod("setFontMetrics",              (q_method_t)QSTYLEOPTION_setFontMetrics);
   QC_QStyleOption->addMethod("fontMetrics",                 (q_method_t)QSTYLEOPTION_fontMetrics);
   QC_QStyleOption->addMethod("setState",                    (q_method_t)QSTYLEOPTION_setState);
   QC_QStyleOption->addMethod("state",                       (q_method_t)QSTYLEOPTION_state);
   QC_QStyleOption->addMethod("setDirection",                (q_method_t)QSTYLEOPTION_setDirection);
   QC_QStyleOption->addMethod("direction",                   (q_method_t)QSTYLEOPTION_direction);
   QC_QStyleOption->addMethod("setType",                     (q_method_t)QSTYLEOPTION_setType);
   QC_QStyleOption->addMethod("type",                        (q_method_t)QSTYLEOPTION_type);
   QC_QStyleOption->addMethod("setVersion",                  (q_method_t)QSTYLEOPTION_setVersion);
   QC_QStyleOption->addMethod("version",                     (q_method_t)QSTYLEOPTION_version);

   return QC_QStyleOption;
}
