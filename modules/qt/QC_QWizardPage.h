/*
 QC_QWizardPage.h
 
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

#ifndef _QORE_QT_QC_QWIZARDPAGE_H

#define _QORE_QT_QC_QWIZARDPAGE_H

#include <QWizardPage>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QWIZARDPAGE;
DLLLOCAL extern class QoreClass *QC_QWizardPage;

DLLLOCAL class QoreClass *initQWizardPageClass(QoreClass *);

class myQWizardPage : public QWizardPage, public QoreQWidgetExtension
{
#define QOREQTYPE QWizardPage
#define MYQOREQTYPE myQWizardPage
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQWizardPage(QoreObject *obj, QWidget* parent = 0) : QWizardPage(parent), QoreQWidgetExtension(obj, this)
      {
         
      }

      DLLLOCAL QVariant parent_field ( const QString & name ) const
      {
	 return field(name);
      }
      DLLLOCAL void parent_registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
      {
	 registerField(name, widget, property, changedSignal);
      }
      DLLLOCAL void parent_setField ( const QString & name, const QVariant & value )
      {
	 setField(name, value);
      }
      DLLLOCAL QWizard * parent_wizard () const
      {
	 return wizard();
      }
};

class QoreAbstractQWizardPage : public QoreAbstractQWidget
{
   public:
      DLLLOCAL virtual QWizardPage *getQWizardPage() const = 0;
      DLLLOCAL virtual QVariant field ( const QString & name ) const = 0;
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 ) = 0;
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value ) = 0;
      DLLLOCAL virtual QWizard * wizard () const = 0;
};

template <typename T, typename V>
class QoreQWizardPageBase : public QoreQWidgetBase<T, V>
{
   public:
      QoreQWizardPageBase(T *qo) : QoreQWidgetBase<T, V>(qo)
      {
      }
      DLLLOCAL virtual QWizardPage *getQWizardPage() const
      {
	 return static_cast<QWizardPage *>(&(*this->qobj));
      }
      DLLLOCAL virtual QVariant field ( const QString & name ) const
      {
	 return this->qobj->parent_field(name);
      }
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
      {
	 this->qobj->parent_registerField(name, widget, property, changedSignal);
      }
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value )
      {
	 this->qobj->parent_setField(name, value);
      }
      DLLLOCAL virtual QWizard * wizard () const
      {
	 return this->qobj->parent_wizard();
      }
};

typedef QoreQWizardPageBase<myQWizardPage, QoreAbstractQWizardPage> QoreQWizardPageImpl;

class QoreQWizardPage : public QoreQWizardPageImpl
{
   public:
      DLLLOCAL QoreQWizardPage(QoreObject *obj, QWidget* parent = 0) : QoreQWizardPageImpl(new myQWizardPage(obj, parent))
      {
      }
};

template <typename T, typename V>
class QoreQtQWizardPageBase : public QoreQtQWidgetBase<T, V>
{
   public:
      DLLLOCAL QoreQtQWizardPageBase(QoreObject *obj, T *qo) : QoreQtQWidgetBase<T, V>(obj, qo)
      {
      }

      DLLLOCAL virtual QWizardPage *getQWizardPage() const
      {
	 return static_cast<QWizardPage *>(&(*this->qobj));
      }

      // the following methods can never be called because they are protected
      DLLLOCAL virtual QVariant field ( const QString & name ) const
      {
	 return QVariant();
      }
      DLLLOCAL virtual void registerField ( const QString & name, QWidget * widget, const char * property = 0, const char * changedSignal = 0 )
      {
      }
      DLLLOCAL virtual void setField ( const QString & name, const QVariant & value )
      {
      }
      DLLLOCAL virtual QWizard * wizard () const
      {
	 return 0;
      }
};

typedef QoreQtQWizardPageBase<QWizardPage, QoreAbstractQWizardPage> QoreQtQWizardPageImpl;

class QoreQtQWizardPage : public QoreQtQWizardPageImpl
{
   public:
      DLLLOCAL QoreQtQWizardPage(QoreObject *obj, QWizardPage *qwp) : QoreQtQWizardPageImpl(obj, qwp)
      {
      }
};

#endif // _QORE_QT_QC_QWIZARDPAGE_H
