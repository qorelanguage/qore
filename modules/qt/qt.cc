/*
  qt.cc
  
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

#include "QC_QObject.h"
#include "QC_QApplication.h"
#include "QC_QPushButton.h"
#include "QC_QFont.h"
#include "QC_QMatrix.h"
#include "QC_QWidget.h"
#include "QC_QFrame.h"
#include "QC_QLCDNumber.h"
#include "QC_QLayout.h"
#include "QC_QBoxLayout.h"
#include "QC_QVBoxLayout.h"
#include "QC_QHBoxLayout.h"
#include "QC_QGridLayout.h"
#include "QC_QBrush.h"
#include "QC_QColor.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QPalette.h"
#include "QC_QPaintDevice.h"
#include "QC_QPainter.h"
#include "QC_QPoint.h"
#include "QC_QSize.h"
#include "QC_QRegion.h"
#include "QC_QTimer.h"
#include "QC_QLabel.h"
#include "QC_QAbstractSlider.h"
#include "QC_QSlider.h"
#include "QC_QPicture.h"
#include "QC_QPixmap.h"
#include "QC_QBitmap.h"
#include "QC_QMovie.h"
#include "QC_QShortcut.h"
#include "QC_QImage.h"
#include "QC_QDateTime.h"
#include "QC_QDate.h"
#include "QC_QTime.h"
#include "QC_QIcon.h"
#include "QC_QKeySequence.h"
#include "QC_QAction.h"
#include "QC_QActionGroup.h"
#include "QC_QPointF.h"
#include "QC_QPolygon.h"
#include "QC_QPolygonF.h"
#include "QC_QLine.h"
#include "QC_QLineF.h"
#include "QC_QAbstractButton.h"
#include "QC_QMenu.h"
#include "QC_QToolButton.h"
#include "QC_QDialog.h"
#include "QC_QLineEdit.h"
#include "QC_QTextLength.h"
#include "QC_QTextFormat.h"
#include "QC_QTextBlockFormat.h"
#include "QC_QTextCharFormat.h"
#include "QC_QPen.h"
#include "QC_QTextFrameFormat.h"
#include "QC_QTextTableFormat.h"
#include "QC_QTextListFormat.h"
#include "QC_QTextImageFormat.h"
#include "QC_QCalendarWidget.h"
#include "QC_QStyleOption.h"
#include "QC_QModelIndex.h"
#include "QC_QStyleOptionViewItem.h"
#include "QC_QStyleOptionViewItemV2.h"
#include "QC_QAbstractItemModel.h"
#include "QC_QAbstractItemDelegate.h"
#include "QC_QItemDelegate.h"
#include "QC_QComboBox.h"
#include "QC_QCheckBox.h"
#include "QC_QAbstractSpinBox.h"
#include "QC_QDateTimeEdit.h"
#include "QC_QLocale.h"
#include "QC_QByteArray.h"
#include "QC_QUrl.h"
#include "QC_QVariant.h"
#include "QC_QGroupBox.h"
#include "QC_QDateEdit.h"
#include "QC_QFontMetrics.h"
#include "QC_QFontDatabase.h"
#include "QC_QFontInfo.h"
#include "QC_QScrollBar.h"
#include "QC_QAbstractScrollArea.h"
#include "QC_QScrollArea.h"
#include "QC_QChar.h"
#include "QC_QMimeData.h"
#include "QC_QClipboard.h"
#include "QC_QFontComboBox.h"
#include "QC_QMainWindow.h"
#include "QC_QRadioButton.h"
#include "QC_QStyle.h"
#include "QC_QStyleOptionComplex.h"
#include "QC_QStyleOptionComboBox.h"
#include "QC_QStyleOptionGroupBox.h"
#include "QC_QStyleOptionSizeGrip.h"
#include "QC_QStyleOptionSlider.h"
#include "QC_QStyleOptionSpinBox.h"
#include "QC_QStyleOptionTitleBar.h"
#include "QC_QStyleOptionToolButton.h"
#include "QC_QSpinBox.h"
#include "QC_QAbstractItemView.h"
#include "QC_QTableView.h"
#include "QC_QTableWidget.h"
#include "QC_QTableWidgetItem.h"
#include "QC_QStyleOptionMenuItem.h"
#include "QC_QMessageBox.h"
#include "QC_QStyleOptionButton.h"
#include "QC_QFileDialog.h"
#include "QC_QDir.h"
#include "QC_QHeaderView.h"
#include "QC_QMetaObject.h"
#include "QC_QMenuBar.h"
#include "QC_QPrinter.h"
#include "QC_QPrintDialog.h"
#include "QC_QRegExp.h"
#include "QC_QValidator.h"
#include "QC_QDoubleValidator.h"
#include "QC_QIntValidator.h"
#include "QC_QRegExpValidator.h"
#include "QC_QFileInfo.h"
#include "QC_QColorDialog.h"
#include "QC_QInputDialog.h"
#include "QC_QIODevice.h"
#include "QC_QImageWriter.h"
#include "QC_QDial.h"
#include "QC_QStackedWidget.h"
#include "QC_QDoubleSpinBox.h"
#include "QC_QTimeEdit.h"
#include "QC_QProgressBar.h"
#include "QC_QPainterPath.h"
#include "QC_QPaintEngine.h"
#include "QC_QBasicTimer.h"
#include "QC_QTextEdit.h"
#include "QC_QTabBar.h"
#include "QC_QStyleOptionTab.h"
#include "QC_QStyleOptionTabWidgetFrame.h"
#include "QC_QTabWidget.h"
#include "QC_QEvent.h"
#include "QC_QDesktopWidget.h"
#include "QC_QSystemTrayIcon.h"
#include "QC_QWizard.h"
#include "QC_QWizardPage.h"
#include "QC_QTranslator.h"
#include "QC_QLibraryInfo.h"
#include "QC_QCoreApplication.h"
#include "QC_QListView.h"
#include "QC_QListWidgetItem.h"
#include "QC_QDialogButtonBox.h"
#include "QC_QToolBar.h"
#include "QC_QProgressDialog.h"
#include "QC_QFontDialog.h"
#include "QC_QErrorMessage.h"
#include "QC_QStackedLayout.h"

#include "qore-qt.h"

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>

#include <assert.h>

AbstractQoreNode *C_Clipboard = 0;

static class QoreStringNode *qt_module_init();
static void qt_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns);
static void qt_module_delete();

#ifndef QORE_MONOLITHIC
DLLEXPORT char qore_module_name[] = "qt";
DLLEXPORT char qore_module_version[] = "0.1";
DLLEXPORT char qore_module_description[] = "QT 4 module";
DLLEXPORT char qore_module_author[] = "David Nichols";
DLLEXPORT char qore_module_url[] = "http://www.qoretechnologies.com/qore";
DLLEXPORT int qore_module_api_major = QORE_MODULE_API_MAJOR;
DLLEXPORT int qore_module_api_minor = QORE_MODULE_API_MINOR;
DLLEXPORT qore_module_init_t qore_module_init = qt_module_init;
DLLEXPORT qore_module_ns_init_t qore_module_ns_init = qt_module_ns_init;
DLLEXPORT qore_module_delete_t qore_module_delete = qt_module_delete;
#endif

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
   //printd(5, "get_qbrush(n=%08p '%s' '%s')\n", n, n ? n->getTypeName() : "n/a", n && n->getType() == NT_OBJECT ? n->getClass()->getName() : "n/a");
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
	 if (bs) {
	    brush = QBrush(bs->getStyle());
	    return 0;
	 }
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

QoreObject *return_object(QoreClass *qclass, AbstractPrivateData *data)
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

static class AbstractQoreNode *f_QObject_connect(const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   class AbstractPrivateData *spd = p ? p->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *sender = spd ? dynamic_cast<QoreAbstractQObject *>(spd) : 0;
   assert(!spd || sender);
   if (!sender) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "first argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder1(spd, xsink);

   const QoreStringNode *str = test_string_param(params, 1);
   if (!str)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing signal string as second argument");
      return 0;
   }
   const char *signal = str->getBuffer();

   const QoreObject *o = test_object_param(params, 2);
   if (!o)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing receiving object as third argument");
      return 0;      
   }
   class AbstractPrivateData *rpd = o ? o->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *receiver = rpd ? dynamic_cast<QoreAbstractQObject *>(rpd) : 0;
   assert(!rpd || receiver);
   if (!receiver) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "third argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder2(rpd, xsink);

   // get member/slot name
   str = test_string_param(params, 3);
   if (!str)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing slot as fourth argument");
      return 0;
   }
   const char *member = str->getBuffer();

   /*
   p = get_param(params, 4);
   int conn_type = is_nothing(p) ? Qt::AutoConnection : p->getAsInt();

   bool b = QObject::connect(sender->getQObject(), signal, receiver->getQObject(), member, (enum Qt::ConnectionType)conn_type);
   return get_bool_node(b);
   */
   receiver->connectDynamic(sender, signal, member, xsink);
   return 0;
}

static class AbstractQoreNode *f_SLOT(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("SLOT-ERROR", "missing slot name");
      return 0;
   }
   QoreStringNode *str = new QoreStringNode("1");
   str->concat(p->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return str;
}

static class AbstractQoreNode *f_SIGNAL(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("SIGNAL-ERROR", "missing signal name");
      return 0;
   }
   QoreStringNode *str = new QoreStringNode("2");
   str->concat(p->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return str;
}

static class AbstractQoreNode *f_TR(const QoreListNode *params, class ExceptionSink *xsink)
{
   // get slot name
   const QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreStringNode(QObject::tr(p->getBuffer()).toUtf8().data(), QCS_UTF8);
}

static class AbstractQoreNode *f_QAPP(const QoreListNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}

