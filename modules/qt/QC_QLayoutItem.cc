/*
 QC_QLayoutItem.cc
 
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

#include "QC_QLayout.h"
#include "QC_QLayoutItem.h"

qore_classid_t CID_QLAYOUTITEM;
QoreClass *QC_QLayoutItem = 0;

bool qlayoutitem_delete_blocker(QoreObject *self, QoreAbstractQLayoutItemData *qli)
{
   bool rc = qli->layoutItemDeleteBlocker();
   //printd(5, "qlayoutitem_delete_blocker() self=%08p, qo=%08p rc=%d\n", self, qli, rc);
   return rc;
}

//QLayoutItem ( Qt::Alignment alignment = 0 )
static void QLAYOUTITEM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(!is_nothing(p) ? p->getAsInt() : 0);
   self->setPrivate(CID_QLAYOUTITEM, new QoreQLayoutItem(self, alignment));
   return;
}

static void QLAYOUTITEM_copy(QoreObject *self, QoreObject *old, QoreAbstractQLayoutItemData *qli, ExceptionSink *xsink)
{
   xsink->raiseException("QLAYOUTITEM-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::Alignment alignment () const
static AbstractQoreNode *QLAYOUTITEM_alignment(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qli->getQLayoutItem()->alignment());
}

//QSizePolicy::ControlTypes controlTypes () const
static AbstractQoreNode *QLAYOUTITEM_controlTypes(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qli->getQLayoutItem()->controlTypes());
}

//virtual Qt::Orientations expandingDirections () const = 0
static AbstractQoreNode *QLAYOUTITEM_expandingDirections(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qli->parent_expandingDirections());
}

//virtual QRect geometry () const = 0
static AbstractQoreNode *QLAYOUTITEM_geometry(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QRect, new QoreQRect(qli->parent_geometry()));
}

//virtual bool hasHeightForWidth () const
static AbstractQoreNode *QLAYOUTITEM_hasHeightForWidth(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qli->parent_hasHeightForWidth());
}

//virtual int heightForWidth ( int w ) const
static AbstractQoreNode *QLAYOUTITEM_heightForWidth(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int w = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qli->parent_heightForWidth(w));
}

//virtual void invalidate ()
static AbstractQoreNode *QLAYOUTITEM_invalidate(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   qli->parent_invalidate();
   return 0;
}

//virtual bool isEmpty () const = 0
static AbstractQoreNode *QLAYOUTITEM_isEmpty(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qli->parent_isEmpty());
}

//virtual QLayout * layout ()
static AbstractQoreNode *QLAYOUTITEM_layout(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   QLayout *qt_qobj = qli->parent_layout();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      return rv_obj->refSelf();
   rv_obj = new QoreObject(QC_QLayout, getProgram());
   QoreQtQLayout *t_qobj = new QoreQtQLayout(rv_obj, qt_qobj);
   rv_obj->setPrivate(CID_QLAYOUT, t_qobj);
   return rv_obj;
}

//virtual QSize maximumSize () const = 0
static AbstractQoreNode *QLAYOUTITEM_maximumSize(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qli->parent_maximumSize()));
}

//virtual int minimumHeightForWidth ( int w ) const
static AbstractQoreNode *QLAYOUTITEM_minimumHeightForWidth(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   int w = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qli->parent_minimumHeightForWidth(w));
}

//virtual QSize minimumSize () const = 0
static AbstractQoreNode *QLAYOUTITEM_minimumSize(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qli->parent_minimumSize()));
}

//void setAlignment ( Qt::Alignment alignment )
static AbstractQoreNode *QLAYOUTITEM_setAlignment(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qli->getQLayoutItem()->setAlignment(alignment);
   return 0;
}

//virtual void setGeometry ( const QRect & r ) = 0
static AbstractQoreNode *QLAYOUTITEM_setGeometry(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QoreQRect *r = (p && p->getType() == NT_OBJECT) ? (QoreQRect *)reinterpret_cast<const QoreObject *>(p)->getReferencedPrivateData(CID_QRECT, xsink) : 0;
   if (!r) {
      if (!xsink->isException())
         xsink->raiseException("QLAYOUTITEM-SETGEOMETRY-PARAM-ERROR", "expecting a QRect object as first argument to QLayoutItem::setGeometry()");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> rHolder(static_cast<AbstractPrivateData *>(r), xsink);
   qli->parent_setGeometry(*(static_cast<QRect *>(r)));
   return 0;
}

//virtual QSize sizeHint () const = 0
static AbstractQoreNode *QLAYOUTITEM_sizeHint(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSize, new QoreQSize(qli->parent_sizeHint()));
}

/*
//virtual QSpacerItem * spacerItem ()
static AbstractQoreNode *QLAYOUTITEM_spacerItem(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_object(QC_QSpacerItem, new QoreQSpacerItem(qli->parent_spacerItem()));
}
*/

