/*
 QC_QShortcut.h
 
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

#ifndef _QORE_QC_QSHORTCUT_H

#define _QORE_QC_QSHORTCUT_H

#include "QoreAbstractQObject.h"

#include <QShortcut>

DLLLOCAL extern int CID_QSHORTCUT;
DLLLOCAL extern QoreClass *QC_QShortcut;

DLLLOCAL class QoreClass *initQShortcutClass(class QoreClass *parent);

class myQShortcut : public QShortcut
{
#define QOREQTYPE QShortcut
#include "qore-qt-metacode.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQShortcut(Object *obj, QWidget *parent = 0) : QShortcut(parent)
      {
	 init(obj);
      }
      DLLLOCAL myQShortcut(Object *obj, const QKeySequence & key, QWidget * parent, const char * member = 0, const char * ambiguousMember = 0, Qt::ShortcutContext context = Qt::WindowShortcut) : QShortcut(key, parent, member, ambiguousMember, context)
      {
	 init(obj);
      }
};

class QoreQShortcut : public QoreAbstractQObject
{
   public:
      myQShortcut *qobj;

      DLLLOCAL QoreQShortcut(Object *obj, QWidget *parent = 0) : qobj(new myQShortcut(obj, parent))
      {
      }

      DLLLOCAL QoreQShortcut(Object *obj, const QKeySequence & key, QWidget * parent, const char * member = 0, const char * ambiguousMember = 0, Qt::ShortcutContext context = Qt::WindowShortcut) : qobj(new myQShortcut(obj, key, parent, member, ambiguousMember, context))
      {
      }

      DLLLOCAL virtual ~QoreQShortcut()
      {
      }

      DLLLOCAL virtual class QObject *getQObject() const
      {
	 return static_cast<QObject *>(qobj);
      }
      QORE_VIRTUAL_QOBJECT_METHODS
};


#endif
