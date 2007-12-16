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
#include "QT_BrushStyle.h"

#include <QBrush>

static qt_enum_map_t bsmap;

bool BrushStyle_Compare(const QoreNode *l, const QoreNode *r, class ExceptionSink *xsink)
{
   return (bool)(l->val.intval != r->val.intval);
}

class QoreString *BrushStyle_MakeString(const QoreNode *n, int format, class ExceptionSink *xsink)
{
   qt_enum_map_t::iterator i = bsmap.find((int)n->val.intval);
   if (i == bsmap.end()) {
      QoreString *str = new QoreString("Qt::BrushStyle::");
      str->sprintf("%d", (int)n->val.intval);
      return str;
   }
   return new QoreString(i->second);
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
   NT_BRUSHSTYLE = new QoreType("BrushStyle", 0, 0, 0, 0, 0, 0, 0, 0, 0, BrushStyle_Compare, 0, BrushStyle_MakeString, true, false);

   QTM.add(NT_BRUSHSTYLE);
}
