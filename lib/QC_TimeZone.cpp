/*
  QC_TimeZone.cpp
  
  Qore Programming Language
  
  Copyright 2003 - 2011 David Nichols

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
#include <qore/intern/QC_TimeZone.h>

qore_classid_t CID_TIMEZONE;
QoreClass *QC_TIMEZONE;

// TimeZone::constructor(string $region)
static void TIMEZONE_constructor_str(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *region = HARD_QORE_STRING(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findLoadRegion(region->getBuffer(), xsink);
   if (*xsink)
      return;

   self->setPrivate(CID_TIMEZONE, new TimeZoneData(zone));
}

// TimeZone::constructor(softint $seconds_east)
static void TIMEZONE_constructor_int(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   int seconds_east = (int)HARD_QORE_INT(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(zone));
}

static void TIMEZONE_copy(QoreObject *self, QoreObject *old, TimeZoneData *z, ExceptionSink *xsink) {
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(*z));
}

// int TimeZone::UTCOffset()  
static AbstractQoreNode *TZ_UTCOffset(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode((*z)->getUTCOffset());
}

// bool TimeZone::hasDST()  
static AbstractQoreNode *TZ_hasDST(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node((*z)->hasDST());
}

// string TimeZone::region()  
static AbstractQoreNode *TZ_region(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode((*z)->getRegionName());
}

// date TimeZone::date(softint, softint = 0)  
static AbstractQoreNode *TZ_date_int(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return DateTimeNode::makeAbsolute(z->get(), HARD_QORE_INT(params, 0), (int)HARD_QORE_INT(params, 1));
}

// date TimeZone::date(date)  
static AbstractQoreNode *TZ_date_date(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   const DateTimeNode *d = HARD_QORE_DATE(params, 0);
   return DateTimeNode::makeAbsolute(z->get(), d->getEpochSecondsUTC(), d->getMicrosecond());
}

// date TimeZone::dateMs(softint)  
static AbstractQoreNode *TZ_dateMs(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   int64 ms = HARD_QORE_INT(params, 0);
   return DateTimeNode::makeAbsolute(z->get(), ms / 1000, (int)((ms % 1000) * 1000));
}

// date TimeZone::dateUs(softint)  
static AbstractQoreNode *TZ_dateUs(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   int64 us = HARD_QORE_INT(params, 0);
   return DateTimeNode::makeAbsolute(z->get(), us / 1000000, (int)(us % 1000000));
}

// static methods
// TimeZone TimeZone::get()  
static AbstractQoreNode *f_TZ_get(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreObject(QC_TIMEZONE, 0, new TimeZoneData(currentTZ()));
}

// nothing TimeZone::set(TimeZone)  
static AbstractQoreNode *f_TZ_set(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(zone, TimeZoneData, params, 0, CID_TIMEZONE, "TimeZone", "TimeZone::setTimeZone", xsink);
   if (*xsink)
      return 0;
   getProgram()->setTZ(zone->get());
   return 0;
}

// nothing TimeZone::setUTCOffset(int $seconds_offset)  
static AbstractQoreNode *f_TZ_setUTCOffset(const QoreListNode *params, ExceptionSink *xsink) {
   int seconds_east = (int)HARD_QORE_INT(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   getProgram()->setTZ(zone);
   return 0;
}

// nothing TimeZone::setRegion(string $region)  
static AbstractQoreNode *f_TZ_setRegion(const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findLoadRegion(str->getBuffer(), xsink);
   if (!*xsink)
      getProgram()->setTZ(zone);
   return 0;
}

QoreClass *initTimeZoneClass() {
   QORE_TRACE("initTimeZoneClass()");

   // note that this class does not block therefore has no QDOM_THREAD
   QC_TIMEZONE = new QoreClass("TimeZone", QDOM_TERMINAL_IO);
   CID_TIMEZONE = QC_TIMEZONE->getID();

   QC_TIMEZONE->setConstructorExtended(TIMEZONE_constructor_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_TIMEZONE->setConstructorExtended(TIMEZONE_constructor_int, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   QC_TIMEZONE->setCopy((q_copy_t)TIMEZONE_copy);

   // int TimeZone::UTCOffset()  
   QC_TIMEZONE->addMethodExtended("UTCOffset", (q_method_t)TZ_UTCOffset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // bool TimeZone::hasDST()  
   QC_TIMEZONE->addMethodExtended("hasDST", (q_method_t)TZ_hasDST, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // string TimeZone::region()  
   QC_TIMEZONE->addMethodExtended("region", (q_method_t)TZ_region, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   // date TimeZone::date(softint $secs, softint $us = 0)  
   QC_TIMEZONE->addMethodExtended("date", (q_method_t)TZ_date_int, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, dateTypeInfo, 2, softBigIntTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, zero());

   // date TimeZone::date(date)  
   QC_TIMEZONE->addMethodExtended("date", (q_method_t)TZ_date_date, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, QORE_PARAM_NO_ARG);

   // date TimeZone::dateMs(softint)  
   QC_TIMEZONE->addMethodExtended("dateMs", (q_method_t)TZ_dateMs, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // date TimeZone::dateUs(softint)  
   QC_TIMEZONE->addMethodExtended("dateUs", (q_method_t)TZ_dateUs, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // static methods
   // TimeZone TimeZone::get()  
   QC_TIMEZONE->addStaticMethodExtended("get", f_TZ_get, false, QC_CONSTANT, QDOM_DEFAULT, QC_TIMEZONE->getTypeInfo());

   // nothing TimeZone::set(TimeZone)  
   QC_TIMEZONE->addStaticMethodExtended("set", f_TZ_set, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, QC_TIMEZONE->getTypeInfo(), QORE_PARAM_NO_ARG);

   // nothing TimeZone::setUTCOffset(int $seconds_offset)  
   QC_TIMEZONE->addStaticMethodExtended("setUTCOffset", f_TZ_setUTCOffset, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // nothing TimeZone::setRegion(string $region)  
   QC_TIMEZONE->addStaticMethodExtended("setRegion", f_TZ_setRegion, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_TIMEZONE;
}
