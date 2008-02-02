/*
 QC_QHeaderView.cc
 
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

#include "QC_QHeaderView.h"
#include "QC_QWidget.h"
#include "QC_QByteArray.h"
#include "QC_QPoint.h"

#include "qore-qt.h"

int CID_QHEADERVIEW;
class QoreClass *QC_QHeaderView = 0;

//QHeaderView ( Qt::Orientation orientation, QWidget * parent = 0 )
static void QHEADERVIEW_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   QoreQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (*xsink)
      return;
   ReferenceHolder<AbstractPrivateData> parentHolder(static_cast<AbstractPrivateData *>(parent), xsink);
   self->setPrivate(CID_QHEADERVIEW, new QoreQHeaderView(self, orientation, parent ? static_cast<QWidget *>(parent->getQWidget()) : 0));
   return;
}

static void QHEADERVIEW_copy(class QoreObject *self, class QoreObject *old, class QoreQHeaderView *qhv, ExceptionSink *xsink)
{
   xsink->raiseException("QHEADERVIEW-COPY-ERROR", "objects of this class cannot be copied");
}

//bool cascadingSectionResizes () const
static AbstractQoreNode *QHEADERVIEW_cascadingSectionResizes(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->cascadingSectionResizes());
}

//int count () const
static AbstractQoreNode *QHEADERVIEW_count(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->count());
}

//Qt::Alignment defaultAlignment () const
static AbstractQoreNode *QHEADERVIEW_defaultAlignment(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->defaultAlignment());
}

//int defaultSectionSize () const
static AbstractQoreNode *QHEADERVIEW_defaultSectionSize(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->defaultSectionSize());
}

//int hiddenSectionCount () const
static AbstractQoreNode *QHEADERVIEW_hiddenSectionCount(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->hiddenSectionCount());
}

//void hideSection ( int logicalIndex )
static AbstractQoreNode *QHEADERVIEW_hideSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->hideSection(logicalIndex);
   return 0;
}

//bool highlightSections () const
static AbstractQoreNode *QHEADERVIEW_highlightSections(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->highlightSections());
}

//bool isClickable () const
static AbstractQoreNode *QHEADERVIEW_isClickable(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->isClickable());
}

//bool isMovable () const
static AbstractQoreNode *QHEADERVIEW_isMovable(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->isMovable());
}

//bool isSectionHidden ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_isSectionHidden(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBoolNode(qhv->getQHeaderView()->isSectionHidden(logicalIndex));
}

//bool isSortIndicatorShown () const
static AbstractQoreNode *QHEADERVIEW_isSortIndicatorShown(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->isSortIndicatorShown());
}

//int length () const
static AbstractQoreNode *QHEADERVIEW_length(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->length());
}

//int logicalIndex ( int visualIndex ) const
static AbstractQoreNode *QHEADERVIEW_logicalIndex(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int visualIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->logicalIndex(visualIndex));
}

//int logicalIndexAt ( int position ) const
//int logicalIndexAt ( int x, int y ) const
//int logicalIndexAt ( const QPoint & pos ) const
static AbstractQoreNode *QHEADERVIEW_logicalIndexAt(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (p && p->type == NT_OBJECT) {
      QoreQPoint *pos = (QoreQPoint *)(reinterpret_cast<QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!pos) {
         if (!xsink->isException())
            xsink->raiseException("QHEADERVIEW-LOGICALINDEXAT-PARAM-ERROR", "QHeaderView::logicalIndexAt() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<QoreObject *>(p))->getClass()->getName());
         return 0;
      }
      ReferenceHolder<AbstractPrivateData> posHolder(static_cast<AbstractPrivateData *>(pos), xsink);
      return new QoreBigIntNode(qhv->getQHeaderView()->logicalIndexAt(*(static_cast<QPoint *>(pos))));
   }
   if (num_params(params) == 1) {
      int position = p ? p->getAsInt() : 0;
      return new QoreBigIntNode(qhv->getQHeaderView()->logicalIndexAt(position));
   }

   int x = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int y = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->logicalIndexAt(x, y));
}

//int minimumSectionSize () const
static AbstractQoreNode *QHEADERVIEW_minimumSectionSize(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->minimumSectionSize());
}

//void moveSection ( int from, int to )
static AbstractQoreNode *QHEADERVIEW_moveSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int from = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int to = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->moveSection(from, to);
   return 0;
}

//int offset () const
static AbstractQoreNode *QHEADERVIEW_offset(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->offset());
}

//Qt::Orientation orientation () const
static AbstractQoreNode *QHEADERVIEW_orientation(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->orientation());
}

//ResizeMode resizeMode ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_resizeMode(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->resizeMode(logicalIndex));
}

//void resizeSection ( int logicalIndex, int size )
static AbstractQoreNode *QHEADERVIEW_resizeSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int size = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->resizeSection(logicalIndex, size);
   return 0;
}

//void resizeSections ( QHeaderView::ResizeMode mode )
static AbstractQoreNode *QHEADERVIEW_resizeSections(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QHeaderView::ResizeMode mode = (QHeaderView::ResizeMode)(p ? p->getAsInt() : 0);
   qhv->getQHeaderView()->resizeSections(mode);
   return 0;
}

//bool restoreState ( const QByteArray & state )
static AbstractQoreNode *QHEADERVIEW_restoreState(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   QByteArray state;
   if (get_qbytearray(p, state, xsink))
      return 0;
   return new QoreBoolNode(qhv->getQHeaderView()->restoreState(state));
}

//QByteArray saveState () const
static AbstractQoreNode *QHEADERVIEW_saveState(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   QoreQByteArray *q_qba = new QoreQByteArray(qhv->getQHeaderView()->saveState());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//int sectionPosition ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_sectionPosition(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->sectionPosition(logicalIndex));
}

//int sectionSize ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_sectionSize(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->sectionSize(logicalIndex));
}

//int sectionSizeHint ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_sectionSizeHint(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->sectionSizeHint(logicalIndex));
}

//int sectionViewportPosition ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_sectionViewportPosition(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->sectionViewportPosition(logicalIndex));
}

//bool sectionsHidden () const
static AbstractQoreNode *QHEADERVIEW_sectionsHidden(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->sectionsHidden());
}

//bool sectionsMoved () const
static AbstractQoreNode *QHEADERVIEW_sectionsMoved(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->sectionsMoved());
}

//void setCascadingSectionResizes ( bool enable )
static AbstractQoreNode *QHEADERVIEW_setCascadingSectionResizes(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setCascadingSectionResizes(enable);
   return 0;
}

//void setClickable ( bool clickable )
static AbstractQoreNode *QHEADERVIEW_setClickable(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool clickable = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setClickable(clickable);
   return 0;
}

//void setDefaultAlignment ( Qt::Alignment alignment )
static AbstractQoreNode *QHEADERVIEW_setDefaultAlignment(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Alignment alignment = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qhv->getQHeaderView()->setDefaultAlignment(alignment);
   return 0;
}

//void setDefaultSectionSize ( int size )
static AbstractQoreNode *QHEADERVIEW_setDefaultSectionSize(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   //printd(0, "QHeaderView::setDefaultSectionSize() obj=%08p, QoreAbstractQHeaderView=%08p, QHeaderView=%08p\n", self, qhv, qhv->getQHeaderView());
   qhv->getQHeaderView()->setDefaultSectionSize(size);
   return 0;
}

//void setHighlightSections ( bool highlight )
static AbstractQoreNode *QHEADERVIEW_setHighlightSections(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool highlight = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setHighlightSections(highlight);
   return 0;
}

//void setMinimumSectionSize ( int size )
static AbstractQoreNode *QHEADERVIEW_setMinimumSectionSize(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int size = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->setMinimumSectionSize(size);
   return 0;
}

//void setMovable ( bool movable )
static AbstractQoreNode *QHEADERVIEW_setMovable(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool movable = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setMovable(movable);
   return 0;
}

//void setResizeMode ( ResizeMode mode )
//void setResizeMode ( int logicalIndex, ResizeMode mode )
static AbstractQoreNode *QHEADERVIEW_setResizeMode(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   if (num_params(params) == 1) {
      QHeaderView::ResizeMode mode = (QHeaderView::ResizeMode)(p ? p->getAsInt() : 0);
      qhv->getQHeaderView()->setResizeMode(mode);
      return 0;
   }

   int logicalIndex = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   QHeaderView::ResizeMode mode = (QHeaderView::ResizeMode)(p ? p->getAsInt() : 0);
   qhv->getQHeaderView()->setResizeMode(logicalIndex, mode);
   return 0;
}

//void setSectionHidden ( int logicalIndex, bool hide )
static AbstractQoreNode *QHEADERVIEW_setSectionHidden(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   bool hide = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setSectionHidden(logicalIndex, hide);
   return 0;
}

//void setSortIndicator ( int logicalIndex, Qt::SortOrder order )
static AbstractQoreNode *QHEADERVIEW_setSortIndicator(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   Qt::SortOrder order = (Qt::SortOrder)(p ? p->getAsInt() : 0);
   qhv->getQHeaderView()->setSortIndicator(logicalIndex, order);
   return 0;
}

//void setSortIndicatorShown ( bool show )
static AbstractQoreNode *QHEADERVIEW_setSortIndicatorShown(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool show = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setSortIndicatorShown(show);
   return 0;
}

//void setStretchLastSection ( bool stretch )
static AbstractQoreNode *QHEADERVIEW_setStretchLastSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   bool stretch = p ? p->getAsBool() : false;
   qhv->getQHeaderView()->setStretchLastSection(stretch);
   return 0;
}

//void showSection ( int logicalIndex )
static AbstractQoreNode *QHEADERVIEW_showSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->showSection(logicalIndex);
   return 0;
}

//virtual QSize sizeHint () const
static AbstractQoreNode *QHEADERVIEW_sizeHint(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qhv->getQHeaderView()->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

//Qt::SortOrder sortIndicatorOrder () const
static AbstractQoreNode *QHEADERVIEW_sortIndicatorOrder(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->sortIndicatorOrder());
}

//int sortIndicatorSection () const
static AbstractQoreNode *QHEADERVIEW_sortIndicatorSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->sortIndicatorSection());
}

//bool stretchLastSection () const
static AbstractQoreNode *QHEADERVIEW_stretchLastSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qhv->getQHeaderView()->stretchLastSection());
}

//int stretchSectionCount () const
static AbstractQoreNode *QHEADERVIEW_stretchSectionCount(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qhv->getQHeaderView()->stretchSectionCount());
}

//void swapSections ( int first, int second )
static AbstractQoreNode *QHEADERVIEW_swapSections(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int first = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int second = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->swapSections(first, second);
   return 0;
}

//int visualIndex ( int logicalIndex ) const
static AbstractQoreNode *QHEADERVIEW_visualIndex(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int logicalIndex = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->visualIndex(logicalIndex));
}

//int visualIndexAt ( int position ) const
static AbstractQoreNode *QHEADERVIEW_visualIndexAt(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int position = p ? p->getAsInt() : 0;
   return new QoreBigIntNode(qhv->getQHeaderView()->visualIndexAt(position));
}

//void headerDataChanged ( Qt::Orientation orientation, int logicalFirst, int logicalLast )
static AbstractQoreNode *QHEADERVIEW_headerDataChanged(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   Qt::Orientation orientation = (Qt::Orientation)(p ? p->getAsInt() : 0);
   p = get_param(params, 1);
   int logicalFirst = p ? p->getAsInt() : 0;
   p = get_param(params, 2);
   int logicalLast = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->headerDataChanged(orientation, logicalFirst, logicalLast);
   return 0;
}

//void setOffset ( int offset )
static AbstractQoreNode *QHEADERVIEW_setOffset(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int offset = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->setOffset(offset);
   return 0;
}

//void setOffsetToLastSection ()
static AbstractQoreNode *QHEADERVIEW_setOffsetToLastSection(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   qhv->getQHeaderView()->setOffsetToLastSection();
   return 0;
}

//void setOffsetToSectionPosition ( int visualIndex )
static AbstractQoreNode *QHEADERVIEW_setOffsetToSectionPosition(QoreObject *self, QoreAbstractQHeaderView *qhv, const QoreListNode *params, ExceptionSink *xsink)
{
   AbstractQoreNode *p = get_param(params, 0);
   int visualIndex = p ? p->getAsInt() : 0;
   qhv->getQHeaderView()->setOffsetToSectionPosition(visualIndex);
   return 0;
}

QoreClass *initQHeaderViewClass(QoreClass *qabstractitemview)
{
   QC_QHeaderView = new QoreClass("QHeaderView", QDOM_GUI);
   CID_QHEADERVIEW = QC_QHeaderView->getID();

   QC_QHeaderView->addBuiltinVirtualBaseClass(qabstractitemview);

   QC_QHeaderView->setConstructor(QHEADERVIEW_constructor);
   QC_QHeaderView->setCopy((q_copy_t)QHEADERVIEW_copy);

   QC_QHeaderView->addMethod("cascadingSectionResizes",     (q_method_t)QHEADERVIEW_cascadingSectionResizes);
   QC_QHeaderView->addMethod("count",                       (q_method_t)QHEADERVIEW_count);
   QC_QHeaderView->addMethod("defaultAlignment",            (q_method_t)QHEADERVIEW_defaultAlignment);
   QC_QHeaderView->addMethod("defaultSectionSize",          (q_method_t)QHEADERVIEW_defaultSectionSize);
   QC_QHeaderView->addMethod("hiddenSectionCount",          (q_method_t)QHEADERVIEW_hiddenSectionCount);
   QC_QHeaderView->addMethod("hideSection",                 (q_method_t)QHEADERVIEW_hideSection);
   QC_QHeaderView->addMethod("highlightSections",           (q_method_t)QHEADERVIEW_highlightSections);
   QC_QHeaderView->addMethod("isClickable",                 (q_method_t)QHEADERVIEW_isClickable);
   QC_QHeaderView->addMethod("isMovable",                   (q_method_t)QHEADERVIEW_isMovable);
   QC_QHeaderView->addMethod("isSectionHidden",             (q_method_t)QHEADERVIEW_isSectionHidden);
   QC_QHeaderView->addMethod("isSortIndicatorShown",        (q_method_t)QHEADERVIEW_isSortIndicatorShown);
   QC_QHeaderView->addMethod("length",                      (q_method_t)QHEADERVIEW_length);
   QC_QHeaderView->addMethod("logicalIndex",                (q_method_t)QHEADERVIEW_logicalIndex);
   QC_QHeaderView->addMethod("logicalIndexAt",              (q_method_t)QHEADERVIEW_logicalIndexAt);
   QC_QHeaderView->addMethod("minimumSectionSize",          (q_method_t)QHEADERVIEW_minimumSectionSize);
   QC_QHeaderView->addMethod("moveSection",                 (q_method_t)QHEADERVIEW_moveSection);
   QC_QHeaderView->addMethod("offset",                      (q_method_t)QHEADERVIEW_offset);
   QC_QHeaderView->addMethod("orientation",                 (q_method_t)QHEADERVIEW_orientation);
   QC_QHeaderView->addMethod("resizeMode",                  (q_method_t)QHEADERVIEW_resizeMode);
   QC_QHeaderView->addMethod("resizeSection",               (q_method_t)QHEADERVIEW_resizeSection);
   QC_QHeaderView->addMethod("resizeSections",              (q_method_t)QHEADERVIEW_resizeSections);
   QC_QHeaderView->addMethod("restoreState",                (q_method_t)QHEADERVIEW_restoreState);
   QC_QHeaderView->addMethod("saveState",                   (q_method_t)QHEADERVIEW_saveState);
   QC_QHeaderView->addMethod("sectionPosition",             (q_method_t)QHEADERVIEW_sectionPosition);
   QC_QHeaderView->addMethod("sectionSize",                 (q_method_t)QHEADERVIEW_sectionSize);
   QC_QHeaderView->addMethod("sectionSizeHint",             (q_method_t)QHEADERVIEW_sectionSizeHint);
   QC_QHeaderView->addMethod("sectionViewportPosition",     (q_method_t)QHEADERVIEW_sectionViewportPosition);
   QC_QHeaderView->addMethod("sectionsHidden",              (q_method_t)QHEADERVIEW_sectionsHidden);
   QC_QHeaderView->addMethod("sectionsMoved",               (q_method_t)QHEADERVIEW_sectionsMoved);
   QC_QHeaderView->addMethod("setCascadingSectionResizes",  (q_method_t)QHEADERVIEW_setCascadingSectionResizes);
   QC_QHeaderView->addMethod("setClickable",                (q_method_t)QHEADERVIEW_setClickable);
   QC_QHeaderView->addMethod("setDefaultAlignment",         (q_method_t)QHEADERVIEW_setDefaultAlignment);
   QC_QHeaderView->addMethod("setDefaultSectionSize",       (q_method_t)QHEADERVIEW_setDefaultSectionSize);
   QC_QHeaderView->addMethod("setHighlightSections",        (q_method_t)QHEADERVIEW_setHighlightSections);
   QC_QHeaderView->addMethod("setMinimumSectionSize",       (q_method_t)QHEADERVIEW_setMinimumSectionSize);
   QC_QHeaderView->addMethod("setMovable",                  (q_method_t)QHEADERVIEW_setMovable);
   QC_QHeaderView->addMethod("setResizeMode",               (q_method_t)QHEADERVIEW_setResizeMode);
   QC_QHeaderView->addMethod("setSectionHidden",            (q_method_t)QHEADERVIEW_setSectionHidden);
   QC_QHeaderView->addMethod("setSortIndicator",            (q_method_t)QHEADERVIEW_setSortIndicator);
   QC_QHeaderView->addMethod("setSortIndicatorShown",       (q_method_t)QHEADERVIEW_setSortIndicatorShown);
   QC_QHeaderView->addMethod("setStretchLastSection",       (q_method_t)QHEADERVIEW_setStretchLastSection);
   QC_QHeaderView->addMethod("showSection",                 (q_method_t)QHEADERVIEW_showSection);
   QC_QHeaderView->addMethod("sizeHint",                    (q_method_t)QHEADERVIEW_sizeHint);
   QC_QHeaderView->addMethod("sortIndicatorOrder",          (q_method_t)QHEADERVIEW_sortIndicatorOrder);
   QC_QHeaderView->addMethod("sortIndicatorSection",        (q_method_t)QHEADERVIEW_sortIndicatorSection);
   QC_QHeaderView->addMethod("stretchLastSection",          (q_method_t)QHEADERVIEW_stretchLastSection);
   QC_QHeaderView->addMethod("stretchSectionCount",         (q_method_t)QHEADERVIEW_stretchSectionCount);
   QC_QHeaderView->addMethod("swapSections",                (q_method_t)QHEADERVIEW_swapSections);
   QC_QHeaderView->addMethod("visualIndex",                 (q_method_t)QHEADERVIEW_visualIndex);
   QC_QHeaderView->addMethod("visualIndexAt",               (q_method_t)QHEADERVIEW_visualIndexAt);
   QC_QHeaderView->addMethod("headerDataChanged",           (q_method_t)QHEADERVIEW_headerDataChanged);
   QC_QHeaderView->addMethod("setOffset",                   (q_method_t)QHEADERVIEW_setOffset);
   QC_QHeaderView->addMethod("setOffsetToLastSection",      (q_method_t)QHEADERVIEW_setOffsetToLastSection);
   QC_QHeaderView->addMethod("setOffsetToSectionPosition",  (q_method_t)QHEADERVIEW_setOffsetToSectionPosition);

   return QC_QHeaderView;
}
