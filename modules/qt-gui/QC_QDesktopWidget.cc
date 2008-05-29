/*
 QC_QDesktopWidget.cc
 
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

#include "QC_QDesktopWidget.h"
#include "QC_QWidget.h"
#include "QC_QPoint.h"
#include "QC_QRect.h"

#include "qore-qt.h"

qore_classid_t CID_QDESKTOPWIDGET;
class QoreClass *QC_QDesktopWidget = 0;

//QDesktopWidget ()
static void QDESKTOPWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   self->setPrivate(CID_QDESKTOPWIDGET, new QoreQDesktopWidget(self));
}

static void QDESKTOPWIDGET_copy(class QoreObject *self, class QoreObject *old, class QoreQDesktopWidget *qdw, ExceptionSink *xsink)
{
   xsink->raiseException("QDESKTOPWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//const QRect availableGeometry ( int screen = -1 ) const
//const QRect availableGeometry ( const QWidget * widget ) const
//const QRect availableGeometry ( const QPoint & p ) const
static AbstractQoreNode *QDESKTOPWIDGET_availableGeometry(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
         if (!widget) {
            if (!xsink->isException())
               xsink->raiseException("QDESKTOPWIDGET-AVAILABLEGEOMETRY-PARAM-ERROR", "QDesktopWidget::availableGeometry() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
         QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
         QoreQRect *q_qr = new QoreQRect(qdw->qobj->availableGeometry(static_cast<QWidget *>(widget->getQWidget())));
         o_qr->setPrivate(CID_QRECT, q_qr);
         return o_qr;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
      QoreQRect *q_qr = new QoreQRect(qdw->qobj->availableGeometry(*(static_cast<QPoint *>(point))));
      o_qr->setPrivate(CID_QRECT, q_qr);
      return o_qr;
   }
   int screen = !is_nothing(p) ? p->getAsInt() : -1;
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qdw->qobj->availableGeometry(screen));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//bool isVirtualDesktop () const
static AbstractQoreNode *QDESKTOPWIDGET_isVirtualDesktop(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qdw->qobj->isVirtualDesktop());
}

//int numScreens () const
static AbstractQoreNode *QDESKTOPWIDGET_numScreens(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdw->qobj->numScreens());
}

//int primaryScreen () const
static AbstractQoreNode *QDESKTOPWIDGET_primaryScreen(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qdw->qobj->primaryScreen());
}

//QWidget * screen ( int screen = -1 )
static AbstractQoreNode *QDESKTOPWIDGET_screen(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int screen = !is_nothing(p) ? p->getAsInt() : -1;
   QWidget *qt_qobj = qdw->qobj->screen(screen);
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QWidget, getProgram());
      QoreQtQWidget *t_qobj = new QoreQtQWidget(rv_obj, qt_qobj);
      rv_obj->setPrivate(CID_QWIDGET, t_qobj);
   }
   return rv_obj;
}

//const QRect screenGeometry ( int screen = -1 ) const
//const QRect screenGeometry ( const QWidget * widget ) const
//const QRect screenGeometry ( const QPoint & p ) const
static AbstractQoreNode *QDESKTOPWIDGET_screenGeometry(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *point = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!point) {
         QoreQWidget *widget = (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink);
         if (!widget) {
            if (!xsink->isException())
               xsink->raiseException("QDESKTOPWIDGET-SCREENGEOMETRY-PARAM-ERROR", "QDesktopWidget::screenGeometry() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
            return 0;
         }
         ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
         QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
         QoreQRect *q_qr = new QoreQRect(qdw->qobj->screenGeometry(static_cast<QWidget *>(widget->getQWidget())));
         o_qr->setPrivate(CID_QRECT, q_qr);
         return o_qr;
      }
      ReferenceHolder<AbstractPrivateData> pHolder(static_cast<AbstractPrivateData *>(point), xsink);
      QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
      QoreQRect *q_qr = new QoreQRect(qdw->qobj->screenGeometry(*(static_cast<QPoint *>(point))));
      o_qr->setPrivate(CID_QRECT, q_qr);
      return o_qr;
   }
   int screen = !is_nothing(p) ? p->getAsInt() : -1;
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qdw->qobj->screenGeometry(screen));
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//int screenNumber ( const QWidget * widget = 0 ) const
//int screenNumber ( const QPoint & point ) const
static AbstractQoreNode *QDESKTOPWIDGET_screenNumber(QoreObject *self, QoreQDesktopWidget *qdw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      return new QoreBigIntNode(qdw->qobj->screenNumber());
   }
   QoreQPoint *point = (p->getType() == NT_OBJECT) ? (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!point) {
      QoreQWidget *widget = (p->getType() == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
      if (!widget) {
	 if (*xsink)
	    return 0;
	 if (p->getType() == NT_OBJECT)
	    xsink->raiseException("QDESKTOPWIDGET-SCREENNUMBER-PARAM-ERROR", "expecting either NOTHING, a QWidget, or a QPoint object as the sole argument to QDesktopWidget::screenNumber(), got class '%s'", (reinterpret_cast<const QoreObject *>(p))->getClassName());
	 else
	    xsink->raiseException("QDESKTOPWIDGET-SCREENNUMBER-PARAM-ERROR", "expecting either NOTHING, a QWidget, or a QPoint object as the sole argument to QDesktopWidget::screenNumber(), got '%s'", p->getTypeName());
      }
      ReferenceHolder<AbstractPrivateData> widgetHolder(static_cast<AbstractPrivateData *>(widget), xsink);
      return new QoreBigIntNode(qdw->qobj->screenNumber(widget ? static_cast<QWidget *>(widget->getQWidget()) : 0));
   }
   ReferenceHolder<AbstractPrivateData> pointHolder(static_cast<AbstractPrivateData *>(point), xsink);
   return new QoreBigIntNode(qdw->qobj->screenNumber(*(static_cast<QPoint *>(point))));
}

QoreClass *initQDesktopWidgetClass(QoreClass *qwidget)
{
   QC_QDesktopWidget = new QoreClass("QDesktopWidget", QDOM_GUI);
   CID_QDESKTOPWIDGET = QC_QDesktopWidget->getID();

   QC_QDesktopWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QDesktopWidget->setConstructor(QDESKTOPWIDGET_constructor);
   QC_QDesktopWidget->setCopy((q_copy_t)QDESKTOPWIDGET_copy);

   QC_QDesktopWidget->addMethod("availableGeometry",           (q_method_t)QDESKTOPWIDGET_availableGeometry);
   QC_QDesktopWidget->addMethod("isVirtualDesktop",            (q_method_t)QDESKTOPWIDGET_isVirtualDesktop);
   QC_QDesktopWidget->addMethod("numScreens",                  (q_method_t)QDESKTOPWIDGET_numScreens);
   QC_QDesktopWidget->addMethod("primaryScreen",               (q_method_t)QDESKTOPWIDGET_primaryScreen);
   QC_QDesktopWidget->addMethod("screen",                      (q_method_t)QDESKTOPWIDGET_screen);
   QC_QDesktopWidget->addMethod("screenGeometry",              (q_method_t)QDESKTOPWIDGET_screenGeometry);
   QC_QDesktopWidget->addMethod("screenNumber",                (q_method_t)QDESKTOPWIDGET_screenNumber);

   return QC_QDesktopWidget;
}
