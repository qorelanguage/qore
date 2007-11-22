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

class QoreQWidgetExtension : public QoreQObjectExtension
{
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
	 //*p_getDC,
	 *p_heightForWidth,
	 *p_inputMethodQuery,
	 *p_minimumSizeHint,
	 //*p_paintEngine,
	 //*p_releaseDC,
	 *p_setVisible,
	 *p_sizeHint,
	 ;

   public:
      DLLLOCAL QoreQWidgetExtension(QoreClass *qc) : QoreQObjectExtension(qc)
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

	 //p_getDC                  = findMethod(qc, "getDC");
	 p_heightForWidth         = findMethod(qc, "heightForWidth");
	 p_inputMethodQuery       = findMethod(qc, "inputMethodQuery");
	 p_minimumSizeHint        = findMethod(qc, "minimumSizeHint");
	 //p_paintEngine            = findMethod(qc, "paintEngine");
	 //p_releaseDC              = findMethod(qc, "releaseDC");
	 p_setVisible             = findMethod(qc, "setVisible");
	 p_sizeHint               = findMethod(qc, "sizeHint");
      }
};

class QoreAbstractQWidget : public QoreAbstractQObject, public QoreAbstractQPaintDevice
{
   public:
      DLLLOCAL virtual QWidget *getQWidget() const = 0;

      // event methods
      DLLLOCAL virtual void actionEvent(QActionEvent * event) = 0;
      DLLLOCAL virtual void changeEvent(QEvent * event) = 0;
      DLLLOCAL virtual void closeEvent(QCloseEvent * event) = 0;
      DLLLOCAL virtual void contextMenuEvent(QContextMenuEvent * event) = 0;
      DLLLOCAL virtual void dragEnterEvent(QDragEnterEvent * event) = 0;
      DLLLOCAL virtual void dragLeaveEvent(QDragLeaveEvent * event) = 0;
      DLLLOCAL virtual void dragMoveEvent(QDragMoveEvent * event) = 0;
      DLLLOCAL virtual void dropEvent(QDropEvent * event) = 0;
      DLLLOCAL virtual void enterEvent(QEvent * event) = 0;
      DLLLOCAL virtual bool event(QEvent * event) = 0;
      DLLLOCAL virtual void focusInEvent(QFocusEvent * event) = 0;
      DLLLOCAL virtual void focusOutEvent(QFocusEvent * event) = 0;
      DLLLOCAL virtual void hideEvent(QHideEvent * event) = 0;
      DLLLOCAL virtual void inputMethodEvent(QInputMethodEvent * event) = 0;
      DLLLOCAL virtual void keyPressEvent(QKeyEvent * event) = 0;
      DLLLOCAL virtual void keyReleaseEvent(QKeyEvent * event) = 0;
      DLLLOCAL virtual void leaveEvent(QEvent * event) = 0;
      //DLLLOCAL virtual bool macEvent(EventHandlerCallRef caller, EventRef event) = 0;
      DLLLOCAL virtual void mouseDoubleClickEvent(QMouseEvent * event) = 0;
      DLLLOCAL virtual void mouseMoveEvent(QMouseEvent * event) = 0;
      DLLLOCAL virtual void mousePressEvent(QMouseEvent * event) = 0;
      DLLLOCAL virtual void mouseReleaseEvent(QMouseEvent * event) = 0;
      DLLLOCAL virtual void moveEvent(QMoveEvent * event) = 0;
      DLLLOCAL virtual void paintEvent(QPaintEvent * event) = 0;
      //DLLLOCAL virtual bool qwsEvent(QWSEvent * event) = 0;
      DLLLOCAL virtual void resizeEvent(QResizeEvent * event) = 0;
      DLLLOCAL virtual void showEvent(QShowEvent * event) = 0;
      DLLLOCAL virtual void tabletEvent(QTabletEvent * event) = 0;
      DLLLOCAL virtual void wheelEvent(QWheelEvent * event) = 0;
      //DLLLOCAL virtual bool winEvent(MSG * message, long * result) = 0;
      //DLLLOCAL virtual bool x11Event(XEvent * event) = 0;

      // other virtual methods
      //DLLLOCAL virtual HDC getDC () const = 0;
      DLLLOCAL virtual int heightForWidth ( int w ) const = 0;
      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const = 0;
      DLLLOCAL virtual QSize minimumSizeHint () const = 0;
      //DLLLOCAL virtual QPaintEngine * paintEngine () const = 0;
      //DLLLOCAL virtual void releaseDC ( HDC hdc ) const = 0;
      DLLLOCAL virtual void setVisible ( bool visible ) = 0;
      DLLLOCAL virtual QSize sizeHint() const = 0;
};

