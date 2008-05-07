
#if 0
class T {
#endif

   protected:
      DLLLOCAL virtual void drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const 
      {
	 if (!m_drawComplexControl) {
	    parent_drawComplexControl(control, option, painter, widget);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(control));
	 args->push(return_object(QC_QStyleOptionComplex, new QoreQStyleOptionComplex(*option)));
	 args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 discard(dispatch_event_intern(qore_obj, m_drawComplexControl, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual void drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      {
	 if (!m_drawControl) {
	    parent_drawControl(element, option, painter, widget);		
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(element));
	 args->push(return_qstyleoption(option));
	 args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 discard(dispatch_event_intern(qore_obj, m_drawControl, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      { 
	 if (!m_drawItemPixmap) {
	    parent_drawItemPixmap(painter, rectangle, alignment, pixmap);	
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 args->push(return_object(QC_QRect, new QoreQRect(rectangle)));
	 args->push(new QoreBigIntNode(alignment));
	 args->push(return_object(QC_QPixmap, new QoreQPixmap(pixmap)));

	 discard(dispatch_event_intern(qore_obj, m_drawItemPixmap, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const 
      { 
	 if (!m_drawItemText) {
	    parent_drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole); 
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 args->push(return_object(QC_QRect, new QoreQRect(rectangle)));
	 args->push(new QoreBigIntNode(alignment));
	 args->push(return_object(QC_QPalette, new QoreQPalette(palette)));
	 args->push(get_bool_node(enabled));
	 args->push(new QoreStringNode(text.toUtf8().data()));
	 args->push(new QoreBigIntNode(textRole));

	 discard(dispatch_event_intern(qore_obj, m_drawItemText, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      {
	 if (!m_itemPixmapRect)
	    return parent_itemPixmapRect(rectangle, alignment, pixmap); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_object(QC_QRect, new QoreQRect(rectangle)));
	 args->push(new QoreBigIntNode(alignment));
	 args->push(return_object(QC_QPixmap, new QoreQPixmap(pixmap)));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_itemPixmapRect, *args, &xsink), &xsink);

         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQRect *qrect = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;
	 if (!qrect) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-ITEMPIXMAPRECT-ERROR", "%s::itemPixmapRect() did not return a QRect object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QRect();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qrect), &xsink);
	 return *qrect;
      }

      DLLLOCAL virtual void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
      {
	 if (!m_drawPrimitive) {
	    parent_drawPrimitive(element, option, painter, widget);
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(element));
	 args->push(return_qstyleoption(option));
	 args->push(return_object(QC_QPainter, new QoreQPainter(painter)));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 discard(dispatch_event_intern(qore_obj, m_drawPrimitive, *args, &xsink), &xsink);
      }

      DLLLOCAL virtual QStyle::SubControl hitTestComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, const QPoint &point, const QWidget *widget = 0) const
      {
	 if (!m_hitTestComplexControl)
	    return parent_hitTestComplexControl(control, option, point, widget);

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(control));
	 args->push(return_object(QC_QStyleOptionComplex, new QoreQStyleOptionComplex(*option)));
	 args->push(return_object(QC_QPoint, new QoreQPoint(point)));
	 if (widget)
	    args->push(return_qobject(const_cast<QWidget *>(widget)));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_hitTestComplexControl, *args, &xsink), &xsink);
	 return (QStyle::SubControl)(*rv ? rv->getAsInt() : 0);
      }

      DLLLOCAL virtual QPixmap standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *option = 0, const QWidget *widget = 0) const 
      {
	 if (!m_standardPixmap)
	    return parent_standardPixmap(standardPixmap, option, widget); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(standardPixmap));
	 args->push(return_qstyleoption(option));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_standardPixmap, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQPixmap *qpixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, &xsink) : 0;
	 if (!qpixmap) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-STANDARDPIXMAP-ERROR", "%s::standardPixmap() did not return a QPixmap object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QPixmap();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qpixmap), &xsink);
	 return *qpixmap;
      }
      
      DLLLOCAL virtual QPixmap generatedIconPixmap(QIcon::Mode mode, const QPixmap &pixmap, const QStyleOption *option) const
      {
	 if (!m_generatedIconPixmap)
	    return parent_generatedIconPixmap(mode, pixmap, option); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(mode));
	 args->push(return_object(QC_QPixmap, new QoreQPixmap(pixmap)));
	 args->push(return_qstyleoption(option));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_generatedIconPixmap, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQPixmap *qpixmap = o ? (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, &xsink) : 0;
	 if (!qpixmap) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-GENERATEDICONPIXMAP-ERROR", "%s::generatedIconPixmap() did not return a QPixmap object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QPixmap();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qpixmap), &xsink);
	 return *qpixmap;
      }

      DLLLOCAL virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const
      {
	 if (!m_itemTextRect)
	    return parent_itemTextRect(metrics, rectangle, alignment, enabled, text); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_object(QC_QFontMetrics, new QoreQFontMetrics(metrics)));
	 args->push(return_object(QC_QRect, new QoreQRect(rectangle)));
	 args->push(new QoreBigIntNode(alignment));
	 args->push(get_bool_node(enabled));
	 args->push(new QoreStringNode(text.toUtf8().data()));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_itemTextRect, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQRect *qrect = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;
	 if (!qrect) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-ITEMPIXMAPRECT-ERROR", "%s::itemPixmapRect() did not return a QRect object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QRect();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qrect), &xsink);
	 return *qrect;
      }
      DLLLOCAL virtual int pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
      {
	 if (!m_pixelMetric)
	    return parent_pixelMetric(metric, option, widget); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(metric));
	 args->push(return_qstyleoption(option));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_pixelMetric, *args, &xsink), &xsink);
	 return *rv ? rv->getAsInt() : 0;
      }
      DLLLOCAL virtual void polish ( QWidget * widget ) 
      {
	 if (!m_polish) {
	    parent_polish(widget); 
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_qwidget(widget, false));
	 discard(dispatch_event_intern(qore_obj, m_polish, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual void polish ( QApplication * application )
      {	
	 if (!m_polish) {
	    parent_polish(application); 
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_qobject(application));
	 discard(dispatch_event_intern(qore_obj, m_polish, *args, &xsink), &xsink);
      }

      DLLLOCAL virtual void polish ( QPalette & palette ) 
      {
	 if (!m_polish) {
	    parent_polish(palette); 
	    return;
	 }

	 ExceptionSink xsink;

	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 QoreObject *q_palette = return_object(QC_QPalette, new QoreQPalette(&palette));
	 args->push(q_palette);

	 // execute method and discard any return value
         discard(qore_obj->evalMethod(*m_polish, *args, &xsink), &xsink);

	 // delete the Qore object wrapper to ensure that it won't be accessed outside its scope
	 // does not delete or otherwise affect the QPalette object
	 q_palette->doDelete(&xsink);
      }

      DLLLOCAL virtual QSize sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const 
      {
         if (!m_sizeFromContents)
	    return parent_sizeFromContents(type, option, contentsSize, widget); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(type));
	 args->push(return_qstyleoption(option));
	 args->push(return_object(QC_QSize, new QoreQSize(contentsSize)));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_sizeFromContents, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQSize *qsize = o ? (QoreQSize *)o->getReferencedPrivateData(CID_QSIZE, &xsink) : 0;
	 if (!qsize) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-SIZEFROMCONTENTS-ERROR", "%s::sizeFromContents() did not return a QSize object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QSize();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qsize), &xsink);
	 return *qsize;
      }
      DLLLOCAL virtual QPalette standardPalette () const 
      {
         if (!m_standardPalette)
	    return parent_standardPalette();

	 ExceptionSink xsink;
	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_standardPalette, 0, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQPalette *qpalette = o ? (QoreQPalette *)o->getReferencedPrivateData(CID_QPALETTE, &xsink) : 0;
	 if (!qpalette) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-STANDARDPALETTE-ERROR", "%s::standardPalette() did not return a QPalette object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QPalette();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qpalette), &xsink);
	 return *(qpalette->getQPalette());
      }
      DLLLOCAL virtual int styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const 
      {
	 //printd(5, "x::styleHint(%d, %08p, %08p, %08p) called, m_styleHint=%08p\n", hint, option, widget, returnData, m_styleHint);
         if (!m_styleHint)
	    return parent_styleHint(hint, option, widget, returnData); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(hint));
	 args->push(return_qstyleoption(option));

	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 // returnData not currently implemented

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_styleHint, *args, &xsink), &xsink);
	 return *rv ? rv->getAsInt() : 0;
      }
      DLLLOCAL virtual QRect subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const 
      {
         if (!m_subControlRect)
	    return parent_subControlRect(control, option, subControl, widget); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(control));
	 args->push(return_qstyleoption(option));
	 args->push(new QoreBigIntNode(subControl));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_subControlRect, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQRect *qrect = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;
	 if (!qrect) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-SUBCONTROLRECT-ERROR", "%s::subControlRect() did not return a QRect object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QRect();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qrect), &xsink);
	 return *qrect;
      }
      DLLLOCAL virtual QRect subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const 
      {
         if (!m_subElementRect)
	    return parent_subElementRect(element, option, widget); 

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(new QoreBigIntNode(element));
	 args->push(return_qstyleoption(option));
	 if (widget)
	    args->push(return_qwidget(const_cast<QWidget *>(widget), false));

	 ReferenceHolder<AbstractQoreNode> rv(dispatch_event_intern(qore_obj, m_subElementRect, *args, &xsink), &xsink);
         QoreObject *o = dynamic_cast<QoreObject *>(*rv);
	 QoreQRect *qrect = o ? (QoreQRect *)o->getReferencedPrivateData(CID_QRECT, &xsink) : 0;
	 if (!qrect) {
	    if (!xsink)
	       xsink.raiseException("QSTYLE-SUBELEMENTRECT-ERROR", "%s::subElementRect() did not return a QRect object (got type '%s' instead)",
	       qore_obj->getClass()->getName(), *rv ? rv->getTypeName() : "NOTHING");
	    return QRect();
	 }
	 ReferenceHolder<AbstractPrivateData> holder(static_cast<AbstractPrivateData *>(qrect), &xsink);
	 return *qrect;
      }
      DLLLOCAL virtual void unpolish ( QWidget * widget ) 
      {		
         if (!m_unpolish) {
	    parent_unpolish(widget);
            return;
         }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_qwidget(widget, false));
	 discard(dispatch_event_intern(qore_obj, m_unpolish, *args, &xsink), &xsink);
      }
      DLLLOCAL virtual void unpolish ( QApplication * application ) 
      {
         if (!m_unpolish) {
	    parent_unpolish(application); 
	    return;
	 }

	 ExceptionSink xsink;
         ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(return_qobject(application));
	 discard(dispatch_event_intern(qore_obj, m_unpolish, *args, &xsink), &xsink);
      }

   public:
