/*
  qt-gui-util.cc
  
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

#include "QC_QAbstractButton.h"
//#include "QC_QAccessibleEvent.h"
#include "QC_QAction.h"
#include "QC_QActionEvent.h"
#include "QC_QBitmap.h"
#include "QC_QBrush.h"
#include "QC_QCDEStyle.h"
#include "QC_QCleanlooksStyle.h"
#include "QC_QCloseEvent.h"
#include "QC_QColor.h"
#include "QC_QContextMenuEvent.h"
#include "QC_QDragEnterEvent.h"
#include "QC_QDragLeaveEvent.h"
#include "QC_QDragMoveEvent.h"
#include "QC_QDropEvent.h"
#include "QC_QFocusEvent.h"
#include "QC_QFont.h"
#include "QC_QHelpEvent.h"
#include "QC_QHideEvent.h"
#include "QC_QIcon.h"
#include "QC_QImage.h"
#include "QC_QInputEvent.h"
#include "QC_QInputMethodEvent.h"
#include "QC_QKeyEvent.h"
#include "QC_QKeySequence.h"
#ifdef DARWIN
#include "QC_QMacStyle.h"
#endif
#include "QC_QMatrix.h"
#include "QC_QMotifStyle.h"
#include "QC_QMouseEvent.h"
#include "QC_QMoveEvent.h"
#include "QC_QPaintEvent.h"
#include "QC_QPalette.h"
#include "QC_QPen.h"
#include "QC_QPixmap.h"
#include "QC_QPlastiqueStyle.h"
#include "QC_QPolygon.h"
#include "QC_QRegion.h"
#include "QC_QResizeEvent.h"
#include "QC_QShowEvent.h"
//#include "QC_QSizePolicy.h"
#include "QC_QStyle.h"
#include "QC_QStyleOption.h"
#include "QC_QStyleOptionButton.h"
#include "QC_QStyleOptionComboBox.h"
#include "QC_QStyleOptionComplex.h"
#include "QC_QStyleOptionGroupBox.h"
#include "QC_QStyleOptionMenuItem.h"
#include "QC_QStyleOptionSizeGrip.h"
#include "QC_QStyleOptionSlider.h"
#include "QC_QStyleOptionSpinBox.h"
#include "QC_QStyleOptionTab.h"
#include "QC_QStyleOptionTabWidgetFrame.h"
#include "QC_QStyleOptionTitleBar.h"
#include "QC_QStyleOptionToolButton.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QStyleOptionViewItemV2.h"
#include "QC_QTabletEvent.h"
#include "QC_QTextFormat.h"
#include "QC_QTextLength.h"
#include "QC_QWheelEvent.h"
#include "QC_QWidget.h"
#include "QC_QWindowsStyle.h"
#ifdef WINDOWS
#include "QC_QWindowsXPStyle.h"
#endif

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>
#include <QVariant>

#include <assert.h>

AbstractQoreNode *C_Clipboard = 0;

int get_qkeysequence(const AbstractQoreNode *n, QKeySequence &ks, class ExceptionSink *xsink, bool suppress_exception)
{
   qore_type_t ntype = n ? n->getType() : 0;

   if (ntype == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
      QoreQKeySequence *qks = (QoreQKeySequence *)o->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink);
      if (*xsink)
	 return 0;
      if (!qks) {
	 if (!suppress_exception)
	    xsink->raiseException("QKEYSEQUENCE-ERROR", "class '%s' is not derived from QKeySequence", o->getClass()->getName());
	 return -1;
      }
      ReferenceHolder<QoreQKeySequence> qksHolder(qks, xsink);
      ks = *qks;
      return 0;
   }

   if (ntype == NT_STRING) {
      QString str;
      get_qstring(n, str, xsink);
      if (*xsink)
	 return -1;

      ks = str;
      return 0;
   }

   if (ntype == NT_INT) {
      QKeySequence::StandardKey key = (QKeySequence::StandardKey)(const_cast<AbstractQoreNode *>(n))->getAsInt();
      ks = key;
      return 0;
   }
   if (!suppress_exception)
      xsink->raiseException("QKEYSEQUENCE-ERROR", "cannot convert type '%s' to QKeySequence", n ? n->getTypeName() : "NOTHING");
   return -1;
}

int get_qbrush(const AbstractQoreNode *n, QBrush &brush, class ExceptionSink *xsink)
{
   //printd(5, "get_qbrush(n=%08p '%s' '%s')\n", n, n ? n->getTypeName() : "n/a", n && n->getType() == NT_OBJECT ? reinterpret_cast<const QoreObject *>(n)->getClassName() : "n/a");
   if (n) {
      qore_type_t ntype = n->getType();
      if (ntype == NT_OBJECT) {
	 const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
	 if (o) {
	    class QoreQBrush *qb = (QoreQBrush *)o->getReferencedPrivateData(CID_QBRUSH, xsink);
	    if (*xsink)
	       return -1;
	    if (!qb) {
	       class QoreQPixmap *pixmap = (QoreQPixmap *)o->getReferencedPrivateData(CID_QPIXMAP, xsink);
	       if (*xsink)
		  return -1;
	       if (!pixmap) {
		  class QoreQImage *image = (QoreQImage *)o->getReferencedPrivateData(CID_QIMAGE, xsink);
		  if (*xsink)
		     return -1;
		  if (!image) {
		     class QoreQColor *color = (QoreQColor *)o->getReferencedPrivateData(CID_QCOLOR, xsink);
		     if (*xsink)
			return -1;
		     if (!color) {
			xsink->raiseException("QBRUSH-ERROR", "class '%s' cannot produce a QBrush object", o->getClass()->getName());
			return -1;
		     }
		     ReferenceHolder<QoreQColor> colorHolder(color, xsink);
		     brush = QBrush(*color);
		     return 0;
		  }
		  ReferenceHolder<QoreQImage> imageHolder(image, xsink);
		  brush = QBrush(*image);
		  return 0;
	       }
	       ReferenceHolder<AbstractPrivateData> pixmapHolder(static_cast<AbstractPrivateData *>(pixmap), xsink);
	       brush = QBrush(*pixmap);
	       return 0;
	    }
	    ReferenceHolder<QoreQBrush> qbHolder(qb, xsink);
	    brush = *(qb->getQBrush());
	    return 0;
	 }
      }
      else if (ntype == NT_BRUSHSTYLE) {
	 const BrushStyleNode *bs = reinterpret_cast<const BrushStyleNode *>(n);
	 brush = QBrush(bs->getStyle());
	 return 0;
      }
      // assume Qt::GlobalColor enum
      else if (ntype == NT_INT) {
	 brush = QBrush((Qt::GlobalColor)(reinterpret_cast<const QoreBigIntNode *>(n)->val));
	 return 0;
      }
   }
   xsink->raiseException("QBRUSH-ERROR", "cannot derive QBrush object from type %s", n ? n->getTypeName() : "NOTHING");
   return -1;
}

QoreObject *return_qstyle(const QString &style, QStyle *qs, ExceptionSink *xsink)
{
   if (!qs) {
      xsink->raiseException("QSTYLEFACTORY-CREATE-ERROR", "unable to create style", style.toUtf8().data());
      return 0;
   }

   QoreClass *qc;
   QoreObject *obj;

   // try to determine what subclass the QStyle is if possible
   QCleanlooksStyle *qcls = dynamic_cast<QCleanlooksStyle *>(qs);
   if (qcls) {
      qc = QC_QCleanlooksStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQCleanlooksStyle(obj, qcls));
      return obj;
   }

   QPlastiqueStyle *qps = dynamic_cast<QPlastiqueStyle *>(qs);
   if (qps) {
      qc = QC_QPlastiqueStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQPlastiqueStyle(obj, qps));
      return obj;
   }

#ifdef WINDOWS
   QWindowsXPStyle *qxps = dynamic_cast<QWindowsXPStyle *>(qs);
   if (qxps) {
      qc = QC_QWindowsXPStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsXPStyle(obj, qxps));
      return obj;
   }
#endif

#ifdef DARWIN
   QMacStyle *qms = dynamic_cast<QMacStyle *>(qs);
   if (qms) {
      qc = QC_QMacStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMacStyle(obj, qms));
      return obj;
   }
#endif

   QWindowsStyle *qws = dynamic_cast<QWindowsStyle *>(qs);
   if (qws) {
      qc = QC_QWindowsStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsStyle(obj, qws));
      return obj;
   }

   QCDEStyle *qcs = dynamic_cast<QCDEStyle *>(qs);
   if (qcs) {
      qc = QC_QCDEStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQCDEStyle(obj, qcs));
      return obj;
   }

   QMotifStyle *qmts = dynamic_cast<QMotifStyle *>(qs);
   if (qmts) {
      qc = QC_QMotifStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMotifStyle(obj, qmts));
      return obj;
   }

   // otherwise return a QStyle object
   obj = new QoreObject(QC_QStyle, getProgram());
   obj->setPrivate(CID_QSTYLE, new QoreQtQStyle(obj, qs));
   return obj;
}

QoreObject *return_qstyleoption(const QStyleOption *qso)
{
   if (!qso)
      return 0;

   const QStyleOptionButton *qsob = qstyleoption_cast<const QStyleOptionButton *>(qso);
   if (qsob)
      return return_object(QC_QStyleOptionButton, new QoreQStyleOptionButton(*qsob));

   const QStyleOptionComboBox *qsocb = qstyleoption_cast<const QStyleOptionComboBox *>(qso);
   if (qsocb)
      return return_object(QC_QStyleOptionComboBox, new QoreQStyleOptionComboBox(*qsocb));

   const QStyleOptionComplex *qsocx = qstyleoption_cast<const QStyleOptionComplex *>(qso);
   if (qsocx)
      return return_object(QC_QStyleOptionComplex, new QoreQStyleOptionComplex(*qsocx));

   const QStyleOptionGroupBox *qsogb = qstyleoption_cast<const QStyleOptionGroupBox *>(qso);
   if (qsogb)
      return return_object(QC_QStyleOptionGroupBox, new QoreQStyleOptionGroupBox(*qsogb));

   const QStyleOptionMenuItem *qsomi = qstyleoption_cast<const QStyleOptionMenuItem *>(qso);
   if (qsomi)
      return return_object(QC_QStyleOptionMenuItem, new QoreQStyleOptionMenuItem(*qsomi));

   const QStyleOptionSizeGrip *qsosg = qstyleoption_cast<const QStyleOptionSizeGrip *>(qso);
   if (qsosg)
      return return_object(QC_QStyleOptionSizeGrip, new QoreQStyleOptionSizeGrip(*qsosg));

   const QStyleOptionSlider *qsoslider = qstyleoption_cast<const QStyleOptionSlider *>(qso);
   if (qsoslider)
      return return_object(QC_QStyleOptionSlider, new QoreQStyleOptionSlider(*qsoslider));

   const QStyleOptionSpinBox *qsospinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(qso);
   if (qsospinbox)
      return return_object(QC_QStyleOptionSpinBox, new QoreQStyleOptionSpinBox(*qsospinbox));

   const QStyleOptionTab *qsotab = qstyleoption_cast<const QStyleOptionTab *>(qso);
   if (qsotab)
      return return_object(QC_QStyleOptionTab, new QoreQStyleOptionTab(*qsotab));

   const QStyleOptionTabWidgetFrame *qsotwf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(qso);
   if (qsotwf)
      return return_object(QC_QStyleOptionTabWidgetFrame, new QoreQStyleOptionTabWidgetFrame(*qsotwf));

   const QStyleOptionTitleBar *qsotitlebar = qstyleoption_cast<const QStyleOptionTitleBar *>(qso);
   if (qsotitlebar)
      return return_object(QC_QStyleOptionTitleBar, new QoreQStyleOptionTitleBar(*qsotitlebar));

   const QStyleOptionToolButton *qsotoolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(qso);
   if (qsotoolbutton)
      return return_object(QC_QStyleOptionToolButton, new QoreQStyleOptionToolButton(*qsotoolbutton));

   const QStyleOptionViewItem *qsoviewitem = qstyleoption_cast<const QStyleOptionViewItem *>(qso);
   if (qsoviewitem)
      return return_object(QC_QStyleOptionViewItem, new QoreQStyleOptionViewItem(*qsoviewitem));

   const QStyleOptionViewItemV2 *qsoviewitemv2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>(qso);
   if (qsoviewitemv2)
      return return_object(QC_QStyleOptionViewItemV2, new QoreQStyleOptionViewItemV2(*qsoviewitemv2));

   return return_object(QC_QStyleOption, new QoreQStyleOption(*qso));
}

AbstractQoreNode *return_gui_qvariant(const QVariant &qv)
{
   QVariant::Type type = qv.type();
   switch (type) {
      case QVariant::Bitmap:
	 return return_object(QC_QBitmap, new QoreQBitmap(qv.value<QBitmap>()));
      case QVariant::Brush:
	 return return_object(QC_QBrush, new QoreQBrush(qv.value<QBrush>()));
      case QVariant::Color:
	 return return_object(QC_QColor, new QoreQColor(qv.value<QColor>()));
      case QVariant::Font:
	 return return_object(QC_QFont, new QoreQFont(qv.value<QFont>()));
      case QVariant::Icon:
	 return return_object(QC_QIcon, new QoreQIcon(qv.value<QIcon>()));
      case QVariant::Image:
	 return return_object(QC_QImage, new QoreQImage(qv.value<QImage>()));
      case QVariant::KeySequence:
         return return_object(QC_QKeySequence, new QoreQKeySequence(qv.value<QKeySequence>()));
      case QVariant::Matrix:
	 return return_object(QC_QMatrix, new QoreQMatrix(qv.value<QMatrix>()));
      case QVariant::Palette:
         return return_object(QC_QPalette, new QoreQPalette(qv.value<QPalette>()));
      case QVariant::Pen:
         return return_object(QC_QPen, new QoreQPen(qv.value<QPen>()));
      case QVariant::Pixmap:
         return return_object(QC_QPixmap, new QoreQPixmap(qv.value<QPixmap>()));
      case QVariant::Polygon:
         return return_object(QC_QPolygon, new QoreQPolygon(qv.value<QPolygon>()));
      case QVariant::Region:
         return return_object(QC_QRegion, new QoreQRegion(qv.value<QRegion>()));
      //case QVariant::SizePolicy:
         //return return_object(QC_QSizePolicy, new QoreQSizePolicy(qv.value<QSizePolicy>()));
      case QVariant::TextFormat:
         return return_object(QC_QTextFormat, new QoreQTextFormat(qv.value<QTextFormat>()));
      case QVariant::TextLength:
         return return_object(QC_QTextLength, new QoreQTextLength(qv.value<QTextLength>()));
      //case QVariant::Transform:
         //return return_object(QC_QTransform, new QTransform(qv.value<QVariant::Transform>()));
      //case QVariant::UserType:

      default:
	 break;
   }
   return 0;
}

// here QWidget subclasses can be determined if necessary
static QoreObject *return_qwidget_intern(QWidget *w, bool managed = true)
{
   // assign as QWidget
   QoreObject *qo = new QoreObject(QC_QWidget, getProgram());
   qo->setPrivate(CID_QWIDGET, new QoreQtQWidget(qo, w, managed));
   return qo;
}

QoreObject *return_qabstractbutton(QAbstractButton *button)
{
   if (!button)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = button->property("qobject");
   QoreObject *qo = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return qo;
   }

   qo = new QoreObject(QC_QAbstractButton, getProgram());
   qo->setPrivate(CID_QABSTRACTBUTTON, new QoreQtQAbstractButton(qo, button));
   return qo;
}

// returns a QoreObject tagged as the appropriate QObject subclass
QoreObject *return_gui_qobject(QObject *o)
{
   // see what subclass it is
   QWidget *qw = dynamic_cast<QWidget *>(o);
   return qw ? return_qwidget_intern(qw) : 0;
}

// returns a QoreObject tagged as the appropriate QWidget subclass
QoreObject *return_qwidget(QWidget *w, bool managed)
{
   if (!w)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = w->property("qobject");
   QoreObject *qo = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return qo;
   }

   return return_qwidget_intern(w, managed);
}

QoreObject *return_qaction(QAction *action)
{
   if (!action)
      return 0;
   QVariant qv_ptr = action->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QAction, getProgram());
      QoreQtQAction *t_qobj = new QoreQtQAction(rv_obj, action);
      rv_obj->setPrivate(CID_QACTION, t_qobj);
   }
   return rv_obj;
}

QoreObject *return_gui_qevent(QEvent *event)
{
   // the order is important here so the most specific subclass is checked before any base classes
/*
   {
      QAccessibleEvent *qae = dynamic_cast<QAccessibleEvent *>(event);
      if (qae)
         return return_object(QC_QAccessibleEvent, new QoreQAccessibleEvent(*qae));
   }
*/
   {
      QActionEvent *qae = dynamic_cast<QActionEvent *>(event);
      if (qae)
         return return_object(QC_QActionEvent, new QoreQActionEvent(*qae));
   }
   {
      QCloseEvent *qce = dynamic_cast<QCloseEvent *>(event);
      if (qce)
         return return_object(QC_QCloseEvent, new QoreQCloseEvent(*qce));
   }
   {
      QContextMenuEvent *qcme = dynamic_cast<QContextMenuEvent *>(event);
      if (qcme)
         return return_object(QC_QContextMenuEvent, new QoreQContextMenuEvent(*qcme));
   }
   {
      QDragEnterEvent *qdee = dynamic_cast<QDragEnterEvent *>(event);
      if (qdee)
         return return_object(QC_QDragEnterEvent, new QoreQDragEnterEvent(*qdee));
   }
   {
      QDragLeaveEvent *qdle = dynamic_cast<QDragLeaveEvent *>(event);
      if (qdle)
         return return_object(QC_QDragLeaveEvent, new QoreQDragLeaveEvent(*qdle));
   }
   {
      QDragMoveEvent *qdme = dynamic_cast<QDragMoveEvent *>(event);
      if (qdme)
         return return_object(QC_QDragMoveEvent, new QoreQDragMoveEvent(*qdme));
   }
   {
      QFocusEvent *qfe = dynamic_cast<QFocusEvent *>(event);
      if (qfe)
         return return_object(QC_QFocusEvent, new QoreQFocusEvent(*qfe));
   }
   {
      QHelpEvent *qhe = dynamic_cast<QHelpEvent *>(event);
      if (qhe)
         return return_object(QC_QHelpEvent, new QoreQHelpEvent(*qhe));
   }
   {
      QHideEvent *qhe = dynamic_cast<QHideEvent *>(event);
      if (qhe)
         return return_object(QC_QHideEvent, new QoreQHideEvent(*qhe));
   }
   {
      QInputMethodEvent *qime = dynamic_cast<QInputMethodEvent *>(event);
      if (qime)
         return return_object(QC_QInputMethodEvent, new QoreQInputMethodEvent(*qime));
   }
   {
      QKeyEvent *qke = dynamic_cast<QKeyEvent *>(event);
      if (qke)
         return return_object(QC_QKeyEvent, new QoreQKeyEvent(*qke));
   }
   {
      QMouseEvent *qme = dynamic_cast<QMouseEvent *>(event);
      if (qme)
         return return_object(QC_QMouseEvent, new QoreQMouseEvent(*qme));
   }
   {
      QMoveEvent *qme = dynamic_cast<QMoveEvent *>(event);
      if (qme)
         return return_object(QC_QMoveEvent, new QoreQMoveEvent(*qme));
   }
   {
      QPaintEvent *qpe = dynamic_cast<QPaintEvent *>(event);
      if (qpe)
         return return_object(QC_QPaintEvent, new QoreQPaintEvent(*qpe));
   }
   {
      QResizeEvent *qre = dynamic_cast<QResizeEvent *>(event);
      if (qre)
         return return_object(QC_QResizeEvent, new QoreQResizeEvent(*qre));
   }
   {
      QShowEvent *qse = dynamic_cast<QShowEvent *>(event);
      if (qse)
         return return_object(QC_QShowEvent, new QoreQShowEvent(*qse));
   }
   {
      QTabletEvent *qte = dynamic_cast<QTabletEvent *>(event);
      if (qte)
         return return_object(QC_QTabletEvent, new QoreQTabletEvent(*qte));
   }
   {
      QWheelEvent *qwe = dynamic_cast<QWheelEvent *>(event);
      if (qwe)
         return return_object(QC_QWheelEvent, new QoreQWheelEvent(*qwe));
   }   
   {
      QInputEvent *qie = dynamic_cast<QInputEvent *>(event);
      if (qie)
         return return_object(QC_QInputEvent, new QoreQInputEvent(*qie));
   }
   {
      QDropEvent *qde = dynamic_cast<QDropEvent *>(event);
      if (qde)
         return return_object(QC_QDropEvent, new QoreQDropEvent(*qde));
   }
   
   return 0;
}
