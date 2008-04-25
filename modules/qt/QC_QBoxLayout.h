/*
 QC_QBoxLayout.h
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#ifndef _QORE_QC_QBOXLAYOUT_H

#define _QORE_QC_QBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"

#include <QBoxLayout>

DLLEXPORT extern qore_classid_t CID_QBOXLAYOUT;

DLLLOCAL class QoreClass *initQBoxLayoutClass(class QoreClass *qlayout);

class myQBoxLayout : public QBoxLayout, public QoreQLayoutExtension
{
#define QOREQTYPE QBoxLayout
#define MYQOREQTYPE myQBoxLayout
#include "qore-qt-metacode.h"
#include "qore-qt-qlayout-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE
      DLLLOCAL myQBoxLayout(QoreObject *obj, QBoxLayout::Direction dir) : QBoxLayout(dir), QoreQLayoutExtension(obj, this)
      {
	 
      }
      DLLLOCAL myQBoxLayout(QoreObject *obj, QBoxLayout::Direction dir, QWidget *parent) : QBoxLayout(dir, parent), QoreQLayoutExtension(obj, this)
      {
	 
      }
};

typedef QoreQBoxLayoutBase<myQBoxLayout, QoreAbstractQBoxLayout> QoreQBoxLayoutImpl;

class QoreQBoxLayout : public QoreQBoxLayoutImpl
{
   public:
      DLLLOCAL QoreQBoxLayout(QoreObject *obj, QBoxLayout::Direction dir) : QoreQBoxLayoutImpl(new myQBoxLayout(obj, dir))
      {
      }

      DLLLOCAL QoreQBoxLayout(QoreObject *obj, QBoxLayout::Direction dir, QWidget *parent) : QoreQBoxLayoutImpl(new myQBoxLayout(obj, dir, parent))
      {
      }
};

#endif
