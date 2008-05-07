
#if 0
class T {
#endif

   public:
      DLLLOCAL virtual bool layoutItemDeleteBlocker()
      {
	 if (item_externally_owned) {
	    manual_delete = true;
	    return true;
	 }
	 return false;
      }

      DLLLOCAL virtual void setItemExternallyOwned()
      {
	 item_externally_owned = true;
      }

      DLLLOCAL virtual Qt::Orientations expandingDirections () const
      {
	 if (!m_expandingDirections)
	    return parent_expandingDirections();

	 ExceptionSink xsink;
	 return (Qt::Orientations)dispatch_event_int(qore_obj, m_expandingDirections, 0, &xsink);
      }

      DLLLOCAL virtual QRect geometry () const
      {
	 if (!m_geometry)
	    return parent_geometry();

         ExceptionSink xsink;

         // call geometry method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_geometry, 0, &xsink), &xsink);
         if (xsink)
            return parent_geometry();

         QoreObject *o = rv && rv->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*rv) : 0;
         QoreQRect *qr = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;

         if (!qr) {
            xsink.raiseException("GEOMETRY-ERROR", "the geometry() method did not return a QRect object");
            return parent_geometry();
         }
         ReferenceHolder<QoreQRect> rectHolder(qr, &xsink);
         QRect rv_qr = *(static_cast<QRect *>(qr));
         return rv_qr;
      }

      DLLLOCAL virtual bool isEmpty () const
      {
	 if (!m_isEmpty)
	    return parent_isEmpty();

         ExceptionSink xsink;
         return dispatch_event_bool(qore_obj, m_isEmpty, 0, &xsink);
      }

      DLLLOCAL virtual QSize maximumSize () const
      {
	 if (!m_maximumSize)
	    return parent_maximumSize();

         ExceptionSink xsink;

         // call maximumSize method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_maximumSize, 0, &xsink), &xsink);
         if (xsink)
            return parent_maximumSize();

         QoreObject *o = rv && rv->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*rv) : 0;
         QoreQSize *qs = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;

         if (!qs) {
            xsink.raiseException("MAXIMUMSIZE-ERROR", "the maximumSize() method did not return a QSize object");
            return parent_maximumSize();
         }
         ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
         QSize rv_qs = *(static_cast<QSize *>(qs));
         return rv_qs;
      }

      DLLLOCAL virtual QSize minimumSize () const
      {
	 if (!m_minimumSize)
	    return parent_minimumSize();

         ExceptionSink xsink;

         // call minimumSize method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_minimumSize, 0, &xsink), &xsink);
         if (xsink)
            return parent_minimumSize();

         QoreObject *o = rv && rv->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*rv) : 0;
         QoreQSize *qs = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;

         if (!qs) {
            xsink.raiseException("MINIMUMSIZE-ERROR", "the minimumSize() method did not return a QSize object");
            return parent_minimumSize();
         }
         ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
         QSize rv_qs = *(static_cast<QSize *>(qs));
         return rv_qs;
      }

      DLLLOCAL virtual void setGeometry ( const QRect & r )
      {
	 if (!m_setGeometry) {
	    parent_setGeometry(r);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(return_object(QC_QRect, new QoreQRect(r)));

         discard(dispatch_event_intern(qore_obj, m_setGeometry, *args, &xsink), &xsink);
      }

      DLLLOCAL virtual QSize sizeHint () const
      {
	 if (!m_sizeHint)
	    return parent_sizeHint();

        ExceptionSink xsink;

         // call sizeHint method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_sizeHint, 0, &xsink), &xsink);
         if (xsink)
            return QOREQTYPE::sizeHint();

         QoreObject *o = rv && rv->getType() == NT_OBJECT ? reinterpret_cast<QoreObject *>(*rv) : 0;
         QoreQSize *qs = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;

         if (!qs) {
            xsink.raiseException("SIZEHINT-ERROR", "the sizeHint() method did not return a QSize object");
            return QOREQTYPE::sizeHint();
         }
         ReferenceHolder<QoreQSize> sizeHolder(qs, &xsink);
         QSize rv_qs = *(static_cast<QSize *>(qs));
         return rv_qs;
      }

      DLLLOCAL virtual bool hasHeightForWidth () const
      {
	 if (!m_hasHeightForWidth)
	    return QOREQTYPE::hasHeightForWidth();
	 
         ExceptionSink xsink;
         return dispatch_event_bool(qore_obj, m_hasHeightForWidth, 0, &xsink);
      }

      DLLLOCAL virtual int heightForWidth ( int w ) const
      {
	 if (!m_heightForWidth)
	    return QOREQTYPE::heightForWidth(w);

         ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
         args->push(new QoreBigIntNode(w));
         
         return dispatch_event_int(qore_obj, m_heightForWidth, *args, &xsink);
      }

      DLLLOCAL virtual void invalidate ()
      {
	 if (!m_invalidate) {
	    QOREQTYPE::invalidate();
	    return;
	 }
	 
         dispatch_event(qore_obj, m_invalidate, 0);
      }

      DLLLOCAL virtual QLayout * layout ()
      {
	 if (!m_layout)
	    return QOREQTYPE::layout();

	 ExceptionSink xsink;

         // call layout method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_layout, 0, &xsink), &xsink);
	 if (xsink)
	    return QOREQTYPE::layout();
	    
	 if (!rv)
	    return 0;
	 
	 if (rv->getType() != NT_OBJECT) {
	    xsink.raiseException("LAYOUT-ERROR", "the layout() method did not return a QLayout object");
	    return QOREQTYPE::layout();
	 }
	 
	 QoreObject *o = reinterpret_cast<QoreObject *>(*rv);
	 QoreAbstractQLayout *ql = o ? (QoreAbstractQLayout *)o->getReferencedPrivateData(CID_QLAYOUT, &xsink) : 0;

         if (!ql) {
            xsink.raiseException("LAYOUT-ERROR", "the layout() method did not return a QLayout object");
            return QOREQTYPE::layout();
         }
         ReferenceHolder<AbstractPrivateData> layoutHolder(ql, &xsink);
	 return ql->getQLayout();
      }

      DLLLOCAL virtual int minimumHeightForWidth ( int w ) const
      {
	 if (!m_minimumHeightForWidth)
	    return QOREQTYPE::minimumHeightForWidth(w);

         ExceptionSink xsink;
         return dispatch_event_int(qore_obj, m_minimumHeightForWidth, 0, &xsink);
      }

      DLLLOCAL virtual QSpacerItem * spacerItem ()
      {
	 if (!m_spacerItem)
	    return QOREQTYPE::spacerItem();

	 // FIXME: implement this
	 return QOREQTYPE::spacerItem();
      }

      DLLLOCAL virtual QWidget * widget ()
      {
	 if (!m_widget)
	    return QOREQTYPE::widget();

	 ExceptionSink xsink;

         // call widget method
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_widget, 0, &xsink), &xsink);
	 if (xsink)
	    return QOREQTYPE::widget();
	 
	 if (!rv)
	    return 0;
	 
	 if (rv->getType() != NT_OBJECT) {
	    xsink.raiseException("WIDGET-ERROR", "the widget() method did not return a QWidget object");
	    return QOREQTYPE::widget();
	 }
	 
	 QoreObject *o = reinterpret_cast<QoreObject *>(*rv);
	 QoreQWidget *qw = o ? (QoreQWidget *)o->getReferencedPrivateData(CID_QWIDGET, &xsink) : 0;

         if (!qw) {
	    xsink.raiseException("WIDGET-ERROR", "the widget() method did not return a QWidget object");
	    return QOREQTYPE::widget();
         }
         ReferenceHolder<AbstractPrivateData> widgetHolder(qw, &xsink);
	 return qw->getQWidget();
      }

