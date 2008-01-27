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
#include "QC_QWidget.h"
#include "QC_QMenu.h"
#include "QC_QAction.h"

#include "qore-qt.h"

int CID_QTOOLBUTTON;
class QoreClass *QC_QToolButton = 0;

//QToolButton ( QWidget * parent = 0 )
static void QTOOLBUTTON_constructor(QoreObject *self, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QTOOLBUTTON, new QoreQToolButton(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QTOOLBUTTON_copy(class QoreObject *self, class QoreObject *old, class QoreQToolButton *qtb, ExceptionSink *xsink)
{
   xsink->raiseException("QTOOLBUTTON-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::ArrowType arrowType () const
static QoreNode *QTOOLBUTTON_arrowType(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQToolButton()->arrowType());
}

//bool autoRaise () const
static QoreNode *QTOOLBUTTON_autoRaise(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode(qtb->getQToolButton()->autoRaise());
}

//QAction * defaultAction () const
static QoreNode *QTOOLBUTTON_defaultAction(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QAction *qt_qobj = qtb->getQToolButton()->defaultAction();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//QMenu * menu () const
static QoreNode *QTOOLBUTTON_menu(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QMenu *qt_qobj = qtb->getQToolButton()->menu();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   assert(rv_obj);
   rv_obj->ref();
   return rv_obj;
}

//ToolButtonPopupMode popupMode () const
static QoreNode *QTOOLBUTTON_popupMode(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQToolButton()->popupMode());
}

//void setArrowType ( Qt::ArrowType type )
static QoreNode *QTOOLBUTTON_setArrowType(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ArrowType type = (Qt::ArrowType)(p ? p->getAsInt() : 0);
   qtb->getQToolButton()->setArrowType(type);
   return 0;
}

//void setAutoRaise ( bool enable )
static QoreNode *QTOOLBUTTON_setAutoRaise(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qtb->getQToolButton()->setAutoRaise(enable);
   return 0;
}

//void setMenu ( QMenu * menu )
static QoreNode *QTOOLBUTTON_setMenu(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQMenu *menu = (p && p->type == NT_OBJECT) ? (QoreAbstractQMenu *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBUTTON-SETMENU-PARAM-ERROR", "expecting a QMenu object as first argument to QToolButton::setMenu()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> menuHolder(static_cast<AbstractPrivateData *>(menu), xsink);
   qtb->getQToolButton()->setMenu(menu->getQMenu());
   return 0;
}

//void setPopupMode ( ToolButtonPopupMode mode )
static QoreNode *QTOOLBUTTON_setPopupMode(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QToolButton::ToolButtonPopupMode mode = (QToolButton::ToolButtonPopupMode)(p ? p->getAsInt() : 0);
   qtb->getQToolButton()->setPopupMode(mode);
   return 0;
}

//Qt::ToolButtonStyle toolButtonStyle () const
static QoreNode *QTOOLBUTTON_toolButtonStyle(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qtb->getQToolButton()->toolButtonStyle());
}

//void setDefaultAction ( QAction * action )
static QoreNode *QTOOLBUTTON_setDefaultAction(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQAction *action = (p && p->type == NT_OBJECT) ? (QoreAbstractQAction *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QACTION, xsink) : 0;
   if (!action) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBUTTON-SETDEFAULTACTION-PARAM-ERROR", "expecting a QAction object as first argument to QToolButton::setDefaultAction()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> actionHolder(static_cast<AbstractPrivateData *>(action), xsink);
   qtb->getQToolButton()->setDefaultAction(action->getQAction());
   return 0;
}

//void setToolButtonStyle ( Qt::ToolButtonStyle style )
static QoreNode *QTOOLBUTTON_setToolButtonStyle(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ToolButtonStyle style = (Qt::ToolButtonStyle)(p ? p->getAsInt() : 0);
   qtb->getQToolButton()->setToolButtonStyle(style);
   return 0;
}

//void showMenu ()
static QoreNode *QTOOLBUTTON_showMenu(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   qtb->getQToolButton()->showMenu();
   return 0;
}

//void initStyleOption ( QStyleOptionToolButton * option ) const
/*
static QoreNode *QTOOLBUTTON_initStyleOption(QoreObject *self, QoreAbstractQToolButton *qtb, const QoreList *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQStyleOptionToolButton *option = (p && p->type == NT_OBJECT) ? (QoreQStyleOptionToolButton *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QSTYLEOPTIONTOOLBUTTON, xsink) : 0;
   if (!option) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLBUTTON-INITSTYLEOPTION-PARAM-ERROR", "expecting a QStyleOptionToolButton object as first argument to QToolButton::initStyleOption()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> optionHolder(static_cast<AbstractPrivateData *>(option), xsink);
   qtb->getQToolButton()->initStyleOption(static_cast<QStyleOptionToolButton *>(option));
   return 0;
}
*/

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
   QC_QToolButton->addMethod("setDefaultAction",            (q_method_t)QTOOLBUTTON_setDefaultAction);
   QC_QToolButton->addMethod("setToolButtonStyle",          (q_method_t)QTOOLBUTTON_setToolButtonStyle);
   QC_QToolButton->addMethod("showMenu",                    (q_method_t)QTOOLBUTTON_showMenu);

   //QC_QToolButton->addMethod("initStyleOption",             (q_method_t)QTOOLBUTTON_initStyleOption, true);

   return QC_QToolButton;
}
