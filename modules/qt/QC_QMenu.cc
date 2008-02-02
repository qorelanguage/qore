/*
 QC_QMenu.cc
 
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

#include "QC_QMenu.h"
#include "QC_QWidget.h"
#include "QC_QPoint.h"
#include "QC_QAction.h"
#include "QC_QRect.h"
#include "QC_QObject.h"
#include "QC_QIcon.h"
#include "QC_QStyleOptionMenuItem.h"

#include "qore-qt.h"

int CID_QMENU;
class QoreClass *QC_QMenu = 0;

//QMenu ( QWidget * parent = 0 )
//QMenu ( const QString & title, QWidget * parent = 0 )
static void QMENU_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_STRING) {
      QString title;

      if (get_qstring(p, title, xsink))
	 return;

      p = get_param(params, 1);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (*xsink)
	 return;
      ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
      self->setPrivate(CID_QMENU, new QoreQMenu(self, title, parent ? parent->getQWidget() : 0));
      return;
   }

   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QMENU, new QoreQMenu(self, parent ? parent->getQWidget() : 0));
}

static void QMENU_copy(class QoreObject *self, class QoreObject *old, class QoreQMenu *qm, ExceptionSink *xsink)
{
   xsink->raiseException("QMENU-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * actionAt ( const QPoint & pt ) const
static AbstractQoreNode *QMENU_actionAt(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQPoint *pt = (p && p->type == NT_OBJECT) ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pt) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-ACTIONAT-PARAM-ERROR", "expecting a QPoint object as first argument to QMenu::actionAt()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> ptHolder(pt, xsink);
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->actionAt(*(static_cast<QPoint *>(pt))));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QRect actionGeometry ( QAction * act ) const
static AbstractQoreNode *QMENU_actionGeometry(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQAction *act = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-ACTIONGEOMETRY-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::actionGeometry()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAction> actHolder(act, xsink);
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qm->getQMenu()->actionGeometry(act->getQAction()));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//QAction * activeAction () const
static AbstractQoreNode *QMENU_activeAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->activeAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * addAction ( const QString & text )
//QAction * addAction ( const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 )

//QAction * addAction ( const QIcon & icon, const QString & text )
//QAction * addAction ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 )

// here we have to create the QoreQAction separately and add it to the menu by hand and then return the QoreQAction object
static AbstractQoreNode *QMENU_addAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int offset = 0;

   QoreQIcon *icon = 0;
   if (p && p->type == NT_OBJECT) {
      icon = (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
	 QoreAbstractQAction *action = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
	 if (!action) {
	    if (!xsink->isException())
	       xsink->raiseException("QWIDGET-ADDACTION-PARAM-ERROR", "expecting a QAction object as first argument to QWidget::addAction()");
	    return 0;
	 }
	 ReferenceHolder<QoreAbstractQAction> actionHolder(action, xsink);
	 qm->getQWidget()->addAction(action->getQAction());
	 return 0;
      }
      offset = 1;
   }

   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);

   p = get_param(params, offset);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMENU-ADDACTION-PARAM-ERROR", "expecting a string as first or second argument to QMenu::addAction()");
      return 0;
   }
   QString text;
   
   if (get_qstring(p, text, xsink))
      return 0;

   p = test_param(params, NT_OBJECT, 1 + offset);
   QoreAbstractQObject *receiver = p ? (QoreAbstractQObject *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return 0;
   if (!receiver) {
      QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
      QoreQAction *q_qa;

      if (icon)
	 q_qa = new QoreQAction(o_qa, *(static_cast<QIcon *>(icon)), text, qm->qobj);
      else
	 q_qa = new QoreQAction(o_qa, text, qm->qobj);

      qm->getQMenu()->addAction(q_qa->qobj);

      o_qa->setPrivate(CID_QACTION, q_qa);
      return o_qa;
   }
   ReferenceHolder<QoreAbstractQObject> receiverHolder(receiver, xsink);
   
   QoreStringNode *pstr = test_string_param(params, 2 + offset);
   if (!pstr || !pstr->strlen()) {
      xsink->raiseException("QMENU-ADDACTION-PARAM-ERROR", "expecting a string as third or fourth argument to QMenu::addAction()");
      return 0;
   }
   const char *member = pstr->getBuffer();
   
   p = get_param(params, 3 + offset);
   QKeySequence shortcut;

   bool got_shortcut = !get_qkeysequence(p, shortcut, xsink, true);
   if (*xsink)
      return 0;

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQAction *q_qa;
   if (icon)
      q_qa = new QoreQAction(o_qa, *(static_cast<QIcon *>(icon)), text, qm->qobj);
   else
      q_qa = new QoreQAction(o_qa, text, qm->qobj);
   if (got_shortcut)
      q_qa->qobj->setShortcut(shortcut);

   // connect action's triggered() signal to the receiver's member slot
   if (receiver->connectDynamic(q_qa, "triggered()", member, xsink)) {
      q_qa->deref(xsink);
      return 0;
   }

   qm->getQMenu()->addAction(q_qa->qobj);

   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

////QAction * addMenu ( QMenu * menu )
////QMenu * addMenu ( const QString & title )
////QMenu * addMenu ( const QIcon & icon, const QString & title )
static AbstractQoreNode *QMENU_addMenu(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         QoreAbstractQMenu *menu = (QoreAbstractQMenu *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMENU, xsink);
         if (!menu) {
            if (!xsink->isException())
               xsink->raiseException("QMENU-ADDMENU-PARAM-ERROR", "QMenu::addMenu() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
            return 0;
         }
         ReferenceHolder<QoreAbstractQMenu> menuHolder(menu, xsink);
         QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
         QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->addMenu(menu->getQMenu()));
         o_qa->setPrivate(CID_QACTION, q_qa);
         return o_qa;
      }
      ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
      p = get_param(params, 1);
      QString title;
      if (get_qstring(p, title, xsink))
	 return 0;

      QoreObject *o_qa = new QoreObject(QC_QMenu, getProgram());
      QoreQMenu *q_qa = new QoreQMenu(o_qa, qm->getQMenu()->addMenu(*(static_cast<QIcon *>(icon)), title));
      o_qa->setPrivate(CID_QMENU, q_qa);
      return o_qa;
   }
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   QoreObject *o_qa = new QoreObject(QC_QMenu, getProgram());
   QoreQMenu *q_qa = new QoreQMenu(o_qa, qm->getQMenu()->addMenu(title));
   o_qa->setPrivate(CID_QMENU, q_qa);
   return o_qa;
}

//QAction * addSeparator ()
static AbstractQoreNode *QMENU_addSeparator(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->addSeparator());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//void clear ()
static AbstractQoreNode *QMENU_clear(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   qm->getQMenu()->clear();
   return 0;
}

//QAction * defaultAction () const
static AbstractQoreNode *QMENU_defaultAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->defaultAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * exec ()
//QAction * exec ( const QPoint & p, QAction * action = 0 )
static AbstractQoreNode *QMENU_exec(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
      QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->exec());
      o_qa->setPrivate(CID_QACTION, q_qa);
      return o_qa;
   }
   QoreQPoint *point = p ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-EXEC-PARAM-ERROR", "this version of QMenu::exec() expects an object derived from QPoint as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQPoint> pHolder(point, xsink);
   p = get_param(params, 1);
   QoreAbstractQAction *action = p ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   ReferenceHolder<QoreAbstractQAction> actionHolder(action, xsink);
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa;
   if (action)
      q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->exec(*(static_cast<QPoint *>(point)), action->getQAction()));
   else
      q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->exec(*(static_cast<QPoint *>(point))));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//void hideTearOffMenu ()
static AbstractQoreNode *QMENU_hideTearOffMenu(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   qm->getQMenu()->hideTearOffMenu();
   return 0;
}

//QIcon icon () const
static AbstractQoreNode *QMENU_icon(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qm->getQMenu()->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//QAction * insertMenu ( QAction * before, QMenu * menu )
static AbstractQoreNode *QMENU_insertMenu(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQAction *before = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTMENU-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::insertMenu()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAction> beforeHolder(before, xsink);
   p = get_param(params, 1);
   QoreQMenu *menu = (p && p->type == NT_OBJECT) ? (QoreQMenu *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTMENU-PARAM-ERROR", "expecting a QMenu object as second argument to QMenu::insertMenu()");
      return 0;
   }
   ReferenceHolder<QoreQMenu> menuHolder(menu, xsink);
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->insertMenu(before->getQAction(), static_cast<QMenu *>(menu->qobj)));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * insertSeparator ( QAction * before )
static AbstractQoreNode *QMENU_insertSeparator(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQAction *before = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTSEPARATOR-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::insertSeparator()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAction> beforeHolder(before, xsink);
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->insertSeparator(before->getQAction()));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//bool isEmpty () const
static AbstractQoreNode *QMENU_isEmpty(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qm->getQMenu()->isEmpty());
}

//bool isTearOffEnabled () const
static AbstractQoreNode *QMENU_isTearOffEnabled(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qm->getQMenu()->isTearOffEnabled());
}

//bool isTearOffMenuVisible () const
static AbstractQoreNode *QMENU_isTearOffMenuVisible(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qm->getQMenu()->isTearOffMenuVisible());
}

//QAction * menuAction () const
static AbstractQoreNode *QMENU_menuAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qm->getQMenu()->menuAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//void popup ( const QPoint & p, QAction * atAction = 0 )
static AbstractQoreNode *QMENU_popup(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQPoint *point = (p && p->type == NT_OBJECT) ? (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-POPUP-PARAM-ERROR", "expecting a QPoint object as first argument to QMenu::popup()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> pHolder(point, xsink);
   p = get_param(params, 1);
   QoreAbstractQAction *atAction = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   ReferenceHolder<QoreAbstractQAction> atActionHolder(atAction, xsink);
   if (atAction)
      qm->getQMenu()->popup(*(static_cast<QPoint *>(point)), atAction->getQAction());
   else
      qm->getQMenu()->popup(*(static_cast<QPoint *>(point)));
   return 0;
}

//bool separatorsCollapsible () const
static AbstractQoreNode *QMENU_separatorsCollapsible(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qm->getQMenu()->separatorsCollapsible());
}

//void setActiveAction ( QAction * act )
static AbstractQoreNode *QMENU_setActiveAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQAction *act = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETACTIVEACTION-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::setActiveAction()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAction> actHolder(act, xsink);
   qm->getQMenu()->setActiveAction(act->getQAction());
   return 0;
}

//void setDefaultAction ( QAction * act )
static AbstractQoreNode *QMENU_setDefaultAction(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreAbstractQAction *act = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETDEFAULTACTION-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::setDefaultAction()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQAction> actHolder(act, xsink);
   qm->getQMenu()->setDefaultAction(act->getQAction());
   return 0;
}

//void setIcon ( const QIcon & icon )
static AbstractQoreNode *QMENU_setIcon(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QMenu::setIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
   qm->getQMenu()->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setSeparatorsCollapsible ( bool collapse )
static AbstractQoreNode *QMENU_setSeparatorsCollapsible(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool collapse = p ? p->getAsBool() : false;
   qm->getQMenu()->setSeparatorsCollapsible(collapse);
   return 0;
}

//void setTearOffEnabled ( bool )
static AbstractQoreNode *QMENU_setTearOffEnabled(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qm->getQMenu()->setTearOffEnabled(b);
   return 0;
}

//void setTitle ( const QString & title )
static AbstractQoreNode *QMENU_setTitle(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   qm->getQMenu()->setTitle(title);
   return 0;
}

//QString title () const
static AbstractQoreNode *QMENU_title(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qm->getQMenu()->title().toUtf8().data(), QCS_UTF8);
}

//int columnCount () const
static AbstractQoreNode *QMENU_columnCount(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qm->columnCount());
}

//void initStyleOption ( QStyleOptionMenuItem * option, const QAction * action ) const
static AbstractQoreNode *QMENU_initStyleOption(QoreObject *self, QoreQMenu *qm, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QoreQStyleOptionMenuItem *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionMenuItem *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONMENUITEM, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionMenuItem object as first argument to QMenu::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   p = get_param(params, 1);
   QoreAbstractQAction *action = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INITSTYLEOPTION-PARAM-ERROR", "expecting a QAction object as second argument to QMenu::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
   qm->initStyleOption(static_cast<QStyleOptionMenuItem *>(option), action->getQAction());
   return 0;
}

QoreClass *initQMenuClass(QoreClass *qwidget)
{
   QC_QMenu = new QoreClass("QMenu", QDOM_GUI);
   CID_QMENU = QC_QMenu->getID();

   QC_QMenu->addBuiltinVirtualBaseClass(qwidget);

   QC_QMenu->setConstructor(QMENU_constructor);
   QC_QMenu->setCopy((q_copy_t)QMENU_copy);

   QC_QMenu->addMethod("actionAt",                    (q_method_t)QMENU_actionAt);
   QC_QMenu->addMethod("actionGeometry",              (q_method_t)QMENU_actionGeometry);
   QC_QMenu->addMethod("activeAction",                (q_method_t)QMENU_activeAction);
   QC_QMenu->addMethod("addAction",                   (q_method_t)QMENU_addAction);
   QC_QMenu->addMethod("addMenu",                     (q_method_t)QMENU_addMenu);
   QC_QMenu->addMethod("addSeparator",                (q_method_t)QMENU_addSeparator);
   QC_QMenu->addMethod("clear",                       (q_method_t)QMENU_clear);
   QC_QMenu->addMethod("defaultAction",               (q_method_t)QMENU_defaultAction);
   QC_QMenu->addMethod("exec",                        (q_method_t)QMENU_exec);
   QC_QMenu->addMethod("hideTearOffMenu",             (q_method_t)QMENU_hideTearOffMenu);
   QC_QMenu->addMethod("icon",                        (q_method_t)QMENU_icon);
   QC_QMenu->addMethod("insertMenu",                  (q_method_t)QMENU_insertMenu);
   QC_QMenu->addMethod("insertSeparator",             (q_method_t)QMENU_insertSeparator);
   QC_QMenu->addMethod("isEmpty",                     (q_method_t)QMENU_isEmpty);
   QC_QMenu->addMethod("isTearOffEnabled",            (q_method_t)QMENU_isTearOffEnabled);
   QC_QMenu->addMethod("isTearOffMenuVisible",        (q_method_t)QMENU_isTearOffMenuVisible);
   QC_QMenu->addMethod("menuAction",                  (q_method_t)QMENU_menuAction);
   QC_QMenu->addMethod("popup",                       (q_method_t)QMENU_popup);
   QC_QMenu->addMethod("separatorsCollapsible",       (q_method_t)QMENU_separatorsCollapsible);
   QC_QMenu->addMethod("setActiveAction",             (q_method_t)QMENU_setActiveAction);
   QC_QMenu->addMethod("setDefaultAction",            (q_method_t)QMENU_setDefaultAction);
   QC_QMenu->addMethod("setIcon",                     (q_method_t)QMENU_setIcon);
   QC_QMenu->addMethod("setSeparatorsCollapsible",    (q_method_t)QMENU_setSeparatorsCollapsible);
   QC_QMenu->addMethod("setTearOffEnabled",           (q_method_t)QMENU_setTearOffEnabled);
   QC_QMenu->addMethod("setTitle",                    (q_method_t)QMENU_setTitle);
   QC_QMenu->addMethod("title",                       (q_method_t)QMENU_title);

   // protected c++ member functions, qore private member methods
   QC_QMenu->addMethod("columnCount",                 (q_method_t)QMENU_columnCount, true);
   QC_QMenu->addMethod("initStyleOption",             (q_method_t)QMENU_initStyleOption, true);

   return QC_QMenu;
}
