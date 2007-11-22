/*
 QoreAbstractQStyle.h
 
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

#ifndef _QORE_QT_QOREABSTRACTQSTYLE_H

#define _QORE_QT_QOREABSTRACTQSTYLE_H

#include <qore/LVarInstantiatorHelper.h>

#include "QoreAbstractQObject.h"
#include "QoreQtEventDispatcher.h"

#include "QC_QStyleOptionComplex.h"
#include "QC_QPalette.h"

#include <QStyle>

class QoreAbstractQStyle : public QoreAbstractQObject
{
   public:
      DLLLOCAL virtual class QStyle *getQStyle() const = 0;

      DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0;

      DLLLOCAL virtual void drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual void drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const = 0;
      DLLLOCAL virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const = 0;
      DLLLOCAL virtual void drawPrimitive ( QStyle::PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const = 0;
      DLLLOCAL virtual QStyle::SubControl hitTestComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const = 0;
      DLLLOCAL virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const = 0;
      DLLLOCAL virtual int pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual void polish ( QWidget * widget ) = 0;
      DLLLOCAL virtual void polish ( QApplication * application ) = 0;
      DLLLOCAL virtual void polish ( QPalette & palette ) = 0;
      DLLLOCAL virtual QSize sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QPalette standardPalette () const = 0;
      DLLLOCAL virtual QPixmap standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *option = 0, const QWidget *widget = 0) const = 0;
      DLLLOCAL virtual int styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const = 0;
      DLLLOCAL virtual QRect subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual QRect subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const = 0;
      DLLLOCAL virtual void unpolish ( QWidget * widget ) = 0;
      DLLLOCAL virtual void unpolish ( QApplication * application ) = 0;
};

class QoreQStyleExtension : public QoreQObjectExtension
{
   protected:
      Method *m_drawComplexControl, *m_drawControl, *m_drawItemPixmap, *m_drawItemText, *m_drawPrimitive, 
	 *m_generatedIconPixmap, *m_hitTestComplexControl, *m_itemPixmapRect, *m_itemTextRect, *m_pixelMetric,
	 *m_polish, *m_sizeFromContents, *m_standardPalette, *m_standardPixmap, *m_styleHint, 
	 *m_subControlRect, *m_subElementRect, *m_unpolish;

   public:
      DLLLOCAL QoreQStyleExtension(QoreClass *qc) : QoreQObjectExtension(qc)
      {
	 m_drawComplexControl      = findMethod(qc, "drawComplexControl");
         m_drawControl             = findMethod(qc, "drawControl");
         m_drawItemPixmap          = findMethod(qc, "drawItemPixmap");
         m_drawItemText            = findMethod(qc, "drawItemText");
         m_drawPrimitive           = findMethod(qc, "drawPrimitive");
         m_generatedIconPixmap     = findMethod(qc, "generatedIconPixmap");
         m_hitTestComplexControl   = findMethod(qc, "hitTestComplexControl");
         m_itemPixmapRect          = findMethod(qc, "itemPixmapRect");
         m_itemTextRect            = findMethod(qc, "itemTextRect");
         m_pixelMetric             = findMethod(qc, "pixelMetric");
         m_polish                  = findMethod(qc, "polish");
         m_sizeFromContents        = findMethod(qc, "sizeFromContents");
         m_standardPalette         = findMethod(qc, "standardPalette");
         m_standardPixmap          = findMethod(qc, "standardPixmap");
         m_styleHint               = findMethod(qc, "styleHint");
         m_subControlRect          = findMethod(qc, "subControlRect");
         m_subElementRect          = findMethod(qc, "subElementRect");
         m_unpolish                = findMethod(qc, "unpolish");
      }
};

#define QORE_VIRTUAL_QSTYLE_METHODS QORE_VIRTUAL_QOBJECT_METHODS	\
   DLLLOCAL virtual QStyle *getQStyle() const { return qobj; }		\
   DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { \
      return qobj->layoutSpacingImplementation(control1, control2, orientation, option, widget); \
   }									\
   DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { \
      return qobj->standardIconImplementation(standardIcon, option, widget); \
   }									\
   DLLLOCAL virtual void drawComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const { \
      qobj->parent_drawComplexControl(control, option, painter, widget);	\
   }									\
   DLLLOCAL virtual void drawControl ( QStyle::ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const { \
      qobj->parent_drawControl(element, option, painter, widget);		\
   }									\
   DLLLOCAL virtual void drawItemPixmap ( QPainter * painter, const QRect & rectangle, int alignment, const QPixmap & pixmap ) const { \
      qobj->parent_drawItemPixmap(painter, rectangle, alignment, pixmap);	\
   }									\
   DLLLOCAL virtual void drawItemText ( QPainter * painter, const QRect & rectangle, int alignment, const QPalette & palette, bool enabled, const QString & text, QPalette::ColorRole textRole = QPalette::NoRole ) const { \
      qobj->parent_drawItemText(painter, rectangle, alignment, palette, enabled, text, textRole); \
   }									\
   DLLLOCAL virtual void drawPrimitive ( QStyle::PrimitiveElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0 ) const { \
      qobj->parent_drawPrimitive(element, option, painter, widget); \
   }									\
   DLLLOCAL virtual QPixmap generatedIconPixmap ( QIcon::Mode iconMode, const QPixmap & pixmap, const QStyleOption * option ) const {\
      return qobj->parent_generatedIconPixmap(iconMode, pixmap, option); \
   }									\
   DLLLOCAL virtual QStyle::SubControl hitTestComplexControl ( QStyle::ComplexControl control, const QStyleOptionComplex * option, const QPoint & position, const QWidget * widget = 0 ) const { \
      return qobj->parent_hitTestComplexControl(control, option, position, widget); \
   }									\
   DLLLOCAL virtual QRect itemPixmapRect ( const QRect & rectangle, int alignment, const QPixmap & pixmap ) const {\
      return qobj->parent_itemPixmapRect(rectangle, alignment, pixmap); \
   }									\
   DLLLOCAL virtual QRect itemTextRect ( const QFontMetrics & metrics, const QRect & rectangle, int alignment, bool enabled, const QString & text ) const { \
      return qobj->parent_itemTextRect(metrics, rectangle, alignment, enabled, text); \
   }									\
   DLLLOCAL virtual int pixelMetric ( QStyle::PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const {\
      return qobj->parent_pixelMetric(metric, option, widget); \
   }									\
   DLLLOCAL virtual void polish ( QWidget * widget ) {\
      qobj->parent_polish(widget); \
   }									\
   DLLLOCAL virtual void polish ( QApplication * application ) {	\
      qobj->parent_polish(application); \
   }									\
   DLLLOCAL virtual void polish ( QPalette & palette ) {\
      qobj->parent_polish(palette); \
   } \
   DLLLOCAL virtual QSize sizeFromContents ( QStyle::ContentsType type, const QStyleOption * option, const QSize & contentsSize, const QWidget * widget = 0 ) const {\
      return qobj->parent_sizeFromContents(type, option, contentsSize, widget); \
   }									\
   DLLLOCAL virtual QPalette standardPalette () const {			\
      return qobj->parent_standardPalette(); \
   }									\
   DLLLOCAL virtual QPixmap standardPixmap(QStyle::StandardPixmap standardPixmap, const QStyleOption *option = 0, const QWidget *widget = 0) const { \
      return qobj->parent_standardPixmap(standardPixmap, option, widget);	\
   }									\
   DLLLOCAL virtual int styleHint ( QStyle::StyleHint hint, const QStyleOption * option = 0, const QWidget * widget = 0, QStyleHintReturn * returnData = 0 ) const {\
      return qobj->parent_styleHint(hint, option, widget, returnData); \
   }									\
   DLLLOCAL virtual QRect subControlRect ( QStyle::ComplexControl control, const QStyleOptionComplex * option, QStyle::SubControl subControl, const QWidget * widget = 0 ) const {\
      return qobj->parent_subControlRect(control, option, subControl, widget); \
   }									\
   DLLLOCAL virtual QRect subElementRect ( QStyle::SubElement element, const QStyleOption * option, const QWidget * widget = 0 ) const {\
      return qobj->parent_subElementRect(element, option, widget); \
   }									\
   DLLLOCAL virtual void unpolish ( QWidget * widget ) {		\
      qobj->parent_unpolish(widget); \
   }									\
   DLLLOCAL virtual void unpolish ( QApplication * application ) {\
      qobj->parent_unpolish(application); \
   }


#endif  // _QORE_QT_QOREABSTRACTQSTYLE_H
