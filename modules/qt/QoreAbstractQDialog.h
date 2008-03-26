/*
 QoreAbstractQDialog.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQDIALOG_H

#define _QORE_QT_QOREABSTRACTQDIALOG_H

#include "QoreAbstractQWidget.h"

class QoreQDialogExtension : public QoreQWidgetExtension
{
   protected:
      const QoreMethod *m_accept, *m_done, *m_reject;

   public:
      DLLLOCAL QoreQDialogExtension(QoreObject *obj, QObject *qo) : QoreQWidgetExtension(obj, qo)
      {
	 const QoreClass *qc = obj->getClass();
         m_accept = findMethod(qc, "accept");
         m_done   = findMethod(qc, "done");
         m_reject = findMethod(qc, "reject");
      }
};

class QoreAbstractQDialog : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual class QDialog *getQDialog() const = 0;

      // virtual methods
      DLLLOCAL virtual void accept () = 0;
      DLLLOCAL virtual void done ( int r ) = 0;
      DLLLOCAL virtual void reject () = 0;
};

#define QORE_VIRTUAL_QDIALOG_METHODS QORE_VIRTUAL_QWIDGET_METHODS \
   DLLLOCAL virtual void accept () { qobj->parent_accept(); } \
   DLLLOCAL virtual void done ( int r ) { qobj->parent_done(r); } \
   DLLLOCAL virtual void reject () { qobj->parent_reject(); }

#endif  // _QORE_QT_QOREABSTRACTQDIALOG_H
