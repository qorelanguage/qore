/*
  qt-util.cc
  
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

#include "QC_QAbstractButton.h"
//#include "QC_QAccessibleEvent.h"
#include "QC_QAction.h"
#include "QC_QActionEvent.h"
#include "QC_QBitmap.h"
#include "QC_QBrush.h"
#include "QC_QByteArray.h"
#include "QC_QCDEStyle.h"
#include "QC_QChar.h"
#include "QC_QChildEvent.h"
#include "QC_QCleanlooksStyle.h"
#include "QC_QCloseEvent.h"
#include "QC_QColor.h"
#include "QC_QContextMenuEvent.h"
#include "QC_QDate.h"
#include "QC_QDateTime.h"
#include "QC_QDragEnterEvent.h"
#include "QC_QDragLeaveEvent.h"
#include "QC_QDragMoveEvent.h"
#include "QC_QDropEvent.h"
#include "QC_QEvent.h"
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
#include "QC_QLine.h"
#include "QC_QLineF.h"
#include "QC_QLocale.h"
#ifdef DARWIN
#include "QC_QMacStyle.h"
#endif
#include "QC_QMatrix.h"
#include "QC_QMotifStyle.h"
#include "QC_QMouseEvent.h"
#include "QC_QMoveEvent.h"
#include "QC_QObject.h"
#include "QC_QPaintEvent.h"
#include "QC_QPalette.h"
#include "QC_QPen.h"
#include "QC_QPixmap.h"
#include "QC_QPlastiqueStyle.h"
#include "QC_QPoint.h"
//#include "QC_QPointArray.h"
#include "QC_QPointF.h"
#include "QC_QPolygon.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QRegExp.h"
#include "QC_QRegion.h"
#include "QC_QResizeEvent.h"
#include "QC_QShowEvent.h"
#include "QC_QSize.h"
//#include "QC_QSizeF.h"
//#include "QC_QSizePolicy.h"
//#include "QC_QStringList.h"
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
#include "QC_QTime.h"
#include "QC_QTimerEvent.h"
#include "QC_QUrl.h"
#include "QC_QVariant.h"
#include "QC_QWheelEvent.h"
#include "QC_QWidget.h"
#include "QC_QWindowsStyle.h"
#ifdef WINDOWS
#include "QC_QWindowsXPStyle.h"
#endif

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>

#include <assert.h>

AbstractQoreNode *C_Clipboard = 0;

int get_qdate(const AbstractQoreNode *n, QDate &date, class ExceptionSink *xsink)
{
   {
      const DateTimeNode *d = dynamic_cast<const DateTimeNode *>(n);
      if (d) {
	 date.setDate(d->getYear(), d->getMonth(), d->getDay());
	 return 0;
      }
   }
   
   const QoreObject *o = dynamic_cast<const QoreObject *>(n);
   QoreQDate *qd = o ? (QoreQDate *)o->getReferencedPrivateData(CID_QDATE, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qd) {
      QoreQDateTime *qdt = o ? (QoreQDateTime *)o->getReferencedPrivateData(CID_QDATETIME, xsink) : 0;
      if (!qdt) {
	 if (!*xsink) {
	    if (o) 
	       xsink->raiseException("DATE-ERROR", "class '%s' is not derived from QDate or QDateTime", o->getClass()->getName());
	    else
	       xsink->raiseException("DATE-ERROR", "cannot convert type '%s' to QDate", n ? n->getTypeName() : "NOTHING");
	 }
	 return -1;
      }

      ReferenceHolder<QoreQDateTime> dtHolder(qdt, xsink);
      date = qdt->date();
      return 0;
   }

   ReferenceHolder<QoreQDate> dtHolder(qd, xsink);
   date.setDate(qd->year(), qd->month(), qd->day());
   return 0;
}

int get_qdatetime(const AbstractQoreNode *n, QDateTime &dt, class ExceptionSink *xsink)
{
   if (n) {
      {
	 const DateTimeNode *qdt = dynamic_cast<const DateTimeNode *>(n);
	 if (qdt) {
	    dt.setDate(QDate(qdt->getYear(), qdt->getMonth(), qdt->getDay()));
	    dt.setTime(QTime(qdt->getHour(), qdt->getMinute(), qdt->getSecond(), qdt->getMillisecond()));
	    return 0;
	 }
      }
   
      const QoreObject *o = dynamic_cast<const QoreObject *>(n);

      if (o) {
	 QoreQDate *qd = (QoreQDate *)o->getReferencedPrivateData(CID_QDATE, xsink);
	 if (*xsink)
	    return -1;
	 if (!qd) {
	    class QoreQDateTime *qdt = (QoreQDateTime *)o->getReferencedPrivateData(CID_QDATETIME, xsink);
	    if (*xsink)
	       return -1;
	    if (!qdt) {
	       class QoreQTime *qt = (QoreQTime *)o->getReferencedPrivateData(CID_QTIME, xsink);
	       if (*xsink)
		  return -1;
	       if (!qt)
		  xsink->raiseException("DATETIME-ERROR", "class '%s' is not derived from QDate, QTime, or QDateTime", o->getClass()->getName());
	       ReferenceHolder<QoreQTime> tHolder(qt, xsink);
	       dt.setDate(QDate());
	       dt.setTime(*(static_cast<QTime *>(qt)));
	       return 0;	       
	    }
	    ReferenceHolder<QoreQDateTime> dtHolder(qdt, xsink);
	    dt = *(static_cast<QDateTime *>(qdt));
	    return 0;
	 }
	 ReferenceHolder<QoreQDate> dHolder(qd, xsink);
	 dt.setTime(QTime());
	 dt.setDate(*(static_cast<QDate *>(qd)));
	 return 0;
      }
   }

   xsink->raiseException("QDATETIME-ERROR", "cannot derive QDateTime value from type '%s'", n ? n->getTypeName() : "NOTHING");
   return -1;
}

int get_qtime(const AbstractQoreNode *n, QTime &time, class ExceptionSink *xsink)
{
   {
      const DateTimeNode *qdt = dynamic_cast<const DateTimeNode *>(n);
      if (qdt) {
	 time.setHMS(qdt->getHour(), qdt->getMinute(), qdt->getSecond(), qdt->getMillisecond());
	 return 0;
      }
   }
   
   const QoreObject *o = dynamic_cast<const QoreObject *>(n);
   class QoreQTime *qt = o ? (QoreQTime *)o->getReferencedPrivateData(CID_QTIME, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qt) {
      class QoreQDateTime *qdt = o ? (QoreQDateTime *)o->getReferencedPrivateData(CID_QDATETIME, xsink) : 0;
      if (!qdt) {
	 if (!*xsink) {
	    if (n && n->getType() == NT_OBJECT) 
	       xsink->raiseException("QTIME-ERROR", "class '%s' is not derived from QTime or QDateTime", o->getClass()->getName());
	    else
	       xsink->raiseException("QTIME-ERROR", "cannot convert type '%s' to QTime", n ? n->getTypeName() : "NOTHING");
	 }
	 return -1;
      }

      ReferenceHolder<QoreQDateTime> dtHolder(qdt, xsink);
      time = qdt->time();
      return 0;
   }

   ReferenceHolder<QoreQTime> timeHolder(qt, xsink);
   time = *qt;
   return 0;
}

int get_qbytearray(const AbstractQoreNode *n, QByteArray &ba, class ExceptionSink *xsink, bool suppress_exception)
{
   qore_type_t ntype = n ? n->getType() : 0;

   if (ntype == NT_OBJECT) {
      const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
      QoreQByteArray *qba = (QoreQByteArray *)o->getReferencedPrivateData(CID_QBYTEARRAY, xsink);
      if (*xsink)
	 return 0;
      if (!qba) {
	 if (!suppress_exception)
	    xsink->raiseException("QBYTEARRAY-ERROR", "class '%s' is not derived from QByteArray", o->getClass()->getName());
	 return -1;
      }
      ReferenceHolder<QoreQByteArray> qbaHolder(qba, xsink);
      ba = *qba;
      return 0;
   }

   if (ntype == NT_BINARY) {
      const BinaryNode *b = reinterpret_cast<const BinaryNode *>(n);
      QByteArray nba((const char *)b->getPtr(), b->size());
      ba = nba;
      return 0;
   }

   if (ntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(n);
      ba.clear();
      ba.append(str->getBuffer());
      return 0;
   }

   if (!suppress_exception)
      xsink->raiseException("QBYTEARRAY-ERROR", "cannot convert type '%s' to QByteArray", n ? n->getTypeName() : "NOTHING");

   return -1;
}

int get_qvariant(const AbstractQoreNode *n, QVariant &qva, class ExceptionSink *xsink, bool suppress_exception)
{
   //printd(5, "get_variant() n=%08p %s\n", n, n ? n->getTypeName() : "n/a");
   if (n) {
      qore_type_t ntype = n->getType();

      if (ntype == NT_OBJECT) {
	 const QoreObject *o = reinterpret_cast<const QoreObject *>(n);
	 QoreQVariant *qv = (QoreQVariant *)o->getReferencedPrivateData(CID_QVARIANT, xsink);
	 if (*xsink)
	    return -1;
	 if (qv) {
	    ReferenceHolder<QoreQVariant> qvHolder(qv, xsink);
	    qva = *qv;
	    return 0;
	 }
	 QoreQLocale *qlocale = (QoreQLocale *)o->getReferencedPrivateData(CID_QLOCALE, xsink);
	 if (*xsink)
	    return -1;
	 if (qlocale) {
	    ReferenceHolder<QoreQLocale> qlocaleHolder(qlocale, xsink);
	    qva = *qlocale;
	    return 0;
	 }
	 if (!suppress_exception)
	    xsink->raiseException("QVARIANT-ERROR", "cannot convert class '%s' to QVariant", o->getClass()->getName());
	 return -1;
      }

      if (ntype == NT_STRING) {
	 const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(n);
	 QVariant n_qv(str->getBuffer());
	 qva = n_qv;
	 return 0;
      }

      if (ntype == NT_INT) {
	 const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(n);
	 if (b->val <= 0x7fffffff)
	    qva.setValue((int)b->val);
	 else
	    qva.setValue(b->val);
	 //printd(5, "qvariant integer %d (%d)\n", (int)b->val, qva.toInt());
	 return 0;
      }

      if (ntype == NT_FLOAT) {
	 qva.setValue(reinterpret_cast<const QoreFloatNode *>(n)->f);
	 return 0;
      }
   }

   if (!suppress_exception)
      xsink->raiseException("QVARIANT-ERROR", "cannot convert type '%s' to QVariant", n ? n->getTypeName() : "NOTHING");
   return -1;
}

int get_qchar(const AbstractQoreNode *n, QChar &c, class ExceptionSink *xsink, bool suppress_exception)
{
   qore_type_t ntype = n ? n->getType() : 0;
   
   if (ntype == NT_STRING) {
      const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(n);
      unsigned int unicode = str->getUnicodePoint(0, xsink);
      if (*xsink)
	 return -1;
      QChar tmp(unicode);
      c = tmp;
      return 0;
   }

   const QoreObject *o = ntype == NT_OBJECT ? reinterpret_cast<const QoreObject *>(n) : 0;
   QoreQChar *qc = o ? (QoreQChar *)o->getReferencedPrivateData(CID_QCHAR, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qc) {
      if (!suppress_exception) {
	 if (o) 
	    xsink->raiseException("QCHAR-ERROR", "class '%s' is not derived from QChar", o->getClass()->getName());
	 else
	    xsink->raiseException("QCHAR-ERROR", "cannot convert type '%s' to QChar", n ? n->getTypeName() : "NOTHING");
      }
      return -1;
   }

   ReferenceHolder<QoreQChar> cHolder(qc, xsink);
   c = *qc;
   return 0;
}

int get_qstring(const AbstractQoreNode *n, QString &str, class ExceptionSink *xsink, bool suppress_exception)
{
   qore_type_t ntype = n ? n->getType() : 0;

   if (ntype == NT_STRING) {
      const QoreStringNode *pstr = reinterpret_cast<const QoreStringNode *>(n);
      if (pstr->getEncoding() == QCS_ISO_8859_1) {
	 str = QString::fromLatin1(pstr->getBuffer());
      }
      else if (pstr->getEncoding() == QCS_USASCII) {
	 str = QString::fromAscii(pstr->getBuffer());
      }
      else {
	 TempEncodingHelper estr(pstr, QCS_UTF8, xsink);
	 if (*xsink)
	    return -1;
	 
	 str = QString::fromUtf8(estr->getBuffer());
      }
      return 0;
   }

   if (!suppress_exception) {
      if (ntype == NT_INT) {
	 str.setNum(reinterpret_cast<const QoreBigIntNode *>(n)->val);
	 return 0;
      }
      if (ntype == NT_FLOAT) {
	 str.setNum(reinterpret_cast<const QoreFloatNode *>(n)->f);
	 return 0;
      }
   }

   const QoreObject *o = ntype == NT_OBJECT ? reinterpret_cast<const QoreObject *>(n) : 0;
   class QoreQChar *qc = o ? (QoreQChar *)o->getReferencedPrivateData(CID_QCHAR, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qc) {
      class QoreQVariant *qv = o ? (QoreQVariant *)o->getReferencedPrivateData(CID_QVARIANT, xsink) : 0;
      if (*xsink)
	 return -1;
      if (!qv) {
	 if (!suppress_exception) {
	    if (o)
	       xsink->raiseException("QSTRING-ERROR", "class '%s' is not derived from QChar or QVariant", o->getClass()->getName());
	    else
	       xsink->raiseException("QSTRING-ERROR", "cannot convert type '%s' to QString", n ? n->getTypeName() : "NOTHING");
	 }
	 return -1;
      }
      if (qv->type() != QVariant::String) {
	 if (!suppress_exception)
	    xsink->raiseException("QSTRING-ERROR", "QVariant passed as QString argument holds type '%s'", qv->typeName());
	 return -1;
      }
      str = qv->toString();

      return 0;
   }

   ReferenceHolder<QoreQChar> cHolder(qc, xsink);
   str = *qc;
   return 0;
}

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

QoreObject *return_object(const QoreClass *qclass, AbstractPrivateData *data)
{
   QoreObject *qore_object = new QoreObject(qclass, getProgram());
   qore_object->setPrivate(qclass->getID(), data);
   return qore_object;
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

class AbstractQoreNode *return_qvariant(const QVariant &qv)
{
   QVariant::Type type = qv.type();
   switch (type) {
      case QVariant::Invalid:
	 return nothing();
      case QVariant::Bitmap:
	 return return_object(QC_QBitmap, new QoreQBitmap(qv.value<QBitmap>()));
      case QVariant::Bool:
	 return get_bool_node(qv.toBool());
      case QVariant::Brush:
	 return return_object(QC_QBrush, new QoreQBrush(qv.value<QBrush>()));
      case QVariant::Color:
	 return return_object(QC_QColor, new QoreQColor(qv.value<QColor>()));
      case QVariant::Date: 
	 return new DateTimeNode(qv.toDate().year(), qv.toDate().month(), qv.toDate().day());
      case QVariant::DateTime: {
	 QDate rv_d = qv.toDateTime().date();
	 QTime rv_t = qv.toDateTime().time();
	 return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
      }
      case QVariant::Double:
	 return new QoreFloatNode(qv.toDouble());
      case QVariant::Font:
	 return return_object(QC_QFont, new QoreQFont(qv.value<QFont>()));
      case QVariant::Icon:
	 return return_object(QC_QIcon, new QoreQIcon(qv.value<QIcon>()));
      case QVariant::Image:
	 return return_object(QC_QImage, new QoreQImage(qv.value<QImage>()));
      case QVariant::Int:
	 return new QoreBigIntNode(qv.toInt());
      case QVariant::KeySequence:
         return return_object(QC_QKeySequence, new QoreQKeySequence(qv.value<QKeySequence>()));
      case QVariant::Line:
         return return_object(QC_QLine, new QoreQLine(qv.value<QLine>()));
      case QVariant::LineF:
         return return_object(QC_QLineF, new QoreQLineF(qv.value<QLineF>()));
      //case QVariant::List:
      case QVariant::Locale:
	 return return_object(QC_QLocale, new QoreQLocale(qv.value<QLocale>()));
      case QVariant::LongLong:
	 return new QoreBigIntNode(qv.toLongLong());
      //case QVariant::Map:
      case QVariant::Matrix:
	 return return_object(QC_QMatrix, new QoreQMatrix(qv.value<QMatrix>()));
      case QVariant::Palette:
         return return_object(QC_QPalette, new QoreQPalette(qv.value<QPalette>()));
      case QVariant::Pen:
         return return_object(QC_QPen, new QoreQPen(qv.value<QPen>()));
      case QVariant::Pixmap:
         return return_object(QC_QPixmap, new QoreQPixmap(qv.value<QPixmap>()));
      case QVariant::Point:
         return return_object(QC_QPoint, new QoreQPoint(qv.value<QPoint>()));
      //case QVariant::PointArray:
         //return return_object(QC_QPointArray, new QoreQPointArray(qv.value<QPointArray>()));
      case QVariant::PointF:
         return return_object(QC_QPointF, new QoreQPointF(qv.value<QPointF>()));
      case QVariant::Polygon:
         return return_object(QC_QPolygon, new QoreQPolygon(qv.value<QPolygon>()));
      case QVariant::Rect:
         return return_object(QC_QRect, new QoreQRect(qv.value<QRect>()));
      case QVariant::RectF:
         return return_object(QC_QRectF, new QoreQRectF(qv.value<QRectF>()));
      case QVariant::RegExp:
         return return_object(QC_QRegExp, new QoreQRegExp(qv.value<QRegExp>()));
      case QVariant::Region:
         return return_object(QC_QRegion, new QoreQRegion(qv.value<QRegion>()));
      case QVariant::Size:
         return return_object(QC_QSize, new QoreQSize(qv.value<QSize>()));
      //case QVariant::SizeF:
         //return return_object(QC_QSizeF, new QoreQSizeF(qv.value<QSizeF>()));
      //case QVariant::SizePolicy:
         //return return_object(QC_QSizePolicy, new QoreQSizePolicy(qv.value<QSizePolicy>()));
      case QVariant::String:
	 return new QoreStringNode(qv.toString().toUtf8().data(), QCS_UTF8);
      //case QVariant::StringList:
         //return return_object(QC_QStringList, new QoreQStringList(qv.value<QStringList>()));
      case QVariant::TextFormat:
         return return_object(QC_QTextFormat, new QoreQTextFormat(qv.value<QTextFormat>()));
      case QVariant::TextLength:
         return return_object(QC_QTextLength, new QoreQTextLength(qv.value<QTextLength>()));
      case QVariant::Time:
	 return new DateTimeNode(1970, 1, 1, qv.toTime().hour(), qv.toTime().minute(), qv.toTime().second(), qv.toTime().msec());
      //case QVariant::Transform:
         //return return_object(QC_QVariant::Transform, new QTransform(qv.value<QVariant::Transform>()));
      case QVariant::UInt:
	 return new QoreBigIntNode(qv.toUInt());
      case QVariant::ULongLong: // WARNING: precision lost here
	 return new QoreBigIntNode((int64)qv.toULongLong());
      case QVariant::Url:
         return return_object(QC_QUrl, new QoreQUrl(qv.value<QUrl>()));
      //case QVariant::UserType:

      default:
	 return return_object(QC_QVariant, new QoreQVariant(qv));
   }
   // to suppress warning
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
QoreObject *return_qobject(QObject *o)
{
   if (!o)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = o->property("qobject");
   QoreObject *qo = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return qo;
   }

   // see what subclass it is
   QWidget *qw = dynamic_cast<QWidget *>(o);
   if (qw)
      return return_qwidget_intern(qw);

   // assign as QObject
   qo = new QoreObject(QC_QObject, getProgram());
   qo->setPrivate(CID_QOBJECT, new QoreQtQObject(qo, o));
   return qo;
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

QoreObject *return_qevent(QEvent *event)
{
   if (!event)
      return 0;

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
      QChildEvent *qce = dynamic_cast<QChildEvent *>(event);
      if (qce)
         return return_object(QC_QChildEvent, new QoreQChildEvent(*qce));
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
      QTimerEvent *qte = dynamic_cast<QTimerEvent *>(event);
      if (qte)
         return return_object(QC_QTimerEvent, new QoreQTimerEvent(*qte));
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

   return return_object(QC_QEvent, new QoreQEvent(*event));
}

QoreListNode *return_qstringlist(const QStringList &l)
{
   QoreListNode *ql = new QoreListNode();
   for (QStringList::const_iterator i = l.begin(), e = l.end(); i != e; ++i)
      ql->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return ql;
}