#if defined(QORE_IS_QLAYOUTITEM) || defined(QORE_IS_QLAYOUT)
      DLLLOCAL virtual QRect parent_geometry () const
      {
	 return QRect();
      }

      DLLLOCAL virtual bool parent_isEmpty () const
      {
	 return false;
      }

      DLLLOCAL virtual void parent_setGeometry ( const QRect & r )
      {
      }

      DLLLOCAL virtual QSize parent_sizeHint () const
      {
	 return QSize();
      }
#else
      DLLLOCAL virtual QRect parent_geometry () const
      {
	 return QOREQTYPE::geometry();
      }

      DLLLOCAL virtual bool parent_isEmpty () const
      {
	 return QOREQTYPE::isEmpty();
      }

      DLLLOCAL virtual void parent_setGeometry ( const QRect & r )
      {
	 QOREQTYPE::setGeometry(r);
      }

      DLLLOCAL virtual QSize parent_sizeHint () const
      {
	 return QOREQTYPE::sizeHint();
      }
#endif

#ifdef QORE_IS_QLAYOUTITEM
      // the following methods are pure virtual in QLayoutItem
      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const
      {
	 return 0;
      }

      DLLLOCAL virtual QSize parent_maximumSize () const
      {
	 return QSize();
      }

      DLLLOCAL virtual QSize parent_minimumSize () const
      {
	 return QSize();
      }
#else
      DLLLOCAL virtual Qt::Orientations parent_expandingDirections () const
      {
	 return QOREQTYPE::expandingDirections();
      }

      DLLLOCAL virtual QSize parent_maximumSize () const
      {
	 return QOREQTYPE::maximumSize();
      }

      DLLLOCAL virtual QSize parent_minimumSize () const
      {
	 return QOREQTYPE::minimumSize();
      }
#endif

      DLLLOCAL virtual bool parent_hasHeightForWidth () const
      {
	 return QOREQTYPE::hasHeightForWidth();
      }

      DLLLOCAL virtual int parent_heightForWidth ( int w ) const
      {
	 return QOREQTYPE::heightForWidth(w);
      }

      DLLLOCAL virtual void parent_invalidate ()
      {
	 QOREQTYPE::invalidate();
      }

      DLLLOCAL virtual QLayout * parent_layout ()
      {
	 return QOREQTYPE::layout();
      }

      DLLLOCAL virtual int parent_minimumHeightForWidth ( int w ) const
      {
	 return QOREQTYPE::minimumHeightForWidth(w);
      }

      DLLLOCAL virtual QSpacerItem * parent_spacerItem ()
      {
	 return QOREQTYPE::spacerItem();
      }

      DLLLOCAL virtual QWidget * parent_widget ()
      {
	 return QOREQTYPE::widget();
      }

#if 0
}
#endif
