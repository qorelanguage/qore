/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_TimeZone.qpp
  
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

/* Qore class Qore::TimeZone */

qore_classid_t CID_TIMEZONE;
QoreClass *QC_TIMEZONE;

// int TimeZone::UTCOffset() {}
static int64 TimeZone_UTCOffset(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   return (*z)->getUTCOffset();
}

// TimeZone::constructor(string region) {}
static void TimeZone_constructor(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* region = HARD_QORE_STRING(args, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findLoadRegion(region->getBuffer(), xsink);
   if (*xsink)
      return;

   self->setPrivate(CID_TIMEZONE, new TimeZoneData(zone));
}

// TimeZone::constructor(softint seconds_east) {}
static void TimeZone_constructor_1(QoreObject* self, const QoreListNode* args, ExceptionSink* xsink) {
   int64 seconds_east = HARD_QORE_INT(args, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(zone));
}

// TimeZone::copy() {}
static void TimeZone_copy(QoreObject* self, QoreObject* old, TimeZoneData* z, ExceptionSink* xsink) {
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(*z));
}

// date TimeZone::date(softint secs, softint us = 0) {}
static AbstractQoreNode* TimeZone_date(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   int64 secs = HARD_QORE_INT(args, 0);
   int64 us = HARD_QORE_INT(args, 1);
   return DateTimeNode::makeAbsolute(z->get(), secs, (int)us);
}

// date TimeZone::date(date d) {}
static AbstractQoreNode* TimeZone_date_1(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   const DateTimeNode* d = HARD_QORE_DATE(args, 0);
   return DateTimeNode::makeAbsolute(z->get(), d->getEpochSecondsUTC(), d->getMicrosecond());
}

// date TimeZone::dateMs(softint ms) {}
static AbstractQoreNode* TimeZone_dateMs(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   int64 ms = HARD_QORE_INT(args, 0);
   return DateTimeNode::makeAbsolute(z->get(), ms / 1000, (int)((ms % 1000) * 1000));
}

// date TimeZone::dateUs(softint us) {}
static AbstractQoreNode* TimeZone_dateUs(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   int64 us = HARD_QORE_INT(args, 0);
   return DateTimeNode::makeAbsolute(z->get(), us / 1000000, (int)(us % 1000000));
}

// bool TimeZone::hasDST() {}
static bool TimeZone_hasDST(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   return (*z)->hasDST();
}

// string TimeZone::region() {}
static AbstractQoreNode* TimeZone_region(QoreObject* self, TimeZoneData* z, const QoreListNode* args, ExceptionSink* xsink) {
   return new QoreStringNode((*z)->getRegionName());
}

// static TimeZone TimeZone::get() {}
static QoreObject* static_TimeZone_get(const QoreListNode* args, ExceptionSink* xsink) {
   return new QoreObject(QC_TIMEZONE, 0, new TimeZoneData(currentTZ()));
}

// static nothing TimeZone::set(TimeZone zone) {}
static AbstractQoreNode* static_TimeZone_set(const QoreListNode* args, ExceptionSink* xsink) {
   HARD_QORE_OBJ_DATA(zone, TimeZoneData, args, 0, CID_TIMEZONE, "TimeZone::set()", "TimeZone", xsink);
   if (*xsink)
      return 0;
   ReferenceHolder<TimeZoneData> holder(zone, xsink);
   getProgram()->setTZ(zone->get());
   return 0;
}

// static nothing TimeZone::setRegion(string region) {}
static AbstractQoreNode* static_TimeZone_setRegion(const QoreListNode* args, ExceptionSink* xsink) {
   const QoreStringNode* region = HARD_QORE_STRING(args, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findLoadRegion(region->getBuffer(), xsink);
   if (!*xsink)
      getProgram()->setTZ(zone);
   return 0;
}

// static nothing TimeZone::setUTCOffset(softint seconds_east) {}
static AbstractQoreNode* static_TimeZone_setUTCOffset(const QoreListNode* args, ExceptionSink* xsink) {
   int64 seconds_east = HARD_QORE_INT(args, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   getProgram()->setTZ(zone);
   return 0;
}

QoreClass* initTimeZoneClass(QoreNamespace &qorens) {
   QC_TIMEZONE = new QoreClass("TimeZone");
   CID_TIMEZONE = QC_TIMEZONE->getID();

   // int TimeZone::UTCOffset() {}
   QC_TIMEZONE->addMethodExtended("UTCOffset", (q_method_int64_t)TimeZone_UTCOffset, false, QC_CONSTANT, QDOM_DEFAULT, bigIntTypeInfo);

   // TimeZone::constructor(string region) {}
   QC_TIMEZONE->setConstructorExtended(TimeZone_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, stringTypeInfo, NULL);

   // TimeZone::constructor(softint seconds_east) {}
   QC_TIMEZONE->setConstructorExtended(TimeZone_constructor_1, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBigIntTypeInfo, NULL);

   // TimeZone::copy() {}
   QC_TIMEZONE->setCopy((q_copy_t)TimeZone_copy);

   // date TimeZone::date(softint secs, softint us = 0) {}
   QC_TIMEZONE->addMethodExtended("date", (q_method_t)TimeZone_date, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 2, softBigIntTypeInfo, NULL, softBigIntTypeInfo, zero());

   // date TimeZone::date(date d) {}
   QC_TIMEZONE->addMethodExtended("date", (q_method_t)TimeZone_date_1, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, dateTypeInfo, NULL);

   // date TimeZone::dateMs(softint ms) {}
   QC_TIMEZONE->addMethodExtended("dateMs", (q_method_t)TimeZone_dateMs, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, NULL);

   // date TimeZone::dateUs(softint us) {}
   QC_TIMEZONE->addMethodExtended("dateUs", (q_method_t)TimeZone_dateUs, false, QC_CONSTANT, QDOM_DEFAULT, dateTypeInfo, 1, softBigIntTypeInfo, NULL);

   // bool TimeZone::hasDST() {}
   QC_TIMEZONE->addMethodExtended("hasDST", (q_method_bool_t)TimeZone_hasDST, false, QC_CONSTANT, QDOM_DEFAULT, boolTypeInfo);

   // string TimeZone::region() {}
   QC_TIMEZONE->addMethodExtended("region", (q_method_t)TimeZone_region, false, QC_CONSTANT, QDOM_DEFAULT, stringTypeInfo);

   // static TimeZone TimeZone::get() {}
   QC_TIMEZONE->addStaticMethodExtended("get", (q_func_t)static_TimeZone_get, false, QC_CONSTANT, QDOM_DEFAULT, QC_TIMEZONE->getTypeInfo());

   // static nothing TimeZone::set(TimeZone zone) {}
   QC_TIMEZONE->addStaticMethodExtended("set", (q_func_t)static_TimeZone_set, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, QC_TIMEZONE->getTypeInfo(), NULL);

   // static nothing TimeZone::setRegion(string region) {}
   QC_TIMEZONE->addStaticMethodExtended("setRegion", (q_func_t)static_TimeZone_setRegion, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, stringTypeInfo, NULL);

   // static nothing TimeZone::setUTCOffset(softint seconds_east) {}
   QC_TIMEZONE->addStaticMethodExtended("setUTCOffset", (q_func_t)static_TimeZone_setUTCOffset, false, QC_NO_FLAGS, QDOM_LOCALE_CONTROL, nothingTypeInfo, 1, softBigIntTypeInfo, NULL);

   return QC_TIMEZONE;
}
