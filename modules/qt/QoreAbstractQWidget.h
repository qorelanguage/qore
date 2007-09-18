/*
 QoreAbstractQWidget.h
 
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

#ifndef _QORE_QOREABSTRACTQWIDGET_H

#define _QORE_QOREABSTRACTQWIDGET_H

#include "QoreAbstractQObject.h"
#include "QoreAbstractQPaintDevice.h"

#include "QC_QSize.h"

#include <QActionEvent>

class QoreQWidgetExtension {
   protected:
      // event methods
      Method *e_changeEvent, *e_enterEvent, *e_event, *e_leaveEvent,
         *e_paintEvent, 
         *e_mouseMoveEvent, *e_mousePressEvent, 
         *e_mouseReleaseEvent, *e_mouseDoubleClickEvent,
         *e_keyPressEvent, *e_keyReleaseEvent,
         *e_resizeEvent,
         *e_moveEvent,
	 *e_actionEvent,
	 *e_closeEvent,
	 *e_contextMenuEvent,
	 *e_dragEnterEvent,
	 *e_dragMoveEvent,
	 *e_dropEvent,
	 *e_dragLeaveEvent,
	 *e_focusInEvent, *e_focusOutEvent,
	 *e_hideEvent,
	 *e_inputMethodEvent,
	 *e_showEvent,
	 *e_tabletEvent,
	 *e_wheelEvent,

	 // other methods
	 *p_sizeHint,

	 ;

   private:
      DLLLOCAL Method *findMethod(QoreClass *qc, const char *n)
      {
	 Method *m = qc->findMethod(n);
	 //printd(5, "findMethod() %s::%s: %s\n", qc->getName(), n, (m && m->getType() == CT_USER) ? "ok" : "x");
	 return (m && m->getType() == CT_USER) ? m : 0;
      }

   public:
      DLLLOCAL QoreQWidgetExtension(QoreClass *qc)
      {
         e_paintEvent             = findMethod(qc, "paintEvent");
         e_mouseMoveEvent         = findMethod(qc, "mouseMoveEvent");
         e_mousePressEvent        = findMethod(qc, "mousePressEvent");
         e_mouseReleaseEvent      = findMethod(qc, "mouseReleaseEvent");
         e_mouseDoubleClickEvent  = findMethod(qc, "mouseDoubleClickEvent");
         e_keyPressEvent          = findMethod(qc, "keyPressEvent");
         e_keyReleaseEvent        = findMethod(qc, "keyReleaseEvent");
         e_changeEvent            = findMethod(qc, "changeEvent");
         e_enterEvent             = findMethod(qc, "enterEvent");
         e_event                  = findMethod(qc, "event");
         e_leaveEvent             = findMethod(qc, "leaveEvent");
         e_resizeEvent            = findMethod(qc, "resizeEvent");
         e_moveEvent              = findMethod(qc, "moveEvent");
	 e_actionEvent            = findMethod(qc, "actionEvent");
	 e_closeEvent             = findMethod(qc, "closeEvent");
         e_contextMenuEvent       = findMethod(qc, "contextMenuEvent");
	 e_dragEnterEvent         = findMethod(qc, "dragEnterEvent");
	 e_dragMoveEvent          = findMethod(qc, "dragMoveEvent");
	 e_dropEvent              = findMethod(qc, "dropEvent");
	 e_dragLeaveEvent         = findMethod(qc, "dragLeaveEvent");
	 e_focusInEvent           = findMethod(qc, "focusInEvent");
	 e_focusOutEvent          = findMethod(qc, "focusOutEvent");
	 e_hideEvent              = findMethod(qc, "hideEvent");
	 e_inputMethodEvent       = findMethod(qc, "inputMethodEvent");
	 e_showEvent              = findMethod(qc, "showEvent");
	 e_tabletEvent            = findMethod(qc, "tabletEvent");
	 e_wheelEvent             = findMethod(qc, "wheelEvent");

	 p_sizeHint               = findMethod(qc, "sizeHint");
      }
};

#define QORE_VIRTUAL_QWIDGET_METHODS QORE_VIRTUAL_QOBJECT_METHODS \
   DLLLOCAL virtual void actionEvent(QActionEvent * event) {\
      qobj->actionEvent(event); \
   }\
   DLLLOCAL virtual void changeEvent(QEvent * event) {	\
      qobj->changeEvent(event); \
   }\
   DLLLOCAL virtual void closeEvent(QCloseEvent * event) {\
      qobj->closeEvent(event); \
   }\
   DLLLOCAL virtual void contextMenuEvent ( QContextMenuEvent * event ) {\
      qobj->contextMenuEvent(event); \
   } \
   DLLLOCAL virtual void dragEnterEvent ( QDragEnterEvent * event ) {\
      qobj->dragEnterEvent(event); \
   }\
   DLLLOCAL virtual void dragLeaveEvent ( QDragLeaveEvent * event ) {\
      qobj->dragLeaveEvent(event); \
   }\
   DLLLOCAL virtual void dragMoveEvent ( QDragMoveEvent * event )  {\
      qobj->dragMoveEvent(event); \
   }\
   DLLLOCAL virtual void dropEvent ( QDropEvent * event ) {	\
      qobj->dropEvent(event); \
   }\
   DLLLOCAL virtual void enterEvent ( QEvent * event ) {	\
      qobj->enterEvent(event); \
   }\
   DLLLOCAL virtual bool event ( QEvent * event ) {	\
      return qobj->event(event); \
   }\
   DLLLOCAL virtual void focusInEvent ( QFocusEvent * event ) {	\
      qobj->focusInEvent(event); \
   }\
   DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event ) {	\
      qobj->focusOutEvent(event); \
   }\
   DLLLOCAL virtual void hideEvent ( QHideEvent * event ) {	\
      qobj->hideEvent(event); \
   }\
   DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event ) { \
      qobj->inputMethodEvent(event); \
   }\
   DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event ) {	\
      qobj->keyPressEvent(event); \
   }\
   DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event ) {	\
      qobj->keyReleaseEvent(event); \
   }\
   DLLLOCAL virtual void leaveEvent ( QEvent * event ) {	\
      qobj->leaveEvent(event); \
   }\
   /*DLLLOCAL virtual bool macEvent ( EventHandlerCallRef caller, EventRef event ) { \
      qobj->macEvent(event);						\
   }*/								\
   DLLLOCAL virtual void mouseDoubleClickEvent ( QMouseEvent * event ) { \
      qobj->mouseDoubleClickEvent(event); \
   }\
   DLLLOCAL virtual void mouseMoveEvent ( QMouseEvent * event ) {	\
      qobj->mouseMoveEvent(event);					\
   }\
   DLLLOCAL virtual void mousePressEvent ( QMouseEvent * event ) {	\
      qobj->mousePressEvent(event); \
   }\
   DLLLOCAL virtual void mouseReleaseEvent ( QMouseEvent * event ) {	\
      qobj->mouseReleaseEvent(event); \
   }\
   DLLLOCAL virtual void moveEvent ( QMoveEvent * event ) {	\
      qobj->moveEvent(event); \
   }\
   DLLLOCAL virtual void paintEvent ( QPaintEvent * event ) {	\
      qobj->paintEvent(event); \
   }\
   /*DLLLOCAL virtual bool qwsEvent ( QWSEvent * event ) {	\
      qobj->qwsEvent(event);					\
   }*/							\
   DLLLOCAL virtual void resizeEvent ( QResizeEvent * event ) {\
      qobj->resizeEvent(event); \
   }\
   DLLLOCAL virtual void showEvent ( QShowEvent * event ) {	\
      qobj->showEvent(event); \
   }\
   DLLLOCAL virtual void tabletEvent ( QTabletEvent * event ) {	\
      qobj->tabletEvent(event); \
   }\
   DLLLOCAL virtual void wheelEvent ( QWheelEvent * event ) {	\
      qobj->wheelEvent(event); \
   }\
   /*DLLLOCAL virtual bool winEvent ( MSG * message, long * result ) {	\
      qobj->winEvent(event);						\
   }									\
   DLLLOCAL virtual bool x11Event ( XEvent * event ) {		\
      qobj->x11Event(event);						\
   }*/ \
   DLLLOCAL virtual QSize sizeHint() const { \
      return qobj->sizeHint();\
   }

class QoreAbstractQWidget : public QoreAbstractQObject, public QoreAbstractQPaintDevice
{
   public:
      DLLLOCAL virtual QWidget *getQWidget() const = 0;
      DLLLOCAL virtual void actionEvent(QActionEvent * event) = 0;
      DLLLOCAL virtual void changeEvent(QEvent * event) = 0;
};

#endif
