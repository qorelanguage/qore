/*
 QC_QWizard.h
 
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

#ifndef _QORE_QT_QC_QWIZARD_H

#define _QORE_QT_QC_QWIZARD_H

#include <QWizard>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QWIZARD;
DLLLOCAL extern class QoreClass *QC_QWizard;

DLLLOCAL QoreNamespace *initQWizardNS(QoreClass *);

class myQWizard : public QWizard, public QoreQDialogExtension
{
      friend class QoreQWizard;
      friend class QoreQtQWizard;

#define QOREQTYPE QWizard
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQWizard(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : QWizard(parent, flags), QoreQDialogExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreAbstractQWizard : public QoreAbstractQDialog
{
   public:
      DLLLOCAL virtual QWizard *getQWizard() const = 0;
};

class QoreQWizard : public QoreAbstractQWizard
{
   public:
      QPointer<myQWizard> qobj;

      DLLLOCAL QoreQWizard(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : qobj(new myQWizard(obj, parent, flags))
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual class QDialog *getQDialog() const
      {
	 return static_cast<QDialog *>(&(*qobj));
      }
      DLLLOCAL virtual class QWizard *getQWizard() const
      {
	 return static_cast<QWizard *>(&(*qobj));
      }
      QORE_VIRTUAL_QDIALOG_METHODS
};

class QoreQtQWizard : public QoreAbstractQWizard
{
   public:
      QoreObject *qore_obj;
      QPointer<QWizard> qobj;

      DLLLOCAL QoreQtQWizard(QoreObject *obj, QWizard *qw) : qore_obj(obj), qobj(qw)
      {
      }
      DLLLOCAL virtual class QObject *getQObject() const
      {
         return static_cast<QObject *>(&(*qobj));
      }
      DLLLOCAL virtual class QWidget *getQWidget() const
      {
         return static_cast<QWidget *>(&(*qobj));
      }
      DLLLOCAL virtual QPaintDevice *getQPaintDevice() const
      {
         return static_cast<QPaintDevice *>(&(*qobj));
      }
      DLLLOCAL virtual class QDialog *getQDialog() const
      {
	 return static_cast<QDialog *>(&(*qobj));
      }
      DLLLOCAL virtual class QWizard *getQWizard() const
      {
	 return static_cast<QWizard *>(&(*qobj));
      }

#include "qore-qt-static-qdialog-methods.h"

};

#endif // _QORE_QT_QC_QWIZARD_H