#define QORE_VIRTUAL_QWIDGET_METHODS QORE_VIRTUAL_QOBJECT_METHODS \
   DLLLOCAL virtual void actionEvent(QActionEvent * event) { \
      qobj->parent_actionEvent(event);		    \
   }\
   DLLLOCAL virtual void changeEvent(QEvent * event) {	\
      qobj->parent_changeEvent(event); \
   }\
   DLLLOCAL virtual void closeEvent(QCloseEvent * event) {\
      qobj->parent_closeEvent(event); \
   }\
   DLLLOCAL virtual void contextMenuEvent ( QContextMenuEvent * event ) {\
      qobj->parent_contextMenuEvent(event); \
   } \
   DLLLOCAL virtual void dragEnterEvent ( QDragEnterEvent * event ) {\
      qobj->parent_dragEnterEvent(event); \
   }\
   DLLLOCAL virtual void dragLeaveEvent ( QDragLeaveEvent * event ) {\
      qobj->parent_dragLeaveEvent(event); \
   }\
   DLLLOCAL virtual void dragMoveEvent ( QDragMoveEvent * event )  {\
      qobj->parent_dragMoveEvent(event); \
   }\
   DLLLOCAL virtual void dropEvent ( QDropEvent * event ) {	\
      qobj->parent_dropEvent(event); \
   }\
   DLLLOCAL virtual void enterEvent ( QEvent * event ) {	\
      qobj->parent_enterEvent(event); \
   }\
   DLLLOCAL virtual bool event ( QEvent * event ) {	\
      return qobj->parent_event(event); \
   }\
   DLLLOCAL virtual void focusInEvent ( QFocusEvent * event ) {	\
      qobj->parent_focusInEvent(event); \
   }\
   DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event ) {	\
      qobj->parent_focusOutEvent(event); \
   }\
   DLLLOCAL virtual void hideEvent ( QHideEvent * event ) {	\
      qobj->parent_hideEvent(event); \
   }\
   DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event ) { \
      qobj->parent_inputMethodEvent(event); \
   }\
   DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event ) {	\
      qobj->parent_keyPressEvent(event); \
   }\
   DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event ) {	\
      qobj->parent_keyReleaseEvent(event); \
   }\
   DLLLOCAL virtual void leaveEvent ( QEvent * event ) {	\
      qobj->parent_leaveEvent(event); \
   }\
   /*DLLLOCAL virtual bool macEvent ( EventHandlerCallRef caller, EventRef event ) { \
      qobj->parent_macEvent(event);						\
   }*/								\
   DLLLOCAL virtual void mouseDoubleClickEvent ( QMouseEvent * event ) { \
      qobj->parent_mouseDoubleClickEvent(event); \
   }\
   DLLLOCAL virtual void mouseMoveEvent ( QMouseEvent * event ) {	\
      qobj->parent_mouseMoveEvent(event);					\
   }\
   DLLLOCAL virtual void mousePressEvent ( QMouseEvent * event ) {	\
      qobj->parent_mousePressEvent(event); \
   }\
   DLLLOCAL virtual void mouseReleaseEvent ( QMouseEvent * event ) {	\
      qobj->parent_mouseReleaseEvent(event); \
   }\
   DLLLOCAL virtual void moveEvent ( QMoveEvent * event ) {	\
      qobj->parent_moveEvent(event); \
   }\
   DLLLOCAL virtual void paintEvent ( QPaintEvent * event ) {	\
      qobj->parent_paintEvent(event); \
   }\
   /*DLLLOCAL virtual bool qwsEvent ( QWSEvent * event ) {	\
      qobj->parent_qwsEvent(event);					\
   }*/							\
   DLLLOCAL virtual void resizeEvent ( QResizeEvent * event ) {\
      qobj->parent_resizeEvent(event); \
   }\
   DLLLOCAL virtual void showEvent ( QShowEvent * event ) {	\
      qobj->parent_showEvent(event); \
   }\
   DLLLOCAL virtual void tabletEvent ( QTabletEvent * event ) {	\
      qobj->parent_tabletEvent(event); \
   }\
   DLLLOCAL virtual void wheelEvent ( QWheelEvent * event ) {	\
      qobj->parent_wheelEvent(event); \
   }\
   /*DLLLOCAL virtual bool winEvent ( MSG * message, long * result ) {	\
      qobj->parent_winEvent(event);						\
   }									\
   DLLLOCAL virtual bool x11Event ( XEvent * event ) {		\
      qobj->parent_x11Event(event);						\
   }*/ \
   DLLLOCAL virtual int heightForWidth ( int w ) const { \
      return qobj->parent_heightForWidth(w); \
   } \
   DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const { \
      return qobj->parent_inputMethodQuery(query); \
   } \
   DLLLOCAL virtual QSize minimumSizeHint () const { \
      return qobj->parent_minimumSizeHint(); \
   } \
   /*DLLLOCAL virtual QPaintEngine * paintEngine () const {	\
      return qobj->parent_paintEngine();				\
   }								\
   DLLLOCAL virtual void releaseDC ( HDC hdc ) const {	\
      qobj->parent_releaseDC(hdc);				\
   }*/							\
   DLLLOCAL virtual void setVisible ( bool visible ) { \
      qobj->parent_setVisible(visible); \
   } \
   DLLLOCAL virtual QSize sizeHint() const { \
      return qobj->parent_sizeHint(); \
   }

#endif
