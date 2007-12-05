/*
 QC_QMainWindow.cc
 
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

#include "QC_QMainWindow.h"

int CID_QMAINWINDOW;
class QoreClass *QC_QMainWindow = 0;

//QMainWindow ( QWidget * parent = 0, Qt::WindowFlags flags = 0 )
static void QMAINWINDOW_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!is_nothing(p) && !parent) {
      if (!xsink->isException())
         xsink->raiseException("QMAINWINDOW-CONSTRUCTOR-PARAM-ERROR", "expecting wither NOTHING or a QWidget object as first argument to QMainWindow::constructor()");
      return;
   }
   ReferenceHolder<QoreQWidget> parentHolder(parent, xsink);
   p = get_param(params, 1);
   Qt::WindowFlags flags = (Qt::WindowFlags)(p ? p->getAsInt() : 0);
   self->setPrivate(CID_QMAINWINDOW, new QoreQMainWindow(self, parent ? parent->getQWidget() : 0, flags));
   return;
}

static void QMAINWINDOW_copy(class QoreObject *self, class QoreObject *old, class QoreQMainWindow *qmw, ExceptionSink *xsink)
{
   xsink->raiseException("QMAINWINDOW-COPY-ERROR", "objects of this class cannot be copied");
}

////void addDockWidget ( Qt::DockWidgetArea area, QDockWidget * dockwidget )
////void addDockWidget ( Qt::DockWidgetArea area, QDockWidget * dockwidget, Qt::Orientation orientation )
//static QoreNode *QMAINWINDOW_addDockWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   Qt::DockWidgetArea area = (Qt::DockWidgetArea)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   ??? QDockWidget* dockwidget = p;
//   qmw->qobj->addDockWidget(area, dockwidget);
//   return 0;
//}

////void addToolBar ( Qt::ToolBarArea area, QToolBar * toolbar )
////void addToolBar ( QToolBar * toolbar )
////QToolBar * addToolBar ( const QString & title )
//static QoreNode *QMAINWINDOW_addToolBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_???) {
//      ??? QToolBar* toolbar = p;
//      qmw->qobj->addToolBar(toolbar);
//      return 0;
//   }
//   if (p && p->type == NT_STRING) {
//      if (!p || p->type != NT_STRING) {
//         xsink->raiseException("QMAINWINDOW-ADDTOOLBAR-PARAM-ERROR", "expecting a string as first argument to QMainWindow::addToolBar()");
//         return 0;
//      }
//      const char *title = p->val.String->getBuffer();
//      qmw->qobj->addToolBar(title);
//      return 0;
//   }
//   Qt::ToolBarArea area = (Qt::ToolBarArea)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   ??? QToolBar* toolbar = p;
//   qmw->qobj->addToolBar(area, toolbar);
//   return 0;
//}

//void addToolBarBreak ( Qt::ToolBarArea area = Qt::TopToolBarArea )
static QoreNode *QMAINWINDOW_addToolBarBreak(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolBarArea area = (Qt::ToolBarArea)(p ? p->getAsInt() : 0);
   qmw->qobj->addToolBarBreak(area);
   return 0;
}

//QWidget * centralWidget () const
static QoreNode *QMAINWINDOW_centralWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qmw->qobj->centralWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//Qt::DockWidgetArea corner ( Qt::Corner corner ) const
static QoreNode *QMAINWINDOW_corner(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Corner corner = (Qt::Corner)(p ? p->getAsInt() : 0);
   return new QoreNode((int64)qmw->qobj->corner(corner));
}

//virtual QMenu * createPopupMenu ()
static QoreNode *QMAINWINDOW_createPopupMenu(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QMenu *qt_qobj = qmw->qobj->createPopupMenu();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

//DockOptions dockOptions () const
static QoreNode *QMAINWINDOW_dockOptions(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmw->qobj->dockOptions());
}

////Qt::DockWidgetArea dockWidgetArea ( QDockWidget * dockwidget ) const
//static QoreNode *QMAINWINDOW_dockWidgetArea(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QDockWidget* dockwidget = p;
//   return new QoreNode((int64)qmw->qobj->dockWidgetArea(dockwidget));
//}

//QSize iconSize () const
static QoreNode *QMAINWINDOW_iconSize(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qmw->qobj->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

////void insertToolBar ( QToolBar * before, QToolBar * toolbar )
//static QoreNode *QMAINWINDOW_insertToolBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* before = p;
//   p = get_param(params, 1);
//   ??? QToolBar* toolbar = p;
//   qmw->qobj->insertToolBar(before, toolbar);
//   return 0;
//}

////void insertToolBarBreak ( QToolBar * before )
//static QoreNode *QMAINWINDOW_insertToolBarBreak(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* before = p;
//   qmw->qobj->insertToolBarBreak(before);
//   return 0;
//}

//bool isAnimated () const
static QoreNode *QMAINWINDOW_isAnimated(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmw->qobj->isAnimated());
}

//bool isDockNestingEnabled () const
static QoreNode *QMAINWINDOW_isDockNestingEnabled(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmw->qobj->isDockNestingEnabled());
}

//QMenuBar * menuBar () const
static QoreNode *QMAINWINDOW_menuBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QMenuBar *qt_qobj = qmw->qobj->menuBar();
   if (!qt_qobj)
      return 0;

   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QMenuBar, getProgram());
      QoreQtQMenuBar *qmb = new QoreQtQMenuBar(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QMENUBAR, qmb);
   }
   return new QoreNode(rv_obj);
}

//QWidget * menuWidget () const
static QoreNode *QMAINWINDOW_menuWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QWidget *qt_qobj = qmw->qobj->menuWidget();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return new QoreNode(rv_obj);
}

////void removeDockWidget ( QDockWidget * dockwidget )
//static QoreNode *QMAINWINDOW_removeDockWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QDockWidget* dockwidget = p;
//   qmw->qobj->removeDockWidget(dockwidget);
//   return 0;
//}

////void removeToolBar ( QToolBar * toolbar )
//static QoreNode *QMAINWINDOW_removeToolBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* toolbar = p;
//   qmw->qobj->removeToolBar(toolbar);
//   return 0;
//}

////void removeToolBarBreak ( QToolBar * before )
//static QoreNode *QMAINWINDOW_removeToolBarBreak(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* before = p;
//   qmw->qobj->removeToolBarBreak(before);
//   return 0;
//}

//bool restoreState ( const QByteArray & state, int version = 0 )
static QoreNode *QMAINWINDOW_restoreState(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QByteArray state;
   if (get_qbytearray(p, state, xsink))
      return 0;
   p = get_param(params, 1);
   int version = p ? p->getAsInt() : 0;
   return new QoreNode(qmw->qobj->restoreState(state, version));
}

//QByteArray saveState ( int version = 0 ) const
static QoreNode *QMAINWINDOW_saveState(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int version = p ? p->getAsInt() : 0;
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qmw->qobj->saveState(version));
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return new QoreNode(o_qba);
}

//void setCentralWidget ( QWidget * widget )
static QoreNode *QMAINWINDOW_setCentralWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QMAINWINDOW-SETCENTRALWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QMainWindow::setCentralWidget()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> widgetHolder(widget, xsink);
   qmw->qobj->setCentralWidget(static_cast<QWidget *>(widget->getQWidget()));
   return 0;
}

//void setCorner ( Qt::Corner corner, Qt::DockWidgetArea area )
static QoreNode *QMAINWINDOW_setCorner(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Corner corner = (Qt::Corner)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   Qt::DockWidgetArea area = (Qt::DockWidgetArea)(p ? p->getAsInt() : 0);
   qmw->qobj->setCorner(corner, area);
   return 0;
}

//void setDockOptions ( DockOptions options )
static QoreNode *QMAINWINDOW_setDockOptions(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QMainWindow::DockOptions options = (QMainWindow::DockOptions)(p ? p->getAsInt() : 0);
   qmw->qobj->setDockOptions(options);
   return 0;
}

//void setIconSize ( const QSize & iconSize )
static QoreNode *QMAINWINDOW_setIconSize(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQSize *iconSize = (p && p->type == NT_OBJECT) ? (QoreQSize *)p->val.object->getReferencedPrivateData(CID_QSIZE, xsink) : 0;
   if (!iconSize) {
      if (!xsink->isException())
         xsink->raiseException("QMAINWINDOW-SETICONSIZE-PARAM-ERROR", "expecting a QSize object as first argument to QMainWindow::setIconSize()");
      return 0;
   }
   ReferenceHolder<QoreQSize> iconSizeHolder(iconSize, xsink);
   qmw->qobj->setIconSize(*(static_cast<QSize *>(iconSize)));
   return 0;
}

//void setMenuBar ( QMenuBar * menuBar )
static QoreNode *QMAINWINDOW_setMenuBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQMenuBar *menuBar = (p && p->type == NT_OBJECT) ? (QoreQMenuBar *)p->val.object->getReferencedPrivateData(CID_QMENUBAR, xsink) : 0;
   if (!menuBar) {
      if (!xsink->isException())
         xsink->raiseException("QMAINWINDOW-SETMENUBAR-PARAM-ERROR", "expecting a QMenuBar object as first argument to QMainWindow::setMenuBar()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> menuBarHolder(static_cast<AbstractPrivateData *>(menuBar), xsink);
   qmw->qobj->setMenuBar(static_cast<QMenuBar *>(menuBar->getQMenuBar()));
   return 0;
}

//void setMenuWidget ( QWidget * menuBar )
static QoreNode *QMAINWINDOW_setMenuWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *menuBar = (p && p->type == NT_OBJECT) ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!menuBar) {
      if (!xsink->isException())
         xsink->raiseException("QMAINWINDOW-SETMENUWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QMainWindow::setMenuWidget()");
      return 0;
   }
   ReferenceHolder<QoreQWidget> menuBarHolder(menuBar, xsink);
   qmw->qobj->setMenuWidget(static_cast<QWidget *>(menuBar->getQWidget()));
   return 0;
}

////void setStatusBar ( QStatusBar * statusbar )
//static QoreNode *QMAINWINDOW_setStatusBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QStatusBar* statusbar = p;
//   qmw->qobj->setStatusBar(statusbar);
//   return 0;
//}

//void setToolButtonStyle ( Qt::ToolButtonStyle toolButtonStyle )
static QoreNode *QMAINWINDOW_setToolButtonStyle(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolButtonStyle toolButtonStyle = (Qt::ToolButtonStyle)(p ? p->getAsInt() : 0);
   qmw->qobj->setToolButtonStyle(toolButtonStyle);
   return 0;
}

//void setUnifiedTitleAndToolBarOnMac ( bool set )
static QoreNode *QMAINWINDOW_setUnifiedTitleAndToolBarOnMac(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool set = p ? p->getAsBool() : false;
   qmw->qobj->setUnifiedTitleAndToolBarOnMac(set);
   return 0;
}

////void splitDockWidget ( QDockWidget * first, QDockWidget * second, Qt::Orientation orientation )
//static QoreNode *QMAINWINDOW_splitDockWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QDockWidget* first = p;
//   p = get_param(params, 1);
//   ??? QDockWidget* second = p;
//   p = get_param(params, 2);
//   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
//   qmw->qobj->splitDockWidget(first, second, orientation);
//   return 0;
//}

////QStatusBar * statusBar () const
//static QoreNode *QMAINWINDOW_statusBar(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qmw->qobj->statusBar());
//}

////void tabifyDockWidget ( QDockWidget * first, QDockWidget * second )
//static QoreNode *QMAINWINDOW_tabifyDockWidget(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QDockWidget* first = p;
//   p = get_param(params, 1);
//   ??? QDockWidget* second = p;
//   qmw->qobj->tabifyDockWidget(first, second);
//   return 0;
//}

////Qt::ToolBarArea toolBarArea ( QToolBar * toolbar ) const
//static QoreNode *QMAINWINDOW_toolBarArea(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* toolbar = p;
//   return new QoreNode((int64)qmw->qobj->toolBarArea(toolbar));
//}

////bool toolBarBreak ( QToolBar * toolbar ) const
//static QoreNode *QMAINWINDOW_toolBarBreak(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QToolBar* toolbar = p;
//   return new QoreNode(qmw->qobj->toolBarBreak(toolbar));
//}

//Qt::ToolButtonStyle toolButtonStyle () const
static QoreNode *QMAINWINDOW_toolButtonStyle(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qmw->qobj->toolButtonStyle());
}

//bool unifiedTitleAndToolBarOnMac () const
static QoreNode *QMAINWINDOW_unifiedTitleAndToolBarOnMac(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qmw->qobj->unifiedTitleAndToolBarOnMac());
}

//void setAnimated ( bool enabled )
static QoreNode *QMAINWINDOW_setAnimated(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qmw->qobj->setAnimated(enabled);
   return 0;
}

//void setDockNestingEnabled ( bool enabled )
static QoreNode *QMAINWINDOW_setDockNestingEnabled(QoreObject *self, QoreQMainWindow *qmw, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enabled = p ? p->getAsBool() : false;
   qmw->qobj->setDockNestingEnabled(enabled);
   return 0;
}

QoreClass *initQMainWindowClass(QoreClass *qwidget)
{
   QC_QMainWindow = new QoreClass("QMainWindow", QDOM_GUI);
   CID_QMAINWINDOW = QC_QMainWindow->getID();

   QC_QMainWindow->addBuiltinVirtualBaseClass(qwidget);

   QC_QMainWindow->setConstructor(QMAINWINDOW_constructor);
   QC_QMainWindow->setCopy((q_copy_t)QMAINWINDOW_copy);

   //QC_QMainWindow->addMethod("addDockWidget",               (q_method_t)QMAINWINDOW_addDockWidget);
   //QC_QMainWindow->addMethod("addToolBar",                  (q_method_t)QMAINWINDOW_addToolBar);
   QC_QMainWindow->addMethod("addToolBarBreak",             (q_method_t)QMAINWINDOW_addToolBarBreak);
   QC_QMainWindow->addMethod("centralWidget",               (q_method_t)QMAINWINDOW_centralWidget);
   QC_QMainWindow->addMethod("corner",                      (q_method_t)QMAINWINDOW_corner);
   QC_QMainWindow->addMethod("createPopupMenu",             (q_method_t)QMAINWINDOW_createPopupMenu);
   QC_QMainWindow->addMethod("dockOptions",                 (q_method_t)QMAINWINDOW_dockOptions);
   //QC_QMainWindow->addMethod("dockWidgetArea",              (q_method_t)QMAINWINDOW_dockWidgetArea);
   QC_QMainWindow->addMethod("iconSize",                    (q_method_t)QMAINWINDOW_iconSize);
   //QC_QMainWindow->addMethod("insertToolBar",               (q_method_t)QMAINWINDOW_insertToolBar);
   //QC_QMainWindow->addMethod("insertToolBarBreak",          (q_method_t)QMAINWINDOW_insertToolBarBreak);
   QC_QMainWindow->addMethod("isAnimated",                  (q_method_t)QMAINWINDOW_isAnimated);
   QC_QMainWindow->addMethod("isDockNestingEnabled",        (q_method_t)QMAINWINDOW_isDockNestingEnabled);
   QC_QMainWindow->addMethod("menuBar",                     (q_method_t)QMAINWINDOW_menuBar);
   QC_QMainWindow->addMethod("menuWidget",                  (q_method_t)QMAINWINDOW_menuWidget);
   //QC_QMainWindow->addMethod("removeDockWidget",            (q_method_t)QMAINWINDOW_removeDockWidget);
   //QC_QMainWindow->addMethod("removeToolBar",               (q_method_t)QMAINWINDOW_removeToolBar);
   //QC_QMainWindow->addMethod("removeToolBarBreak",          (q_method_t)QMAINWINDOW_removeToolBarBreak);
   QC_QMainWindow->addMethod("restoreState",                (q_method_t)QMAINWINDOW_restoreState);
   QC_QMainWindow->addMethod("saveState",                   (q_method_t)QMAINWINDOW_saveState);
   QC_QMainWindow->addMethod("setCentralWidget",            (q_method_t)QMAINWINDOW_setCentralWidget);
   QC_QMainWindow->addMethod("setCorner",                   (q_method_t)QMAINWINDOW_setCorner);
   QC_QMainWindow->addMethod("setDockOptions",              (q_method_t)QMAINWINDOW_setDockOptions);
   QC_QMainWindow->addMethod("setIconSize",                 (q_method_t)QMAINWINDOW_setIconSize);
   QC_QMainWindow->addMethod("setMenuBar",                  (q_method_t)QMAINWINDOW_setMenuBar);
   QC_QMainWindow->addMethod("setMenuWidget",               (q_method_t)QMAINWINDOW_setMenuWidget);
   //QC_QMainWindow->addMethod("setStatusBar",                (q_method_t)QMAINWINDOW_setStatusBar);
   QC_QMainWindow->addMethod("setToolButtonStyle",          (q_method_t)QMAINWINDOW_setToolButtonStyle);
   QC_QMainWindow->addMethod("setUnifiedTitleAndToolBarOnMac", (q_method_t)QMAINWINDOW_setUnifiedTitleAndToolBarOnMac);
   //QC_QMainWindow->addMethod("splitDockWidget",             (q_method_t)QMAINWINDOW_splitDockWidget);
   //QC_QMainWindow->addMethod("statusBar",                   (q_method_t)QMAINWINDOW_statusBar);
   //QC_QMainWindow->addMethod("tabifyDockWidget",            (q_method_t)QMAINWINDOW_tabifyDockWidget);
   //QC_QMainWindow->addMethod("toolBarArea",                 (q_method_t)QMAINWINDOW_toolBarArea);
   //QC_QMainWindow->addMethod("toolBarBreak",                (q_method_t)QMAINWINDOW_toolBarBreak);
   QC_QMainWindow->addMethod("toolButtonStyle",             (q_method_t)QMAINWINDOW_toolButtonStyle);
   QC_QMainWindow->addMethod("unifiedTitleAndToolBarOnMac", (q_method_t)QMAINWINDOW_unifiedTitleAndToolBarOnMac);
   QC_QMainWindow->addMethod("setAnimated",                 (q_method_t)QMAINWINDOW_setAnimated);
   QC_QMainWindow->addMethod("setDockNestingEnabled",       (q_method_t)QMAINWINDOW_setDockNestingEnabled);

   return QC_QMainWindow;
}
