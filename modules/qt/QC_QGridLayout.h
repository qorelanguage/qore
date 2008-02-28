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

class myQGridLayout : public QGridLayout, public QoreQObjectExtension
{
#define QOREQTYPE QGridLayout
#include "qore-qt-metacode.h"
#undef QOREQTYPE
      DLLLOCAL myQGridLayout(QoreObject *obj) : QGridLayout(), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }

      DLLLOCAL myQGridLayout(QoreObject *obj, QWidget *parent) : QGridLayout(parent), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQGridLayout : public QoreAbstractQLayout
{
   public:
      QPointer<myQGridLayout> qobj;

      DLLLOCAL QoreQGridLayout(QoreObject *obj) : qobj(new myQGridLayout(obj))
      {
      }

      DLLLOCAL QoreQGridLayout(QoreObject *obj, QWidget *parent) : qobj(new myQGridLayout(obj, parent))
      {
      }

      DLLLOCAL virtual QObject *getQObject() const
      {
	 return static_cast<QObject *>(&(*qobj));
      }

      DLLLOCAL virtual QLayout *getQLayout() const
      {
	 return static_cast<QLayout *>(&(*qobj));
      }

      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif
