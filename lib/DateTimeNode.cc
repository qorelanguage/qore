/*
  DateTimeNode.cc

  DateTimeNode Class Definition

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

DateTimeNode::DateTimeNode(bool r) : SimpleValueQoreNode(NT_DATE), DateTime(r)
{
}

DateTimeNode::~DateTimeNode()
{
}

DateTimeNode::DateTimeNode(int y, int mo, int d, int h, int mi, int s, short ms, bool r) : SimpleValueQoreNode(NT_DATE), DateTime(y, mo, d, h, mi, s, ms, r)
{
}

DateTimeNode::DateTimeNode(int64 seconds) : SimpleValueQoreNode(NT_DATE), DateTime(seconds)
{
}

DateTimeNode::DateTimeNode(int64 seconds, int ms) : SimpleValueQoreNode(NT_DATE), DateTime(seconds, ms)
{
}

DateTimeNode::DateTimeNode(const char *date) : SimpleValueQoreNode(NT_DATE), DateTime(date)
{
}

DateTimeNode::DateTimeNode(struct tm *tms) : SimpleValueQoreNode(NT_DATE), DateTime(tms)
{
}

DateTimeNode::DateTimeNode(const DateTime &dt) : SimpleValueQoreNode(NT_DATE), DateTime(dt)
{
}

DateTimeNode::DateTimeNode(const DateTimeNode &dt) : SimpleValueQoreNode(NT_DATE), DateTime(dt)
{
}

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *DateTimeNode::getStringRepresentation(bool &del) const
{
   del = true;
   return new QoreString(this);
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void DateTimeNode::getStringRepresentation(QoreString &str) const
{
   str.concat(this);
}

// if del is true, then the returned DateTime * should be deleted, if false, then it should not
class DateTime *DateTimeNode::getDateTimeRepresentation(bool &del) const
{
   del = false;
   return const_cast<DateTimeNode *>(this);
}

// assign date representation to a DateTime (no action for complex types = default implementation)
void DateTimeNode::getDateTimeRepresentation(DateTime &dt) const
{
   dt.setDate(*this);
}

bool DateTimeNode::getAsBoolImpl() const
{
   return getEpochSeconds() ? true : false;
}

int DateTimeNode::getAsIntImpl() const
{
   return getEpochSeconds();
}

int64 DateTimeNode::getAsBigIntImpl() const
{
   return getEpochSeconds();
}

double DateTimeNode::getAsFloatImpl() const
{
   return (double)getEpochSeconds();
}

// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *DateTimeNode::getAsString(bool &del, int foff, ExceptionSink *xsink) const
{
   del = true;
   class QoreString *str = new QoreString();
   getAsString(*str, foff, xsink);
   return str;
}

#define PL(n) (n == 1 ? "" : "s")

int DateTimeNode::getAsString(QoreString &str, int foff, ExceptionSink *xsink) const
{
   if (!priv->relative) {
      format(str, "YYYY-MM-DD HH:mm:SS");
      if (priv->millisecond)
	 str.sprintf(".%03d", priv->millisecond);
      return 0;
   }

   int f = 0;
   str.concat("<time:");
   if (priv->year)
      str.sprintf(" %d year%s", priv->year, PL(priv->year)), f++;
   if (priv->month)
      str.sprintf(" %d month%s", priv->month, PL(priv->month)), f++;
   if (priv->day)
      str.sprintf(" %d day%s", priv->day, PL(priv->day)), f++;
   if (priv->hour)
      str.sprintf(" %d hour%s", priv->hour, PL(priv->hour)), f++;
   if (priv->minute)
      str.sprintf(" %d minute%s", priv->minute, PL(priv->minute)), f++;
   if (priv->second || (!f && !priv->millisecond))
      str.sprintf(" %d second%s", priv->second, PL(priv->second));
   if (priv->millisecond)
      str.sprintf(" %d millisecond%s", priv->millisecond, PL(priv->millisecond));
   str.concat('>');
   return 0;
}
#undef PL

AbstractQoreNode *DateTimeNode::realCopy() const
{
   return new DateTimeNode(*this);
}

bool DateTimeNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   DateTimeValueHelper date(v);
   return isEqual(*date);
}

bool DateTimeNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const DateTimeNode *date = dynamic_cast<const DateTimeNode *>(v);
   if (!date)
      return false;

   return isEqual(date);
}

const char *DateTimeNode::getTypeName() const
{
   return getStaticTypeName();
}

class DateTimeNode *DateTimeNode::copy() const
{
   return new DateTimeNode(*this);
}

class DateTimeNode *DateTimeNode::add(const class DateTime *dt) const
{
   DateTimeNode *rv;
   if (!priv->relative) {
      rv = new DateTimeNode(*this);
      addAbsoluteToRelative(*rv, dt);
      return rv;
   }
   if (!dt->priv->relative) {
      rv = new DateTimeNode(*this);
      dt->addAbsoluteToRelative(*rv, this);
      return rv;
   }
   rv = new DateTimeNode();
   addRelativeToRelative(*rv, dt);
   return rv;
}

class DateTimeNode *DateTimeNode::subtractBy(const class DateTime *dt) const
{
   DateTimeNode *rv;
   if (!priv->relative)
   {
      if (dt->priv->relative) {
	 rv = new DateTimeNode(*this);
	 subtractAbsoluteByRelative(*rv, dt);
	 return rv;
      }
      rv = new DateTimeNode(true);
      calcDifference(*rv, dt);
      return rv;
   }
   if (!dt->priv->relative) {
      rv = new DateTimeNode(*this);
      dt->subtractAbsoluteByRelative(*rv, this);
      return rv;
   }
   rv = new DateTimeNode();
   subtractRelativeByRelative(*rv, dt);
   return rv;
}

// note that ISO-8601 week days go from 1 - 7 = Mon - Sun
// a NULL return value means an exception was raised
// static method
DateTimeNode *DateTimeNode::getDateFromISOWeek(int year, int week, int day, ExceptionSink *xsink)
{
   SimpleRefHolder<DateTimeNode> rv(new DateTimeNode());
   if (getDateFromISOWeekIntern(*(*rv), year, week, day, xsink))
      return 0;
   return rv.release();
}
