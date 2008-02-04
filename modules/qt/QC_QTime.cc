/*
 QC_QTime.cc
 
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
#include "QC_QTime.h"

#include "qore-qt.h"

int CID_QTIME;
QoreClass *QC_QTime = 0;

static void QTIME_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQTime *qdt;

   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qdt = new QoreQTime();
   else if (p->type == NT_DATE) {
      const DateTimeNode *dt = reinterpret_cast<const DateTimeNode *>(p);
      qdt = new QoreQTime(dt->getHour(), dt->getMinute(), dt->getSecond(), dt->getMillisecond());
   }
   else {
      int h = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int m = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int s = p ? p->getAsInt() : 0;
      p = get_param(params, 3);
      int ms = p ? p->getAsInt() : 0;

      qdt = new QoreQTime(h, m, s, ms);
   }

   self->setPrivate(CID_QTIME, qdt);
}

static void QTIME_copy(class QoreObject *self, class QoreObject *old, class QoreQTime *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QTIME, new QoreQTime(*qf));
   //xsink->raiseException("QTIME-COPY-ERROR", "objects of this class cannot be copied");
}

//QTime addMSecs ( int ms ) const
static AbstractQoreNode *QTIME_addMSecs(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int ms = p ? p->getAsInt() : 0;
   QoreObject *o_qt = new QoreObject(self->getClass(CID_QTIME), getProgram());
   QoreQTime *q_qt = new QoreQTime(qt->addMSecs(ms));
   o_qt->setPrivate(CID_QTIME, q_qt);
   return o_qt;
}

//QTime addSecs ( int s ) const
static AbstractQoreNode *QTIME_addSecs(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int s = p ? p->getAsInt() : 0;
   QoreObject *o_qt = new QoreObject(self->getClass(CID_QTIME), getProgram());
   QoreQTime *q_qt = new QoreQTime(qt->addSecs(s));
   o_qt->setPrivate(CID_QTIME, q_qt);
   return o_qt;
}

//int elapsed () const
static AbstractQoreNode *QTIME_elapsed(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->elapsed());
}

//int hour () const
static AbstractQoreNode *QTIME_hour(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->hour());
}

//bool isNull () const
static AbstractQoreNode *QTIME_isNull(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qt->isNull());
}

//bool isValid () const
static AbstractQoreNode *QTIME_isValid(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qt->isValid());
}

//int minute () const
static AbstractQoreNode *QTIME_minute(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->minute());
}

//int msec () const
static AbstractQoreNode *QTIME_msec(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->msec());
}

//int msecsTo ( const QTime & t ) const
static AbstractQoreNode *QTIME_msecsTo(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTime *t = p ? (QoreQTime *)p->getReferencedPrivateData(CID_QTIME, xsink) : 0;
   if (!t) {
      if (!xsink->isException())
         xsink->raiseException("QTIME-MSECSTO-PARAM-ERROR", "expecting a QTime object as first argument to QTime::msecsTo()");
      return 0;
   }
   ReferenceHolder<QoreQTime> holder(t, xsink);
   return new QoreBigIntNode(qt->msecsTo(*(static_cast<QTime *>(t))));
}

//int restart ()
static AbstractQoreNode *QTIME_restart(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->restart());
}

//int second () const
static AbstractQoreNode *QTIME_second(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qt->second());
}

//int secsTo ( const QTime & t ) const
static AbstractQoreNode *QTIME_secsTo(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQTime *t = p ? (QoreQTime *)p->getReferencedPrivateData(CID_QTIME, xsink) : 0;
   if (!t) {
      if (!xsink->isException())
         xsink->raiseException("QTIME-SECSTO-PARAM-ERROR", "expecting a QTime object as first argument to QTime::secsTo()");
      return 0;
   }
   ReferenceHolder<QoreQTime> holder(t, xsink);
   return new QoreBigIntNode(qt->secsTo(*(static_cast<QTime *>(t))));
}

//bool setHMS ( int h, int m, int s, int ms = 0 )
static AbstractQoreNode *QTIME_setHMS(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int h = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int m = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int s = p ? p->getAsInt() : 0;
   p = get_param(params, 3);
   int ms = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qt->setHMS(h, m, s, ms));
}

//void start ()
static AbstractQoreNode *QTIME_start(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   qt->start();
   return 0;
}

//QString toString ( const QString & format ) const
//QString toString ( Qt::DateFormat f = Qt::TextDate ) const
static AbstractQoreNode *QTIME_toString(QoreObject *self, QoreQTime *qt, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString format;
   if (get_qstring(p, format, xsink)) {
      Qt::DateFormat f = (Qt::DateFormat)(p ? p->getAsInt() : 0);
      return new QoreStringNode(qt->toString(f).toUtf8().data(), QCS_UTF8);
   }
   return new QoreStringNode(qt->toString(format).toUtf8().data(), QCS_UTF8);
}

class QoreClass *initQTimeClass()
{
   tracein("initQTimeClass()");
   
   QC_QTime = new QoreClass("QTime", QDOM_GUI);
   CID_QTIME = QC_QTime->getID();
   QC_QTime->setConstructor(QTIME_constructor);
   QC_QTime->setCopy((q_copy_t)QTIME_copy);

   QC_QTime->addMethod("addMSecs",                    (q_method_t)QTIME_addMSecs);
   QC_QTime->addMethod("addSecs",                     (q_method_t)QTIME_addSecs);
   QC_QTime->addMethod("elapsed",                     (q_method_t)QTIME_elapsed);
   QC_QTime->addMethod("hour",                        (q_method_t)QTIME_hour);
   QC_QTime->addMethod("isNull",                      (q_method_t)QTIME_isNull);
   QC_QTime->addMethod("isValid",                     (q_method_t)QTIME_isValid);
   QC_QTime->addMethod("minute",                      (q_method_t)QTIME_minute);
   QC_QTime->addMethod("msec",                        (q_method_t)QTIME_msec);
   QC_QTime->addMethod("msecsTo",                     (q_method_t)QTIME_msecsTo);
   QC_QTime->addMethod("restart",                     (q_method_t)QTIME_restart);
   QC_QTime->addMethod("second",                      (q_method_t)QTIME_second);
   QC_QTime->addMethod("secsTo",                      (q_method_t)QTIME_secsTo);
   QC_QTime->addMethod("setHMS",                      (q_method_t)QTIME_setHMS);
   QC_QTime->addMethod("start",                       (q_method_t)QTIME_start);
   QC_QTime->addMethod("toString",                    (q_method_t)QTIME_toString);

   traceout("initQTimeClass()");
   return QC_QTime;
}
