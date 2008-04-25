/*
 QC_QSvgWidget.h
 
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

#ifndef _QORE_QT_QC_QSVGWIDGET_H

#define _QORE_QT_QC_QSVGWIDGET_H

#include <QSvgWidget>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QSVGWIDGET;
DLLLOCAL extern QoreClass *QC_QSvgWidget;
DLLLOCAL QoreClass *initQSvgWidgetClass(QoreClass *);

class myQSvgWidget : public QSvgWidget, public QoreQWidgetExtension
{
#define QOREQTYPE QSvgWidget
#define MYQOREQTYPE myQSvgWidget
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQSvgWidget(QoreObject *obj, QWidget* parent = 0) : QSvgWidget(parent), QoreQWidgetExtension(obj, this)
      {
      }
      DLLLOCAL myQSvgWidget(QoreObject *obj, const QString& file, QWidget* parent = 0) : QSvgWidget(file, parent), QoreQWidgetExtension(obj, this)
      {
      }
};

typedef QoreQWidgetBase<myQSvgWidget, QoreAbstractQWidget> QoreQSvgWidgetImpl;

class QoreQSvgWidget : public QoreQSvgWidgetImpl
{
   public:
      DLLLOCAL QoreQSvgWidget(QoreObject *obj, QWidget* parent = 0) : QoreQSvgWidgetImpl(new myQSvgWidget(obj, parent))
      {
      }
      DLLLOCAL QoreQSvgWidget(QoreObject *obj, const QString& file, QWidget* parent = 0) : QoreQSvgWidgetImpl(new myQSvgWidget(obj, file, parent))
      {
      }
};

typedef QoreQtQWidgetBase<QSvgWidget, QoreAbstractQWidget> QoreQtQSvgWidgetImpl;

class QoreQtQSvgWidget : public QoreQtQSvgWidgetImpl
{
   public:
      DLLLOCAL QoreQtQSvgWidget(QoreObject *obj, QSvgWidget *qsvgwidget) : QoreQtQSvgWidgetImpl(obj, qsvgwidget)
      {
      }
};

#endif // _QORE_QT_QC_QSVGWIDGET_H
