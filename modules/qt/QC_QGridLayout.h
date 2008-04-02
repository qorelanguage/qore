/*
 QC_QGridLayout.h
 
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

#ifndef _QORE_QC_QGRIDLAYOUT_H

#define _QORE_QC_QGRIDLAYOUT_H

#include "QoreAbstractQLayout.h"

#include <QGridLayout>

DLLEXPORT extern qore_classid_t CID_QGRIDLAYOUT;

DLLLOCAL class QoreClass *initQGridLayoutClass(class QoreClass *qlayout);

class myQGridLayout : public QGridLayout, public QoreQLayoutExtension
{
#define QOREQTYPE QGridLayout
#define MYQOREQTYPE myQGridLayout
#include "qore-qt-metacode.h"
#include "qore-qt-qlayout-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
      DLLLOCAL myQGridLayout(QoreObject *obj) : QGridLayout(), QoreQLayoutExtension(obj, this)
      {
      }

      DLLLOCAL myQGridLayout(QoreObject *obj, QWidget *parent) : QGridLayout(parent), QoreQLayoutExtension(obj, this)
      {
      }
};

typedef QoreQLayoutBase<myQGridLayout, QoreAbstractQLayout> QoreQGridLayoutImpl;

class QoreQGridLayout : public QoreQGridLayoutImpl
{
   public:
      DLLLOCAL QoreQGridLayout(QoreObject *obj) : QoreQGridLayoutImpl(new myQGridLayout(obj))
      {
      }

      DLLLOCAL QoreQGridLayout(QoreObject *obj, QWidget *parent) : QoreQGridLayoutImpl(new myQGridLayout(obj, parent))
      {
      }
};

#endif
