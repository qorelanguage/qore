/*
 QC_QSplitterHandle.cc
 
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

#include "qore-qt.h"

#include "QC_QSplitterHandle.h"
#include "QC_QSplitter.h"

qore_classid_t CID_QSPLITTERHANDLE;
QoreClass *QC_QSplitterHandle = 0;

//QSplitterHandle ( Qt::Orientation orientation, QSplitter * parent )
static void QSPLITTERHANDLE_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQSplitter *parent = (p && p->getType() == NT_OBJECT) ? (QoreQSplitter *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QSPLITTER, xsink) : 0;
   if (!parent) {
      if (!xsink->isException())
         xsink->raiseException("QSPLITTERHANDLE-CONSTRUCTOR-PARAM-ERROR", "expecting a QSplitter object as second argument to QSplitterHandle::constructor()");
      return;
   }
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QSPLITTERHANDLE, new QoreQSplitterHandle(self, orientation, static_cast<QSplitter *>(parent->qobj)));
   return;
}

static void QSPLITTERHANDLE_copy(QoreObject *self, QoreObject *old, QoreQSplitterHandle *qsh, ExceptionSink *xsink)
{
   xsink->raiseException("QSPLITTERHANDLE-COPY-ERROR", "objects of this class cannot be copied");
}

//bool opaqueResize () const
static AbstractQoreNode *QSPLITTERHANDLE_opaqueResize(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qsh->getQSplitterHandle()->opaqueResize());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QSPLITTERHANDLE_orientation(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qsh->getQSplitterHandle()->orientation());
}

//void setOrientation ( Qt::Orientation orientation )
static AbstractQoreNode *QSPLITTERHANDLE_setOrientation(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   qsh->getQSplitterHandle()->setOrientation(orientation);
   return 0;
}

//QSplitter * splitter () const
static AbstractQoreNode *QSPLITTERHANDLE_splitter(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   QSplitter *qt_qobj = qsh->getQSplitterHandle()->splitter();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QSplitter, getProgram());
   QoreQtQSplitter *t_qobj = new QoreQtQSplitter(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QSPLITTER, t_qobj);
   return rv_obj;
}

//int closestLegalPosition ( int pos )
static AbstractQoreNode *QSPLITTERHANDLE_closestLegalPosition(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qsh->closestLegalPosition(pos));
}

//void moveSplitter ( int pos )
static AbstractQoreNode *QSPLITTERHANDLE_moveSplitter(QoreObject *self, QoreAbstractQSplitterHandle *qsh, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int pos = p ? p->getAsInt() : 0;
   qsh->moveSplitter(pos);
   return 0;
}

QoreClass *initQSplitterHandleClass(QoreClass *qwidget)
{
   QC_QSplitterHandle = new QoreClass("QSplitterHandle", QDOM_GUI);
   CID_QSPLITTERHANDLE = QC_QSplitterHandle->getID();

   QC_QSplitterHandle->addBuiltinVirtualBaseClass(qwidget);

   QC_QSplitterHandle->setConstructor(QSPLITTERHANDLE_constructor);
   QC_QSplitterHandle->setCopy((q_copy_t)QSPLITTERHANDLE_copy);

   QC_QSplitterHandle->addMethod("opaqueResize",                (q_method_t)QSPLITTERHANDLE_opaqueResize);
   QC_QSplitterHandle->addMethod("orientation",                 (q_method_t)QSPLITTERHANDLE_orientation);
   QC_QSplitterHandle->addMethod("setOrientation",              (q_method_t)QSPLITTERHANDLE_setOrientation);
   QC_QSplitterHandle->addMethod("splitter",                    (q_method_t)QSPLITTERHANDLE_splitter);
   QC_QSplitterHandle->addMethod("closestLegalPosition",        (q_method_t)QSPLITTERHANDLE_closestLegalPosition);
   QC_QSplitterHandle->addMethod("moveSplitter",                (q_method_t)QSPLITTERHANDLE_moveSplitter);

   return QC_QSplitterHandle;
}
