/*
 QoreAbstractQStyle.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQSTYLE_H

#define _QORE_QT_QOREABSTRACTQSTYLE_H

#include "QoreAbstractQObject.h"

#include <QStyle>

class QoreAbstractQStyle : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual class QStyle *getQStyle() const = 0;
      DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0;
};

#define QORE_VIRTUAL_QSTYLE_METHODS QORE_VIRTUAL_QOBJECT_METHODS	\
   DLLLOCAL virtual QStyle *getQStyle() const { return qobj; } \
   DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const {\
      return qobj->layoutSpacingImplementation(control1, control2, orientation, option, widget); \
   }									\
   DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { \
      return qobj->standardIconImplementation(standardIcon, option, widget); \
   }

#endif  // _QORE_QT_QOREABSTRACTQSTYLE_H
