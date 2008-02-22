/*
  PenStyleNode.cc

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

#include "qore-qt.h"
#include "PenStyleNode.h"

static qt_enum_map_t psmap;

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *PenStyleNode::getStringRepresentation(bool &del) const
{
   del = true;
   QoreString *str = new QoreString();
   getStringRepresentation(*str);
   return str;
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void PenStyleNode::getStringRepresentation(QoreString &str) const
{
   qt_enum_map_t::iterator i = psmap.find((int)val);
   if (i == psmap.end()) {
      str.concat("Qt::PenStyle::");
      str.sprintf("%d", (int)val);
      return;
   }
   str.concat(i->second);
}

bool PenStyleNode::getAsBoolImpl() const
{
   return (bool)val;
}

int PenStyleNode::getAsIntImpl() const
{
   return (int)val;
}

int64 PenStyleNode::getAsBigIntImpl() const
{
   return (int64)val;
}

double PenStyleNode::getAsFloatImpl() const
{
   return (double)val;
}

// FIXME: move QoreString * to first argument
// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *PenStyleNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   return getStringRepresentation(del);
}

int PenStyleNode::getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
{
   getStringRepresentation(str);
   return 0;
}

class AbstractQoreNode *PenStyleNode::realCopy() const
{
   return new PenStyleNode(val);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode *val) const;
// the type passed must always be equal to the current type
bool PenStyleNode::is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   return (Qt::PenStyle)v->getAsInt() == val;
}

bool PenStyleNode::is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const
{
   const PenStyleNode *ps = dynamic_cast<const PenStyleNode *>(v);
   if (!ps)
      return false;

   return ps->val == val;
}

// returns the data type
const QoreType *PenStyleNode::getType() const
{
   return NT_PENSTYLE;
}

// returns the type name as a c string
const char *PenStyleNode::getTypeName() const
{
   return "PenStyle";
}

void addPenStyleType()
{
   // make map for descriptions
   psmap[(int)Qt::NoPen] = "NoPen";
   psmap[(int)Qt::SolidLine] = "SolidLine";
   psmap[(int)Qt::DashLine] = "DashLine";
   psmap[(int)Qt::DotLine] = "DotLine";
   psmap[(int)Qt::DashDotLine] = "DashDotLine";
   psmap[(int)Qt::DashDotDotLine] = "DashDotDotLine";
   psmap[(int)Qt::CustomDashLine] = "CustomDashLine";

   // add types for enums
   NT_PENSTYLE = new QoreType();
   QTM.add(NT_PENSTYLE);
}
