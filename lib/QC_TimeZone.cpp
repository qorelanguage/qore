/*
  QC_TimeZone.cpp
  
  Qore Programming Language
  
  Copyright 2003 - 2010 David Nichols

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
   int seconds_east = HARD_QORE_INT(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(zone));
}

static void TIMEZONE_copy(QoreObject *self, QoreObject *old, TimeZoneData *z, ExceptionSink *xsink) {
   self->setPrivate(CID_TIMEZONE, new TimeZoneData(*z));
}

// TimeZone::getUTCOffset() returns int
static AbstractQoreNode *TZ_UTCOffset(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode((*z)->getGMTOffset());
}

// TimeZone::hasDST() returns bool
static AbstractQoreNode *TZ_hasDST(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return get_bool_node((*z)->hasDST());
}

// TimeZone::region() returns string
static AbstractQoreNode *TZ_region(QoreObject *self, TimeZoneData *z, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreStringNode((*z)->getRegionName());
}

// static methods
// TimeZone::get() returns TimeZone
static AbstractQoreNode *f_TZ_get(const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreObject(QC_TIMEZONE, 0, new TimeZoneData(currentTZ()));
}

// TimeZone::set(TimeZone) returns nothing
static AbstractQoreNode *f_TZ_set(const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(zone, TimeZoneData, params, 0, CID_TIMEZONE, "TimeZone", "TimeZone::setTimeZone", xsink);
   if (*xsink)
      return 0;
   getProgram()->setTZ(zone->get());
   return 0;
}

// TimeZone::setUTCOffset(int $seconds_offset) returns nothing
static AbstractQoreNode *f_TZ_setUTCOffset(const QoreListNode *params, ExceptionSink *xsink) {
   int seconds_east = HARD_QORE_INT(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   getProgram()->setTZ(zone);
   return 0;
}

// TimeZone::setRegion(string $region) returns nothing
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

   // TimeZone::getUTCOffset() returns int
   QC_TIMEZONE->addMethodExtended("UTCOffset", (q_method_t)TZ_UTCOffset, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // TimeZone::hasDST() returns bool
   QC_TIMEZONE->addMethodExtended("hasDST", (q_method_t)TZ_hasDST, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo);

   // TimeZone::region() returns string
   QC_TIMEZONE->addMethodExtended("region", (q_method_t)TZ_region, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, stringTypeInfo);

   // static methods
   // TimeZone::get() returns TimeZone
   QC_TIMEZONE->addStaticMethodExtended("get", f_TZ_get, false, QC_CONSTANT, QDOM_DEFAULT, QC_TIMEZONE->getTypeInfo());

   // TimeZone::set(TimeZone) returns nothing
   QC_TIMEZONE->addStaticMethodExtended("set", f_TZ_set, false, QC_NO_FLAGS, QDOM_LOCALE, nothingTypeInfo, 1, QC_TIMEZONE->getTypeInfo(), QORE_PARAM_NO_ARG);

   // TimeZone::setUTCOffset(int $seconds_offset) returns nothing
   QC_TIMEZONE->addStaticMethodExtended("setUTCOffset", f_TZ_setUTCOffset, false, QC_NO_FLAGS, QDOM_LOCALE, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // TimeZone::setRegion(string $region) returns nothing
   QC_TIMEZONE->addStaticMethodExtended("setRegion", f_TZ_setRegion, false, QC_NO_FLAGS, QDOM_LOCALE, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   return QC_TIMEZONE;
}
