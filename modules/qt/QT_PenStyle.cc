/*
  QT_PenStyle.cc

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
#include "QT_PenStyle.h"

#include <QPen>

static qt_enum_map_t psmap;

bool PenStyle_Compare(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink)
{
   return (bool)(l->val.intval != r->val.intval);
}

class QoreString *PenStyle_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink)
{
   qt_enum_map_t::iterator i = psmap.find((int)n->val.intval);
   if (i == psmap.end()) {
      QoreString *str = new QoreString("Qt::PenStyle::");
      str->sprintf("%d", (int)n->val.intval);
      return str;
   }
   return new QoreString(i->second);
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
   NT_PENSTYLE = new QoreType("PenStyle", 0, 0, 0, 0, 0, 0, 0, 0, 0, PenStyle_Compare, 0, PenStyle_MakeString, true, false);

   QTM.add(NT_PENSTYLE);
}
