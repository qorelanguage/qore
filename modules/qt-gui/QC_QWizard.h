/*
 QC_QWizard.h
 
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

#ifndef _QORE_QT_QC_QWIZARD_H

#define _QORE_QT_QC_QWIZARD_H

#include <QWizard>
#include "QoreAbstractQDialog.h"
#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QWIZARD;
DLLEXPORT extern class QoreClass *QC_QWizard;

DLLEXPORT QoreNamespace *initQWizardNS(QoreClass *);

class myQWizard : public QWizard, public QoreQDialogExtension
{
      friend class QoreQWizard;
      friend class QoreQtQWizard;

#define QOREQTYPE QWizard
#define MYQOREQTYPE myQWizard
#include "qore-qt-metacode.h"
#include "qore-qt-qdialog-methods.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQWizard(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : QWizard(parent, flags), QoreQDialogExtension(obj, this)
      {
         
      }
};

class QoreAbstractQWizard : public QoreAbstractQDialog
{
   public:
      DLLLOCAL virtual QWizard *getQWizard() const = 0;
};

template<typename T, typename V>
class QoreQWizardBase : public QoreQDialogBase<T, V>
{
   public:
      DLLLOCAL QoreQWizardBase(T *qo) : QoreQDialogBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QWizard *getQWizard() const
      {
	 return &(*this->qobj);
      }
};


template<typename T, typename V>
class QoreQtQWizardBase : public QoreQtQDialogBase<T, V>
{
   public:
      DLLLOCAL QoreQtQWizardBase(QoreObject *obj, T *qo) : QoreQtQDialogBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QWizard *getQWizard() const
      {
         return this->qobj;
      }
};

typedef QoreQWizardBase<myQWizard, QoreAbstractQWizard> QoreQWizardImpl;

class QoreQWizard : public QoreQWizardImpl
{
   public:
      DLLLOCAL QoreQWizard(QoreObject *obj, QWidget* parent = 0, Qt::WindowFlags flags = 0) : QoreQWizardImpl(new myQWizard(obj, parent, flags))
      {
      }
};

typedef QoreQtQWizardBase<QWizard, QoreAbstractQWizard> QoreQtQWizardImpl;

class QoreQtQWizard : public QoreQtQWizardImpl
{
   public:
      DLLLOCAL QoreQtQWizard(QoreObject *obj, QWizard *qw) : QoreQtQWizardImpl(obj, qw)
      {
      }
};

#endif // _QORE_QT_QC_QWIZARD_H
