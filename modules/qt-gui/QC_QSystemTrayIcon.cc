/*
 QC_QSystemTrayIcon.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#include "QC_QSystemTrayIcon.h"
#include "QC_QObject.h"
#include "QC_QMenu.h"
#include "QC_QRect.h"
#include "QC_QIcon.h"

#include "qore-qt.h"

qore_classid_t CID_QSYSTEMTRAYICON;
class QoreClass *QC_QSystemTrayIcon = 0;

//QSystemTrayIcon ( QObject * parent = 0 )
//QSystemTrayIcon ( const QIcon & icon, QObject * parent = 0 )
static void QSYSTEMTRAYICON_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSYSTEMTRAYICON, new QoreQSystemTrayIcon(self));
      return;
   }

   const QoreObject *o = dynamic_cast<const QoreObject *>(p);
   QoreQIcon *icon = o ? (QoreQIcon *)o->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (*xsink)
      return;
   if (!icon) {
      QoreAbstractQObject *parent = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
      if (*xsink)
	 return;
      if (!parent) {
	 xsink->raiseException("QSYSTEMTRAYICON-CONSTRUCTOR-ERROR", "QSystemTrayIcon::constructor() expects either no arguments, or an oject derived from either QIcon or QObject as the first argument");
	 return;
      }
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QSYSTEMTRAYICON, new QoreQSystemTrayIcon(self, parent ? parent->getQObject() : 0));
      return;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);

   o = test_object_param(params, 1);
   QoreAbstractQObject *parent = o ? (QoreAbstractQObject *)o->getReferencedPrivateData(CID_QOBJECT, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSYSTEMTRAYICON, new QoreQSystemTrayIcon(self, *(static_cast<QIcon *>(icon)), parent ? parent->getQObject() : 0));
   return;
}

static void QSYSTEMTRAYICON_copy(class QoreObject *self, class QoreObject *old, class QoreQSystemTrayIcon *qsti, ExceptionSink *xsink)
{
   xsink->raiseException("QSYSTEMTRAYICON-COPY-ERROR", "objects of this class cannot be copied");
}

//QMenu * contextMenu () const
static AbstractQoreNode *QSYSTEMTRAYICON_contextMenu(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   QMenu *qt_qobj = qsti->qobj->contextMenu();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QMenu, getProgram());
      QoreQtQMenu *t_qobj = new QoreQtQMenu(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QMENU, t_qobj);
   }
   return rv_obj;
}

//QRect geometry () const
static AbstractQoreNode *QSYSTEMTRAYICON_geometry(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qsti->qobj->geometry());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//QIcon icon () const
static AbstractQoreNode *QSYSTEMTRAYICON_icon(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qi = new QoreObject(QC_QIcon, getProgram());
   QoreQIcon *q_qi = new QoreQIcon(qsti->qobj->icon());
   o_qi->setPrivate(CID_QICON, q_qi);
   return o_qi;
}

//bool isVisible () const
static AbstractQoreNode *QSYSTEMTRAYICON_isVisible(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qsti->qobj->isVisible());
}

//void setContextMenu ( QMenu * menu )
static AbstractQoreNode *QSYSTEMTRAYICON_setContextMenu(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQMenu *menu = p ? (QoreQMenu *)p->getReferencedPrivateData(CID_QMENU, xsink) : 0;
   if (!menu) {
      if (!xsink->isException())
         xsink->raiseException("QSYSTEMTRAYICON-SETCONTEXTMENU-PARAM-ERROR", "expecting a QMenu object as first argument to QSystemTrayIcon::setContextMenu()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> menuHolder(static_cast<AbstractPrivateData *>(menu), xsink);
   qsti->qobj->setContextMenu(static_cast<QMenu *>(menu->qobj));
   return 0;
}

//void setIcon ( const QIcon & icon )
static AbstractQoreNode *QSYSTEMTRAYICON_setIcon(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *p = test_object_param(params, 0);
   QoreQIcon *icon = p ? (QoreQIcon *)p->getReferencedPrivateData(CID_QICON, xsink) : 0;
   if (!icon) {
      if (!xsink->isException())
         xsink->raiseException("QSYSTEMTRAYICON-SETICON-PARAM-ERROR", "expecting a QIcon object as first argument to QSystemTrayIcon::setIcon()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> iconHolder(static_cast<AbstractPrivateData *>(icon), xsink);
   qsti->qobj->setIcon(*(static_cast<QIcon *>(icon)));
   return 0;
}

//void setToolTip ( const QString & tip )
static AbstractQoreNode *QSYSTEMTRAYICON_setToolTip(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString tip;
   if (get_qstring(p, tip, xsink))
      return 0;
   qsti->qobj->setToolTip(tip);
   return 0;
}

//void showMessage ( const QString & title, const QString & message, MessageIcon icon = Information, int millisecondsTimeoutHint = 10000 )
static AbstractQoreNode *QSYSTEMTRAYICON_showMessage(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString title;
   if (get_qstring(p, title, xsink))
      return 0;
   p = get_param(params, 1);
   QString message;
   if (get_qstring(p, message, xsink))
      return 0;
   p = get_param(params, 2);
   QSystemTrayIcon::MessageIcon icon = !is_nothing(p) ? (QSystemTrayIcon::MessageIcon)p->getAsInt() : QSystemTrayIcon::Information;
   p = get_param(params, 3);
   int millisecondsTimeoutHint = !is_nothing(p) ? p->getAsInt() : 10000;
   qsti->qobj->showMessage(title, message, icon, millisecondsTimeoutHint);
   return 0;
}

//QString toolTip () const
static AbstractQoreNode *QSYSTEMTRAYICON_toolTip(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qsti->qobj->toolTip().toUtf8().data(), QCS_UTF8);
}

//void hide ()
static AbstractQoreNode *QSYSTEMTRAYICON_hide(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   qsti->qobj->hide();
   return 0;
}

//void setVisible ( bool visible )
static AbstractQoreNode *QSYSTEMTRAYICON_setVisible(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   bool visible = p ? p->getAsBool() : false;
   qsti->qobj->setVisible(visible);
   return 0;
}

//void show ()
static AbstractQoreNode *QSYSTEMTRAYICON_show(QoreObject *self, QoreQSystemTrayIcon *qsti, const QoreListNode *params, ExceptionSink *xsink)
{
   qsti->qobj->show();
   return 0;
}

static QoreClass *initQSystemTrayIconClass(QoreClass *qobject)
{
   QC_QSystemTrayIcon = new QoreClass("QSystemTrayIcon", QDOM_GUI);
   CID_QSYSTEMTRAYICON = QC_QSystemTrayIcon->getID();

   QC_QSystemTrayIcon->addBuiltinVirtualBaseClass(qobject);

   QC_QSystemTrayIcon->setConstructor(QSYSTEMTRAYICON_constructor);
   QC_QSystemTrayIcon->setCopy((q_copy_t)QSYSTEMTRAYICON_copy);

   QC_QSystemTrayIcon->addMethod("contextMenu",                 (q_method_t)QSYSTEMTRAYICON_contextMenu);
   QC_QSystemTrayIcon->addMethod("geometry",                    (q_method_t)QSYSTEMTRAYICON_geometry);
   QC_QSystemTrayIcon->addMethod("icon",                        (q_method_t)QSYSTEMTRAYICON_icon);
   QC_QSystemTrayIcon->addMethod("isVisible",                   (q_method_t)QSYSTEMTRAYICON_isVisible);
   QC_QSystemTrayIcon->addMethod("setContextMenu",              (q_method_t)QSYSTEMTRAYICON_setContextMenu);
   QC_QSystemTrayIcon->addMethod("setIcon",                     (q_method_t)QSYSTEMTRAYICON_setIcon);
   QC_QSystemTrayIcon->addMethod("setToolTip",                  (q_method_t)QSYSTEMTRAYICON_setToolTip);
   QC_QSystemTrayIcon->addMethod("showMessage",                 (q_method_t)QSYSTEMTRAYICON_showMessage);
   QC_QSystemTrayIcon->addMethod("toolTip",                     (q_method_t)QSYSTEMTRAYICON_toolTip);
   QC_QSystemTrayIcon->addMethod("hide",                        (q_method_t)QSYSTEMTRAYICON_hide);
   QC_QSystemTrayIcon->addMethod("setVisible",                  (q_method_t)QSYSTEMTRAYICON_setVisible);
   QC_QSystemTrayIcon->addMethod("show",                        (q_method_t)QSYSTEMTRAYICON_show);

   return QC_QSystemTrayIcon;
}

QoreNamespace *initQSystemTrayIconNS(QoreClass *qobject)
{
   QoreNamespace *ns = new QoreNamespace("QSystemTrayIcon");

   ns->addSystemClass(initQSystemTrayIconClass(qobject));

   // ActivateReason enum
   ns->addConstant("Unknown",                  new QoreBigIntNode(QSystemTrayIcon::Unknown));
   ns->addConstant("Context",                  new QoreBigIntNode(QSystemTrayIcon::Context));
   ns->addConstant("DoubleClick",              new QoreBigIntNode(QSystemTrayIcon::DoubleClick));
   ns->addConstant("Trigger",                  new QoreBigIntNode(QSystemTrayIcon::Trigger));
   ns->addConstant("MiddleClick",              new QoreBigIntNode(QSystemTrayIcon::MiddleClick));

   // MessageIcon enum
   ns->addConstant("NoIcon",                   new QoreBigIntNode(QSystemTrayIcon::NoIcon));
   ns->addConstant("Information",              new QoreBigIntNode(QSystemTrayIcon::Information));
   ns->addConstant("Warning",                  new QoreBigIntNode(QSystemTrayIcon::Warning));
   ns->addConstant("Critical",                 new QoreBigIntNode(QSystemTrayIcon::Critical));

   return ns;
}

//bool isSystemTrayAvailable ()
static AbstractQoreNode *f_QSystemTrayIcon_isSystemTrayAvailable(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QSystemTrayIcon::isSystemTrayAvailable());
}

//bool supportsMessages ()
static AbstractQoreNode *f_QSystemTrayIcon_supportsMessages(const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(QSystemTrayIcon::supportsMessages());
}

void initQSystemTrayIconStaticFunctions()
{
   builtinFunctions.add("QSystemTrayIcon_isSystemTrayAvailable",        f_QSystemTrayIcon_isSystemTrayAvailable, QDOM_GUI);
   builtinFunctions.add("QSystemTrayIcon_supportsMessages",             f_QSystemTrayIcon_supportsMessages, QDOM_GUI);
}
