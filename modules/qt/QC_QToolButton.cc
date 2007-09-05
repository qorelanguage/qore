/*
 QC_QToolButton.cc
 
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

#include "QC_QToolButton.h"

int CID_QTOOLBUTTON;
class QoreClass *QC_QToolButton = 0;

//QToolButton ( QWidget * parent = 0 )
static void QTOOLBUTTON_constructor(Object *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);
   self->setPrivate(CID_QTOOLBUTTON, new QoreQToolButton(self, parent ? parent->getQWidget() : 0));
   return;
}

static void QTOOLBUTTON_copy(class Object *self, class Object *old, class QoreQToolButton *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTOOLBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::ArrowType arrowType () const
static QoreNode *QTOOLBUTTON_arrowType(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->arrowType());
}

//bool autoRaise () const
static QoreNode *QTOOLBUTTON_autoRaise(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->qobj->autoRaise());
}

//QAction * defaultAction () const
static QoreNode *QTOOLBUTTON_defaultAction(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qa = new Object(QC_QAction, getProgram());
   QoreQAction *q_qa = new QoreQAction(o_qa, qtb->qobj->defaultAction());
   o_qa->setPrivate(CID_QACTION, q_qa);
   return new QoreNode(o_qa);
}

//QMenu * menu () const
static QoreNode *QTOOLBUTTON_menu(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qm = new Object(QC_QMenu, getProgram());
   QoreQMenu *q_qm = new QoreQMenu(o_qm, qtb->qobj->menu());
   o_qm->setPrivate(CID_QMENU, q_qm);
   return new QoreNode(o_qm);
}

//QToolButton::ToolButtonPopupMode popupMode () const
static QoreNode *QTOOLBUTTON_popupMode(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->popupMode());
}

//void setArrowType ( Qt::ArrowType type )
static QoreNode *QTOOLBUTTON_setArrowType(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ArrowType type = (Qt::ArrowType)(p ? p->getAsInt() : 0);
   qtb->qobj->setArrowType(type);
   return 0;
}

//void setAutoRaise ( bool enable )
static QoreNode *QTOOLBUTTON_setAutoRaise(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qtb->qobj->setAutoRaise(enable);
   return 0;
}

//void setMenu ( QMenu * menu )
static QoreNode *QTOOLBUTTON_setMenu(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQMenu *menu = (p && p->type == NT_OBJECT) ? (QoreQMenu *)p->val.object->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBUTTON-SETMENU-PARAM-ERROR", "expecting a QMenu object as first argument to QToolButton::setMenu()");
      return 0;
   }
   ReferenceHolder<QoreQMenu> menuHolder(menu, xsink);
   qtb->qobj->setMenu(static_cast<QMenu *>(menu->qobj));
   return 0;
}

//void setPopupMode ( ToolButtonPopupMode mode )
static QoreNode *QTOOLBUTTON_setPopupMode(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QToolButton::ToolButtonPopupMode mode = (QToolButton::ToolButtonPopupMode)(p ? p->getAsInt() : 0);
   qtb->qobj->setPopupMode(mode);
   return 0;
}

//Qt::ToolButtonStyle toolButtonStyle () const
static QoreNode *QTOOLBUTTON_toolButtonStyle(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->qobj->toolButtonStyle());
}

//QSize sizeHint () const
static QoreNode *QTOOLBUTTON_sizeHint(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->qobj->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QSize minimumSizeHint () const
static QoreNode *QTOOLBUTTON_minimumSizeHint(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qs = new Object(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qtb->qobj->minimumSizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//void setDefaultAction ( QAction * action )
static QoreNode *QTOOLBUTTON_setDefaultAction(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQAction *action = (p && p->type == NT_OBJECT) ? (QoreQAction *)p->val.object->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBUTTON-SETDEFAULTACTION-PARAM-ERROR", "expecting a QAction object as first argument to QToolButton::setDefaultAction()");
      return 0;
   }
   ReferenceHolder<QoreQAction> actionHolder(action, xsink);
   qtb->qobj->setDefaultAction(static_cast<QAction *>(action->qobj));
   return 0;
}


//void setToolButtonStyle ( Qt::ToolButtonStyle style )
static QoreNode *QTOOLBUTTON_setToolButtonStyle(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolButtonStyle style = (Qt::ToolButtonStyle)(p ? p->getAsInt() : 0);
   qtb->qobj->setToolButtonStyle(style);
   return 0;
}

//void showMenu ()
static QoreNode *QTOOLBUTTON_showMenu(Object *self, QoreQToolButton *qtb, QoreNode *params, ExceptionSink *xsink)
{
   qtb->qobj->showMenu();
   return 0;
}

QoreClass *initQToolButtonClass(QoreClass *qabstractbutton)
{
   QC_QToolButton = new QoreClass("QToolButton", QDOM_GUI);
   CID_QTOOLBUTTON = QC_QToolButton->getID();

   QC_QToolButton->addBuiltinVirtualBaseClass(qabstractbutton);

   QC_QToolButton->setConstructor(QTOOLBUTTON_constructor);
   QC_QToolButton->setCopy((q_copy_t)QTOOLBUTTON_copy);

   QC_QToolButton->addMethod("arrowType",                   (q_method_t)QTOOLBUTTON_arrowType);
   QC_QToolButton->addMethod("autoRaise",                   (q_method_t)QTOOLBUTTON_autoRaise);
   QC_QToolButton->addMethod("defaultAction",               (q_method_t)QTOOLBUTTON_defaultAction);
   QC_QToolButton->addMethod("menu",                        (q_method_t)QTOOLBUTTON_menu);
   QC_QToolButton->addMethod("popupMode",                   (q_method_t)QTOOLBUTTON_popupMode);
   QC_QToolButton->addMethod("setArrowType",                (q_method_t)QTOOLBUTTON_setArrowType);
   QC_QToolButton->addMethod("setAutoRaise",                (q_method_t)QTOOLBUTTON_setAutoRaise);
   QC_QToolButton->addMethod("setMenu",                     (q_method_t)QTOOLBUTTON_setMenu);
   QC_QToolButton->addMethod("setPopupMode",                (q_method_t)QTOOLBUTTON_setPopupMode);
   QC_QToolButton->addMethod("toolButtonStyle",             (q_method_t)QTOOLBUTTON_toolButtonStyle);

   QC_QToolButton->addMethod("sizeHint",                    (q_method_t)QTOOLBUTTON_sizeHint);
   QC_QToolButton->addMethod("minimumSizeHint",             (q_method_t)QTOOLBUTTON_minimumSizeHint);
   QC_QToolButton->addMethod("setDefaultAction",            (q_method_t)QTOOLBUTTON_setDefaultAction);
   QC_QToolButton->addMethod("setToolButtonStyle",          (q_method_t)QTOOLBUTTON_setToolButtonStyle);
   QC_QToolButton->addMethod("showMenu",                    (q_method_t)QTOOLBUTTON_showMenu);

   return QC_QToolButton;
}
