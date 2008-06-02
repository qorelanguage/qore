/*
 QC_QSvgWidget.cc
 
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

#include "qore-qt-gui.h"

#include "QC_QSvgWidget.h"
#include "QC_QSvgRenderer.h"
#include "QC_QWidget.h"

qore_classid_t CID_QSVGWIDGET;
QoreClass *QC_QSvgWidget = 0;

//QSvgWidget ( QWidget * parent = 0 )
//QSvgWidget ( const QString & file, QWidget * parent = 0 )
static void QSVGWIDGET_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QSVGWIDGET, new QoreQSvgWidget(self));
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQWidget *parent = (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink);
      if (*xsink)
         return;
      ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
      self->setPrivate(CID_QSVGWIDGET, new QoreQSvgWidget(self, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
      return;
   }
   QString file;
   if (get_qstring(p, file, xsink))
      return;
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->getType() == NT_OBJECT) ? (QoreQWidget *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSVGWIDGET, new QoreQSvgWidget(self, file, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QSVGWIDGET_copy(QoreObject *self, QoreObject *old, QoreQSvgWidget *qsw, ExceptionSink *xsink)
{
   xsink->raiseException("QSVGWIDGET-COPY-ERROR", "objects of this class cannot be copied");
}

//QSvgRenderer * renderer () const
static AbstractQoreNode *QSVGWIDGET_renderer(QoreObject *self, QoreQSvgWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   QSvgRenderer *qt_qobj = qsw->qobj->renderer();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QSvgRenderer, getProgram());
   QoreQtQSvgRenderer *t_qobj = new QoreQtQSvgRenderer(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QSVGRENDERER, t_qobj);
   return rv_obj;
}

//void load ( const QString & file )
//void load ( const QByteArray & contents )
static AbstractQoreNode *QSVGWIDGET_load(QoreObject *self, QoreQSvgWidget *qsw, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString file;
   if (get_qstring(p, file, xsink))
      return 0;
   qsw->qobj->load(file);
   return 0;
}

QoreClass *initQSvgWidgetClass(QoreClass *qwidget)
{
   QC_QSvgWidget = new QoreClass("QSvgWidget", QDOM_GUI);
   CID_QSVGWIDGET = QC_QSvgWidget->getID();

   QC_QSvgWidget->addBuiltinVirtualBaseClass(qwidget);

   QC_QSvgWidget->setConstructor(QSVGWIDGET_constructor);
   QC_QSvgWidget->setCopy((q_copy_t)QSVGWIDGET_copy);

   QC_QSvgWidget->addMethod("renderer",                    (q_method_t)QSVGWIDGET_renderer);
   QC_QSvgWidget->addMethod("load",                        (q_method_t)QSVGWIDGET_load);

   return QC_QSvgWidget;
}