#ifdef QORE_IS_QSTYLE
      DLLLOCAL virtual void parent_drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
      }
      DLLLOCAL virtual void parent_drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
      }
      DLLLOCAL virtual void parent_drawPrimitive ( QStyle::PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
      }									
      DLLLOCAL virtual QPixmap parent_generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const
      {
	 return QPixmap();
      }									
      DLLLOCAL virtual QStyle::SubControl parent_hitTestComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const 
      { 
	 return QStyle::SC_None;
      }									
      DLLLOCAL virtual int parent_pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
      {
	 return 0;
      }									
      DLLLOCAL virtual QSize parent_sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const 
      {
	 return QSize();
      }
      DLLLOCAL virtual QPixmap parent_standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *opt = 0, const QWidget *widget = 0) const 
      {
	 return QPixmap();
      }
      DLLLOCAL virtual int parent_styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const 
      {
	 return 0;
      }									
      DLLLOCAL virtual QRect parent_subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const 
      {
	 return QRect();
      }									
      DLLLOCAL virtual QRect parent_subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const 
      {
	 return QRect();
      }									
#else
      DLLLOCAL virtual void parent_drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 QOREQTYPE::drawComplexControl(control, option, painter, widget);	
      }
      DLLLOCAL virtual void parent_drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 QOREQTYPE::drawControl(element, option, painter, widget);		
      }
      DLLLOCAL virtual void parent_drawPrimitive ( QStyle::PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 QOREQTYPE::drawPrimitive(element, option, painter, widget); 
      }									
      DLLLOCAL virtual QPixmap parent_generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const
      {
	 return QOREQTYPE::generatedIconPixmap(iconMode, pixmap, option); 
      }									
      DLLLOCAL virtual QStyle::SubControl parent_hitTestComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const 
      { 
	 return QOREQTYPE::hitTestComplexControl(control, option, position, widget); 
      }									
      DLLLOCAL virtual int parent_pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const
      {
	 return QOREQTYPE::pixelMetric(metric, option, widget); 
      }									
      DLLLOCAL virtual QSize parent_sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const 
      {
	 return QOREQTYPE::sizeFromContents(type, option, contentsSize, widget); 
      }									
      DLLLOCAL virtual QPixmap parent_standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *opt = 0, const QWidget *widget = 0) const 
      {
	 return QOREQTYPE::standardPixmap(standardPixmap, opt, widget);
      }
      DLLLOCAL virtual int parent_styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const
      {
	 //printd(5, "parent_styleHint(%d, %08p, %08p, %08p) called this=%08p\n", hint, option, widget, returnData, this);
	 int rc = QOREQTYPE::styleHint(hint, option, widget, returnData); 
	 //printd(5, "parent_styleHint() rc=%d\n", rc);
	 return rc;
      }									
      DLLLOCAL virtual QRect parent_subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const 
      {
	 return QOREQTYPE::subControlRect(control, option, subControl, widget); 
      }									
      DLLLOCAL virtual QRect parent_subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const 
      {
	 return QOREQTYPE::subElementRect(element, option, widget); 
      }									
