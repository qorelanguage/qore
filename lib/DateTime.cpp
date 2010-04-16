/*
  DateTime.cpp

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
#include <qore/intern/qore_date_private.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

DateTime::DateTime(qore_date_private *n_priv) : priv(n_priv) {
}

DateTime::DateTime(bool r) : priv(new qore_date_private(r)) {
}

DateTime::DateTime(const DateTime &dt) : priv(new qore_date_private(*dt.priv)) {
}

DateTime::DateTime(int y, int mo, int d, int h, int mi, int s, short ms, bool r) : priv(new qore_date_private(y, mo, d, h, mi, s, (int)ms * 1000, r)) {
}

DateTime::DateTime(const struct tm *tms) : priv(new qore_date_private) {
   setDate(tms);
}

DateTime::DateTime(int64 seconds) : priv(new qore_date_private) {
   setDate(seconds);
}

DateTime::DateTime(int64 seconds, int ms) : priv(new qore_date_private) {
   setDate(seconds, ms);
}

DateTime::DateTime(const char *str) : priv(new qore_date_private) {
   setDate(str);
}

DateTime::~DateTime() {
   delete priv;
}

bool DateTime::isRelative() const {
   return priv->isRelative();
}

bool DateTime::isAbsolute() const {
   return !priv->isRelative();
}

short DateTime::getYear() const {
   return priv->getYear();
}

int DateTime::getMonth() const {
   return priv->getMonth();
}

int DateTime::getDay() const {
   return priv->getDay();
}

int DateTime::getHour() const {
   return priv->getHour();
}

int DateTime::getMinute() const {
   return priv->getMinute();
}

int DateTime::getSecond() const {
   return priv->getSecond();
}

int DateTime::getMillisecond() const {
   return priv->getMillisecond();
}

void DateTime::setTime(int h, int m, int s, short ms) {
   priv->setTime(h, m, s, (int)ms * 1000);
}

int DateTime::getDayNumber() const {
   return priv->getDayNumber();
}

int DateTime::getDayOfWeek() const {
   return priv->getDayOfWeek();
}

void DateTime::format(QoreString &str, const char *fmt) const {
   return priv->format(str, fmt);
}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch) plus milliseconds
void DateTime::setDate(int64 seconds, int ms) {
   priv->setDate(seconds, ms * 1000);
}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch) plus microseconds
void DateTime::setDate(const AbstractQoreZoneInfo *n_zone, int64 seconds, int us) {
   priv->setDate(n_zone, seconds, us);
}

// set the date from the number of seconds since January 1, 1970 (UNIX epoch)
void DateTime::setDate(int64 seconds) {
   priv->setDate(seconds);
}

void DateTime::setDate(const DateTime &date) {
   priv->setDate(*date.priv);
}

void DateTime::setDate(const AbstractQoreZoneInfo *n_zone, int n_year, int n_month, int n_day, int n_hour, int n_minute, int n_second, int n_us) {
   priv->setDate(n_zone, n_year, n_month, n_day, n_hour, n_minute, n_second, n_us);
}

// get the number of seconds before or after January 1, 1970 (UNIX epoch)
int64 DateTime::getEpochSeconds() const {
   return priv->getEpochSeconds();
}

void DateTime::setDateLiteral(int64 date) {
   priv->setDateLiteral(date);
}

void DateTime::setRelativeDateLiteral(int64 date) {
   priv->setRelativeDateLiteral(date);
}

// return the ISO-8601 calendar week information - note that the ISO-8601 calendar year may be different than the actual year
void DateTime::getISOWeek(int &yr, int &week, int &wday) const {
   priv->getISOWeek(yr, week, wday);
}

// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// a NULL return value means an exception was raised
// static method
DateTime *DateTime::getDateFromISOWeek(int year, int week, int day, ExceptionSink *xsink) {
   std::auto_ptr<DateTime> rv(new DateTime);
   if (qore_date_private::getDateFromISOWeek(*rv->priv, year, week, day, xsink)) 
      return 0;
   return rv.release();
}

int DateTime::compareDates(const DateTime *left, const DateTime *right) {
   return qore_date_private::compare(*left->priv, *right->priv);
}

// FIXME: implement and use
bool DateTime::checkValidity() const {
   return true;
}

int64 DateTime::getRelativeSeconds() const {
   return priv->getRelativeSeconds();
}

int64 DateTime::getRelativeMilliseconds() const {
   return priv->getRelativeMilliseconds();
}

// static methods
bool DateTime::isLeapYear(int year) {
   return qore_date_info::isLeapYear(year);
}

// static function
int DateTime::getLastDayOfMonth(int month, int year) {
   return qore_date_info::getLastDayOfMonth(month, year);
}

void DateTime::getTM(struct tm *tms) const {
   return priv->localtime(*tms);
}

void DateTime::setDate(const struct tm *tms, short ms) {
   priv->setDate(*tms, (int)ms * 1000);
}

void DateTime::setDate(const char *str) {
   priv->setDate(str);
}

void DateTime::setRelativeDate(const char *str) {
   priv->setRelativeDate(str);
}

bool DateTime::isEqual(const DateTime *dt) const {
   return priv->isEqual(*dt->priv);
}

DateTime *DateTime::add(const DateTime *dt) const {
   DateTime *rv;
   if (isRelative()) {
      rv = new DateTime(*dt);
      rv->priv->add(*priv);
   }
   else {
      rv = new DateTime(*this);
      rv->priv->add(*dt->priv);
   }
   return rv;
}

DateTime *DateTime::subtractBy(const DateTime *dt) const {
   DateTime *rv;
   if (isRelative()) {
      rv = new DateTime(*dt);
      rv->priv->subtractBy(*priv);
   }
   else {
      rv = new DateTime(*this);
      rv->priv->subtractBy(*dt->priv);
   }
   return rv;
}

bool DateTime::hasValue() const {
   return priv->hasValue();
}

DateTime *DateTime::unaryMinus() const {
   DateTime *rv = new DateTime(*this);
   rv->priv->unaryMinus();
   return rv;
}

void DateTime::unaryMinusInPlace() {
   priv->unaryMinus();
}

void DateTime::getInfo(const AbstractQoreZoneInfo *n_zone, qore_tm &info) const {
   qore_time_info i;
   priv->get(n_zone, i);
   i.copyTo(info);
}

void DateTime::getInfo(qore_tm &info) const {
   qore_time_info i;
   priv->get(i);
   i.copyTo(info);
}

DateTime *DateTime::makeAbsolute(const AbstractQoreZoneInfo *z, int y, int mo, int d, int h, int mi, int s, int u) {
   return new DateTime(new qore_date_private(z, y, mo, d, h, mi, s, u));
}

DateTime *DateTime::makeAbsolute(const AbstractQoreZoneInfo *zone, int64 seconds, int us) {
   return new DateTime(new qore_date_private(zone, seconds, us));
}

DateTime *DateTime::makeRelative(int y, int mo, int d, int h, int mi, int s, int u) {
   return new DateTime(new qore_date_private(y, mo, d, h, mi, s, u, true));
}
