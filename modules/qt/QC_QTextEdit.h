/*
 QC_QTextEdit.h
 
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

#ifndef _QORE_QT_QC_QTEXTEDIT_H

#define _QORE_QT_QC_QTEXTEDIT_H

#include <QTextEdit>
#include "QoreAbstractQTextEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QTEXTEDIT;
DLLLOCAL extern class QoreClass *QC_QTextEdit;

DLLLOCAL QoreNamespace *initQTextEditNS(QoreClass *);

class myQTextEdit : public QTextEdit, public QoreQWidgetExtension
{
#define QOREQTYPE QTextEdit
#define MYQOREQTYPE myQTextEdit
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTextEdit(QoreObject *obj, QWidget* parent = 0) : QTextEdit(parent), QoreQWidgetExtension(obj, this)
      {
      }
      DLLLOCAL myQTextEdit(QoreObject *obj, const QString& text, QWidget* parent = 0) : QTextEdit(text, parent), QoreQWidgetExtension(obj, this)
      {         
      }

      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
         setupViewport(w);
      }
};

typedef QoreQTextEditBase<myQTextEdit, QoreAbstractQTextEdit> QoreQTextEditImpl;

class QoreQTextEdit : public QoreQTextEditImpl
{
   public:
      DLLLOCAL QoreQTextEdit(QoreObject *obj, QWidget* parent = 0) : QoreQTextEditImpl(new myQTextEdit(obj, parent))
      {
      }
      DLLLOCAL QoreQTextEdit(QoreObject *obj, const QString& text, QWidget* parent = 0) : QoreQTextEditImpl(new myQTextEdit(obj, text, parent))
      {
      }
};

#endif // _QORE_QT_QC_QTEXTEDIT_H
