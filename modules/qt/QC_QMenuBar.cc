/*
 QC_QMenuBar.cc
 
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

#include "QC_QMenuBar.h"
#include "QC_QWidget.h"
#include "QC_QAction.h"
#include "QC_QMenu.h"
#include "QC_QObject.h"
#include "QC_QIcon.h"

#include "qore-qt.h"

int CID_QMENUBAR;
class QoreClass *QC_QMenuBar = 0;

//QMenuBar ( QWidget * parent = 0 )
static void QMENUBAR_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QMENUBAR, new QoreQMenuBar(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QMENUBAR_copy(class QoreObject *self, class QoreObject *old, class QoreQMenuBar *qmb, ExceptionSink *xsink)
{
   xsink->raiseException("QMENUBAR-COPY-ERROR", "objects of this class cannot be copied");
}

//QAction * activeAction () const
static QoreNode *QMENUBAR_activeAction(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QAction *qt_qobj = qmb->getQMenuBar()->activeAction();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QAction, getProgram());
      QoreQtQAction *q_qa = new QoreQtQAction(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QACTION, q_qa);
   }

   return rv_obj;
}

//QAction * addAction ( const QString & text )
//QAction * addAction ( const QString & text, const QObject * receiver, const char * member )
static QoreNode *QMENUBAR_addAction(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   QAction *qaction;

   if (num_params(params) == 1)
      qaction = qmb->getQMenuBar()->addAction(text);
   else {
      p = get_param(params, 1);
      QoreAbstractQObject *receiver = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (!receiver) {
	 if (!xsink->isException())
	    xsink->raiseException("QMENUBAR-ADDACTION-PARAM-ERROR", "this version of QMenuBar::addAction() expects an object derived from QObject as the second argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
	 return 0;
      }
      ReferenceHolder<AbstractPrivateData> receiverHolder(static_cast<AbstractPrivateData *>(receiver), xsink);

      QoreStringNode *p = test_string_param(params, 2);
      if (!p) {
         xsink->raiseException("QMENUBAR-ADDACTION-PARAM-ERROR", "expecting a string as third argument to QMenuBar::addAction()");
         return 0;
      }
      const char *member = p->getBuffer();

      qaction = qmb->getQMenuBar()->addAction(text, receiver->getQObject(), member);
   }

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qaction);
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * addMenu ( QMenu * menu )
//QMenu * addMenu ( const QString & title )
//QMenu * addMenu ( const QIcon & icon, const QString & title )
static QoreNode *QMENUBAR_addMenu(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQIcon *icon = (QoreQIcon *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QICON, xsink);
      if (!icon) {
         QoreQMenu *menu = (QoreQMenu *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMENU, xsink);
         if (!menu) {
            if (!xsink->isException())
               xsink->raiseException("QMENUBAR-ADDMENU-PARAM-ERROR", "QMenuBar::addMenu() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> menuHolder(static_cast<AbstractPrivateData *>(menu), xsink);
         QAction *qt_qobj = qmb->getQMenuBar()->addMenu(static_cast<QMenu *>(menu->qobj));

	 QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
	 QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qt_qobj);
	 o_qa->setPrivate(CID_QACTION, q_qa);
	 return o_qa;
      }
      ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
      p = get_param(params, 1);
      QString title;
      if (get_qstring(p, title, xsink))
         return 0;
      QMenu *qt_qobj = qmb->getQMenuBar()->addMenu(*(static_cast<QIcon *>(icon)), title);

      QoreObject *o_qa = new QoreObject(QC_QMenu, getProgram());
      QoreQtQMenu *q_qa = new QoreQtQMenu(o_qa, qt_qobj);
      o_qa->setPrivate(CID_QMENU, q_qa);
      return o_qa;
   }
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   QMenu *qt_qobj = qmb->getQMenuBar()->addMenu(title);

   QoreObject *o_qa = new QoreObject(QC_QMenu, getProgram());
   QoreQtQMenu *q_qa = new QoreQtQMenu(o_qa, qt_qobj);
   o_qa->setPrivate(CID_QMENU, q_qa);
   return o_qa;
}

//QAction * addSeparator ()
static QoreNode *QMENUBAR_addSeparator(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QAction *qt_qobj = qmb->getQMenuBar()->addSeparator();

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qt_qobj);
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//void clear ()
static QoreNode *QMENUBAR_clear(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   qmb->getQMenuBar()->clear();
   return 0;
}

//QAction * insertMenu ( QAction * before, QMenu * menu )
static QoreNode *QMENUBAR_insertMenu(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAction *before = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENUBAR-INSERTMENU-PARAM-ERROR", "expecting a QAction object as first argument to QMenuBar::insertMenu()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);
   p = get_param(params, 1);
   QoreQMenu *menu = (p && p->type == NT_OBJECT) ? (QoreQMenu *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QMENUBAR-INSERTMENU-PARAM-ERROR", "expecting a QMenu object as second argument to QMenuBar::insertMenu()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> menuHolder(static_cast<AbstractPrivateData *>(menu), xsink);
   QAction *qt_qobj = qmb->getQMenuBar()->insertMenu(before->getQAction(), static_cast<QMenu *>(menu->qobj));

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qt_qobj);
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//QAction * insertSeparator ( QAction * before )
static QoreNode *QMENUBAR_insertSeparator(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAction *before = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!before) {
      if (!xsink->isException())
         xsink->raiseException("QMENUBAR-INSERTSEPARATOR-PARAM-ERROR", "expecting a QAction object as first argument to QMenuBar::insertSeparator()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> beforeHolder(static_cast<AbstractPrivateData *>(before), xsink);
   QAction *qt_qobj = qmb->getQMenuBar()->insertSeparator(before->getQAction());

   QoreObject *o_qa = new QoreObject(QC_QAction, getProgram());
   QoreQtQAction *q_qa = new QoreQtQAction(o_qa, qt_qobj);
   o_qa->setPrivate(CID_QACTION, q_qa);
   return o_qa;
}

//bool isDefaultUp () const
static QoreNode *QMENUBAR_isDefaultUp(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qmb->getQMenuBar()->isDefaultUp());
}

//void setActiveAction ( QAction * act )
static QoreNode *QMENUBAR_setActiveAction(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAction *act = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!act) {
      if (!xsink->isException())
         xsink->raiseException("QMENUBAR-SETACTIVEACTION-PARAM-ERROR", "expecting a QAction object as first argument to QMenuBar::setActiveAction()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> actHolder(static_cast<AbstractPrivateData *>(act), xsink);
   qmb->getQMenuBar()->setActiveAction(act->getQAction());
   return 0;
}

//void setDefaultUp ( bool )
static QoreNode *QMENUBAR_setDefaultUp(QoreObject *self, QoreAbstractQMenuBar *qmb, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qmb->getQMenuBar()->setDefaultUp(b);
   return 0;
}

QoreClass *initQMenuBarClass(QoreClass *qwidget)
{
   QC_QMenuBar = new QoreClass("QMenuBar", QDOM_GUI);
   CID_QMENUBAR = QC_QMenuBar->getID();

   QC_QMenuBar->addBuiltinVirtualBaseClass(qwidget);

   QC_QMenuBar->setConstructor(QMENUBAR_constructor);
   QC_QMenuBar->setCopy((q_copy_t)QMENUBAR_copy);

   QC_QMenuBar->addMethod("activeAction",                (q_method_t)QMENUBAR_activeAction);
   QC_QMenuBar->addMethod("addAction",                   (q_method_t)QMENUBAR_addAction);
   QC_QMenuBar->addMethod("addMenu",                     (q_method_t)QMENUBAR_addMenu);
   QC_QMenuBar->addMethod("addSeparator",                (q_method_t)QMENUBAR_addSeparator);
   QC_QMenuBar->addMethod("clear",                       (q_method_t)QMENUBAR_clear);
   QC_QMenuBar->addMethod("insertMenu",                  (q_method_t)QMENUBAR_insertMenu);
   QC_QMenuBar->addMethod("insertSeparator",             (q_method_t)QMENUBAR_insertSeparator);
   QC_QMenuBar->addMethod("isDefaultUp",                 (q_method_t)QMENUBAR_isDefaultUp);
   QC_QMenuBar->addMethod("setActiveAction",             (q_method_t)QMENUBAR_setActiveAction);
   QC_QMenuBar->addMethod("setDefaultUp",                (q_method_t)QMENUBAR_setDefaultUp);

   return QC_QMenuBar;
}
