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

int CID_QMENU;
class QoreClass *QC_QMenu = 0;

//QMenu ( QWidget * parent = 0 )
//QMenu ( const QString & title, QWidget * parent = 0 )
static void QMENU_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_STRING) {
      QString title;

      if (get_qstring(p, title, xsink))
	 return;

      p = get_param(params, 1);
      QoreAbstractQWidget *parent = p ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
      self->setPrivate(CID_QMENU, new QoreQMenu(self, title, parent ? parent->getQWidget() : 0));
      return;
   }

   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   if (!parent) {
      self->setPrivate(CID_QMENU, new QoreQMenu(self));
      return;
   }

   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QMENU, new QoreQMenu(self, parent->getQWidget()));
   return;
}

static void QMENU_copy(class Object *self, class Object *old, class QoreQMenu *qm, ExceptionSink *xsink)
{
   xsink->raiseException("QMENU-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * actionAt ( const QPoint & pt ) const
static QoreNode *QMENU_actionAt(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pt = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pt) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-ACTIONAT-PARAM-ERROR", "expecting a QPoint object as first argument to QMenu::actionAt()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> ptHolder(pt, xsink);
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->actionAt(*(static_cast<QPoint *>(pt))));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//QRect actionGeometry ( QAction * act ) const
static QoreNode *QMENU_actionGeometry(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *act = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-ACTIONGEOMETRY-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::actionGeometry()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actHolder(act, xsink);
   Object *o_qr = new Object(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qm->qobj->actionGeometry(static_cast<QAction *>(act->qobj)));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return new QoreNode(o_qr);
}

//QAction * activeAction () const
static QoreNode *QMENU_activeAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->activeAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//QAction * addAction ( const QString & text )
//QAction * addAction ( const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 )

//QAction * addAction ( const QIcon & icon, const QString & text )
//QAction * addAction ( const QIcon & icon, const QString & text, const QObject * receiver, const char * member, const QKeySequence & shortcut = 0 )
static QoreNode *QMENU_addAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int offset = 0;

   QoreQIcon *icon = 0;
   if (p && p->type == NT_OBJECT) {
      icon = (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
	 if (!xsink->isException())
	    xsink->raiseException("QMENU-ADDACTION-PARAM-ERROR", "QMenu::addAction() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
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
   QoreAbstractQObject *receiver = p ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!receiver) {
      Object *o_qa = new Object(QC_QAction, getProgram());
      QoreQAction *q_qa;
      if (icon)
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(*(static_cast<QIcon *>(icon)), text));
      else
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(text));
      o_qa->setPrivate(CID_QACTION, q_qa);
      return new QoreNode(o_qa);
   }
   ReferenceHolder<QoreAbstractQObject> receiverHolder(receiver, xsink);
   
   p = get_param(params, 2 + offset);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QMENU-ADDACTION-PARAM-ERROR", "expecting a string as third or fourth argument to QMenu::addAction()");
      return 0;
   }
   const char *member = p->val.String->getBuffer();
   
   p = get_param(params, 3 + offset);
   QoreQKeySequence *shortcut = p ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   ReferenceHolder<QoreQKeySequence> shortcutHolder(shortcut, xsink);
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa;
   if (icon)
      if (shortcut)
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(*(static_cast<QIcon *>(icon)), text, receiver->getQObject(), member, *(static_cast<QKeySequence *>(shortcut))));
      else
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(*(static_cast<QIcon *>(icon)), text, receiver->getQObject(), member));
   else
      if (shortcut)
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(text, receiver->getQObject(), member, *(static_cast<QKeySequence *>(shortcut))));
      else
	 q_qa = new QoreQAction(o_qa, qm->qobj->addAction(text, receiver->getQObject(), member));

   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

////QAction * addMenu ( QMenu * menu )
////QMenu * addMenu ( const QString & title )
////QMenu * addMenu ( const QIcon & icon, const QString & title )
static QoreNode *QMENU_addMenu(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         QoreQMenu *menu = (QoreQMenu *)p->val.object->getReferencedPrivateData(CID_QMENU, xsink);
         if (!menu) {
            if (!xsink->isException())
               xsink->raiseException("QMENU-ADDMENU-PARAM-ERROR", "QMenu::addMenu() does not know how to handle arguments of class '%s' as passed as the first argument", p->val.object->getClass()->getName());
            return 0;
         }
         ReferenceHolder<QoreQMenu> menuHolder(menu, xsink);
         Object *o_qa = new Object(QC_QAction, getProgram());
         QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->addMenu(static_cast<QMenu *>(menu->qobj)));
         o_qa->setPrivate(CID_QACTION, q_qa);
         return new QoreNode(o_qa);
      }
      ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
      p = get_param(params, 1);
      QString title;
      if (get_qstring(p, title, xsink))
	 return 0;

      Object *o_qa = new Object(QC_QMenu, getProgram());
      QoreQMenu *q_qa = new QoreQMenu(o_qa, qm->qobj->addMenu(*(static_cast<QIcon *>(icon)), title));
      o_qa->setPrivate(CID_QMENU, q_qa);
      return new QoreNode(o_qa);
   }
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   Object *o_qa = new Object(QC_QMenu, getProgram());
   QoreQMenu *q_qa = new QoreQMenu(o_qa, qm->qobj->addMenu(title));
   o_qa->setPrivate(CID_QMENU, q_qa);
   return new QoreNode(o_qa);
}

