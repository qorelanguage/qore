/*
  QT_BrushStyle.cc

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
#include "BrushStyleNode.h"

static qt_enum_map_t bsmap;

// get the value of the type in a string context (default implementation = del = false and returns NullString)
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// use the QoreStringValueHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *BrushStyleNode::getStringRepresentation(bool &del) const
{
   del = true;
   QoreString *str = new QoreString();
   getStringRepresentation(*str);
   return str;
}

// concatenate string representation to a QoreString (no action for complex types = default implementation)
void BrushStyleNode::getStringRepresentation(QoreString &str) const
{
   qt_enum_map_t::iterator i = bsmap.find((int)val);
   if (i == bsmap.end()) {
      str.concat("Qt::BrushStyle::");
      str.sprintf("%d", (int)val);
      return;
   }
   str.concat(i->second);
}

bool BrushStyleNode::getAsBool() const
{
   return (bool)val;
}

int BrushStyleNode::getAsInt() const
{
   return (int)val;
}

int64 BrushStyleNode::getAsBigInt() const
{
   return (int64)val;
}

double BrushStyleNode::getAsFloat() const
{
   return (double)val;
}

// FIXME: move QoreString * to first argument
// get string representation (for %n and %N), foff is for multi-line formatting offset, -1 = no line breaks
// if del is true, then the returned QoreString * should be deleted, if false, then it must not be
// the ExceptionSink is only needed for QoreObject where a method may be executed
// use the QoreNodeAsStringHelper class (defined in QoreStringNode.h) instead of using this function directly
QoreString *BrushStyleNode::getAsString(bool &del, int foff, class ExceptionSink *xsink) const
{
   return getStringRepresentation(del);
}

int BrushStyleNode::getAsString(QoreString &str, int foff, class ExceptionSink *xsink) const
{
   getStringRepresentation(str);
   return 0;
}

class QoreNode *BrushStyleNode::realCopy() const
{
   return new BrushStyleNode(val);
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const QoreNode *val) const;
// the type passed must always be equal to the current type
bool BrushStyleNode::is_equal_soft(const QoreNode *v, ExceptionSink *xsink) const
{
   return (Qt::BrushStyle)v->getAsInt() == val;
}

bool BrushStyleNode::is_equal_hard(const QoreNode *v, ExceptionSink *xsink) const
{
   const BrushStyleNode *ps = dynamic_cast<const BrushStyleNode *>(v);
   if (!ps)
      return false;

   return ps->val == val;
}

// returns the data type
const QoreType *BrushStyleNode::getType() const
{
   return NT_BRUSHSTYLE;
}

// returns the type name as a c string
const char *BrushStyleNode::getTypeName() const
{
   return "BrushStyle";
}

void addBrushStyleType()
{
   // make map for descriptions
   bsmap[(int)Qt::NoBrush] = "NoBrush";
   bsmap[(int)Qt::SolidPattern] = "SolidPattern";
   bsmap[(int)Qt::Dense1Pattern] = "Dense1Pattern";
   bsmap[(int)Qt::Dense2Pattern] = "Dense2Pattern";
   bsmap[(int)Qt::Dense3Pattern] = "Dense3Pattern";
   bsmap[(int)Qt::Dense4Pattern] = "Dense4Pattern";
   bsmap[(int)Qt::Dense5Pattern] = "Dense5Pattern";
   bsmap[(int)Qt::Dense6Pattern] = "Dense6Pattern";
   bsmap[(int)Qt::Dense7Pattern] = "Dense7Pattern";
   bsmap[(int)Qt::HorPattern] = "HorPattern";
   bsmap[(int)Qt::VerPattern] = "VerPattern";
   bsmap[(int)Qt::CrossPattern] = "CrossPattern";
   bsmap[(int)Qt::BDiagPattern] = "BDiagPattern";
   bsmap[(int)Qt::FDiagPattern] = "FDiagPattern";
   bsmap[(int)Qt::DiagCrossPattern] = "DiagCrossPattern";
   bsmap[(int)Qt::LinearGradientPattern] = "LinearGradientPattern";
   bsmap[(int)Qt::RadialGradientPattern] = "RadialGradientPattern";
   bsmap[(int)Qt::ConicalGradientPattern] = "ConicalGradientPattern";
   bsmap[(int)Qt::TexturePattern] = "TexturePattern";  

   // add types for enums
   NT_BRUSHSTYLE = new QoreType("BrushStyle");

   QTM.add(NT_BRUSHSTYLE);
}
