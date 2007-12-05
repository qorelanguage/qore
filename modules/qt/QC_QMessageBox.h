/*
 QC_QMessageBox.h
 
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

#ifndef _QORE_QT_QC_QMESSAGEBOX_H

#define _QORE_QT_QC_QMESSAGEBOX_H

#include <QMessageBox>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QMESSAGEBOX;
DLLLOCAL extern class QoreClass *QC_QMessageBox;

DLLLOCAL class QoreClass *initQMessageBoxClass(QoreClass *);
DLLLOCAL void initQMessageBoxStaticFunctions();

class myQMessageBox : public QMessageBox, public QoreQDialogExtension
{
#define QOREQTYPE QMessageBox
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef QOREQTYPE

   public:
      DLLLOCAL myQMessageBox(QoreObject *obj, QWidget* parent = 0) : QMessageBox(parent), QoreQDialogExtension(obj->getClass())
      {
         init(obj);
      }
      DLLLOCAL myQMessageBox(QoreObject *obj, Icon icon, const QString& title, const QString& text, StandardButtons buttons = NoButton, QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) : QMessageBox(icon, title, text, buttons, parent, f), QoreQDialogExtension(obj->getClass())
      {
         init(obj);
      }
};

class QoreQMessageBox : public QoreAbstractQDialog
{
   public:
      QPointer<myQMessageBox> qobj;

      DLLLOCAL QoreQMessageBox(QoreObject *obj, QWidget* parent = 0) : qobj(new myQMessageBox(obj, parent))
      {
      }
      DLLLOCAL QoreQMessageBox(QoreObject *obj, QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::NoButton, QWidget* parent = 0, Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) : qobj(new myQMessageBox(obj, icon, title, text, buttons, parent, f))
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

#endif // _QORE_QT_QC_QMESSAGEBOX_H