static class AbstractQoreNode *f_qDebug(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qDebug(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qWarning(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qWarning(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qCritical(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qCritical(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qFatal(const QoreListNode *params, class ExceptionSink *xsink)
{
   QoreStringNodeHolder str(q_sprintf(params, 0, 0, xsink));
   if (*xsink)
      return 0;

   qFatal(str->getBuffer());
   return 0;
}

static class AbstractQoreNode *f_qRound(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   return new QoreBigIntNode(qRound(p ? p->getAsFloat() : 0.0));
}

static class AbstractQoreNode *f_qsrand(const QoreListNode *params, class ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   qsrand(p ? p->getAsInt() : 0);
   return 0;
}

static class AbstractQoreNode *f_qrand(const QoreListNode *params, class ExceptionSink *xsink)
{
   return new QoreBigIntNode(qrand());
}

static AbstractQoreNode *f_QToolTip_font(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(QToolTip::font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return o_qf;
}

//void hideText ()
static AbstractQoreNode *f_QToolTip_hideText(const QoreListNode *params, ExceptionSink *xsink)
{
   QToolTip::hideText();
   return 0;
}

//QPalette palette ()
static AbstractQoreNode *f_QToolTip_palette(const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(QToolTip::palette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return o_qp;
}

//void setFont ( const QFont & font )
static AbstractQoreNode *f_QToolTip_setFont(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQFont *font = p ? (QoreQFont *)p->getReferencedPrivateData(CID_QFONT, xsink) : 0;
   if (!font) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLTIP-SETFONT-PARAM-ERROR", "expecting a QFont object as first argument to QToolTip_setFont()");
      return 0;
   }
   ReferenceHolder<QoreQFont> fontHolder(font, xsink);
   QToolTip::setFont(*(static_cast<QFont *>(font)));
   return 0;
}

//void setPalette ( const QPalette & palette )
static AbstractQoreNode *f_QToolTip_setPalette(const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQPalette *palette = p ? (QoreQPalette *)p->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLTIP-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as first argument to QToolTip_setPalette()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);
   QToolTip::setPalette(*(palette->getQPalette()));
   return 0;
}


//void showText ( const QPoint & pos, const QString & text, QWidget * w, const QRect & rect ) 
//void showText ( const QPoint & pos, const QString & text, QWidget * w = 0 )
static class AbstractQoreNode *f_QToolTip_showText(const QoreListNode *params, class ExceptionSink *xsink)
{
   const QoreObject *p = test_object_param(params, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)p->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() was expecting a QPoint as the first argument");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   const QoreStringNode *str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "expecting a string as second argument to QToolTip_showText()");
      return 0;
   }
   const char *text = str->getBuffer();

   p = test_object_param(params, 2);
   QoreQWidget *w = p ? (QoreQWidget *)p->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (p && !w) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the third argument", p->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQWidget> wHolder(w, xsink);

   QoreQRect *rect = 0;
   if (w) {
      p = test_object_param(params, 3);
      rect = p ? (QoreQRect *)p->getReferencedPrivateData(CID_QRECT, xsink) : 0;
      if (!rect && p) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "this version of QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the fourth argument", p->getClass()->getName());
	 return 0;
      }
   }
   ReferenceHolder<QoreQRect> rectHolder(rect, xsink);

   if (rect)
      QToolTip::showText(*(static_cast<QPoint *>(pos)), text, w ? w->getQWidget() : 0, *(static_cast<QRect *>(rect)));
   else
      QToolTip::showText(*(static_cast<QPoint *>(pos)), text, w ? w->getQWidget() : 0);

   return 0;
}

//QStyle * create ( const QString & key )
AbstractQoreNode *f_QStyleFactory_create(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;

   return return_qstyle(key, QStyleFactory::create(key), xsink);
}

//QStringList keys ()
static AbstractQoreNode *f_QStyleFactory_keys(const QoreListNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QStyleFactory::keys();
   QoreListNode *l = new QoreListNode();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return l;
}

static QoreNamespace *qt_ns = new QoreNamespace("Qt");

static void init_namespace()
{
   qt_ns = new QoreNamespace("Qt");

    // the order is sensitive here as child classes need the parent IDs
   class QoreClass *qobject, *qcoreapplication, *qwidget, *qlayout, *qframe, 
      *qboxlayout, *qpaintdevice, *qpixmap, *qabstractslider;

   qt_ns->addSystemClass((qobject = initQObjectClass()));
   qt_ns->addSystemClass((qcoreapplication = initQCoreApplicationClass(qobject)));
   qt_ns->addSystemClass(initQApplicationClass(qcoreapplication));
   qt_ns->addSystemClass(initQActionClass(qobject));
   qt_ns->addSystemClass(initQActionGroupClass(qobject));
   qt_ns->addSystemClass(initQShortcutClass(qobject));

   qt_ns->addSystemClass((qpaintdevice = initQPaintDeviceClass()));
   qt_ns->addSystemClass(initQPictureClass(qpaintdevice));

   qt_ns->addSystemClass((qpixmap = initQPixmapClass(qpaintdevice)));
   qt_ns->addSystemClass(initQBitmapClass(qpixmap));

   qt_ns->addSystemClass((qwidget = initQWidgetClass(qobject, qpaintdevice)));

   qt_ns->addSystemClass((qabstractslider = initQAbstractSliderClass(qwidget)));

   qt_ns->addSystemClass((qframe = initQFrameClass(qwidget)));
   qt_ns->addSystemClass(initQLCDNumberClass(qframe));
   qt_ns->addSystemClass(initQLabelClass(qframe));

   qt_ns->addSystemClass(initQTimerClass(qobject));

   qt_ns->addSystemClass(initQRectClass());
   qt_ns->addSystemClass(initQRectFClass());
   qt_ns->addSystemClass(initQBrushClass());
   qt_ns->addSystemClass(initQColorClass());
   qt_ns->addSystemClass(initQPointClass());
   qt_ns->addSystemClass(initQSizeClass());

   qt_ns->addSystemClass(initQDateTimeClass());
   qt_ns->addSystemClass(initQDateClass());
   qt_ns->addSystemClass(initQTimeClass());

   qt_ns->addSystemClass(initQKeySequenceClass());
   qt_ns->addSystemClass(initQFontClass());
   qt_ns->addSystemClass(initQMatrixClass());

   qt_ns->addInitialNamespace(initQEventNS());

   QoreNamespace *qimage = new QoreNamespace("QImage");

   // InvertMode enum
   qimage->addConstant("InvertRgb",                new QoreBigIntNode(QImage::InvertRgb));
   qimage->addConstant("InvertRgba",               new QoreBigIntNode(QImage::InvertRgba));

   // Format enum
   qimage->addConstant("Format_Invalid",           new QoreBigIntNode(QImage::Format_Invalid));
   qimage->addConstant("Format_Mono",              new QoreBigIntNode(QImage::Format_Mono));
   qimage->addConstant("Format_MonoLSB",           new QoreBigIntNode(QImage::Format_MonoLSB));
   qimage->addConstant("Format_Indexed8",          new QoreBigIntNode(QImage::Format_Indexed8));
   qimage->addConstant("Format_RGB32",             new QoreBigIntNode(QImage::Format_RGB32));
   qimage->addConstant("Format_ARGB32",            new QoreBigIntNode(QImage::Format_ARGB32));
   qimage->addConstant("Format_ARGB32_Premultiplied", new QoreBigIntNode(QImage::Format_ARGB32_Premultiplied));
   qimage->addConstant("Format_RGB16",             new QoreBigIntNode(QImage::Format_RGB16));

   qimage->addSystemClass(initQImageClass(qpaintdevice));

   qt_ns->addInitialNamespace(qimage);

   QoreNamespace *qregion = new QoreNamespace("QRegion");
   
   // RegionType enum
   qregion->addConstant("Rectangle",                new QoreBigIntNode(QRegion::Rectangle));
   qregion->addConstant("Ellipse",                  new QoreBigIntNode(QRegion::Ellipse));

   qregion->addSystemClass(initQRegionClass());

   qt_ns->addInitialNamespace(qregion);

   QoreNamespace *qlayout_ns = new QoreNamespace("QLayout");

   qlayout_ns->addSystemClass((qlayout = initQLayoutClass(qobject)));
   qlayout_ns->addSystemClass(initQGridLayoutClass(qlayout));

   qlayout_ns->addSystemClass((qboxlayout = initQBoxLayoutClass(qlayout)));
   qlayout_ns->addSystemClass(initQVBoxLayoutClass(qboxlayout));
   qlayout_ns->addSystemClass(initQHBoxLayoutClass(qboxlayout));

   qlayout_ns->addConstant("SetNoConstraint",          new QoreBigIntNode(QLayout::SetNoConstraint));
   qlayout_ns->addConstant("SetMinimumSize",           new QoreBigIntNode(QLayout::SetMinimumSize));
   qlayout_ns->addConstant("SetFixedSize",             new QoreBigIntNode(QLayout::SetFixedSize));
   qlayout_ns->addConstant("SetMaximumSize",           new QoreBigIntNode(QLayout::SetMaximumSize));
   qlayout_ns->addConstant("SetMinAndMaxSize",         new QoreBigIntNode(QLayout::SetMinAndMaxSize));

   qt_ns->addInitialNamespace(qlayout_ns);

   QoreNamespace *qmovie = new QoreNamespace("QMovie");

   // MovieState enum
   qmovie->addConstant("NotRunning",               new QoreBigIntNode(QMovie::NotRunning));
   qmovie->addConstant("Paused",                   new QoreBigIntNode(QMovie::Paused));
   qmovie->addConstant("Running",                  new QoreBigIntNode(QMovie::Running));

   // CacheMode enum
   qmovie->addConstant("CacheNone",                new QoreBigIntNode(QMovie::CacheNone));
   qmovie->addConstant("CacheAll",                 new QoreBigIntNode(QMovie::CacheAll));

   qmovie->addSystemClass(initQMovieClass(qobject));

   qt_ns->addInitialNamespace(qmovie);

   QoreNamespace *qslider = new QoreNamespace("QSlider");

   // TickPosition enum
   qslider->addConstant("NoTicks",                  new QoreBigIntNode(QSlider::NoTicks));
   qslider->addConstant("TicksAbove",               new QoreBigIntNode(QSlider::TicksAbove));
   qslider->addConstant("TicksLeft",                new QoreBigIntNode(QSlider::TicksLeft));
   qslider->addConstant("TicksBelow",               new QoreBigIntNode(QSlider::TicksBelow));
   qslider->addConstant("TicksRight",               new QoreBigIntNode(QSlider::TicksRight));
   qslider->addConstant("TicksBothSides",           new QoreBigIntNode(QSlider::TicksBothSides));

   qslider->addSystemClass(initQSliderClass(qabstractslider));

   qt_ns->addInitialNamespace(qslider);

   QoreNamespace *qsizepolicy = new QoreNamespace("QSizePolicy");

   // PolicyFlag enum
   qsizepolicy->addConstant("GrowFlag",                 new QoreBigIntNode(QSizePolicy::GrowFlag));
   qsizepolicy->addConstant("ExpandFlag",               new QoreBigIntNode(QSizePolicy::ExpandFlag));
   qsizepolicy->addConstant("ShrinkFlag",               new QoreBigIntNode(QSizePolicy::ShrinkFlag));
   qsizepolicy->addConstant("IgnoreFlag",               new QoreBigIntNode(QSizePolicy::IgnoreFlag));

   // Policy enum
   qsizepolicy->addConstant("Fixed",                    new QoreBigIntNode(QSizePolicy::Fixed));
   qsizepolicy->addConstant("Minimum",                  new QoreBigIntNode(QSizePolicy::Minimum));
   qsizepolicy->addConstant("Maximum",                  new QoreBigIntNode(QSizePolicy::Maximum));
   qsizepolicy->addConstant("Preferred",                new QoreBigIntNode(QSizePolicy::Preferred));
   qsizepolicy->addConstant("MinimumExpanding",         new QoreBigIntNode(QSizePolicy::MinimumExpanding));
   qsizepolicy->addConstant("Expanding",                new QoreBigIntNode(QSizePolicy::Expanding));
   qsizepolicy->addConstant("Ignored",                  new QoreBigIntNode(QSizePolicy::Ignored));

   // ControlType enum
   qsizepolicy->addConstant("DefaultType",              new QoreBigIntNode(QSizePolicy::DefaultType));
   qsizepolicy->addConstant("ButtonBox",                new QoreBigIntNode(QSizePolicy::ButtonBox));
   qsizepolicy->addConstant("CheckBox",                 new QoreBigIntNode(QSizePolicy::CheckBox));
   qsizepolicy->addConstant("ComboBox",                 new QoreBigIntNode(QSizePolicy::ComboBox));
   qsizepolicy->addConstant("Frame",                    new QoreBigIntNode(QSizePolicy::Frame));
   qsizepolicy->addConstant("GroupBox",                 new QoreBigIntNode(QSizePolicy::GroupBox));
   qsizepolicy->addConstant("Label",                    new QoreBigIntNode(QSizePolicy::Label));
   qsizepolicy->addConstant("Line",                     new QoreBigIntNode(QSizePolicy::Line));
   qsizepolicy->addConstant("LineEdit",                 new QoreBigIntNode(QSizePolicy::LineEdit));
   qsizepolicy->addConstant("PushButton",               new QoreBigIntNode(QSizePolicy::PushButton));
   qsizepolicy->addConstant("RadioButton",              new QoreBigIntNode(QSizePolicy::RadioButton));
   qsizepolicy->addConstant("Slider",                   new QoreBigIntNode(QSizePolicy::Slider));
   qsizepolicy->addConstant("SpinBox",                  new QoreBigIntNode(QSizePolicy::SpinBox));
   qsizepolicy->addConstant("TabWidget",                new QoreBigIntNode(QSizePolicy::TabWidget));
   qsizepolicy->addConstant("ToolButton",               new QoreBigIntNode(QSizePolicy::ToolButton));

   qt_ns->addInitialNamespace(qsizepolicy);

   qt_ns->addInitialNamespace(initQLibraryInfoNS());

   QoreNamespace *qicon = new QoreNamespace("QIcon");

   // Mode enum
   qicon->addConstant("Normal",                   new QoreBigIntNode(QIcon::Normal));
   qicon->addConstant("Disabled",                 new QoreBigIntNode(QIcon::Disabled));
   qicon->addConstant("Active",                   new QoreBigIntNode(QIcon::Active));
   qicon->addConstant("Selected",                 new QoreBigIntNode(QIcon::Selected));

   // State enum
   qicon->addConstant("On",                       new QoreBigIntNode(QIcon::On));
   qicon->addConstant("Off",                      new QoreBigIntNode(QIcon::Off));

   qicon->addSystemClass(initQIconClass());
   qt_ns->addInitialNamespace(qicon);

   qt_ns->addInitialNamespace(initQPaletteNS());

   QoreNamespace *qpainter_ns = new QoreNamespace("QPainter");
   
   // RenderHint enum
   qpainter_ns->addConstant("Antialiasing",             new QoreBigIntNode(QPainter::Antialiasing));
   qpainter_ns->addConstant("TextAntialiasing",         new QoreBigIntNode(QPainter::TextAntialiasing));
   qpainter_ns->addConstant("SmoothPixmapTransform",    new QoreBigIntNode(QPainter::SmoothPixmapTransform));
   qpainter_ns->addConstant("HighQualityAntialiasing",  new QoreBigIntNode(QPainter::HighQualityAntialiasing));
   
   // CompositionMode enum
   qpainter_ns->addConstant("CompositionMode_SourceOver",      new QoreBigIntNode(QPainter::CompositionMode_SourceOver));
   qpainter_ns->addConstant("CompositionMode_DestinationOver", new QoreBigIntNode(QPainter::CompositionMode_DestinationOver));
   qpainter_ns->addConstant("CompositionMode_Clear",           new QoreBigIntNode(QPainter::CompositionMode_Clear));
   qpainter_ns->addConstant("CompositionMode_Source",          new QoreBigIntNode(QPainter::CompositionMode_Source));
   qpainter_ns->addConstant("CompositionMode_Destination",     new QoreBigIntNode(QPainter::CompositionMode_Destination));
   qpainter_ns->addConstant("CompositionMode_SourceIn",        new QoreBigIntNode(QPainter::CompositionMode_SourceIn));
   qpainter_ns->addConstant("CompositionMode_DestinationIn",   new QoreBigIntNode(QPainter::CompositionMode_DestinationIn));
   qpainter_ns->addConstant("CompositionMode_SourceOut",       new QoreBigIntNode(QPainter::CompositionMode_SourceOut));
   qpainter_ns->addConstant("CompositionMode_DestinationOut",  new QoreBigIntNode(QPainter::CompositionMode_DestinationOut));
   qpainter_ns->addConstant("CompositionMode_SourceAtop",      new QoreBigIntNode(QPainter::CompositionMode_SourceAtop));
   qpainter_ns->addConstant("CompositionMode_DestinationAtop", new QoreBigIntNode(QPainter::CompositionMode_DestinationAtop));
   qpainter_ns->addConstant("CompositionMode_Xor",             new QoreBigIntNode(QPainter::CompositionMode_Xor));
   qpainter_ns->addConstant("CompositionMode_Plus",            new QoreBigIntNode(QPainter::CompositionMode_Plus));
   qpainter_ns->addConstant("CompositionMode_Multiply",        new QoreBigIntNode(QPainter::CompositionMode_Multiply));
   qpainter_ns->addConstant("CompositionMode_Screen",          new QoreBigIntNode(QPainter::CompositionMode_Screen));
   qpainter_ns->addConstant("CompositionMode_Overlay",         new QoreBigIntNode(QPainter::CompositionMode_Overlay));
   qpainter_ns->addConstant("CompositionMode_Darken",          new QoreBigIntNode(QPainter::CompositionMode_Darken));
   qpainter_ns->addConstant("CompositionMode_Lighten",         new QoreBigIntNode(QPainter::CompositionMode_Lighten));
   qpainter_ns->addConstant("CompositionMode_ColorDodge",      new QoreBigIntNode(QPainter::CompositionMode_ColorDodge));
   qpainter_ns->addConstant("CompositionMode_ColorBurn",       new QoreBigIntNode(QPainter::CompositionMode_ColorBurn));
   qpainter_ns->addConstant("CompositionMode_HardLight",       new QoreBigIntNode(QPainter::CompositionMode_HardLight));
   qpainter_ns->addConstant("CompositionMode_SoftLight",       new QoreBigIntNode(QPainter::CompositionMode_SoftLight));
   qpainter_ns->addConstant("CompositionMode_Difference",      new QoreBigIntNode(QPainter::CompositionMode_Difference));
   qpainter_ns->addConstant("CompositionMode_Exclusion",       new QoreBigIntNode(QPainter::CompositionMode_Exclusion));

   qpainter_ns->addSystemClass(initQPainterClass());

   qt_ns->addInitialNamespace(qpainter_ns);

   QoreClass *qabstractbutton, *qtextformat, *qtextframeformat, *qtextcharformat,
      *qstyleoption, *qstyleoptionviewitem, *qabstractitemdelegate,
      *qabstractspinbox, *qdatetimeedit, *qabstractscrollarea, 
      *qcombobox, *qstyleoptioncomplex, *qabstractitemview, 
      *qtableview, *qdialog, *qvalidator;

   QoreNamespace *qdialog_ns = new QoreNamespace("QDialog");

   qdialog_ns->addSystemClass((qdialog = initQDialogClass(qwidget)));
   qdialog_ns->addInitialNamespace(initQFileDialogNS(qdialog));
   qdialog_ns->addSystemClass(initQPrintDialogClass(qdialog));

   qdialog_ns->addConstant("Rejected",   new QoreBigIntNode(QDialog::Rejected));
   qdialog_ns->addConstant("Accepted",   new QoreBigIntNode(QDialog::Accepted));
 
   qt_ns->addInitialNamespace(initQStyleNS(qobject));

   // automatically added classes
   qt_ns->addSystemClass(initQPointFClass());
   qt_ns->addSystemClass(initQPolygonClass());
   qt_ns->addSystemClass(initQPolygonFClass());
   qt_ns->addSystemClass(initQLineClass());
   qt_ns->addSystemClass(initQLineFClass());
   qt_ns->addSystemClass((qabstractbutton = initQAbstractButtonClass(qwidget)));
   qt_ns->addSystemClass(initQPushButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQMenuClass(qwidget));
   qt_ns->addSystemClass(initQToolButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQTextLengthClass());
   qt_ns->addSystemClass((qtextformat = initQTextFormatClass()));
   qt_ns->addSystemClass(initQTextBlockFormatClass(qtextformat));
   qt_ns->addSystemClass((qtextcharformat = initQTextCharFormatClass(qtextformat)));
   qt_ns->addSystemClass(initQPenClass());
   qt_ns->addSystemClass((qtextframeformat = initQTextFrameFormatClass(qtextformat)));
   qt_ns->addSystemClass(initQTextTableFormatClass(qtextframeformat));
   qt_ns->addSystemClass(initQTextListFormatClass(qtextformat));
   qt_ns->addSystemClass(initQTextImageFormatClass(qtextcharformat));
   qt_ns->addSystemClass((qstyleoption = initQStyleOptionClass()));
   qt_ns->addSystemClass((qstyleoptioncomplex = initQStyleOptionComplexClass(qstyleoption)));
   qt_ns->addSystemClass(initQStyleOptionComboBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionGroupBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSizeGripClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSliderClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionSpinBoxClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionTitleBarClass(qstyleoptioncomplex));
   qt_ns->addSystemClass(initQStyleOptionToolButtonClass(qstyleoptioncomplex));
   qt_ns->addInitialNamespace(initQStyleOptionButtonNS(qstyleoption));
   qt_ns->addSystemClass(initQModelIndexClass());
   qt_ns->addSystemClass((qstyleoptionviewitem = initQStyleOptionViewItemClass(qstyleoption)));
   qt_ns->addSystemClass(initQStyleOptionViewItemV2Class(qstyleoptionviewitem));
   qt_ns->addSystemClass(initQAbstractItemModelClass(qobject));
   qt_ns->addSystemClass((qabstractitemdelegate = initQAbstractItemDelegateClass(qobject)));
   qt_ns->addSystemClass(initQItemDelegateClass(qabstractitemdelegate));
   qt_ns->addSystemClass((qcombobox = initQComboBoxClass(qwidget)));
   qt_ns->addSystemClass(initQCheckBoxClass(qabstractbutton));
   qt_ns->addSystemClass((qabstractspinbox = initQAbstractSpinBoxClass(qwidget)));
   qt_ns->addSystemClass(initQByteArrayClass());
   qt_ns->addSystemClass(initQUrlClass());
   qt_ns->addSystemClass(initQVariantClass());
   qt_ns->addSystemClass(initQGroupBoxClass(qwidget));
   qt_ns->addSystemClass(initQFontMetricsClass());
   qt_ns->addSystemClass(initQFontDatabaseClass());
   qt_ns->addSystemClass(initQFontInfoClass());
   qt_ns->addSystemClass(initQScrollBarClass(qabstractslider));
   qt_ns->addSystemClass((qabstractscrollarea = initQAbstractScrollAreaClass(qframe)));
   qt_ns->addSystemClass(initQScrollAreaClass(qabstractscrollarea));
   qt_ns->addSystemClass(initQMimeDataClass(qobject));
   qt_ns->addSystemClass(initQFontComboBoxClass(qcombobox));
   qt_ns->addSystemClass(initQMainWindowClass(qwidget));
   qt_ns->addSystemClass(initQRadioButtonClass(qabstractbutton));
   qt_ns->addSystemClass(initQSpinBoxClass(qabstractspinbox));
   qt_ns->addSystemClass(initQTableWidgetItemClass());
   qt_ns->addSystemClass(initQStyleOptionMenuItemClass(qstyleoption));
   qt_ns->addInitialNamespace(initQDirNS());
   qt_ns->addSystemClass(initQMetaObjectClass());
   qt_ns->addSystemClass(initQMenuBarClass(qwidget));
   qt_ns->addSystemClass(initQRegExpClass());
   qt_ns->addSystemClass((qvalidator = initQValidatorClass(qobject)));
   qt_ns->addSystemClass(initQDoubleValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQIntValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQRegExpValidatorClass(qvalidator));
   qt_ns->addSystemClass(initQFileInfoClass());
   qt_ns->addSystemClass(initQIODeviceClass(qobject));
   qt_ns->addSystemClass(initQImageWriterClass());
   qt_ns->addSystemClass(initQDialClass(qabstractslider));
   qt_ns->addSystemClass(initQStackedWidgetClass(qframe));
   qt_ns->addSystemClass(initQDoubleSpinBoxClass(qabstractspinbox));
   qt_ns->addSystemClass(initQProgressBarClass(qwidget));
   qt_ns->addSystemClass(initQPainterPathClass());
   qt_ns->addSystemClass(initQPaintEngineClass());
   qt_ns->addSystemClass(initQBasicTimerClass());
   qt_ns->addSystemClass(initQTabBarClass(qwidget));
   qt_ns->addSystemClass(initQStyleOptionTabClass(qstyleoption));
   qt_ns->addSystemClass(initQStyleOptionTabWidgetFrameClass(qstyleoption));
   qt_ns->addSystemClass(initQTabWidgetClass(qwidget));

   qt_ns->addInitialNamespace(initQTextEditNS(qabstractscrollarea));
   qt_ns->addSystemClass(initQDesktopWidgetClass(qwidget));
   qt_ns->addSystemClass(initQWizardPageClass(qwidget));
   qt_ns->addSystemClass(initQTranslatorClass(qobject));
   qt_ns->addInitialNamespace(initQListWidgetItemNS());
   qt_ns->addInitialNamespace(initQDialogButtonBoxNS(qwidget));
   qt_ns->addInitialNamespace(initQToolBarNS(qwidget));
   qdialog_ns->addSystemClass(initQProgressDialogClass(qdialog));
   qdialog_ns->addSystemClass(initQErrorMessageClass(qdialog));
   qt_ns->addSystemClass(initQStackedLayoutClass(qobject));

   // add QBoxLayout namespace and constants
   class QoreNamespace *qbl = new QoreNamespace("QBoxLayout");

   // Direction enum
   qbl->addConstant("LeftToRight",    new QoreBigIntNode(QBoxLayout::LeftToRight));
   qbl->addConstant("RightToLeft",    new QoreBigIntNode(QBoxLayout::RightToLeft));
   qbl->addConstant("TopToBottom",    new QoreBigIntNode(QBoxLayout::TopToBottom));
   qbl->addConstant("BottomToTop",    new QoreBigIntNode(QBoxLayout::BottomToTop));

   qt_ns->addInitialNamespace(qbl);

   qt_ns->addInitialNamespace(initQSystemTrayIconNS(qobject));

   QoreNamespace *qdatetimeedit_ns = new QoreNamespace("QDateTimeEdit");
   
   // Section enum
   qdatetimeedit_ns->addConstant("NoSection",                new QoreBigIntNode(QDateTimeEdit::NoSection));
   qdatetimeedit_ns->addConstant("AmPmSection",              new QoreBigIntNode(QDateTimeEdit::AmPmSection));
   qdatetimeedit_ns->addConstant("MSecSection",              new QoreBigIntNode(QDateTimeEdit::MSecSection));
   qdatetimeedit_ns->addConstant("SecondSection",            new QoreBigIntNode(QDateTimeEdit::SecondSection));
   qdatetimeedit_ns->addConstant("MinuteSection",            new QoreBigIntNode(QDateTimeEdit::MinuteSection));
   qdatetimeedit_ns->addConstant("HourSection",              new QoreBigIntNode(QDateTimeEdit::HourSection));
   qdatetimeedit_ns->addConstant("DaySection",               new QoreBigIntNode(QDateTimeEdit::DaySection));
   qdatetimeedit_ns->addConstant("MonthSection",             new QoreBigIntNode(QDateTimeEdit::MonthSection));
   qdatetimeedit_ns->addConstant("YearSection",              new QoreBigIntNode(QDateTimeEdit::YearSection));
   qdatetimeedit_ns->addConstant("TimeSections_Mask",        new QoreBigIntNode(QDateTimeEdit::TimeSections_Mask));
   qdatetimeedit_ns->addConstant("DateSections_Mask",        new QoreBigIntNode(QDateTimeEdit::DateSections_Mask));

   qdatetimeedit_ns->addSystemClass((qdatetimeedit = initQDateTimeEditClass(qabstractspinbox)));
   qdatetimeedit_ns->addSystemClass(initQDateEditClass(qdatetimeedit));
   qdatetimeedit_ns->addSystemClass(initQTimeEditClass(qdatetimeedit));

   qt_ns->addInitialNamespace(qdatetimeedit_ns);

   qdialog_ns->addInitialNamespace(initQWizardNS(qdialog));

   QoreNamespace *qmessagebox = new QoreNamespace("QMessageBox");
   qmessagebox->addSystemClass(initQMessageBoxClass(qdialog));

   // Icon enum
   qmessagebox->addConstant("NoIcon",                   new QoreBigIntNode(QMessageBox::NoIcon));
   qmessagebox->addConstant("Information",              new QoreBigIntNode(QMessageBox::Information));
   qmessagebox->addConstant("Warning",                  new QoreBigIntNode(QMessageBox::Warning));
   qmessagebox->addConstant("Critical",                 new QoreBigIntNode(QMessageBox::Critical));
   qmessagebox->addConstant("Question",                 new QoreBigIntNode(QMessageBox::Question));

   // ButtonRole enum
   qmessagebox->addConstant("InvalidRole",              new QoreBigIntNode(QMessageBox::InvalidRole));
   qmessagebox->addConstant("AcceptRole",               new QoreBigIntNode(QMessageBox::AcceptRole));
   qmessagebox->addConstant("RejectRole",               new QoreBigIntNode(QMessageBox::RejectRole));
   qmessagebox->addConstant("DestructiveRole",          new QoreBigIntNode(QMessageBox::DestructiveRole));
   qmessagebox->addConstant("ActionRole",               new QoreBigIntNode(QMessageBox::ActionRole));
   qmessagebox->addConstant("HelpRole",                 new QoreBigIntNode(QMessageBox::HelpRole));
   qmessagebox->addConstant("YesRole",                  new QoreBigIntNode(QMessageBox::YesRole));
   qmessagebox->addConstant("NoRole",                   new QoreBigIntNode(QMessageBox::NoRole));
   qmessagebox->addConstant("ResetRole",                new QoreBigIntNode(QMessageBox::ResetRole));
   qmessagebox->addConstant("ApplyRole",                new QoreBigIntNode(QMessageBox::ApplyRole));

   // StandardButton enum
   qmessagebox->addConstant("NoButton",                 new QoreBigIntNode(QMessageBox::NoButton));
   qmessagebox->addConstant("Ok",                       new QoreBigIntNode(QMessageBox::Ok));
   qmessagebox->addConstant("Save",                     new QoreBigIntNode(QMessageBox::Save));
   qmessagebox->addConstant("SaveAll",                  new QoreBigIntNode(QMessageBox::SaveAll));
   qmessagebox->addConstant("Open",                     new QoreBigIntNode(QMessageBox::Open));
   qmessagebox->addConstant("Yes",                      new QoreBigIntNode(QMessageBox::Yes));
   qmessagebox->addConstant("YesToAll",                 new QoreBigIntNode(QMessageBox::YesToAll));
   qmessagebox->addConstant("No",                       new QoreBigIntNode(QMessageBox::No));
   qmessagebox->addConstant("NoToAll",                  new QoreBigIntNode(QMessageBox::NoToAll));
   qmessagebox->addConstant("Abort",                    new QoreBigIntNode(QMessageBox::Abort));
   qmessagebox->addConstant("Retry",                    new QoreBigIntNode(QMessageBox::Retry));
   qmessagebox->addConstant("Ignore",                   new QoreBigIntNode(QMessageBox::Ignore));
   qmessagebox->addConstant("Close",                    new QoreBigIntNode(QMessageBox::Close));
   qmessagebox->addConstant("Cancel",                   new QoreBigIntNode(QMessageBox::Cancel));
   qmessagebox->addConstant("Discard",                  new QoreBigIntNode(QMessageBox::Discard));
   qmessagebox->addConstant("Help",                     new QoreBigIntNode(QMessageBox::Help));
   qmessagebox->addConstant("Apply",                    new QoreBigIntNode(QMessageBox::Apply));
   qmessagebox->addConstant("Reset",                    new QoreBigIntNode(QMessageBox::Reset));
   qmessagebox->addConstant("RestoreDefaults",          new QoreBigIntNode(QMessageBox::RestoreDefaults));
   qmessagebox->addConstant("FirstButton",              new QoreBigIntNode(QMessageBox::FirstButton));
   qmessagebox->addConstant("LastButton",               new QoreBigIntNode(QMessageBox::LastButton));
   qmessagebox->addConstant("YesAll",                   new QoreBigIntNode(QMessageBox::YesAll));
   qmessagebox->addConstant("NoAll",                    new QoreBigIntNode(QMessageBox::NoAll));
   qmessagebox->addConstant("Default",                  new QoreBigIntNode(QMessageBox::Default));
   qmessagebox->addConstant("Escape",                   new QoreBigIntNode(QMessageBox::Escape));
   qmessagebox->addConstant("FlagMask",                 new QoreBigIntNode(QMessageBox::FlagMask));
   qmessagebox->addConstant("ButtonMask",               new QoreBigIntNode(QMessageBox::ButtonMask));

   qdialog_ns->addInitialNamespace(qmessagebox);

   qt_ns->addInitialNamespace(qdialog_ns);

   QoreNamespace *qprinter = new QoreNamespace("QPrinter");

   qprinter->addSystemClass(initQPrinterClass(qpaintdevice));

   // PrinterMode enum
   qprinter->addConstant("ScreenResolution",         new QoreBigIntNode(QPrinter::ScreenResolution));
   qprinter->addConstant("PrinterResolution",        new QoreBigIntNode(QPrinter::PrinterResolution));
   qprinter->addConstant("HighResolution",           new QoreBigIntNode(QPrinter::HighResolution));

   // Orientation enum
   qprinter->addConstant("Portrait",                 new QoreBigIntNode(QPrinter::Portrait));
   qprinter->addConstant("Landscape",                new QoreBigIntNode(QPrinter::Landscape));

   // PageSize enum
   qprinter->addConstant("A4",                       new QoreBigIntNode(QPrinter::A4));
   qprinter->addConstant("B5",                       new QoreBigIntNode(QPrinter::B5));
   qprinter->addConstant("Letter",                   new QoreBigIntNode(QPrinter::Letter));
   qprinter->addConstant("Legal",                    new QoreBigIntNode(QPrinter::Legal));
   qprinter->addConstant("Executive",                new QoreBigIntNode(QPrinter::Executive));
   qprinter->addConstant("A0",                       new QoreBigIntNode(QPrinter::A0));
   qprinter->addConstant("A1",                       new QoreBigIntNode(QPrinter::A1));
   qprinter->addConstant("A2",                       new QoreBigIntNode(QPrinter::A2));
   qprinter->addConstant("A3",                       new QoreBigIntNode(QPrinter::A3));
   qprinter->addConstant("A5",                       new QoreBigIntNode(QPrinter::A5));
   qprinter->addConstant("A6",                       new QoreBigIntNode(QPrinter::A6));
   qprinter->addConstant("A7",                       new QoreBigIntNode(QPrinter::A7));
   qprinter->addConstant("A8",                       new QoreBigIntNode(QPrinter::A8));
   qprinter->addConstant("A9",                       new QoreBigIntNode(QPrinter::A9));
   qprinter->addConstant("B0",                       new QoreBigIntNode(QPrinter::B0));
   qprinter->addConstant("B1",                       new QoreBigIntNode(QPrinter::B1));
   qprinter->addConstant("B10",                      new QoreBigIntNode(QPrinter::B10));
   qprinter->addConstant("B2",                       new QoreBigIntNode(QPrinter::B2));
   qprinter->addConstant("B3",                       new QoreBigIntNode(QPrinter::B3));
   qprinter->addConstant("B4",                       new QoreBigIntNode(QPrinter::B4));
   qprinter->addConstant("B6",                       new QoreBigIntNode(QPrinter::B6));
   qprinter->addConstant("B7",                       new QoreBigIntNode(QPrinter::B7));
   qprinter->addConstant("B8",                       new QoreBigIntNode(QPrinter::B8));
   qprinter->addConstant("B9",                       new QoreBigIntNode(QPrinter::B9));
   qprinter->addConstant("C5E",                      new QoreBigIntNode(QPrinter::C5E));
   qprinter->addConstant("Comm10E",                  new QoreBigIntNode(QPrinter::Comm10E));
   qprinter->addConstant("DLE",                      new QoreBigIntNode(QPrinter::DLE));
   qprinter->addConstant("Folio",                    new QoreBigIntNode(QPrinter::Folio));
   qprinter->addConstant("Ledger",                   new QoreBigIntNode(QPrinter::Ledger));
   qprinter->addConstant("Tabloid",                  new QoreBigIntNode(QPrinter::Tabloid));
   qprinter->addConstant("Custom",                   new QoreBigIntNode(QPrinter::Custom));

   // PageOrder enum
   qprinter->addConstant("FirstPageFirst",           new QoreBigIntNode(QPrinter::FirstPageFirst));
   qprinter->addConstant("LastPageFirst",            new QoreBigIntNode(QPrinter::LastPageFirst));
   
   // ColorMode enum
   qprinter->addConstant("GrayScale",                new QoreBigIntNode(QPrinter::GrayScale));
   qprinter->addConstant("Color",                    new QoreBigIntNode(QPrinter::Color));

   // PaperSource enum
   qprinter->addConstant("OnlyOne",                  new QoreBigIntNode(QPrinter::OnlyOne));
   qprinter->addConstant("Lower",                    new QoreBigIntNode(QPrinter::Lower));
   qprinter->addConstant("Middle",                   new QoreBigIntNode(QPrinter::Middle));
   qprinter->addConstant("Manual",                   new QoreBigIntNode(QPrinter::Manual));
   qprinter->addConstant("Envelope",                 new QoreBigIntNode(QPrinter::Envelope));
   qprinter->addConstant("EnvelopeManual",           new QoreBigIntNode(QPrinter::EnvelopeManual));
   qprinter->addConstant("Auto",                     new QoreBigIntNode(QPrinter::Auto));
   qprinter->addConstant("Tractor",                  new QoreBigIntNode(QPrinter::Tractor));
   qprinter->addConstant("SmallFormat",              new QoreBigIntNode(QPrinter::SmallFormat));
   qprinter->addConstant("LargeFormat",              new QoreBigIntNode(QPrinter::LargeFormat));
   qprinter->addConstant("LargeCapacity",            new QoreBigIntNode(QPrinter::LargeCapacity));
   qprinter->addConstant("Cassette",                 new QoreBigIntNode(QPrinter::Cassette));
   qprinter->addConstant("FormSource",               new QoreBigIntNode(QPrinter::FormSource));
   qprinter->addConstant("MaxPageSource",            new QoreBigIntNode(QPrinter::MaxPageSource));

   // PrinterState enum
   qprinter->addConstant("Idle",                     new QoreBigIntNode(QPrinter::Idle));
   qprinter->addConstant("Active",                   new QoreBigIntNode(QPrinter::Active));
   qprinter->addConstant("Aborted",                  new QoreBigIntNode(QPrinter::Aborted));
   qprinter->addConstant("Error",                    new QoreBigIntNode(QPrinter::Error));

   // OutputFormat enum
   qprinter->addConstant("NativeFormat",             new QoreBigIntNode(QPrinter::NativeFormat));
   qprinter->addConstant("PdfFormat",                new QoreBigIntNode(QPrinter::PdfFormat));
   qprinter->addConstant("PostScriptFormat",         new QoreBigIntNode(QPrinter::PostScriptFormat));

   // PrintRange enum
   qprinter->addConstant("AllPages",                 new QoreBigIntNode(QPrinter::AllPages));
   qprinter->addConstant("Selection",                new QoreBigIntNode(QPrinter::Selection));
   qprinter->addConstant("PageRange",                new QoreBigIntNode(QPrinter::PageRange));

   qt_ns->addInitialNamespace(qprinter);

   QoreNamespace *qlineedit = new QoreNamespace("QLineEdit");

   // EchoMode enum
   qlineedit->addConstant("Normal",                   new QoreBigIntNode(QLineEdit::Normal));
   qlineedit->addConstant("NoEcho",                   new QoreBigIntNode(QLineEdit::NoEcho));
   qlineedit->addConstant("Password",                 new QoreBigIntNode(QLineEdit::Password));
   qlineedit->addConstant("PasswordEchoOnEdit",       new QoreBigIntNode(QLineEdit::PasswordEchoOnEdit));

   qlineedit->addSystemClass(initQLineEditClass(qwidget));

   qt_ns->addInitialNamespace(qlineedit);

   QoreNamespace *qabstractitemview_ns = new QoreNamespace("QAbstractItemView");
   
   // SelectionMode enum
   qabstractitemview_ns->addConstant("NoSelection",              new QoreBigIntNode(QAbstractItemView::NoSelection));
   qabstractitemview_ns->addConstant("SingleSelection",          new QoreBigIntNode(QAbstractItemView::SingleSelection));
   qabstractitemview_ns->addConstant("MultiSelection",           new QoreBigIntNode(QAbstractItemView::MultiSelection));
   qabstractitemview_ns->addConstant("ExtendedSelection",        new QoreBigIntNode(QAbstractItemView::ExtendedSelection));
   qabstractitemview_ns->addConstant("ContiguousSelection",      new QoreBigIntNode(QAbstractItemView::ContiguousSelection));

   // SelectionBehavior enum
   qabstractitemview_ns->addConstant("SelectItems",              new QoreBigIntNode(QAbstractItemView::SelectItems));
   qabstractitemview_ns->addConstant("SelectRows",               new QoreBigIntNode(QAbstractItemView::SelectRows));
   qabstractitemview_ns->addConstant("SelectColumns",            new QoreBigIntNode(QAbstractItemView::SelectColumns));

   // ScrollHint enum
   qabstractitemview_ns->addConstant("EnsureVisible",            new QoreBigIntNode(QAbstractItemView::EnsureVisible));
   qabstractitemview_ns->addConstant("PositionAtTop",            new QoreBigIntNode(QAbstractItemView::PositionAtTop));
   qabstractitemview_ns->addConstant("PositionAtBottom",         new QoreBigIntNode(QAbstractItemView::PositionAtBottom));
   qabstractitemview_ns->addConstant("PositionAtCenter",         new QoreBigIntNode(QAbstractItemView::PositionAtCenter));

   // EditTrigger enum
   qabstractitemview_ns->addConstant("NoEditTriggers",           new QoreBigIntNode(QAbstractItemView::NoEditTriggers));
   qabstractitemview_ns->addConstant("CurrentChanged",           new QoreBigIntNode(QAbstractItemView::CurrentChanged));
   qabstractitemview_ns->addConstant("DoubleClicked",            new QoreBigIntNode(QAbstractItemView::DoubleClicked));
   qabstractitemview_ns->addConstant("SelectedClicked",          new QoreBigIntNode(QAbstractItemView::SelectedClicked));
   qabstractitemview_ns->addConstant("EditKeyPressed",           new QoreBigIntNode(QAbstractItemView::EditKeyPressed));
   qabstractitemview_ns->addConstant("AnyKeyPressed",            new QoreBigIntNode(QAbstractItemView::AnyKeyPressed));
   qabstractitemview_ns->addConstant("AllEditTriggers",          new QoreBigIntNode(QAbstractItemView::AllEditTriggers));

   // ScrollMode enum
   qabstractitemview_ns->addConstant("ScrollPerItem",            new QoreBigIntNode(QAbstractItemView::ScrollPerItem));
   qabstractitemview_ns->addConstant("ScrollPerPixel",           new QoreBigIntNode(QAbstractItemView::ScrollPerPixel));

   qabstractitemview_ns->addSystemClass((qabstractitemview = initQAbstractItemViewClass(qabstractscrollarea)));
   qabstractitemview_ns->addSystemClass((qtableview = initQTableViewClass(qabstractitemview)));
   qabstractitemview_ns->addSystemClass(initQTableWidgetClass(qtableview));

   qabstractitemview_ns->addInitialNamespace(initQListViewNS(qabstractitemview));
   
   qt_ns->addInitialNamespace(qabstractitemview_ns);

   QoreNamespace *qheaderview = new QoreNamespace("QHeaderView");

   // ResizeMode enum
   qheaderview->addConstant("Interactive",              new QoreBigIntNode(QHeaderView::Interactive));
   qheaderview->addConstant("Stretch",                  new QoreBigIntNode(QHeaderView::Stretch));
   qheaderview->addConstant("Fixed",                    new QoreBigIntNode(QHeaderView::Fixed));
   qheaderview->addConstant("ResizeToContents",         new QoreBigIntNode(QHeaderView::ResizeToContents));
   qheaderview->addConstant("Custom",                   new QoreBigIntNode(QHeaderView::Custom));

   qheaderview->addSystemClass(initQHeaderViewClass(qabstractitemview));

   qt_ns->addInitialNamespace(qheaderview);


   QoreNamespace *qclipboard = new QoreNamespace("QClipboard");
   
   // Mode enum
   qclipboard->addConstant("Clipboard",                new QoreBigIntNode(QClipboard::Clipboard));
   qclipboard->addConstant("Selection",                new QoreBigIntNode(QClipboard::Selection));
   qclipboard->addConstant("FindBuffer",               new QoreBigIntNode(QClipboard::FindBuffer));
   qclipboard->addConstant("LastMode",                 new QoreBigIntNode(QClipboard::LastMode));

   qclipboard->addSystemClass(initQClipboardClass(qobject));

   qt_ns->addInitialNamespace(qclipboard);

   QoreNamespace *qchar = new QoreNamespace("QChar");
   qchar->addSystemClass(initQCharClass());

   // SpecialCharacter enum
   qchar->addConstant("Null",                     new QoreBigIntNode(QChar::Null));
   qchar->addConstant("Nbsp",                     new QoreBigIntNode(QChar::Nbsp));
   qchar->addConstant("ReplacementCharacter",     new QoreBigIntNode(QChar::ReplacementCharacter));
   qchar->addConstant("ObjectReplacementCharacter", new QoreBigIntNode(QChar::ObjectReplacementCharacter));
   qchar->addConstant("ByteOrderMark",            new QoreBigIntNode(QChar::ByteOrderMark));
   qchar->addConstant("ByteOrderSwapped",         new QoreBigIntNode(QChar::ByteOrderSwapped));
   qchar->addConstant("ParagraphSeparator",       new QoreBigIntNode(QChar::ParagraphSeparator));
   qchar->addConstant("LineSeparator",            new QoreBigIntNode(QChar::LineSeparator));

   // Category enum
   qchar->addConstant("NoCategory",               new QoreBigIntNode(QChar::NoCategory));
   qchar->addConstant("Mark_NonSpacing",          new QoreBigIntNode(QChar::Mark_NonSpacing));
   qchar->addConstant("Mark_SpacingCombining",    new QoreBigIntNode(QChar::Mark_SpacingCombining));
   qchar->addConstant("Mark_Enclosing",           new QoreBigIntNode(QChar::Mark_Enclosing));
   qchar->addConstant("Number_DecimalDigit",      new QoreBigIntNode(QChar::Number_DecimalDigit));
   qchar->addConstant("Number_Letter",            new QoreBigIntNode(QChar::Number_Letter));
   qchar->addConstant("Number_Other",             new QoreBigIntNode(QChar::Number_Other));
   qchar->addConstant("Separator_Space",          new QoreBigIntNode(QChar::Separator_Space));
   qchar->addConstant("Separator_Line",           new QoreBigIntNode(QChar::Separator_Line));
   qchar->addConstant("Separator_Paragraph",      new QoreBigIntNode(QChar::Separator_Paragraph));
   qchar->addConstant("Other_Control",            new QoreBigIntNode(QChar::Other_Control));
   qchar->addConstant("Other_Format",             new QoreBigIntNode(QChar::Other_Format));
   qchar->addConstant("Other_Surrogate",          new QoreBigIntNode(QChar::Other_Surrogate));
   qchar->addConstant("Other_PrivateUse",         new QoreBigIntNode(QChar::Other_PrivateUse));
   qchar->addConstant("Other_NotAssigned",        new QoreBigIntNode(QChar::Other_NotAssigned));
   qchar->addConstant("Letter_Uppercase",         new QoreBigIntNode(QChar::Letter_Uppercase));
   qchar->addConstant("Letter_Lowercase",         new QoreBigIntNode(QChar::Letter_Lowercase));
   qchar->addConstant("Letter_Titlecase",         new QoreBigIntNode(QChar::Letter_Titlecase));
   qchar->addConstant("Letter_Modifier",          new QoreBigIntNode(QChar::Letter_Modifier));
   qchar->addConstant("Letter_Other",             new QoreBigIntNode(QChar::Letter_Other));
   qchar->addConstant("Punctuation_Connector",    new QoreBigIntNode(QChar::Punctuation_Connector));
   qchar->addConstant("Punctuation_Dash",         new QoreBigIntNode(QChar::Punctuation_Dash));
   qchar->addConstant("Punctuation_Open",         new QoreBigIntNode(QChar::Punctuation_Open));
   qchar->addConstant("Punctuation_Close",        new QoreBigIntNode(QChar::Punctuation_Close));
   qchar->addConstant("Punctuation_InitialQuote", new QoreBigIntNode(QChar::Punctuation_InitialQuote));
   qchar->addConstant("Punctuation_FinalQuote",   new QoreBigIntNode(QChar::Punctuation_FinalQuote));
   qchar->addConstant("Punctuation_Other",        new QoreBigIntNode(QChar::Punctuation_Other));
   qchar->addConstant("Symbol_Math",              new QoreBigIntNode(QChar::Symbol_Math));
   qchar->addConstant("Symbol_Currency",          new QoreBigIntNode(QChar::Symbol_Currency));
   qchar->addConstant("Symbol_Modifier",          new QoreBigIntNode(QChar::Symbol_Modifier));
   qchar->addConstant("Symbol_Other",             new QoreBigIntNode(QChar::Symbol_Other));
   qchar->addConstant("Punctuation_Dask",         new QoreBigIntNode(QChar::Punctuation_Dask));

   // Direction enum
   qchar->addConstant("DirL",                     new QoreBigIntNode(QChar::DirL));
   qchar->addConstant("DirR",                     new QoreBigIntNode(QChar::DirR));
   qchar->addConstant("DirEN",                    new QoreBigIntNode(QChar::DirEN));
   qchar->addConstant("DirES",                    new QoreBigIntNode(QChar::DirES));
   qchar->addConstant("DirET",                    new QoreBigIntNode(QChar::DirET));
   qchar->addConstant("DirAN",                    new QoreBigIntNode(QChar::DirAN));
   qchar->addConstant("DirCS",                    new QoreBigIntNode(QChar::DirCS));
   qchar->addConstant("DirB",                     new QoreBigIntNode(QChar::DirB));
   qchar->addConstant("DirS",                     new QoreBigIntNode(QChar::DirS));
   qchar->addConstant("DirWS",                    new QoreBigIntNode(QChar::DirWS));
   qchar->addConstant("DirON",                    new QoreBigIntNode(QChar::DirON));
   qchar->addConstant("DirLRE",                   new QoreBigIntNode(QChar::DirLRE));
   qchar->addConstant("DirLRO",                   new QoreBigIntNode(QChar::DirLRO));
   qchar->addConstant("DirAL",                    new QoreBigIntNode(QChar::DirAL));
   qchar->addConstant("DirRLE",                   new QoreBigIntNode(QChar::DirRLE));
   qchar->addConstant("DirRLO",                   new QoreBigIntNode(QChar::DirRLO));
   qchar->addConstant("DirPDF",                   new QoreBigIntNode(QChar::DirPDF));
   qchar->addConstant("DirNSM",                   new QoreBigIntNode(QChar::DirNSM));
   qchar->addConstant("DirBN",                    new QoreBigIntNode(QChar::DirBN));

   // Decomposition enum
   qchar->addConstant("NoDecomposition",          new QoreBigIntNode(QChar::NoDecomposition));
   qchar->addConstant("Canonical",                new QoreBigIntNode(QChar::Canonical));
   qchar->addConstant("Font",                     new QoreBigIntNode(QChar::Font));
   qchar->addConstant("NoBreak",                  new QoreBigIntNode(QChar::NoBreak));
   qchar->addConstant("Initial",                  new QoreBigIntNode(QChar::Initial));
   qchar->addConstant("Medial",                   new QoreBigIntNode(QChar::Medial));
   qchar->addConstant("Final",                    new QoreBigIntNode(QChar::Final));
   qchar->addConstant("Isolated",                 new QoreBigIntNode(QChar::Isolated));
   qchar->addConstant("Circle",                   new QoreBigIntNode(QChar::Circle));
   qchar->addConstant("Super",                    new QoreBigIntNode(QChar::Super));
   qchar->addConstant("Sub",                      new QoreBigIntNode(QChar::Sub));
   qchar->addConstant("Vertical",                 new QoreBigIntNode(QChar::Vertical));
   qchar->addConstant("Wide",                     new QoreBigIntNode(QChar::Wide));
   qchar->addConstant("Narrow",                   new QoreBigIntNode(QChar::Narrow));
   qchar->addConstant("Small",                    new QoreBigIntNode(QChar::Small));
   qchar->addConstant("Square",                   new QoreBigIntNode(QChar::Square));
   qchar->addConstant("Compat",                   new QoreBigIntNode(QChar::Compat));
   qchar->addConstant("Fraction",                 new QoreBigIntNode(QChar::Fraction));

   // Joining enum
   qchar->addConstant("OtherJoining",             new QoreBigIntNode(QChar::OtherJoining));
   qchar->addConstant("Dual",                     new QoreBigIntNode(QChar::Dual));
   qchar->addConstant("Right",                    new QoreBigIntNode(QChar::Right));
   qchar->addConstant("Center",                   new QoreBigIntNode(QChar::Center));

   // Combining class
   qchar->addConstant("Combining_BelowLeftAttached", new QoreBigIntNode(QChar::Combining_BelowLeftAttached));
   qchar->addConstant("Combining_BelowAttached",  new QoreBigIntNode(QChar::Combining_BelowAttached));
   qchar->addConstant("Combining_BelowRightAttached", new QoreBigIntNode(QChar::Combining_BelowRightAttached));
   qchar->addConstant("Combining_LeftAttached",   new QoreBigIntNode(QChar::Combining_LeftAttached));
   qchar->addConstant("Combining_RightAttached",  new QoreBigIntNode(QChar::Combining_RightAttached));
   qchar->addConstant("Combining_AboveLeftAttached", new QoreBigIntNode(QChar::Combining_AboveLeftAttached));
   qchar->addConstant("Combining_AboveAttached",  new QoreBigIntNode(QChar::Combining_AboveAttached));
   qchar->addConstant("Combining_AboveRightAttached", new QoreBigIntNode(QChar::Combining_AboveRightAttached));
   qchar->addConstant("Combining_BelowLeft",      new QoreBigIntNode(QChar::Combining_BelowLeft));
   qchar->addConstant("Combining_Below",          new QoreBigIntNode(QChar::Combining_Below));
   qchar->addConstant("Combining_BelowRight",     new QoreBigIntNode(QChar::Combining_BelowRight));
   qchar->addConstant("Combining_Left",           new QoreBigIntNode(QChar::Combining_Left));
   qchar->addConstant("Combining_Right",          new QoreBigIntNode(QChar::Combining_Right));
   qchar->addConstant("Combining_AboveLeft",      new QoreBigIntNode(QChar::Combining_AboveLeft));
   qchar->addConstant("Combining_Above",          new QoreBigIntNode(QChar::Combining_Above));
   qchar->addConstant("Combining_AboveRight",     new QoreBigIntNode(QChar::Combining_AboveRight));
   qchar->addConstant("Combining_DoubleBelow",    new QoreBigIntNode(QChar::Combining_DoubleBelow));
   qchar->addConstant("Combining_DoubleAbove",    new QoreBigIntNode(QChar::Combining_DoubleAbove));
   qchar->addConstant("Combining_IotaSubscript",  new QoreBigIntNode(QChar::Combining_IotaSubscript));

   // UnicodeVersion
   qchar->addConstant("Unicode_Unassigned",       new QoreBigIntNode(QChar::Unicode_Unassigned));
   qchar->addConstant("Unicode_1_1",              new QoreBigIntNode(QChar::Unicode_1_1));
   qchar->addConstant("Unicode_2_0",              new QoreBigIntNode(QChar::Unicode_2_0));
   qchar->addConstant("Unicode_2_1_2",            new QoreBigIntNode(QChar::Unicode_2_1_2));
   qchar->addConstant("Unicode_3_0",              new QoreBigIntNode(QChar::Unicode_3_0));
   qchar->addConstant("Unicode_3_1",              new QoreBigIntNode(QChar::Unicode_3_1));
   qchar->addConstant("Unicode_3_2",              new QoreBigIntNode(QChar::Unicode_3_2));
   qchar->addConstant("Unicode_4_0",              new QoreBigIntNode(QChar::Unicode_4_0));
   qchar->addConstant("Unicode_4_1",              new QoreBigIntNode(QChar::Unicode_4_1));
   qchar->addConstant("Unicode_5_0",              new QoreBigIntNode(QChar::Unicode_5_0));

   qt_ns->addInitialNamespace(qchar);

   QoreNamespace *qcalendarwidget = new QoreNamespace("QCalendarWidget");

   qcalendarwidget->addSystemClass(initQCalendarWidgetClass(qwidget));

   // SelectionMode enum
   qcalendarwidget->addConstant("NoSelection",              new QoreBigIntNode(QCalendarWidget::NoSelection));
   qcalendarwidget->addConstant("SingleSelection",          new QoreBigIntNode(QCalendarWidget::SingleSelection));

   // HorizontalHeaderFormat enum
   qcalendarwidget->addConstant("NoHorizontalHeader",       new QoreBigIntNode(QCalendarWidget::NoHorizontalHeader));
   qcalendarwidget->addConstant("SingleLetterDayNames",     new QoreBigIntNode(QCalendarWidget::SingleLetterDayNames));
   qcalendarwidget->addConstant("ShortDayNames",            new QoreBigIntNode(QCalendarWidget::ShortDayNames));
   qcalendarwidget->addConstant("LongDayNames",             new QoreBigIntNode(QCalendarWidget::LongDayNames));

   // VeritcalHeaderFormat enum
   qcalendarwidget->addConstant("NoVerticalHeader",         new QoreBigIntNode(QCalendarWidget::NoVerticalHeader));
   qcalendarwidget->addConstant("ISOWeekNumbers",           new QoreBigIntNode(QCalendarWidget::ISOWeekNumbers));

   qt_ns->addInitialNamespace(qcalendarwidget);

   QoreNamespace *qlocale = new QoreNamespace("QLocale");
   qlocale->addSystemClass(initQLocaleClass());

   // Language enum
   qlocale->addConstant("C",                        new QoreBigIntNode(QLocale::C));
   qlocale->addConstant("Abkhazian",                new QoreBigIntNode(QLocale::Abkhazian));
   qlocale->addConstant("Afan",                     new QoreBigIntNode(QLocale::Afan));
   qlocale->addConstant("Afar",                     new QoreBigIntNode(QLocale::Afar));
   qlocale->addConstant("Afrikaans",                new QoreBigIntNode(QLocale::Afrikaans));
   qlocale->addConstant("Albanian",                 new QoreBigIntNode(QLocale::Albanian));
   qlocale->addConstant("Amharic",                  new QoreBigIntNode(QLocale::Amharic));
   qlocale->addConstant("Arabic",                   new QoreBigIntNode(QLocale::Arabic));
   qlocale->addConstant("Armenian",                 new QoreBigIntNode(QLocale::Armenian));
   qlocale->addConstant("Assamese",                 new QoreBigIntNode(QLocale::Assamese));
   qlocale->addConstant("Aymara",                   new QoreBigIntNode(QLocale::Aymara));
   qlocale->addConstant("Azerbaijani",              new QoreBigIntNode(QLocale::Azerbaijani));
   qlocale->addConstant("Bashkir",                  new QoreBigIntNode(QLocale::Bashkir));
   qlocale->addConstant("Basque",                   new QoreBigIntNode(QLocale::Basque));
   qlocale->addConstant("Bengali",                  new QoreBigIntNode(QLocale::Bengali));
   qlocale->addConstant("Bhutani",                  new QoreBigIntNode(QLocale::Bhutani));
   qlocale->addConstant("Bihari",                   new QoreBigIntNode(QLocale::Bihari));
   qlocale->addConstant("Bislama",                  new QoreBigIntNode(QLocale::Bislama));
   qlocale->addConstant("Breton",                   new QoreBigIntNode(QLocale::Breton));
   qlocale->addConstant("Bulgarian",                new QoreBigIntNode(QLocale::Bulgarian));
   qlocale->addConstant("Burmese",                  new QoreBigIntNode(QLocale::Burmese));
   qlocale->addConstant("Byelorussian",             new QoreBigIntNode(QLocale::Byelorussian));
   qlocale->addConstant("Cambodian",                new QoreBigIntNode(QLocale::Cambodian));
   qlocale->addConstant("Catalan",                  new QoreBigIntNode(QLocale::Catalan));
   qlocale->addConstant("Chinese",                  new QoreBigIntNode(QLocale::Chinese));
   qlocale->addConstant("Corsican",                 new QoreBigIntNode(QLocale::Corsican));
   qlocale->addConstant("Croatian",                 new QoreBigIntNode(QLocale::Croatian));
   qlocale->addConstant("Czech",                    new QoreBigIntNode(QLocale::Czech));
   qlocale->addConstant("Danish",                   new QoreBigIntNode(QLocale::Danish));
   qlocale->addConstant("Dutch",                    new QoreBigIntNode(QLocale::Dutch));
   qlocale->addConstant("English",                  new QoreBigIntNode(QLocale::English));
   qlocale->addConstant("Esperanto",                new QoreBigIntNode(QLocale::Esperanto));
   qlocale->addConstant("Estonian",                 new QoreBigIntNode(QLocale::Estonian));
   qlocale->addConstant("Faroese",                  new QoreBigIntNode(QLocale::Faroese));
   qlocale->addConstant("FijiLanguage",             new QoreBigIntNode(QLocale::FijiLanguage));
   qlocale->addConstant("Finnish",                  new QoreBigIntNode(QLocale::Finnish));
   qlocale->addConstant("French",                   new QoreBigIntNode(QLocale::French));
   qlocale->addConstant("Frisian",                  new QoreBigIntNode(QLocale::Frisian));
   qlocale->addConstant("Gaelic",                   new QoreBigIntNode(QLocale::Gaelic));
   qlocale->addConstant("Galician",                 new QoreBigIntNode(QLocale::Galician));
   qlocale->addConstant("Georgian",                 new QoreBigIntNode(QLocale::Georgian));
   qlocale->addConstant("German",                   new QoreBigIntNode(QLocale::German));
   qlocale->addConstant("Greek",                    new QoreBigIntNode(QLocale::Greek));
   qlocale->addConstant("Greenlandic",              new QoreBigIntNode(QLocale::Greenlandic));
   qlocale->addConstant("Guarani",                  new QoreBigIntNode(QLocale::Guarani));
   qlocale->addConstant("Gujarati",                 new QoreBigIntNode(QLocale::Gujarati));
   qlocale->addConstant("Hausa",                    new QoreBigIntNode(QLocale::Hausa));
   qlocale->addConstant("Hebrew",                   new QoreBigIntNode(QLocale::Hebrew));
   qlocale->addConstant("Hindi",                    new QoreBigIntNode(QLocale::Hindi));
   qlocale->addConstant("Hungarian",                new QoreBigIntNode(QLocale::Hungarian));
   qlocale->addConstant("Icelandic",                new QoreBigIntNode(QLocale::Icelandic));
   qlocale->addConstant("Indonesian",               new QoreBigIntNode(QLocale::Indonesian));
   qlocale->addConstant("Interlingua",              new QoreBigIntNode(QLocale::Interlingua));
   qlocale->addConstant("Interlingue",              new QoreBigIntNode(QLocale::Interlingue));
   qlocale->addConstant("Inuktitut",                new QoreBigIntNode(QLocale::Inuktitut));
   qlocale->addConstant("Inupiak",                  new QoreBigIntNode(QLocale::Inupiak));
   qlocale->addConstant("Irish",                    new QoreBigIntNode(QLocale::Irish));
   qlocale->addConstant("Italian",                  new QoreBigIntNode(QLocale::Italian));
   qlocale->addConstant("Japanese",                 new QoreBigIntNode(QLocale::Japanese));
   qlocale->addConstant("Javanese",                 new QoreBigIntNode(QLocale::Javanese));
   qlocale->addConstant("Kannada",                  new QoreBigIntNode(QLocale::Kannada));
   qlocale->addConstant("Kashmiri",                 new QoreBigIntNode(QLocale::Kashmiri));
   qlocale->addConstant("Kazakh",                   new QoreBigIntNode(QLocale::Kazakh));
   qlocale->addConstant("Kinyarwanda",              new QoreBigIntNode(QLocale::Kinyarwanda));
   qlocale->addConstant("Kirghiz",                  new QoreBigIntNode(QLocale::Kirghiz));
   qlocale->addConstant("Korean",                   new QoreBigIntNode(QLocale::Korean));
   qlocale->addConstant("Kurdish",                  new QoreBigIntNode(QLocale::Kurdish));
   qlocale->addConstant("Kurundi",                  new QoreBigIntNode(QLocale::Kurundi));
   qlocale->addConstant("Laothian",                 new QoreBigIntNode(QLocale::Laothian));
   qlocale->addConstant("Latin",                    new QoreBigIntNode(QLocale::Latin));
   qlocale->addConstant("Latvian",                  new QoreBigIntNode(QLocale::Latvian));
   qlocale->addConstant("Lingala",                  new QoreBigIntNode(QLocale::Lingala));
   qlocale->addConstant("Lithuanian",               new QoreBigIntNode(QLocale::Lithuanian));
   qlocale->addConstant("Macedonian",               new QoreBigIntNode(QLocale::Macedonian));
   qlocale->addConstant("Malagasy",                 new QoreBigIntNode(QLocale::Malagasy));
   qlocale->addConstant("Malay",                    new QoreBigIntNode(QLocale::Malay));
   qlocale->addConstant("Malayalam",                new QoreBigIntNode(QLocale::Malayalam));
   qlocale->addConstant("Maltese",                  new QoreBigIntNode(QLocale::Maltese));
   qlocale->addConstant("Maori",                    new QoreBigIntNode(QLocale::Maori));
   qlocale->addConstant("Marathi",                  new QoreBigIntNode(QLocale::Marathi));
   qlocale->addConstant("Moldavian",                new QoreBigIntNode(QLocale::Moldavian));
   qlocale->addConstant("Mongolian",                new QoreBigIntNode(QLocale::Mongolian));
   qlocale->addConstant("NauruLanguage",            new QoreBigIntNode(QLocale::NauruLanguage));
   qlocale->addConstant("Nepali",                   new QoreBigIntNode(QLocale::Nepali));
   qlocale->addConstant("Norwegian",                new QoreBigIntNode(QLocale::Norwegian));
   qlocale->addConstant("NorwegianBokmal",          new QoreBigIntNode(QLocale::NorwegianBokmal));
   qlocale->addConstant("Occitan",                  new QoreBigIntNode(QLocale::Occitan));
   qlocale->addConstant("Oriya",                    new QoreBigIntNode(QLocale::Oriya));
   qlocale->addConstant("Pashto",                   new QoreBigIntNode(QLocale::Pashto));
   qlocale->addConstant("Persian",                  new QoreBigIntNode(QLocale::Persian));
   qlocale->addConstant("Polish",                   new QoreBigIntNode(QLocale::Polish));
   qlocale->addConstant("Portuguese",               new QoreBigIntNode(QLocale::Portuguese));
   qlocale->addConstant("Punjabi",                  new QoreBigIntNode(QLocale::Punjabi));
   qlocale->addConstant("Quechua",                  new QoreBigIntNode(QLocale::Quechua));
   qlocale->addConstant("RhaetoRomance",            new QoreBigIntNode(QLocale::RhaetoRomance));
   qlocale->addConstant("Romanian",                 new QoreBigIntNode(QLocale::Romanian));
   qlocale->addConstant("Russian",                  new QoreBigIntNode(QLocale::Russian));
   qlocale->addConstant("Samoan",                   new QoreBigIntNode(QLocale::Samoan));
   qlocale->addConstant("Sangho",                   new QoreBigIntNode(QLocale::Sangho));
   qlocale->addConstant("Sanskrit",                 new QoreBigIntNode(QLocale::Sanskrit));
   qlocale->addConstant("Serbian",                  new QoreBigIntNode(QLocale::Serbian));
   qlocale->addConstant("SerboCroatian",            new QoreBigIntNode(QLocale::SerboCroatian));
   qlocale->addConstant("Sesotho",                  new QoreBigIntNode(QLocale::Sesotho));
   qlocale->addConstant("Setswana",                 new QoreBigIntNode(QLocale::Setswana));
   qlocale->addConstant("Shona",                    new QoreBigIntNode(QLocale::Shona));
   qlocale->addConstant("Sindhi",                   new QoreBigIntNode(QLocale::Sindhi));
   qlocale->addConstant("Singhalese",               new QoreBigIntNode(QLocale::Singhalese));
   qlocale->addConstant("Siswati",                  new QoreBigIntNode(QLocale::Siswati));
   qlocale->addConstant("Slovak",                   new QoreBigIntNode(QLocale::Slovak));
   qlocale->addConstant("Slovenian",                new QoreBigIntNode(QLocale::Slovenian));
   qlocale->addConstant("Somali",                   new QoreBigIntNode(QLocale::Somali));
   qlocale->addConstant("Spanish",                  new QoreBigIntNode(QLocale::Spanish));
   qlocale->addConstant("Sundanese",                new QoreBigIntNode(QLocale::Sundanese));
   qlocale->addConstant("Swahili",                  new QoreBigIntNode(QLocale::Swahili));
   qlocale->addConstant("Swedish",                  new QoreBigIntNode(QLocale::Swedish));
   qlocale->addConstant("Tagalog",                  new QoreBigIntNode(QLocale::Tagalog));
   qlocale->addConstant("Tajik",                    new QoreBigIntNode(QLocale::Tajik));
   qlocale->addConstant("Tamil",                    new QoreBigIntNode(QLocale::Tamil));
   qlocale->addConstant("Tatar",                    new QoreBigIntNode(QLocale::Tatar));
   qlocale->addConstant("Telugu",                   new QoreBigIntNode(QLocale::Telugu));
   qlocale->addConstant("Thai",                     new QoreBigIntNode(QLocale::Thai));
   qlocale->addConstant("Tibetan",                  new QoreBigIntNode(QLocale::Tibetan));
   qlocale->addConstant("Tigrinya",                 new QoreBigIntNode(QLocale::Tigrinya));
   qlocale->addConstant("TongaLanguage",            new QoreBigIntNode(QLocale::TongaLanguage));
   qlocale->addConstant("Tsonga",                   new QoreBigIntNode(QLocale::Tsonga));
   qlocale->addConstant("Turkish",                  new QoreBigIntNode(QLocale::Turkish));
   qlocale->addConstant("Turkmen",                  new QoreBigIntNode(QLocale::Turkmen));
   qlocale->addConstant("Twi",                      new QoreBigIntNode(QLocale::Twi));
   qlocale->addConstant("Uigur",                    new QoreBigIntNode(QLocale::Uigur));
   qlocale->addConstant("Ukrainian",                new QoreBigIntNode(QLocale::Ukrainian));
   qlocale->addConstant("Urdu",                     new QoreBigIntNode(QLocale::Urdu));
   qlocale->addConstant("Uzbek",                    new QoreBigIntNode(QLocale::Uzbek));
   qlocale->addConstant("Vietnamese",               new QoreBigIntNode(QLocale::Vietnamese));
   qlocale->addConstant("Volapuk",                  new QoreBigIntNode(QLocale::Volapuk));
   qlocale->addConstant("Welsh",                    new QoreBigIntNode(QLocale::Welsh));
   qlocale->addConstant("Wolof",                    new QoreBigIntNode(QLocale::Wolof));
   qlocale->addConstant("Xhosa",                    new QoreBigIntNode(QLocale::Xhosa));
   qlocale->addConstant("Yiddish",                  new QoreBigIntNode(QLocale::Yiddish));
   qlocale->addConstant("Yoruba",                   new QoreBigIntNode(QLocale::Yoruba));
   qlocale->addConstant("Zhuang",                   new QoreBigIntNode(QLocale::Zhuang));
   qlocale->addConstant("Zulu",                     new QoreBigIntNode(QLocale::Zulu));
   qlocale->addConstant("NorwegianNynorsk",         new QoreBigIntNode(QLocale::NorwegianNynorsk));
   qlocale->addConstant("Nynorsk",                  new QoreBigIntNode(QLocale::Nynorsk));
   qlocale->addConstant("Bosnian",                  new QoreBigIntNode(QLocale::Bosnian));
   qlocale->addConstant("Divehi",                   new QoreBigIntNode(QLocale::Divehi));
   qlocale->addConstant("Manx",                     new QoreBigIntNode(QLocale::Manx));
   qlocale->addConstant("Cornish",                  new QoreBigIntNode(QLocale::Cornish));
   qlocale->addConstant("Akan",                     new QoreBigIntNode(QLocale::Akan));
   qlocale->addConstant("Konkani",                  new QoreBigIntNode(QLocale::Konkani));
   qlocale->addConstant("Ga",                       new QoreBigIntNode(QLocale::Ga));
   qlocale->addConstant("Igbo",                     new QoreBigIntNode(QLocale::Igbo));
   qlocale->addConstant("Kamba",                    new QoreBigIntNode(QLocale::Kamba));
   qlocale->addConstant("Syriac",                   new QoreBigIntNode(QLocale::Syriac));
   qlocale->addConstant("Blin",                     new QoreBigIntNode(QLocale::Blin));
   qlocale->addConstant("Geez",                     new QoreBigIntNode(QLocale::Geez));
   qlocale->addConstant("Koro",                     new QoreBigIntNode(QLocale::Koro));
   qlocale->addConstant("Sidamo",                   new QoreBigIntNode(QLocale::Sidamo));
   qlocale->addConstant("Atsam",                    new QoreBigIntNode(QLocale::Atsam));
   qlocale->addConstant("Tigre",                    new QoreBigIntNode(QLocale::Tigre));
   qlocale->addConstant("Jju",                      new QoreBigIntNode(QLocale::Jju));
   qlocale->addConstant("Friulian",                 new QoreBigIntNode(QLocale::Friulian));
   qlocale->addConstant("Venda",                    new QoreBigIntNode(QLocale::Venda));
   qlocale->addConstant("Ewe",                      new QoreBigIntNode(QLocale::Ewe));
   qlocale->addConstant("Walamo",                   new QoreBigIntNode(QLocale::Walamo));
   qlocale->addConstant("Hawaiian",                 new QoreBigIntNode(QLocale::Hawaiian));
   qlocale->addConstant("Tyap",                     new QoreBigIntNode(QLocale::Tyap));
   qlocale->addConstant("Chewa",                    new QoreBigIntNode(QLocale::Chewa));
   qlocale->addConstant("LastLanguage",             new QoreBigIntNode(QLocale::LastLanguage));

   // Country enum
   qlocale->addConstant("AnyCountry",               new QoreBigIntNode(QLocale::AnyCountry));
   qlocale->addConstant("Afghanistan",              new QoreBigIntNode(QLocale::Afghanistan));
   qlocale->addConstant("Albania",                  new QoreBigIntNode(QLocale::Albania));
   qlocale->addConstant("Algeria",                  new QoreBigIntNode(QLocale::Algeria));
   qlocale->addConstant("AmericanSamoa",            new QoreBigIntNode(QLocale::AmericanSamoa));
   qlocale->addConstant("Andorra",                  new QoreBigIntNode(QLocale::Andorra));
   qlocale->addConstant("Angola",                   new QoreBigIntNode(QLocale::Angola));
   qlocale->addConstant("Anguilla",                 new QoreBigIntNode(QLocale::Anguilla));
   qlocale->addConstant("Antarctica",               new QoreBigIntNode(QLocale::Antarctica));
   qlocale->addConstant("AntiguaAndBarbuda",        new QoreBigIntNode(QLocale::AntiguaAndBarbuda));
   qlocale->addConstant("Argentina",                new QoreBigIntNode(QLocale::Argentina));
   qlocale->addConstant("Armenia",                  new QoreBigIntNode(QLocale::Armenia));
   qlocale->addConstant("Aruba",                    new QoreBigIntNode(QLocale::Aruba));
   qlocale->addConstant("Australia",                new QoreBigIntNode(QLocale::Australia));
   qlocale->addConstant("Austria",                  new QoreBigIntNode(QLocale::Austria));
   qlocale->addConstant("Azerbaijan",               new QoreBigIntNode(QLocale::Azerbaijan));
   qlocale->addConstant("Bahamas",                  new QoreBigIntNode(QLocale::Bahamas));
   qlocale->addConstant("Bahrain",                  new QoreBigIntNode(QLocale::Bahrain));
   qlocale->addConstant("Bangladesh",               new QoreBigIntNode(QLocale::Bangladesh));
   qlocale->addConstant("Barbados",                 new QoreBigIntNode(QLocale::Barbados));
   qlocale->addConstant("Belarus",                  new QoreBigIntNode(QLocale::Belarus));
   qlocale->addConstant("Belgium",                  new QoreBigIntNode(QLocale::Belgium));
   qlocale->addConstant("Belize",                   new QoreBigIntNode(QLocale::Belize));
   qlocale->addConstant("Benin",                    new QoreBigIntNode(QLocale::Benin));
   qlocale->addConstant("Bermuda",                  new QoreBigIntNode(QLocale::Bermuda));
   qlocale->addConstant("Bhutan",                   new QoreBigIntNode(QLocale::Bhutan));
   qlocale->addConstant("Bolivia",                  new QoreBigIntNode(QLocale::Bolivia));
   qlocale->addConstant("BosniaAndHerzegowina",     new QoreBigIntNode(QLocale::BosniaAndHerzegowina));
   qlocale->addConstant("Botswana",                 new QoreBigIntNode(QLocale::Botswana));
   qlocale->addConstant("BouvetIsland",             new QoreBigIntNode(QLocale::BouvetIsland));
   qlocale->addConstant("Brazil",                   new QoreBigIntNode(QLocale::Brazil));
   qlocale->addConstant("BritishIndianOceanTerritory", new QoreBigIntNode(QLocale::BritishIndianOceanTerritory));
   qlocale->addConstant("BruneiDarussalam",         new QoreBigIntNode(QLocale::BruneiDarussalam));
   qlocale->addConstant("Bulgaria",                 new QoreBigIntNode(QLocale::Bulgaria));
   qlocale->addConstant("BurkinaFaso",              new QoreBigIntNode(QLocale::BurkinaFaso));
   qlocale->addConstant("Burundi",                  new QoreBigIntNode(QLocale::Burundi));
   qlocale->addConstant("Cambodia",                 new QoreBigIntNode(QLocale::Cambodia));
   qlocale->addConstant("Cameroon",                 new QoreBigIntNode(QLocale::Cameroon));
   qlocale->addConstant("Canada",                   new QoreBigIntNode(QLocale::Canada));
   qlocale->addConstant("CapeVerde",                new QoreBigIntNode(QLocale::CapeVerde));
   qlocale->addConstant("CaymanIslands",            new QoreBigIntNode(QLocale::CaymanIslands));
   qlocale->addConstant("CentralAfricanRepublic",   new QoreBigIntNode(QLocale::CentralAfricanRepublic));
   qlocale->addConstant("Chad",                     new QoreBigIntNode(QLocale::Chad));
   qlocale->addConstant("Chile",                    new QoreBigIntNode(QLocale::Chile));
   qlocale->addConstant("China",                    new QoreBigIntNode(QLocale::China));
   qlocale->addConstant("ChristmasIsland",          new QoreBigIntNode(QLocale::ChristmasIsland));
   qlocale->addConstant("CocosIslands",             new QoreBigIntNode(QLocale::CocosIslands));
   qlocale->addConstant("Colombia",                 new QoreBigIntNode(QLocale::Colombia));
   qlocale->addConstant("Comoros",                  new QoreBigIntNode(QLocale::Comoros));
   qlocale->addConstant("DemocraticRepublicOfCongo", new QoreBigIntNode(QLocale::DemocraticRepublicOfCongo));
   qlocale->addConstant("PeoplesRepublicOfCongo",   new QoreBigIntNode(QLocale::PeoplesRepublicOfCongo));
   qlocale->addConstant("CookIslands",              new QoreBigIntNode(QLocale::CookIslands));
   qlocale->addConstant("CostaRica",                new QoreBigIntNode(QLocale::CostaRica));
   qlocale->addConstant("IvoryCoast",               new QoreBigIntNode(QLocale::IvoryCoast));
   qlocale->addConstant("Croatia",                  new QoreBigIntNode(QLocale::Croatia));
   qlocale->addConstant("Cuba",                     new QoreBigIntNode(QLocale::Cuba));
   qlocale->addConstant("Cyprus",                   new QoreBigIntNode(QLocale::Cyprus));
   qlocale->addConstant("CzechRepublic",            new QoreBigIntNode(QLocale::CzechRepublic));
   qlocale->addConstant("Denmark",                  new QoreBigIntNode(QLocale::Denmark));
   qlocale->addConstant("Djibouti",                 new QoreBigIntNode(QLocale::Djibouti));
   qlocale->addConstant("Dominica",                 new QoreBigIntNode(QLocale::Dominica));
   qlocale->addConstant("DominicanRepublic",        new QoreBigIntNode(QLocale::DominicanRepublic));
   qlocale->addConstant("EastTimor",                new QoreBigIntNode(QLocale::EastTimor));
   qlocale->addConstant("Ecuador",                  new QoreBigIntNode(QLocale::Ecuador));
   qlocale->addConstant("Egypt",                    new QoreBigIntNode(QLocale::Egypt));
   qlocale->addConstant("ElSalvador",               new QoreBigIntNode(QLocale::ElSalvador));
   qlocale->addConstant("EquatorialGuinea",         new QoreBigIntNode(QLocale::EquatorialGuinea));
   qlocale->addConstant("Eritrea",                  new QoreBigIntNode(QLocale::Eritrea));
   qlocale->addConstant("Estonia",                  new QoreBigIntNode(QLocale::Estonia));
   qlocale->addConstant("Ethiopia",                 new QoreBigIntNode(QLocale::Ethiopia));
   qlocale->addConstant("FalklandIslands",          new QoreBigIntNode(QLocale::FalklandIslands));
   qlocale->addConstant("FaroeIslands",             new QoreBigIntNode(QLocale::FaroeIslands));
   qlocale->addConstant("FijiCountry",              new QoreBigIntNode(QLocale::FijiCountry));
   qlocale->addConstant("Finland",                  new QoreBigIntNode(QLocale::Finland));
   qlocale->addConstant("France",                   new QoreBigIntNode(QLocale::France));
   qlocale->addConstant("MetropolitanFrance",       new QoreBigIntNode(QLocale::MetropolitanFrance));
   qlocale->addConstant("FrenchGuiana",             new QoreBigIntNode(QLocale::FrenchGuiana));
   qlocale->addConstant("FrenchPolynesia",          new QoreBigIntNode(QLocale::FrenchPolynesia));
   qlocale->addConstant("FrenchSouthernTerritories", new QoreBigIntNode(QLocale::FrenchSouthernTerritories));
   qlocale->addConstant("Gabon",                    new QoreBigIntNode(QLocale::Gabon));
   qlocale->addConstant("Gambia",                   new QoreBigIntNode(QLocale::Gambia));
   qlocale->addConstant("Georgia",                  new QoreBigIntNode(QLocale::Georgia));
   qlocale->addConstant("Germany",                  new QoreBigIntNode(QLocale::Germany));
   qlocale->addConstant("Ghana",                    new QoreBigIntNode(QLocale::Ghana));
   qlocale->addConstant("Gibraltar",                new QoreBigIntNode(QLocale::Gibraltar));
   qlocale->addConstant("Greece",                   new QoreBigIntNode(QLocale::Greece));
   qlocale->addConstant("Greenland",                new QoreBigIntNode(QLocale::Greenland));
   qlocale->addConstant("Grenada",                  new QoreBigIntNode(QLocale::Grenada));
   qlocale->addConstant("Guadeloupe",               new QoreBigIntNode(QLocale::Guadeloupe));
   qlocale->addConstant("Guam",                     new QoreBigIntNode(QLocale::Guam));
   qlocale->addConstant("Guatemala",                new QoreBigIntNode(QLocale::Guatemala));
   qlocale->addConstant("Guinea",                   new QoreBigIntNode(QLocale::Guinea));
   qlocale->addConstant("GuineaBissau",             new QoreBigIntNode(QLocale::GuineaBissau));
   qlocale->addConstant("Guyana",                   new QoreBigIntNode(QLocale::Guyana));
   qlocale->addConstant("Haiti",                    new QoreBigIntNode(QLocale::Haiti));
   qlocale->addConstant("HeardAndMcDonaldIslands",  new QoreBigIntNode(QLocale::HeardAndMcDonaldIslands));
   qlocale->addConstant("Honduras",                 new QoreBigIntNode(QLocale::Honduras));
   qlocale->addConstant("HongKong",                 new QoreBigIntNode(QLocale::HongKong));
   qlocale->addConstant("Hungary",                  new QoreBigIntNode(QLocale::Hungary));
   qlocale->addConstant("Iceland",                  new QoreBigIntNode(QLocale::Iceland));
   qlocale->addConstant("India",                    new QoreBigIntNode(QLocale::India));
   qlocale->addConstant("Indonesia",                new QoreBigIntNode(QLocale::Indonesia));
   qlocale->addConstant("Iran",                     new QoreBigIntNode(QLocale::Iran));
   qlocale->addConstant("Iraq",                     new QoreBigIntNode(QLocale::Iraq));
   qlocale->addConstant("Ireland",                  new QoreBigIntNode(QLocale::Ireland));
   qlocale->addConstant("Israel",                   new QoreBigIntNode(QLocale::Israel));
   qlocale->addConstant("Italy",                    new QoreBigIntNode(QLocale::Italy));
   qlocale->addConstant("Jamaica",                  new QoreBigIntNode(QLocale::Jamaica));
   qlocale->addConstant("Japan",                    new QoreBigIntNode(QLocale::Japan));
   qlocale->addConstant("Jordan",                   new QoreBigIntNode(QLocale::Jordan));
   qlocale->addConstant("Kazakhstan",               new QoreBigIntNode(QLocale::Kazakhstan));
   qlocale->addConstant("Kenya",                    new QoreBigIntNode(QLocale::Kenya));
   qlocale->addConstant("Kiribati",                 new QoreBigIntNode(QLocale::Kiribati));
   qlocale->addConstant("DemocraticRepublicOfKorea", new QoreBigIntNode(QLocale::DemocraticRepublicOfKorea));
   qlocale->addConstant("RepublicOfKorea",          new QoreBigIntNode(QLocale::RepublicOfKorea));
   qlocale->addConstant("Kuwait",                   new QoreBigIntNode(QLocale::Kuwait));
   qlocale->addConstant("Kyrgyzstan",               new QoreBigIntNode(QLocale::Kyrgyzstan));
   qlocale->addConstant("Lao",                      new QoreBigIntNode(QLocale::Lao));
   qlocale->addConstant("Latvia",                   new QoreBigIntNode(QLocale::Latvia));
   qlocale->addConstant("Lebanon",                  new QoreBigIntNode(QLocale::Lebanon));
   qlocale->addConstant("Lesotho",                  new QoreBigIntNode(QLocale::Lesotho));
   qlocale->addConstant("Liberia",                  new QoreBigIntNode(QLocale::Liberia));
   qlocale->addConstant("LibyanArabJamahiriya",     new QoreBigIntNode(QLocale::LibyanArabJamahiriya));
   qlocale->addConstant("Liechtenstein",            new QoreBigIntNode(QLocale::Liechtenstein));
   qlocale->addConstant("Lithuania",                new QoreBigIntNode(QLocale::Lithuania));
   qlocale->addConstant("Luxembourg",               new QoreBigIntNode(QLocale::Luxembourg));
   qlocale->addConstant("Macau",                    new QoreBigIntNode(QLocale::Macau));
   qlocale->addConstant("Macedonia",                new QoreBigIntNode(QLocale::Macedonia));
   qlocale->addConstant("Madagascar",               new QoreBigIntNode(QLocale::Madagascar));
   qlocale->addConstant("Malawi",                   new QoreBigIntNode(QLocale::Malawi));
   qlocale->addConstant("Malaysia",                 new QoreBigIntNode(QLocale::Malaysia));
   qlocale->addConstant("Maldives",                 new QoreBigIntNode(QLocale::Maldives));
   qlocale->addConstant("Mali",                     new QoreBigIntNode(QLocale::Mali));
   qlocale->addConstant("Malta",                    new QoreBigIntNode(QLocale::Malta));
   qlocale->addConstant("MarshallIslands",          new QoreBigIntNode(QLocale::MarshallIslands));
   qlocale->addConstant("Martinique",               new QoreBigIntNode(QLocale::Martinique));
   qlocale->addConstant("Mauritania",               new QoreBigIntNode(QLocale::Mauritania));
   qlocale->addConstant("Mauritius",                new QoreBigIntNode(QLocale::Mauritius));
   qlocale->addConstant("Mayotte",                  new QoreBigIntNode(QLocale::Mayotte));
   qlocale->addConstant("Mexico",                   new QoreBigIntNode(QLocale::Mexico));
   qlocale->addConstant("Micronesia",               new QoreBigIntNode(QLocale::Micronesia));
   qlocale->addConstant("Moldova",                  new QoreBigIntNode(QLocale::Moldova));
   qlocale->addConstant("Monaco",                   new QoreBigIntNode(QLocale::Monaco));
   qlocale->addConstant("Mongolia",                 new QoreBigIntNode(QLocale::Mongolia));
   qlocale->addConstant("Montserrat",               new QoreBigIntNode(QLocale::Montserrat));
   qlocale->addConstant("Morocco",                  new QoreBigIntNode(QLocale::Morocco));
   qlocale->addConstant("Mozambique",               new QoreBigIntNode(QLocale::Mozambique));
   qlocale->addConstant("Myanmar",                  new QoreBigIntNode(QLocale::Myanmar));
   qlocale->addConstant("Namibia",                  new QoreBigIntNode(QLocale::Namibia));
   qlocale->addConstant("NauruCountry",             new QoreBigIntNode(QLocale::NauruCountry));
   qlocale->addConstant("Nepal",                    new QoreBigIntNode(QLocale::Nepal));
   qlocale->addConstant("Netherlands",              new QoreBigIntNode(QLocale::Netherlands));
   qlocale->addConstant("NetherlandsAntilles",      new QoreBigIntNode(QLocale::NetherlandsAntilles));
   qlocale->addConstant("NewCaledonia",             new QoreBigIntNode(QLocale::NewCaledonia));
   qlocale->addConstant("NewZealand",               new QoreBigIntNode(QLocale::NewZealand));
   qlocale->addConstant("Nicaragua",                new QoreBigIntNode(QLocale::Nicaragua));
   qlocale->addConstant("Niger",                    new QoreBigIntNode(QLocale::Niger));
   qlocale->addConstant("Nigeria",                  new QoreBigIntNode(QLocale::Nigeria));
   qlocale->addConstant("Niue",                     new QoreBigIntNode(QLocale::Niue));
   qlocale->addConstant("NorfolkIsland",            new QoreBigIntNode(QLocale::NorfolkIsland));
   qlocale->addConstant("NorthernMarianaIslands",   new QoreBigIntNode(QLocale::NorthernMarianaIslands));
   qlocale->addConstant("Norway",                   new QoreBigIntNode(QLocale::Norway));
   qlocale->addConstant("Oman",                     new QoreBigIntNode(QLocale::Oman));
   qlocale->addConstant("Pakistan",                 new QoreBigIntNode(QLocale::Pakistan));
   qlocale->addConstant("Palau",                    new QoreBigIntNode(QLocale::Palau));
   qlocale->addConstant("PalestinianTerritory",     new QoreBigIntNode(QLocale::PalestinianTerritory));
   qlocale->addConstant("Panama",                   new QoreBigIntNode(QLocale::Panama));
   qlocale->addConstant("PapuaNewGuinea",           new QoreBigIntNode(QLocale::PapuaNewGuinea));
   qlocale->addConstant("Paraguay",                 new QoreBigIntNode(QLocale::Paraguay));
   qlocale->addConstant("Peru",                     new QoreBigIntNode(QLocale::Peru));
   qlocale->addConstant("Philippines",              new QoreBigIntNode(QLocale::Philippines));
   qlocale->addConstant("Pitcairn",                 new QoreBigIntNode(QLocale::Pitcairn));
   qlocale->addConstant("Poland",                   new QoreBigIntNode(QLocale::Poland));
   qlocale->addConstant("Portugal",                 new QoreBigIntNode(QLocale::Portugal));
   qlocale->addConstant("PuertoRico",               new QoreBigIntNode(QLocale::PuertoRico));
   qlocale->addConstant("Qatar",                    new QoreBigIntNode(QLocale::Qatar));
   qlocale->addConstant("Reunion",                  new QoreBigIntNode(QLocale::Reunion));
   qlocale->addConstant("Romania",                  new QoreBigIntNode(QLocale::Romania));
   qlocale->addConstant("RussianFederation",        new QoreBigIntNode(QLocale::RussianFederation));
   qlocale->addConstant("Rwanda",                   new QoreBigIntNode(QLocale::Rwanda));
   qlocale->addConstant("SaintKittsAndNevis",       new QoreBigIntNode(QLocale::SaintKittsAndNevis));
   qlocale->addConstant("StLucia",                  new QoreBigIntNode(QLocale::StLucia));
   qlocale->addConstant("StVincentAndTheGrenadines", new QoreBigIntNode(QLocale::StVincentAndTheGrenadines));
   qlocale->addConstant("Samoa",                    new QoreBigIntNode(QLocale::Samoa));
   qlocale->addConstant("SanMarino",                new QoreBigIntNode(QLocale::SanMarino));
   qlocale->addConstant("SaoTomeAndPrincipe",       new QoreBigIntNode(QLocale::SaoTomeAndPrincipe));
   qlocale->addConstant("SaudiArabia",              new QoreBigIntNode(QLocale::SaudiArabia));
   qlocale->addConstant("Senegal",                  new QoreBigIntNode(QLocale::Senegal));
   qlocale->addConstant("Seychelles",               new QoreBigIntNode(QLocale::Seychelles));
   qlocale->addConstant("SierraLeone",              new QoreBigIntNode(QLocale::SierraLeone));
   qlocale->addConstant("Singapore",                new QoreBigIntNode(QLocale::Singapore));
   qlocale->addConstant("Slovakia",                 new QoreBigIntNode(QLocale::Slovakia));
   qlocale->addConstant("Slovenia",                 new QoreBigIntNode(QLocale::Slovenia));
   qlocale->addConstant("SolomonIslands",           new QoreBigIntNode(QLocale::SolomonIslands));
   qlocale->addConstant("Somalia",                  new QoreBigIntNode(QLocale::Somalia));
   qlocale->addConstant("SouthAfrica",              new QoreBigIntNode(QLocale::SouthAfrica));
   qlocale->addConstant("SouthGeorgiaAndTheSouthSandwichIslands", new QoreBigIntNode(QLocale::SouthGeorgiaAndTheSouthSandwichIslands));
   qlocale->addConstant("Spain",                    new QoreBigIntNode(QLocale::Spain));
   qlocale->addConstant("SriLanka",                 new QoreBigIntNode(QLocale::SriLanka));
   qlocale->addConstant("StHelena",                 new QoreBigIntNode(QLocale::StHelena));
   qlocale->addConstant("StPierreAndMiquelon",      new QoreBigIntNode(QLocale::StPierreAndMiquelon));
   qlocale->addConstant("Sudan",                    new QoreBigIntNode(QLocale::Sudan));
   qlocale->addConstant("Suriname",                 new QoreBigIntNode(QLocale::Suriname));
   qlocale->addConstant("SvalbardAndJanMayenIslands", new QoreBigIntNode(QLocale::SvalbardAndJanMayenIslands));
   qlocale->addConstant("Swaziland",                new QoreBigIntNode(QLocale::Swaziland));
   qlocale->addConstant("Sweden",                   new QoreBigIntNode(QLocale::Sweden));
   qlocale->addConstant("Switzerland",              new QoreBigIntNode(QLocale::Switzerland));
   qlocale->addConstant("SyrianArabRepublic",       new QoreBigIntNode(QLocale::SyrianArabRepublic));
   qlocale->addConstant("Taiwan",                   new QoreBigIntNode(QLocale::Taiwan));
   qlocale->addConstant("Tajikistan",               new QoreBigIntNode(QLocale::Tajikistan));
   qlocale->addConstant("Tanzania",                 new QoreBigIntNode(QLocale::Tanzania));
   qlocale->addConstant("Thailand",                 new QoreBigIntNode(QLocale::Thailand));
   qlocale->addConstant("Togo",                     new QoreBigIntNode(QLocale::Togo));
   qlocale->addConstant("Tokelau",                  new QoreBigIntNode(QLocale::Tokelau));
   qlocale->addConstant("TongaCountry",             new QoreBigIntNode(QLocale::TongaCountry));
   qlocale->addConstant("TrinidadAndTobago",        new QoreBigIntNode(QLocale::TrinidadAndTobago));
   qlocale->addConstant("Tunisia",                  new QoreBigIntNode(QLocale::Tunisia));
   qlocale->addConstant("Turkey",                   new QoreBigIntNode(QLocale::Turkey));
   qlocale->addConstant("Turkmenistan",             new QoreBigIntNode(QLocale::Turkmenistan));
   qlocale->addConstant("TurksAndCaicosIslands",    new QoreBigIntNode(QLocale::TurksAndCaicosIslands));
   qlocale->addConstant("Tuvalu",                   new QoreBigIntNode(QLocale::Tuvalu));
   qlocale->addConstant("Uganda",                   new QoreBigIntNode(QLocale::Uganda));
   qlocale->addConstant("Ukraine",                  new QoreBigIntNode(QLocale::Ukraine));
   qlocale->addConstant("UnitedArabEmirates",       new QoreBigIntNode(QLocale::UnitedArabEmirates));
   qlocale->addConstant("UnitedKingdom",            new QoreBigIntNode(QLocale::UnitedKingdom));
   qlocale->addConstant("UnitedStates",             new QoreBigIntNode(QLocale::UnitedStates));
   qlocale->addConstant("UnitedStatesMinorOutlyingIslands", new QoreBigIntNode(QLocale::UnitedStatesMinorOutlyingIslands));
   qlocale->addConstant("Uruguay",                  new QoreBigIntNode(QLocale::Uruguay));
   qlocale->addConstant("Uzbekistan",               new QoreBigIntNode(QLocale::Uzbekistan));
   qlocale->addConstant("Vanuatu",                  new QoreBigIntNode(QLocale::Vanuatu));
   qlocale->addConstant("VaticanCityState",         new QoreBigIntNode(QLocale::VaticanCityState));
   qlocale->addConstant("Venezuela",                new QoreBigIntNode(QLocale::Venezuela));
   qlocale->addConstant("VietNam",                  new QoreBigIntNode(QLocale::VietNam));
   qlocale->addConstant("BritishVirginIslands",     new QoreBigIntNode(QLocale::BritishVirginIslands));
   qlocale->addConstant("USVirginIslands",          new QoreBigIntNode(QLocale::USVirginIslands));
   qlocale->addConstant("WallisAndFutunaIslands",   new QoreBigIntNode(QLocale::WallisAndFutunaIslands));
   qlocale->addConstant("WesternSahara",            new QoreBigIntNode(QLocale::WesternSahara));
   qlocale->addConstant("Yemen",                    new QoreBigIntNode(QLocale::Yemen));
   qlocale->addConstant("Yugoslavia",               new QoreBigIntNode(QLocale::Yugoslavia));
   qlocale->addConstant("Zambia",                   new QoreBigIntNode(QLocale::Zambia));
   qlocale->addConstant("Zimbabwe",                 new QoreBigIntNode(QLocale::Zimbabwe));
   qlocale->addConstant("SerbiaAndMontenegro",      new QoreBigIntNode(QLocale::SerbiaAndMontenegro));
   qlocale->addConstant("LastCountry",              new QoreBigIntNode(QLocale::LastCountry));

   qt_ns->addInitialNamespace(qlocale);

   // add QFont namespaces and constants
   class QoreNamespace *qframens = new QoreNamespace("QFrame");
   // Shadow enum
   qframens->addConstant("Plain",    new QoreBigIntNode(QFrame::Plain));
   qframens->addConstant("Raised",   new QoreBigIntNode(QFrame::Raised));
   qframens->addConstant("Sunken",   new QoreBigIntNode(QFrame::Sunken));

   // Shape enum
   qframens->addConstant("NoFrame",      new QoreBigIntNode(QFrame::NoFrame));
   qframens->addConstant("Box",          new QoreBigIntNode(QFrame::Box));
   qframens->addConstant("Panel",        new QoreBigIntNode(QFrame::Panel));
   qframens->addConstant("StyledPanel",  new QoreBigIntNode(QFrame::StyledPanel));
   qframens->addConstant("HLine",        new QoreBigIntNode(QFrame::HLine));
   qframens->addConstant("VLine",        new QoreBigIntNode(QFrame::VLine));
   qframens->addConstant("WinPanel",     new QoreBigIntNode(QFrame::WinPanel));

   // StyleMask
   qframens->addConstant("Shadow_Mask",  new QoreBigIntNode(QFrame::Shadow_Mask));
   qframens->addConstant("Shape_Mask",   new QoreBigIntNode(QFrame::Shape_Mask));

   qt_ns->addInitialNamespace(qframens);

   // add QFont namespaces and constants
   class QoreNamespace *qf = new QoreNamespace("QFont");
   // Weight enum
   qf->addConstant("Light",    new QoreBigIntNode(QFont::Light));
   qf->addConstant("Normal",   new QoreBigIntNode(QFont::Normal));
   qf->addConstant("DemiBold", new QoreBigIntNode(QFont::DemiBold));
   qf->addConstant("Bold",     new QoreBigIntNode(QFont::Bold));
   qf->addConstant("Black",    new QoreBigIntNode(QFont::Black));

   // StyleHint enum
   qf->addConstant("Helvetica",    new QoreBigIntNode(QFont::Helvetica));
   qf->addConstant("SansSerif",    new QoreBigIntNode(QFont::SansSerif));
   qf->addConstant("Times",        new QoreBigIntNode(QFont::Times));
   qf->addConstant("Serif",        new QoreBigIntNode(QFont::Serif));
   qf->addConstant("Courier",      new QoreBigIntNode(QFont::Courier));
   qf->addConstant("TypeWriter",   new QoreBigIntNode(QFont::TypeWriter));
   qf->addConstant("OldEnglish",   new QoreBigIntNode(QFont::OldEnglish));
   qf->addConstant("Decorative",   new QoreBigIntNode(QFont::Decorative));
   qf->addConstant("System",       new QoreBigIntNode(QFont::System));
   qf->addConstant("AnyStyle",     new QoreBigIntNode(QFont::AnyStyle));

   // StyleStrategy
   qf->addConstant("PreferDefault",    new QoreBigIntNode(QFont::PreferDefault));
   qf->addConstant("PreferBitmap",     new QoreBigIntNode(QFont::PreferBitmap));
   qf->addConstant("PreferDevice",     new QoreBigIntNode(QFont::PreferDevice));
   qf->addConstant("PreferOutline",    new QoreBigIntNode(QFont::PreferOutline));
   qf->addConstant("ForceOutline",     new QoreBigIntNode(QFont::ForceOutline));
   qf->addConstant("PreferMatch",      new QoreBigIntNode(QFont::PreferMatch));
   qf->addConstant("PreferQuality",    new QoreBigIntNode(QFont::PreferQuality));
   qf->addConstant("PreferAntialias",  new QoreBigIntNode(QFont::PreferAntialias));
   qf->addConstant("NoAntialias",      new QoreBigIntNode(QFont::NoAntialias));
   qf->addConstant("OpenGLCompatible", new QoreBigIntNode(QFont::OpenGLCompatible));
   qf->addConstant("NoFontMerging",    new QoreBigIntNode(QFont::NoFontMerging));

   // Style enum
   qf->addConstant("StyleNormal",   new QoreBigIntNode(QFont::StyleNormal));
   qf->addConstant("StyleItalic",   new QoreBigIntNode(QFont::StyleItalic));
   qf->addConstant("StyleOblique",  new QoreBigIntNode(QFont::StyleOblique));

   // Stretch enum
   qf->addConstant("UltraCondensed",  new QoreBigIntNode(QFont::UltraCondensed));
   qf->addConstant("ExtraCondensed",  new QoreBigIntNode(QFont::ExtraCondensed));
   qf->addConstant("Condensed",       new QoreBigIntNode(QFont::Condensed));
   qf->addConstant("SemiCondensed",   new QoreBigIntNode(QFont::SemiCondensed));
   qf->addConstant("Unstretched",     new QoreBigIntNode(QFont::Unstretched));
   qf->addConstant("SemiExpanded",    new QoreBigIntNode(QFont::SemiExpanded));
   qf->addConstant("Expanded",        new QoreBigIntNode(QFont::Expanded));
   qf->addConstant("ExtraExpanded",   new QoreBigIntNode(QFont::ExtraExpanded));
   qf->addConstant("UltraExpanded",   new QoreBigIntNode(QFont::UltraExpanded));

   qt_ns->addInitialNamespace(qf);

   // add QLCDNumber namespace and constants
   class QoreNamespace *qlcdn = new QoreNamespace("QLCDNumber");
   qlcdn->addConstant("Outline",   new QoreBigIntNode(QLCDNumber::Outline));
   qlcdn->addConstant("Filled",    new QoreBigIntNode(QLCDNumber::Filled));
   qlcdn->addConstant("Flat",      new QoreBigIntNode(QLCDNumber::Flat));
   qlcdn->addConstant("Hex",       new QoreBigIntNode(QLCDNumber::Hex));
   qlcdn->addConstant("Dec",       new QoreBigIntNode(QLCDNumber::Dec));
   qlcdn->addConstant("Oct",       new QoreBigIntNode(QLCDNumber::Oct));
   qlcdn->addConstant("Bin",       new QoreBigIntNode(QLCDNumber::Bin));
   qt_ns->addInitialNamespace(qlcdn);

   // add QAbstractSlider namespace and constants
   class QoreNamespace *qas = new QoreNamespace("QAbstractSlider");
   qas->addConstant("SliderNoAction",        new QoreBigIntNode(QAbstractSlider::SliderNoAction));
   qas->addConstant("SliderSingleStepAdd",   new QoreBigIntNode(QAbstractSlider::SliderSingleStepAdd));
   qas->addConstant("SliderSingleStepSub",   new QoreBigIntNode(QAbstractSlider::SliderSingleStepSub));
   qas->addConstant("SliderPageStepAdd",     new QoreBigIntNode(QAbstractSlider::SliderPageStepAdd));
   qas->addConstant("SliderPageStepSub",     new QoreBigIntNode(QAbstractSlider::SliderPageStepSub));
   qas->addConstant("SliderToMinimum",       new QoreBigIntNode(QAbstractSlider::SliderToMinimum));
   qas->addConstant("SliderToMaximum",       new QoreBigIntNode(QAbstractSlider::SliderToMaximum));
   qas->addConstant("SliderMove",            new QoreBigIntNode(QAbstractSlider::SliderMove));
   qt_ns->addInitialNamespace(qas);

   // CheckState enum
   qt_ns->addConstant("Unchecked",                new QoreBigIntNode(Qt::Unchecked));
   qt_ns->addConstant("PartiallyChecked",         new QoreBigIntNode(Qt::PartiallyChecked));
   qt_ns->addConstant("Checked",                  new QoreBigIntNode(Qt::Checked));

   // orientation enum values
   qt_ns->addConstant("Vertical",        new QoreBigIntNode(Qt::Vertical));
   qt_ns->addConstant("Horizontal",      new QoreBigIntNode(Qt::Horizontal));

   // GlobalColor enum
   qt_ns->addConstant("color0",            new QoreBigIntNode(Qt::color0));
   qt_ns->addConstant("color1",            new QoreBigIntNode(Qt::color1));
   qt_ns->addConstant("black",             new QoreBigIntNode(Qt::black));
   qt_ns->addConstant("white",             new QoreBigIntNode(Qt::white));
   qt_ns->addConstant("darkGray",          new QoreBigIntNode(Qt::darkGray));
   qt_ns->addConstant("gray",              new QoreBigIntNode(Qt::gray));
   qt_ns->addConstant("lightGray",         new QoreBigIntNode(Qt::lightGray));
   qt_ns->addConstant("red",               new QoreBigIntNode(Qt::red));
   qt_ns->addConstant("green",             new QoreBigIntNode(Qt::green));
   qt_ns->addConstant("blue",              new QoreBigIntNode(Qt::blue));
   qt_ns->addConstant("cyan",              new QoreBigIntNode(Qt::cyan));
   qt_ns->addConstant("magenta",           new QoreBigIntNode(Qt::magenta));
   qt_ns->addConstant("yellow",            new QoreBigIntNode(Qt::yellow));
   qt_ns->addConstant("darkRed",           new QoreBigIntNode(Qt::darkRed));
   qt_ns->addConstant("darkGreen",         new QoreBigIntNode(Qt::darkGreen));
   qt_ns->addConstant("darkBlue",          new QoreBigIntNode(Qt::darkBlue));
   qt_ns->addConstant("darkCyan",          new QoreBigIntNode(Qt::darkCyan));
   qt_ns->addConstant("darkMagenta",       new QoreBigIntNode(Qt::darkMagenta));
   qt_ns->addConstant("darkYellow",        new QoreBigIntNode(Qt::darkYellow));
   qt_ns->addConstant("transparent",       new QoreBigIntNode(Qt::transparent));

   // BrushStyle enum
   qt_ns->addConstant("NoBrush",                  new BrushStyleNode(Qt::NoBrush));
   qt_ns->addConstant("SolidPattern",             new BrushStyleNode(Qt::SolidPattern));
   qt_ns->addConstant("Dense1Pattern",            new BrushStyleNode(Qt::Dense1Pattern));
   qt_ns->addConstant("Dense2Pattern",            new BrushStyleNode(Qt::Dense2Pattern));
   qt_ns->addConstant("Dense3Pattern",            new BrushStyleNode(Qt::Dense3Pattern));
   qt_ns->addConstant("Dense4Pattern",            new BrushStyleNode(Qt::Dense4Pattern));
   qt_ns->addConstant("Dense5Pattern",            new BrushStyleNode(Qt::Dense5Pattern));
   qt_ns->addConstant("Dense6Pattern",            new BrushStyleNode(Qt::Dense6Pattern));
   qt_ns->addConstant("Dense7Pattern",            new BrushStyleNode(Qt::Dense7Pattern));
   qt_ns->addConstant("HorPattern",               new BrushStyleNode(Qt::HorPattern));
   qt_ns->addConstant("VerPattern",               new BrushStyleNode(Qt::VerPattern));
   qt_ns->addConstant("CrossPattern",             new BrushStyleNode(Qt::CrossPattern));
   qt_ns->addConstant("BDiagPattern",             new BrushStyleNode(Qt::BDiagPattern));
   qt_ns->addConstant("FDiagPattern",             new BrushStyleNode(Qt::FDiagPattern));
   qt_ns->addConstant("DiagCrossPattern",         new BrushStyleNode(Qt::DiagCrossPattern));
   qt_ns->addConstant("LinearGradientPattern",    new BrushStyleNode(Qt::LinearGradientPattern));
   qt_ns->addConstant("RadialGradientPattern",    new BrushStyleNode(Qt::RadialGradientPattern));
   qt_ns->addConstant("ConicalGradientPattern",   new BrushStyleNode(Qt::ConicalGradientPattern));
   qt_ns->addConstant("TexturePattern",           new BrushStyleNode(Qt::TexturePattern));

   // PenStyle enum
   qt_ns->addConstant("NoPen",             new PenStyleNode(Qt::NoPen));
   qt_ns->addConstant("SolidLine",         new PenStyleNode(Qt::SolidLine));
   qt_ns->addConstant("DashLine",          new PenStyleNode(Qt::DashLine));
   qt_ns->addConstant("DotLine",           new PenStyleNode(Qt::DotLine));
   qt_ns->addConstant("DashDotLine",       new PenStyleNode(Qt::DashDotLine));
   qt_ns->addConstant("DashDotDotLine",    new PenStyleNode(Qt::DashDotDotLine));
   qt_ns->addConstant("CustomDashLine",    new PenStyleNode(Qt::CustomDashLine));

   // AlignmentFlag enum
   qt_ns->addConstant("AlignLeft",                new QoreBigIntNode(Qt::AlignLeft));
   qt_ns->addConstant("AlignLeading",             new QoreBigIntNode(Qt::AlignLeading));
   qt_ns->addConstant("AlignRight",               new QoreBigIntNode(Qt::AlignRight));
   qt_ns->addConstant("AlignTrailing",            new QoreBigIntNode(Qt::AlignTrailing));
   qt_ns->addConstant("AlignHCenter",             new QoreBigIntNode(Qt::AlignHCenter));
   qt_ns->addConstant("AlignJustify",             new QoreBigIntNode(Qt::AlignJustify));
   qt_ns->addConstant("AlignAbsolute",            new QoreBigIntNode(Qt::AlignAbsolute));
   qt_ns->addConstant("AlignHorizontal_Mask",     new QoreBigIntNode(Qt::AlignHorizontal_Mask));
   qt_ns->addConstant("AlignTop",                 new QoreBigIntNode(Qt::AlignTop));
   qt_ns->addConstant("AlignBottom",              new QoreBigIntNode(Qt::AlignBottom));
   qt_ns->addConstant("AlignVCenter",             new QoreBigIntNode(Qt::AlignVCenter));
   qt_ns->addConstant("AlignVertical_Mask",       new QoreBigIntNode(Qt::AlignVertical_Mask));
   qt_ns->addConstant("AlignCenter",              new QoreBigIntNode(Qt::AlignCenter));

   // MouseButton enum
   qt_ns->addConstant("NoButton",                 new QoreBigIntNode(Qt::NoButton));
   qt_ns->addConstant("LeftButton",               new QoreBigIntNode(Qt::LeftButton));
   qt_ns->addConstant("RightButton",              new QoreBigIntNode(Qt::RightButton));
   qt_ns->addConstant("MidButton",                new QoreBigIntNode(Qt::MidButton));
   qt_ns->addConstant("XButton1",                 new QoreBigIntNode(Qt::XButton1));
   qt_ns->addConstant("XButton2",                 new QoreBigIntNode(Qt::XButton2));
   qt_ns->addConstant("MouseButtonMask",          new QoreBigIntNode(Qt::MouseButtonMask));

   // Modifier enum
   qt_ns->addConstant("META",                     new QoreBigIntNode(Qt::META));
   qt_ns->addConstant("SHIFT",                    new QoreBigIntNode(Qt::SHIFT));
   qt_ns->addConstant("CTRL",                     new QoreBigIntNode(Qt::CTRL));
   qt_ns->addConstant("ALT",                      new QoreBigIntNode(Qt::ALT));
   qt_ns->addConstant("MODIFIER_MASK",            new QoreBigIntNode(Qt::MODIFIER_MASK));
   qt_ns->addConstant("UNICODE_ACCEL",            new QoreBigIntNode(Qt::UNICODE_ACCEL));

   // DayOfWeek
   qt_ns->addConstant("Monday",                   new QoreBigIntNode(Qt::Monday));
   qt_ns->addConstant("Tuesday",                  new QoreBigIntNode(Qt::Tuesday));
   qt_ns->addConstant("Wednesday",                new QoreBigIntNode(Qt::Wednesday));
   qt_ns->addConstant("Thursday",                 new QoreBigIntNode(Qt::Thursday));
   qt_ns->addConstant("Friday",                   new QoreBigIntNode(Qt::Friday));
   qt_ns->addConstant("Saturday",                 new QoreBigIntNode(Qt::Saturday));
   qt_ns->addConstant("Sunday",                   new QoreBigIntNode(Qt::Sunday));

   // ContextMenuPolicy enum
   qt_ns->addConstant("NoContextMenu",            new QoreBigIntNode(Qt::NoContextMenu));
   qt_ns->addConstant("DefaultContextMenu",       new QoreBigIntNode(Qt::DefaultContextMenu));
   qt_ns->addConstant("ActionsContextMenu",       new QoreBigIntNode(Qt::ActionsContextMenu));
   qt_ns->addConstant("CustomContextMenu",        new QoreBigIntNode(Qt::CustomContextMenu));
   qt_ns->addConstant("PreventContextMenu",       new QoreBigIntNode(Qt::PreventContextMenu));

   // Key enum
   qt_ns->addConstant("Key_Escape",               new QoreBigIntNode(Qt::Key_Escape));
   qt_ns->addConstant("Key_Tab",                  new QoreBigIntNode(Qt::Key_Tab));
   qt_ns->addConstant("Key_Backtab",              new QoreBigIntNode(Qt::Key_Backtab));
   qt_ns->addConstant("Key_Backspace",            new QoreBigIntNode(Qt::Key_Backspace));
   qt_ns->addConstant("Key_Return",               new QoreBigIntNode(Qt::Key_Return));
   qt_ns->addConstant("Key_Enter",                new QoreBigIntNode(Qt::Key_Enter));
   qt_ns->addConstant("Key_Insert",               new QoreBigIntNode(Qt::Key_Insert));
   qt_ns->addConstant("Key_Delete",               new QoreBigIntNode(Qt::Key_Delete));
   qt_ns->addConstant("Key_Pause",                new QoreBigIntNode(Qt::Key_Pause));
   qt_ns->addConstant("Key_Print",                new QoreBigIntNode(Qt::Key_Print));
   qt_ns->addConstant("Key_SysReq",               new QoreBigIntNode(Qt::Key_SysReq));
   qt_ns->addConstant("Key_Clear",                new QoreBigIntNode(Qt::Key_Clear));
   qt_ns->addConstant("Key_Home",                 new QoreBigIntNode(Qt::Key_Home));
   qt_ns->addConstant("Key_End",                  new QoreBigIntNode(Qt::Key_End));
   qt_ns->addConstant("Key_Left",                 new QoreBigIntNode(Qt::Key_Left));
   qt_ns->addConstant("Key_Up",                   new QoreBigIntNode(Qt::Key_Up));
   qt_ns->addConstant("Key_Right",                new QoreBigIntNode(Qt::Key_Right));
   qt_ns->addConstant("Key_Down",                 new QoreBigIntNode(Qt::Key_Down));
   qt_ns->addConstant("Key_PageUp",               new QoreBigIntNode(Qt::Key_PageUp));
   qt_ns->addConstant("Key_PageDown",             new QoreBigIntNode(Qt::Key_PageDown));
   qt_ns->addConstant("Key_Shift",                new QoreBigIntNode(Qt::Key_Shift));
   qt_ns->addConstant("Key_Control",              new QoreBigIntNode(Qt::Key_Control));
   qt_ns->addConstant("Key_Meta",                 new QoreBigIntNode(Qt::Key_Meta));
   qt_ns->addConstant("Key_Alt",                  new QoreBigIntNode(Qt::Key_Alt));
   qt_ns->addConstant("Key_CapsLock",             new QoreBigIntNode(Qt::Key_CapsLock));
   qt_ns->addConstant("Key_NumLock",              new QoreBigIntNode(Qt::Key_NumLock));
   qt_ns->addConstant("Key_ScrollLock",           new QoreBigIntNode(Qt::Key_ScrollLock));
   qt_ns->addConstant("Key_F1",                   new QoreBigIntNode(Qt::Key_F1));
   qt_ns->addConstant("Key_F2",                   new QoreBigIntNode(Qt::Key_F2));
   qt_ns->addConstant("Key_F3",                   new QoreBigIntNode(Qt::Key_F3));
   qt_ns->addConstant("Key_F4",                   new QoreBigIntNode(Qt::Key_F4));
   qt_ns->addConstant("Key_F5",                   new QoreBigIntNode(Qt::Key_F5));
   qt_ns->addConstant("Key_F6",                   new QoreBigIntNode(Qt::Key_F6));
   qt_ns->addConstant("Key_F7",                   new QoreBigIntNode(Qt::Key_F7));
   qt_ns->addConstant("Key_F8",                   new QoreBigIntNode(Qt::Key_F8));
   qt_ns->addConstant("Key_F9",                   new QoreBigIntNode(Qt::Key_F9));
   qt_ns->addConstant("Key_F10",                  new QoreBigIntNode(Qt::Key_F10));
   qt_ns->addConstant("Key_F11",                  new QoreBigIntNode(Qt::Key_F11));
   qt_ns->addConstant("Key_F12",                  new QoreBigIntNode(Qt::Key_F12));
   qt_ns->addConstant("Key_F13",                  new QoreBigIntNode(Qt::Key_F13));
   qt_ns->addConstant("Key_F14",                  new QoreBigIntNode(Qt::Key_F14));
   qt_ns->addConstant("Key_F15",                  new QoreBigIntNode(Qt::Key_F15));
   qt_ns->addConstant("Key_F16",                  new QoreBigIntNode(Qt::Key_F16));
   qt_ns->addConstant("Key_F17",                  new QoreBigIntNode(Qt::Key_F17));
   qt_ns->addConstant("Key_F18",                  new QoreBigIntNode(Qt::Key_F18));
   qt_ns->addConstant("Key_F19",                  new QoreBigIntNode(Qt::Key_F19));
   qt_ns->addConstant("Key_F20",                  new QoreBigIntNode(Qt::Key_F20));
   qt_ns->addConstant("Key_F21",                  new QoreBigIntNode(Qt::Key_F21));
   qt_ns->addConstant("Key_F22",                  new QoreBigIntNode(Qt::Key_F22));
   qt_ns->addConstant("Key_F23",                  new QoreBigIntNode(Qt::Key_F23));
   qt_ns->addConstant("Key_F24",                  new QoreBigIntNode(Qt::Key_F24));
   qt_ns->addConstant("Key_F25",                  new QoreBigIntNode(Qt::Key_F25));
   qt_ns->addConstant("Key_F26",                  new QoreBigIntNode(Qt::Key_F26));
   qt_ns->addConstant("Key_F27",                  new QoreBigIntNode(Qt::Key_F27));
   qt_ns->addConstant("Key_F28",                  new QoreBigIntNode(Qt::Key_F28));
   qt_ns->addConstant("Key_F29",                  new QoreBigIntNode(Qt::Key_F29));
   qt_ns->addConstant("Key_F30",                  new QoreBigIntNode(Qt::Key_F30));
   qt_ns->addConstant("Key_F31",                  new QoreBigIntNode(Qt::Key_F31));
   qt_ns->addConstant("Key_F32",                  new QoreBigIntNode(Qt::Key_F32));
   qt_ns->addConstant("Key_F33",                  new QoreBigIntNode(Qt::Key_F33));
   qt_ns->addConstant("Key_F34",                  new QoreBigIntNode(Qt::Key_F34));
   qt_ns->addConstant("Key_F35",                  new QoreBigIntNode(Qt::Key_F35));
   qt_ns->addConstant("Key_Super_L",              new QoreBigIntNode(Qt::Key_Super_L));
   qt_ns->addConstant("Key_Super_R",              new QoreBigIntNode(Qt::Key_Super_R));
   qt_ns->addConstant("Key_Menu",                 new QoreBigIntNode(Qt::Key_Menu));
   qt_ns->addConstant("Key_Hyper_L",              new QoreBigIntNode(Qt::Key_Hyper_L));
   qt_ns->addConstant("Key_Hyper_R",              new QoreBigIntNode(Qt::Key_Hyper_R));
   qt_ns->addConstant("Key_Help",                 new QoreBigIntNode(Qt::Key_Help));
   qt_ns->addConstant("Key_Direction_L",          new QoreBigIntNode(Qt::Key_Direction_L));
   qt_ns->addConstant("Key_Direction_R",          new QoreBigIntNode(Qt::Key_Direction_R));
   qt_ns->addConstant("Key_Space",                new QoreBigIntNode(Qt::Key_Space));
   qt_ns->addConstant("Key_Any",                  new QoreBigIntNode(Qt::Key_Any));
   qt_ns->addConstant("Key_Exclam",               new QoreBigIntNode(Qt::Key_Exclam));
   qt_ns->addConstant("Key_QuoteDbl",             new QoreBigIntNode(Qt::Key_QuoteDbl));
   qt_ns->addConstant("Key_NumberSign",           new QoreBigIntNode(Qt::Key_NumberSign));
   qt_ns->addConstant("Key_Dollar",               new QoreBigIntNode(Qt::Key_Dollar));
   qt_ns->addConstant("Key_Percent",              new QoreBigIntNode(Qt::Key_Percent));
   qt_ns->addConstant("Key_Ampersand",            new QoreBigIntNode(Qt::Key_Ampersand));
   qt_ns->addConstant("Key_Apostrophe",           new QoreBigIntNode(Qt::Key_Apostrophe));
   qt_ns->addConstant("Key_ParenLeft",            new QoreBigIntNode(Qt::Key_ParenLeft));
   qt_ns->addConstant("Key_ParenRight",           new QoreBigIntNode(Qt::Key_ParenRight));
   qt_ns->addConstant("Key_Asterisk",             new QoreBigIntNode(Qt::Key_Asterisk));
   qt_ns->addConstant("Key_Plus",                 new QoreBigIntNode(Qt::Key_Plus));
   qt_ns->addConstant("Key_Comma",                new QoreBigIntNode(Qt::Key_Comma));
   qt_ns->addConstant("Key_Minus",                new QoreBigIntNode(Qt::Key_Minus));
   qt_ns->addConstant("Key_Period",               new QoreBigIntNode(Qt::Key_Period));
   qt_ns->addConstant("Key_Slash",                new QoreBigIntNode(Qt::Key_Slash));
   qt_ns->addConstant("Key_0",                    new QoreBigIntNode(Qt::Key_0));
   qt_ns->addConstant("Key_1",                    new QoreBigIntNode(Qt::Key_1));
   qt_ns->addConstant("Key_2",                    new QoreBigIntNode(Qt::Key_2));
   qt_ns->addConstant("Key_3",                    new QoreBigIntNode(Qt::Key_3));
   qt_ns->addConstant("Key_4",                    new QoreBigIntNode(Qt::Key_4));
   qt_ns->addConstant("Key_5",                    new QoreBigIntNode(Qt::Key_5));
   qt_ns->addConstant("Key_6",                    new QoreBigIntNode(Qt::Key_6));
   qt_ns->addConstant("Key_7",                    new QoreBigIntNode(Qt::Key_7));
   qt_ns->addConstant("Key_8",                    new QoreBigIntNode(Qt::Key_8));
   qt_ns->addConstant("Key_9",                    new QoreBigIntNode(Qt::Key_9));
   qt_ns->addConstant("Key_Colon",                new QoreBigIntNode(Qt::Key_Colon));
   qt_ns->addConstant("Key_Semicolon",            new QoreBigIntNode(Qt::Key_Semicolon));
   qt_ns->addConstant("Key_Less",                 new QoreBigIntNode(Qt::Key_Less));
   qt_ns->addConstant("Key_Equal",                new QoreBigIntNode(Qt::Key_Equal));
   qt_ns->addConstant("Key_Greater",              new QoreBigIntNode(Qt::Key_Greater));
   qt_ns->addConstant("Key_Question",             new QoreBigIntNode(Qt::Key_Question));
   qt_ns->addConstant("Key_At",                   new QoreBigIntNode(Qt::Key_At));
   qt_ns->addConstant("Key_A",                    new QoreBigIntNode(Qt::Key_A));
   qt_ns->addConstant("Key_B",                    new QoreBigIntNode(Qt::Key_B));
   qt_ns->addConstant("Key_C",                    new QoreBigIntNode(Qt::Key_C));
   qt_ns->addConstant("Key_D",                    new QoreBigIntNode(Qt::Key_D));
   qt_ns->addConstant("Key_E",                    new QoreBigIntNode(Qt::Key_E));
   qt_ns->addConstant("Key_F",                    new QoreBigIntNode(Qt::Key_F));
   qt_ns->addConstant("Key_G",                    new QoreBigIntNode(Qt::Key_G));
   qt_ns->addConstant("Key_H",                    new QoreBigIntNode(Qt::Key_H));
   qt_ns->addConstant("Key_I",                    new QoreBigIntNode(Qt::Key_I));
   qt_ns->addConstant("Key_J",                    new QoreBigIntNode(Qt::Key_J));
   qt_ns->addConstant("Key_K",                    new QoreBigIntNode(Qt::Key_K));
   qt_ns->addConstant("Key_L",                    new QoreBigIntNode(Qt::Key_L));
   qt_ns->addConstant("Key_M",                    new QoreBigIntNode(Qt::Key_M));
   qt_ns->addConstant("Key_N",                    new QoreBigIntNode(Qt::Key_N));
   qt_ns->addConstant("Key_O",                    new QoreBigIntNode(Qt::Key_O));
   qt_ns->addConstant("Key_P",                    new QoreBigIntNode(Qt::Key_P));
   qt_ns->addConstant("Key_Q",                    new QoreBigIntNode(Qt::Key_Q));
   qt_ns->addConstant("Key_R",                    new QoreBigIntNode(Qt::Key_R));
   qt_ns->addConstant("Key_S",                    new QoreBigIntNode(Qt::Key_S));
   qt_ns->addConstant("Key_T",                    new QoreBigIntNode(Qt::Key_T));
   qt_ns->addConstant("Key_U",                    new QoreBigIntNode(Qt::Key_U));
   qt_ns->addConstant("Key_V",                    new QoreBigIntNode(Qt::Key_V));
   qt_ns->addConstant("Key_W",                    new QoreBigIntNode(Qt::Key_W));
   qt_ns->addConstant("Key_X",                    new QoreBigIntNode(Qt::Key_X));
   qt_ns->addConstant("Key_Y",                    new QoreBigIntNode(Qt::Key_Y));
   qt_ns->addConstant("Key_Z",                    new QoreBigIntNode(Qt::Key_Z));
   qt_ns->addConstant("Key_BracketLeft",          new QoreBigIntNode(Qt::Key_BracketLeft));
   qt_ns->addConstant("Key_Backslash",            new QoreBigIntNode(Qt::Key_Backslash));
   qt_ns->addConstant("Key_BracketRight",         new QoreBigIntNode(Qt::Key_BracketRight));
   qt_ns->addConstant("Key_AsciiCircum",          new QoreBigIntNode(Qt::Key_AsciiCircum));
   qt_ns->addConstant("Key_Underscore",           new QoreBigIntNode(Qt::Key_Underscore));
   qt_ns->addConstant("Key_QuoteLeft",            new QoreBigIntNode(Qt::Key_QuoteLeft));
   qt_ns->addConstant("Key_BraceLeft",            new QoreBigIntNode(Qt::Key_BraceLeft));
   qt_ns->addConstant("Key_Bar",                  new QoreBigIntNode(Qt::Key_Bar));
   qt_ns->addConstant("Key_BraceRight",           new QoreBigIntNode(Qt::Key_BraceRight));
   qt_ns->addConstant("Key_AsciiTilde",           new QoreBigIntNode(Qt::Key_AsciiTilde));
   qt_ns->addConstant("Key_nobreakspace",         new QoreBigIntNode(Qt::Key_nobreakspace));
   qt_ns->addConstant("Key_exclamdown",           new QoreBigIntNode(Qt::Key_exclamdown));
   qt_ns->addConstant("Key_cent",                 new QoreBigIntNode(Qt::Key_cent));
   qt_ns->addConstant("Key_sterling",             new QoreBigIntNode(Qt::Key_sterling));
   qt_ns->addConstant("Key_currency",             new QoreBigIntNode(Qt::Key_currency));
   qt_ns->addConstant("Key_yen",                  new QoreBigIntNode(Qt::Key_yen));
   qt_ns->addConstant("Key_brokenbar",            new QoreBigIntNode(Qt::Key_brokenbar));
   qt_ns->addConstant("Key_section",              new QoreBigIntNode(Qt::Key_section));
   qt_ns->addConstant("Key_diaeresis",            new QoreBigIntNode(Qt::Key_diaeresis));
   qt_ns->addConstant("Key_copyright",            new QoreBigIntNode(Qt::Key_copyright));
   qt_ns->addConstant("Key_ordfeminine",          new QoreBigIntNode(Qt::Key_ordfeminine));
   qt_ns->addConstant("Key_guillemotleft",        new QoreBigIntNode(Qt::Key_guillemotleft));
   qt_ns->addConstant("Key_notsign",              new QoreBigIntNode(Qt::Key_notsign));
   qt_ns->addConstant("Key_hyphen",               new QoreBigIntNode(Qt::Key_hyphen));
   qt_ns->addConstant("Key_registered",           new QoreBigIntNode(Qt::Key_registered));
   qt_ns->addConstant("Key_macron",               new QoreBigIntNode(Qt::Key_macron));
   qt_ns->addConstant("Key_degree",               new QoreBigIntNode(Qt::Key_degree));
   qt_ns->addConstant("Key_plusminus",            new QoreBigIntNode(Qt::Key_plusminus));
   qt_ns->addConstant("Key_twosuperior",          new QoreBigIntNode(Qt::Key_twosuperior));
   qt_ns->addConstant("Key_threesuperior",        new QoreBigIntNode(Qt::Key_threesuperior));
   qt_ns->addConstant("Key_acute",                new QoreBigIntNode(Qt::Key_acute));
   qt_ns->addConstant("Key_mu",                   new QoreBigIntNode(Qt::Key_mu));
   qt_ns->addConstant("Key_paragraph",            new QoreBigIntNode(Qt::Key_paragraph));
   qt_ns->addConstant("Key_periodcentered",       new QoreBigIntNode(Qt::Key_periodcentered));
   qt_ns->addConstant("Key_cedilla",              new QoreBigIntNode(Qt::Key_cedilla));
   qt_ns->addConstant("Key_onesuperior",          new QoreBigIntNode(Qt::Key_onesuperior));
   qt_ns->addConstant("Key_masculine",            new QoreBigIntNode(Qt::Key_masculine));
   qt_ns->addConstant("Key_guillemotright",       new QoreBigIntNode(Qt::Key_guillemotright));
   qt_ns->addConstant("Key_onequarter",           new QoreBigIntNode(Qt::Key_onequarter));
   qt_ns->addConstant("Key_onehalf",              new QoreBigIntNode(Qt::Key_onehalf));
   qt_ns->addConstant("Key_threequarters",        new QoreBigIntNode(Qt::Key_threequarters));
   qt_ns->addConstant("Key_questiondown",         new QoreBigIntNode(Qt::Key_questiondown));
   qt_ns->addConstant("Key_Agrave",               new QoreBigIntNode(Qt::Key_Agrave));
   qt_ns->addConstant("Key_Aacute",               new QoreBigIntNode(Qt::Key_Aacute));
   qt_ns->addConstant("Key_Acircumflex",          new QoreBigIntNode(Qt::Key_Acircumflex));
   qt_ns->addConstant("Key_Atilde",               new QoreBigIntNode(Qt::Key_Atilde));
   qt_ns->addConstant("Key_Adiaeresis",           new QoreBigIntNode(Qt::Key_Adiaeresis));
   qt_ns->addConstant("Key_Aring",                new QoreBigIntNode(Qt::Key_Aring));
   qt_ns->addConstant("Key_AE",                   new QoreBigIntNode(Qt::Key_AE));
   qt_ns->addConstant("Key_Ccedilla",             new QoreBigIntNode(Qt::Key_Ccedilla));
   qt_ns->addConstant("Key_Egrave",               new QoreBigIntNode(Qt::Key_Egrave));
   qt_ns->addConstant("Key_Eacute",               new QoreBigIntNode(Qt::Key_Eacute));
   qt_ns->addConstant("Key_Ecircumflex",          new QoreBigIntNode(Qt::Key_Ecircumflex));
   qt_ns->addConstant("Key_Ediaeresis",           new QoreBigIntNode(Qt::Key_Ediaeresis));
   qt_ns->addConstant("Key_Igrave",               new QoreBigIntNode(Qt::Key_Igrave));
   qt_ns->addConstant("Key_Iacute",               new QoreBigIntNode(Qt::Key_Iacute));
   qt_ns->addConstant("Key_Icircumflex",          new QoreBigIntNode(Qt::Key_Icircumflex));
   qt_ns->addConstant("Key_Idiaeresis",           new QoreBigIntNode(Qt::Key_Idiaeresis));
   qt_ns->addConstant("Key_ETH",                  new QoreBigIntNode(Qt::Key_ETH));
   qt_ns->addConstant("Key_Ntilde",               new QoreBigIntNode(Qt::Key_Ntilde));
   qt_ns->addConstant("Key_Ograve",               new QoreBigIntNode(Qt::Key_Ograve));
   qt_ns->addConstant("Key_Oacute",               new QoreBigIntNode(Qt::Key_Oacute));
   qt_ns->addConstant("Key_Ocircumflex",          new QoreBigIntNode(Qt::Key_Ocircumflex));
   qt_ns->addConstant("Key_Otilde",               new QoreBigIntNode(Qt::Key_Otilde));
   qt_ns->addConstant("Key_Odiaeresis",           new QoreBigIntNode(Qt::Key_Odiaeresis));
   qt_ns->addConstant("Key_multiply",             new QoreBigIntNode(Qt::Key_multiply));
   qt_ns->addConstant("Key_Ooblique",             new QoreBigIntNode(Qt::Key_Ooblique));
   qt_ns->addConstant("Key_Ugrave",               new QoreBigIntNode(Qt::Key_Ugrave));
   qt_ns->addConstant("Key_Uacute",               new QoreBigIntNode(Qt::Key_Uacute));
   qt_ns->addConstant("Key_Ucircumflex",          new QoreBigIntNode(Qt::Key_Ucircumflex));
   qt_ns->addConstant("Key_Udiaeresis",           new QoreBigIntNode(Qt::Key_Udiaeresis));
   qt_ns->addConstant("Key_Yacute",               new QoreBigIntNode(Qt::Key_Yacute));
   qt_ns->addConstant("Key_THORN",                new QoreBigIntNode(Qt::Key_THORN));
   qt_ns->addConstant("Key_ssharp",               new QoreBigIntNode(Qt::Key_ssharp));
   qt_ns->addConstant("Key_division",             new QoreBigIntNode(Qt::Key_division));
   qt_ns->addConstant("Key_ydiaeresis",           new QoreBigIntNode(Qt::Key_ydiaeresis));
   qt_ns->addConstant("Key_AltGr",                new QoreBigIntNode(Qt::Key_AltGr));
   qt_ns->addConstant("Key_Multi_key",            new QoreBigIntNode(Qt::Key_Multi_key));
   qt_ns->addConstant("Key_Codeinput",            new QoreBigIntNode(Qt::Key_Codeinput));
   qt_ns->addConstant("Key_SingleCandidate",      new QoreBigIntNode(Qt::Key_SingleCandidate));
   qt_ns->addConstant("Key_MultipleCandidate",    new QoreBigIntNode(Qt::Key_MultipleCandidate));
   qt_ns->addConstant("Key_PreviousCandidate",    new QoreBigIntNode(Qt::Key_PreviousCandidate));
   qt_ns->addConstant("Key_Mode_switch",          new QoreBigIntNode(Qt::Key_Mode_switch));
   qt_ns->addConstant("Key_Kanji",                new QoreBigIntNode(Qt::Key_Kanji));
   qt_ns->addConstant("Key_Muhenkan",             new QoreBigIntNode(Qt::Key_Muhenkan));
   qt_ns->addConstant("Key_Henkan",               new QoreBigIntNode(Qt::Key_Henkan));
   qt_ns->addConstant("Key_Romaji",               new QoreBigIntNode(Qt::Key_Romaji));
   qt_ns->addConstant("Key_Hiragana",             new QoreBigIntNode(Qt::Key_Hiragana));
   qt_ns->addConstant("Key_Katakana",             new QoreBigIntNode(Qt::Key_Katakana));
   qt_ns->addConstant("Key_Hiragana_Katakana",    new QoreBigIntNode(Qt::Key_Hiragana_Katakana));
   qt_ns->addConstant("Key_Zenkaku",              new QoreBigIntNode(Qt::Key_Zenkaku));
   qt_ns->addConstant("Key_Hankaku",              new QoreBigIntNode(Qt::Key_Hankaku));
   qt_ns->addConstant("Key_Zenkaku_Hankaku",      new QoreBigIntNode(Qt::Key_Zenkaku_Hankaku));
   qt_ns->addConstant("Key_Touroku",              new QoreBigIntNode(Qt::Key_Touroku));
   qt_ns->addConstant("Key_Massyo",               new QoreBigIntNode(Qt::Key_Massyo));
   qt_ns->addConstant("Key_Kana_Lock",            new QoreBigIntNode(Qt::Key_Kana_Lock));
   qt_ns->addConstant("Key_Kana_Shift",           new QoreBigIntNode(Qt::Key_Kana_Shift));
   qt_ns->addConstant("Key_Eisu_Shift",           new QoreBigIntNode(Qt::Key_Eisu_Shift));
   qt_ns->addConstant("Key_Eisu_toggle",          new QoreBigIntNode(Qt::Key_Eisu_toggle));
   qt_ns->addConstant("Key_Hangul",               new QoreBigIntNode(Qt::Key_Hangul));
   qt_ns->addConstant("Key_Hangul_Start",         new QoreBigIntNode(Qt::Key_Hangul_Start));
   qt_ns->addConstant("Key_Hangul_End",           new QoreBigIntNode(Qt::Key_Hangul_End));
   qt_ns->addConstant("Key_Hangul_Hanja",         new QoreBigIntNode(Qt::Key_Hangul_Hanja));
   qt_ns->addConstant("Key_Hangul_Jamo",          new QoreBigIntNode(Qt::Key_Hangul_Jamo));
   qt_ns->addConstant("Key_Hangul_Romaja",        new QoreBigIntNode(Qt::Key_Hangul_Romaja));
   qt_ns->addConstant("Key_Hangul_Jeonja",        new QoreBigIntNode(Qt::Key_Hangul_Jeonja));
   qt_ns->addConstant("Key_Hangul_Banja",         new QoreBigIntNode(Qt::Key_Hangul_Banja));
   qt_ns->addConstant("Key_Hangul_PreHanja",      new QoreBigIntNode(Qt::Key_Hangul_PreHanja));
   qt_ns->addConstant("Key_Hangul_PostHanja",     new QoreBigIntNode(Qt::Key_Hangul_PostHanja));
   qt_ns->addConstant("Key_Hangul_Special",       new QoreBigIntNode(Qt::Key_Hangul_Special));
   qt_ns->addConstant("Key_Dead_Grave",           new QoreBigIntNode(Qt::Key_Dead_Grave));
   qt_ns->addConstant("Key_Dead_Acute",           new QoreBigIntNode(Qt::Key_Dead_Acute));
   qt_ns->addConstant("Key_Dead_Circumflex",      new QoreBigIntNode(Qt::Key_Dead_Circumflex));
   qt_ns->addConstant("Key_Dead_Tilde",           new QoreBigIntNode(Qt::Key_Dead_Tilde));
   qt_ns->addConstant("Key_Dead_Macron",          new QoreBigIntNode(Qt::Key_Dead_Macron));
   qt_ns->addConstant("Key_Dead_Breve",           new QoreBigIntNode(Qt::Key_Dead_Breve));
   qt_ns->addConstant("Key_Dead_Abovedot",        new QoreBigIntNode(Qt::Key_Dead_Abovedot));
   qt_ns->addConstant("Key_Dead_Diaeresis",       new QoreBigIntNode(Qt::Key_Dead_Diaeresis));
   qt_ns->addConstant("Key_Dead_Abovering",       new QoreBigIntNode(Qt::Key_Dead_Abovering));
   qt_ns->addConstant("Key_Dead_Doubleacute",     new QoreBigIntNode(Qt::Key_Dead_Doubleacute));
   qt_ns->addConstant("Key_Dead_Caron",           new QoreBigIntNode(Qt::Key_Dead_Caron));
   qt_ns->addConstant("Key_Dead_Cedilla",         new QoreBigIntNode(Qt::Key_Dead_Cedilla));
   qt_ns->addConstant("Key_Dead_Ogonek",          new QoreBigIntNode(Qt::Key_Dead_Ogonek));
   qt_ns->addConstant("Key_Dead_Iota",            new QoreBigIntNode(Qt::Key_Dead_Iota));
   qt_ns->addConstant("Key_Dead_Voiced_Sound",    new QoreBigIntNode(Qt::Key_Dead_Voiced_Sound));
   qt_ns->addConstant("Key_Dead_Semivoiced_Sound", new QoreBigIntNode(Qt::Key_Dead_Semivoiced_Sound));
   qt_ns->addConstant("Key_Dead_Belowdot",        new QoreBigIntNode(Qt::Key_Dead_Belowdot));
   qt_ns->addConstant("Key_Dead_Hook",            new QoreBigIntNode(Qt::Key_Dead_Hook));
   qt_ns->addConstant("Key_Dead_Horn",            new QoreBigIntNode(Qt::Key_Dead_Horn));
   qt_ns->addConstant("Key_Back",                 new QoreBigIntNode(Qt::Key_Back));
   qt_ns->addConstant("Key_Forward",              new QoreBigIntNode(Qt::Key_Forward));
   qt_ns->addConstant("Key_Stop",                 new QoreBigIntNode(Qt::Key_Stop));
   qt_ns->addConstant("Key_Refresh",              new QoreBigIntNode(Qt::Key_Refresh));
   qt_ns->addConstant("Key_VolumeDown",           new QoreBigIntNode(Qt::Key_VolumeDown));
   qt_ns->addConstant("Key_VolumeMute",           new QoreBigIntNode(Qt::Key_VolumeMute));
   qt_ns->addConstant("Key_VolumeUp",             new QoreBigIntNode(Qt::Key_VolumeUp));
   qt_ns->addConstant("Key_BassBoost",            new QoreBigIntNode(Qt::Key_BassBoost));
   qt_ns->addConstant("Key_BassUp",               new QoreBigIntNode(Qt::Key_BassUp));
   qt_ns->addConstant("Key_BassDown",             new QoreBigIntNode(Qt::Key_BassDown));
   qt_ns->addConstant("Key_TrebleUp",             new QoreBigIntNode(Qt::Key_TrebleUp));
   qt_ns->addConstant("Key_TrebleDown",           new QoreBigIntNode(Qt::Key_TrebleDown));
   qt_ns->addConstant("Key_MediaPlay",            new QoreBigIntNode(Qt::Key_MediaPlay));
   qt_ns->addConstant("Key_MediaStop",            new QoreBigIntNode(Qt::Key_MediaStop));
   qt_ns->addConstant("Key_MediaPrevious",        new QoreBigIntNode(Qt::Key_MediaPrevious));
   qt_ns->addConstant("Key_MediaNext",            new QoreBigIntNode(Qt::Key_MediaNext));
   qt_ns->addConstant("Key_MediaRecord",          new QoreBigIntNode(Qt::Key_MediaRecord));
   qt_ns->addConstant("Key_HomePage",             new QoreBigIntNode(Qt::Key_HomePage));
   qt_ns->addConstant("Key_Favorites",            new QoreBigIntNode(Qt::Key_Favorites));
   qt_ns->addConstant("Key_Search",               new QoreBigIntNode(Qt::Key_Search));
   qt_ns->addConstant("Key_Standby",              new QoreBigIntNode(Qt::Key_Standby));
   qt_ns->addConstant("Key_OpenUrl",              new QoreBigIntNode(Qt::Key_OpenUrl));
   qt_ns->addConstant("Key_LaunchMail",           new QoreBigIntNode(Qt::Key_LaunchMail));
   qt_ns->addConstant("Key_LaunchMedia",          new QoreBigIntNode(Qt::Key_LaunchMedia));
   qt_ns->addConstant("Key_Launch0",              new QoreBigIntNode(Qt::Key_Launch0));
   qt_ns->addConstant("Key_Launch1",              new QoreBigIntNode(Qt::Key_Launch1));
   qt_ns->addConstant("Key_Launch2",              new QoreBigIntNode(Qt::Key_Launch2));
   qt_ns->addConstant("Key_Launch3",              new QoreBigIntNode(Qt::Key_Launch3));
   qt_ns->addConstant("Key_Launch4",              new QoreBigIntNode(Qt::Key_Launch4));
   qt_ns->addConstant("Key_Launch5",              new QoreBigIntNode(Qt::Key_Launch5));
   qt_ns->addConstant("Key_Launch6",              new QoreBigIntNode(Qt::Key_Launch6));
   qt_ns->addConstant("Key_Launch7",              new QoreBigIntNode(Qt::Key_Launch7));
   qt_ns->addConstant("Key_Launch8",              new QoreBigIntNode(Qt::Key_Launch8));
   qt_ns->addConstant("Key_Launch9",              new QoreBigIntNode(Qt::Key_Launch9));
   qt_ns->addConstant("Key_LaunchA",              new QoreBigIntNode(Qt::Key_LaunchA));
   qt_ns->addConstant("Key_LaunchB",              new QoreBigIntNode(Qt::Key_LaunchB));
   qt_ns->addConstant("Key_LaunchC",              new QoreBigIntNode(Qt::Key_LaunchC));
   qt_ns->addConstant("Key_LaunchD",              new QoreBigIntNode(Qt::Key_LaunchD));
   qt_ns->addConstant("Key_LaunchE",              new QoreBigIntNode(Qt::Key_LaunchE));
   qt_ns->addConstant("Key_LaunchF",              new QoreBigIntNode(Qt::Key_LaunchF));
   qt_ns->addConstant("Key_MediaLast",            new QoreBigIntNode(Qt::Key_MediaLast));
   qt_ns->addConstant("Key_Select",               new QoreBigIntNode(Qt::Key_Select));
   qt_ns->addConstant("Key_Yes",                  new QoreBigIntNode(Qt::Key_Yes));
   qt_ns->addConstant("Key_No",                   new QoreBigIntNode(Qt::Key_No));
   qt_ns->addConstant("Key_Cancel",               new QoreBigIntNode(Qt::Key_Cancel));
   qt_ns->addConstant("Key_Printer",              new QoreBigIntNode(Qt::Key_Printer));
   qt_ns->addConstant("Key_Execute",              new QoreBigIntNode(Qt::Key_Execute));
   qt_ns->addConstant("Key_Sleep",                new QoreBigIntNode(Qt::Key_Sleep));
   qt_ns->addConstant("Key_Play",                 new QoreBigIntNode(Qt::Key_Play));
   qt_ns->addConstant("Key_Zoom",                 new QoreBigIntNode(Qt::Key_Zoom));
   qt_ns->addConstant("Key_Context1",             new QoreBigIntNode(Qt::Key_Context1));
   qt_ns->addConstant("Key_Context2",             new QoreBigIntNode(Qt::Key_Context2));
   qt_ns->addConstant("Key_Context3",             new QoreBigIntNode(Qt::Key_Context3));
   qt_ns->addConstant("Key_Context4",             new QoreBigIntNode(Qt::Key_Context4));
   qt_ns->addConstant("Key_Call",                 new QoreBigIntNode(Qt::Key_Call));
   qt_ns->addConstant("Key_Hangup",               new QoreBigIntNode(Qt::Key_Hangup));
   qt_ns->addConstant("Key_Flip",                 new QoreBigIntNode(Qt::Key_Flip));
   qt_ns->addConstant("Key_unknown",              new QoreBigIntNode(Qt::Key_unknown));

   // MatchFlag enum
   qt_ns->addConstant("MatchExactly",             new QoreBigIntNode(Qt::MatchExactly));
   qt_ns->addConstant("MatchContains",            new QoreBigIntNode(Qt::MatchContains));
   qt_ns->addConstant("MatchStartsWith",          new QoreBigIntNode(Qt::MatchStartsWith));
   qt_ns->addConstant("MatchEndsWith",            new QoreBigIntNode(Qt::MatchEndsWith));
   qt_ns->addConstant("MatchRegExp",              new QoreBigIntNode(Qt::MatchRegExp));
   qt_ns->addConstant("MatchWildcard",            new QoreBigIntNode(Qt::MatchWildcard));
   qt_ns->addConstant("MatchFixedString",         new QoreBigIntNode(Qt::MatchFixedString));
   qt_ns->addConstant("MatchCaseSensitive",       new QoreBigIntNode(Qt::MatchCaseSensitive));
   qt_ns->addConstant("MatchWrap",                new QoreBigIntNode(Qt::MatchWrap));
   qt_ns->addConstant("MatchRecursive",           new QoreBigIntNode(Qt::MatchRecursive));

   // ItemDataRole enum
   qt_ns->addConstant("DisplayRole",              new QoreBigIntNode(Qt::DisplayRole));
   qt_ns->addConstant("DecorationRole",           new QoreBigIntNode(Qt::DecorationRole));
   qt_ns->addConstant("EditRole",                 new QoreBigIntNode(Qt::EditRole));
   qt_ns->addConstant("ToolTipRole",              new QoreBigIntNode(Qt::ToolTipRole));
   qt_ns->addConstant("StatusTipRole",            new QoreBigIntNode(Qt::StatusTipRole));
   qt_ns->addConstant("WhatsThisRole",            new QoreBigIntNode(Qt::WhatsThisRole));
   qt_ns->addConstant("FontRole",                 new QoreBigIntNode(Qt::FontRole));
   qt_ns->addConstant("TextAlignmentRole",        new QoreBigIntNode(Qt::TextAlignmentRole));
   qt_ns->addConstant("BackgroundColorRole",      new QoreBigIntNode(Qt::BackgroundColorRole));
   qt_ns->addConstant("BackgroundRole",           new QoreBigIntNode(Qt::BackgroundRole));
   qt_ns->addConstant("TextColorRole",            new QoreBigIntNode(Qt::TextColorRole));
   qt_ns->addConstant("ForegroundRole",           new QoreBigIntNode(Qt::ForegroundRole));
   qt_ns->addConstant("CheckStateRole",           new QoreBigIntNode(Qt::CheckStateRole));
   qt_ns->addConstant("AccessibleTextRole",       new QoreBigIntNode(Qt::AccessibleTextRole));
   qt_ns->addConstant("AccessibleDescriptionRole", new QoreBigIntNode(Qt::AccessibleDescriptionRole));
   qt_ns->addConstant("SizeHintRole",             new QoreBigIntNode(Qt::SizeHintRole));
   qt_ns->addConstant("UserRole",                 new QoreBigIntNode(Qt::UserRole));

   // ItemFlag enum
   qt_ns->addConstant("ItemIsSelectable",         new QoreBigIntNode(Qt::ItemIsSelectable));
   qt_ns->addConstant("ItemIsEditable",           new QoreBigIntNode(Qt::ItemIsEditable));
   qt_ns->addConstant("ItemIsDragEnabled",        new QoreBigIntNode(Qt::ItemIsDragEnabled));
   qt_ns->addConstant("ItemIsDropEnabled",        new QoreBigIntNode(Qt::ItemIsDropEnabled));
   qt_ns->addConstant("ItemIsUserCheckable",      new QoreBigIntNode(Qt::ItemIsUserCheckable));
   qt_ns->addConstant("ItemIsEnabled",            new QoreBigIntNode(Qt::ItemIsEnabled));
   qt_ns->addConstant("ItemIsTristate",           new QoreBigIntNode(Qt::ItemIsTristate));
	
   // AspectRatioMode enum
   qt_ns->addConstant("IgnoreAspectRatio",        new QoreBigIntNode(Qt::IgnoreAspectRatio));
   qt_ns->addConstant("KeepAspectRatio",          new QoreBigIntNode(Qt::KeepAspectRatio));
   qt_ns->addConstant("KeepAspectRatioByExpanding", new QoreBigIntNode(Qt::KeepAspectRatioByExpanding));

   // TextFormat enum
   qt_ns->addConstant("PlainText",                new QoreBigIntNode(Qt::PlainText));
   qt_ns->addConstant("RichText",                 new QoreBigIntNode(Qt::RichText));
   qt_ns->addConstant("AutoText",                 new QoreBigIntNode(Qt::AutoText));
   qt_ns->addConstant("LogText",                  new QoreBigIntNode(Qt::LogText));

   // CursorShape enum
   qt_ns->addConstant("ArrowCursor",              new QoreBigIntNode(Qt::ArrowCursor));
   qt_ns->addConstant("UpArrowCursor",            new QoreBigIntNode(Qt::UpArrowCursor));
   qt_ns->addConstant("CrossCursor",              new QoreBigIntNode(Qt::CrossCursor));
   qt_ns->addConstant("WaitCursor",               new QoreBigIntNode(Qt::WaitCursor));
   qt_ns->addConstant("IBeamCursor",              new QoreBigIntNode(Qt::IBeamCursor));
   qt_ns->addConstant("SizeVerCursor",            new QoreBigIntNode(Qt::SizeVerCursor));
   qt_ns->addConstant("SizeHorCursor",            new QoreBigIntNode(Qt::SizeHorCursor));
   qt_ns->addConstant("SizeBDiagCursor",          new QoreBigIntNode(Qt::SizeBDiagCursor));
   qt_ns->addConstant("SizeFDiagCursor",          new QoreBigIntNode(Qt::SizeFDiagCursor));
   qt_ns->addConstant("SizeAllCursor",            new QoreBigIntNode(Qt::SizeAllCursor));
   qt_ns->addConstant("BlankCursor",              new QoreBigIntNode(Qt::BlankCursor));
   qt_ns->addConstant("SplitVCursor",             new QoreBigIntNode(Qt::SplitVCursor));
   qt_ns->addConstant("SplitHCursor",             new QoreBigIntNode(Qt::SplitHCursor));
   qt_ns->addConstant("PointingHandCursor",       new QoreBigIntNode(Qt::PointingHandCursor));
   qt_ns->addConstant("ForbiddenCursor",          new QoreBigIntNode(Qt::ForbiddenCursor));
   qt_ns->addConstant("WhatsThisCursor",          new QoreBigIntNode(Qt::WhatsThisCursor));
   qt_ns->addConstant("BusyCursor",               new QoreBigIntNode(Qt::BusyCursor));
   qt_ns->addConstant("OpenHandCursor",           new QoreBigIntNode(Qt::OpenHandCursor));
   qt_ns->addConstant("ClosedHandCursor",         new QoreBigIntNode(Qt::ClosedHandCursor));
   qt_ns->addConstant("LastCursor",               new QoreBigIntNode(Qt::LastCursor));
   qt_ns->addConstant("BitmapCursor",             new QoreBigIntNode(Qt::BitmapCursor));
   qt_ns->addConstant("CustomCursor",             new QoreBigIntNode(Qt::CustomCursor));

   // AnchorAttribute enum
   qt_ns->addConstant("AnchorName",               new QoreBigIntNode(Qt::AnchorName));
   qt_ns->addConstant("AnchorHref",               new QoreBigIntNode(Qt::AnchorHref));

   // DockWidgetArea enum
   qt_ns->addConstant("LeftDockWidgetArea",       new QoreBigIntNode(Qt::LeftDockWidgetArea));
   qt_ns->addConstant("RightDockWidgetArea",      new QoreBigIntNode(Qt::RightDockWidgetArea));
   qt_ns->addConstant("TopDockWidgetArea",        new QoreBigIntNode(Qt::TopDockWidgetArea));
   qt_ns->addConstant("BottomDockWidgetArea",     new QoreBigIntNode(Qt::BottomDockWidgetArea));
   qt_ns->addConstant("DockWidgetArea_Mask",      new QoreBigIntNode(Qt::DockWidgetArea_Mask));
   qt_ns->addConstant("AllDockWidgetAreas",       new QoreBigIntNode(Qt::AllDockWidgetAreas));
   qt_ns->addConstant("NoDockWidgetArea",         new QoreBigIntNode(Qt::NoDockWidgetArea));

   // DockWidgetAreaSizes enum
   qt_ns->addConstant("NDockWidgetAreas",         new QoreBigIntNode(Qt::NDockWidgetAreas));

   // ToolBarArea enum
   qt_ns->addConstant("LeftToolBarArea",          new QoreBigIntNode(Qt::LeftToolBarArea));
   qt_ns->addConstant("RightToolBarArea",         new QoreBigIntNode(Qt::RightToolBarArea));
   qt_ns->addConstant("TopToolBarArea",           new QoreBigIntNode(Qt::TopToolBarArea));
   qt_ns->addConstant("BottomToolBarArea",        new QoreBigIntNode(Qt::BottomToolBarArea));
   qt_ns->addConstant("ToolBarArea_Mask",         new QoreBigIntNode(Qt::ToolBarArea_Mask));
   qt_ns->addConstant("AllToolBarAreas",          new QoreBigIntNode(Qt::AllToolBarAreas));
   qt_ns->addConstant("NoToolBarArea",            new QoreBigIntNode(Qt::NoToolBarArea));

   // ToolBarSizes enum
   qt_ns->addConstant("NToolBarAreas",            new QoreBigIntNode(Qt::NToolBarAreas));

   // PenCapStyle enum
   qt_ns->addConstant("FlatCap",                  new QoreBigIntNode(Qt::FlatCap));
   qt_ns->addConstant("SquareCap",                new QoreBigIntNode(Qt::SquareCap));
   qt_ns->addConstant("RoundCap",                 new QoreBigIntNode(Qt::RoundCap));
   qt_ns->addConstant("MPenCapStyle",             new QoreBigIntNode(Qt::MPenCapStyle));

   // PenJoinStyle enum
   qt_ns->addConstant("MiterJoin",                new QoreBigIntNode(Qt::MiterJoin));
   qt_ns->addConstant("BevelJoin",                new QoreBigIntNode(Qt::BevelJoin));
   qt_ns->addConstant("RoundJoin",                new QoreBigIntNode(Qt::RoundJoin));
   qt_ns->addConstant("SvgMiterJoin",             new QoreBigIntNode(Qt::SvgMiterJoin));
   qt_ns->addConstant("MPenJoinStyle",            new QoreBigIntNode(Qt::MPenJoinStyle));

   // WidgetAttribute enum
   qt_ns->addConstant("WA_Disabled",              new QoreBigIntNode(Qt::WA_Disabled));
   qt_ns->addConstant("WA_UnderMouse",            new QoreBigIntNode(Qt::WA_UnderMouse));
   qt_ns->addConstant("WA_MouseTracking",         new QoreBigIntNode(Qt::WA_MouseTracking));
   qt_ns->addConstant("WA_ContentsPropagated",    new QoreBigIntNode(Qt::WA_ContentsPropagated));
   qt_ns->addConstant("WA_OpaquePaintEvent",      new QoreBigIntNode(Qt::WA_OpaquePaintEvent));
   qt_ns->addConstant("WA_NoBackground",          new QoreBigIntNode(Qt::WA_NoBackground));
   qt_ns->addConstant("WA_StaticContents",        new QoreBigIntNode(Qt::WA_StaticContents));
   qt_ns->addConstant("WA_LaidOut",               new QoreBigIntNode(Qt::WA_LaidOut));
   qt_ns->addConstant("WA_PaintOnScreen",         new QoreBigIntNode(Qt::WA_PaintOnScreen));
   qt_ns->addConstant("WA_NoSystemBackground",    new QoreBigIntNode(Qt::WA_NoSystemBackground));
   qt_ns->addConstant("WA_UpdatesDisabled",       new QoreBigIntNode(Qt::WA_UpdatesDisabled));
   qt_ns->addConstant("WA_Mapped",                new QoreBigIntNode(Qt::WA_Mapped));
   qt_ns->addConstant("WA_MacNoClickThrough",     new QoreBigIntNode(Qt::WA_MacNoClickThrough));
   qt_ns->addConstant("WA_PaintOutsidePaintEvent", new QoreBigIntNode(Qt::WA_PaintOutsidePaintEvent));
   qt_ns->addConstant("WA_InputMethodEnabled",    new QoreBigIntNode(Qt::WA_InputMethodEnabled));
   qt_ns->addConstant("WA_WState_Visible",        new QoreBigIntNode(Qt::WA_WState_Visible));
   qt_ns->addConstant("WA_WState_Hidden",         new QoreBigIntNode(Qt::WA_WState_Hidden));
   qt_ns->addConstant("WA_ForceDisabled",         new QoreBigIntNode(Qt::WA_ForceDisabled));
   qt_ns->addConstant("WA_KeyCompression",        new QoreBigIntNode(Qt::WA_KeyCompression));
   qt_ns->addConstant("WA_PendingMoveEvent",      new QoreBigIntNode(Qt::WA_PendingMoveEvent));
   qt_ns->addConstant("WA_PendingResizeEvent",    new QoreBigIntNode(Qt::WA_PendingResizeEvent));
   qt_ns->addConstant("WA_SetPalette",            new QoreBigIntNode(Qt::WA_SetPalette));
   qt_ns->addConstant("WA_SetFont",               new QoreBigIntNode(Qt::WA_SetFont));
   qt_ns->addConstant("WA_SetCursor",             new QoreBigIntNode(Qt::WA_SetCursor));
   qt_ns->addConstant("WA_NoChildEventsFromChildren", new QoreBigIntNode(Qt::WA_NoChildEventsFromChildren));
   qt_ns->addConstant("WA_WindowModified",        new QoreBigIntNode(Qt::WA_WindowModified));
   qt_ns->addConstant("WA_Resized",               new QoreBigIntNode(Qt::WA_Resized));
   qt_ns->addConstant("WA_Moved",                 new QoreBigIntNode(Qt::WA_Moved));
   qt_ns->addConstant("WA_PendingUpdate",         new QoreBigIntNode(Qt::WA_PendingUpdate));
   qt_ns->addConstant("WA_InvalidSize",           new QoreBigIntNode(Qt::WA_InvalidSize));
   qt_ns->addConstant("WA_MacBrushedMetal",       new QoreBigIntNode(Qt::WA_MacBrushedMetal));
   qt_ns->addConstant("WA_MacMetalStyle",         new QoreBigIntNode(Qt::WA_MacMetalStyle));
   qt_ns->addConstant("WA_CustomWhatsThis",       new QoreBigIntNode(Qt::WA_CustomWhatsThis));
   qt_ns->addConstant("WA_LayoutOnEntireRect",    new QoreBigIntNode(Qt::WA_LayoutOnEntireRect));
   qt_ns->addConstant("WA_OutsideWSRange",        new QoreBigIntNode(Qt::WA_OutsideWSRange));
   qt_ns->addConstant("WA_GrabbedShortcut",       new QoreBigIntNode(Qt::WA_GrabbedShortcut));
   qt_ns->addConstant("WA_TransparentForMouseEvents", new QoreBigIntNode(Qt::WA_TransparentForMouseEvents));
   qt_ns->addConstant("WA_PaintUnclipped",        new QoreBigIntNode(Qt::WA_PaintUnclipped));
   qt_ns->addConstant("WA_SetWindowIcon",         new QoreBigIntNode(Qt::WA_SetWindowIcon));
   qt_ns->addConstant("WA_NoMouseReplay",         new QoreBigIntNode(Qt::WA_NoMouseReplay));
   qt_ns->addConstant("WA_DeleteOnClose",         new QoreBigIntNode(Qt::WA_DeleteOnClose));
   qt_ns->addConstant("WA_RightToLeft",           new QoreBigIntNode(Qt::WA_RightToLeft));
   qt_ns->addConstant("WA_SetLayoutDirection",    new QoreBigIntNode(Qt::WA_SetLayoutDirection));
   qt_ns->addConstant("WA_NoChildEventsForParent", new QoreBigIntNode(Qt::WA_NoChildEventsForParent));
   qt_ns->addConstant("WA_ForceUpdatesDisabled",  new QoreBigIntNode(Qt::WA_ForceUpdatesDisabled));
   qt_ns->addConstant("WA_WState_Created",        new QoreBigIntNode(Qt::WA_WState_Created));
   qt_ns->addConstant("WA_WState_CompressKeys",   new QoreBigIntNode(Qt::WA_WState_CompressKeys));
   qt_ns->addConstant("WA_WState_InPaintEvent",   new QoreBigIntNode(Qt::WA_WState_InPaintEvent));
   qt_ns->addConstant("WA_WState_Reparented",     new QoreBigIntNode(Qt::WA_WState_Reparented));
   qt_ns->addConstant("WA_WState_ConfigPending",  new QoreBigIntNode(Qt::WA_WState_ConfigPending));
   qt_ns->addConstant("WA_WState_Polished",       new QoreBigIntNode(Qt::WA_WState_Polished));
   qt_ns->addConstant("WA_WState_DND",            new QoreBigIntNode(Qt::WA_WState_DND));
   qt_ns->addConstant("WA_WState_OwnSizePolicy",  new QoreBigIntNode(Qt::WA_WState_OwnSizePolicy));
   qt_ns->addConstant("WA_WState_ExplicitShowHide", new QoreBigIntNode(Qt::WA_WState_ExplicitShowHide));
   qt_ns->addConstant("WA_ShowModal",             new QoreBigIntNode(Qt::WA_ShowModal));
   qt_ns->addConstant("WA_MouseNoMask",           new QoreBigIntNode(Qt::WA_MouseNoMask));
   qt_ns->addConstant("WA_GroupLeader",           new QoreBigIntNode(Qt::WA_GroupLeader));
   qt_ns->addConstant("WA_NoMousePropagation",    new QoreBigIntNode(Qt::WA_NoMousePropagation));
   qt_ns->addConstant("WA_Hover",                 new QoreBigIntNode(Qt::WA_Hover));
   qt_ns->addConstant("WA_InputMethodTransparent", new QoreBigIntNode(Qt::WA_InputMethodTransparent));
   qt_ns->addConstant("WA_QuitOnClose",           new QoreBigIntNode(Qt::WA_QuitOnClose));
   qt_ns->addConstant("WA_KeyboardFocusChange",   new QoreBigIntNode(Qt::WA_KeyboardFocusChange));
   qt_ns->addConstant("WA_AcceptDrops",           new QoreBigIntNode(Qt::WA_AcceptDrops));
   qt_ns->addConstant("WA_DropSiteRegistered",    new QoreBigIntNode(Qt::WA_DropSiteRegistered));
   qt_ns->addConstant("WA_ForceAcceptDrops",      new QoreBigIntNode(Qt::WA_ForceAcceptDrops));
   qt_ns->addConstant("WA_WindowPropagation",     new QoreBigIntNode(Qt::WA_WindowPropagation));
   qt_ns->addConstant("WA_NoX11EventCompression", new QoreBigIntNode(Qt::WA_NoX11EventCompression));
   qt_ns->addConstant("WA_TintedBackground",      new QoreBigIntNode(Qt::WA_TintedBackground));
   qt_ns->addConstant("WA_X11OpenGLOverlay",      new QoreBigIntNode(Qt::WA_X11OpenGLOverlay));
   qt_ns->addConstant("WA_AlwaysShowToolTips",    new QoreBigIntNode(Qt::WA_AlwaysShowToolTips));
   qt_ns->addConstant("WA_MacOpaqueSizeGrip",     new QoreBigIntNode(Qt::WA_MacOpaqueSizeGrip));
   qt_ns->addConstant("WA_SetStyle",              new QoreBigIntNode(Qt::WA_SetStyle));
   qt_ns->addConstant("WA_SetLocale",             new QoreBigIntNode(Qt::WA_SetLocale));
   qt_ns->addConstant("WA_MacShowFocusRect",      new QoreBigIntNode(Qt::WA_MacShowFocusRect));
   qt_ns->addConstant("WA_MacNormalSize",         new QoreBigIntNode(Qt::WA_MacNormalSize));
   qt_ns->addConstant("WA_MacSmallSize",          new QoreBigIntNode(Qt::WA_MacSmallSize));
   qt_ns->addConstant("WA_MacMiniSize",           new QoreBigIntNode(Qt::WA_MacMiniSize));
   qt_ns->addConstant("WA_LayoutUsesWidgetRect",  new QoreBigIntNode(Qt::WA_LayoutUsesWidgetRect));
   qt_ns->addConstant("WA_StyledBackground",      new QoreBigIntNode(Qt::WA_StyledBackground));
   qt_ns->addConstant("WA_MSWindowsUseDirect3D",  new QoreBigIntNode(Qt::WA_MSWindowsUseDirect3D));
   qt_ns->addConstant("WA_CanHostQMdiSubWindowTitleBar", new QoreBigIntNode(Qt::WA_CanHostQMdiSubWindowTitleBar));
   qt_ns->addConstant("WA_MacAlwaysShowToolWindow", new QoreBigIntNode(Qt::WA_MacAlwaysShowToolWindow));
   qt_ns->addConstant("WA_StyleSheet",            new QoreBigIntNode(Qt::WA_StyleSheet));
   qt_ns->addConstant("WA_AttributeCount",        new QoreBigIntNode(Qt::WA_AttributeCount));
   
   // WindowType enum
   qt_ns->addConstant("Widget",                   new QoreBigIntNode(Qt::Widget));
   qt_ns->addConstant("Window",                   new QoreBigIntNode(Qt::Window));
   qt_ns->addConstant("Dialog",                   new QoreBigIntNode(Qt::Dialog));
   qt_ns->addConstant("Sheet",                    new QoreBigIntNode(Qt::Sheet));
   qt_ns->addConstant("Drawer",                   new QoreBigIntNode(Qt::Drawer));
   qt_ns->addConstant("Popup",                    new QoreBigIntNode(Qt::Popup));
   qt_ns->addConstant("Tool",                     new QoreBigIntNode(Qt::Tool));
   qt_ns->addConstant("ToolTip",                  new QoreBigIntNode(Qt::ToolTip));
   qt_ns->addConstant("SplashScreen",             new QoreBigIntNode(Qt::SplashScreen));
   qt_ns->addConstant("Desktop",                  new QoreBigIntNode(Qt::Desktop));
   qt_ns->addConstant("SubWindow",                new QoreBigIntNode(Qt::SubWindow));
   qt_ns->addConstant("WindowType_Mask",          new QoreBigIntNode(Qt::WindowType_Mask));
   qt_ns->addConstant("MSWindowsFixedSizeDialogHint", new QoreBigIntNode(Qt::MSWindowsFixedSizeDialogHint));
   qt_ns->addConstant("MSWindowsOwnDC",           new QoreBigIntNode(Qt::MSWindowsOwnDC));
   qt_ns->addConstant("X11BypassWindowManagerHint", new QoreBigIntNode(Qt::X11BypassWindowManagerHint));
   qt_ns->addConstant("FramelessWindowHint",      new QoreBigIntNode(Qt::FramelessWindowHint));
   qt_ns->addConstant("WindowTitleHint",          new QoreBigIntNode(Qt::WindowTitleHint));
   qt_ns->addConstant("WindowSystemMenuHint",     new QoreBigIntNode(Qt::WindowSystemMenuHint));
   qt_ns->addConstant("WindowMinimizeButtonHint", new QoreBigIntNode(Qt::WindowMinimizeButtonHint));
   qt_ns->addConstant("WindowMaximizeButtonHint", new QoreBigIntNode(Qt::WindowMaximizeButtonHint));
   qt_ns->addConstant("WindowMinMaxButtonsHint",  new QoreBigIntNode(Qt::WindowMinMaxButtonsHint));
   qt_ns->addConstant("WindowContextHelpButtonHint", new QoreBigIntNode(Qt::WindowContextHelpButtonHint));
   qt_ns->addConstant("WindowShadeButtonHint",    new QoreBigIntNode(Qt::WindowShadeButtonHint));
   qt_ns->addConstant("WindowStaysOnTopHint",     new QoreBigIntNode(Qt::WindowStaysOnTopHint));
   qt_ns->addConstant("CustomizeWindowHint",      new QoreBigIntNode(Qt::CustomizeWindowHint));

   // FocusPolicy enum
   qt_ns->addConstant("NoFocus",                  new QoreBigIntNode(Qt::NoFocus));
   qt_ns->addConstant("TabFocus",                 new QoreBigIntNode(Qt::TabFocus));
   qt_ns->addConstant("ClickFocus",               new QoreBigIntNode(Qt::ClickFocus));
   qt_ns->addConstant("StrongFocus",              new QoreBigIntNode(Qt::StrongFocus));
   qt_ns->addConstant("WheelFocus",               new QoreBigIntNode(Qt::WheelFocus));

   // ConnectionType enum
   qt_ns->addConstant("AutoConnection",           new QoreBigIntNode(Qt::AutoConnection));
   qt_ns->addConstant("DirectConnection",         new QoreBigIntNode(Qt::DirectConnection));
   qt_ns->addConstant("QueuedConnection",         new QoreBigIntNode(Qt::QueuedConnection));
   qt_ns->addConstant("AutoCompatConnection",     new QoreBigIntNode(Qt::AutoCompatConnection));
   qt_ns->addConstant("BlockingQueuedConnection", new QoreBigIntNode(Qt::BlockingQueuedConnection));

   // DateFormat enum
   qt_ns->addConstant("TextDate",                 new QoreBigIntNode(Qt::TextDate));
   qt_ns->addConstant("ISODate",                  new QoreBigIntNode(Qt::ISODate));
   qt_ns->addConstant("SystemLocaleDate",         new QoreBigIntNode(Qt::SystemLocaleDate));
   qt_ns->addConstant("LocalDate",                new QoreBigIntNode(Qt::LocalDate));
   qt_ns->addConstant("LocaleDate",               new QoreBigIntNode(Qt::LocaleDate));

   // TimeSpec enum
   qt_ns->addConstant("LocalTime",                new QoreBigIntNode(Qt::LocalTime));
   qt_ns->addConstant("UTC",                      new QoreBigIntNode(Qt::UTC));

   // ScrollBarPolicy enum
   qt_ns->addConstant("ScrollBarAsNeeded",        new QoreBigIntNode(Qt::ScrollBarAsNeeded));
   qt_ns->addConstant("ScrollBarAlwaysOff",       new QoreBigIntNode(Qt::ScrollBarAlwaysOff));
   qt_ns->addConstant("ScrollBarAlwaysOn",        new QoreBigIntNode(Qt::ScrollBarAlwaysOn));

   // CaseSensitivity enum
   qt_ns->addConstant("CaseInsensitive",          new QoreBigIntNode(Qt::CaseInsensitive));
   qt_ns->addConstant("CaseSensitive",            new QoreBigIntNode(Qt::CaseSensitive));

   // Corner enum
   qt_ns->addConstant("TopLeftCorner",            new QoreBigIntNode(Qt::TopLeftCorner));
   qt_ns->addConstant("TopRightCorner",           new QoreBigIntNode(Qt::TopRightCorner));
   qt_ns->addConstant("BottomLeftCorner",         new QoreBigIntNode(Qt::BottomLeftCorner));
   qt_ns->addConstant("BottomRightCorner",        new QoreBigIntNode(Qt::BottomRightCorner));

   // ShortcutContext enum
   qt_ns->addConstant("WidgetShortcut",           new QoreBigIntNode(Qt::WidgetShortcut));
   qt_ns->addConstant("WindowShortcut",           new QoreBigIntNode(Qt::WindowShortcut));
   qt_ns->addConstant("ApplicationShortcut",      new QoreBigIntNode(Qt::ApplicationShortcut));

   // FillRule enum
   qt_ns->addConstant("OddEvenFill",              new QoreBigIntNode(Qt::OddEvenFill));
   qt_ns->addConstant("WindingFill",              new QoreBigIntNode(Qt::WindingFill));

   // MaskMode enum
   qt_ns->addConstant("MaskInColor",              new QoreBigIntNode(Qt::MaskInColor));
   qt_ns->addConstant("MaskOutColor",             new QoreBigIntNode(Qt::MaskOutColor));

   // ClipOperation enum
   qt_ns->addConstant("NoClip",                   new QoreBigIntNode(Qt::NoClip));
   qt_ns->addConstant("ReplaceClip",              new QoreBigIntNode(Qt::ReplaceClip));
   qt_ns->addConstant("IntersectClip",            new QoreBigIntNode(Qt::IntersectClip));
   qt_ns->addConstant("UniteClip",                new QoreBigIntNode(Qt::UniteClip));

   // LayoutDirection enum
   qt_ns->addConstant("LeftToRight",              new QoreBigIntNode(Qt::LeftToRight));
   qt_ns->addConstant("RightToLeft",              new QoreBigIntNode(Qt::RightToLeft));

   // ItemSelectionMode
   qt_ns->addConstant("ContainsItemShape",        new QoreBigIntNode(Qt::ContainsItemShape));
   qt_ns->addConstant("IntersectsItemShape",      new QoreBigIntNode(Qt::IntersectsItemShape));
   qt_ns->addConstant("ContainsItemBoundingRect", new QoreBigIntNode(Qt::ContainsItemBoundingRect));
   qt_ns->addConstant("IntersectsItemBoundingRect", new QoreBigIntNode(Qt::IntersectsItemBoundingRect));

   // TransformationMode enum
   qt_ns->addConstant("FastTransformation",       new QoreBigIntNode(Qt::FastTransformation));
   qt_ns->addConstant("SmoothTransformation",     new QoreBigIntNode(Qt::SmoothTransformation));

   // Axis enum
   qt_ns->addConstant("XAxis",                    new QoreBigIntNode(Qt::XAxis));
   qt_ns->addConstant("YAxis",                    new QoreBigIntNode(Qt::YAxis));
   qt_ns->addConstant("ZAxis",                    new QoreBigIntNode(Qt::ZAxis));

   // WindowModality enum
   qt_ns->addConstant("NonModal",                 new QoreBigIntNode(Qt::NonModal));
   qt_ns->addConstant("WindowModal",              new QoreBigIntNode(Qt::WindowModal));
   qt_ns->addConstant("ApplicationModal",         new QoreBigIntNode(Qt::ApplicationModal));

}

static class QoreStringNode *qt_module_init()
{
   // add new types
   addBrushStyleType();
   addPenStyleType();
   
   // initialize namespace (must come after type initialization)
   init_namespace();

   builtinFunctions.add("QObject_connect",            f_QObject_connect);
   builtinFunctions.add("SLOT",                       f_SLOT);
   builtinFunctions.add("SIGNAL",                     f_SIGNAL);
   builtinFunctions.add("TR",                         f_TR);
   builtinFunctions.add("QAPP",                       f_QAPP);
   builtinFunctions.add("qDebug",                     f_qDebug);
   builtinFunctions.add("qWarning",                   f_qWarning);
   builtinFunctions.add("qCritical",                  f_qCritical);
   builtinFunctions.add("qFatal",                     f_qFatal);
   builtinFunctions.add("qRound",                     f_qRound);
   builtinFunctions.add("qsrand",                     f_qsrand);
   builtinFunctions.add("qrand",                      f_qrand);

   // QToolTip static functions
   builtinFunctions.add("QToolTip_font",              f_QToolTip_font);
   builtinFunctions.add("QToolTip_hideText",          f_QToolTip_hideText);
   builtinFunctions.add("QToolTip_palette",           f_QToolTip_palette);
   builtinFunctions.add("QToolTip_setFont",           f_QToolTip_setFont);
   builtinFunctions.add("QToolTip_setPalette",        f_QToolTip_setPalette);
   builtinFunctions.add("QToolTip_showText",          f_QToolTip_showText);

   // QStyleFactory static functions
   builtinFunctions.add("QStyleFactory_create",       f_QStyleFactory_create);
   builtinFunctions.add("QStyleFactory_keys",         f_QStyleFactory_keys);

   // add static class functions as builtin functions
   initQCoreApplicationStaticFunctions();
   initQApplicationStaticFunctions();
   initQLocaleStaticFunctions();
   initQFontDatabaseStaticFunctions();
   initQMessageBoxStaticFunctions();
   initQPixmapStaticFunctions();
   initQFileDialogStaticFunctions();
   initQDirStaticFunctions();
   initQMovieStaticFunctions();
   initQColorDialogStaticFunctions();
   initQInputDialogStaticFunctions();
   initQImageWriterStaticFunctions();
   initQColorStaticFunctions();
   initQTimerStaticFunctions();
   initQSystemTrayIconStaticFunctions();
   initQLibraryInfoStaticFunctions();
   initQFontDialogStaticFunctions();

   return 0;
}

static void qt_module_ns_init(class QoreNamespace *rns, class QoreNamespace *qns)
{
   qns->addInitialNamespace(qt_ns->copy());
}

static void qt_module_delete()
{
   if (C_Clipboard) {
      ExceptionSink xsink;
      C_Clipboard->deref(&xsink);
   }
   delete qt_ns;
}
