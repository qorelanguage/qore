/*
 QC_QTextBrowser.h
 
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

#ifndef _QORE_QT_QC_QTEXTBROWSER_H

#define _QORE_QT_QC_QTEXTBROWSER_H

#include <QTextBrowser>
#include "QoreAbstractQTextEdit.h"
#include "qore-qt-events.h"

DLLLOCAL extern int CID_QTEXTBROWSER;
DLLLOCAL extern QoreClass *QC_QTextBrowser;
DLLLOCAL QoreClass *initQTextBrowserClass(QoreClass *);

class myQTextBrowser : public QTextBrowser, public QoreQWidgetExtension
{
#define QOREQTYPE QTextBrowser
#define MYQOREQTYPE myQTextBrowser
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE

   public:
      DLLLOCAL myQTextBrowser(QoreObject *obj, QWidget* parent = 0) : QTextBrowser(parent), QoreQWidgetExtension(obj, this)
      {
      }
      
      DLLLOCAL void pub_setupViewport(QWidget *w)
      {
	 setupViewport(w);
      }
};

typedef QoreQTextEditBase<myQTextBrowser, QoreAbstractQTextEdit> QoreQTextBrowserImpl;

class QoreQTextBrowser : public QoreQTextBrowserImpl
{
   public:
      DLLLOCAL QoreQTextBrowser(QoreObject *obj, QWidget* parent = 0) : QoreQTextBrowserImpl(new myQTextBrowser(obj, parent))
      {
      }
};

typedef QoreQtQTextEditBase<QTextBrowser, QoreAbstractQTextEdit> QoreQtQTextBrowserImpl;

class QoreQtQTextBrowser : public QoreQtQTextBrowserImpl
{
   public:
      DLLLOCAL QoreQtQTextBrowser(QoreObject *obj, QTextBrowser *qtextbrowser) : QoreQtQTextBrowserImpl(obj, qtextbrowser)
      {
      }
};

#endif // _QORE_QT_QC_QTEXTBROWSER_H
