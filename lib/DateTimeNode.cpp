/*
  DateTimeNode.cpp

  DateTimeNode Class Definition

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

DateTimeNode::DateTimeNode(qore_date_private *n_priv) : SimpleValueQoreNode(NT_DATE), DateTime(n_priv) {
}

DateTimeNode::DateTimeNode(bool r) : SimpleValueQoreNode(NT_DATE), DateTime(r) {
}

DateTimeNode::~DateTimeNode() {
}

DateTimeNode::DateTimeNode(int y, int mo, int d, int h, int mi, int s, short ms, bool r) : SimpleValueQoreNode(NT_DATE), DateTime(y, mo, d, h, mi, s, ms, r) {
}

DateTimeNode::DateTimeNode(const AbstractQoreZoneInfo *z, int64 seconds, int n_us) : SimpleValueQoreNode(NT_DATE), DateTime(z, seconds, n_us) {
}

DateTimeNode::DateTimeNode(int64 seconds) : SimpleValueQoreNode(NT_DATE), DateTime(seconds) {
}

DateTimeNode::DateTimeNode(int64 seconds, int ms) : SimpleValueQoreNode(NT_DATE), DateTime(seconds, ms) {
}

DateTimeNode::DateTimeNode(const char *date) : SimpleValueQoreNode(NT_DATE), DateTime(date) {
}

DateTimeNode::DateTimeNode(struct tm *tms) : SimpleValueQoreNode(NT_DATE), DateTime(tms) {
}

DateTimeNode::DateTimeNode(const DateTime &dt) : SimpleValueQoreNode(NT_DATE), DateTime(dt) {
}

DateTimeNode::DateTimeNode(const DateTimeNode &dt) : SimpleValueQoreNode(NT_DATE), DateTime(dt) {
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *DateTimeNode::getStringRepresentation(bool &del) const {
   del = true;
   return new QoreString(this);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void DateTimeNode::getStringRepresentation(QoreString &str) const {
   str.concat(this);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
DateTime *DateTimeNode::getDateTimeRepresentation(bool &del) const {
   del = false;
   return const_cast<DateTimeNode *>(this);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void DateTimeNode::getDateTimeRepresentation(DateTime &dt) const {
   dt.setDate(*this);
}

bool DateTimeNode::getAsBoolImpl() const {
   return getEpochSeconds() ? true : false;
}

int DateTimeNode::getAsIntImpl() const {
   return getEpochSeconds();
}

int64 DateTimeNode::getAsBigIntImpl() const {
   return getEpochSeconds();
}

double DateTimeNode::getAsFloatImpl() const {
   return (double)getEpochSeconds();
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *DateTimeNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const {
   del = true;
   QoreString *str = new QoreString;
   getAsString(*str, foff, xsink);
   return str;
}

int DateTimeNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const {
   priv->getAsString(str);
   return 0;
}

AbstractQoreNode *DateTimeNode::realCopy() const {
   return new DateTimeNode(*this);
}

bool DateTimeNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   DateTimeValueHelper date(v);
   return isEqual(*date);
}

bool DateTimeNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
   const DateTimeNode *date = dynamic_cast<const DateTimeNode *>(v);
   if (!date)
      return false;

   return isEqual(date);
}

const char *DateTimeNode::getTypeName() const {
   return getStaticTypeName();
}

DateTimeNode *DateTimeNode::copy() const {
   return new DateTimeNode(*this);
}

DateTimeNode *DateTimeNode::add(const DateTime *dt) const {
   assert(dt);
   DateTimeNode *rv;
   if (isRelative()) {
      rv = new DateTimeNode(*dt);
      rv->priv->add(*priv);
   }
   else {
      rv = new DateTimeNode(*this);
      rv->priv->add(*dt->priv);
   }
   return rv;
}

DateTimeNode *DateTimeNode::subtractBy(const DateTime *dt) const {
   DateTimeNode *rv;
   if (isRelative()) {
      rv = new DateTimeNode(*dt);
      rv->priv->subtractBy(*priv);
   }
   else {
      rv = new DateTimeNode(*this);
      rv->priv->subtractBy(*dt->priv);
   }
   return rv;
}

// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// a NULL return value means an exception was raised
// static method
DateTimeNode *DateTimeNode::getDateFromISOWeek(int year, int week, int day, ExceptionSink *xsink) {
   SimpleRefHolder<DateTimeNode> rv(new DateTimeNode());
   if (qore_date_private::getDateFromISOWeek(*rv->priv, year, week, day, xsink))
      return 0;
   return rv.release();
}

DateTimeNode *DateTimeNode::unaryMinus() const {
   DateTimeNode *rv = new DateTimeNode(*this);
   rv->priv->unaryMinus();
   return rv;
}

DateTimeNode *DateTimeNode::makeAbsolute(const AbstractQoreZoneInfo *z, int y, int mo, int d, int h, int mi, int s, int u) {
   return new DateTimeNode(new qore_date_private(z, y, mo, d, h, mi, s, u));
}

DateTimeNode *DateTimeNode::makeAbsolute(const AbstractQoreZoneInfo *zone, int64 seconds, int us) {
   return new DateTimeNode(new qore_date_private(zone, seconds, us));
}

DateTimeNode *DateTimeNode::makeRelative(int y, int mo, int d, int h, int mi, int s, int u) {
   return new DateTimeNode(new qore_date_private(y, mo, d, h, mi, s, u, true));
}
