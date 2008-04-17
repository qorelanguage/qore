/*
 QC_QSplashScreen.h
 
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

#ifndef _QORE_QT_QC_QSPLASHSCREEN_H

#define _QORE_QT_QC_QSPLASHSCREEN_H

#include <QSplashScreen>
#include "QoreAbstractQWidget.h"
#include "qore-qt-events.h"

DLLLOCAL extern qore_classid_t CID_QSPLASHSCREEN;
DLLLOCAL extern QoreClass *QC_QSplashScreen;
DLLLOCAL QoreClass *initQSplashScreenClass(QoreClass *);

class myQSplashScreen : public QSplashScreen, public QoreQWidgetExtension
{
#define QOREQTYPE QSplashScreen
#define MYQOREQTYPE myQSplashScreen
#include "qore-qt-metacode.h"
#include "qore-qt-widget-events.h"
#undef MYQOREQTYPE
#undef QOREQTYPE


   protected:
      const QoreMethod *m_drawContents;

      void init(const QoreClass *qc)
      {
	 m_drawContents = qc->findMethod("drawContents");
      }

      DLLLOCAL virtual void drawContents ( QPainter * painter )
      {
	 if (!m_drawContents) {
	    QSplashScreen::drawContents(painter);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(return_object(QC_QPainter, new QoreQPainter(painter)));

         discard(dispatch_event_intern(qore_obj, m_drawContents, *args, &xsink), &xsink);
      }
      
   public:
      DLLLOCAL myQSplashScreen(QoreObject *obj, const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = 0) : QSplashScreen(pixmap, f), QoreQWidgetExtension(obj, this)
      {
	 init(obj->getClass());
      }
      DLLLOCAL myQSplashScreen(QoreObject *obj, QWidget* parent, const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = 0) : QSplashScreen(parent, pixmap, f), QoreQWidgetExtension(obj, this)
      {
	 init(obj->getClass());
      }

      DLLLOCAL void parent_drawContents ( QPainter * painter )
      {
	 QSplashScreen::drawContents(painter);
      }
};

typedef QoreQWidgetBase<myQSplashScreen, QoreAbstractQWidget> QoreQSplashScreenImpl;

class QoreQSplashScreen : public QoreQSplashScreenImpl
{
   public:
      DLLLOCAL QoreQSplashScreen(QoreObject *obj, const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = 0) : QoreQSplashScreenImpl(new myQSplashScreen(obj, pixmap, f))
      {
      }
      DLLLOCAL QoreQSplashScreen(QoreObject *obj, QWidget* parent, const QPixmap& pixmap = QPixmap(), Qt::WindowFlags f = 0) : QoreQSplashScreenImpl(new myQSplashScreen(obj, parent, pixmap, f))
      {
      }

      DLLLOCAL void drawContents ( QPainter * painter )
      {
	 return qobj->parent_drawContents(painter);
      }
};

#endif // _QORE_QT_QC_QSPLASHSCREEN_H
