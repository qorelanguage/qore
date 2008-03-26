/*
 QC_QLCDNumber.h
 
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

#ifndef _QORE_QC_QLCDNUMBER_H

#define _QORE_QC_QLCDNUMBER_H

#include "QoreAbstractQFrame.h"

#include <QLCDNumber>

#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QLCDNUMBER;

DLLLOCAL class QoreClass *initQLCDNumberClass(class QoreClass *qframe);

class myQLCDNumber : public QLCDNumber, public QoreQWidgetExtension
{
#define QOREQTYPE QLCDNumber
#define MYQOREQTYPE myQLCDNumber
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQLCDNumber(QoreObject *obj, QWidget *parent = 0) : QLCDNumber(parent), QoreQWidgetExtension(obj, this)
      {
      }
      DLLLOCAL myQLCDNumber(QoreObject *obj, int num_digits, QWidget *parent = 0) : QLCDNumber(num_digits, parent), QoreQWidgetExtension(obj, this)
      {
      }
};

typedef QoreQFrameBase<myQLCDNumber, QoreAbstractQFrame> QoreQLCDNumberImpl;

class QoreQLCDNumber : public QoreQLCDNumberImpl
{
   public:
      DLLLOCAL QoreQLCDNumber(QoreObject *obj, int num_digits, QWidget *parent = 0) : QoreQLCDNumberImpl(new myQLCDNumber(obj, num_digits, parent))
      {
      }
      DLLLOCAL QoreQLCDNumber(QoreObject *obj, QWidget *parent = 0) : QoreQLCDNumberImpl(new myQLCDNumber(obj, parent))
      {
      }
};

#endif
