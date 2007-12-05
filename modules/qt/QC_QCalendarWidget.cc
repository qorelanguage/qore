/*
 QC_QCalendarWidget.cc
 
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

#include "QC_QCalendarWidget.h"

int CID_QCALENDARWIDGET;
class QoreClass *QC_QCalendarWidget = 0;

//QCalendarWidget ( QWidget * parent = 0 )
static void QCALENDARWIDGET_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QCALENDARWIDGET, new QoreQCalendarWidget(self, parent ? parent->getQWidget() : 0));
   return;
}

static void QCALENDARWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQCalendarWidget *qcw, ExceptionSink *xsink)
{
   xsink->raiseException("QCALENDARWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//int dateEditAcceptDelay () const
static QoreNode *QCALENDARWIDGET_dateEditAcceptDelay(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->dateEditAcceptDelay());
}

////QMap<QDate, QTextCharFormat> dateTextFormat () const
////QTextCharFormat dateTextFormat ( const QDate & date ) const
//static QoreNode *QCALENDARWIDGET_dateTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (is_nothing(p)) {
//      ??? return new QoreNode((int64)qcw->qobj->dateTextFormat());
//   }
//   QDate date;
//   if (get_qdate(p, date, xsink))
//      return 0;
//   ??? return new QoreNode((int64)qcw->qobj->dateTextFormat(date));
//}

//Qt::DayOfWeek firstDayOfWeek () const
static QoreNode *QCALENDARWIDGET_firstDayOfWeek(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->firstDayOfWeek());
}

//QTextCharFormat headerTextFormat () const
static QoreNode *QCALENDARWIDGET_headerTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qtcf = new QoreObject(QC_QTextCharFormat, getProgram());
   QoreQTextCharFormat *q_qtcf = new QoreQTextCharFormat(qcw->qobj->headerTextFormat());
   o_qtcf->setPrivate(CID_QTEXTCHARFORMAT, q_qtcf);
   return new QoreNode(o_qtcf);
}

//HorizontalHeaderFormat horizontalHeaderFormat () const
static QoreNode *QCALENDARWIDGET_horizontalHeaderFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->horizontalHeaderFormat());
}

//bool isDateEditEnabled () const
static QoreNode *QCALENDARWIDGET_isDateEditEnabled(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcw->qobj->isDateEditEnabled());
}

//bool isGridVisible () const
static QoreNode *QCALENDARWIDGET_isGridVisible(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcw->qobj->isGridVisible());
}

//bool isNavigationBarVisible () const
static QoreNode *QCALENDARWIDGET_isNavigationBarVisible(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qcw->qobj->isNavigationBarVisible());
}

//QDate maximumDate () const
static QoreNode *QCALENDARWIDGET_maximumDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qcw->qobj->maximumDate();
   return new QoreNode(new DateTime(rv_date.year(), rv_date.month(), rv_date.day()));
}

//QDate minimumDate () const
static QoreNode *QCALENDARWIDGET_minimumDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qcw->qobj->minimumDate();
   return new QoreNode(new DateTime(rv_date.year(), rv_date.month(), rv_date.day()));
}

//int monthShown () const
static QoreNode *QCALENDARWIDGET_monthShown(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->monthShown());
}

//QDate selectedDate () const
static QoreNode *QCALENDARWIDGET_selectedDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qcw->qobj->selectedDate();
   return new QoreNode(new DateTime(rv_date.year(), rv_date.month(), rv_date.day()));
}

//SelectionMode selectionMode () const
static QoreNode *QCALENDARWIDGET_selectionMode(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->selectionMode());
}

//void setDateEditAcceptDelay ( int delay )
static QoreNode *QCALENDARWIDGET_setDateEditAcceptDelay(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int delay = p ? p->getAsInt() : 0;
   qcw->qobj->setDateEditAcceptDelay(delay);
   return 0;
}

//void setDateEditEnabled ( bool enable )
static QoreNode *QCALENDARWIDGET_setDateEditEnabled(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qcw->qobj->setDateEditEnabled(enable);
   return 0;
}

//void setDateTextFormat ( const QDate & date, const QTextCharFormat & format )
static QoreNode *QCALENDARWIDGET_setDateTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDate date;
   if (get_qdate(p, date, xsink))
      return 0;
   p = get_param(params, 1);
   QoreQTextCharFormat *format = (p && p->type == NT_OBJECT) ? (QoreQTextCharFormat *)p->val.object->getReferencedPrivateData(CID_QTEXTCHARFORMAT, xsink) : 0;
   if (!format) {
      if (!xsink->isException())
         xsink->raiseException("QCALENDARWIDGET-SETDATETEXTFORMAT-PARAM-ERROR", "expecting a QTextCharFormat object as second argument to QCalendarWidget::setDateTextFormat()");
      return 0;
   }
   ReferenceHolder<QoreQTextCharFormat> formatHolder(format, xsink);
   qcw->qobj->setDateTextFormat(date, *(static_cast<QTextCharFormat *>(format)));
   return 0;
}

//void setFirstDayOfWeek ( Qt::DayOfWeek dayOfWeek )
static QoreNode *QCALENDARWIDGET_setFirstDayOfWeek(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DayOfWeek dayOfWeek = (Qt::DayOfWeek)(p ? p->getAsInt() : 0);
   qcw->qobj->setFirstDayOfWeek(dayOfWeek);
   return 0;
}

//void setHeaderTextFormat ( const QTextCharFormat & format )
static QoreNode *QCALENDARWIDGET_setHeaderTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQTextCharFormat *format = (p && p->type == NT_OBJECT) ? (QoreQTextCharFormat *)p->val.object->getReferencedPrivateData(CID_QTEXTCHARFORMAT, xsink) : 0;
   if (!format) {
      if (!xsink->isException())
         xsink->raiseException("QCALENDARWIDGET-SETHEADERTEXTFORMAT-PARAM-ERROR", "expecting a QTextCharFormat object as first argument to QCalendarWidget::setHeaderTextFormat()");
      return 0;
   }
   ReferenceHolder<QoreQTextCharFormat> formatHolder(format, xsink);
   qcw->qobj->setHeaderTextFormat(*(static_cast<QTextCharFormat *>(format)));
   return 0;
}

//void setHorizontalHeaderFormat ( HorizontalHeaderFormat format )
static QoreNode *QCALENDARWIDGET_setHorizontalHeaderFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QCalendarWidget::HorizontalHeaderFormat format = (QCalendarWidget::HorizontalHeaderFormat)(p ? p->getAsInt() : 0);
   qcw->qobj->setHorizontalHeaderFormat(format);
   return 0;
}

//void setMaximumDate ( const QDate & date )
static QoreNode *QCALENDARWIDGET_setMaximumDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDate date;
   if (get_qdate(p, date, xsink))
      return 0;
   qcw->qobj->setMaximumDate(date);
   return 0;
}

//void setMinimumDate ( const QDate & date )
static QoreNode *QCALENDARWIDGET_setMinimumDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDate date;
   if (get_qdate(p, date, xsink))
      return 0;
   qcw->qobj->setMinimumDate(date);
   return 0;
}

//void setSelectionMode ( SelectionMode mode )
static QoreNode *QCALENDARWIDGET_setSelectionMode(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QCalendarWidget::SelectionMode mode = (QCalendarWidget::SelectionMode)(p ? p->getAsInt() : 0);
   qcw->qobj->setSelectionMode(mode);
   return 0;
}

//void setVerticalHeaderFormat ( VerticalHeaderFormat format )
static QoreNode *QCALENDARWIDGET_setVerticalHeaderFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QCalendarWidget::VerticalHeaderFormat format = (QCalendarWidget::VerticalHeaderFormat)(p ? p->getAsInt() : 0);
   qcw->qobj->setVerticalHeaderFormat(format);
   return 0;
}

//void setWeekdayTextFormat ( Qt::DayOfWeek dayOfWeek, const QTextCharFormat & format )
static QoreNode *QCALENDARWIDGET_setWeekdayTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DayOfWeek dayOfWeek = (Qt::DayOfWeek)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQTextCharFormat *format = (p && p->type == NT_OBJECT) ? (QoreQTextCharFormat *)p->val.object->getReferencedPrivateData(CID_QTEXTCHARFORMAT, xsink) : 0;
   if (!format) {
      if (!xsink->isException())
         xsink->raiseException("QCALENDARWIDGET-SETWEEKDAYTEXTFORMAT-PARAM-ERROR", "expecting a QTextCharFormat object as second argument to QCalendarWidget::setWeekdayTextFormat()");
      return 0;
   }
   ReferenceHolder<QoreQTextCharFormat> formatHolder(format, xsink);
   qcw->qobj->setWeekdayTextFormat(dayOfWeek, *(static_cast<QTextCharFormat *>(format)));
   return 0;
}

//VerticalHeaderFormat verticalHeaderFormat () const
static QoreNode *QCALENDARWIDGET_verticalHeaderFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->verticalHeaderFormat());
}

//QTextCharFormat weekdayTextFormat ( Qt::DayOfWeek dayOfWeek ) const
static QoreNode *QCALENDARWIDGET_weekdayTextFormat(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::DayOfWeek dayOfWeek = (Qt::DayOfWeek)(p ? p->getAsInt() : 0);
   QoreObject *o_qtcf = new QoreObject(QC_QTextCharFormat, getProgram());
   QoreQTextCharFormat *q_qtcf = new QoreQTextCharFormat(qcw->qobj->weekdayTextFormat(dayOfWeek));
   o_qtcf->setPrivate(CID_QTEXTCHARFORMAT, q_qtcf);
   return new QoreNode(o_qtcf);
}

//int yearShown () const
static QoreNode *QCALENDARWIDGET_yearShown(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qcw->qobj->yearShown());
}

//void setCurrentPage ( int year, int month )
static QoreNode *QCALENDARWIDGET_setCurrentPage(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int year = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int month = p ? p->getAsInt() : 0;
   qcw->qobj->setCurrentPage(year, month);
   return 0;
}

//void setDateRange ( const QDate & min, const QDate & max )
static QoreNode *QCALENDARWIDGET_setDateRange(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDate min;
   if (get_qdate(p, min, xsink))
      return 0;
   p = get_param(params, 1);
   QDate max;
   if (get_qdate(p, max, xsink))
      return 0;
   qcw->qobj->setDateRange(min, max);
   return 0;
}

//void setGridVisible ( bool show )
static QoreNode *QCALENDARWIDGET_setGridVisible(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool show = p ? p->getAsBool() : false;
   qcw->qobj->setGridVisible(show);
   return 0;
}

//void setNavigationBarVisible ( bool visible )
static QoreNode *QCALENDARWIDGET_setNavigationBarVisible(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : false;
   qcw->qobj->setNavigationBarVisible(visible);
   return 0;
}

//void setSelectedDate ( const QDate & date )
static QoreNode *QCALENDARWIDGET_setSelectedDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QDate date;
   if (get_qdate(p, date, xsink))
      return 0;
   qcw->qobj->setSelectedDate(date);
   return 0;
}

//void showNextMonth ()
static QoreNode *QCALENDARWIDGET_showNextMonth(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showNextMonth();
   return 0;
}

//void showNextYear ()
static QoreNode *QCALENDARWIDGET_showNextYear(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showNextYear();
   return 0;
}

//void showPreviousMonth ()
static QoreNode *QCALENDARWIDGET_showPreviousMonth(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showPreviousMonth();
   return 0;
}

//void showPreviousYear ()
static QoreNode *QCALENDARWIDGET_showPreviousYear(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showPreviousYear();
   return 0;
}

//void showSelectedDate ()
static QoreNode *QCALENDARWIDGET_showSelectedDate(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showSelectedDate();
   return 0;
}

//void showToday ()
static QoreNode *QCALENDARWIDGET_showToday(QoreObject *self, QoreQCalendarWidget *qcw, QoreNode *params, ExceptionSink *xsink)
{
   qcw->qobj->showToday();
   return 0;
}

QoreClass *initQCalendarWidgetClass(QoreClass *parent)
{
   QC_QCalendarWidget = new QoreClass("QCalendarWidget", QDOM_GUI);
   CID_QCALENDARWIDGET = QC_QCalendarWidget->getID();

   QC_QCalendarWidget->addBuiltinVirtualBaseClass(parent);

   QC_QCalendarWidget->setConstructor(QCALENDARWIDGET_constructor);
   QC_QCalendarWidget->setCopy((q_copy_t)QCALENDARWIDGET_copy);

   QC_QCalendarWidget->addMethod("dateEditAcceptDelay",         (q_method_t)QCALENDARWIDGET_dateEditAcceptDelay);
   //QC_QCalendarWidget->addMethod("dateTextFormat",              (q_method_t)QCALENDARWIDGET_dateTextFormat);
   QC_QCalendarWidget->addMethod("firstDayOfWeek",              (q_method_t)QCALENDARWIDGET_firstDayOfWeek);
   QC_QCalendarWidget->addMethod("headerTextFormat",            (q_method_t)QCALENDARWIDGET_headerTextFormat);
   QC_QCalendarWidget->addMethod("horizontalHeaderFormat",      (q_method_t)QCALENDARWIDGET_horizontalHeaderFormat);
   QC_QCalendarWidget->addMethod("isDateEditEnabled",           (q_method_t)QCALENDARWIDGET_isDateEditEnabled);
   QC_QCalendarWidget->addMethod("isGridVisible",               (q_method_t)QCALENDARWIDGET_isGridVisible);
   QC_QCalendarWidget->addMethod("isNavigationBarVisible",      (q_method_t)QCALENDARWIDGET_isNavigationBarVisible);
   QC_QCalendarWidget->addMethod("maximumDate",                 (q_method_t)QCALENDARWIDGET_maximumDate);
   QC_QCalendarWidget->addMethod("minimumDate",                 (q_method_t)QCALENDARWIDGET_minimumDate);
   QC_QCalendarWidget->addMethod("monthShown",                  (q_method_t)QCALENDARWIDGET_monthShown);
   QC_QCalendarWidget->addMethod("selectedDate",                (q_method_t)QCALENDARWIDGET_selectedDate);
   QC_QCalendarWidget->addMethod("selectionMode",               (q_method_t)QCALENDARWIDGET_selectionMode);
   QC_QCalendarWidget->addMethod("setDateEditAcceptDelay",      (q_method_t)QCALENDARWIDGET_setDateEditAcceptDelay);
   QC_QCalendarWidget->addMethod("setDateEditEnabled",          (q_method_t)QCALENDARWIDGET_setDateEditEnabled);
   QC_QCalendarWidget->addMethod("setDateTextFormat",           (q_method_t)QCALENDARWIDGET_setDateTextFormat);
   QC_QCalendarWidget->addMethod("setFirstDayOfWeek",           (q_method_t)QCALENDARWIDGET_setFirstDayOfWeek);
   QC_QCalendarWidget->addMethod("setHeaderTextFormat",         (q_method_t)QCALENDARWIDGET_setHeaderTextFormat);
   QC_QCalendarWidget->addMethod("setHorizontalHeaderFormat",   (q_method_t)QCALENDARWIDGET_setHorizontalHeaderFormat);
   QC_QCalendarWidget->addMethod("setMaximumDate",              (q_method_t)QCALENDARWIDGET_setMaximumDate);
   QC_QCalendarWidget->addMethod("setMinimumDate",              (q_method_t)QCALENDARWIDGET_setMinimumDate);
   QC_QCalendarWidget->addMethod("setSelectionMode",            (q_method_t)QCALENDARWIDGET_setSelectionMode);
   QC_QCalendarWidget->addMethod("setVerticalHeaderFormat",     (q_method_t)QCALENDARWIDGET_setVerticalHeaderFormat);
   QC_QCalendarWidget->addMethod("setWeekdayTextFormat",        (q_method_t)QCALENDARWIDGET_setWeekdayTextFormat);
   QC_QCalendarWidget->addMethod("verticalHeaderFormat",        (q_method_t)QCALENDARWIDGET_verticalHeaderFormat);
   QC_QCalendarWidget->addMethod("weekdayTextFormat",           (q_method_t)QCALENDARWIDGET_weekdayTextFormat);
   QC_QCalendarWidget->addMethod("yearShown",                   (q_method_t)QCALENDARWIDGET_yearShown);
   QC_QCalendarWidget->addMethod("setCurrentPage",              (q_method_t)QCALENDARWIDGET_setCurrentPage);
   QC_QCalendarWidget->addMethod("setDateRange",                (q_method_t)QCALENDARWIDGET_setDateRange);
   QC_QCalendarWidget->addMethod("setGridVisible",              (q_method_t)QCALENDARWIDGET_setGridVisible);
   QC_QCalendarWidget->addMethod("setNavigationBarVisible",     (q_method_t)QCALENDARWIDGET_setNavigationBarVisible);
   QC_QCalendarWidget->addMethod("setSelectedDate",             (q_method_t)QCALENDARWIDGET_setSelectedDate);
   QC_QCalendarWidget->addMethod("showNextMonth",               (q_method_t)QCALENDARWIDGET_showNextMonth);
   QC_QCalendarWidget->addMethod("showNextYear",                (q_method_t)QCALENDARWIDGET_showNextYear);
   QC_QCalendarWidget->addMethod("showPreviousMonth",           (q_method_t)QCALENDARWIDGET_showPreviousMonth);
   QC_QCalendarWidget->addMethod("showPreviousYear",            (q_method_t)QCALENDARWIDGET_showPreviousYear);
   QC_QCalendarWidget->addMethod("showSelectedDate",            (q_method_t)QCALENDARWIDGET_showSelectedDate);
   QC_QCalendarWidget->addMethod("showToday",                   (q_method_t)QCALENDARWIDGET_showToday);

   return QC_QCalendarWidget;
}
