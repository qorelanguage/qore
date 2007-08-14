/*
 QC_QAction.cc
 
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
#include "QC_QAction.h"
#include "QC_QIcon.h"
#include "QC_QActionGroup.h"
#include "QC_QFont.h"

int CID_QACTION;
QoreClass *QC_QAction = 0;

static void QACTION_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = 0;
   const char *text = 0;
   int offset = 0;
   if (p && p->type == NT_OBJECT) {
      icon = p ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;

      if (icon)
	 p = get_param(params, ++offset);
   }
   ReferenceHolder<QoreQIcon> holder(icon, xsink);

   QoreQAction *qa;
   
   if (p && p->type == NT_STRING) {
      text = p->val.String->getBuffer();
      p = get_param(params, ++offset);
   } else if (icon) {
      xsink->raiseException("QACTION-CONSTRUCTOR-PARAM-ERROR", "expecting a string as second argument to QAction::constructor() when the first argument is an object derived from QIcon");
      return;
   }

   QoreAbstractQObject *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQObject *)p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (!parent)
   {
      if (!xsink->isException())
         xsink->raiseException("QACTION-CONSTRUCTOR-PARAM-ERROR", "expecting an object derived from QObject as first argument to QAction::constructor()");
      return;
   }

   if (icon)
      qa = new QoreQAction(self, *icon, text, parent->getQObject());
   else if (text)
      qa = new QoreQAction(self, text, parent->getQObject());
   else 
      qa = new QoreQAction(self, parent->getQObject());

   self->setPrivate(CID_QACTION, qa);
}

static void QACTION_copy(class Object *self, class Object *old, class QoreQAction *qa, ExceptionSink *xsink)
{
   xsink->raiseException("QACTION-COPY-ERROR", "objects of this class cannot be copied");
}

//QActionGroup * actionGroup () const
static QoreNode *QACTION_actionGroup(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QActionGroup *qag = qa->qobj->actionGroup();
   if (!qag)
      return 0;

   Object *o_qag = new Object(QC_QActionGroup, getProgram());
   QoreQActionGroup *q_qag = new QoreQActionGroup(o_qag, qag);
   o_qag->setPrivate(CID_QACTIONGROUP, q_qag);
   return new QoreNode(o_qag);
}

//void activate ( ActionEvent event )
static QoreNode *QACTION_activate(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAction::ActionEvent event = (QAction::ActionEvent)(p ? p->getAsInt() : 0);
   qa->qobj->activate(event);
   return 0;
}

//QList<QWidget *> associatedWidgets () const
//static QoreNode *QACTION_associatedWidgets(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qa->qobj->associatedWidgets();
//}

//bool autoRepeat () const
static QoreNode *QACTION_autoRepeat(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->autoRepeat());
}

//QVariant data () const
//static QoreNode *QACTION_data(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qa->qobj->data());
//}

//QFont font () const
static QoreNode *QACTION_font(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qf = new Object(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(qa->qobj->font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//QIcon icon () const
static QoreNode *QACTION_icon(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qi = new Object(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qa->qobj->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return new QoreNode(o_qi);
}

//QString iconText () const
static QoreNode *QACTION_iconText(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qa->qobj->iconText().toUtf8().data(), QCS_UTF8));
}

//bool isCheckable () const
static QoreNode *QACTION_isCheckable(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->isCheckable());
}

//bool isChecked () const
static QoreNode *QACTION_isChecked(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->isChecked());
}

//bool isEnabled () const
static QoreNode *QACTION_isEnabled(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->isEnabled());
}

//bool isSeparator () const
static QoreNode *QACTION_isSeparator(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->isSeparator());
}

//bool isVisible () const
static QoreNode *QACTION_isVisible(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qa->qobj->isVisible());
}

//QMenu * menu () const
//static QoreNode *QACTION_menu(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qa->qobj->menu();
//}

//MenuRole menuRole () const
//static QoreNode *QACTION_menuRole(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qa->qobj->menuRole());
//}

//QWidget * parentWidget () const
//static QoreNode *QACTION_parentWidget(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qa->qobj->parentWidget();
//}

//void setActionGroup ( QActionGroup * group )
static QoreNode *QACTION_setActionGroup(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQActionGroup *group = (p && p->type == NT_OBJECT) ? (QoreQActionGroup *)p->val.object->getReferencedPrivateData(CID_QACTIONGROUP, xsink) : 0;
   if (!group) {
      if (!xsink->isException())
         xsink->raiseException("QACTION-SETACTIONGROUP-PARAM-ERROR", "expecting a QActionGroup object as first argument to QAction::setActionGroup()");
      return 0;
   }
   ReferenceHolder<QoreQActionGroup> holder(group, xsink);
   qa->qobj->setActionGroup(static_cast<QActionGroup *>(group->qobj));
   return 0;
}

//void setAutoRepeat ( bool )
static QoreNode *QACTION_setAutoRepeat(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setAutoRepeat(b);
   return 0;
}

//void setCheckable ( bool )
static QoreNode *QACTION_setCheckable(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setCheckable(b);
   return 0;
}

//void setData ( const QVariant & userData )
//static QoreNode *QACTION_setData(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QVariant userData = p;
//   qa->qobj->setData(userData);
//   return 0;
//}

//void setFont ( const QFont & font )
static QoreNode *QACTION_setFont(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QACTION-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QAction::setFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> holder(font, xsink);
   qa->qobj->setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setIcon ( const QIcon & icon )
static QoreNode *QACTION_setIcon(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQIcon *icon = (p && p->type == NT_OBJECT) ? (QoreQIcon *)p->val.object->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QACTION-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QAction::setIcon()");
      return 0;
   }
   ReferenceHolder<QoreQIcon> holder(icon, xsink);
   qa->qobj->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setIconText ( const QString & text )
static QoreNode *QACTION_setIconText(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QACTION-SETICONTEXT-PARAM-ERROR", "expecting a string as first argument to QAction::setIconText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   qa->qobj->setIconText(text);
   return 0;
}

//void setMenu ( QMenu * menu )
//static QoreNode *QACTION_setMenu(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QMenu* menu = p;
//   qa->qobj->setMenu(menu);
//   return 0;
//}

//void setMenuRole ( MenuRole menuRole )
static QoreNode *QACTION_setMenuRole(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QAction::MenuRole menuRole = (QAction::MenuRole)(p ? p->getAsInt() : 0);
   qa->qobj->setMenuRole(menuRole);
   return 0;
}

//void setSeparator ( bool b )
static QoreNode *QACTION_setSeparator(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setSeparator(b);
   return 0;
}

//void setShortcut ( const QKeySequence & shortcut )
static QoreNode *QACTION_setShortcut(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQKeySequence *shortcut = (p && p->type == NT_OBJECT) ? (QoreQKeySequence *)p->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink) : 0;
   if (!shortcut) {
      if (!xsink->isException())
         xsink->raiseException("QACTION-SETSHORTCUT-PARAM-ERROR", "expecting a QKeySequence object as first argument to QAction::setShortcut()");
      return 0;
   }
   ReferenceHolder<QoreQKeySequence> holder(shortcut, xsink);
   qa->qobj->setShortcut(*(static_cast<QKeySequence *>(shortcut)));
   return 0;
}

//void setShortcutContext ( Qt::ShortcutContext context )
static QoreNode *QACTION_setShortcutContext(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::ShortcutContext context = (Qt::ShortcutContext)(p ? p->getAsInt() : 0);
   qa->qobj->setShortcutContext(context);
   return 0;
}

//void setShortcuts ( const QList<QKeySequence> & shortcuts )
//void setShortcuts ( QKeySequence::StandardKey key )
//static QoreNode *QACTION_setShortcuts(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   if (p && p->type == NT_???) {
//      ??? QList<QKeySequence> shortcuts = p;
//      qa->qobj->setShortcuts(shortcuts);
//      return 0;
//   }
//   QKeySequence::StandardKey key = (QKeySequence::StandardKey)(p ? p->getAsInt() : 0);
//   qa->qobj->setShortcuts(key);
//   return 0;
//}

//void setStatusTip ( const QString & statusTip )
static QoreNode *QACTION_setStatusTip(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QACTION-SETSTATUSTIP-PARAM-ERROR", "expecting a string as first argument to QAction::setStatusTip()");
      return 0;
   }
   const char *statusTip = p->val.String->getBuffer();
   qa->qobj->setStatusTip(statusTip);
   return 0;
}

//void setText ( const QString & text )
static QoreNode *QACTION_setText(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QACTION-SETTEXT-PARAM-ERROR", "expecting a string as first argument to QAction::setText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();
   qa->qobj->setText(text);
   return 0;
}

//void setToolTip ( const QString & tip )
static QoreNode *QACTION_setToolTip(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QACTION-SETTOOLTIP-PARAM-ERROR", "expecting a string as first argument to QAction::setToolTip()");
      return 0;
   }
   const char *tip = p->val.String->getBuffer();
   qa->qobj->setToolTip(tip);
   return 0;
}

//void setWhatsThis ( const QString & what )
static QoreNode *QACTION_setWhatsThis(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   if (!p || p->type != NT_STRING) {
      xsink->raiseException("QACTION-SETWHATSTHIS-PARAM-ERROR", "expecting a string as first argument to QAction::setWhatsThis()");
      return 0;
   }
   const char *what = p->val.String->getBuffer();
   qa->qobj->setWhatsThis(what);
   return 0;
}

//QKeySequence shortcut () const
//static QoreNode *QACTION_shortcut(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qa->qobj->shortcut());
//}

//Qt::ShortcutContext shortcutContext () const
static QoreNode *QACTION_shortcutContext(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qa->qobj->shortcutContext());
}

//QList<QKeySequence> shortcuts () const
//static QoreNode *QACTION_shortcuts(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qa->qobj->shortcuts());
//}

//bool showStatusText ( QWidget * widget = 0 )
static QoreNode *QACTION_showStatusText(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreAbstractQWidget *widget = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!widget) {
      if (!xsink->isException())
         xsink->raiseException("QACTION-SHOWSTATUSTEXT-PARAM-ERROR", "expecting a QWidget object as first argument to QAction::showStatusText()");
      return 0;
   }
   ReferenceHolder<QoreAbstractQWidget> holder(widget, xsink);
   return new QoreNode(qa->qobj->showStatusText(widget->getQWidget()));
}

//QString statusTip () const
static QoreNode *QACTION_statusTip(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qa->qobj->statusTip().toUtf8().data(), QCS_UTF8));
}

//QString text () const
static QoreNode *QACTION_text(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qa->qobj->text().toUtf8().data(), QCS_UTF8));
}

//QString toolTip () const
static QoreNode *QACTION_toolTip(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qa->qobj->toolTip().toUtf8().data(), QCS_UTF8));
}

//QString whatsThis () const
static QoreNode *QACTION_whatsThis(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qa->qobj->whatsThis().toUtf8().data(), QCS_UTF8));
}

//void hover ()
static QoreNode *QACTION_hover(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   qa->qobj->hover();
   return 0;
}

//void setChecked ( bool )
static QoreNode *QACTION_setChecked(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setChecked(b);
   return 0;
}

//void setDisabled ( bool b )
static QoreNode *QACTION_setDisabled(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setDisabled(b);
   return 0;
}

//void setEnabled ( bool )
static QoreNode *QACTION_setEnabled(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setEnabled(b);
   return 0;
}

//void setVisible ( bool )
static QoreNode *QACTION_setVisible(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qa->qobj->setVisible(b);
   return 0;
}

//void toggle ()
static QoreNode *QACTION_toggle(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   qa->qobj->toggle();
   return 0;
}

//void trigger ()
static QoreNode *QACTION_trigger(Object *self, QoreQAction *qa, QoreNode *params, ExceptionSink *xsink)
{
   qa->qobj->trigger();
   return 0;
}

class QoreClass *initQActionClass(class QoreClass *qobject)
{
   tracein("initQActionClass()");
   
   QC_QAction = new QoreClass("QAction", QDOM_GUI);
   CID_QACTION = QC_QAction->getID();

   QC_QAction->addBuiltinVirtualBaseClass(qobject);

   QC_QAction->setConstructor(QACTION_constructor);
   QC_QAction->setCopy((q_copy_t)QACTION_copy);

   QC_QAction->addMethod("actionGroup",                 (q_method_t)QACTION_actionGroup);
   QC_QAction->addMethod("activate",                    (q_method_t)QACTION_activate);
   //QC_QAction->addMethod("associatedWidgets",           (q_method_t)QACTION_associatedWidgets);
   QC_QAction->addMethod("autoRepeat",                  (q_method_t)QACTION_autoRepeat);
   //QC_QAction->addMethod("data",                        (q_method_t)QACTION_data);
   QC_QAction->addMethod("font",                        (q_method_t)QACTION_font);
   QC_QAction->addMethod("icon",                        (q_method_t)QACTION_icon);
   QC_QAction->addMethod("iconText",                    (q_method_t)QACTION_iconText);
   QC_QAction->addMethod("isCheckable",                 (q_method_t)QACTION_isCheckable);
   QC_QAction->addMethod("isChecked",                   (q_method_t)QACTION_isChecked);
   QC_QAction->addMethod("isEnabled",                   (q_method_t)QACTION_isEnabled);
   QC_QAction->addMethod("isSeparator",                 (q_method_t)QACTION_isSeparator);
   QC_QAction->addMethod("isVisible",                   (q_method_t)QACTION_isVisible);
   //QC_QAction->addMethod("menu",                        (q_method_t)QACTION_menu);
   //QC_QAction->addMethod("menuRole",                    (q_method_t)QACTION_menuRole);
   //QC_QAction->addMethod("parentWidget",                (q_method_t)QACTION_parentWidget);
   QC_QAction->addMethod("setActionGroup",              (q_method_t)QACTION_setActionGroup);
   QC_QAction->addMethod("setAutoRepeat",               (q_method_t)QACTION_setAutoRepeat);
   QC_QAction->addMethod("setCheckable",                (q_method_t)QACTION_setCheckable);
   //QC_QAction->addMethod("setData",                     (q_method_t)QACTION_setData);
   QC_QAction->addMethod("setFont",                     (q_method_t)QACTION_setFont);
   QC_QAction->addMethod("setIcon",                     (q_method_t)QACTION_setIcon);
   QC_QAction->addMethod("setIconText",                 (q_method_t)QACTION_setIconText);
   //QC_QAction->addMethod("setMenu",                     (q_method_t)QACTION_setMenu);
   QC_QAction->addMethod("setMenuRole",                 (q_method_t)QACTION_setMenuRole);
   QC_QAction->addMethod("setSeparator",                (q_method_t)QACTION_setSeparator);
   QC_QAction->addMethod("setShortcut",                 (q_method_t)QACTION_setShortcut);
   QC_QAction->addMethod("setShortcutContext",          (q_method_t)QACTION_setShortcutContext);
   //QC_QAction->addMethod("setShortcuts",                (q_method_t)QACTION_setShortcuts);
   QC_QAction->addMethod("setStatusTip",                (q_method_t)QACTION_setStatusTip);
   QC_QAction->addMethod("setText",                     (q_method_t)QACTION_setText);
   QC_QAction->addMethod("setToolTip",                  (q_method_t)QACTION_setToolTip);
   QC_QAction->addMethod("setWhatsThis",                (q_method_t)QACTION_setWhatsThis);
   //QC_QAction->addMethod("shortcut",                    (q_method_t)QACTION_shortcut);
   QC_QAction->addMethod("shortcutContext",             (q_method_t)QACTION_shortcutContext);
   //QC_QAction->addMethod("shortcuts",                   (q_method_t)QACTION_shortcuts);
   QC_QAction->addMethod("showStatusText",              (q_method_t)QACTION_showStatusText);
   QC_QAction->addMethod("statusTip",                   (q_method_t)QACTION_statusTip);
   QC_QAction->addMethod("text",                        (q_method_t)QACTION_text);
   QC_QAction->addMethod("toolTip",                     (q_method_t)QACTION_toolTip);
   QC_QAction->addMethod("whatsThis",                   (q_method_t)QACTION_whatsThis);
   QC_QAction->addMethod("hover",                       (q_method_t)QACTION_hover);
   QC_QAction->addMethod("setChecked",                  (q_method_t)QACTION_setChecked);
   QC_QAction->addMethod("setDisabled",                 (q_method_t)QACTION_setDisabled);
   QC_QAction->addMethod("setEnabled",                  (q_method_t)QACTION_setEnabled);
   QC_QAction->addMethod("setVisible",                  (q_method_t)QACTION_setVisible);
   QC_QAction->addMethod("toggle",                      (q_method_t)QACTION_toggle);
   QC_QAction->addMethod("trigger",                     (q_method_t)QACTION_trigger);

   traceout("initQActionClass()");
   return QC_QAction;
}
