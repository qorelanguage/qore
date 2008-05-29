/*
 QoreAbstractQDialog.h
 
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

template<typename T, typename V>
class QoreQDialogBase : public QoreQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQDialogBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QDialog *getQDialog() const
      {
         return &(*this->qobj);
      }

      DLLLOCAL virtual void accept()
      { 
	 this->qobj->parent_accept(); 
      }
      DLLLOCAL virtual void done(int r)
      {
	 this->qobj->parent_done(r);
      }
      DLLLOCAL virtual void reject()
      {
	 this->qobj->parent_reject();
      }
};

template<typename T, typename V>
class QoreQtQDialogBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQDialogBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QDialog *getQDialog() const
      {
         return this->qobj;
      }

      DLLLOCAL virtual void accept() { this->qobj->accept(); }
      DLLLOCAL virtual void done(int r) { /*this->qobj->done(r);*/ }
      DLLLOCAL virtual void reject() { this->qobj->reject(); }
};

#endif  // _QORE_QT_QOREABSTRACTQDIALOG_H
