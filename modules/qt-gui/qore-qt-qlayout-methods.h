
#if 0
class T {
#endif

   public:
      DLLLOCAL virtual void addItem(QLayoutItem * item)
      {
	 if (!m_addItem) {
	    parent_addItem(item);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(return_object(QC_QLayoutItem, new QoreQtQLayoutItem(item)));

         discard(dispatch_event_intern(qore_obj, m_addItem, *args, &xsink), &xsink);
      }

      DLLLOCAL virtual QLayoutItem *itemAt(int index) const
      {
	 if (!m_itemAt)
	    return parent_itemAt(index);

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(new QoreBigIntNode(index));

	 QoreAbstractQLayoutItemData *qli;
         // call itemAt method
	 {
	    ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_itemAt, *args, &xsink), &xsink);
	    if (xsink)
	       return parent_itemAt(index);

	    if (!rv)
	       return 0;

	    if (rv->getType() != NT_OBJECT) {
	       xsink.raiseException("ITEMAT-ERROR", "the itemAt() method did not return a QLayoutItem object");
	       return parent_itemAt(index);
	    }

	    QoreObject *o = reinterpret_cast<QoreObject *>(*rv);
	    qli = o ? (QoreAbstractQLayoutItemData *)o->getReferencedPrivateData(CID_QLAYOUTITEM, &xsink) : 0;
	 }

         if (!qli) {
            xsink.raiseException("ITEMAT-ERROR", "the itemAt() method did not return a QLayoutItem object");
            return parent_itemAt(index);
         }
         ReferenceHolder<AbstractPrivateData> holder(qli, &xsink);
	 return qli->getQLayoutItem();
      }

      DLLLOCAL virtual QLayoutItem *takeAt(int index)
      {
	 if (!m_takeAt)
	    return parent_takeAt(index);

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(new QoreBigIntNode(index));

	 QoreAbstractQLayoutItemData *qli;
         // call takeAt method
	 {
	    ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_takeAt, *args, &xsink), &xsink);
	    if (xsink)
	       return parent_takeAt(index);

	    if (!rv)
	       return 0;

	    if (rv->getType() != NT_OBJECT) {
	       xsink.raiseException("TAKEAT-ERROR", "the takeAt() method did not return a QLayoutItem object");
	       return parent_takeAt(index);
	    }

	    QoreObject *o = reinterpret_cast<QoreObject *>(*rv);
	    qli = o ? (QoreAbstractQLayoutItemData *)o->getReferencedPrivateData(CID_QLAYOUTITEM, &xsink) : 0;
	 }

         if (!qli) {
            xsink.raiseException("TAKEAT-ERROR", "the takeAt() method did not return a QLayoutItem object");
            return parent_takeAt(index);
         }
         ReferenceHolder<AbstractPrivateData> holder(qli, &xsink);
	 return qli->getQLayoutItem();
      }

      DLLLOCAL virtual int count () const
      {
	 if (!m_count)
	    return parent_count();

         ExceptionSink xsink;
         return dispatch_event_int(qore_obj, m_count, 0, &xsink);
      }

      DLLLOCAL virtual int indexOf ( QWidget * widget ) const
      {
	 if (!m_indexOf)
	    return QOREQTYPE::indexOf(widget);

         ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         if (widget)
            args->push(return_qwidget(const_cast<QWidget *>(widget), false));
         return dispatch_event_int(qore_obj, m_indexOf, *args, &xsink);
      }

#ifdef QORE_IS_QLAYOUT
      // these functions are pure virtual in QLayout
      DLLLOCAL virtual void parent_addItem ( QLayoutItem * item )
      {
      }

      DLLLOCAL virtual int parent_count () const
      {
	 return 0;
      }

      DLLLOCAL virtual QLayoutItem * parent_itemAt ( int index ) const
      {
	 return 0;
      }

      DLLLOCAL virtual QLayoutItem * parent_takeAt ( int index )
      {
	 return 0;
      }
#else
      DLLLOCAL virtual void parent_addItem ( QLayoutItem * item )
      {
	 QOREQTYPE::addItem(item);
      }

      DLLLOCAL virtual int parent_count () const
      {
	 return QOREQTYPE::count();
      }

      DLLLOCAL virtual QLayoutItem * parent_itemAt ( int index ) const
      {
	 return QOREQTYPE::itemAt(index);
      }

      DLLLOCAL virtual QLayoutItem * parent_takeAt ( int index )
      {
	 return QOREQTYPE::takeAt(index);
      }
#endif

      DLLLOCAL virtual int parent_indexOf ( QWidget * widget ) const
      {
	 return QOREQTYPE::indexOf(widget);
      }

#include "qore-qt-qlayoutitem-methods.h"

#if 0
}
#endif
