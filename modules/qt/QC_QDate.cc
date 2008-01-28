/*
 QC_QDate.cc
 
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
#include "QC_QDate.h"

#include "qore-qt.h"

int CID_QDATE;
QoreClass *QC_QDate = 0;

static void QDATE_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreQDate *qdt;

   QoreNode *p = get_param(params, 0);
   if (is_nothing(p))
      qdt = new QoreQDate();
   else if (p->type == NT_DATE) {
      DateTimeNode *dt = reinterpret_cast<DateTimeNode *>(p);
      qdt = new QoreQDate(dt->getYear(), dt->getMonth(), dt->getDay());
   }
   else {
      int y = p ? p->getAsInt() : 0;
      p = get_param(params, 1);
      int m = p ? p->getAsInt() : 0;
      p = get_param(params, 2);
      int d = p ? p->getAsInt() : 0;

      qdt = new QoreQDate(y, m, d);
   }

   self->setPrivate(CID_QDATE, qdt);
}

static void QDATE_copy(class QoreObject *self, class QoreObject *old, class QoreQDate *qf, ExceptionSink *xsink)
{
   self->setPrivate(CID_QDATE, new QoreQDate(*qf));
   //xsink->raiseException("QDATE-COPY-ERROR", "objects of this class cannot be copied");
}

//QDate addDays ( int ndays ) const
static QoreNode *QDATE_addDays(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int ndays = p ? p->getAsInt() : 0;
   QoreObject *o_qd = new QoreObject(self->getClass(CID_QDATE), getProgram());
   QoreQDate *q_qd = new QoreQDate(qd->addDays(ndays));
   o_qd->setPrivate(CID_QDATE, q_qd);
   return o_qd;
}

//QDate addMonths ( int nmonths ) const
static QoreNode *QDATE_addMonths(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int nmonths = p ? p->getAsInt() : 0;
   QoreObject *o_qd = new QoreObject(self->getClass(CID_QDATE), getProgram());
   QoreQDate *q_qd = new QoreQDate(qd->addMonths(nmonths));
   o_qd->setPrivate(CID_QDATE, q_qd);
   return o_qd;
}

//QDate addYears ( int nyears ) const
static QoreNode *QDATE_addYears(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int nyears = p ? p->getAsInt() : 0;
   QoreObject *o_qd = new QoreObject(self->getClass(CID_QDATE), getProgram());
   QoreQDate *q_qd = new QoreQDate(qd->addYears(nyears));
   o_qd->setPrivate(CID_QDATE, q_qd);
   return o_qd;
}

//int day () const
static QoreNode *QDATE_day(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->day());
}

//int dayOfWeek () const
static QoreNode *QDATE_dayOfWeek(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->dayOfWeek());
}

//int dayOfYear () const
static QoreNode *QDATE_dayOfYear(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->dayOfYear());
}

//int daysInMonth () const
static QoreNode *QDATE_daysInMonth(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->daysInMonth());
}

//int daysInYear () const
static QoreNode *QDATE_daysInYear(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->daysInYear());
}

//int daysTo ( const QDate & d ) const
static QoreNode *QDATE_daysTo(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQDate *d = (p && p->type == NT_OBJECT) ? (QoreQDate *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QDATE, xsink) : 0;
   if (!d) {
      if (!xsink->isException())
         xsink->raiseException("QDATE-DAYSTO-PARAM-ERROR", "expecting a QDate object as first argument to QDate::daysTo()");
      return 0;
   }
   ReferenceHolder<QoreQDate> holder(d, xsink);
   return new QoreBigIntNode(qd->daysTo(*(static_cast<QDate *>(d))));
}

//bool isNull () const
static QoreNode *QDATE_isNull(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qd->isNull());
}

//bool isValid () const
static QoreNode *QDATE_isValid(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qd->isValid());
}

//int month () const
static QoreNode *QDATE_month(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->month());
}

//bool setDate ( int year, int month, int day )
static QoreNode *QDATE_setDate(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int year = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int month = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int day = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qd->setDate(year, month, day));
}

//int toJulianDay () const
static QoreNode *QDATE_toJulianDay(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->toJulianDay());
}

//QString toString ( const QString & format ) const
//QString toString ( Qt::DateFormat format = Qt::TextDate ) const
static QoreNode *QDATE_toString(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   {
      QString format;
      if (!get_qstring(p, format, xsink, true))
	 return new QoreStringNode(qd->toString(format).toUtf8().data(), QCS_UTF8);
   }
   if (*xsink)
      return 0;

   Qt::DateFormat format = (Qt::DateFormat)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qd->toString(format).toUtf8().data(), QCS_UTF8);
}

//int weekNumber ( int * yearNumber = 0 ) const
//static QoreNode *QDATE_weekNumber(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? int* yearNumber = p;
//   return new QoreBigIntNode(qd->weekNumber(yearNumber));
//}

//int year () const
static QoreNode *QDATE_year(QoreObject *self, QoreQDate *qd, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qd->year());
}

class QoreClass *initQDateClass()
{
   tracein("initQDateClass()");
   
   QC_QDate = new QoreClass("QDate", QDOM_GUI);
   CID_QDATE = QC_QDate->getID();
   QC_QDate->setConstructor(QDATE_constructor);
   QC_QDate->setCopy((q_copy_t)QDATE_copy);

   QC_QDate->addMethod("addDays",                     (q_method_t)QDATE_addDays);
   QC_QDate->addMethod("addMonths",                   (q_method_t)QDATE_addMonths);
   QC_QDate->addMethod("addYears",                    (q_method_t)QDATE_addYears);
   QC_QDate->addMethod("day",                         (q_method_t)QDATE_day);
   QC_QDate->addMethod("dayOfWeek",                   (q_method_t)QDATE_dayOfWeek);
   QC_QDate->addMethod("dayOfYear",                   (q_method_t)QDATE_dayOfYear);
   QC_QDate->addMethod("daysInMonth",                 (q_method_t)QDATE_daysInMonth);
   QC_QDate->addMethod("daysInYear",                  (q_method_t)QDATE_daysInYear);
   QC_QDate->addMethod("daysTo",                      (q_method_t)QDATE_daysTo);
   QC_QDate->addMethod("isNull",                      (q_method_t)QDATE_isNull);
   QC_QDate->addMethod("isValid",                     (q_method_t)QDATE_isValid);
   QC_QDate->addMethod("month",                       (q_method_t)QDATE_month);
   QC_QDate->addMethod("setDate",                     (q_method_t)QDATE_setDate);
   QC_QDate->addMethod("toJulianDay",                 (q_method_t)QDATE_toJulianDay);
   QC_QDate->addMethod("toString",                    (q_method_t)QDATE_toString);
   //QC_QDate->addMethod("weekNumber",                  (q_method_t)QDATE_weekNumber);
   QC_QDate->addMethod("year",                        (q_method_t)QDATE_year);

   traceout("initQDateClass()");
   return QC_QDate;
}
