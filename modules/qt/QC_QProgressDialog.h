/*
 QC_QProgressDialog.h
 
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

#ifndef _QORE_QT_QC_QPROGRESSDIALOG_H

#define _QORE_QT_QC_QPROGRESSDIALOG_H

#include <QProgressDialog>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QPROGRESSDIALOG;
DLLLOCAL extern QoreClass *QC_QProgressDialog;
DLLLOCAL QoreClass *initQProgressDialogClass(QoreClass *);

class myQProgressDialog : public QProgressDialog, public QoreQDialogExtension
{
#define QOREQTYPE QProgressDialog
#define MYQOREQTYPE myQProgressDialog
#include "qore-qt-metacode.h"
#include  "qore-qt-qdialog-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQProgressDialog(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags f = 0) : QProgressDialog(parent, f), QoreQDialogExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQProgressDialog(QoreObject *obj, const QString& labelText, const QString& cancelButtonText, int minimum, int maximum, QWidget* parent = 0, Qt::WindowFlags f = 0) : QProgressDialog(labelText, cancelButtonText, minimum, maximum, parent, f), QoreQDialogExtension(obj->getClass())
      {
         init(obj);
      }
      void parent_forceShow()
      {
         forceShow();
      }
};

class QoreQProgressDialog : public QoreAbstractQDialog
{
   public:
      QPointer<myQProgressDialog> qobj;

      DLLLOCAL QoreQProgressDialog(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags f = 0) : qobj(new myQProgressDialog(obj, parent, f))
      {
      }
      DLLLOCAL QoreQProgressDialog(QoreObject *obj, const QString& labelText, const QString& cancelButtonText, int minimum, int maximum, QWidget* parent = 0, Qt::WindowFlags f = 0) : qobj(new myQProgressDialog(obj, labelText, cancelButtonText, minimum, maximum, parent, f))
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
      QORE_VIRTUAL_QDIALOG_METHODS
};

#endif // _QORE_QT_QC_QPROGRESSDIALOG_H
