/*
 QC_QWidget.h
 
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

#ifndef _QORE_QC_QWIDGET_H

#define _QORE_QC_QWIDGET_H

#include "QoreAbstractQDialog.h"
#include "QoreAbstractQLayout.h"

#include <QWidget>

#include <string>

#include "qore-qt-events.h"

DLLEXPORT extern qore_classid_t CID_QWIDGET;
DLLEXPORT extern QoreClass *QC_QWidget;

DLLEXPORT QoreClass *initQWidgetClass(class QoreClass *qobject, class QoreClass *qpaintdevice);

class myQWidget : public QWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QWidget
#define MYQOREQTYPE myQWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQWidget(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : QWidget(parent, window_flags), QoreQWidgetExtension(obj, this)
      {
      }
};

typedef QoreQWidgetBase<myQWidget, QoreAbstractQWidget> QoreQWidgetImpl;

class QoreQWidget : public QoreQWidgetImpl
{
   public:
      DLLLOCAL QoreQWidget(QoreObject *obj, QWidget *parent = 0, Qt::WindowFlags window_flags = 0) : QoreQWidgetImpl(new myQWidget(obj, parent, window_flags))
      {
      }
};

typedef QoreQtQWidgetBase<QWidget, QoreAbstractQWidget> QoreQtQWidgetImpl;

class QoreQtQWidget : public QoreQtQWidgetImpl
{
   public:
   DLLLOCAL QoreQtQWidget(QoreObject *obj, QWidget *qw, bool managed = true) : QoreQtQWidgetImpl(obj, qw, managed)
      {
      }
};

#endif