//QAction * addSeparator ()
static QoreNode *QMENU_addSeparator(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->addSeparator());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//void clear ()
static QoreNode *QMENU_clear(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   qm->qobj->clear();
   return 0;
}

//QAction * defaultAction () const
static QoreNode *QMENU_defaultAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->defaultAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//QAction * exec ()
//QAction * exec ( const QPoint & p, QAction * action = 0 )
static QoreNode *QMENU_exec(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      Object *o_qa = new Object(QC_QAction, getProgram());
      QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->exec());
      o_qa->setPrivate(CID_QACTION, q_qa);
      return new QoreNode(o_qa);
   }
   QoreQPoint *point = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-EXEC-PARAM-ERROR", "this version of QMenu::exec() expects an object derived from QPoint as the first argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQPoint> pHolder(point, xsink);
   p = get_param(params, 1);
   QoreQAction *action = p ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa;
   if (action)
      q_qa = new QoreQAction(o_qa, qm->qobj->exec(*(static_cast<QPoint *>(point)), static_cast<QAction *>(action->qobj)));
   else
      q_qa = new QoreQAction(o_qa, qm->qobj->exec(*(static_cast<QPoint *>(point))));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//void hideTearOffMenu ()
static QoreNode *QMENU_hideTearOffMenu(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   qm->qobj->hideTearOffMenu();
   return 0;
}

//QIcon icon () const
static QoreNode *QMENU_icon(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qm->qobj->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QAction * insertMenu ( QAction * before, QMenu * menu )
static QoreNode *QMENU_insertMenu(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTMENU-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::insertMenu()");
      return 0;
   }
   ReferenceHolder<QoreQAction> beforeHolder(before, xsink);
   p = get_param(params, 1);
   QoreQMenu *menu = (p && p->type == NT_OBJECT) ? (QoreQMenu *)p->val.object->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTMENU-PARAM-ERROR", "expecting a QMenu object as second argument to QMenu::insertMenu()");
      return 0;
   }
   ReferenceHolder<QoreQMenu> menuHolder(menu, xsink);
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->insertMenu(static_cast<QAction *>(before->qobj), static_cast<QMenu *>(menu->qobj)));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//QAction * insertSeparator ( QAction * before )
static QoreNode *QMENU_insertSeparator(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *before = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-INSERTSEPARATOR-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::insertSeparator()");
      return 0;
   }
   ReferenceHolder<QoreQAction> beforeHolder(before, xsink);
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->insertSeparator(static_cast<QAction *>(before->qobj)));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//bool isEmpty () const
static QoreNode *QMENU_isEmpty(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->isEmpty());
}

//bool isTearOffEnabled () const
static QoreNode *QMENU_isTearOffEnabled(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->isTearOffEnabled());
}

//bool isTearOffMenuVisible () const
static QoreNode *QMENU_isTearOffMenuVisible(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->isTearOffMenuVisible());
}

//QAction * menuAction () const
static QoreNode *QMENU_menuAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qm->qobj->menuAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//void popup ( const QPoint & p, QAction * atAction = 0 )
static QoreNode *QMENU_popup(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *point = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-POPUP-PARAM-ERROR", "expecting a QPoint object as first argument to QMenu::popup()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> pHolder(point, xsink);
   p = get_param(params, 1);
   QoreQAction *atAction = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   ReferenceHolder<QoreQAction> atActionHolder(atAction, xsink);
   if (atAction)
      qm->qobj->popup(*(static_cast<QPoint *>(point)), static_cast<QAction *>(atAction->qobj));
   else
      qm->qobj->popup(*(static_cast<QPoint *>(point)));
   return 0;
}

//bool separatorsCollapsible () const
static QoreNode *QMENU_separatorsCollapsible(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qm->qobj->separatorsCollapsible());
}

//void setActiveAction ( QAction * act )
static QoreNode *QMENU_setActiveAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *act = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETACTIVEACTION-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::setActiveAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actHolder(act, xsink);
   qm->qobj->setActiveAction(static_cast<QAction *>(act->qobj));
   return 0;
}

//void setDefaultAction ( QAction * act )
static QoreNode *QMENU_setDefaultAction(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *act = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETDEFAULTACTION-PARAM-ERROR", "expecting a QAction object as first argument to QMenu::setDefaultAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actHolder(act, xsink);
   qm->qobj->setDefaultAction(static_cast<QAction *>(act->qobj));
   return 0;
}

//void setIcon ( const QIcon & icon )
static QoreNode *QMENU_setIcon(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QMENU-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QMenu::setIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
   qm->qobj->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setSeparatorsCollapsible ( bool collapse )
static QoreNode *QMENU_setSeparatorsCollapsible(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool collapse = p ? p->getAsBool() : false;
   qm->qobj->setSeparatorsCollapsible(collapse);
   return 0;
}

//void setTearOffEnabled ( bool )
static QoreNode *QMENU_setTearOffEnabled(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qm->qobj->setTearOffEnabled(b);
   return 0;
}

//void setTitle ( const QString & title )
static QoreNode *QMENU_setTitle(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   qm->qobj->setTitle(title);
   return 0;
}

//QString title () const
static QoreNode *QMENU_title(Object *self, QoreQMenu *qm, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qm->qobj->title().toUtf8().data(), QCS_UTF8));
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

   return QC_QMenu;
}
