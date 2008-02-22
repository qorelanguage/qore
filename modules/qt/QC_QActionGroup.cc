/*
 QC_QActionGroup.cc
 
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

#include "QC_QActionGroup.h"
#include "QC_QAction.h"
#include "QC_QObject.h"
#include "QC_QIcon.h"

#include "qore-qt.h"

int CID_QACTIONGROUP;
QoreClass *QC_QActionGroup = 0;

static void QACTIONGROUP_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreAbstractQObject *parent = p ? (QoreAbstractQObject *)p->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!parent)
   {
      if (!xsink->isException())
         xsink->raiseException("QOBJECT-CONSTRUCTOR-PARAM-ERROR", "expecting an object derived from QObject as sole argument to QActionGroup::constructor()");
      return;
   }
   ReferenceHolder<QoreAbstractQObject> holder(parent, xsink);
   
   QoreQActionGroup *qag = new QoreQActionGroup(self, parent->getQObject());

   self->setPrivate(CID_QACTIONGROUP, qag);
}

static void QACTIONGROUP_copy(class QoreObject *self, class QoreObject *old, class QoreQActionGroup *qa, ExceptionSink *xsink)
{
   xsink->raiseException("QACTIONGROUP-COPY-ERROR", "objects of this class cannot be copied");
}

//QList<QAction *> actions () const
static AbstractQoreNode *QACTIONGROUP_actions(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<QAction *> al = qag->qobj->actions();

   QoreListNode *l = new QoreListNode();
   for (QList<QAction *>::iterator i = al.begin(), e = al.end(); i != e; ++i)
   {
      QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
      QoreQtQAction *q_qa = new QoreQtQAction(o_qa, *i);
      o_qa->setPrivate(CID_QACTION, q_qa);
      
      l->push(o_qa);
   }

   return l;
}

//QAction * addAction ( QAction * action )
//QAction * addAction ( const QString & text )
//QAction * addAction ( const QIcon & icon, const QString & text )
static AbstractQoreNode *QACTIONGROUP_addAction(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQAction *action = (QoreQAction *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink);
      if (!action) {
         QoreQIcon *icon = (QoreQIcon *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink);
         if (!icon) {
            if (!xsink->isException())
               xsink->raiseException("QACTIONGROUP-ADDACTION-PARAM-ERROR", "QActionGroup::addAction() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
            return 0;
         }
         ReferenceHolder<QoreQIcon> iconHolder(icon, xsink);
         p = get_param(params, 1);
	 QString text;
	 if (get_qstring(p, text, xsink))
	    return 0;

         QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
         QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qag->qobj->addAction(*(static_cast<QIcon *>(icon)), text));
         o_qa->setPrivate(CID_QACTION, q_qa);
         return o_qa;
      }
      ReferenceHolder<QoreQAction> actionHolder(action, xsink);
      QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
      QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qag->qobj->addAction(static_cast<QAction *>(action->qobj)));
      o_qa->setPrivate(CID_QACTION, q_qa);
      return o_qa;
   }
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qag->qobj->addAction(text));
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * checkedAction () const
static AbstractQoreNode *QACTIONGROUP_checkedAction(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   QAction *qa = qag->qobj->checkedAction();
   if (!qa)
      return 0;

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qa);
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//bool isEnabled () const
static AbstractQoreNode *QACTIONGROUP_isEnabled(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qag->qobj->isEnabled());
}

//bool isExclusive () const
static AbstractQoreNode *QACTIONGROUP_isExclusive(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qag->qobj->isExclusive());
}

//bool isVisible () const
static AbstractQoreNode *QACTIONGROUP_isVisible(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qag->qobj->isVisible());
}

//void removeAction ( QAction * action )
static AbstractQoreNode *QACTIONGROUP_removeAction(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQAction *action = p ? (QoreQAction *)p->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QACTIONGROUP-REMOVEACTION-PARAM-ERROR", "expecting a QAction object as first argument to QActionGroup::removeAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> holder(action, xsink);
   qag->qobj->removeAction(static_cast<QAction *>(action->qobj));
   return 0;
}

//slots
//void setDisabled ( bool b )
static AbstractQoreNode *QACTIONGROUP_setDisabled(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qag->qobj->setDisabled(b);
   return 0;
}

//void setEnabled ( bool )
static AbstractQoreNode *QACTIONGROUP_setEnabled(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qag->qobj->setEnabled(b);
   return 0;
}

//void setExclusive ( bool )
static AbstractQoreNode *QACTIONGROUP_setExclusive(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qag->qobj->setExclusive(b);
   return 0;
}

//void setVisible ( bool )
static AbstractQoreNode *QACTIONGROUP_setVisible(QoreObject *self, QoreQActionGroup *qag, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qag->qobj->setVisible(b);
   return 0;
}

class QoreClass *initQActionGroupClass(class QoreClass *qobject)
{
   tracein("initQActionGroupClass()");
   
   QC_QActionGroup = new QoreClass("QActionGroup", QDOM_GUI);
   CID_QACTIONGROUP = QC_QActionGroup->getID();

   QC_QActionGroup->addBuiltinVirtualBaseClass(qobject);

   QC_QActionGroup->setConstructor(QACTIONGROUP_constructor);
   QC_QActionGroup->setCopy((q_copy_t)QACTIONGROUP_copy);

   QC_QActionGroup->addMethod("actions",                     (q_method_t)QACTIONGROUP_actions);
   QC_QActionGroup->addMethod("addAction",                   (q_method_t)QACTIONGROUP_addAction);
   QC_QActionGroup->addMethod("checkedAction",               (q_method_t)QACTIONGROUP_checkedAction);
   QC_QActionGroup->addMethod("isEnabled",                   (q_method_t)QACTIONGROUP_isEnabled);
   QC_QActionGroup->addMethod("isExclusive",                 (q_method_t)QACTIONGROUP_isExclusive);
   QC_QActionGroup->addMethod("isVisible",                   (q_method_t)QACTIONGROUP_isVisible);
   QC_QActionGroup->addMethod("removeAction",                (q_method_t)QACTIONGROUP_removeAction);

   // slots
   QC_QActionGroup->addMethod("setDisabled",                 (q_method_t)QACTIONGROUP_setDisabled);
   QC_QActionGroup->addMethod("setEnabled",                  (q_method_t)QACTIONGROUP_setEnabled);
   QC_QActionGroup->addMethod("setExclusive",                (q_method_t)QACTIONGROUP_setExclusive);
   QC_QActionGroup->addMethod("setVisible",                  (q_method_t)QACTIONGROUP_setVisible);

   traceout("initQActionGroupClass()");
   return QC_QActionGroup;
}
