/*
 QC_QHBoxLayout.h
 
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

#ifndef _QORE_QC_QHBOXLAYOUT_H

#define _QORE_QC_QHBOXLAYOUT_H

#include "QoreAbstractQBoxLayout.h"

#include <QHBoxLayout>

DLLEXPORT extern int CID_QHBOXLAYOUT;

DLLLOCAL class QoreClass *initQHBoxLayoutClass(class QoreClass *qboxlayout);

class myQHBoxLayout : public QHBoxLayout, public QoreQObjectExtension
{
#define QOREQTYPE QHBoxLayout
#include "qore-qt-metacode.h"
#undef QOREQTYPE
   public:
      DLLLOCAL myQHBoxLayout(QoreObject *obj) : QHBoxLayout(), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
      DLLLOCAL myQHBoxLayout(QoreObject *obj, QWidget *parent) : QHBoxLayout(parent), QoreQObjectExtension(obj->getClass())
      {
	 init(obj);
      }
};

class QoreQHBoxLayout : public QoreAbstractQBoxLayout
{
   public:
      QPointer<myQHBoxLayout> qobj;

      DLLLOCAL QoreQHBoxLayout(QoreObject *obj) : qobj(new myQHBoxLayout(obj))
      {
      }

      DLLLOCAL QoreQHBoxLayout(QoreObject *obj, QWidget *parent) : qobj(new myQHBoxLayout(obj, parent))
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

      DLLLOCAL virtual QBoxLayout *getQBoxLayout() const
      {
	 return static_cast<QBoxLayout *>(&(*qobj));
      }

      QORE_VIRTUAL_QOBJECT_METHODS
};

#endif
