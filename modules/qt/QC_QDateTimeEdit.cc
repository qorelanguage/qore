/*
 QC_QDateTimeEdit.cc
 
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

#include "QC_QDateTimeEdit.h"
#include "QC_QWidget.h"

#include "qore-qt.h"

qore_classid_t CID_QDATETIMEEDIT;
class QoreClass *QC_QDateTimeEdit = 0;

//QDateTimeEdit ( QWidget * parent = 0 )
//QDateTimeEdit ( const QDateTime & datetime, QWidget * parent = 0 )
//QDateTimeEdit ( const QDate & date, QWidget * parent = 0 )
//QDateTimeEdit ( const QTime & time, QWidget * parent = 0 )
static void QDATETIMEEDIT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QDATETIMEEDIT, new QoreQDateTimeEdit(self));
      return;
   }

   QoreQWidget *parent = 0;
   if (p->getType() == NT_OBJECT) {
      parent = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
      self->setPrivate(CID_QDATETIMEEDIT, new QoreQDateTimeEdit(self, parent->getQWidget()));
      return;
   }

   QDateTime datetime;
   if (get_qdatetime(p, datetime, xsink))
      return;

   // get parent widget as second argument if possible
   const QoreObject *o = test_object_param(params, 1);
   parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);

   self->setPrivate(CID_QDATETIMEEDIT, new QoreQDateTimeEdit(self, datetime, parent ? parent->getQWidget() : 0));
}

static void QDATETIMEEDIT_copy(class QoreObject *self, class QoreObject *old, class QoreQDateTimeEdit *qdte, ExceptionSink *xsink)
{
   xsink->raiseException("QDATETIMEEDIT-COPY-ERROR", "objects of this class cannot be copied");
}

//bool calendarPopup () const
static AbstractQoreNode *QDATETIMEEDIT_calendarPopup(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qdte->getQDateTimeEdit()->calendarPopup());
}

//void clearMaximumDate ()
static AbstractQoreNode *QDATETIMEEDIT_clearMaximumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   qdte->getQDateTimeEdit()->clearMaximumDate();
   return 0;
}

//void clearMaximumTime ()
static AbstractQoreNode *QDATETIMEEDIT_clearMaximumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   qdte->getQDateTimeEdit()->clearMaximumTime();
   return 0;
}

//void clearMinimumDate ()
static AbstractQoreNode *QDATETIMEEDIT_clearMinimumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   qdte->getQDateTimeEdit()->clearMinimumDate();
   return 0;
}

//void clearMinimumTime ()
static AbstractQoreNode *QDATETIMEEDIT_clearMinimumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   qdte->getQDateTimeEdit()->clearMinimumTime();
   return 0;
}

//Section currentSection () const
static AbstractQoreNode *QDATETIMEEDIT_currentSection(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdte->getQDateTimeEdit()->currentSection());
}

//int currentSectionIndex () const
static AbstractQoreNode *QDATETIMEEDIT_currentSectionIndex(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdte->getQDateTimeEdit()->currentSectionIndex());
}

//QDate date () const
static AbstractQoreNode *QDATETIMEEDIT_date(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qdte->getQDateTimeEdit()->date();
   return new DateTimeNode(rv_date.year(), rv_date.month(), rv_date.day());
}

//QDateTime dateTime () const
static AbstractQoreNode *QDATETIMEEDIT_dateTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QDateTime rv_dt = qdte->getQDateTimeEdit()->dateTime();
   QDate rv_d = rv_dt.date();
   QTime rv_t = rv_dt.time();
   return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//QString displayFormat () const
static AbstractQoreNode *QDATETIMEEDIT_displayFormat(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qdte->getQDateTimeEdit()->displayFormat().toUtf8().data(), QCS_UTF8);
}

//Sections displayedSections () const
static AbstractQoreNode *QDATETIMEEDIT_displayedSections(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdte->getQDateTimeEdit()->displayedSections());
}

//QDate maximumDate () const
static AbstractQoreNode *QDATETIMEEDIT_maximumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qdte->getQDateTimeEdit()->maximumDate();
   return new DateTimeNode(rv_date.year(), rv_date.month(), rv_date.day());
}

//QTime maximumTime () const
static AbstractQoreNode *QDATETIMEEDIT_maximumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QTime rv_t = qdte->getQDateTimeEdit()->maximumTime();
   return new DateTimeNode(1970, 1, 1, rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//QDate minimumDate () const
static AbstractQoreNode *QDATETIMEEDIT_minimumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qdte->getQDateTimeEdit()->minimumDate();
   return new DateTimeNode(rv_date.year(), rv_date.month(), rv_date.day());
}

//QTime minimumTime () const
static AbstractQoreNode *QDATETIMEEDIT_minimumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QTime rv_t = qdte->getQDateTimeEdit()->minimumTime();
   return new DateTimeNode(1970, 1, 1, rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//Section sectionAt ( int index ) const
static AbstractQoreNode *QDATETIMEEDIT_sectionAt(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qdte->getQDateTimeEdit()->sectionAt(index));
}

//int sectionCount () const
static AbstractQoreNode *QDATETIMEEDIT_sectionCount(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdte->getQDateTimeEdit()->sectionCount());
}

//QString sectionText ( Section section ) const
static AbstractQoreNode *QDATETIMEEDIT_sectionText(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDateTimeEdit::Section section = (QDateTimeEdit::Section)(p ? p->getAsInt() : 0);
   return new QoreStringNode(qdte->getQDateTimeEdit()->sectionText(section).toUtf8().data(), QCS_UTF8);
}

//void setCalendarPopup ( bool enable )
static AbstractQoreNode *QDATETIMEEDIT_setCalendarPopup(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qdte->getQDateTimeEdit()->setCalendarPopup(enable);
   return 0;
}

//void setCurrentSection ( Section section )
static AbstractQoreNode *QDATETIMEEDIT_setCurrentSection(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDateTimeEdit::Section section = (QDateTimeEdit::Section)(p ? p->getAsInt() : 0);
   qdte->getQDateTimeEdit()->setCurrentSection(section);
   return 0;
}

//void setCurrentSectionIndex ( int index )
static AbstractQoreNode *QDATETIMEEDIT_setCurrentSectionIndex(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int index = p ? p->getAsInt() : 0;
   qdte->getQDateTimeEdit()->setCurrentSectionIndex(index);
   return 0;
}

//void setDateRange ( const QDate & min, const QDate & max )
static AbstractQoreNode *QDATETIMEEDIT_setDateRange(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDate min;
   if (get_qdate(p, min, xsink))
      return 0;
   p = get_param(params, 1);
   QDate max;
   if (get_qdate(p, max, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setDateRange(min, max);
   return 0;
}

//void setDisplayFormat ( const QString & format )
static AbstractQoreNode *QDATETIMEEDIT_setDisplayFormat(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString format;
   if (get_qstring(p, format, xsink))
      return 0;

   qdte->getQDateTimeEdit()->setDisplayFormat(format);
   return 0;
}

//void setMaximumDate ( const QDate & max )
static AbstractQoreNode *QDATETIMEEDIT_setMaximumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDate max;
   if (get_qdate(p, max, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setMaximumDate(max);
   return 0;
}

//void setMaximumTime ( const QTime & max )
static AbstractQoreNode *QDATETIMEEDIT_setMaximumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTime max;
   if (get_qtime(p, max, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setMaximumTime(max);
   return 0;
}

//void setMinimumDate ( const QDate & min )
static AbstractQoreNode *QDATETIMEEDIT_setMinimumDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDate min;
   if (get_qdate(p, min, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setMinimumDate(min);
   return 0;
}

//void setMinimumTime ( const QTime & min )
static AbstractQoreNode *QDATETIMEEDIT_setMinimumTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTime min;
   if (get_qtime(p, min, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setMinimumTime(min);
   return 0;
}

//void setSelectedSection ( Section section )
static AbstractQoreNode *QDATETIMEEDIT_setSelectedSection(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDateTimeEdit::Section section = (QDateTimeEdit::Section)(p ? p->getAsInt() : 0);
   qdte->getQDateTimeEdit()->setSelectedSection(section);
   return 0;
}

//void setTimeRange ( const QTime & min, const QTime & max )
static AbstractQoreNode *QDATETIMEEDIT_setTimeRange(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTime min;
   if (get_qtime(p, min, xsink))
      return 0;
   p = get_param(params, 1);
   QTime max;
   if (get_qtime(p, max, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setTimeRange(min, max);
   return 0;
}

//QTime time () const
static AbstractQoreNode *QDATETIMEEDIT_time(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   QTime rv_t = qdte->getQDateTimeEdit()->time();
   return new DateTimeNode(1970, 1, 1, rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//void setDate ( const QDate & date )
static AbstractQoreNode *QDATETIMEEDIT_setDate(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDate date;
   if (get_qdate(p, date, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setDate(date);
   return 0;
}

//void setDateTime ( const QDateTime & dateTime )
static AbstractQoreNode *QDATETIMEEDIT_setDateTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QDateTime dateTime;
   if (get_qdatetime(p, dateTime, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setDateTime(dateTime);
   return 0;
}

//void setTime ( const QTime & time )
static AbstractQoreNode *QDATETIMEEDIT_setTime(QoreObject *self, QoreAbstractQDateTimeEdit *qdte, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QTime time;
   if (get_qtime(p, time, xsink))
      return 0;
   qdte->getQDateTimeEdit()->setTime(time);
   return 0;
}

QoreClass *initQDateTimeEditClass(QoreClass *qabstractspinbox)
{
   QC_QDateTimeEdit = new QoreClass("QDateTimeEdit", QDOM_GUI);
   CID_QDATETIMEEDIT = QC_QDateTimeEdit->getID();

   QC_QDateTimeEdit->addBuiltinVirtualBaseClass(qabstractspinbox);

   QC_QDateTimeEdit->setConstructor(QDATETIMEEDIT_constructor);
   QC_QDateTimeEdit->setCopy((q_copy_t)QDATETIMEEDIT_copy);

   QC_QDateTimeEdit->addMethod("calendarPopup",               (q_method_t)QDATETIMEEDIT_calendarPopup);
   QC_QDateTimeEdit->addMethod("clearMaximumDate",            (q_method_t)QDATETIMEEDIT_clearMaximumDate);
   QC_QDateTimeEdit->addMethod("clearMaximumTime",            (q_method_t)QDATETIMEEDIT_clearMaximumTime);
   QC_QDateTimeEdit->addMethod("clearMinimumDate",            (q_method_t)QDATETIMEEDIT_clearMinimumDate);
   QC_QDateTimeEdit->addMethod("clearMinimumTime",            (q_method_t)QDATETIMEEDIT_clearMinimumTime);
   QC_QDateTimeEdit->addMethod("currentSection",              (q_method_t)QDATETIMEEDIT_currentSection);
   QC_QDateTimeEdit->addMethod("currentSectionIndex",         (q_method_t)QDATETIMEEDIT_currentSectionIndex);
   QC_QDateTimeEdit->addMethod("date",                        (q_method_t)QDATETIMEEDIT_date);
   QC_QDateTimeEdit->addMethod("dateTime",                    (q_method_t)QDATETIMEEDIT_dateTime);
   QC_QDateTimeEdit->addMethod("displayFormat",               (q_method_t)QDATETIMEEDIT_displayFormat);
   QC_QDateTimeEdit->addMethod("displayedSections",           (q_method_t)QDATETIMEEDIT_displayedSections);
   QC_QDateTimeEdit->addMethod("maximumDate",                 (q_method_t)QDATETIMEEDIT_maximumDate);
   QC_QDateTimeEdit->addMethod("maximumTime",                 (q_method_t)QDATETIMEEDIT_maximumTime);
   QC_QDateTimeEdit->addMethod("minimumDate",                 (q_method_t)QDATETIMEEDIT_minimumDate);
   QC_QDateTimeEdit->addMethod("minimumTime",                 (q_method_t)QDATETIMEEDIT_minimumTime);
   QC_QDateTimeEdit->addMethod("sectionAt",                   (q_method_t)QDATETIMEEDIT_sectionAt);
   QC_QDateTimeEdit->addMethod("sectionCount",                (q_method_t)QDATETIMEEDIT_sectionCount);
   QC_QDateTimeEdit->addMethod("sectionText",                 (q_method_t)QDATETIMEEDIT_sectionText);
   QC_QDateTimeEdit->addMethod("setCalendarPopup",            (q_method_t)QDATETIMEEDIT_setCalendarPopup);
   QC_QDateTimeEdit->addMethod("setCurrentSection",           (q_method_t)QDATETIMEEDIT_setCurrentSection);
   QC_QDateTimeEdit->addMethod("setCurrentSectionIndex",      (q_method_t)QDATETIMEEDIT_setCurrentSectionIndex);
   QC_QDateTimeEdit->addMethod("setDateRange",                (q_method_t)QDATETIMEEDIT_setDateRange);
   QC_QDateTimeEdit->addMethod("setDisplayFormat",            (q_method_t)QDATETIMEEDIT_setDisplayFormat);
   QC_QDateTimeEdit->addMethod("setMaximumDate",              (q_method_t)QDATETIMEEDIT_setMaximumDate);
   QC_QDateTimeEdit->addMethod("setMaximumTime",              (q_method_t)QDATETIMEEDIT_setMaximumTime);
   QC_QDateTimeEdit->addMethod("setMinimumDate",              (q_method_t)QDATETIMEEDIT_setMinimumDate);
   QC_QDateTimeEdit->addMethod("setMinimumTime",              (q_method_t)QDATETIMEEDIT_setMinimumTime);
   QC_QDateTimeEdit->addMethod("setSelectedSection",          (q_method_t)QDATETIMEEDIT_setSelectedSection);
   QC_QDateTimeEdit->addMethod("setTimeRange",                (q_method_t)QDATETIMEEDIT_setTimeRange);
   QC_QDateTimeEdit->addMethod("time",                        (q_method_t)QDATETIMEEDIT_time);
   QC_QDateTimeEdit->addMethod("setDate",                     (q_method_t)QDATETIMEEDIT_setDate);
   QC_QDateTimeEdit->addMethod("setDateTime",                 (q_method_t)QDATETIMEEDIT_setDateTime);
   QC_QDateTimeEdit->addMethod("setTime",                     (q_method_t)QDATETIMEEDIT_setTime);

   return QC_QDateTimeEdit;
}