#endif

      DLLLOCAL virtual int parent_layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { 
	 return layoutSpacingImplementation(control1, control2, orientation, option, widget); 
      }									
      DLLLOCAL virtual QIcon parent_standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { 
	 return parent_standardIconImplementation(standardIcon, option, widget); 
      }
      DLLLOCAL virtual void parent_drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      { 
	 QOREQTYPE::drawItemPixmap(painter, rectangle, alignment, pixmap);	
      }
      DLLLOCAL virtual void parent_drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const 
      { 
	 QOREQTYPE::drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole); 
      }
      DLLLOCAL virtual QRect parent_itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      {
	 return QOREQTYPE::itemPixmapRect(rectangle, alignment, pixmap); 
      }									
      DLLLOCAL virtual QRect parent_itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const
      { 
	 return QOREQTYPE::itemTextRect(metrics, rectangle, alignment, enabled, text); 
      }
      DLLLOCAL virtual void parent_polish ( QWidget * widget ) 
      {
	 QOREQTYPE::polish(widget); 
      }									
      DLLLOCAL virtual void parent_polish ( QApplication * application )
      {	
	 QOREQTYPE::polish(application); 
      }									
      DLLLOCAL virtual void parent_polish ( QPalette & palette ) 
      {
	 QOREQTYPE::polish(palette);
      } 
      DLLLOCAL virtual QPalette parent_standardPalette () const 
      {			
	 return QOREQTYPE::standardPalette(); 
      }
      DLLLOCAL virtual void parent_unpolish ( QWidget * widget ) 
      {		
	 QOREQTYPE::unpolish(widget); 
      }						
      DLLLOCAL virtual void parent_unpolish ( QApplication * application ) 
      {
	 QOREQTYPE::unpolish(application); 
      }

#if 0
}
#endif
