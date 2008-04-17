/*
 QC_QSplashScreen.cc
 
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

#include <qore/Qore.h>

#include "QC_QSplashScreen.h"

qore_classid_t CID_QSPLASHSCREEN;
QoreClass *QC_QSplashScreen = 0;

//QSplashScreen ( const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0 )
//QSplashScreen ( QWidget * parent, const QPixmap & pixmap = QPixmap(), Qt::WindowFlags f = 0 )
static void QSPLASHSCREEN_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSPLASHSCREEN, new QoreQSplashScreen(self));
      return;
   }
   if (p->getType() != NT_OBJECT) {
      xsink->raiseException("QSPLASHSCREEN-CONSTRUCTOR-ERROR", "QSplashscreen::constructor() does not know how to handle arguments of type '%s' as the first argument", p->getTypeName());
      return;
   }

   QoreQWidget *parent = (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink);
   if (!parent) {
      QoreQPixmap *pixmap = (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink);
      if (*xsink)
	 return;
      ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
      p = get_param(params, 1);
      Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
      self->setPrivate(CID_QSPLASHSCREEN, new QoreQSplashScreen(self, pixmap ? *(static_cast<QPixmap *>(pixmap)) : QPixmap(), f));
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   p = get_param(params, 1);
   QoreQPixmap *pixmap = (p && p->getType() == NT_OBJECT) ? (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   p = get_param(params, 2);
   Qt::WindowFlags f = (Qt::WindowFlags)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QSPLASHSCREEN, new QoreQSplashScreen(self, static_cast<QWidget *>(parent->getQWidget()), pixmap ? *(static_cast<QPixmap *>(pixmap)) : QPixmap(), f));
   return;
}

static void QSPLASHSCREEN_copy(QoreObject *self, QoreObject *old, QoreQSplashScreen *qss, ExceptionSink *xsink)
{
   xsink->raiseException("QSPLASHSCREEN-COPY-ERROR", "objects of this class cannot be copied");
}

//void finish ( QWidget * mainWin )
static AbstractQoreNode *QSPLASHSCREEN_finish(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQWidget *mainWin = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!mainWin) {
      if (!xsink->isException())
         xsink->raiseException("QSPLASHSCREEN-FINISH-PARAM-ERROR", "expecting a QWidget object as first argument to QSplashScreen::finish()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> mainWinHolder(static_cast<AbstractPrivateData *>(mainWin), xsink);
   qss->qobj->finish(static_cast<QWidget *>(mainWin->getQWidget()));
   return 0;
}

//const QPixmap pixmap () const
static AbstractQoreNode *QSPLASHSCREEN_pixmap(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QPixmap, new QoreQPixmap(qss->qobj->pixmap()));
}

//void repaint ()
static AbstractQoreNode *QSPLASHSCREEN_repaint(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   qss->qobj->repaint();
   return 0;
}

//void setPixmap ( const QPixmap & pixmap )
static AbstractQoreNode *QSPLASHSCREEN_setPixmap(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPixmap *pixmap = (p && p->getType() == NT_OBJECT) ? (QoreQPixmap *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPIXMAP, xsink) : 0;
   if (!pixmap) {
      if (!xsink->isException())
         xsink->raiseException("QSPLASHSCREEN-SETPIXMAP-PARAM-ERROR", "expecting a QPixmap object as first argument to QSplashScreen::setPixmap()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
   qss->qobj->setPixmap(*(static_cast<QPixmap *>(pixmap)));
   return 0;
}

//void clearMessage ()
static AbstractQoreNode *QSPLASHSCREEN_clearMessage(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   qss->qobj->clearMessage();
   return 0;
}

//void showMessage ( const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black )
static AbstractQoreNode *QSPLASHSCREEN_showMessage(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString message;
   if (get_qstring(p, message, xsink))
      return 0;
   p = get_param(params, 1);
   int alignment = !is_nothing(p) ? p->getAsInt() : Qt::AlignLeft;
   p = get_param(params, 2);
   QoreQColor *color = (p && p->getType() == NT_OBJECT) ? (QoreQColor *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QCOLOR, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> colorHolder(static_cast<AbstractPrivateData *>(color), xsink);
   qss->qobj->showMessage(message, alignment, *(static_cast<QColor *>(color)));
   return 0;
}

//virtual void drawContents ( QPainter * painter )
static AbstractQoreNode *QSPLASHSCREEN_drawContents(QoreObject *self, QoreQSplashScreen *qss, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQPainter *painter = (p && p->getType() == NT_OBJECT) ? (QoreQPainter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QPAINTER, xsink) : 0;
   if (!painter) {
      if (!xsink->isException())
         xsink->raiseException("QSPLASHSCREEN-DRAWCONTENTS-PARAM-ERROR", "expecting a QPainter object as first argument to QSplashScreen::drawContents()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> painterHolder(static_cast<AbstractPrivateData *>(painter), xsink);
   qss->drawContents(painter->getQPainter());
   return 0;
}

QoreClass *initQSplashScreenClass(QoreClass *qwidget)
{
   QC_QSplashScreen = new QoreClass("QSplashScreen", QDOM_GUI);
   CID_QSPLASHSCREEN = QC_QSplashScreen->getID();

   QC_QSplashScreen->addBuiltinVirtualBaseClass(qwidget);

   QC_QSplashScreen->setConstructor(QSPLASHSCREEN_constructor);
   QC_QSplashScreen->setCopy((q_copy_t)QSPLASHSCREEN_copy);

   QC_QSplashScreen->addMethod("finish",                      (q_method_t)QSPLASHSCREEN_finish);
   QC_QSplashScreen->addMethod("pixmap",                      (q_method_t)QSPLASHSCREEN_pixmap);
   QC_QSplashScreen->addMethod("repaint",                     (q_method_t)QSPLASHSCREEN_repaint);
   QC_QSplashScreen->addMethod("setPixmap",                   (q_method_t)QSPLASHSCREEN_setPixmap);
   QC_QSplashScreen->addMethod("clearMessage",                (q_method_t)QSPLASHSCREEN_clearMessage);
   QC_QSplashScreen->addMethod("showMessage",                 (q_method_t)QSPLASHSCREEN_showMessage);

   // private methods
   QC_QSplashScreen->addMethod("drawContents",                (q_method_t)QSPLASHSCREEN_drawContents, true);

   return QC_QSplashScreen;
}
