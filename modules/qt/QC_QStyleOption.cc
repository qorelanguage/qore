/*
 QC_QStyleOption.cc
 
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

#include "QC_QStyleOption.h"
#include "QC_QWidget.h"
#include "QC_QRect.h"
#include "QC_QPalette.h"
#include "QC_QFontMetrics.h"

#include "qore-qt.h"

qore_classid_t CID_QSTYLEOPTION;
QoreClass *QC_QStyleOption = 0;

int QStyleOption_Notification(QoreObject *obj, QStyleOption *qso, const char *mem, ExceptionSink *xsink)
{
   AbstractQoreNode *p;

   if (!strcmp(mem, "rect")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQRect *rect = (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, xsink);
      if (!rect)
	 return 0;
      ReferenceHolder<AbstractPrivateData> rectHolder(static_cast<AbstractPrivateData *>(rect), xsink);
      qso->rect = *(static_cast<QRect *>(rect));      

      return 0;
   }

   if (!strcmp(mem, "palette")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQPalette *palette = (QoreQPalette *)o->getReferencedPrivateData(CID_QPALETTE, xsink);
      if (!palette)
	 return 0;

      ReferenceHolder<AbstractPrivateData> paletteHolder(static_cast<AbstractPrivateData *>(palette), xsink);
      qso->palette = *(palette->getQPalette());
      return 0;
   }

   if (!strcmp(mem, "fontMetrics")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink || !p || p->getType() != NT_OBJECT)
	 return 0;

      QoreObject *o = reinterpret_cast<QoreObject *>(p);
      QoreQFontMetrics *fontMetrics = (QoreQFontMetrics *)o->getReferencedPrivateData(CID_QFONTMETRICS, xsink);
      if (!fontMetrics)
	 return 0;
      ReferenceHolder<AbstractPrivateData> fontMetricsHolder(static_cast<AbstractPrivateData *>(fontMetrics), xsink);
      qso->fontMetrics = *(static_cast<QFontMetrics *>(fontMetrics));
      return 0;
   }

   if (!strcmp(mem, "state")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      QStyle::State state = (QStyle::State)(p ? p->getAsInt() : 0);
      qso->state = state;
      return 0;
   }

   if (!strcmp(mem, "direction")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      Qt::LayoutDirection direction = (Qt::LayoutDirection)(p ? p->getAsInt() : 0);
      qso->direction = direction;
      return 0;
   }

   if (!strcmp(mem, "type")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int type = p ? p->getAsInt() : 0;
      qso->type = type;
      return 0;
   }

   if (!strcmp(mem, "version")) {
      AutoVLock vl(xsink);
      p = obj->getMemberValueNoMethod(mem, &vl, xsink);
      if (*xsink)
	 return 0;

      int version = p ? p->getAsInt() : 0;
      qso->version = version;
      return 0;
   }

   return -1;
}

AbstractQoreNode *QStyleOption_MemberGate(QStyleOption *qso, const char *mem)
{
   if (!strcmp(mem, "rect")) {
      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
      QoreQRect *q_qr = new QoreQRect(qso->rect);
      o_qr->setPrivate(CID_QRECT, q_qr);
      return o_qr;
   }

   if (!strcmp(mem, "palette")) {
      QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
      QoreQPalette *q_qp = new QoreQPalette(qso->palette);
      o_qp->setPrivate(CID_QPALETTE, q_qp);
      return o_qp;
   }

   if (!strcmp(mem, "fontMetrics")) {
      QoreObject *o_qfm = new QoreObject(QC_QFontMetrics, getProgram());
      QoreQFontMetrics *q_qfm = new QoreQFontMetrics(qso->fontMetrics);
      o_qfm->setPrivate(CID_QFONTMETRICS, q_qfm);
      return o_qfm;
   }

   if (!strcmp(mem, "state")) {
      return new QoreBigIntNode(qso->state);
   }

   if (!strcmp(mem, "direction")) {
      return new QoreBigIntNode(qso->direction);
   }

   if (!strcmp(mem, "type")) {
      return new QoreBigIntNode(qso->type);
   }

   if (!strcmp(mem, "version")) {
      return new QoreBigIntNode(qso->version);
   }

   return 0;
}

static AbstractQoreNode *QSTYLEOPTION_memberNotification(QoreObject *self, QoreQStyleOption *qso, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   QStyleOption_Notification(self, qso, str->getBuffer(), xsink);
   return 0;
}

static AbstractQoreNode *QSTYLEOPTION_memberGate(QoreObject *self, QoreQStyleOption *qso, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *str = test_string_param(params, 0);
   if (!str || !str->strlen())
      return 0;

   const char *member = str->getBuffer();
   AbstractQoreNode *rv = QStyleOption_MemberGate(qso, member);
   if (rv)
      return rv;

   AutoVLock vl(xsink);
   rv = self->getMemberValueNoMethod(member, &vl, xsink);
   return rv ? rv->refSelf() : 0;
}

//QStyleOption ( int version = QStyleOption::Version, int type = SO_Default )
//QStyleOption ( const QStyleOption & other )
static void QSTYLEOPTION_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
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
static AbstractQoreNode *QSTYLEOPTION_initFrom(QoreObject *self, QoreQStyleOption *qso, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQWidget *widget = p ? (QoreAbstractQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QSTYLEOPTION-INITFROM-PARAM-ERROR", "expecting a QWidget object as first argument to QStyleOption::initFrom()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> widgetHolder(widget, xsink);
   qso->initFrom(widget->getQWidget());
   return 0;
}


QoreClass *initQStyleOptionClass()
{
   QC_QStyleOption = new QoreClass("QStyleOption", QDOM_GUI);
   CID_QSTYLEOPTION = QC_QStyleOption->getID();

   QC_QStyleOption->setConstructor(QSTYLEOPTION_constructor);
   QC_QStyleOption->setCopy((q_copy_t)QSTYLEOPTION_copy);

   // add special methods
   QC_QStyleOption->addMethod("memberNotification",          (q_method_t)QSTYLEOPTION_memberNotification);
   QC_QStyleOption->addMethod("memberGate",                  (q_method_t)QSTYLEOPTION_memberGate);

   QC_QStyleOption->addMethod("initFrom",                    (q_method_t)QSTYLEOPTION_initFrom);

   return QC_QStyleOption;
}