//virtual QWidget * widget ()
static AbstractQoreNode *QLAYOUTITEM_widget(QoreObject *self, QoreAbstractQLayoutItemData *qli, const QoreListNode *params, ExceptionSink *xsink)
{
   return return_qwidget(qli->parent_widget());
}

QoreClass *initQLayoutItemClass()
{
   QC_QLayoutItem = new QoreClass("QLayoutItem", QDOM_GUI);
   CID_QLAYOUTITEM = QC_QLayoutItem->getID();

   QC_QLayoutItem->setDeleteBlocker((q_delete_blocker_t)qlayoutitem_delete_blocker);

   QC_QLayoutItem->setConstructor(QLAYOUTITEM_constructor);
   QC_QLayoutItem->setCopy((q_copy_t)QLAYOUTITEM_copy);

   QC_QLayoutItem->addMethod("alignment",                   (q_method_t)QLAYOUTITEM_alignment);
   QC_QLayoutItem->addMethod("controlTypes",                (q_method_t)QLAYOUTITEM_controlTypes);
   QC_QLayoutItem->addMethod("expandingDirections",         (q_method_t)QLAYOUTITEM_expandingDirections);
   QC_QLayoutItem->addMethod("geometry",                    (q_method_t)QLAYOUTITEM_geometry);
   QC_QLayoutItem->addMethod("hasHeightForWidth",           (q_method_t)QLAYOUTITEM_hasHeightForWidth);
   QC_QLayoutItem->addMethod("heightForWidth",              (q_method_t)QLAYOUTITEM_heightForWidth);
   QC_QLayoutItem->addMethod("invalidate",                  (q_method_t)QLAYOUTITEM_invalidate);
   QC_QLayoutItem->addMethod("isEmpty",                     (q_method_t)QLAYOUTITEM_isEmpty);
   QC_QLayoutItem->addMethod("layout",                      (q_method_t)QLAYOUTITEM_layout);
   QC_QLayoutItem->addMethod("maximumSize",                 (q_method_t)QLAYOUTITEM_maximumSize);
   QC_QLayoutItem->addMethod("minimumHeightForWidth",       (q_method_t)QLAYOUTITEM_minimumHeightForWidth);
   QC_QLayoutItem->addMethod("minimumSize",                 (q_method_t)QLAYOUTITEM_minimumSize);
   QC_QLayoutItem->addMethod("setAlignment",                (q_method_t)QLAYOUTITEM_setAlignment);
   QC_QLayoutItem->addMethod("setGeometry",                 (q_method_t)QLAYOUTITEM_setGeometry);
   QC_QLayoutItem->addMethod("sizeHint",                    (q_method_t)QLAYOUTITEM_sizeHint);
   //QC_QLayoutItem->addMethod("spacerItem",                  (q_method_t)QLAYOUTITEM_spacerItem);
   QC_QLayoutItem->addMethod("widget",                      (q_method_t)QLAYOUTITEM_widget);

   return QC_QLayoutItem;
}
