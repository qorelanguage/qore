/*
 QC_QToolBar.cc
 
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

#include "QC_QToolBar.h"
#include "QC_QWidget.h"
#include "QC_QObject.h"
#include "QC_QAction.h"
#include "QC_QPoint.h"
#include "QC_QIcon.h"

#include "qore-qt.h"

int CID_QTOOLBAR;
class QoreClass *QC_QToolBar = 0;

//QToolBar ( const QString & title, QWidget * parent = 0 )
//QToolBar ( QWidget * parent = 0 )
static void QTOOLBAR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self));
      return;
   }
   QString title;
   if (!get_qstring(p, title, xsink, true)) {
      if (*xsink)
	 return;

      const QoreObject *o = dynamic_cast<const QoreObject *>(p);
      QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self, parent ? parent->getQWidget() : 0));
      return;
   }
   const QoreObject *o = test_object_param(params, 1);
   QoreQWidget *parent = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTOOLBAR, new QoreQToolBar(self, title, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTOOLBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQToolBar *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTOOLBAR-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * actionAt ( const QPoint & p ) const
//QAction * actionAt ( int x, int y ) const
static AbstractQoreNode *QTOOLBAR_actionAt(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         if (!xsink->isException())
            xsink->raiseException("QTOOLBAR-ACTIONAT-PARAM-ERROR", "QToolBar::actionAt() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      return return_qaction(qtb->qobj->actionAt(*(static_cast<QPoint *>(point))));
   }
   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return return_qaction(qtb->qobj->actionAt(x, y));
}

//void addAction ( QAction * action )
//QAction * addAction ( const QString & text )
//QAction * addAction ( const QIcon & icon, const QString & text )
//QAction * addAction ( const QString & text, const QObject * receiver, const char * member )
//QAction * addAction ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member )
static AbstractQoreNode *QTOOLBAR_addAction(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);

   QString text;
   if (!get_qstring(p, text, xsink, true)) {
      if (num_params(params) == 1)
	 return return_qaction(qtb->qobj->addAction(text));

      const QoreObject *o = test_object_param(params, 1);
      QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (!receiver) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "this version of QToolBar::addAction() expects an object derived from QObject as the third argument");
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

      const QoreStringNode *pstr = test_string_param(params, 2);
      if (!pstr) {
	 xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "expecting a string as fourth argument to QToolBar::addAction()");
	 return 0;
      }
      const char *member = pstr->getBuffer();
      return return_qaction(qtb->qobj->addAction(text, receiver->getQObject(), member));
   }
   if (*xsink)
      return 0;
   if (!p | p->type != NT_OBJECT) {
      xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "QToolBar::addAction() does not know how to handle arguments of type '%s' as the first argument", p ? p->getTypeName() : "NOTHING");
      return 0;
   }

   const QoreObject *o = reinterpret_cast<const QoreObject *>(p);
   QoreQIcon *icon = (QoreQIcon *)o->getReferencedPrivateData(CID_QICON, xsink);
   if (!icon) {
      QoreQAction *action = (QoreQAction *)o->getReferencedPrivateData(CID_QACTION, xsink);
      if (!action) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "QToolBar::addAction() does not know how to handle arguments of class '%s' as passed as the first argument", o->getClassName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
      qtb->qobj->addAction(static_cast<QAction *>(action->getQAction()));
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);

   p = get_param(params, 1);
   if (get_qstring(p, text, xsink))
      return 0;

   o = test_object_param(params, 2);
   QoreAbstractQObject *receiver = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "this version of QToolBar::addAction() expects an object derived from QObject as the third argument");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

   const QoreStringNode *pstr = test_string_param(params, 3);
   if (!pstr) {
      xsink->raiseException("QTOOLBAR-ADDACTION-PARAM-ERROR", "expecting a string as fourth argument to QToolBar::addAction()");
      return 0;
   }
   const char *member = pstr->getBuffer();
   return return_qaction(qtb->qobj->addAction(*(static_cast<QIcon *>(icon)), text, receiver->getQObject(), member));
}

//QAction * addSeparator ()
static AbstractQoreNode *QTOOLBAR_addSeparator(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qaction(qtb->qobj->addSeparator());
}

//QAction * addWidget ( QWidget * widget )
static AbstractQoreNode *QTOOLBAR_addWidget(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQWidget *widget = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-ADDWIDGET-PARAM-ERROR", "expecting a QWidget object as first argument to QToolBar::addWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return return_qaction(qtb->qobj->addWidget(static_cast<QWidget *>(widget->getQWidget())));
}

//Qt::ToolBarAreas allowedAreas () const
static AbstractQoreNode *QTOOLBAR_allowedAreas(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtb->qobj->allowedAreas());
}

//void clear ()
static AbstractQoreNode *QTOOLBAR_clear(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   qtb->qobj->clear();
   return 0;
}

//QSize iconSize () const
static AbstractQoreNode *QTOOLBAR_iconSize(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->qobj->iconSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//QAction * insertSeparator ( QAction * before )
static AbstractQoreNode *QTOOLBAR_insertSeparator(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAction *before = p ? (QoreQAction *)p->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTSEPARATOR-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::insertSeparator()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);
   return return_qaction(qtb->qobj->insertSeparator(static_cast<QAction *>(before->getQAction())));
}

//QAction * insertWidget ( QAction * before, QWidget * widget )
static AbstractQoreNode *QTOOLBAR_insertWidget(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *o = test_object_param(params, 0);
   QoreQAction *before = o ? (QoreQAction *)o->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTWIDGET-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);

   o = test_object_param(params, 1);
   QoreQWidget *widget = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-INSERTWIDGET-PARAM-ERROR", "expecting a QWidget object as second argument to QToolBar::insertWidget()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
   return return_qaction(qtb->qobj->insertWidget(static_cast<QAction *>(before->getQAction()), static_cast<QWidget *>(widget->getQWidget())));
}

//bool isAreaAllowed ( Qt::ToolBarArea area ) const
static AbstractQoreNode *QTOOLBAR_isAreaAllowed(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ToolBarArea area = (Qt::ToolBarArea)(p ? p->getAsInt() : 0);
   return new QoreBoolNode(qtb->qobj->isAreaAllowed(area));
}

//bool isFloatable () const
static AbstractQoreNode *QTOOLBAR_isFloatable(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtb->qobj->isFloatable());
}

//bool isFloating () const
static AbstractQoreNode *QTOOLBAR_isFloating(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtb->qobj->isFloating());
}

//bool isMovable () const
static AbstractQoreNode *QTOOLBAR_isMovable(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qtb->qobj->isMovable());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QTOOLBAR_orientation(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtb->qobj->orientation());
}

//void setAllowedAreas ( Qt::ToolBarAreas areas )
static AbstractQoreNode *QTOOLBAR_setAllowedAreas(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::ToolBarAreas areas = (Qt::ToolBarAreas)(p ? p->getAsInt() : 0);
   qtb->qobj->setAllowedAreas(areas);
   return 0;
}

//void setFloatable ( bool floatable )
static AbstractQoreNode *QTOOLBAR_setFloatable(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool floatable = p ? p->getAsBool() : false;
   qtb->qobj->setFloatable(floatable);
   return 0;
}

//void setMovable ( bool movable )
static AbstractQoreNode *QTOOLBAR_setMovable(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool movable = p ? p->getAsBool() : false;
   qtb->qobj->setMovable(movable);
   return 0;
}

//void setOrientation ( Qt::Orientation orientation )
static AbstractQoreNode *QTOOLBAR_setOrientation(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qtb->qobj->setOrientation(orientation);
   return 0;
}

//QAction * toggleViewAction () const
static AbstractQoreNode *QTOOLBAR_toggleViewAction(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qaction(qtb->qobj->toggleViewAction());
}

//Qt::ToolButtonStyle toolButtonStyle () const
static AbstractQoreNode *QTOOLBAR_toolButtonStyle(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qtb->qobj->toolButtonStyle());
}

//QWidget * widgetForAction ( QAction * action ) const
static AbstractQoreNode *QTOOLBAR_widgetForAction(QoreObject *self, QoreQToolBar *qtb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAction *action = p ? (QoreQAction *)p->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBAR-WIDGETFORACTION-PARAM-ERROR", "expecting a QAction object as first argument to QToolBar::widgetForAction()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
   return return_qwidget(qtb->qobj->widgetForAction(static_cast<QAction *>(action->getQAction())));
}

static QoreClass *initQToolBarClass(QoreClass *qwidget)
{
   QC_QToolBar = new QoreClass("QToolBar", QDOM_GUI);
   CID_QTOOLBAR = QC_QToolBar->getID();

   QC_QToolBar->addBuiltinVirtualBaseClass(qwidget);

   QC_QToolBar->setConstructor(QTOOLBAR_constructor);
   QC_QToolBar->setCopy((q_copy_t)QTOOLBAR_copy);

   QC_QToolBar->addMethod("actionAt",                    (q_method_t)QTOOLBAR_actionAt);
   QC_QToolBar->addMethod("addAction",                   (q_method_t)QTOOLBAR_addAction);
   QC_QToolBar->addMethod("addSeparator",                (q_method_t)QTOOLBAR_addSeparator);
   QC_QToolBar->addMethod("addWidget",                   (q_method_t)QTOOLBAR_addWidget);
   QC_QToolBar->addMethod("allowedAreas",                (q_method_t)QTOOLBAR_allowedAreas);
   QC_QToolBar->addMethod("clear",                       (q_method_t)QTOOLBAR_clear);
   QC_QToolBar->addMethod("iconSize",                    (q_method_t)QTOOLBAR_iconSize);
   QC_QToolBar->addMethod("insertSeparator",             (q_method_t)QTOOLBAR_insertSeparator);
   QC_QToolBar->addMethod("insertWidget",                (q_method_t)QTOOLBAR_insertWidget);
   QC_QToolBar->addMethod("isAreaAllowed",               (q_method_t)QTOOLBAR_isAreaAllowed);
   QC_QToolBar->addMethod("isFloatable",                 (q_method_t)QTOOLBAR_isFloatable);
   QC_QToolBar->addMethod("isFloating",                  (q_method_t)QTOOLBAR_isFloating);
   QC_QToolBar->addMethod("isMovable",                   (q_method_t)QTOOLBAR_isMovable);
   QC_QToolBar->addMethod("orientation",                 (q_method_t)QTOOLBAR_orientation);
   QC_QToolBar->addMethod("setAllowedAreas",             (q_method_t)QTOOLBAR_setAllowedAreas);
   QC_QToolBar->addMethod("setFloatable",                (q_method_t)QTOOLBAR_setFloatable);
   QC_QToolBar->addMethod("setMovable",                  (q_method_t)QTOOLBAR_setMovable);
   QC_QToolBar->addMethod("setOrientation",              (q_method_t)QTOOLBAR_setOrientation);
   QC_QToolBar->addMethod("toggleViewAction",            (q_method_t)QTOOLBAR_toggleViewAction);
   QC_QToolBar->addMethod("toolButtonStyle",             (q_method_t)QTOOLBAR_toolButtonStyle);
   QC_QToolBar->addMethod("widgetForAction",             (q_method_t)QTOOLBAR_widgetForAction);

   return QC_QToolBar;
}

QoreNamespace *initQToolBarNS(QoreClass *qwidget)
{
   QoreNamespace *ns = new QoreNamespace("QToolBar");
   ns->addSystemClass(initQToolBarClass(qwidget));

   return ns;
}
