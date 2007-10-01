
#include "qore-qt-static-qobject-methods.h"

#if 0
class T {
#endif

   public:

      // these functions will never be called
      DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { return 0; }
      DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { return QIcon(); }

      // these functions could be called
      DLLLOCAL virtual void drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 qobj->drawComplexControl(control, option, painter, widget);	
      }
      DLLLOCAL virtual void drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 qobj->drawControl(element, option, painter, widget);
      }
      DLLLOCAL virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      { 
	 qobj->drawItemPixmap(painter, rectangle, alignment, pixmap);	
      }									
      DLLLOCAL virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const 
      { 
	 qobj->drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole); 
      }									
      DLLLOCAL virtual void drawPrimitive ( QStyle::PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const 
      { 
	 qobj->drawPrimitive(element, option, painter, widget); 
      }									
      DLLLOCAL virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const
      {
	 return qobj->generatedIconPixmap(iconMode, pixmap, option); 
      }									
      DLLLOCAL virtual QStyle::SubControl hitTestComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const 
      { 
	 return qobj->hitTestComplexControl(control, option, position, widget); 
      }
      DLLLOCAL virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const 
      {
	 return qobj->itemPixmapRect(rectangle, alignment, pixmap); 
      }									
      DLLLOCAL virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const 
      { 
	 return qobj->itemTextRect(metrics, rectangle, alignment, enabled, text); 
      }									
      DLLLOCAL virtual int pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const 
      {
	 return qobj->pixelMetric(metric, option, widget); 
      }					
      DLLLOCAL virtual void polish ( QWidget * widget ) 
      {
	 qobj->polish(widget); 
      }									
      DLLLOCAL virtual void polish ( QApplication * application ) 
      {	
	 qobj->polish(application); 
      }									
      DLLLOCAL virtual void polish ( QPalette & palette ) 
      {
	 qobj->polish(palette); 
      } 
      DLLLOCAL virtual QSize sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const 
      {
	 return qobj->sizeFromContents(type, option, contentsSize, widget); 
      }									
      DLLLOCAL virtual QPalette standardPalette () const 
      {			
	 return qobj->standardPalette(); 
      }
      DLLLOCAL virtual QPixmap standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *option = 0, const QWidget *widget = 0) const 
      {
	 return qobj->standardPixmap(standardPixmap, option, widget);
      }
      DLLLOCAL virtual int styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const 
      {
	 return qobj->styleHint(hint, option, widget, returnData); 
      }									
      DLLLOCAL virtual QRect subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const 
      {
	 return qobj->subControlRect(control, option, subControl, widget); 
      }									
      DLLLOCAL virtual QRect subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const 
      {
	 return qobj->subElementRect(element, option, widget); 
      }									
      DLLLOCAL virtual void unpolish ( QWidget * widget ) 
      {		
	 qobj->unpolish(widget); 
      }									
      DLLLOCAL virtual void unpolish ( QApplication * application ) 
      {
	 qobj->unpolish(application); 
      }


#if 0
};
#endif
