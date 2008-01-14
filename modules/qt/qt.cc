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

#include "QT_BrushStyle.h"
#include "QT_PenStyle.h"

#include "qore-qt.h"

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>

#include <assert.h>

QoreType *NT_BRUSHSTYLE = 0, *NT_PENSTYLE = 0;

QoreNode *C_Clipboard = 0;

static class QoreString *qt_module_init();
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

int get_qdate(const QoreNode *n, QDate &date, class ExceptionSink *xsink)
{
   if (n && n->type == NT_DATE) {
      date.setDate(n->val.date_time->getYear(), n->val.date_time->getMonth(), n->val.date_time->getDay());
      return 0;
   }
   
   class QoreQDate *qd = (n && n->type == NT_OBJECT) ? (QoreQDate *)n->val.object->getReferencedPrivateData(CID_QDATE, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qd) {
      class QoreQDateTime *qdt = (n && n->type == NT_OBJECT) ? (QoreQDateTime *)n->val.object->getReferencedPrivateData(CID_QDATETIME, xsink) : 0;
      if (!qdt) {
	 if (!*xsink) {
	    if (n && n->type == NT_OBJECT) 
	       xsink->raiseException("DATE-ERROR", "class '%s' is not derived from QDate or QDateTime", n->val.object->getClass()->getName());
	    else
	       xsink->raiseException("DATE-ERROR", "cannot convert type '%s' to QDate", n ? n->type->getName() : "NOTHING");
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

int get_qdatetime(const QoreNode *n, QDateTime &dt, class ExceptionSink *xsink)
{
   if (n) {
      if (n->type == NT_DATE) {
	 DateTime *qdt = n->val.date_time;
	 dt.setDate(QDate(qdt->getYear(), qdt->getMonth(), qdt->getDay()));
	 dt.setTime(QTime(qdt->getHour(), qdt->getMinute(), qdt->getSecond(), qdt->getMillisecond()));
	 return 0;
      }
   
      if (n->type == NT_OBJECT) {
	 class QoreQDate *qd = (QoreQDate *)n->val.object->getReferencedPrivateData(CID_QDATE, xsink);
	 if (*xsink)
	    return -1;
	 if (!qd) {
	    class QoreQDateTime *qdt = (QoreQDateTime *)n->val.object->getReferencedPrivateData(CID_QDATETIME, xsink);
	    if (*xsink)
	       return -1;
	    if (!qdt) {
	       class QoreQTime *qt = (QoreQTime *)n->val.object->getReferencedPrivateData(CID_QTIME, xsink);
	       if (*xsink)
		  return -1;
	       if (!qt)
		  xsink->raiseException("DATETIME-ERROR", "class '%s' is not derived from QDate, QTime, or QDateTime", n->val.object->getClass()->getName());
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

   xsink->raiseException("QDATETIME-ERROR", "cannot derive QDateTime value from type '%s'", n ? n->type->getName() : "NOTHING");
   return -1;
}

int get_qtime(const QoreNode *n, QTime &time, class ExceptionSink *xsink)
{
   if (n && n->type == NT_DATE) {
      DateTime *qdt = n->val.date_time;
      time.setHMS(qdt->getHour(), qdt->getMinute(), qdt->getSecond(), qdt->getMillisecond());
      return 0;
   }
   
   class QoreQTime *qt = (n && n->type == NT_OBJECT) ? (QoreQTime *)n->val.object->getReferencedPrivateData(CID_QTIME, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qt) {
      class QoreQDateTime *qdt = (n && n->type == NT_OBJECT) ? (QoreQDateTime *)n->val.object->getReferencedPrivateData(CID_QDATETIME, xsink) : 0;
      if (!qdt) {
	 if (!*xsink) {
	    if (n && n->type == NT_OBJECT) 
	       xsink->raiseException("QTIME-ERROR", "class '%s' is not derived from QTime or QDateTime", n->val.object->getClass()->getName());
	    else
	       xsink->raiseException("QTIME-ERROR", "cannot convert type '%s' to QTime", n ? n->type->getName() : "NOTHING");
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

int get_qbytearray(const QoreNode *n, QByteArray &ba, class ExceptionSink *xsink, bool suppress_exception)
{
   if (n && n->type == NT_OBJECT) {
      class QoreQByteArray *qba = (QoreQByteArray *)n->val.object->getReferencedPrivateData(CID_QBYTEARRAY, xsink);
      if (*xsink)
	 return 0;
      if (!qba) {
	 if (!suppress_exception)
	    xsink->raiseException("QBYTEARRAY-ERROR", "class '%s' is not derived from QByteArray", n->val.object->getClass()->getName());
	 return -1;
      }
      ReferenceHolder<QoreQByteArray> qbaHolder(qba, xsink);
      ba = *qba;
      return 0;
   }
   if (n && n->type == NT_BINARY) {
      QByteArray nba((const char *)n->val.bin->getPtr(), n->val.bin->size());
      ba = nba;
      return 0;
   }
   
   {
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (str) {
	 ba.clear();
	 ba.append(str->getBuffer());
	 return 0;
      }
   }

   if (!suppress_exception)
      xsink->raiseException("QBYTEARRAY-ERROR", "cannot convert type '%s' to QByteArray", n ? n->type->getName() : "NOTHING");
   return -1;
}

int get_qvariant(const QoreNode *n, QVariant &qva, class ExceptionSink *xsink, bool suppress_exception)
{
   //printd(5, "get_variant() n=%08p %s\n", n, n ? n->type->getName() : "n/a");
   if (n) {
      if (n->type == NT_OBJECT) {
	 class QoreQVariant *qv = (QoreQVariant *)n->val.object->getReferencedPrivateData(CID_QVARIANT, xsink);
	 if (*xsink)
	    return -1;
	 if (qv) {
	    ReferenceHolder<QoreQVariant> qvHolder(qv, xsink);
	    qva = *qv;
	    return 0;
	 }
	 class QoreQLocale *qlocale = (QoreQLocale *)n->val.object->getReferencedPrivateData(CID_QLOCALE, xsink);
	 if (*xsink)
	    return -1;
	 if (qlocale) {
	    ReferenceHolder<QoreQLocale> qlocaleHolder(qlocale, xsink);
	    qva = *qlocale;
	    return 0;
	 }
	 if (!suppress_exception)
	    xsink->raiseException("QVARIANT-ERROR", "cannot convert class '%s' to QVariant", n->val.object->getClass()->getName());
	 return -1;
      }

      {
	 const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
	 if (str) {
	    QVariant n_qv(str->getBuffer());
	    qva = n_qv;
	    return 0;
	 }
      }

      if (n->type == NT_INT) {
	 if (n->val.intval <= 0x7fffffff)
	    qva.setValue((int)n->val.intval);
	 else
	    qva.setValue(n->val.intval);
	 //printd(5, "qvariant integer %d (%d)\n", (int)n->val.intval, qva.toInt());
	 return 0;
      }
      if (n->type == NT_FLOAT) {
	 qva.setValue(n->val.floatval);
	 return 0;
      }
   }
   if (!suppress_exception)
      xsink->raiseException("QVARIANT-ERROR", "cannot convert type '%s' to QVariant", n ? n->type->getName() : "NOTHING");
   return -1;
}

int get_qchar(const QoreNode *n, QChar &c, class ExceptionSink *xsink, bool suppress_exception)
{
   {
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (str) {
	 unsigned int unicode = str->getUnicodePoint(0, xsink);
	 if (*xsink)
	    return -1;
	 QChar tmp(unicode);
	 c = tmp;
	 return 0;
      }
   }

   class QoreQChar *qc = (n && n->type == NT_OBJECT) ? (QoreQChar *)n->val.object->getReferencedPrivateData(CID_QCHAR, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qc) {
      if (!suppress_exception) {
	 if (n && n->type == NT_OBJECT) 
	    xsink->raiseException("QCHAR-ERROR", "class '%s' is not derived from QChar", n->val.object->getClass()->getName());
	 else
	    xsink->raiseException("QCHAR-ERROR", "cannot convert type '%s' to QChar", n ? n->type->getName() : "NOTHING");
      }
      return -1;
   }

   ReferenceHolder<QoreQChar> cHolder(qc, xsink);
   c = *qc;
   return 0;
}

int get_qstring(const QoreNode *n, QString &str, class ExceptionSink *xsink, bool suppress_exception)
{
   {
      const QoreStringNode *pstr = dynamic_cast<const QoreStringNode *>(n);
      if (pstr) {
	 if (pstr->getEncoding() == QCS_ISO_8859_1) {
	    str = QString::fromLatin1(pstr->getBuffer());
	 }
	 else if (pstr->getEncoding() == QCS_USASCII) {
	    str = QString::fromAscii(pstr->getBuffer());
	 }
	 else {
	    ConstTempEncodingHelper estr(pstr, QCS_UTF8, xsink);
	    if (*xsink)
	       return -1;
	    
	    str = QString::fromUtf8(estr->getBuffer());
	 }
	 return 0;
      }
   }

   if (!suppress_exception) {
      if (n && n->type == NT_INT) {
	 str.setNum(n->val.intval);
	 return 0;
      }
      if (n && n->type == NT_FLOAT) {
	 str.setNum(n->val.floatval);
	 return 0;
      }
   }

   class QoreQChar *qc = (n && n->type == NT_OBJECT) ? (QoreQChar *)n->val.object->getReferencedPrivateData(CID_QCHAR, xsink) : 0;
   if (*xsink)
      return -1;
   if (!qc) {
      class QoreQVariant *qv = (n && n->type == NT_OBJECT) ? (QoreQVariant *)n->val.object->getReferencedPrivateData(CID_QVARIANT, xsink) : 0;
      if (*xsink)
	 return -1;
      if (!qv) {
	 if (!suppress_exception) {
	    if (n && n->type == NT_OBJECT) 
	       xsink->raiseException("QSTRING-ERROR", "class '%s' is not derived from QChar or QVariant", n->val.object->getClass()->getName());
	    else
	       xsink->raiseException("QSTRING-ERROR", "cannot convert type '%s' to QString", n ? n->type->getName() : "NOTHING");
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

int get_qkeysequence(const QoreNode *n, QKeySequence &ks, class ExceptionSink *xsink, bool suppress_exception)
{
   if (n && n->type == NT_OBJECT) {
      class QoreQKeySequence *qks = (QoreQKeySequence *)n->val.object->getReferencedPrivateData(CID_QKEYSEQUENCE, xsink);
      if (*xsink)
	 return 0;
      if (!qks) {
	 if (!suppress_exception)
	    xsink->raiseException("QKEYSEQUENCE-ERROR", "class '%s' is not derived from QKeySequence", n->val.object->getClass()->getName());
	 return -1;
      }
      ReferenceHolder<QoreQKeySequence> qksHolder(qks, xsink);
      ks = *qks;
      return 0;
   }
   if (n && n->type == NT_STRING) {
      QString str;
      get_qstring(n, str, xsink);
      if (*xsink)
	 return -1;

      ks = str;
      return 0;
   }
   if (n && n->type == NT_INT) {
      QKeySequence::StandardKey key = (QKeySequence::StandardKey)(const_cast<QoreNode *>(n))->getAsInt();
      ks = key;
      return 0;
   }
   if (!suppress_exception)
      xsink->raiseException("QKEYSEQUENCE-ERROR", "cannot convert type '%s' to QKeySequence", n ? n->type->getName() : "NOTHING");
   return -1;
}

int get_qbrush(const QoreNode *n, QBrush &brush, class ExceptionSink *xsink)
{
   //printd(5, "get_qbrush(n=%08p '%s' '%s')\n", n, n ? n->type->getName() : "n/a", n && n->type == NT_OBJECT ? n->val.object->getClass()->getName() : "n/a");
   if (n) {
      if (n->type == NT_OBJECT) {
	 class QoreQBrush *qb = (QoreQBrush *)n->val.object->getReferencedPrivateData(CID_QBRUSH, xsink);
	 if (*xsink)
	    return -1;
	 if (!qb) {
	    class QoreQPixmap *pixmap = (QoreQPixmap *)n->val.object->getReferencedPrivateData(CID_QPIXMAP, xsink);
	    if (*xsink)
	       return -1;
	    if (!pixmap) {
	       class QoreQImage *image = (QoreQImage *)n->val.object->getReferencedPrivateData(CID_QIMAGE, xsink);
	       if (*xsink)
		  return -1;
	       if (!image) {
		  class QoreQColor *color = (QoreQColor *)n->val.object->getReferencedPrivateData(CID_QCOLOR, xsink);
		  if (*xsink)
		     return -1;
		  if (!color) {
		     xsink->raiseException("QBRUSH-ERROR", "class '%s' cannot produce a QBrush object", n->val.object->getClass()->getName());
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
      if (n->type == NT_BRUSHSTYLE) {
	 brush = QBrush((Qt::BrushStyle)n->val.intval);
	 return 0;
      }
      // assume Qt::GlobalColor enum
      if (n->type == NT_INT) {
	 brush = QBrush((Qt::GlobalColor)n->val.intval);
	 return 0;
      }
   }
   xsink->raiseException("QBRUSH-ERROR", "cannot derive QBrush object from type %s", n ? n->type->getName() : "NOTHING");
   return -1;
}

class QoreNode *return_object(QoreClass *qclass, AbstractPrivateData *data)
{
   QoreObject *qore_object = new QoreObject(qclass, getProgram());
   qore_object->setPrivate(qclass->getID(), data);
   return new QoreNode(qore_object);
}

class QoreNode *return_qstyle(const QString &style, QStyle *qs, ExceptionSink *xsink)
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
      return new QoreNode(obj);
   }

   QPlastiqueStyle *qps = dynamic_cast<QPlastiqueStyle *>(qs);
   if (qps) {
      qc = QC_QPlastiqueStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQPlastiqueStyle(obj, qps));
      return new QoreNode(obj);
   }

#ifdef WINDOWS
   QWindowsXPStyle *qxps = dynamic_cast<QWindowsXPStyle *>(qs);
   if (qxps) {
      qc = QC_QWindowsXPStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsXPStyle(obj, qxps));
      return new QoreNode(obj);
   }
#endif

#ifdef DARWIN
   QMacStyle *qms = dynamic_cast<QMacStyle *>(qs);
   if (qms) {
      qc = QC_QMacStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMacStyle(obj, qms));
      return new QoreNode(obj);
   }
#endif

   QWindowsStyle *qws = dynamic_cast<QWindowsStyle *>(qs);
   if (qws) {
      qc = QC_QWindowsStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsStyle(obj, qws));
      return new QoreNode(obj);
   }

   QCDEStyle *qcs = dynamic_cast<QCDEStyle *>(qs);
   if (qcs) {
      qc = QC_QCDEStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQCDEStyle(obj, qcs));
      return new QoreNode(obj);
   }

   QMotifStyle *qmts = dynamic_cast<QMotifStyle *>(qs);
   if (qmts) {
      qc = QC_QMotifStyle;
      obj = new QoreObject(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMotifStyle(obj, qmts));
      return new QoreNode(obj);
   }

   // otherwise return a QStyle object
   obj = new QoreObject(QC_QStyle, getProgram());
   obj->setPrivate(CID_QSTYLE, new QoreQtQStyle(obj, qs));
   return new QoreNode(obj);
}

class QoreNode *return_qstyleoption(const QStyleOption *qso)
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

class QoreNode *return_qvariant(const QVariant &qv)
{
   QVariant::Type type = qv.type();
   switch (type) {
      case QVariant::Invalid:
	 return nothing();
      case QVariant::Bitmap:
	 return return_object(QC_QBitmap, new QoreQBitmap(qv.value<QBitmap>()));
      case QVariant::Bool:
	 return new QoreNode(qv.toBool());
      case QVariant::Brush:
	 return return_object(QC_QBrush, new QoreQBrush(qv.value<QBrush>()));
      case QVariant::Color:
	 return return_object(QC_QColor, new QoreQColor(qv.value<QColor>()));
      case QVariant::Date: 
	 return new QoreNode(new DateTime(qv.toDate().year(), qv.toDate().month(), qv.toDate().day()));
      case QVariant::DateTime: {
	 QDate rv_d = qv.toDateTime().date();
	 QTime rv_t = qv.toDateTime().time();
	 return new QoreNode(new DateTime(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), 
					  rv_t.minute(), rv_t.second(), rv_t.msec()));
      }
      case QVariant::Double:
	 return new QoreNode(qv.toDouble());
      case QVariant::Font:
	 return return_object(QC_QFont, new QoreQFont(qv.value<QFont>()));
      case QVariant::Icon:
	 return return_object(QC_QIcon, new QoreQIcon(qv.value<QIcon>()));
      case QVariant::Image:
	 return return_object(QC_QImage, new QoreQImage(qv.value<QImage>()));
      case QVariant::Int:
	 return new QoreNode((int64)qv.toInt());
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
	 return new QoreNode((int64)qv.toLongLong());
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
	 return new QoreNode(new DateTime(1970, 1, 1, qv.toTime().hour(), qv.toTime().minute(), 
					  qv.toTime().second(), qv.toTime().msec()));
      //case QVariant::Transform:
         //return return_object(QC_QVariant::Transform, new QTransform(qv.value<QVariant::Transform>()));
      case QVariant::UInt:
	 return new QoreNode((int64)qv.toUInt());
      case QVariant::ULongLong:
	 return new QoreNode((int64)qv.toULongLong());
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
static QoreNode *return_qwidget_intern(QWidget *w)
{
   // assign as QWidget
   QoreObject *qo = new QoreObject(QC_QWidget, getProgram());
   qo->setPrivate(CID_QWIDGET, new QoreQtQWidget(qo, w));
   return new QoreNode(qo);
}

// returns a QoreNode tagged as the appropriate QObject subclass
class QoreNode *return_qobject(QObject *o)
{
   if (!o)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = o->property("qobject");
   QoreObject *qo = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return new QoreNode(qo);
   }

   // see what subclass it is
   QWidget *qw = dynamic_cast<QWidget *>(o);
   if (qw)
      return return_qwidget_intern(qw);

   // assign as QObject
   qo = new QoreObject(QC_QObject, getProgram());
   qo->setPrivate(CID_QOBJECT, new QoreQtQObject(qo, o));
   return new QoreNode(qo);
}

// returns a QoreNode tagged as the appropriate QWidget subclass
class QoreNode *return_qwidget(QWidget *w)
{
   if (!w)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = w->property("qobject");
   QoreObject *qo = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return new QoreNode(qo);
   }

   return return_qwidget_intern(w);
}

class QoreNode *return_qaction(QAction *action)
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
   return new QoreNode(rv_obj);
}

class QoreNode *return_qevent(QEvent *event)
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

static class QoreNode *f_QObject_connect(const QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   class AbstractPrivateData *spd = p ? p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
   QoreAbstractQObject *sender = spd ? dynamic_cast<QoreAbstractQObject *>(spd) : 0;
   assert(!spd || sender);
   if (!sender) {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "first argument is not a QObject");
      return 0;
   }
   ReferenceHolder<AbstractPrivateData> holder1(spd, xsink);

   QoreStringNode *str = test_string_param(params, 1);
   if (!str)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing signal string as second argument");
      return 0;
   }
   const char *signal = str->getBuffer();

   p = get_param(params, 2);
   if (!p || p->type != NT_OBJECT)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing receiving object as third argument");
      return 0;      
   }
   class AbstractPrivateData *rpd = p ? p->val.object->getReferencedPrivateData(CID_QOBJECT, xsink) : NULL;
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
   return new QoreNode(b);
   */
   receiver->connectDynamic(sender, signal, member, xsink);
   return 0;
}

static class QoreNode *f_SLOT(const QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreStringNode *p = test_string_param(params, 0);
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

static class QoreNode *f_SIGNAL(const QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreStringNode *p = test_string_param(params, 0);
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

static class QoreNode *f_TR(const QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreStringNode *p = test_string_param(params, 0);
   if (!p || !p->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreStringNode(QObject::tr(p->getBuffer()).toUtf8().data(), QCS_UTF8);
}

static class QoreNode *f_QAPP(const QoreNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}

static class QoreNode *f_qDebug(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qDebug(str->getBuffer());
   return 0;
}

static class QoreNode *f_qWarning(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qWarning(str->getBuffer());
   return 0;
}

static class QoreNode *f_qCritical(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qCritical(str->getBuffer());
   return 0;
}

static class QoreNode *f_qFatal(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreString *str = q_sprintf(params, 0, 0, xsink);
   if (*xsink) {
      if (str)
	 delete str;
   }
   else
      qFatal(str->getBuffer());
   return 0;
}

static class QoreNode *f_qRound(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   return new QoreNode((int64)qRound(p ? p->getAsFloat() : 0.0));
}

static class QoreNode *f_qsrand(const QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qsrand(p ? p->getAsInt() : 0);
   return 0;
}

static class QoreNode *f_qrand(const QoreNode *params, class ExceptionSink *xsink)
{
   return new QoreNode((int64)qrand());
}

static QoreNode *f_QToolTip_font(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(QToolTip::font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//void hideText ()
static QoreNode *f_QToolTip_hideText(const QoreNode *params, ExceptionSink *xsink)
{
   QToolTip::hideText();
   return 0;
}

//QPalette palette ()
static QoreNode *f_QToolTip_palette(const QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(QToolTip::palette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
}

//void setFont ( const QFont & font )
static QoreNode *f_QToolTip_setFont(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQFont *font = (p && p->type == NT_OBJECT) ? (QoreQFont *)p->val.object->getReferencedPrivateData(CID_QFONT, xsink) : 0;
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
static QoreNode *f_QToolTip_setPalette(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
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
static class QoreNode *f_QToolTip_showText(const QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() was expecting a QPoint as the first argument");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   QoreStringNode *str = test_string_param(params, 1);
   if (!str) {
      xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "expecting a string as second argument to QToolTip_showText()");
      return 0;
   }
   const char *text = str->getBuffer();

   p = test_param(params, NT_OBJECT, 2);
   QoreQWidget *w = p ? (QoreQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (p && !w) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the third argument", p->val.object->getClass()->getName());
      return 0;
   }
   ReferenceHolder<QoreQWidget> wHolder(w, xsink);

   QoreQRect *rect = 0;
   if (w) {
      p = test_param(params, NT_OBJECT, 3);
      rect = p ? (QoreQRect *)p->val.object->getReferencedPrivateData(CID_QRECT, xsink) : 0;
      if (!rect && p) {
	 if (!xsink->isException())
	    xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "this version of QToolTip_showText() does not know how to handle arguments of class '%s' as passed as the fourth argument", p->val.object->getClass()->getName());
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
static QoreNode *f_QStyleFactory_create(const QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;

   return return_qstyle(key, QStyleFactory::create(key), xsink);
}

//QStringList keys ()
static QoreNode *f_QStyleFactory_keys(const QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QStyleFactory::keys();
   QoreList *l = new QoreList();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreStringNode((*i).toUtf8().data(), QCS_UTF8));
   return new QoreNode(l);
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
   qimage->addConstant("InvertRgb",                new QoreNode((int64)QImage::InvertRgb));
   qimage->addConstant("InvertRgba",               new QoreNode((int64)QImage::InvertRgba));

   // Format enum
   qimage->addConstant("Format_Invalid",           new QoreNode((int64)QImage::Format_Invalid));
   qimage->addConstant("Format_Mono",              new QoreNode((int64)QImage::Format_Mono));
   qimage->addConstant("Format_MonoLSB",           new QoreNode((int64)QImage::Format_MonoLSB));
   qimage->addConstant("Format_Indexed8",          new QoreNode((int64)QImage::Format_Indexed8));
   qimage->addConstant("Format_RGB32",             new QoreNode((int64)QImage::Format_RGB32));
   qimage->addConstant("Format_ARGB32",            new QoreNode((int64)QImage::Format_ARGB32));
   qimage->addConstant("Format_ARGB32_Premultiplied", new QoreNode((int64)QImage::Format_ARGB32_Premultiplied));
   qimage->addConstant("Format_RGB16",             new QoreNode((int64)QImage::Format_RGB16));

   qimage->addSystemClass(initQImageClass(qpaintdevice));

   qt_ns->addInitialNamespace(qimage);

   QoreNamespace *qregion = new QoreNamespace("QRegion");
   
   // RegionType enum
   qregion->addConstant("Rectangle",                new QoreNode((int64)QRegion::Rectangle));
   qregion->addConstant("Ellipse",                  new QoreNode((int64)QRegion::Ellipse));

   qregion->addSystemClass(initQRegionClass());

   qt_ns->addInitialNamespace(qregion);

   QoreNamespace *qlayout_ns = new QoreNamespace("QLayout");

   qlayout_ns->addSystemClass((qlayout = initQLayoutClass(qobject)));
   qlayout_ns->addSystemClass(initQGridLayoutClass(qlayout));

   qlayout_ns->addSystemClass((qboxlayout = initQBoxLayoutClass(qlayout)));
   qlayout_ns->addSystemClass(initQVBoxLayoutClass(qboxlayout));
   qlayout_ns->addSystemClass(initQHBoxLayoutClass(qboxlayout));

   qlayout_ns->addConstant("SetNoConstraint",          new QoreNode((int64)QLayout::SetNoConstraint));
   qlayout_ns->addConstant("SetMinimumSize",           new QoreNode((int64)QLayout::SetMinimumSize));
   qlayout_ns->addConstant("SetFixedSize",             new QoreNode((int64)QLayout::SetFixedSize));
   qlayout_ns->addConstant("SetMaximumSize",           new QoreNode((int64)QLayout::SetMaximumSize));
   qlayout_ns->addConstant("SetMinAndMaxSize",         new QoreNode((int64)QLayout::SetMinAndMaxSize));

   qt_ns->addInitialNamespace(qlayout_ns);

   QoreNamespace *qmovie = new QoreNamespace("QMovie");

   // MovieState enum
   qmovie->addConstant("NotRunning",               new QoreNode((int64)QMovie::NotRunning));
   qmovie->addConstant("Paused",                   new QoreNode((int64)QMovie::Paused));
   qmovie->addConstant("Running",                  new QoreNode((int64)QMovie::Running));

   // CacheMode enum
   qmovie->addConstant("CacheNone",                new QoreNode((int64)QMovie::CacheNone));
   qmovie->addConstant("CacheAll",                 new QoreNode((int64)QMovie::CacheAll));

   qmovie->addSystemClass(initQMovieClass(qobject));

   qt_ns->addInitialNamespace(qmovie);

   QoreNamespace *qslider = new QoreNamespace("QSlider");

   // TickPosition enum
   qslider->addConstant("NoTicks",                  new QoreNode((int64)QSlider::NoTicks));
   qslider->addConstant("TicksAbove",               new QoreNode((int64)QSlider::TicksAbove));
   qslider->addConstant("TicksLeft",                new QoreNode((int64)QSlider::TicksLeft));
   qslider->addConstant("TicksBelow",               new QoreNode((int64)QSlider::TicksBelow));
   qslider->addConstant("TicksRight",               new QoreNode((int64)QSlider::TicksRight));
   qslider->addConstant("TicksBothSides",           new QoreNode((int64)QSlider::TicksBothSides));

   qslider->addSystemClass(initQSliderClass(qabstractslider));

   qt_ns->addInitialNamespace(qslider);

   QoreNamespace *qsizepolicy = new QoreNamespace("QSizePolicy");

   // PolicyFlag enum
   qsizepolicy->addConstant("GrowFlag",                 new QoreNode((int64)QSizePolicy::GrowFlag));
   qsizepolicy->addConstant("ExpandFlag",               new QoreNode((int64)QSizePolicy::ExpandFlag));
   qsizepolicy->addConstant("ShrinkFlag",               new QoreNode((int64)QSizePolicy::ShrinkFlag));
   qsizepolicy->addConstant("IgnoreFlag",               new QoreNode((int64)QSizePolicy::IgnoreFlag));

   // Policy enum
   qsizepolicy->addConstant("Fixed",                    new QoreNode((int64)QSizePolicy::Fixed));
   qsizepolicy->addConstant("Minimum",                  new QoreNode((int64)QSizePolicy::Minimum));
   qsizepolicy->addConstant("Maximum",                  new QoreNode((int64)QSizePolicy::Maximum));
   qsizepolicy->addConstant("Preferred",                new QoreNode((int64)QSizePolicy::Preferred));
   qsizepolicy->addConstant("MinimumExpanding",         new QoreNode((int64)QSizePolicy::MinimumExpanding));
   qsizepolicy->addConstant("Expanding",                new QoreNode((int64)QSizePolicy::Expanding));
   qsizepolicy->addConstant("Ignored",                  new QoreNode((int64)QSizePolicy::Ignored));

   // ControlType enum
   qsizepolicy->addConstant("DefaultType",              new QoreNode((int64)QSizePolicy::DefaultType));
   qsizepolicy->addConstant("ButtonBox",                new QoreNode((int64)QSizePolicy::ButtonBox));
   qsizepolicy->addConstant("CheckBox",                 new QoreNode((int64)QSizePolicy::CheckBox));
   qsizepolicy->addConstant("ComboBox",                 new QoreNode((int64)QSizePolicy::ComboBox));
   qsizepolicy->addConstant("Frame",                    new QoreNode((int64)QSizePolicy::Frame));
   qsizepolicy->addConstant("GroupBox",                 new QoreNode((int64)QSizePolicy::GroupBox));
   qsizepolicy->addConstant("Label",                    new QoreNode((int64)QSizePolicy::Label));
   qsizepolicy->addConstant("Line",                     new QoreNode((int64)QSizePolicy::Line));
   qsizepolicy->addConstant("LineEdit",                 new QoreNode((int64)QSizePolicy::LineEdit));
   qsizepolicy->addConstant("PushButton",               new QoreNode((int64)QSizePolicy::PushButton));
   qsizepolicy->addConstant("RadioButton",              new QoreNode((int64)QSizePolicy::RadioButton));
   qsizepolicy->addConstant("Slider",                   new QoreNode((int64)QSizePolicy::Slider));
   qsizepolicy->addConstant("SpinBox",                  new QoreNode((int64)QSizePolicy::SpinBox));
   qsizepolicy->addConstant("TabWidget",                new QoreNode((int64)QSizePolicy::TabWidget));
   qsizepolicy->addConstant("ToolButton",               new QoreNode((int64)QSizePolicy::ToolButton));

   qt_ns->addInitialNamespace(qsizepolicy);

   qt_ns->addInitialNamespace(initQLibraryInfoNS());

   QoreNamespace *qicon = new QoreNamespace("QIcon");

   // Mode enum
   qicon->addConstant("Normal",                   new QoreNode((int64)QIcon::Normal));
   qicon->addConstant("Disabled",                 new QoreNode((int64)QIcon::Disabled));
   qicon->addConstant("Active",                   new QoreNode((int64)QIcon::Active));
   qicon->addConstant("Selected",                 new QoreNode((int64)QIcon::Selected));

   // State enum
   qicon->addConstant("On",                       new QoreNode((int64)QIcon::On));
   qicon->addConstant("Off",                      new QoreNode((int64)QIcon::Off));

   qicon->addSystemClass(initQIconClass());
   qt_ns->addInitialNamespace(qicon);

   qt_ns->addInitialNamespace(initQPaletteNS());

   QoreNamespace *qpainter_ns = new QoreNamespace("QPainter");
   
   // RenderHint enum
   qpainter_ns->addConstant("Antialiasing",             new QoreNode((int64)QPainter::Antialiasing));
   qpainter_ns->addConstant("TextAntialiasing",         new QoreNode((int64)QPainter::TextAntialiasing));
   qpainter_ns->addConstant("SmoothPixmapTransform",    new QoreNode((int64)QPainter::SmoothPixmapTransform));
   qpainter_ns->addConstant("HighQualityAntialiasing",  new QoreNode((int64)QPainter::HighQualityAntialiasing));
   
   // CompositionMode enum
   qpainter_ns->addConstant("CompositionMode_SourceOver",      new QoreNode((int64)QPainter::CompositionMode_SourceOver));
   qpainter_ns->addConstant("CompositionMode_DestinationOver", new QoreNode((int64)QPainter::CompositionMode_DestinationOver));
   qpainter_ns->addConstant("CompositionMode_Clear",           new QoreNode((int64)QPainter::CompositionMode_Clear));
   qpainter_ns->addConstant("CompositionMode_Source",          new QoreNode((int64)QPainter::CompositionMode_Source));
   qpainter_ns->addConstant("CompositionMode_Destination",     new QoreNode((int64)QPainter::CompositionMode_Destination));
   qpainter_ns->addConstant("CompositionMode_SourceIn",        new QoreNode((int64)QPainter::CompositionMode_SourceIn));
   qpainter_ns->addConstant("CompositionMode_DestinationIn",   new QoreNode((int64)QPainter::CompositionMode_DestinationIn));
   qpainter_ns->addConstant("CompositionMode_SourceOut",       new QoreNode((int64)QPainter::CompositionMode_SourceOut));
   qpainter_ns->addConstant("CompositionMode_DestinationOut",  new QoreNode((int64)QPainter::CompositionMode_DestinationOut));
   qpainter_ns->addConstant("CompositionMode_SourceAtop",      new QoreNode((int64)QPainter::CompositionMode_SourceAtop));
   qpainter_ns->addConstant("CompositionMode_DestinationAtop", new QoreNode((int64)QPainter::CompositionMode_DestinationAtop));
   qpainter_ns->addConstant("CompositionMode_Xor",             new QoreNode((int64)QPainter::CompositionMode_Xor));
   qpainter_ns->addConstant("CompositionMode_Plus",            new QoreNode((int64)QPainter::CompositionMode_Plus));
   qpainter_ns->addConstant("CompositionMode_Multiply",        new QoreNode((int64)QPainter::CompositionMode_Multiply));
   qpainter_ns->addConstant("CompositionMode_Screen",          new QoreNode((int64)QPainter::CompositionMode_Screen));
   qpainter_ns->addConstant("CompositionMode_Overlay",         new QoreNode((int64)QPainter::CompositionMode_Overlay));
   qpainter_ns->addConstant("CompositionMode_Darken",          new QoreNode((int64)QPainter::CompositionMode_Darken));
   qpainter_ns->addConstant("CompositionMode_Lighten",         new QoreNode((int64)QPainter::CompositionMode_Lighten));
   qpainter_ns->addConstant("CompositionMode_ColorDodge",      new QoreNode((int64)QPainter::CompositionMode_ColorDodge));
   qpainter_ns->addConstant("CompositionMode_ColorBurn",       new QoreNode((int64)QPainter::CompositionMode_ColorBurn));
   qpainter_ns->addConstant("CompositionMode_HardLight",       new QoreNode((int64)QPainter::CompositionMode_HardLight));
   qpainter_ns->addConstant("CompositionMode_SoftLight",       new QoreNode((int64)QPainter::CompositionMode_SoftLight));
   qpainter_ns->addConstant("CompositionMode_Difference",      new QoreNode((int64)QPainter::CompositionMode_Difference));
   qpainter_ns->addConstant("CompositionMode_Exclusion",       new QoreNode((int64)QPainter::CompositionMode_Exclusion));

   qpainter_ns->addSystemClass(initQPainterClass());

   qt_ns->addInitialNamespace(qpainter_ns);

   QoreClass *qabstractbutton, *qtextformat, *qtextframeformat, *qtextcharformat,
      *qstyleoption, *qstyleoptionviewitem, *qabstractitemdelegate,
      *qabstractspinbox, *qdatetimeedit, *qabstractscrollarea, 
      *qcombobox, *qstyleoptioncomplex, *qabstractitemview, 
      *qtableview, *qdialog, *qvalidator;
 
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
   qt_ns->addSystemClass(initQDirClass());
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

   // add QBoxLayout namespace and constants
   class QoreNamespace *qbl = new QoreNamespace("QBoxLayout");

   // Direction enum
   qbl->addConstant("LeftToRight",    new QoreNode((int64)QBoxLayout::LeftToRight));
   qbl->addConstant("RightToLeft",    new QoreNode((int64)QBoxLayout::RightToLeft));
   qbl->addConstant("TopToBottom",    new QoreNode((int64)QBoxLayout::TopToBottom));
   qbl->addConstant("BottomToTop",    new QoreNode((int64)QBoxLayout::BottomToTop));

   qt_ns->addInitialNamespace(qbl);

   qt_ns->addInitialNamespace(initQSystemTrayIconNS(qobject));

   QoreNamespace *qdatetimeedit_ns = new QoreNamespace("QDateTimeEdit");
   
   // Section enum
   qdatetimeedit_ns->addConstant("NoSection",                new QoreNode((int64)QDateTimeEdit::NoSection));
   qdatetimeedit_ns->addConstant("AmPmSection",              new QoreNode((int64)QDateTimeEdit::AmPmSection));
   qdatetimeedit_ns->addConstant("MSecSection",              new QoreNode((int64)QDateTimeEdit::MSecSection));
   qdatetimeedit_ns->addConstant("SecondSection",            new QoreNode((int64)QDateTimeEdit::SecondSection));
   qdatetimeedit_ns->addConstant("MinuteSection",            new QoreNode((int64)QDateTimeEdit::MinuteSection));
   qdatetimeedit_ns->addConstant("HourSection",              new QoreNode((int64)QDateTimeEdit::HourSection));
   qdatetimeedit_ns->addConstant("DaySection",               new QoreNode((int64)QDateTimeEdit::DaySection));
   qdatetimeedit_ns->addConstant("MonthSection",             new QoreNode((int64)QDateTimeEdit::MonthSection));
   qdatetimeedit_ns->addConstant("YearSection",              new QoreNode((int64)QDateTimeEdit::YearSection));
   qdatetimeedit_ns->addConstant("TimeSections_Mask",        new QoreNode((int64)QDateTimeEdit::TimeSections_Mask));
   qdatetimeedit_ns->addConstant("DateSections_Mask",        new QoreNode((int64)QDateTimeEdit::DateSections_Mask));

   qdatetimeedit_ns->addSystemClass((qdatetimeedit = initQDateTimeEditClass(qabstractspinbox)));
   qdatetimeedit_ns->addSystemClass(initQDateEditClass(qdatetimeedit));
   qdatetimeedit_ns->addSystemClass(initQTimeEditClass(qdatetimeedit));

   qt_ns->addInitialNamespace(qdatetimeedit_ns);

   QoreNamespace *qdialog_ns = new QoreNamespace("QDialog");

   qdialog_ns->addSystemClass((qdialog = initQDialogClass(qwidget)));
   qdialog_ns->addSystemClass(initQFileDialogClass(qdialog));
   qdialog_ns->addSystemClass(initQPrintDialogClass(qdialog));

   qdialog_ns->addConstant("Rejected",   new QoreNode((int64)QDialog::Rejected));
   qdialog_ns->addConstant("Accepted",   new QoreNode((int64)QDialog::Accepted));

   qdialog_ns->addInitialNamespace(initQWizardNS(qdialog));

   QoreNamespace *qmessagebox = new QoreNamespace("QMessageBox");
   qmessagebox->addSystemClass(initQMessageBoxClass(qdialog));

   // Icon enum
   qmessagebox->addConstant("NoIcon",                   new QoreNode((int64)QMessageBox::NoIcon));
   qmessagebox->addConstant("Information",              new QoreNode((int64)QMessageBox::Information));
   qmessagebox->addConstant("Warning",                  new QoreNode((int64)QMessageBox::Warning));
   qmessagebox->addConstant("Critical",                 new QoreNode((int64)QMessageBox::Critical));
   qmessagebox->addConstant("Question",                 new QoreNode((int64)QMessageBox::Question));

   // ButtonRole enum
   qmessagebox->addConstant("InvalidRole",              new QoreNode((int64)QMessageBox::InvalidRole));
   qmessagebox->addConstant("AcceptRole",               new QoreNode((int64)QMessageBox::AcceptRole));
   qmessagebox->addConstant("RejectRole",               new QoreNode((int64)QMessageBox::RejectRole));
   qmessagebox->addConstant("DestructiveRole",          new QoreNode((int64)QMessageBox::DestructiveRole));
   qmessagebox->addConstant("ActionRole",               new QoreNode((int64)QMessageBox::ActionRole));
   qmessagebox->addConstant("HelpRole",                 new QoreNode((int64)QMessageBox::HelpRole));
   qmessagebox->addConstant("YesRole",                  new QoreNode((int64)QMessageBox::YesRole));
   qmessagebox->addConstant("NoRole",                   new QoreNode((int64)QMessageBox::NoRole));
   qmessagebox->addConstant("ResetRole",                new QoreNode((int64)QMessageBox::ResetRole));
   qmessagebox->addConstant("ApplyRole",                new QoreNode((int64)QMessageBox::ApplyRole));

   // StandardButton enum
   qmessagebox->addConstant("NoButton",                 new QoreNode((int64)QMessageBox::NoButton));
   qmessagebox->addConstant("Ok",                       new QoreNode((int64)QMessageBox::Ok));
   qmessagebox->addConstant("Save",                     new QoreNode((int64)QMessageBox::Save));
   qmessagebox->addConstant("SaveAll",                  new QoreNode((int64)QMessageBox::SaveAll));
   qmessagebox->addConstant("Open",                     new QoreNode((int64)QMessageBox::Open));
   qmessagebox->addConstant("Yes",                      new QoreNode((int64)QMessageBox::Yes));
   qmessagebox->addConstant("YesToAll",                 new QoreNode((int64)QMessageBox::YesToAll));
   qmessagebox->addConstant("No",                       new QoreNode((int64)QMessageBox::No));
   qmessagebox->addConstant("NoToAll",                  new QoreNode((int64)QMessageBox::NoToAll));
   qmessagebox->addConstant("Abort",                    new QoreNode((int64)QMessageBox::Abort));
   qmessagebox->addConstant("Retry",                    new QoreNode((int64)QMessageBox::Retry));
   qmessagebox->addConstant("Ignore",                   new QoreNode((int64)QMessageBox::Ignore));
   qmessagebox->addConstant("Close",                    new QoreNode((int64)QMessageBox::Close));
   qmessagebox->addConstant("Cancel",                   new QoreNode((int64)QMessageBox::Cancel));
   qmessagebox->addConstant("Discard",                  new QoreNode((int64)QMessageBox::Discard));
   qmessagebox->addConstant("Help",                     new QoreNode((int64)QMessageBox::Help));
   qmessagebox->addConstant("Apply",                    new QoreNode((int64)QMessageBox::Apply));
   qmessagebox->addConstant("Reset",                    new QoreNode((int64)QMessageBox::Reset));
   qmessagebox->addConstant("RestoreDefaults",          new QoreNode((int64)QMessageBox::RestoreDefaults));
   qmessagebox->addConstant("FirstButton",              new QoreNode((int64)QMessageBox::FirstButton));
   qmessagebox->addConstant("LastButton",               new QoreNode((int64)QMessageBox::LastButton));
   qmessagebox->addConstant("YesAll",                   new QoreNode((int64)QMessageBox::YesAll));
   qmessagebox->addConstant("NoAll",                    new QoreNode((int64)QMessageBox::NoAll));
   qmessagebox->addConstant("Default",                  new QoreNode((int64)QMessageBox::Default));
   qmessagebox->addConstant("Escape",                   new QoreNode((int64)QMessageBox::Escape));
   qmessagebox->addConstant("FlagMask",                 new QoreNode((int64)QMessageBox::FlagMask));
   qmessagebox->addConstant("ButtonMask",               new QoreNode((int64)QMessageBox::ButtonMask));

   qdialog_ns->addInitialNamespace(qmessagebox);

   qt_ns->addInitialNamespace(qdialog_ns);

   QoreNamespace *qprinter = new QoreNamespace("QPrinter");

   qprinter->addSystemClass(initQPrinterClass(qpaintdevice));

   // PrinterMode enum
   qprinter->addConstant("ScreenResolution",         new QoreNode((int64)QPrinter::ScreenResolution));
   qprinter->addConstant("PrinterResolution",        new QoreNode((int64)QPrinter::PrinterResolution));
   qprinter->addConstant("HighResolution",           new QoreNode((int64)QPrinter::HighResolution));

   // Orientation enum
   qprinter->addConstant("Portrait",                 new QoreNode((int64)QPrinter::Portrait));
   qprinter->addConstant("Landscape",                new QoreNode((int64)QPrinter::Landscape));

   // PageSize enum
   qprinter->addConstant("A4",                       new QoreNode((int64)QPrinter::A4));
   qprinter->addConstant("B5",                       new QoreNode((int64)QPrinter::B5));
   qprinter->addConstant("Letter",                   new QoreNode((int64)QPrinter::Letter));
   qprinter->addConstant("Legal",                    new QoreNode((int64)QPrinter::Legal));
   qprinter->addConstant("Executive",                new QoreNode((int64)QPrinter::Executive));
   qprinter->addConstant("A0",                       new QoreNode((int64)QPrinter::A0));
   qprinter->addConstant("A1",                       new QoreNode((int64)QPrinter::A1));
   qprinter->addConstant("A2",                       new QoreNode((int64)QPrinter::A2));
   qprinter->addConstant("A3",                       new QoreNode((int64)QPrinter::A3));
   qprinter->addConstant("A5",                       new QoreNode((int64)QPrinter::A5));
   qprinter->addConstant("A6",                       new QoreNode((int64)QPrinter::A6));
   qprinter->addConstant("A7",                       new QoreNode((int64)QPrinter::A7));
   qprinter->addConstant("A8",                       new QoreNode((int64)QPrinter::A8));
   qprinter->addConstant("A9",                       new QoreNode((int64)QPrinter::A9));
   qprinter->addConstant("B0",                       new QoreNode((int64)QPrinter::B0));
   qprinter->addConstant("B1",                       new QoreNode((int64)QPrinter::B1));
   qprinter->addConstant("B10",                      new QoreNode((int64)QPrinter::B10));
   qprinter->addConstant("B2",                       new QoreNode((int64)QPrinter::B2));
   qprinter->addConstant("B3",                       new QoreNode((int64)QPrinter::B3));
   qprinter->addConstant("B4",                       new QoreNode((int64)QPrinter::B4));
   qprinter->addConstant("B6",                       new QoreNode((int64)QPrinter::B6));
   qprinter->addConstant("B7",                       new QoreNode((int64)QPrinter::B7));
   qprinter->addConstant("B8",                       new QoreNode((int64)QPrinter::B8));
   qprinter->addConstant("B9",                       new QoreNode((int64)QPrinter::B9));
   qprinter->addConstant("C5E",                      new QoreNode((int64)QPrinter::C5E));
   qprinter->addConstant("Comm10E",                  new QoreNode((int64)QPrinter::Comm10E));
   qprinter->addConstant("DLE",                      new QoreNode((int64)QPrinter::DLE));
   qprinter->addConstant("Folio",                    new QoreNode((int64)QPrinter::Folio));
   qprinter->addConstant("Ledger",                   new QoreNode((int64)QPrinter::Ledger));
   qprinter->addConstant("Tabloid",                  new QoreNode((int64)QPrinter::Tabloid));
   qprinter->addConstant("Custom",                   new QoreNode((int64)QPrinter::Custom));

   // PageOrder enum
   qprinter->addConstant("FirstPageFirst",           new QoreNode((int64)QPrinter::FirstPageFirst));
   qprinter->addConstant("LastPageFirst",            new QoreNode((int64)QPrinter::LastPageFirst));
   
   // ColorMode enum
   qprinter->addConstant("GrayScale",                new QoreNode((int64)QPrinter::GrayScale));
   qprinter->addConstant("Color",                    new QoreNode((int64)QPrinter::Color));

   // PaperSource enum
   qprinter->addConstant("OnlyOne",                  new QoreNode((int64)QPrinter::OnlyOne));
   qprinter->addConstant("Lower",                    new QoreNode((int64)QPrinter::Lower));
   qprinter->addConstant("Middle",                   new QoreNode((int64)QPrinter::Middle));
   qprinter->addConstant("Manual",                   new QoreNode((int64)QPrinter::Manual));
   qprinter->addConstant("Envelope",                 new QoreNode((int64)QPrinter::Envelope));
   qprinter->addConstant("EnvelopeManual",           new QoreNode((int64)QPrinter::EnvelopeManual));
   qprinter->addConstant("Auto",                     new QoreNode((int64)QPrinter::Auto));
   qprinter->addConstant("Tractor",                  new QoreNode((int64)QPrinter::Tractor));
   qprinter->addConstant("SmallFormat",              new QoreNode((int64)QPrinter::SmallFormat));
   qprinter->addConstant("LargeFormat",              new QoreNode((int64)QPrinter::LargeFormat));
   qprinter->addConstant("LargeCapacity",            new QoreNode((int64)QPrinter::LargeCapacity));
   qprinter->addConstant("Cassette",                 new QoreNode((int64)QPrinter::Cassette));
   qprinter->addConstant("FormSource",               new QoreNode((int64)QPrinter::FormSource));
   qprinter->addConstant("MaxPageSource",            new QoreNode((int64)QPrinter::MaxPageSource));

   // PrinterState enum
   qprinter->addConstant("Idle",                     new QoreNode((int64)QPrinter::Idle));
   qprinter->addConstant("Active",                   new QoreNode((int64)QPrinter::Active));
   qprinter->addConstant("Aborted",                  new QoreNode((int64)QPrinter::Aborted));
   qprinter->addConstant("Error",                    new QoreNode((int64)QPrinter::Error));

   // OutputFormat enum
   qprinter->addConstant("NativeFormat",             new QoreNode((int64)QPrinter::NativeFormat));
   qprinter->addConstant("PdfFormat",                new QoreNode((int64)QPrinter::PdfFormat));
   qprinter->addConstant("PostScriptFormat",         new QoreNode((int64)QPrinter::PostScriptFormat));

   // PrintRange enum
   qprinter->addConstant("AllPages",                 new QoreNode((int64)QPrinter::AllPages));
   qprinter->addConstant("Selection",                new QoreNode((int64)QPrinter::Selection));
   qprinter->addConstant("PageRange",                new QoreNode((int64)QPrinter::PageRange));

   qt_ns->addInitialNamespace(qprinter);

   QoreNamespace *qlineedit = new QoreNamespace("QLineEdit");

   // EchoMode enum
   qlineedit->addConstant("Normal",                   new QoreNode((int64)QLineEdit::Normal));
   qlineedit->addConstant("NoEcho",                   new QoreNode((int64)QLineEdit::NoEcho));
   qlineedit->addConstant("Password",                 new QoreNode((int64)QLineEdit::Password));
   qlineedit->addConstant("PasswordEchoOnEdit",       new QoreNode((int64)QLineEdit::PasswordEchoOnEdit));

   qlineedit->addSystemClass(initQLineEditClass(qwidget));

   qt_ns->addInitialNamespace(qlineedit);

   QoreNamespace *qabstractitemview_ns = new QoreNamespace("QAbstractItemView");
   
   // SelectionMode enum
   qabstractitemview_ns->addConstant("NoSelection",              new QoreNode((int64)QAbstractItemView::NoSelection));
   qabstractitemview_ns->addConstant("SingleSelection",          new QoreNode((int64)QAbstractItemView::SingleSelection));
   qabstractitemview_ns->addConstant("MultiSelection",           new QoreNode((int64)QAbstractItemView::MultiSelection));
   qabstractitemview_ns->addConstant("ExtendedSelection",        new QoreNode((int64)QAbstractItemView::ExtendedSelection));
   qabstractitemview_ns->addConstant("ContiguousSelection",      new QoreNode((int64)QAbstractItemView::ContiguousSelection));

   // SelectionBehavior enum
   qabstractitemview_ns->addConstant("SelectItems",              new QoreNode((int64)QAbstractItemView::SelectItems));
   qabstractitemview_ns->addConstant("SelectRows",               new QoreNode((int64)QAbstractItemView::SelectRows));
   qabstractitemview_ns->addConstant("SelectColumns",            new QoreNode((int64)QAbstractItemView::SelectColumns));

   // ScrollHint enum
   qabstractitemview_ns->addConstant("EnsureVisible",            new QoreNode((int64)QAbstractItemView::EnsureVisible));
   qabstractitemview_ns->addConstant("PositionAtTop",            new QoreNode((int64)QAbstractItemView::PositionAtTop));
   qabstractitemview_ns->addConstant("PositionAtBottom",         new QoreNode((int64)QAbstractItemView::PositionAtBottom));
   qabstractitemview_ns->addConstant("PositionAtCenter",         new QoreNode((int64)QAbstractItemView::PositionAtCenter));

   // EditTrigger enum
   qabstractitemview_ns->addConstant("NoEditTriggers",           new QoreNode((int64)QAbstractItemView::NoEditTriggers));
   qabstractitemview_ns->addConstant("CurrentChanged",           new QoreNode((int64)QAbstractItemView::CurrentChanged));
   qabstractitemview_ns->addConstant("DoubleClicked",            new QoreNode((int64)QAbstractItemView::DoubleClicked));
   qabstractitemview_ns->addConstant("SelectedClicked",          new QoreNode((int64)QAbstractItemView::SelectedClicked));
   qabstractitemview_ns->addConstant("EditKeyPressed",           new QoreNode((int64)QAbstractItemView::EditKeyPressed));
   qabstractitemview_ns->addConstant("AnyKeyPressed",            new QoreNode((int64)QAbstractItemView::AnyKeyPressed));
   qabstractitemview_ns->addConstant("AllEditTriggers",          new QoreNode((int64)QAbstractItemView::AllEditTriggers));

   // ScrollMode enum
   qabstractitemview_ns->addConstant("ScrollPerItem",            new QoreNode((int64)QAbstractItemView::ScrollPerItem));
   qabstractitemview_ns->addConstant("ScrollPerPixel",           new QoreNode((int64)QAbstractItemView::ScrollPerPixel));

   qabstractitemview_ns->addSystemClass((qabstractitemview = initQAbstractItemViewClass(qabstractscrollarea)));
   qabstractitemview_ns->addSystemClass((qtableview = initQTableViewClass(qabstractitemview)));
   qabstractitemview_ns->addSystemClass(initQTableWidgetClass(qtableview));

   qabstractitemview_ns->addInitialNamespace(initQListViewNS(qabstractitemview));
   
   qt_ns->addInitialNamespace(qabstractitemview_ns);

   QoreNamespace *qheaderview = new QoreNamespace("QHeaderView");

   // ResizeMode enum
   qheaderview->addConstant("Interactive",              new QoreNode((int64)QHeaderView::Interactive));
   qheaderview->addConstant("Stretch",                  new QoreNode((int64)QHeaderView::Stretch));
   qheaderview->addConstant("Fixed",                    new QoreNode((int64)QHeaderView::Fixed));
   qheaderview->addConstant("ResizeToContents",         new QoreNode((int64)QHeaderView::ResizeToContents));
   qheaderview->addConstant("Custom",                   new QoreNode((int64)QHeaderView::Custom));

   qheaderview->addSystemClass(initQHeaderViewClass(qabstractitemview));

   qt_ns->addInitialNamespace(qheaderview);


   QoreNamespace *qclipboard = new QoreNamespace("QClipboard");
   
   // Mode enum
   qclipboard->addConstant("Clipboard",                new QoreNode((int64)QClipboard::Clipboard));
   qclipboard->addConstant("Selection",                new QoreNode((int64)QClipboard::Selection));
   qclipboard->addConstant("FindBuffer",               new QoreNode((int64)QClipboard::FindBuffer));
   qclipboard->addConstant("LastMode",                 new QoreNode((int64)QClipboard::LastMode));

   qclipboard->addSystemClass(initQClipboardClass(qobject));

   qt_ns->addInitialNamespace(qclipboard);

   QoreNamespace *qchar = new QoreNamespace("QChar");
   qchar->addSystemClass(initQCharClass());

   // SpecialCharacter enum
   qchar->addConstant("Null",                     new QoreNode((int64)QChar::Null));
   qchar->addConstant("Nbsp",                     new QoreNode((int64)QChar::Nbsp));
   qchar->addConstant("ReplacementCharacter",     new QoreNode((int64)QChar::ReplacementCharacter));
   qchar->addConstant("ObjectReplacementCharacter", new QoreNode((int64)QChar::ObjectReplacementCharacter));
   qchar->addConstant("ByteOrderMark",            new QoreNode((int64)QChar::ByteOrderMark));
   qchar->addConstant("ByteOrderSwapped",         new QoreNode((int64)QChar::ByteOrderSwapped));
   qchar->addConstant("ParagraphSeparator",       new QoreNode((int64)QChar::ParagraphSeparator));
   qchar->addConstant("LineSeparator",            new QoreNode((int64)QChar::LineSeparator));

   // Category enum
   qchar->addConstant("NoCategory",               new QoreNode((int64)QChar::NoCategory));
   qchar->addConstant("Mark_NonSpacing",          new QoreNode((int64)QChar::Mark_NonSpacing));
   qchar->addConstant("Mark_SpacingCombining",    new QoreNode((int64)QChar::Mark_SpacingCombining));
   qchar->addConstant("Mark_Enclosing",           new QoreNode((int64)QChar::Mark_Enclosing));
   qchar->addConstant("Number_DecimalDigit",      new QoreNode((int64)QChar::Number_DecimalDigit));
   qchar->addConstant("Number_Letter",            new QoreNode((int64)QChar::Number_Letter));
   qchar->addConstant("Number_Other",             new QoreNode((int64)QChar::Number_Other));
   qchar->addConstant("Separator_Space",          new QoreNode((int64)QChar::Separator_Space));
   qchar->addConstant("Separator_Line",           new QoreNode((int64)QChar::Separator_Line));
   qchar->addConstant("Separator_Paragraph",      new QoreNode((int64)QChar::Separator_Paragraph));
   qchar->addConstant("Other_Control",            new QoreNode((int64)QChar::Other_Control));
   qchar->addConstant("Other_Format",             new QoreNode((int64)QChar::Other_Format));
   qchar->addConstant("Other_Surrogate",          new QoreNode((int64)QChar::Other_Surrogate));
   qchar->addConstant("Other_PrivateUse",         new QoreNode((int64)QChar::Other_PrivateUse));
   qchar->addConstant("Other_NotAssigned",        new QoreNode((int64)QChar::Other_NotAssigned));
   qchar->addConstant("Letter_Uppercase",         new QoreNode((int64)QChar::Letter_Uppercase));
   qchar->addConstant("Letter_Lowercase",         new QoreNode((int64)QChar::Letter_Lowercase));
   qchar->addConstant("Letter_Titlecase",         new QoreNode((int64)QChar::Letter_Titlecase));
   qchar->addConstant("Letter_Modifier",          new QoreNode((int64)QChar::Letter_Modifier));
   qchar->addConstant("Letter_Other",             new QoreNode((int64)QChar::Letter_Other));
   qchar->addConstant("Punctuation_Connector",    new QoreNode((int64)QChar::Punctuation_Connector));
   qchar->addConstant("Punctuation_Dash",         new QoreNode((int64)QChar::Punctuation_Dash));
   qchar->addConstant("Punctuation_Open",         new QoreNode((int64)QChar::Punctuation_Open));
   qchar->addConstant("Punctuation_Close",        new QoreNode((int64)QChar::Punctuation_Close));
   qchar->addConstant("Punctuation_InitialQuote", new QoreNode((int64)QChar::Punctuation_InitialQuote));
   qchar->addConstant("Punctuation_FinalQuote",   new QoreNode((int64)QChar::Punctuation_FinalQuote));
   qchar->addConstant("Punctuation_Other",        new QoreNode((int64)QChar::Punctuation_Other));
   qchar->addConstant("Symbol_Math",              new QoreNode((int64)QChar::Symbol_Math));
   qchar->addConstant("Symbol_Currency",          new QoreNode((int64)QChar::Symbol_Currency));
   qchar->addConstant("Symbol_Modifier",          new QoreNode((int64)QChar::Symbol_Modifier));
   qchar->addConstant("Symbol_Other",             new QoreNode((int64)QChar::Symbol_Other));
   qchar->addConstant("Punctuation_Dask",         new QoreNode((int64)QChar::Punctuation_Dask));

   // Direction enum
   qchar->addConstant("DirL",                     new QoreNode((int64)QChar::DirL));
   qchar->addConstant("DirR",                     new QoreNode((int64)QChar::DirR));
   qchar->addConstant("DirEN",                    new QoreNode((int64)QChar::DirEN));
   qchar->addConstant("DirES",                    new QoreNode((int64)QChar::DirES));
   qchar->addConstant("DirET",                    new QoreNode((int64)QChar::DirET));
   qchar->addConstant("DirAN",                    new QoreNode((int64)QChar::DirAN));
   qchar->addConstant("DirCS",                    new QoreNode((int64)QChar::DirCS));
   qchar->addConstant("DirB",                     new QoreNode((int64)QChar::DirB));
   qchar->addConstant("DirS",                     new QoreNode((int64)QChar::DirS));
   qchar->addConstant("DirWS",                    new QoreNode((int64)QChar::DirWS));
   qchar->addConstant("DirON",                    new QoreNode((int64)QChar::DirON));
   qchar->addConstant("DirLRE",                   new QoreNode((int64)QChar::DirLRE));
   qchar->addConstant("DirLRO",                   new QoreNode((int64)QChar::DirLRO));
   qchar->addConstant("DirAL",                    new QoreNode((int64)QChar::DirAL));
   qchar->addConstant("DirRLE",                   new QoreNode((int64)QChar::DirRLE));
   qchar->addConstant("DirRLO",                   new QoreNode((int64)QChar::DirRLO));
   qchar->addConstant("DirPDF",                   new QoreNode((int64)QChar::DirPDF));
   qchar->addConstant("DirNSM",                   new QoreNode((int64)QChar::DirNSM));
   qchar->addConstant("DirBN",                    new QoreNode((int64)QChar::DirBN));

   // Decomposition enum
   qchar->addConstant("NoDecomposition",          new QoreNode((int64)QChar::NoDecomposition));
   qchar->addConstant("Canonical",                new QoreNode((int64)QChar::Canonical));
   qchar->addConstant("Font",                     new QoreNode((int64)QChar::Font));
   qchar->addConstant("NoBreak",                  new QoreNode((int64)QChar::NoBreak));
   qchar->addConstant("Initial",                  new QoreNode((int64)QChar::Initial));
   qchar->addConstant("Medial",                   new QoreNode((int64)QChar::Medial));
   qchar->addConstant("Final",                    new QoreNode((int64)QChar::Final));
   qchar->addConstant("Isolated",                 new QoreNode((int64)QChar::Isolated));
   qchar->addConstant("Circle",                   new QoreNode((int64)QChar::Circle));
   qchar->addConstant("Super",                    new QoreNode((int64)QChar::Super));
   qchar->addConstant("Sub",                      new QoreNode((int64)QChar::Sub));
   qchar->addConstant("Vertical",                 new QoreNode((int64)QChar::Vertical));
   qchar->addConstant("Wide",                     new QoreNode((int64)QChar::Wide));
   qchar->addConstant("Narrow",                   new QoreNode((int64)QChar::Narrow));
   qchar->addConstant("Small",                    new QoreNode((int64)QChar::Small));
   qchar->addConstant("Square",                   new QoreNode((int64)QChar::Square));
   qchar->addConstant("Compat",                   new QoreNode((int64)QChar::Compat));
   qchar->addConstant("Fraction",                 new QoreNode((int64)QChar::Fraction));

   // Joining enum
   qchar->addConstant("OtherJoining",             new QoreNode((int64)QChar::OtherJoining));
   qchar->addConstant("Dual",                     new QoreNode((int64)QChar::Dual));
   qchar->addConstant("Right",                    new QoreNode((int64)QChar::Right));
   qchar->addConstant("Center",                   new QoreNode((int64)QChar::Center));

   // Combining class
   qchar->addConstant("Combining_BelowLeftAttached", new QoreNode((int64)QChar::Combining_BelowLeftAttached));
   qchar->addConstant("Combining_BelowAttached",  new QoreNode((int64)QChar::Combining_BelowAttached));
   qchar->addConstant("Combining_BelowRightAttached", new QoreNode((int64)QChar::Combining_BelowRightAttached));
   qchar->addConstant("Combining_LeftAttached",   new QoreNode((int64)QChar::Combining_LeftAttached));
   qchar->addConstant("Combining_RightAttached",  new QoreNode((int64)QChar::Combining_RightAttached));
   qchar->addConstant("Combining_AboveLeftAttached", new QoreNode((int64)QChar::Combining_AboveLeftAttached));
   qchar->addConstant("Combining_AboveAttached",  new QoreNode((int64)QChar::Combining_AboveAttached));
   qchar->addConstant("Combining_AboveRightAttached", new QoreNode((int64)QChar::Combining_AboveRightAttached));
   qchar->addConstant("Combining_BelowLeft",      new QoreNode((int64)QChar::Combining_BelowLeft));
   qchar->addConstant("Combining_Below",          new QoreNode((int64)QChar::Combining_Below));
   qchar->addConstant("Combining_BelowRight",     new QoreNode((int64)QChar::Combining_BelowRight));
   qchar->addConstant("Combining_Left",           new QoreNode((int64)QChar::Combining_Left));
   qchar->addConstant("Combining_Right",          new QoreNode((int64)QChar::Combining_Right));
   qchar->addConstant("Combining_AboveLeft",      new QoreNode((int64)QChar::Combining_AboveLeft));
   qchar->addConstant("Combining_Above",          new QoreNode((int64)QChar::Combining_Above));
   qchar->addConstant("Combining_AboveRight",     new QoreNode((int64)QChar::Combining_AboveRight));
   qchar->addConstant("Combining_DoubleBelow",    new QoreNode((int64)QChar::Combining_DoubleBelow));
   qchar->addConstant("Combining_DoubleAbove",    new QoreNode((int64)QChar::Combining_DoubleAbove));
   qchar->addConstant("Combining_IotaSubscript",  new QoreNode((int64)QChar::Combining_IotaSubscript));

   // UnicodeVersion
   qchar->addConstant("Unicode_Unassigned",       new QoreNode((int64)QChar::Unicode_Unassigned));
   qchar->addConstant("Unicode_1_1",              new QoreNode((int64)QChar::Unicode_1_1));
   qchar->addConstant("Unicode_2_0",              new QoreNode((int64)QChar::Unicode_2_0));
   qchar->addConstant("Unicode_2_1_2",            new QoreNode((int64)QChar::Unicode_2_1_2));
   qchar->addConstant("Unicode_3_0",              new QoreNode((int64)QChar::Unicode_3_0));
   qchar->addConstant("Unicode_3_1",              new QoreNode((int64)QChar::Unicode_3_1));
   qchar->addConstant("Unicode_3_2",              new QoreNode((int64)QChar::Unicode_3_2));
   qchar->addConstant("Unicode_4_0",              new QoreNode((int64)QChar::Unicode_4_0));
   qchar->addConstant("Unicode_4_1",              new QoreNode((int64)QChar::Unicode_4_1));
   qchar->addConstant("Unicode_5_0",              new QoreNode((int64)QChar::Unicode_5_0));

   qt_ns->addInitialNamespace(qchar);

   QoreNamespace *qcalendarwidget = new QoreNamespace("QCalendarWidget");

   qcalendarwidget->addSystemClass(initQCalendarWidgetClass(qwidget));

   // SelectionMode enum
   qcalendarwidget->addConstant("NoSelection",              new QoreNode((int64)QCalendarWidget::NoSelection));
   qcalendarwidget->addConstant("SingleSelection",          new QoreNode((int64)QCalendarWidget::SingleSelection));

   // HorizontalHeaderFormat enum
   qcalendarwidget->addConstant("NoHorizontalHeader",       new QoreNode((int64)QCalendarWidget::NoHorizontalHeader));
   qcalendarwidget->addConstant("SingleLetterDayNames",     new QoreNode((int64)QCalendarWidget::SingleLetterDayNames));
   qcalendarwidget->addConstant("ShortDayNames",            new QoreNode((int64)QCalendarWidget::ShortDayNames));
   qcalendarwidget->addConstant("LongDayNames",             new QoreNode((int64)QCalendarWidget::LongDayNames));

   // VeritcalHeaderFormat enum
   qcalendarwidget->addConstant("NoVerticalHeader",         new QoreNode((int64)QCalendarWidget::NoVerticalHeader));
   qcalendarwidget->addConstant("ISOWeekNumbers",           new QoreNode((int64)QCalendarWidget::ISOWeekNumbers));

   qt_ns->addInitialNamespace(qcalendarwidget);

   QoreNamespace *qlocale = new QoreNamespace("QLocale");
   qlocale->addSystemClass(initQLocaleClass());

   // Language enum
   qlocale->addConstant("C",                        new QoreNode((int64)QLocale::C));
   qlocale->addConstant("Abkhazian",                new QoreNode((int64)QLocale::Abkhazian));
   qlocale->addConstant("Afan",                     new QoreNode((int64)QLocale::Afan));
   qlocale->addConstant("Afar",                     new QoreNode((int64)QLocale::Afar));
   qlocale->addConstant("Afrikaans",                new QoreNode((int64)QLocale::Afrikaans));
   qlocale->addConstant("Albanian",                 new QoreNode((int64)QLocale::Albanian));
   qlocale->addConstant("Amharic",                  new QoreNode((int64)QLocale::Amharic));
   qlocale->addConstant("Arabic",                   new QoreNode((int64)QLocale::Arabic));
   qlocale->addConstant("Armenian",                 new QoreNode((int64)QLocale::Armenian));
   qlocale->addConstant("Assamese",                 new QoreNode((int64)QLocale::Assamese));
   qlocale->addConstant("Aymara",                   new QoreNode((int64)QLocale::Aymara));
   qlocale->addConstant("Azerbaijani",              new QoreNode((int64)QLocale::Azerbaijani));
   qlocale->addConstant("Bashkir",                  new QoreNode((int64)QLocale::Bashkir));
   qlocale->addConstant("Basque",                   new QoreNode((int64)QLocale::Basque));
   qlocale->addConstant("Bengali",                  new QoreNode((int64)QLocale::Bengali));
   qlocale->addConstant("Bhutani",                  new QoreNode((int64)QLocale::Bhutani));
   qlocale->addConstant("Bihari",                   new QoreNode((int64)QLocale::Bihari));
   qlocale->addConstant("Bislama",                  new QoreNode((int64)QLocale::Bislama));
   qlocale->addConstant("Breton",                   new QoreNode((int64)QLocale::Breton));
   qlocale->addConstant("Bulgarian",                new QoreNode((int64)QLocale::Bulgarian));
   qlocale->addConstant("Burmese",                  new QoreNode((int64)QLocale::Burmese));
   qlocale->addConstant("Byelorussian",             new QoreNode((int64)QLocale::Byelorussian));
   qlocale->addConstant("Cambodian",                new QoreNode((int64)QLocale::Cambodian));
   qlocale->addConstant("Catalan",                  new QoreNode((int64)QLocale::Catalan));
   qlocale->addConstant("Chinese",                  new QoreNode((int64)QLocale::Chinese));
   qlocale->addConstant("Corsican",                 new QoreNode((int64)QLocale::Corsican));
   qlocale->addConstant("Croatian",                 new QoreNode((int64)QLocale::Croatian));
   qlocale->addConstant("Czech",                    new QoreNode((int64)QLocale::Czech));
   qlocale->addConstant("Danish",                   new QoreNode((int64)QLocale::Danish));
   qlocale->addConstant("Dutch",                    new QoreNode((int64)QLocale::Dutch));
   qlocale->addConstant("English",                  new QoreNode((int64)QLocale::English));
   qlocale->addConstant("Esperanto",                new QoreNode((int64)QLocale::Esperanto));
   qlocale->addConstant("Estonian",                 new QoreNode((int64)QLocale::Estonian));
   qlocale->addConstant("Faroese",                  new QoreNode((int64)QLocale::Faroese));
   qlocale->addConstant("FijiLanguage",             new QoreNode((int64)QLocale::FijiLanguage));
   qlocale->addConstant("Finnish",                  new QoreNode((int64)QLocale::Finnish));
   qlocale->addConstant("French",                   new QoreNode((int64)QLocale::French));
   qlocale->addConstant("Frisian",                  new QoreNode((int64)QLocale::Frisian));
   qlocale->addConstant("Gaelic",                   new QoreNode((int64)QLocale::Gaelic));
   qlocale->addConstant("Galician",                 new QoreNode((int64)QLocale::Galician));
   qlocale->addConstant("Georgian",                 new QoreNode((int64)QLocale::Georgian));
   qlocale->addConstant("German",                   new QoreNode((int64)QLocale::German));
   qlocale->addConstant("Greek",                    new QoreNode((int64)QLocale::Greek));
   qlocale->addConstant("Greenlandic",              new QoreNode((int64)QLocale::Greenlandic));
   qlocale->addConstant("Guarani",                  new QoreNode((int64)QLocale::Guarani));
   qlocale->addConstant("Gujarati",                 new QoreNode((int64)QLocale::Gujarati));
   qlocale->addConstant("Hausa",                    new QoreNode((int64)QLocale::Hausa));
   qlocale->addConstant("Hebrew",                   new QoreNode((int64)QLocale::Hebrew));
   qlocale->addConstant("Hindi",                    new QoreNode((int64)QLocale::Hindi));
   qlocale->addConstant("Hungarian",                new QoreNode((int64)QLocale::Hungarian));
   qlocale->addConstant("Icelandic",                new QoreNode((int64)QLocale::Icelandic));
   qlocale->addConstant("Indonesian",               new QoreNode((int64)QLocale::Indonesian));
   qlocale->addConstant("Interlingua",              new QoreNode((int64)QLocale::Interlingua));
   qlocale->addConstant("Interlingue",              new QoreNode((int64)QLocale::Interlingue));
   qlocale->addConstant("Inuktitut",                new QoreNode((int64)QLocale::Inuktitut));
   qlocale->addConstant("Inupiak",                  new QoreNode((int64)QLocale::Inupiak));
   qlocale->addConstant("Irish",                    new QoreNode((int64)QLocale::Irish));
   qlocale->addConstant("Italian",                  new QoreNode((int64)QLocale::Italian));
   qlocale->addConstant("Japanese",                 new QoreNode((int64)QLocale::Japanese));
   qlocale->addConstant("Javanese",                 new QoreNode((int64)QLocale::Javanese));
   qlocale->addConstant("Kannada",                  new QoreNode((int64)QLocale::Kannada));
   qlocale->addConstant("Kashmiri",                 new QoreNode((int64)QLocale::Kashmiri));
   qlocale->addConstant("Kazakh",                   new QoreNode((int64)QLocale::Kazakh));
   qlocale->addConstant("Kinyarwanda",              new QoreNode((int64)QLocale::Kinyarwanda));
   qlocale->addConstant("Kirghiz",                  new QoreNode((int64)QLocale::Kirghiz));
   qlocale->addConstant("Korean",                   new QoreNode((int64)QLocale::Korean));
   qlocale->addConstant("Kurdish",                  new QoreNode((int64)QLocale::Kurdish));
   qlocale->addConstant("Kurundi",                  new QoreNode((int64)QLocale::Kurundi));
   qlocale->addConstant("Laothian",                 new QoreNode((int64)QLocale::Laothian));
   qlocale->addConstant("Latin",                    new QoreNode((int64)QLocale::Latin));
   qlocale->addConstant("Latvian",                  new QoreNode((int64)QLocale::Latvian));
   qlocale->addConstant("Lingala",                  new QoreNode((int64)QLocale::Lingala));
   qlocale->addConstant("Lithuanian",               new QoreNode((int64)QLocale::Lithuanian));
   qlocale->addConstant("Macedonian",               new QoreNode((int64)QLocale::Macedonian));
   qlocale->addConstant("Malagasy",                 new QoreNode((int64)QLocale::Malagasy));
   qlocale->addConstant("Malay",                    new QoreNode((int64)QLocale::Malay));
   qlocale->addConstant("Malayalam",                new QoreNode((int64)QLocale::Malayalam));
   qlocale->addConstant("Maltese",                  new QoreNode((int64)QLocale::Maltese));
   qlocale->addConstant("Maori",                    new QoreNode((int64)QLocale::Maori));
   qlocale->addConstant("Marathi",                  new QoreNode((int64)QLocale::Marathi));
   qlocale->addConstant("Moldavian",                new QoreNode((int64)QLocale::Moldavian));
   qlocale->addConstant("Mongolian",                new QoreNode((int64)QLocale::Mongolian));
   qlocale->addConstant("NauruLanguage",            new QoreNode((int64)QLocale::NauruLanguage));
   qlocale->addConstant("Nepali",                   new QoreNode((int64)QLocale::Nepali));
   qlocale->addConstant("Norwegian",                new QoreNode((int64)QLocale::Norwegian));
   qlocale->addConstant("NorwegianBokmal",          new QoreNode((int64)QLocale::NorwegianBokmal));
   qlocale->addConstant("Occitan",                  new QoreNode((int64)QLocale::Occitan));
   qlocale->addConstant("Oriya",                    new QoreNode((int64)QLocale::Oriya));
   qlocale->addConstant("Pashto",                   new QoreNode((int64)QLocale::Pashto));
   qlocale->addConstant("Persian",                  new QoreNode((int64)QLocale::Persian));
   qlocale->addConstant("Polish",                   new QoreNode((int64)QLocale::Polish));
   qlocale->addConstant("Portuguese",               new QoreNode((int64)QLocale::Portuguese));
   qlocale->addConstant("Punjabi",                  new QoreNode((int64)QLocale::Punjabi));
   qlocale->addConstant("Quechua",                  new QoreNode((int64)QLocale::Quechua));
   qlocale->addConstant("RhaetoRomance",            new QoreNode((int64)QLocale::RhaetoRomance));
   qlocale->addConstant("Romanian",                 new QoreNode((int64)QLocale::Romanian));
   qlocale->addConstant("Russian",                  new QoreNode((int64)QLocale::Russian));
   qlocale->addConstant("Samoan",                   new QoreNode((int64)QLocale::Samoan));
   qlocale->addConstant("Sangho",                   new QoreNode((int64)QLocale::Sangho));
   qlocale->addConstant("Sanskrit",                 new QoreNode((int64)QLocale::Sanskrit));
   qlocale->addConstant("Serbian",                  new QoreNode((int64)QLocale::Serbian));
   qlocale->addConstant("SerboCroatian",            new QoreNode((int64)QLocale::SerboCroatian));
   qlocale->addConstant("Sesotho",                  new QoreNode((int64)QLocale::Sesotho));
   qlocale->addConstant("Setswana",                 new QoreNode((int64)QLocale::Setswana));
   qlocale->addConstant("Shona",                    new QoreNode((int64)QLocale::Shona));
   qlocale->addConstant("Sindhi",                   new QoreNode((int64)QLocale::Sindhi));
   qlocale->addConstant("Singhalese",               new QoreNode((int64)QLocale::Singhalese));
   qlocale->addConstant("Siswati",                  new QoreNode((int64)QLocale::Siswati));
   qlocale->addConstant("Slovak",                   new QoreNode((int64)QLocale::Slovak));
   qlocale->addConstant("Slovenian",                new QoreNode((int64)QLocale::Slovenian));
   qlocale->addConstant("Somali",                   new QoreNode((int64)QLocale::Somali));
   qlocale->addConstant("Spanish",                  new QoreNode((int64)QLocale::Spanish));
   qlocale->addConstant("Sundanese",                new QoreNode((int64)QLocale::Sundanese));
   qlocale->addConstant("Swahili",                  new QoreNode((int64)QLocale::Swahili));
   qlocale->addConstant("Swedish",                  new QoreNode((int64)QLocale::Swedish));
   qlocale->addConstant("Tagalog",                  new QoreNode((int64)QLocale::Tagalog));
   qlocale->addConstant("Tajik",                    new QoreNode((int64)QLocale::Tajik));
   qlocale->addConstant("Tamil",                    new QoreNode((int64)QLocale::Tamil));
   qlocale->addConstant("Tatar",                    new QoreNode((int64)QLocale::Tatar));
   qlocale->addConstant("Telugu",                   new QoreNode((int64)QLocale::Telugu));
   qlocale->addConstant("Thai",                     new QoreNode((int64)QLocale::Thai));
   qlocale->addConstant("Tibetan",                  new QoreNode((int64)QLocale::Tibetan));
   qlocale->addConstant("Tigrinya",                 new QoreNode((int64)QLocale::Tigrinya));
   qlocale->addConstant("TongaLanguage",            new QoreNode((int64)QLocale::TongaLanguage));
   qlocale->addConstant("Tsonga",                   new QoreNode((int64)QLocale::Tsonga));
   qlocale->addConstant("Turkish",                  new QoreNode((int64)QLocale::Turkish));
   qlocale->addConstant("Turkmen",                  new QoreNode((int64)QLocale::Turkmen));
   qlocale->addConstant("Twi",                      new QoreNode((int64)QLocale::Twi));
   qlocale->addConstant("Uigur",                    new QoreNode((int64)QLocale::Uigur));
   qlocale->addConstant("Ukrainian",                new QoreNode((int64)QLocale::Ukrainian));
   qlocale->addConstant("Urdu",                     new QoreNode((int64)QLocale::Urdu));
   qlocale->addConstant("Uzbek",                    new QoreNode((int64)QLocale::Uzbek));
   qlocale->addConstant("Vietnamese",               new QoreNode((int64)QLocale::Vietnamese));
   qlocale->addConstant("Volapuk",                  new QoreNode((int64)QLocale::Volapuk));
   qlocale->addConstant("Welsh",                    new QoreNode((int64)QLocale::Welsh));
   qlocale->addConstant("Wolof",                    new QoreNode((int64)QLocale::Wolof));
   qlocale->addConstant("Xhosa",                    new QoreNode((int64)QLocale::Xhosa));
   qlocale->addConstant("Yiddish",                  new QoreNode((int64)QLocale::Yiddish));
   qlocale->addConstant("Yoruba",                   new QoreNode((int64)QLocale::Yoruba));
   qlocale->addConstant("Zhuang",                   new QoreNode((int64)QLocale::Zhuang));
   qlocale->addConstant("Zulu",                     new QoreNode((int64)QLocale::Zulu));
   qlocale->addConstant("NorwegianNynorsk",         new QoreNode((int64)QLocale::NorwegianNynorsk));
   qlocale->addConstant("Nynorsk",                  new QoreNode((int64)QLocale::Nynorsk));
   qlocale->addConstant("Bosnian",                  new QoreNode((int64)QLocale::Bosnian));
   qlocale->addConstant("Divehi",                   new QoreNode((int64)QLocale::Divehi));
   qlocale->addConstant("Manx",                     new QoreNode((int64)QLocale::Manx));
   qlocale->addConstant("Cornish",                  new QoreNode((int64)QLocale::Cornish));
   qlocale->addConstant("Akan",                     new QoreNode((int64)QLocale::Akan));
   qlocale->addConstant("Konkani",                  new QoreNode((int64)QLocale::Konkani));
   qlocale->addConstant("Ga",                       new QoreNode((int64)QLocale::Ga));
   qlocale->addConstant("Igbo",                     new QoreNode((int64)QLocale::Igbo));
   qlocale->addConstant("Kamba",                    new QoreNode((int64)QLocale::Kamba));
   qlocale->addConstant("Syriac",                   new QoreNode((int64)QLocale::Syriac));
   qlocale->addConstant("Blin",                     new QoreNode((int64)QLocale::Blin));
   qlocale->addConstant("Geez",                     new QoreNode((int64)QLocale::Geez));
   qlocale->addConstant("Koro",                     new QoreNode((int64)QLocale::Koro));
   qlocale->addConstant("Sidamo",                   new QoreNode((int64)QLocale::Sidamo));
   qlocale->addConstant("Atsam",                    new QoreNode((int64)QLocale::Atsam));
   qlocale->addConstant("Tigre",                    new QoreNode((int64)QLocale::Tigre));
   qlocale->addConstant("Jju",                      new QoreNode((int64)QLocale::Jju));
   qlocale->addConstant("Friulian",                 new QoreNode((int64)QLocale::Friulian));
   qlocale->addConstant("Venda",                    new QoreNode((int64)QLocale::Venda));
   qlocale->addConstant("Ewe",                      new QoreNode((int64)QLocale::Ewe));
   qlocale->addConstant("Walamo",                   new QoreNode((int64)QLocale::Walamo));
   qlocale->addConstant("Hawaiian",                 new QoreNode((int64)QLocale::Hawaiian));
   qlocale->addConstant("Tyap",                     new QoreNode((int64)QLocale::Tyap));
   qlocale->addConstant("Chewa",                    new QoreNode((int64)QLocale::Chewa));
   qlocale->addConstant("LastLanguage",             new QoreNode((int64)QLocale::LastLanguage));

   // Country enum
   qlocale->addConstant("AnyCountry",               new QoreNode((int64)QLocale::AnyCountry));
   qlocale->addConstant("Afghanistan",              new QoreNode((int64)QLocale::Afghanistan));
   qlocale->addConstant("Albania",                  new QoreNode((int64)QLocale::Albania));
   qlocale->addConstant("Algeria",                  new QoreNode((int64)QLocale::Algeria));
   qlocale->addConstant("AmericanSamoa",            new QoreNode((int64)QLocale::AmericanSamoa));
   qlocale->addConstant("Andorra",                  new QoreNode((int64)QLocale::Andorra));
   qlocale->addConstant("Angola",                   new QoreNode((int64)QLocale::Angola));
   qlocale->addConstant("Anguilla",                 new QoreNode((int64)QLocale::Anguilla));
   qlocale->addConstant("Antarctica",               new QoreNode((int64)QLocale::Antarctica));
   qlocale->addConstant("AntiguaAndBarbuda",        new QoreNode((int64)QLocale::AntiguaAndBarbuda));
   qlocale->addConstant("Argentina",                new QoreNode((int64)QLocale::Argentina));
   qlocale->addConstant("Armenia",                  new QoreNode((int64)QLocale::Armenia));
   qlocale->addConstant("Aruba",                    new QoreNode((int64)QLocale::Aruba));
   qlocale->addConstant("Australia",                new QoreNode((int64)QLocale::Australia));
   qlocale->addConstant("Austria",                  new QoreNode((int64)QLocale::Austria));
   qlocale->addConstant("Azerbaijan",               new QoreNode((int64)QLocale::Azerbaijan));
   qlocale->addConstant("Bahamas",                  new QoreNode((int64)QLocale::Bahamas));
   qlocale->addConstant("Bahrain",                  new QoreNode((int64)QLocale::Bahrain));
   qlocale->addConstant("Bangladesh",               new QoreNode((int64)QLocale::Bangladesh));
   qlocale->addConstant("Barbados",                 new QoreNode((int64)QLocale::Barbados));
   qlocale->addConstant("Belarus",                  new QoreNode((int64)QLocale::Belarus));
   qlocale->addConstant("Belgium",                  new QoreNode((int64)QLocale::Belgium));
   qlocale->addConstant("Belize",                   new QoreNode((int64)QLocale::Belize));
   qlocale->addConstant("Benin",                    new QoreNode((int64)QLocale::Benin));
   qlocale->addConstant("Bermuda",                  new QoreNode((int64)QLocale::Bermuda));
   qlocale->addConstant("Bhutan",                   new QoreNode((int64)QLocale::Bhutan));
   qlocale->addConstant("Bolivia",                  new QoreNode((int64)QLocale::Bolivia));
   qlocale->addConstant("BosniaAndHerzegowina",     new QoreNode((int64)QLocale::BosniaAndHerzegowina));
   qlocale->addConstant("Botswana",                 new QoreNode((int64)QLocale::Botswana));
   qlocale->addConstant("BouvetIsland",             new QoreNode((int64)QLocale::BouvetIsland));
   qlocale->addConstant("Brazil",                   new QoreNode((int64)QLocale::Brazil));
   qlocale->addConstant("BritishIndianOceanTerritory", new QoreNode((int64)QLocale::BritishIndianOceanTerritory));
   qlocale->addConstant("BruneiDarussalam",         new QoreNode((int64)QLocale::BruneiDarussalam));
   qlocale->addConstant("Bulgaria",                 new QoreNode((int64)QLocale::Bulgaria));
   qlocale->addConstant("BurkinaFaso",              new QoreNode((int64)QLocale::BurkinaFaso));
   qlocale->addConstant("Burundi",                  new QoreNode((int64)QLocale::Burundi));
   qlocale->addConstant("Cambodia",                 new QoreNode((int64)QLocale::Cambodia));
   qlocale->addConstant("Cameroon",                 new QoreNode((int64)QLocale::Cameroon));
   qlocale->addConstant("Canada",                   new QoreNode((int64)QLocale::Canada));
   qlocale->addConstant("CapeVerde",                new QoreNode((int64)QLocale::CapeVerde));
   qlocale->addConstant("CaymanIslands",            new QoreNode((int64)QLocale::CaymanIslands));
   qlocale->addConstant("CentralAfricanRepublic",   new QoreNode((int64)QLocale::CentralAfricanRepublic));
   qlocale->addConstant("Chad",                     new QoreNode((int64)QLocale::Chad));
   qlocale->addConstant("Chile",                    new QoreNode((int64)QLocale::Chile));
   qlocale->addConstant("China",                    new QoreNode((int64)QLocale::China));
   qlocale->addConstant("ChristmasIsland",          new QoreNode((int64)QLocale::ChristmasIsland));
   qlocale->addConstant("CocosIslands",             new QoreNode((int64)QLocale::CocosIslands));
   qlocale->addConstant("Colombia",                 new QoreNode((int64)QLocale::Colombia));
   qlocale->addConstant("Comoros",                  new QoreNode((int64)QLocale::Comoros));
   qlocale->addConstant("DemocraticRepublicOfCongo", new QoreNode((int64)QLocale::DemocraticRepublicOfCongo));
   qlocale->addConstant("PeoplesRepublicOfCongo",   new QoreNode((int64)QLocale::PeoplesRepublicOfCongo));
   qlocale->addConstant("CookIslands",              new QoreNode((int64)QLocale::CookIslands));
   qlocale->addConstant("CostaRica",                new QoreNode((int64)QLocale::CostaRica));
   qlocale->addConstant("IvoryCoast",               new QoreNode((int64)QLocale::IvoryCoast));
   qlocale->addConstant("Croatia",                  new QoreNode((int64)QLocale::Croatia));
   qlocale->addConstant("Cuba",                     new QoreNode((int64)QLocale::Cuba));
   qlocale->addConstant("Cyprus",                   new QoreNode((int64)QLocale::Cyprus));
   qlocale->addConstant("CzechRepublic",            new QoreNode((int64)QLocale::CzechRepublic));
   qlocale->addConstant("Denmark",                  new QoreNode((int64)QLocale::Denmark));
   qlocale->addConstant("Djibouti",                 new QoreNode((int64)QLocale::Djibouti));
   qlocale->addConstant("Dominica",                 new QoreNode((int64)QLocale::Dominica));
   qlocale->addConstant("DominicanRepublic",        new QoreNode((int64)QLocale::DominicanRepublic));
   qlocale->addConstant("EastTimor",                new QoreNode((int64)QLocale::EastTimor));
   qlocale->addConstant("Ecuador",                  new QoreNode((int64)QLocale::Ecuador));
   qlocale->addConstant("Egypt",                    new QoreNode((int64)QLocale::Egypt));
   qlocale->addConstant("ElSalvador",               new QoreNode((int64)QLocale::ElSalvador));
   qlocale->addConstant("EquatorialGuinea",         new QoreNode((int64)QLocale::EquatorialGuinea));
   qlocale->addConstant("Eritrea",                  new QoreNode((int64)QLocale::Eritrea));
   qlocale->addConstant("Estonia",                  new QoreNode((int64)QLocale::Estonia));
   qlocale->addConstant("Ethiopia",                 new QoreNode((int64)QLocale::Ethiopia));
   qlocale->addConstant("FalklandIslands",          new QoreNode((int64)QLocale::FalklandIslands));
   qlocale->addConstant("FaroeIslands",             new QoreNode((int64)QLocale::FaroeIslands));
   qlocale->addConstant("FijiCountry",              new QoreNode((int64)QLocale::FijiCountry));
   qlocale->addConstant("Finland",                  new QoreNode((int64)QLocale::Finland));
   qlocale->addConstant("France",                   new QoreNode((int64)QLocale::France));
   qlocale->addConstant("MetropolitanFrance",       new QoreNode((int64)QLocale::MetropolitanFrance));
   qlocale->addConstant("FrenchGuiana",             new QoreNode((int64)QLocale::FrenchGuiana));
   qlocale->addConstant("FrenchPolynesia",          new QoreNode((int64)QLocale::FrenchPolynesia));
   qlocale->addConstant("FrenchSouthernTerritories", new QoreNode((int64)QLocale::FrenchSouthernTerritories));
   qlocale->addConstant("Gabon",                    new QoreNode((int64)QLocale::Gabon));
   qlocale->addConstant("Gambia",                   new QoreNode((int64)QLocale::Gambia));
   qlocale->addConstant("Georgia",                  new QoreNode((int64)QLocale::Georgia));
   qlocale->addConstant("Germany",                  new QoreNode((int64)QLocale::Germany));
   qlocale->addConstant("Ghana",                    new QoreNode((int64)QLocale::Ghana));
   qlocale->addConstant("Gibraltar",                new QoreNode((int64)QLocale::Gibraltar));
   qlocale->addConstant("Greece",                   new QoreNode((int64)QLocale::Greece));
   qlocale->addConstant("Greenland",                new QoreNode((int64)QLocale::Greenland));
   qlocale->addConstant("Grenada",                  new QoreNode((int64)QLocale::Grenada));
   qlocale->addConstant("Guadeloupe",               new QoreNode((int64)QLocale::Guadeloupe));
   qlocale->addConstant("Guam",                     new QoreNode((int64)QLocale::Guam));
   qlocale->addConstant("Guatemala",                new QoreNode((int64)QLocale::Guatemala));
   qlocale->addConstant("Guinea",                   new QoreNode((int64)QLocale::Guinea));
   qlocale->addConstant("GuineaBissau",             new QoreNode((int64)QLocale::GuineaBissau));
   qlocale->addConstant("Guyana",                   new QoreNode((int64)QLocale::Guyana));
   qlocale->addConstant("Haiti",                    new QoreNode((int64)QLocale::Haiti));
   qlocale->addConstant("HeardAndMcDonaldIslands",  new QoreNode((int64)QLocale::HeardAndMcDonaldIslands));
   qlocale->addConstant("Honduras",                 new QoreNode((int64)QLocale::Honduras));
   qlocale->addConstant("HongKong",                 new QoreNode((int64)QLocale::HongKong));
   qlocale->addConstant("Hungary",                  new QoreNode((int64)QLocale::Hungary));
   qlocale->addConstant("Iceland",                  new QoreNode((int64)QLocale::Iceland));
   qlocale->addConstant("India",                    new QoreNode((int64)QLocale::India));
   qlocale->addConstant("Indonesia",                new QoreNode((int64)QLocale::Indonesia));
   qlocale->addConstant("Iran",                     new QoreNode((int64)QLocale::Iran));
   qlocale->addConstant("Iraq",                     new QoreNode((int64)QLocale::Iraq));
   qlocale->addConstant("Ireland",                  new QoreNode((int64)QLocale::Ireland));
   qlocale->addConstant("Israel",                   new QoreNode((int64)QLocale::Israel));
   qlocale->addConstant("Italy",                    new QoreNode((int64)QLocale::Italy));
   qlocale->addConstant("Jamaica",                  new QoreNode((int64)QLocale::Jamaica));
   qlocale->addConstant("Japan",                    new QoreNode((int64)QLocale::Japan));
   qlocale->addConstant("Jordan",                   new QoreNode((int64)QLocale::Jordan));
   qlocale->addConstant("Kazakhstan",               new QoreNode((int64)QLocale::Kazakhstan));
   qlocale->addConstant("Kenya",                    new QoreNode((int64)QLocale::Kenya));
   qlocale->addConstant("Kiribati",                 new QoreNode((int64)QLocale::Kiribati));
   qlocale->addConstant("DemocraticRepublicOfKorea", new QoreNode((int64)QLocale::DemocraticRepublicOfKorea));
   qlocale->addConstant("RepublicOfKorea",          new QoreNode((int64)QLocale::RepublicOfKorea));
   qlocale->addConstant("Kuwait",                   new QoreNode((int64)QLocale::Kuwait));
   qlocale->addConstant("Kyrgyzstan",               new QoreNode((int64)QLocale::Kyrgyzstan));
   qlocale->addConstant("Lao",                      new QoreNode((int64)QLocale::Lao));
   qlocale->addConstant("Latvia",                   new QoreNode((int64)QLocale::Latvia));
   qlocale->addConstant("Lebanon",                  new QoreNode((int64)QLocale::Lebanon));
   qlocale->addConstant("Lesotho",                  new QoreNode((int64)QLocale::Lesotho));
   qlocale->addConstant("Liberia",                  new QoreNode((int64)QLocale::Liberia));
   qlocale->addConstant("LibyanArabJamahiriya",     new QoreNode((int64)QLocale::LibyanArabJamahiriya));
   qlocale->addConstant("Liechtenstein",            new QoreNode((int64)QLocale::Liechtenstein));
   qlocale->addConstant("Lithuania",                new QoreNode((int64)QLocale::Lithuania));
   qlocale->addConstant("Luxembourg",               new QoreNode((int64)QLocale::Luxembourg));
   qlocale->addConstant("Macau",                    new QoreNode((int64)QLocale::Macau));
   qlocale->addConstant("Macedonia",                new QoreNode((int64)QLocale::Macedonia));
   qlocale->addConstant("Madagascar",               new QoreNode((int64)QLocale::Madagascar));
   qlocale->addConstant("Malawi",                   new QoreNode((int64)QLocale::Malawi));
   qlocale->addConstant("Malaysia",                 new QoreNode((int64)QLocale::Malaysia));
   qlocale->addConstant("Maldives",                 new QoreNode((int64)QLocale::Maldives));
   qlocale->addConstant("Mali",                     new QoreNode((int64)QLocale::Mali));
   qlocale->addConstant("Malta",                    new QoreNode((int64)QLocale::Malta));
   qlocale->addConstant("MarshallIslands",          new QoreNode((int64)QLocale::MarshallIslands));
   qlocale->addConstant("Martinique",               new QoreNode((int64)QLocale::Martinique));
   qlocale->addConstant("Mauritania",               new QoreNode((int64)QLocale::Mauritania));
   qlocale->addConstant("Mauritius",                new QoreNode((int64)QLocale::Mauritius));
   qlocale->addConstant("Mayotte",                  new QoreNode((int64)QLocale::Mayotte));
   qlocale->addConstant("Mexico",                   new QoreNode((int64)QLocale::Mexico));
   qlocale->addConstant("Micronesia",               new QoreNode((int64)QLocale::Micronesia));
   qlocale->addConstant("Moldova",                  new QoreNode((int64)QLocale::Moldova));
   qlocale->addConstant("Monaco",                   new QoreNode((int64)QLocale::Monaco));
   qlocale->addConstant("Mongolia",                 new QoreNode((int64)QLocale::Mongolia));
   qlocale->addConstant("Montserrat",               new QoreNode((int64)QLocale::Montserrat));
   qlocale->addConstant("Morocco",                  new QoreNode((int64)QLocale::Morocco));
   qlocale->addConstant("Mozambique",               new QoreNode((int64)QLocale::Mozambique));
   qlocale->addConstant("Myanmar",                  new QoreNode((int64)QLocale::Myanmar));
   qlocale->addConstant("Namibia",                  new QoreNode((int64)QLocale::Namibia));
   qlocale->addConstant("NauruCountry",             new QoreNode((int64)QLocale::NauruCountry));
   qlocale->addConstant("Nepal",                    new QoreNode((int64)QLocale::Nepal));
   qlocale->addConstant("Netherlands",              new QoreNode((int64)QLocale::Netherlands));
   qlocale->addConstant("NetherlandsAntilles",      new QoreNode((int64)QLocale::NetherlandsAntilles));
   qlocale->addConstant("NewCaledonia",             new QoreNode((int64)QLocale::NewCaledonia));
   qlocale->addConstant("NewZealand",               new QoreNode((int64)QLocale::NewZealand));
   qlocale->addConstant("Nicaragua",                new QoreNode((int64)QLocale::Nicaragua));
   qlocale->addConstant("Niger",                    new QoreNode((int64)QLocale::Niger));
   qlocale->addConstant("Nigeria",                  new QoreNode((int64)QLocale::Nigeria));
   qlocale->addConstant("Niue",                     new QoreNode((int64)QLocale::Niue));
   qlocale->addConstant("NorfolkIsland",            new QoreNode((int64)QLocale::NorfolkIsland));
   qlocale->addConstant("NorthernMarianaIslands",   new QoreNode((int64)QLocale::NorthernMarianaIslands));
   qlocale->addConstant("Norway",                   new QoreNode((int64)QLocale::Norway));
   qlocale->addConstant("Oman",                     new QoreNode((int64)QLocale::Oman));
   qlocale->addConstant("Pakistan",                 new QoreNode((int64)QLocale::Pakistan));
   qlocale->addConstant("Palau",                    new QoreNode((int64)QLocale::Palau));
   qlocale->addConstant("PalestinianTerritory",     new QoreNode((int64)QLocale::PalestinianTerritory));
   qlocale->addConstant("Panama",                   new QoreNode((int64)QLocale::Panama));
   qlocale->addConstant("PapuaNewGuinea",           new QoreNode((int64)QLocale::PapuaNewGuinea));
   qlocale->addConstant("Paraguay",                 new QoreNode((int64)QLocale::Paraguay));
   qlocale->addConstant("Peru",                     new QoreNode((int64)QLocale::Peru));
   qlocale->addConstant("Philippines",              new QoreNode((int64)QLocale::Philippines));
   qlocale->addConstant("Pitcairn",                 new QoreNode((int64)QLocale::Pitcairn));
   qlocale->addConstant("Poland",                   new QoreNode((int64)QLocale::Poland));
   qlocale->addConstant("Portugal",                 new QoreNode((int64)QLocale::Portugal));
   qlocale->addConstant("PuertoRico",               new QoreNode((int64)QLocale::PuertoRico));
   qlocale->addConstant("Qatar",                    new QoreNode((int64)QLocale::Qatar));
   qlocale->addConstant("Reunion",                  new QoreNode((int64)QLocale::Reunion));
   qlocale->addConstant("Romania",                  new QoreNode((int64)QLocale::Romania));
   qlocale->addConstant("RussianFederation",        new QoreNode((int64)QLocale::RussianFederation));
   qlocale->addConstant("Rwanda",                   new QoreNode((int64)QLocale::Rwanda));
   qlocale->addConstant("SaintKittsAndNevis",       new QoreNode((int64)QLocale::SaintKittsAndNevis));
   qlocale->addConstant("StLucia",                  new QoreNode((int64)QLocale::StLucia));
   qlocale->addConstant("StVincentAndTheGrenadines", new QoreNode((int64)QLocale::StVincentAndTheGrenadines));
   qlocale->addConstant("Samoa",                    new QoreNode((int64)QLocale::Samoa));
   qlocale->addConstant("SanMarino",                new QoreNode((int64)QLocale::SanMarino));
   qlocale->addConstant("SaoTomeAndPrincipe",       new QoreNode((int64)QLocale::SaoTomeAndPrincipe));
   qlocale->addConstant("SaudiArabia",              new QoreNode((int64)QLocale::SaudiArabia));
   qlocale->addConstant("Senegal",                  new QoreNode((int64)QLocale::Senegal));
   qlocale->addConstant("Seychelles",               new QoreNode((int64)QLocale::Seychelles));
   qlocale->addConstant("SierraLeone",              new QoreNode((int64)QLocale::SierraLeone));
   qlocale->addConstant("Singapore",                new QoreNode((int64)QLocale::Singapore));
   qlocale->addConstant("Slovakia",                 new QoreNode((int64)QLocale::Slovakia));
   qlocale->addConstant("Slovenia",                 new QoreNode((int64)QLocale::Slovenia));
   qlocale->addConstant("SolomonIslands",           new QoreNode((int64)QLocale::SolomonIslands));
   qlocale->addConstant("Somalia",                  new QoreNode((int64)QLocale::Somalia));
   qlocale->addConstant("SouthAfrica",              new QoreNode((int64)QLocale::SouthAfrica));
   qlocale->addConstant("SouthGeorgiaAndTheSouthSandwichIslands", new QoreNode((int64)QLocale::SouthGeorgiaAndTheSouthSandwichIslands));
   qlocale->addConstant("Spain",                    new QoreNode((int64)QLocale::Spain));
   qlocale->addConstant("SriLanka",                 new QoreNode((int64)QLocale::SriLanka));
   qlocale->addConstant("StHelena",                 new QoreNode((int64)QLocale::StHelena));
   qlocale->addConstant("StPierreAndMiquelon",      new QoreNode((int64)QLocale::StPierreAndMiquelon));
   qlocale->addConstant("Sudan",                    new QoreNode((int64)QLocale::Sudan));
   qlocale->addConstant("Suriname",                 new QoreNode((int64)QLocale::Suriname));
   qlocale->addConstant("SvalbardAndJanMayenIslands", new QoreNode((int64)QLocale::SvalbardAndJanMayenIslands));
   qlocale->addConstant("Swaziland",                new QoreNode((int64)QLocale::Swaziland));
   qlocale->addConstant("Sweden",                   new QoreNode((int64)QLocale::Sweden));
   qlocale->addConstant("Switzerland",              new QoreNode((int64)QLocale::Switzerland));
   qlocale->addConstant("SyrianArabRepublic",       new QoreNode((int64)QLocale::SyrianArabRepublic));
   qlocale->addConstant("Taiwan",                   new QoreNode((int64)QLocale::Taiwan));
   qlocale->addConstant("Tajikistan",               new QoreNode((int64)QLocale::Tajikistan));
   qlocale->addConstant("Tanzania",                 new QoreNode((int64)QLocale::Tanzania));
   qlocale->addConstant("Thailand",                 new QoreNode((int64)QLocale::Thailand));
   qlocale->addConstant("Togo",                     new QoreNode((int64)QLocale::Togo));
   qlocale->addConstant("Tokelau",                  new QoreNode((int64)QLocale::Tokelau));
   qlocale->addConstant("TongaCountry",             new QoreNode((int64)QLocale::TongaCountry));
   qlocale->addConstant("TrinidadAndTobago",        new QoreNode((int64)QLocale::TrinidadAndTobago));
   qlocale->addConstant("Tunisia",                  new QoreNode((int64)QLocale::Tunisia));
   qlocale->addConstant("Turkey",                   new QoreNode((int64)QLocale::Turkey));
   qlocale->addConstant("Turkmenistan",             new QoreNode((int64)QLocale::Turkmenistan));
   qlocale->addConstant("TurksAndCaicosIslands",    new QoreNode((int64)QLocale::TurksAndCaicosIslands));
   qlocale->addConstant("Tuvalu",                   new QoreNode((int64)QLocale::Tuvalu));
   qlocale->addConstant("Uganda",                   new QoreNode((int64)QLocale::Uganda));
   qlocale->addConstant("Ukraine",                  new QoreNode((int64)QLocale::Ukraine));
   qlocale->addConstant("UnitedArabEmirates",       new QoreNode((int64)QLocale::UnitedArabEmirates));
   qlocale->addConstant("UnitedKingdom",            new QoreNode((int64)QLocale::UnitedKingdom));
   qlocale->addConstant("UnitedStates",             new QoreNode((int64)QLocale::UnitedStates));
   qlocale->addConstant("UnitedStatesMinorOutlyingIslands", new QoreNode((int64)QLocale::UnitedStatesMinorOutlyingIslands));
   qlocale->addConstant("Uruguay",                  new QoreNode((int64)QLocale::Uruguay));
   qlocale->addConstant("Uzbekistan",               new QoreNode((int64)QLocale::Uzbekistan));
   qlocale->addConstant("Vanuatu",                  new QoreNode((int64)QLocale::Vanuatu));
   qlocale->addConstant("VaticanCityState",         new QoreNode((int64)QLocale::VaticanCityState));
   qlocale->addConstant("Venezuela",                new QoreNode((int64)QLocale::Venezuela));
   qlocale->addConstant("VietNam",                  new QoreNode((int64)QLocale::VietNam));
   qlocale->addConstant("BritishVirginIslands",     new QoreNode((int64)QLocale::BritishVirginIslands));
   qlocale->addConstant("USVirginIslands",          new QoreNode((int64)QLocale::USVirginIslands));
   qlocale->addConstant("WallisAndFutunaIslands",   new QoreNode((int64)QLocale::WallisAndFutunaIslands));
   qlocale->addConstant("WesternSahara",            new QoreNode((int64)QLocale::WesternSahara));
   qlocale->addConstant("Yemen",                    new QoreNode((int64)QLocale::Yemen));
   qlocale->addConstant("Yugoslavia",               new QoreNode((int64)QLocale::Yugoslavia));
   qlocale->addConstant("Zambia",                   new QoreNode((int64)QLocale::Zambia));
   qlocale->addConstant("Zimbabwe",                 new QoreNode((int64)QLocale::Zimbabwe));
   qlocale->addConstant("SerbiaAndMontenegro",      new QoreNode((int64)QLocale::SerbiaAndMontenegro));
   qlocale->addConstant("LastCountry",              new QoreNode((int64)QLocale::LastCountry));

   qt_ns->addInitialNamespace(qlocale);

   // add QFont namespaces and constants
   class QoreNamespace *qframens = new QoreNamespace("QFrame");
   // Shadow enum
   qframens->addConstant("Plain",    new QoreNode((int64)QFrame::Plain));
   qframens->addConstant("Raised",   new QoreNode((int64)QFrame::Raised));
   qframens->addConstant("Sunken",   new QoreNode((int64)QFrame::Sunken));

   // Shape enum
   qframens->addConstant("NoFrame",      new QoreNode((int64)QFrame::NoFrame));
   qframens->addConstant("Box",          new QoreNode((int64)QFrame::Box));
   qframens->addConstant("Panel",        new QoreNode((int64)QFrame::Panel));
   qframens->addConstant("StyledPanel",  new QoreNode((int64)QFrame::StyledPanel));
   qframens->addConstant("HLine",        new QoreNode((int64)QFrame::HLine));
   qframens->addConstant("VLine",        new QoreNode((int64)QFrame::VLine));
   qframens->addConstant("WinPanel",     new QoreNode((int64)QFrame::WinPanel));

   // StyleMask
   qframens->addConstant("Shadow_Mask",  new QoreNode((int64)QFrame::Shadow_Mask));
   qframens->addConstant("Shape_Mask",   new QoreNode((int64)QFrame::Shape_Mask));

   qt_ns->addInitialNamespace(qframens);

   // add QFont namespaces and constants
   class QoreNamespace *qf = new QoreNamespace("QFont");
   // Weight enum
   qf->addConstant("Light",    new QoreNode((int64)QFont::Light));
   qf->addConstant("Normal",   new QoreNode((int64)QFont::Normal));
   qf->addConstant("DemiBold", new QoreNode((int64)QFont::DemiBold));
   qf->addConstant("Bold",     new QoreNode((int64)QFont::Bold));
   qf->addConstant("Black",    new QoreNode((int64)QFont::Black));

   // StyleHint enum
   qf->addConstant("Helvetica",    new QoreNode((int64)QFont::Helvetica));
   qf->addConstant("SansSerif",    new QoreNode((int64)QFont::SansSerif));
   qf->addConstant("Times",        new QoreNode((int64)QFont::Times));
   qf->addConstant("Serif",        new QoreNode((int64)QFont::Serif));
   qf->addConstant("Courier",      new QoreNode((int64)QFont::Courier));
   qf->addConstant("TypeWriter",   new QoreNode((int64)QFont::TypeWriter));
   qf->addConstant("OldEnglish",   new QoreNode((int64)QFont::OldEnglish));
   qf->addConstant("Decorative",   new QoreNode((int64)QFont::Decorative));
   qf->addConstant("System",       new QoreNode((int64)QFont::System));
   qf->addConstant("AnyStyle",     new QoreNode((int64)QFont::AnyStyle));

   // StyleStrategy
   qf->addConstant("PreferDefault",    new QoreNode((int64)QFont::PreferDefault));
   qf->addConstant("PreferBitmap",     new QoreNode((int64)QFont::PreferBitmap));
   qf->addConstant("PreferDevice",     new QoreNode((int64)QFont::PreferDevice));
   qf->addConstant("PreferOutline",    new QoreNode((int64)QFont::PreferOutline));
   qf->addConstant("ForceOutline",     new QoreNode((int64)QFont::ForceOutline));
   qf->addConstant("PreferMatch",      new QoreNode((int64)QFont::PreferMatch));
   qf->addConstant("PreferQuality",    new QoreNode((int64)QFont::PreferQuality));
   qf->addConstant("PreferAntialias",  new QoreNode((int64)QFont::PreferAntialias));
   qf->addConstant("NoAntialias",      new QoreNode((int64)QFont::NoAntialias));
   qf->addConstant("OpenGLCompatible", new QoreNode((int64)QFont::OpenGLCompatible));
   qf->addConstant("NoFontMerging",    new QoreNode((int64)QFont::NoFontMerging));

   // Style enum
   qf->addConstant("StyleNormal",   new QoreNode((int64)QFont::StyleNormal));
   qf->addConstant("StyleItalic",   new QoreNode((int64)QFont::StyleItalic));
   qf->addConstant("StyleOblique",  new QoreNode((int64)QFont::StyleOblique));

   // Stretch enum
   qf->addConstant("UltraCondensed",  new QoreNode((int64)QFont::UltraCondensed));
   qf->addConstant("ExtraCondensed",  new QoreNode((int64)QFont::ExtraCondensed));
   qf->addConstant("Condensed",       new QoreNode((int64)QFont::Condensed));
   qf->addConstant("SemiCondensed",   new QoreNode((int64)QFont::SemiCondensed));
   qf->addConstant("Unstretched",     new QoreNode((int64)QFont::Unstretched));
   qf->addConstant("SemiExpanded",    new QoreNode((int64)QFont::SemiExpanded));
   qf->addConstant("Expanded",        new QoreNode((int64)QFont::Expanded));
   qf->addConstant("ExtraExpanded",   new QoreNode((int64)QFont::ExtraExpanded));
   qf->addConstant("UltraExpanded",   new QoreNode((int64)QFont::UltraExpanded));

   qt_ns->addInitialNamespace(qf);

   // add QLCDNumber namespace and constants
   class QoreNamespace *qlcdn = new QoreNamespace("QLCDNumber");
   qlcdn->addConstant("Outline",   new QoreNode((int64)QLCDNumber::Outline));
   qlcdn->addConstant("Filled",    new QoreNode((int64)QLCDNumber::Filled));
   qlcdn->addConstant("Flat",      new QoreNode((int64)QLCDNumber::Flat));
   qlcdn->addConstant("Hex",       new QoreNode((int64)QLCDNumber::Hex));
   qlcdn->addConstant("Dec",       new QoreNode((int64)QLCDNumber::Dec));
   qlcdn->addConstant("Oct",       new QoreNode((int64)QLCDNumber::Oct));
   qlcdn->addConstant("Bin",       new QoreNode((int64)QLCDNumber::Bin));
   qt_ns->addInitialNamespace(qlcdn);

   // add QAbstractSlider namespace and constants
   class QoreNamespace *qas = new QoreNamespace("QAbstractSlider");
   qas->addConstant("SliderNoAction",        new QoreNode((int64)QAbstractSlider::SliderNoAction));
   qas->addConstant("SliderSingleStepAdd",   new QoreNode((int64)QAbstractSlider::SliderSingleStepAdd));
   qas->addConstant("SliderSingleStepSub",   new QoreNode((int64)QAbstractSlider::SliderSingleStepSub));
   qas->addConstant("SliderPageStepAdd",     new QoreNode((int64)QAbstractSlider::SliderPageStepAdd));
   qas->addConstant("SliderPageStepSub",     new QoreNode((int64)QAbstractSlider::SliderPageStepSub));
   qas->addConstant("SliderToMinimum",       new QoreNode((int64)QAbstractSlider::SliderToMinimum));
   qas->addConstant("SliderToMaximum",       new QoreNode((int64)QAbstractSlider::SliderToMaximum));
   qas->addConstant("SliderMove",            new QoreNode((int64)QAbstractSlider::SliderMove));
   qt_ns->addInitialNamespace(qas);

   // CheckState enum
   qt_ns->addConstant("Unchecked",                new QoreNode((int64)Qt::Unchecked));
   qt_ns->addConstant("PartiallyChecked",         new QoreNode((int64)Qt::PartiallyChecked));
   qt_ns->addConstant("Checked",                  new QoreNode((int64)Qt::Checked));

   // orientation enum values
   qt_ns->addConstant("Vertical",        new QoreNode((int64)Qt::Vertical));
   qt_ns->addConstant("Horizontal",      new QoreNode((int64)Qt::Horizontal));

   // GlobalColor enum
   qt_ns->addConstant("color0",            new QoreNode((int64)Qt::color0));
   qt_ns->addConstant("color1",            new QoreNode((int64)Qt::color1));
   qt_ns->addConstant("black",             new QoreNode((int64)Qt::black));
   qt_ns->addConstant("white",             new QoreNode((int64)Qt::white));
   qt_ns->addConstant("darkGray",          new QoreNode((int64)Qt::darkGray));
   qt_ns->addConstant("gray",              new QoreNode((int64)Qt::gray));
   qt_ns->addConstant("lightGray",         new QoreNode((int64)Qt::lightGray));
   qt_ns->addConstant("red",               new QoreNode((int64)Qt::red));
   qt_ns->addConstant("green",             new QoreNode((int64)Qt::green));
   qt_ns->addConstant("blue",              new QoreNode((int64)Qt::blue));
   qt_ns->addConstant("cyan",              new QoreNode((int64)Qt::cyan));
   qt_ns->addConstant("magenta",           new QoreNode((int64)Qt::magenta));
   qt_ns->addConstant("yellow",            new QoreNode((int64)Qt::yellow));
   qt_ns->addConstant("darkRed",           new QoreNode((int64)Qt::darkRed));
   qt_ns->addConstant("darkGreen",         new QoreNode((int64)Qt::darkGreen));
   qt_ns->addConstant("darkBlue",          new QoreNode((int64)Qt::darkBlue));
   qt_ns->addConstant("darkCyan",          new QoreNode((int64)Qt::darkCyan));
   qt_ns->addConstant("darkMagenta",       new QoreNode((int64)Qt::darkMagenta));
   qt_ns->addConstant("darkYellow",        new QoreNode((int64)Qt::darkYellow));
   qt_ns->addConstant("transparent",       new QoreNode((int64)Qt::transparent));

   // BrushStyle enum
   qt_ns->addConstant("NoBrush",                  make_enum(NT_BRUSHSTYLE, (int)Qt::NoBrush));
   qt_ns->addConstant("SolidPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::SolidPattern));
   qt_ns->addConstant("Dense1Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense1Pattern));
   qt_ns->addConstant("Dense2Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense2Pattern));
   qt_ns->addConstant("Dense3Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense3Pattern));
   qt_ns->addConstant("Dense4Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense4Pattern));
   qt_ns->addConstant("Dense5Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense5Pattern));
   qt_ns->addConstant("Dense6Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense6Pattern));
   qt_ns->addConstant("Dense7Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense7Pattern));
   qt_ns->addConstant("HorPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::HorPattern));
   qt_ns->addConstant("VerPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::VerPattern));
   qt_ns->addConstant("CrossPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::CrossPattern));
   qt_ns->addConstant("BDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::BDiagPattern));
   qt_ns->addConstant("FDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::FDiagPattern));
   qt_ns->addConstant("DiagCrossPattern",         make_enum(NT_BRUSHSTYLE, (int)Qt::DiagCrossPattern));
   qt_ns->addConstant("LinearGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::LinearGradientPattern));
   qt_ns->addConstant("RadialGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::RadialGradientPattern));
   qt_ns->addConstant("ConicalGradientPattern",   make_enum(NT_BRUSHSTYLE, (int)Qt::ConicalGradientPattern));
   qt_ns->addConstant("TexturePattern",           make_enum(NT_BRUSHSTYLE, (int)Qt::TexturePattern));

   // PenStyle enum
   qt_ns->addConstant("NoPen",             make_enum(NT_PENSTYLE, (int)Qt::NoPen));
   qt_ns->addConstant("SolidLine",         make_enum(NT_PENSTYLE, (int)Qt::SolidLine));
   qt_ns->addConstant("DashLine",          make_enum(NT_PENSTYLE, (int)Qt::DashLine));
   qt_ns->addConstant("DotLine",           make_enum(NT_PENSTYLE, (int)Qt::DotLine));
   qt_ns->addConstant("DashDotLine",       make_enum(NT_PENSTYLE, (int)Qt::DashDotLine));
   qt_ns->addConstant("DashDotDotLine",    make_enum(NT_PENSTYLE, (int)Qt::DashDotDotLine));
   qt_ns->addConstant("CustomDashLine",    make_enum(NT_PENSTYLE, (int)Qt::CustomDashLine));

   // AlignmentFlag enum
   qt_ns->addConstant("AlignLeft",                new QoreNode((int64)Qt::AlignLeft));
   qt_ns->addConstant("AlignLeading",             new QoreNode((int64)Qt::AlignLeading));
   qt_ns->addConstant("AlignRight",               new QoreNode((int64)Qt::AlignRight));
   qt_ns->addConstant("AlignTrailing",            new QoreNode((int64)Qt::AlignTrailing));
   qt_ns->addConstant("AlignHCenter",             new QoreNode((int64)Qt::AlignHCenter));
   qt_ns->addConstant("AlignJustify",             new QoreNode((int64)Qt::AlignJustify));
   qt_ns->addConstant("AlignAbsolute",            new QoreNode((int64)Qt::AlignAbsolute));
   qt_ns->addConstant("AlignHorizontal_Mask",     new QoreNode((int64)Qt::AlignHorizontal_Mask));
   qt_ns->addConstant("AlignTop",                 new QoreNode((int64)Qt::AlignTop));
   qt_ns->addConstant("AlignBottom",              new QoreNode((int64)Qt::AlignBottom));
   qt_ns->addConstant("AlignVCenter",             new QoreNode((int64)Qt::AlignVCenter));
   qt_ns->addConstant("AlignVertical_Mask",       new QoreNode((int64)Qt::AlignVertical_Mask));
   qt_ns->addConstant("AlignCenter",              new QoreNode((int64)Qt::AlignCenter));

   // MouseButton enum
   qt_ns->addConstant("NoButton",                 new QoreNode((int64)Qt::NoButton));
   qt_ns->addConstant("LeftButton",               new QoreNode((int64)Qt::LeftButton));
   qt_ns->addConstant("RightButton",              new QoreNode((int64)Qt::RightButton));
   qt_ns->addConstant("MidButton",                new QoreNode((int64)Qt::MidButton));
   qt_ns->addConstant("XButton1",                 new QoreNode((int64)Qt::XButton1));
   qt_ns->addConstant("XButton2",                 new QoreNode((int64)Qt::XButton2));
   qt_ns->addConstant("MouseButtonMask",          new QoreNode((int64)Qt::MouseButtonMask));

   // Modifier enum
   qt_ns->addConstant("META",                     new QoreNode((int64)Qt::META));
   qt_ns->addConstant("SHIFT",                    new QoreNode((int64)Qt::SHIFT));
   qt_ns->addConstant("CTRL",                     new QoreNode((int64)Qt::CTRL));
   qt_ns->addConstant("ALT",                      new QoreNode((int64)Qt::ALT));
   qt_ns->addConstant("MODIFIER_MASK",            new QoreNode((int64)Qt::MODIFIER_MASK));
   qt_ns->addConstant("UNICODE_ACCEL",            new QoreNode((int64)Qt::UNICODE_ACCEL));

   // DayOfWeek
   qt_ns->addConstant("Monday",                   new QoreNode((int64)Qt::Monday));
   qt_ns->addConstant("Tuesday",                  new QoreNode((int64)Qt::Tuesday));
   qt_ns->addConstant("Wednesday",                new QoreNode((int64)Qt::Wednesday));
   qt_ns->addConstant("Thursday",                 new QoreNode((int64)Qt::Thursday));
   qt_ns->addConstant("Friday",                   new QoreNode((int64)Qt::Friday));
   qt_ns->addConstant("Saturday",                 new QoreNode((int64)Qt::Saturday));
   qt_ns->addConstant("Sunday",                   new QoreNode((int64)Qt::Sunday));

   // ContextMenuPolicy enum
   qt_ns->addConstant("NoContextMenu",            new QoreNode((int64)Qt::NoContextMenu));
   qt_ns->addConstant("DefaultContextMenu",       new QoreNode((int64)Qt::DefaultContextMenu));
   qt_ns->addConstant("ActionsContextMenu",       new QoreNode((int64)Qt::ActionsContextMenu));
   qt_ns->addConstant("CustomContextMenu",        new QoreNode((int64)Qt::CustomContextMenu));
   qt_ns->addConstant("PreventContextMenu",       new QoreNode((int64)Qt::PreventContextMenu));

   // Key enum
   qt_ns->addConstant("Key_Escape",               new QoreNode((int64)Qt::Key_Escape));
   qt_ns->addConstant("Key_Tab",                  new QoreNode((int64)Qt::Key_Tab));
   qt_ns->addConstant("Key_Backtab",              new QoreNode((int64)Qt::Key_Backtab));
   qt_ns->addConstant("Key_Backspace",            new QoreNode((int64)Qt::Key_Backspace));
   qt_ns->addConstant("Key_Return",               new QoreNode((int64)Qt::Key_Return));
   qt_ns->addConstant("Key_Enter",                new QoreNode((int64)Qt::Key_Enter));
   qt_ns->addConstant("Key_Insert",               new QoreNode((int64)Qt::Key_Insert));
   qt_ns->addConstant("Key_Delete",               new QoreNode((int64)Qt::Key_Delete));
   qt_ns->addConstant("Key_Pause",                new QoreNode((int64)Qt::Key_Pause));
   qt_ns->addConstant("Key_Print",                new QoreNode((int64)Qt::Key_Print));
   qt_ns->addConstant("Key_SysReq",               new QoreNode((int64)Qt::Key_SysReq));
   qt_ns->addConstant("Key_Clear",                new QoreNode((int64)Qt::Key_Clear));
   qt_ns->addConstant("Key_Home",                 new QoreNode((int64)Qt::Key_Home));
   qt_ns->addConstant("Key_End",                  new QoreNode((int64)Qt::Key_End));
   qt_ns->addConstant("Key_Left",                 new QoreNode((int64)Qt::Key_Left));
   qt_ns->addConstant("Key_Up",                   new QoreNode((int64)Qt::Key_Up));
   qt_ns->addConstant("Key_Right",                new QoreNode((int64)Qt::Key_Right));
   qt_ns->addConstant("Key_Down",                 new QoreNode((int64)Qt::Key_Down));
   qt_ns->addConstant("Key_PageUp",               new QoreNode((int64)Qt::Key_PageUp));
   qt_ns->addConstant("Key_PageDown",             new QoreNode((int64)Qt::Key_PageDown));
   qt_ns->addConstant("Key_Shift",                new QoreNode((int64)Qt::Key_Shift));
   qt_ns->addConstant("Key_Control",              new QoreNode((int64)Qt::Key_Control));
   qt_ns->addConstant("Key_Meta",                 new QoreNode((int64)Qt::Key_Meta));
   qt_ns->addConstant("Key_Alt",                  new QoreNode((int64)Qt::Key_Alt));
   qt_ns->addConstant("Key_CapsLock",             new QoreNode((int64)Qt::Key_CapsLock));
   qt_ns->addConstant("Key_NumLock",              new QoreNode((int64)Qt::Key_NumLock));
   qt_ns->addConstant("Key_ScrollLock",           new QoreNode((int64)Qt::Key_ScrollLock));
   qt_ns->addConstant("Key_F1",                   new QoreNode((int64)Qt::Key_F1));
   qt_ns->addConstant("Key_F2",                   new QoreNode((int64)Qt::Key_F2));
   qt_ns->addConstant("Key_F3",                   new QoreNode((int64)Qt::Key_F3));
   qt_ns->addConstant("Key_F4",                   new QoreNode((int64)Qt::Key_F4));
   qt_ns->addConstant("Key_F5",                   new QoreNode((int64)Qt::Key_F5));
   qt_ns->addConstant("Key_F6",                   new QoreNode((int64)Qt::Key_F6));
   qt_ns->addConstant("Key_F7",                   new QoreNode((int64)Qt::Key_F7));
   qt_ns->addConstant("Key_F8",                   new QoreNode((int64)Qt::Key_F8));
   qt_ns->addConstant("Key_F9",                   new QoreNode((int64)Qt::Key_F9));
   qt_ns->addConstant("Key_F10",                  new QoreNode((int64)Qt::Key_F10));
   qt_ns->addConstant("Key_F11",                  new QoreNode((int64)Qt::Key_F11));
   qt_ns->addConstant("Key_F12",                  new QoreNode((int64)Qt::Key_F12));
   qt_ns->addConstant("Key_F13",                  new QoreNode((int64)Qt::Key_F13));
   qt_ns->addConstant("Key_F14",                  new QoreNode((int64)Qt::Key_F14));
   qt_ns->addConstant("Key_F15",                  new QoreNode((int64)Qt::Key_F15));
   qt_ns->addConstant("Key_F16",                  new QoreNode((int64)Qt::Key_F16));
   qt_ns->addConstant("Key_F17",                  new QoreNode((int64)Qt::Key_F17));
   qt_ns->addConstant("Key_F18",                  new QoreNode((int64)Qt::Key_F18));
   qt_ns->addConstant("Key_F19",                  new QoreNode((int64)Qt::Key_F19));
   qt_ns->addConstant("Key_F20",                  new QoreNode((int64)Qt::Key_F20));
   qt_ns->addConstant("Key_F21",                  new QoreNode((int64)Qt::Key_F21));
   qt_ns->addConstant("Key_F22",                  new QoreNode((int64)Qt::Key_F22));
   qt_ns->addConstant("Key_F23",                  new QoreNode((int64)Qt::Key_F23));
   qt_ns->addConstant("Key_F24",                  new QoreNode((int64)Qt::Key_F24));
   qt_ns->addConstant("Key_F25",                  new QoreNode((int64)Qt::Key_F25));
   qt_ns->addConstant("Key_F26",                  new QoreNode((int64)Qt::Key_F26));
   qt_ns->addConstant("Key_F27",                  new QoreNode((int64)Qt::Key_F27));
   qt_ns->addConstant("Key_F28",                  new QoreNode((int64)Qt::Key_F28));
   qt_ns->addConstant("Key_F29",                  new QoreNode((int64)Qt::Key_F29));
   qt_ns->addConstant("Key_F30",                  new QoreNode((int64)Qt::Key_F30));
   qt_ns->addConstant("Key_F31",                  new QoreNode((int64)Qt::Key_F31));
   qt_ns->addConstant("Key_F32",                  new QoreNode((int64)Qt::Key_F32));
   qt_ns->addConstant("Key_F33",                  new QoreNode((int64)Qt::Key_F33));
   qt_ns->addConstant("Key_F34",                  new QoreNode((int64)Qt::Key_F34));
   qt_ns->addConstant("Key_F35",                  new QoreNode((int64)Qt::Key_F35));
   qt_ns->addConstant("Key_Super_L",              new QoreNode((int64)Qt::Key_Super_L));
   qt_ns->addConstant("Key_Super_R",              new QoreNode((int64)Qt::Key_Super_R));
   qt_ns->addConstant("Key_Menu",                 new QoreNode((int64)Qt::Key_Menu));
   qt_ns->addConstant("Key_Hyper_L",              new QoreNode((int64)Qt::Key_Hyper_L));
   qt_ns->addConstant("Key_Hyper_R",              new QoreNode((int64)Qt::Key_Hyper_R));
   qt_ns->addConstant("Key_Help",                 new QoreNode((int64)Qt::Key_Help));
   qt_ns->addConstant("Key_Direction_L",          new QoreNode((int64)Qt::Key_Direction_L));
   qt_ns->addConstant("Key_Direction_R",          new QoreNode((int64)Qt::Key_Direction_R));
   qt_ns->addConstant("Key_Space",                new QoreNode((int64)Qt::Key_Space));
   qt_ns->addConstant("Key_Any",                  new QoreNode((int64)Qt::Key_Any));
   qt_ns->addConstant("Key_Exclam",               new QoreNode((int64)Qt::Key_Exclam));
   qt_ns->addConstant("Key_QuoteDbl",             new QoreNode((int64)Qt::Key_QuoteDbl));
   qt_ns->addConstant("Key_NumberSign",           new QoreNode((int64)Qt::Key_NumberSign));
   qt_ns->addConstant("Key_Dollar",               new QoreNode((int64)Qt::Key_Dollar));
   qt_ns->addConstant("Key_Percent",              new QoreNode((int64)Qt::Key_Percent));
   qt_ns->addConstant("Key_Ampersand",            new QoreNode((int64)Qt::Key_Ampersand));
   qt_ns->addConstant("Key_Apostrophe",           new QoreNode((int64)Qt::Key_Apostrophe));
   qt_ns->addConstant("Key_ParenLeft",            new QoreNode((int64)Qt::Key_ParenLeft));
   qt_ns->addConstant("Key_ParenRight",           new QoreNode((int64)Qt::Key_ParenRight));
   qt_ns->addConstant("Key_Asterisk",             new QoreNode((int64)Qt::Key_Asterisk));
   qt_ns->addConstant("Key_Plus",                 new QoreNode((int64)Qt::Key_Plus));
   qt_ns->addConstant("Key_Comma",                new QoreNode((int64)Qt::Key_Comma));
   qt_ns->addConstant("Key_Minus",                new QoreNode((int64)Qt::Key_Minus));
   qt_ns->addConstant("Key_Period",               new QoreNode((int64)Qt::Key_Period));
   qt_ns->addConstant("Key_Slash",                new QoreNode((int64)Qt::Key_Slash));
   qt_ns->addConstant("Key_0",                    new QoreNode((int64)Qt::Key_0));
   qt_ns->addConstant("Key_1",                    new QoreNode((int64)Qt::Key_1));
   qt_ns->addConstant("Key_2",                    new QoreNode((int64)Qt::Key_2));
   qt_ns->addConstant("Key_3",                    new QoreNode((int64)Qt::Key_3));
   qt_ns->addConstant("Key_4",                    new QoreNode((int64)Qt::Key_4));
   qt_ns->addConstant("Key_5",                    new QoreNode((int64)Qt::Key_5));
   qt_ns->addConstant("Key_6",                    new QoreNode((int64)Qt::Key_6));
   qt_ns->addConstant("Key_7",                    new QoreNode((int64)Qt::Key_7));
   qt_ns->addConstant("Key_8",                    new QoreNode((int64)Qt::Key_8));
   qt_ns->addConstant("Key_9",                    new QoreNode((int64)Qt::Key_9));
   qt_ns->addConstant("Key_Colon",                new QoreNode((int64)Qt::Key_Colon));
   qt_ns->addConstant("Key_Semicolon",            new QoreNode((int64)Qt::Key_Semicolon));
   qt_ns->addConstant("Key_Less",                 new QoreNode((int64)Qt::Key_Less));
   qt_ns->addConstant("Key_Equal",                new QoreNode((int64)Qt::Key_Equal));
   qt_ns->addConstant("Key_Greater",              new QoreNode((int64)Qt::Key_Greater));
   qt_ns->addConstant("Key_Question",             new QoreNode((int64)Qt::Key_Question));
   qt_ns->addConstant("Key_At",                   new QoreNode((int64)Qt::Key_At));
   qt_ns->addConstant("Key_A",                    new QoreNode((int64)Qt::Key_A));
   qt_ns->addConstant("Key_B",                    new QoreNode((int64)Qt::Key_B));
   qt_ns->addConstant("Key_C",                    new QoreNode((int64)Qt::Key_C));
   qt_ns->addConstant("Key_D",                    new QoreNode((int64)Qt::Key_D));
   qt_ns->addConstant("Key_E",                    new QoreNode((int64)Qt::Key_E));
   qt_ns->addConstant("Key_F",                    new QoreNode((int64)Qt::Key_F));
   qt_ns->addConstant("Key_G",                    new QoreNode((int64)Qt::Key_G));
   qt_ns->addConstant("Key_H",                    new QoreNode((int64)Qt::Key_H));
   qt_ns->addConstant("Key_I",                    new QoreNode((int64)Qt::Key_I));
   qt_ns->addConstant("Key_J",                    new QoreNode((int64)Qt::Key_J));
   qt_ns->addConstant("Key_K",                    new QoreNode((int64)Qt::Key_K));
   qt_ns->addConstant("Key_L",                    new QoreNode((int64)Qt::Key_L));
   qt_ns->addConstant("Key_M",                    new QoreNode((int64)Qt::Key_M));
   qt_ns->addConstant("Key_N",                    new QoreNode((int64)Qt::Key_N));
   qt_ns->addConstant("Key_O",                    new QoreNode((int64)Qt::Key_O));
   qt_ns->addConstant("Key_P",                    new QoreNode((int64)Qt::Key_P));
   qt_ns->addConstant("Key_Q",                    new QoreNode((int64)Qt::Key_Q));
   qt_ns->addConstant("Key_R",                    new QoreNode((int64)Qt::Key_R));
   qt_ns->addConstant("Key_S",                    new QoreNode((int64)Qt::Key_S));
   qt_ns->addConstant("Key_T",                    new QoreNode((int64)Qt::Key_T));
   qt_ns->addConstant("Key_U",                    new QoreNode((int64)Qt::Key_U));
   qt_ns->addConstant("Key_V",                    new QoreNode((int64)Qt::Key_V));
   qt_ns->addConstant("Key_W",                    new QoreNode((int64)Qt::Key_W));
   qt_ns->addConstant("Key_X",                    new QoreNode((int64)Qt::Key_X));
   qt_ns->addConstant("Key_Y",                    new QoreNode((int64)Qt::Key_Y));
   qt_ns->addConstant("Key_Z",                    new QoreNode((int64)Qt::Key_Z));
   qt_ns->addConstant("Key_BracketLeft",          new QoreNode((int64)Qt::Key_BracketLeft));
   qt_ns->addConstant("Key_Backslash",            new QoreNode((int64)Qt::Key_Backslash));
   qt_ns->addConstant("Key_BracketRight",         new QoreNode((int64)Qt::Key_BracketRight));
   qt_ns->addConstant("Key_AsciiCircum",          new QoreNode((int64)Qt::Key_AsciiCircum));
   qt_ns->addConstant("Key_Underscore",           new QoreNode((int64)Qt::Key_Underscore));
   qt_ns->addConstant("Key_QuoteLeft",            new QoreNode((int64)Qt::Key_QuoteLeft));
   qt_ns->addConstant("Key_BraceLeft",            new QoreNode((int64)Qt::Key_BraceLeft));
   qt_ns->addConstant("Key_Bar",                  new QoreNode((int64)Qt::Key_Bar));
   qt_ns->addConstant("Key_BraceRight",           new QoreNode((int64)Qt::Key_BraceRight));
   qt_ns->addConstant("Key_AsciiTilde",           new QoreNode((int64)Qt::Key_AsciiTilde));
   qt_ns->addConstant("Key_nobreakspace",         new QoreNode((int64)Qt::Key_nobreakspace));
   qt_ns->addConstant("Key_exclamdown",           new QoreNode((int64)Qt::Key_exclamdown));
   qt_ns->addConstant("Key_cent",                 new QoreNode((int64)Qt::Key_cent));
   qt_ns->addConstant("Key_sterling",             new QoreNode((int64)Qt::Key_sterling));
   qt_ns->addConstant("Key_currency",             new QoreNode((int64)Qt::Key_currency));
   qt_ns->addConstant("Key_yen",                  new QoreNode((int64)Qt::Key_yen));
   qt_ns->addConstant("Key_brokenbar",            new QoreNode((int64)Qt::Key_brokenbar));
   qt_ns->addConstant("Key_section",              new QoreNode((int64)Qt::Key_section));
   qt_ns->addConstant("Key_diaeresis",            new QoreNode((int64)Qt::Key_diaeresis));
   qt_ns->addConstant("Key_copyright",            new QoreNode((int64)Qt::Key_copyright));
   qt_ns->addConstant("Key_ordfeminine",          new QoreNode((int64)Qt::Key_ordfeminine));
   qt_ns->addConstant("Key_guillemotleft",        new QoreNode((int64)Qt::Key_guillemotleft));
   qt_ns->addConstant("Key_notsign",              new QoreNode((int64)Qt::Key_notsign));
   qt_ns->addConstant("Key_hyphen",               new QoreNode((int64)Qt::Key_hyphen));
   qt_ns->addConstant("Key_registered",           new QoreNode((int64)Qt::Key_registered));
   qt_ns->addConstant("Key_macron",               new QoreNode((int64)Qt::Key_macron));
   qt_ns->addConstant("Key_degree",               new QoreNode((int64)Qt::Key_degree));
   qt_ns->addConstant("Key_plusminus",            new QoreNode((int64)Qt::Key_plusminus));
   qt_ns->addConstant("Key_twosuperior",          new QoreNode((int64)Qt::Key_twosuperior));
   qt_ns->addConstant("Key_threesuperior",        new QoreNode((int64)Qt::Key_threesuperior));
   qt_ns->addConstant("Key_acute",                new QoreNode((int64)Qt::Key_acute));
   qt_ns->addConstant("Key_mu",                   new QoreNode((int64)Qt::Key_mu));
   qt_ns->addConstant("Key_paragraph",            new QoreNode((int64)Qt::Key_paragraph));
   qt_ns->addConstant("Key_periodcentered",       new QoreNode((int64)Qt::Key_periodcentered));
   qt_ns->addConstant("Key_cedilla",              new QoreNode((int64)Qt::Key_cedilla));
   qt_ns->addConstant("Key_onesuperior",          new QoreNode((int64)Qt::Key_onesuperior));
   qt_ns->addConstant("Key_masculine",            new QoreNode((int64)Qt::Key_masculine));
   qt_ns->addConstant("Key_guillemotright",       new QoreNode((int64)Qt::Key_guillemotright));
   qt_ns->addConstant("Key_onequarter",           new QoreNode((int64)Qt::Key_onequarter));
   qt_ns->addConstant("Key_onehalf",              new QoreNode((int64)Qt::Key_onehalf));
   qt_ns->addConstant("Key_threequarters",        new QoreNode((int64)Qt::Key_threequarters));
   qt_ns->addConstant("Key_questiondown",         new QoreNode((int64)Qt::Key_questiondown));
   qt_ns->addConstant("Key_Agrave",               new QoreNode((int64)Qt::Key_Agrave));
   qt_ns->addConstant("Key_Aacute",               new QoreNode((int64)Qt::Key_Aacute));
   qt_ns->addConstant("Key_Acircumflex",          new QoreNode((int64)Qt::Key_Acircumflex));
   qt_ns->addConstant("Key_Atilde",               new QoreNode((int64)Qt::Key_Atilde));
   qt_ns->addConstant("Key_Adiaeresis",           new QoreNode((int64)Qt::Key_Adiaeresis));
   qt_ns->addConstant("Key_Aring",                new QoreNode((int64)Qt::Key_Aring));
   qt_ns->addConstant("Key_AE",                   new QoreNode((int64)Qt::Key_AE));
   qt_ns->addConstant("Key_Ccedilla",             new QoreNode((int64)Qt::Key_Ccedilla));
   qt_ns->addConstant("Key_Egrave",               new QoreNode((int64)Qt::Key_Egrave));
   qt_ns->addConstant("Key_Eacute",               new QoreNode((int64)Qt::Key_Eacute));
   qt_ns->addConstant("Key_Ecircumflex",          new QoreNode((int64)Qt::Key_Ecircumflex));
   qt_ns->addConstant("Key_Ediaeresis",           new QoreNode((int64)Qt::Key_Ediaeresis));
   qt_ns->addConstant("Key_Igrave",               new QoreNode((int64)Qt::Key_Igrave));
   qt_ns->addConstant("Key_Iacute",               new QoreNode((int64)Qt::Key_Iacute));
   qt_ns->addConstant("Key_Icircumflex",          new QoreNode((int64)Qt::Key_Icircumflex));
   qt_ns->addConstant("Key_Idiaeresis",           new QoreNode((int64)Qt::Key_Idiaeresis));
   qt_ns->addConstant("Key_ETH",                  new QoreNode((int64)Qt::Key_ETH));
   qt_ns->addConstant("Key_Ntilde",               new QoreNode((int64)Qt::Key_Ntilde));
   qt_ns->addConstant("Key_Ograve",               new QoreNode((int64)Qt::Key_Ograve));
   qt_ns->addConstant("Key_Oacute",               new QoreNode((int64)Qt::Key_Oacute));
   qt_ns->addConstant("Key_Ocircumflex",          new QoreNode((int64)Qt::Key_Ocircumflex));
   qt_ns->addConstant("Key_Otilde",               new QoreNode((int64)Qt::Key_Otilde));
   qt_ns->addConstant("Key_Odiaeresis",           new QoreNode((int64)Qt::Key_Odiaeresis));
   qt_ns->addConstant("Key_multiply",             new QoreNode((int64)Qt::Key_multiply));
   qt_ns->addConstant("Key_Ooblique",             new QoreNode((int64)Qt::Key_Ooblique));
   qt_ns->addConstant("Key_Ugrave",               new QoreNode((int64)Qt::Key_Ugrave));
   qt_ns->addConstant("Key_Uacute",               new QoreNode((int64)Qt::Key_Uacute));
   qt_ns->addConstant("Key_Ucircumflex",          new QoreNode((int64)Qt::Key_Ucircumflex));
   qt_ns->addConstant("Key_Udiaeresis",           new QoreNode((int64)Qt::Key_Udiaeresis));
   qt_ns->addConstant("Key_Yacute",               new QoreNode((int64)Qt::Key_Yacute));
   qt_ns->addConstant("Key_THORN",                new QoreNode((int64)Qt::Key_THORN));
   qt_ns->addConstant("Key_ssharp",               new QoreNode((int64)Qt::Key_ssharp));
   qt_ns->addConstant("Key_division",             new QoreNode((int64)Qt::Key_division));
   qt_ns->addConstant("Key_ydiaeresis",           new QoreNode((int64)Qt::Key_ydiaeresis));
   qt_ns->addConstant("Key_AltGr",                new QoreNode((int64)Qt::Key_AltGr));
   qt_ns->addConstant("Key_Multi_key",            new QoreNode((int64)Qt::Key_Multi_key));
   qt_ns->addConstant("Key_Codeinput",            new QoreNode((int64)Qt::Key_Codeinput));
   qt_ns->addConstant("Key_SingleCandidate",      new QoreNode((int64)Qt::Key_SingleCandidate));
   qt_ns->addConstant("Key_MultipleCandidate",    new QoreNode((int64)Qt::Key_MultipleCandidate));
   qt_ns->addConstant("Key_PreviousCandidate",    new QoreNode((int64)Qt::Key_PreviousCandidate));
   qt_ns->addConstant("Key_Mode_switch",          new QoreNode((int64)Qt::Key_Mode_switch));
   qt_ns->addConstant("Key_Kanji",                new QoreNode((int64)Qt::Key_Kanji));
   qt_ns->addConstant("Key_Muhenkan",             new QoreNode((int64)Qt::Key_Muhenkan));
   qt_ns->addConstant("Key_Henkan",               new QoreNode((int64)Qt::Key_Henkan));
   qt_ns->addConstant("Key_Romaji",               new QoreNode((int64)Qt::Key_Romaji));
   qt_ns->addConstant("Key_Hiragana",             new QoreNode((int64)Qt::Key_Hiragana));
   qt_ns->addConstant("Key_Katakana",             new QoreNode((int64)Qt::Key_Katakana));
   qt_ns->addConstant("Key_Hiragana_Katakana",    new QoreNode((int64)Qt::Key_Hiragana_Katakana));
   qt_ns->addConstant("Key_Zenkaku",              new QoreNode((int64)Qt::Key_Zenkaku));
   qt_ns->addConstant("Key_Hankaku",              new QoreNode((int64)Qt::Key_Hankaku));
   qt_ns->addConstant("Key_Zenkaku_Hankaku",      new QoreNode((int64)Qt::Key_Zenkaku_Hankaku));
   qt_ns->addConstant("Key_Touroku",              new QoreNode((int64)Qt::Key_Touroku));
   qt_ns->addConstant("Key_Massyo",               new QoreNode((int64)Qt::Key_Massyo));
   qt_ns->addConstant("Key_Kana_Lock",            new QoreNode((int64)Qt::Key_Kana_Lock));
   qt_ns->addConstant("Key_Kana_Shift",           new QoreNode((int64)Qt::Key_Kana_Shift));
   qt_ns->addConstant("Key_Eisu_Shift",           new QoreNode((int64)Qt::Key_Eisu_Shift));
   qt_ns->addConstant("Key_Eisu_toggle",          new QoreNode((int64)Qt::Key_Eisu_toggle));
   qt_ns->addConstant("Key_Hangul",               new QoreNode((int64)Qt::Key_Hangul));
   qt_ns->addConstant("Key_Hangul_Start",         new QoreNode((int64)Qt::Key_Hangul_Start));
   qt_ns->addConstant("Key_Hangul_End",           new QoreNode((int64)Qt::Key_Hangul_End));
   qt_ns->addConstant("Key_Hangul_Hanja",         new QoreNode((int64)Qt::Key_Hangul_Hanja));
   qt_ns->addConstant("Key_Hangul_Jamo",          new QoreNode((int64)Qt::Key_Hangul_Jamo));
   qt_ns->addConstant("Key_Hangul_Romaja",        new QoreNode((int64)Qt::Key_Hangul_Romaja));
   qt_ns->addConstant("Key_Hangul_Jeonja",        new QoreNode((int64)Qt::Key_Hangul_Jeonja));
   qt_ns->addConstant("Key_Hangul_Banja",         new QoreNode((int64)Qt::Key_Hangul_Banja));
   qt_ns->addConstant("Key_Hangul_PreHanja",      new QoreNode((int64)Qt::Key_Hangul_PreHanja));
   qt_ns->addConstant("Key_Hangul_PostHanja",     new QoreNode((int64)Qt::Key_Hangul_PostHanja));
   qt_ns->addConstant("Key_Hangul_Special",       new QoreNode((int64)Qt::Key_Hangul_Special));
   qt_ns->addConstant("Key_Dead_Grave",           new QoreNode((int64)Qt::Key_Dead_Grave));
   qt_ns->addConstant("Key_Dead_Acute",           new QoreNode((int64)Qt::Key_Dead_Acute));
   qt_ns->addConstant("Key_Dead_Circumflex",      new QoreNode((int64)Qt::Key_Dead_Circumflex));
   qt_ns->addConstant("Key_Dead_Tilde",           new QoreNode((int64)Qt::Key_Dead_Tilde));
   qt_ns->addConstant("Key_Dead_Macron",          new QoreNode((int64)Qt::Key_Dead_Macron));
   qt_ns->addConstant("Key_Dead_Breve",           new QoreNode((int64)Qt::Key_Dead_Breve));
   qt_ns->addConstant("Key_Dead_Abovedot",        new QoreNode((int64)Qt::Key_Dead_Abovedot));
   qt_ns->addConstant("Key_Dead_Diaeresis",       new QoreNode((int64)Qt::Key_Dead_Diaeresis));
   qt_ns->addConstant("Key_Dead_Abovering",       new QoreNode((int64)Qt::Key_Dead_Abovering));
   qt_ns->addConstant("Key_Dead_Doubleacute",     new QoreNode((int64)Qt::Key_Dead_Doubleacute));
   qt_ns->addConstant("Key_Dead_Caron",           new QoreNode((int64)Qt::Key_Dead_Caron));
   qt_ns->addConstant("Key_Dead_Cedilla",         new QoreNode((int64)Qt::Key_Dead_Cedilla));
   qt_ns->addConstant("Key_Dead_Ogonek",          new QoreNode((int64)Qt::Key_Dead_Ogonek));
   qt_ns->addConstant("Key_Dead_Iota",            new QoreNode((int64)Qt::Key_Dead_Iota));
   qt_ns->addConstant("Key_Dead_Voiced_Sound",    new QoreNode((int64)Qt::Key_Dead_Voiced_Sound));
   qt_ns->addConstant("Key_Dead_Semivoiced_Sound", new QoreNode((int64)Qt::Key_Dead_Semivoiced_Sound));
   qt_ns->addConstant("Key_Dead_Belowdot",        new QoreNode((int64)Qt::Key_Dead_Belowdot));
   qt_ns->addConstant("Key_Dead_Hook",            new QoreNode((int64)Qt::Key_Dead_Hook));
   qt_ns->addConstant("Key_Dead_Horn",            new QoreNode((int64)Qt::Key_Dead_Horn));
   qt_ns->addConstant("Key_Back",                 new QoreNode((int64)Qt::Key_Back));
   qt_ns->addConstant("Key_Forward",              new QoreNode((int64)Qt::Key_Forward));
   qt_ns->addConstant("Key_Stop",                 new QoreNode((int64)Qt::Key_Stop));
   qt_ns->addConstant("Key_Refresh",              new QoreNode((int64)Qt::Key_Refresh));
   qt_ns->addConstant("Key_VolumeDown",           new QoreNode((int64)Qt::Key_VolumeDown));
   qt_ns->addConstant("Key_VolumeMute",           new QoreNode((int64)Qt::Key_VolumeMute));
   qt_ns->addConstant("Key_VolumeUp",             new QoreNode((int64)Qt::Key_VolumeUp));
   qt_ns->addConstant("Key_BassBoost",            new QoreNode((int64)Qt::Key_BassBoost));
   qt_ns->addConstant("Key_BassUp",               new QoreNode((int64)Qt::Key_BassUp));
   qt_ns->addConstant("Key_BassDown",             new QoreNode((int64)Qt::Key_BassDown));
   qt_ns->addConstant("Key_TrebleUp",             new QoreNode((int64)Qt::Key_TrebleUp));
   qt_ns->addConstant("Key_TrebleDown",           new QoreNode((int64)Qt::Key_TrebleDown));
   qt_ns->addConstant("Key_MediaPlay",            new QoreNode((int64)Qt::Key_MediaPlay));
   qt_ns->addConstant("Key_MediaStop",            new QoreNode((int64)Qt::Key_MediaStop));
   qt_ns->addConstant("Key_MediaPrevious",        new QoreNode((int64)Qt::Key_MediaPrevious));
   qt_ns->addConstant("Key_MediaNext",            new QoreNode((int64)Qt::Key_MediaNext));
   qt_ns->addConstant("Key_MediaRecord",          new QoreNode((int64)Qt::Key_MediaRecord));
   qt_ns->addConstant("Key_HomePage",             new QoreNode((int64)Qt::Key_HomePage));
   qt_ns->addConstant("Key_Favorites",            new QoreNode((int64)Qt::Key_Favorites));
   qt_ns->addConstant("Key_Search",               new QoreNode((int64)Qt::Key_Search));
   qt_ns->addConstant("Key_Standby",              new QoreNode((int64)Qt::Key_Standby));
   qt_ns->addConstant("Key_OpenUrl",              new QoreNode((int64)Qt::Key_OpenUrl));
   qt_ns->addConstant("Key_LaunchMail",           new QoreNode((int64)Qt::Key_LaunchMail));
   qt_ns->addConstant("Key_LaunchMedia",          new QoreNode((int64)Qt::Key_LaunchMedia));
   qt_ns->addConstant("Key_Launch0",              new QoreNode((int64)Qt::Key_Launch0));
   qt_ns->addConstant("Key_Launch1",              new QoreNode((int64)Qt::Key_Launch1));
   qt_ns->addConstant("Key_Launch2",              new QoreNode((int64)Qt::Key_Launch2));
   qt_ns->addConstant("Key_Launch3",              new QoreNode((int64)Qt::Key_Launch3));
   qt_ns->addConstant("Key_Launch4",              new QoreNode((int64)Qt::Key_Launch4));
   qt_ns->addConstant("Key_Launch5",              new QoreNode((int64)Qt::Key_Launch5));
   qt_ns->addConstant("Key_Launch6",              new QoreNode((int64)Qt::Key_Launch6));
   qt_ns->addConstant("Key_Launch7",              new QoreNode((int64)Qt::Key_Launch7));
   qt_ns->addConstant("Key_Launch8",              new QoreNode((int64)Qt::Key_Launch8));
   qt_ns->addConstant("Key_Launch9",              new QoreNode((int64)Qt::Key_Launch9));
   qt_ns->addConstant("Key_LaunchA",              new QoreNode((int64)Qt::Key_LaunchA));
   qt_ns->addConstant("Key_LaunchB",              new QoreNode((int64)Qt::Key_LaunchB));
   qt_ns->addConstant("Key_LaunchC",              new QoreNode((int64)Qt::Key_LaunchC));
   qt_ns->addConstant("Key_LaunchD",              new QoreNode((int64)Qt::Key_LaunchD));
   qt_ns->addConstant("Key_LaunchE",              new QoreNode((int64)Qt::Key_LaunchE));
   qt_ns->addConstant("Key_LaunchF",              new QoreNode((int64)Qt::Key_LaunchF));
   qt_ns->addConstant("Key_MediaLast",            new QoreNode((int64)Qt::Key_MediaLast));
   qt_ns->addConstant("Key_Select",               new QoreNode((int64)Qt::Key_Select));
   qt_ns->addConstant("Key_Yes",                  new QoreNode((int64)Qt::Key_Yes));
   qt_ns->addConstant("Key_No",                   new QoreNode((int64)Qt::Key_No));
   qt_ns->addConstant("Key_Cancel",               new QoreNode((int64)Qt::Key_Cancel));
   qt_ns->addConstant("Key_Printer",              new QoreNode((int64)Qt::Key_Printer));
   qt_ns->addConstant("Key_Execute",              new QoreNode((int64)Qt::Key_Execute));
   qt_ns->addConstant("Key_Sleep",                new QoreNode((int64)Qt::Key_Sleep));
   qt_ns->addConstant("Key_Play",                 new QoreNode((int64)Qt::Key_Play));
   qt_ns->addConstant("Key_Zoom",                 new QoreNode((int64)Qt::Key_Zoom));
   qt_ns->addConstant("Key_Context1",             new QoreNode((int64)Qt::Key_Context1));
   qt_ns->addConstant("Key_Context2",             new QoreNode((int64)Qt::Key_Context2));
   qt_ns->addConstant("Key_Context3",             new QoreNode((int64)Qt::Key_Context3));
   qt_ns->addConstant("Key_Context4",             new QoreNode((int64)Qt::Key_Context4));
   qt_ns->addConstant("Key_Call",                 new QoreNode((int64)Qt::Key_Call));
   qt_ns->addConstant("Key_Hangup",               new QoreNode((int64)Qt::Key_Hangup));
   qt_ns->addConstant("Key_Flip",                 new QoreNode((int64)Qt::Key_Flip));
   qt_ns->addConstant("Key_unknown",              new QoreNode((int64)Qt::Key_unknown));

   // MatchFlag enum
   qt_ns->addConstant("MatchExactly",             new QoreNode((int64)Qt::MatchExactly));
   qt_ns->addConstant("MatchContains",            new QoreNode((int64)Qt::MatchContains));
   qt_ns->addConstant("MatchStartsWith",          new QoreNode((int64)Qt::MatchStartsWith));
   qt_ns->addConstant("MatchEndsWith",            new QoreNode((int64)Qt::MatchEndsWith));
   qt_ns->addConstant("MatchRegExp",              new QoreNode((int64)Qt::MatchRegExp));
   qt_ns->addConstant("MatchWildcard",            new QoreNode((int64)Qt::MatchWildcard));
   qt_ns->addConstant("MatchFixedString",         new QoreNode((int64)Qt::MatchFixedString));
   qt_ns->addConstant("MatchCaseSensitive",       new QoreNode((int64)Qt::MatchCaseSensitive));
   qt_ns->addConstant("MatchWrap",                new QoreNode((int64)Qt::MatchWrap));
   qt_ns->addConstant("MatchRecursive",           new QoreNode((int64)Qt::MatchRecursive));

   // ItemDataRole enum
   qt_ns->addConstant("DisplayRole",              new QoreNode((int64)Qt::DisplayRole));
   qt_ns->addConstant("DecorationRole",           new QoreNode((int64)Qt::DecorationRole));
   qt_ns->addConstant("EditRole",                 new QoreNode((int64)Qt::EditRole));
   qt_ns->addConstant("ToolTipRole",              new QoreNode((int64)Qt::ToolTipRole));
   qt_ns->addConstant("StatusTipRole",            new QoreNode((int64)Qt::StatusTipRole));
   qt_ns->addConstant("WhatsThisRole",            new QoreNode((int64)Qt::WhatsThisRole));
   qt_ns->addConstant("FontRole",                 new QoreNode((int64)Qt::FontRole));
   qt_ns->addConstant("TextAlignmentRole",        new QoreNode((int64)Qt::TextAlignmentRole));
   qt_ns->addConstant("BackgroundColorRole",      new QoreNode((int64)Qt::BackgroundColorRole));
   qt_ns->addConstant("BackgroundRole",           new QoreNode((int64)Qt::BackgroundRole));
   qt_ns->addConstant("TextColorRole",            new QoreNode((int64)Qt::TextColorRole));
   qt_ns->addConstant("ForegroundRole",           new QoreNode((int64)Qt::ForegroundRole));
   qt_ns->addConstant("CheckStateRole",           new QoreNode((int64)Qt::CheckStateRole));
   qt_ns->addConstant("AccessibleTextRole",       new QoreNode((int64)Qt::AccessibleTextRole));
   qt_ns->addConstant("AccessibleDescriptionRole", new QoreNode((int64)Qt::AccessibleDescriptionRole));
   qt_ns->addConstant("SizeHintRole",             new QoreNode((int64)Qt::SizeHintRole));
   qt_ns->addConstant("UserRole",                 new QoreNode((int64)Qt::UserRole));

   // ItemFlag enum
   qt_ns->addConstant("ItemIsSelectable",         new QoreNode((int64)Qt::ItemIsSelectable));
   qt_ns->addConstant("ItemIsEditable",           new QoreNode((int64)Qt::ItemIsEditable));
   qt_ns->addConstant("ItemIsDragEnabled",        new QoreNode((int64)Qt::ItemIsDragEnabled));
   qt_ns->addConstant("ItemIsDropEnabled",        new QoreNode((int64)Qt::ItemIsDropEnabled));
   qt_ns->addConstant("ItemIsUserCheckable",      new QoreNode((int64)Qt::ItemIsUserCheckable));
   qt_ns->addConstant("ItemIsEnabled",            new QoreNode((int64)Qt::ItemIsEnabled));
   qt_ns->addConstant("ItemIsTristate",           new QoreNode((int64)Qt::ItemIsTristate));
	
   // AspectRatioMode enum
   qt_ns->addConstant("IgnoreAspectRatio",        new QoreNode((int64)Qt::IgnoreAspectRatio));
   qt_ns->addConstant("KeepAspectRatio",          new QoreNode((int64)Qt::KeepAspectRatio));
   qt_ns->addConstant("KeepAspectRatioByExpanding", new QoreNode((int64)Qt::KeepAspectRatioByExpanding));

   // TextFormat enum
   qt_ns->addConstant("PlainText",                new QoreNode((int64)Qt::PlainText));
   qt_ns->addConstant("RichText",                 new QoreNode((int64)Qt::RichText));
   qt_ns->addConstant("AutoText",                 new QoreNode((int64)Qt::AutoText));
   qt_ns->addConstant("LogText",                  new QoreNode((int64)Qt::LogText));

   // CursorShape enum
   qt_ns->addConstant("ArrowCursor",              new QoreNode((int64)Qt::ArrowCursor));
   qt_ns->addConstant("UpArrowCursor",            new QoreNode((int64)Qt::UpArrowCursor));
   qt_ns->addConstant("CrossCursor",              new QoreNode((int64)Qt::CrossCursor));
   qt_ns->addConstant("WaitCursor",               new QoreNode((int64)Qt::WaitCursor));
   qt_ns->addConstant("IBeamCursor",              new QoreNode((int64)Qt::IBeamCursor));
   qt_ns->addConstant("SizeVerCursor",            new QoreNode((int64)Qt::SizeVerCursor));
   qt_ns->addConstant("SizeHorCursor",            new QoreNode((int64)Qt::SizeHorCursor));
   qt_ns->addConstant("SizeBDiagCursor",          new QoreNode((int64)Qt::SizeBDiagCursor));
   qt_ns->addConstant("SizeFDiagCursor",          new QoreNode((int64)Qt::SizeFDiagCursor));
   qt_ns->addConstant("SizeAllCursor",            new QoreNode((int64)Qt::SizeAllCursor));
   qt_ns->addConstant("BlankCursor",              new QoreNode((int64)Qt::BlankCursor));
   qt_ns->addConstant("SplitVCursor",             new QoreNode((int64)Qt::SplitVCursor));
   qt_ns->addConstant("SplitHCursor",             new QoreNode((int64)Qt::SplitHCursor));
   qt_ns->addConstant("PointingHandCursor",       new QoreNode((int64)Qt::PointingHandCursor));
   qt_ns->addConstant("ForbiddenCursor",          new QoreNode((int64)Qt::ForbiddenCursor));
   qt_ns->addConstant("WhatsThisCursor",          new QoreNode((int64)Qt::WhatsThisCursor));
   qt_ns->addConstant("BusyCursor",               new QoreNode((int64)Qt::BusyCursor));
   qt_ns->addConstant("OpenHandCursor",           new QoreNode((int64)Qt::OpenHandCursor));
   qt_ns->addConstant("ClosedHandCursor",         new QoreNode((int64)Qt::ClosedHandCursor));
   qt_ns->addConstant("LastCursor",               new QoreNode((int64)Qt::LastCursor));
   qt_ns->addConstant("BitmapCursor",             new QoreNode((int64)Qt::BitmapCursor));
   qt_ns->addConstant("CustomCursor",             new QoreNode((int64)Qt::CustomCursor));

   // AnchorAttribute enum
   qt_ns->addConstant("AnchorName",               new QoreNode((int64)Qt::AnchorName));
   qt_ns->addConstant("AnchorHref",               new QoreNode((int64)Qt::AnchorHref));

   // DockWidgetArea enum
   qt_ns->addConstant("LeftDockWidgetArea",       new QoreNode((int64)Qt::LeftDockWidgetArea));
   qt_ns->addConstant("RightDockWidgetArea",      new QoreNode((int64)Qt::RightDockWidgetArea));
   qt_ns->addConstant("TopDockWidgetArea",        new QoreNode((int64)Qt::TopDockWidgetArea));
   qt_ns->addConstant("BottomDockWidgetArea",     new QoreNode((int64)Qt::BottomDockWidgetArea));
   qt_ns->addConstant("DockWidgetArea_Mask",      new QoreNode((int64)Qt::DockWidgetArea_Mask));
   qt_ns->addConstant("AllDockWidgetAreas",       new QoreNode((int64)Qt::AllDockWidgetAreas));
   qt_ns->addConstant("NoDockWidgetArea",         new QoreNode((int64)Qt::NoDockWidgetArea));

   // DockWidgetAreaSizes enum
   qt_ns->addConstant("NDockWidgetAreas",         new QoreNode((int64)Qt::NDockWidgetAreas));

   // ToolBarArea enum
   qt_ns->addConstant("LeftToolBarArea",          new QoreNode((int64)Qt::LeftToolBarArea));
   qt_ns->addConstant("RightToolBarArea",         new QoreNode((int64)Qt::RightToolBarArea));
   qt_ns->addConstant("TopToolBarArea",           new QoreNode((int64)Qt::TopToolBarArea));
   qt_ns->addConstant("BottomToolBarArea",        new QoreNode((int64)Qt::BottomToolBarArea));
   qt_ns->addConstant("ToolBarArea_Mask",         new QoreNode((int64)Qt::ToolBarArea_Mask));
   qt_ns->addConstant("AllToolBarAreas",          new QoreNode((int64)Qt::AllToolBarAreas));
   qt_ns->addConstant("NoToolBarArea",            new QoreNode((int64)Qt::NoToolBarArea));

   // ToolBarSizes enum
   qt_ns->addConstant("NToolBarAreas",            new QoreNode((int64)Qt::NToolBarAreas));

   // PenCapStyle enum
   qt_ns->addConstant("FlatCap",                  new QoreNode((int64)Qt::FlatCap));
   qt_ns->addConstant("SquareCap",                new QoreNode((int64)Qt::SquareCap));
   qt_ns->addConstant("RoundCap",                 new QoreNode((int64)Qt::RoundCap));
   qt_ns->addConstant("MPenCapStyle",             new QoreNode((int64)Qt::MPenCapStyle));

   // PenJoinStyle enum
   qt_ns->addConstant("MiterJoin",                new QoreNode((int64)Qt::MiterJoin));
   qt_ns->addConstant("BevelJoin",                new QoreNode((int64)Qt::BevelJoin));
   qt_ns->addConstant("RoundJoin",                new QoreNode((int64)Qt::RoundJoin));
   qt_ns->addConstant("SvgMiterJoin",             new QoreNode((int64)Qt::SvgMiterJoin));
   qt_ns->addConstant("MPenJoinStyle",            new QoreNode((int64)Qt::MPenJoinStyle));

   // WidgetAttribute enum
   qt_ns->addConstant("WA_Disabled",              new QoreNode((int64)Qt::WA_Disabled));
   qt_ns->addConstant("WA_UnderMouse",            new QoreNode((int64)Qt::WA_UnderMouse));
   qt_ns->addConstant("WA_MouseTracking",         new QoreNode((int64)Qt::WA_MouseTracking));
   qt_ns->addConstant("WA_ContentsPropagated",    new QoreNode((int64)Qt::WA_ContentsPropagated));
   qt_ns->addConstant("WA_OpaquePaintEvent",      new QoreNode((int64)Qt::WA_OpaquePaintEvent));
   qt_ns->addConstant("WA_NoBackground",          new QoreNode((int64)Qt::WA_NoBackground));
   qt_ns->addConstant("WA_StaticContents",        new QoreNode((int64)Qt::WA_StaticContents));
   qt_ns->addConstant("WA_LaidOut",               new QoreNode((int64)Qt::WA_LaidOut));
   qt_ns->addConstant("WA_PaintOnScreen",         new QoreNode((int64)Qt::WA_PaintOnScreen));
   qt_ns->addConstant("WA_NoSystemBackground",    new QoreNode((int64)Qt::WA_NoSystemBackground));
   qt_ns->addConstant("WA_UpdatesDisabled",       new QoreNode((int64)Qt::WA_UpdatesDisabled));
   qt_ns->addConstant("WA_Mapped",                new QoreNode((int64)Qt::WA_Mapped));
   qt_ns->addConstant("WA_MacNoClickThrough",     new QoreNode((int64)Qt::WA_MacNoClickThrough));
   qt_ns->addConstant("WA_PaintOutsidePaintEvent", new QoreNode((int64)Qt::WA_PaintOutsidePaintEvent));
   qt_ns->addConstant("WA_InputMethodEnabled",    new QoreNode((int64)Qt::WA_InputMethodEnabled));
   qt_ns->addConstant("WA_WState_Visible",        new QoreNode((int64)Qt::WA_WState_Visible));
   qt_ns->addConstant("WA_WState_Hidden",         new QoreNode((int64)Qt::WA_WState_Hidden));
   qt_ns->addConstant("WA_ForceDisabled",         new QoreNode((int64)Qt::WA_ForceDisabled));
   qt_ns->addConstant("WA_KeyCompression",        new QoreNode((int64)Qt::WA_KeyCompression));
   qt_ns->addConstant("WA_PendingMoveEvent",      new QoreNode((int64)Qt::WA_PendingMoveEvent));
   qt_ns->addConstant("WA_PendingResizeEvent",    new QoreNode((int64)Qt::WA_PendingResizeEvent));
   qt_ns->addConstant("WA_SetPalette",            new QoreNode((int64)Qt::WA_SetPalette));
   qt_ns->addConstant("WA_SetFont",               new QoreNode((int64)Qt::WA_SetFont));
   qt_ns->addConstant("WA_SetCursor",             new QoreNode((int64)Qt::WA_SetCursor));
   qt_ns->addConstant("WA_NoChildEventsFromChildren", new QoreNode((int64)Qt::WA_NoChildEventsFromChildren));
   qt_ns->addConstant("WA_WindowModified",        new QoreNode((int64)Qt::WA_WindowModified));
   qt_ns->addConstant("WA_Resized",               new QoreNode((int64)Qt::WA_Resized));
   qt_ns->addConstant("WA_Moved",                 new QoreNode((int64)Qt::WA_Moved));
   qt_ns->addConstant("WA_PendingUpdate",         new QoreNode((int64)Qt::WA_PendingUpdate));
   qt_ns->addConstant("WA_InvalidSize",           new QoreNode((int64)Qt::WA_InvalidSize));
   qt_ns->addConstant("WA_MacBrushedMetal",       new QoreNode((int64)Qt::WA_MacBrushedMetal));
   qt_ns->addConstant("WA_MacMetalStyle",         new QoreNode((int64)Qt::WA_MacMetalStyle));
   qt_ns->addConstant("WA_CustomWhatsThis",       new QoreNode((int64)Qt::WA_CustomWhatsThis));
   qt_ns->addConstant("WA_LayoutOnEntireRect",    new QoreNode((int64)Qt::WA_LayoutOnEntireRect));
   qt_ns->addConstant("WA_OutsideWSRange",        new QoreNode((int64)Qt::WA_OutsideWSRange));
   qt_ns->addConstant("WA_GrabbedShortcut",       new QoreNode((int64)Qt::WA_GrabbedShortcut));
   qt_ns->addConstant("WA_TransparentForMouseEvents", new QoreNode((int64)Qt::WA_TransparentForMouseEvents));
   qt_ns->addConstant("WA_PaintUnclipped",        new QoreNode((int64)Qt::WA_PaintUnclipped));
   qt_ns->addConstant("WA_SetWindowIcon",         new QoreNode((int64)Qt::WA_SetWindowIcon));
   qt_ns->addConstant("WA_NoMouseReplay",         new QoreNode((int64)Qt::WA_NoMouseReplay));
   qt_ns->addConstant("WA_DeleteOnClose",         new QoreNode((int64)Qt::WA_DeleteOnClose));
   qt_ns->addConstant("WA_RightToLeft",           new QoreNode((int64)Qt::WA_RightToLeft));
   qt_ns->addConstant("WA_SetLayoutDirection",    new QoreNode((int64)Qt::WA_SetLayoutDirection));
   qt_ns->addConstant("WA_NoChildEventsForParent", new QoreNode((int64)Qt::WA_NoChildEventsForParent));
   qt_ns->addConstant("WA_ForceUpdatesDisabled",  new QoreNode((int64)Qt::WA_ForceUpdatesDisabled));
   qt_ns->addConstant("WA_WState_Created",        new QoreNode((int64)Qt::WA_WState_Created));
   qt_ns->addConstant("WA_WState_CompressKeys",   new QoreNode((int64)Qt::WA_WState_CompressKeys));
   qt_ns->addConstant("WA_WState_InPaintEvent",   new QoreNode((int64)Qt::WA_WState_InPaintEvent));
   qt_ns->addConstant("WA_WState_Reparented",     new QoreNode((int64)Qt::WA_WState_Reparented));
   qt_ns->addConstant("WA_WState_ConfigPending",  new QoreNode((int64)Qt::WA_WState_ConfigPending));
   qt_ns->addConstant("WA_WState_Polished",       new QoreNode((int64)Qt::WA_WState_Polished));
   qt_ns->addConstant("WA_WState_DND",            new QoreNode((int64)Qt::WA_WState_DND));
   qt_ns->addConstant("WA_WState_OwnSizePolicy",  new QoreNode((int64)Qt::WA_WState_OwnSizePolicy));
   qt_ns->addConstant("WA_WState_ExplicitShowHide", new QoreNode((int64)Qt::WA_WState_ExplicitShowHide));
   qt_ns->addConstant("WA_ShowModal",             new QoreNode((int64)Qt::WA_ShowModal));
   qt_ns->addConstant("WA_MouseNoMask",           new QoreNode((int64)Qt::WA_MouseNoMask));
   qt_ns->addConstant("WA_GroupLeader",           new QoreNode((int64)Qt::WA_GroupLeader));
   qt_ns->addConstant("WA_NoMousePropagation",    new QoreNode((int64)Qt::WA_NoMousePropagation));
   qt_ns->addConstant("WA_Hover",                 new QoreNode((int64)Qt::WA_Hover));
   qt_ns->addConstant("WA_InputMethodTransparent", new QoreNode((int64)Qt::WA_InputMethodTransparent));
   qt_ns->addConstant("WA_QuitOnClose",           new QoreNode((int64)Qt::WA_QuitOnClose));
   qt_ns->addConstant("WA_KeyboardFocusChange",   new QoreNode((int64)Qt::WA_KeyboardFocusChange));
   qt_ns->addConstant("WA_AcceptDrops",           new QoreNode((int64)Qt::WA_AcceptDrops));
   qt_ns->addConstant("WA_DropSiteRegistered",    new QoreNode((int64)Qt::WA_DropSiteRegistered));
   qt_ns->addConstant("WA_ForceAcceptDrops",      new QoreNode((int64)Qt::WA_ForceAcceptDrops));
   qt_ns->addConstant("WA_WindowPropagation",     new QoreNode((int64)Qt::WA_WindowPropagation));
   qt_ns->addConstant("WA_NoX11EventCompression", new QoreNode((int64)Qt::WA_NoX11EventCompression));
   qt_ns->addConstant("WA_TintedBackground",      new QoreNode((int64)Qt::WA_TintedBackground));
   qt_ns->addConstant("WA_X11OpenGLOverlay",      new QoreNode((int64)Qt::WA_X11OpenGLOverlay));
   qt_ns->addConstant("WA_AlwaysShowToolTips",    new QoreNode((int64)Qt::WA_AlwaysShowToolTips));
   qt_ns->addConstant("WA_MacOpaqueSizeGrip",     new QoreNode((int64)Qt::WA_MacOpaqueSizeGrip));
   qt_ns->addConstant("WA_SetStyle",              new QoreNode((int64)Qt::WA_SetStyle));
   qt_ns->addConstant("WA_SetLocale",             new QoreNode((int64)Qt::WA_SetLocale));
   qt_ns->addConstant("WA_MacShowFocusRect",      new QoreNode((int64)Qt::WA_MacShowFocusRect));
   qt_ns->addConstant("WA_MacNormalSize",         new QoreNode((int64)Qt::WA_MacNormalSize));
   qt_ns->addConstant("WA_MacSmallSize",          new QoreNode((int64)Qt::WA_MacSmallSize));
   qt_ns->addConstant("WA_MacMiniSize",           new QoreNode((int64)Qt::WA_MacMiniSize));
   qt_ns->addConstant("WA_LayoutUsesWidgetRect",  new QoreNode((int64)Qt::WA_LayoutUsesWidgetRect));
   qt_ns->addConstant("WA_StyledBackground",      new QoreNode((int64)Qt::WA_StyledBackground));
   qt_ns->addConstant("WA_MSWindowsUseDirect3D",  new QoreNode((int64)Qt::WA_MSWindowsUseDirect3D));
   qt_ns->addConstant("WA_CanHostQMdiSubWindowTitleBar", new QoreNode((int64)Qt::WA_CanHostQMdiSubWindowTitleBar));
   qt_ns->addConstant("WA_MacAlwaysShowToolWindow", new QoreNode((int64)Qt::WA_MacAlwaysShowToolWindow));
   qt_ns->addConstant("WA_StyleSheet",            new QoreNode((int64)Qt::WA_StyleSheet));
   qt_ns->addConstant("WA_AttributeCount",        new QoreNode((int64)Qt::WA_AttributeCount));
   
   // WindowType enum
   qt_ns->addConstant("Widget",                   new QoreNode((int64)Qt::Widget));
   qt_ns->addConstant("Window",                   new QoreNode((int64)Qt::Window));
   qt_ns->addConstant("Dialog",                   new QoreNode((int64)Qt::Dialog));
   qt_ns->addConstant("Sheet",                    new QoreNode((int64)Qt::Sheet));
   qt_ns->addConstant("Drawer",                   new QoreNode((int64)Qt::Drawer));
   qt_ns->addConstant("Popup",                    new QoreNode((int64)Qt::Popup));
   qt_ns->addConstant("Tool",                     new QoreNode((int64)Qt::Tool));
   qt_ns->addConstant("ToolTip",                  new QoreNode((int64)Qt::ToolTip));
   qt_ns->addConstant("SplashScreen",             new QoreNode((int64)Qt::SplashScreen));
   qt_ns->addConstant("Desktop",                  new QoreNode((int64)Qt::Desktop));
   qt_ns->addConstant("SubWindow",                new QoreNode((int64)Qt::SubWindow));
   qt_ns->addConstant("WindowType_Mask",          new QoreNode((int64)Qt::WindowType_Mask));
   qt_ns->addConstant("MSWindowsFixedSizeDialogHint", new QoreNode((int64)Qt::MSWindowsFixedSizeDialogHint));
   qt_ns->addConstant("MSWindowsOwnDC",           new QoreNode((int64)Qt::MSWindowsOwnDC));
   qt_ns->addConstant("X11BypassWindowManagerHint", new QoreNode((int64)Qt::X11BypassWindowManagerHint));
   qt_ns->addConstant("FramelessWindowHint",      new QoreNode((int64)Qt::FramelessWindowHint));
   qt_ns->addConstant("WindowTitleHint",          new QoreNode((int64)Qt::WindowTitleHint));
   qt_ns->addConstant("WindowSystemMenuHint",     new QoreNode((int64)Qt::WindowSystemMenuHint));
   qt_ns->addConstant("WindowMinimizeButtonHint", new QoreNode((int64)Qt::WindowMinimizeButtonHint));
   qt_ns->addConstant("WindowMaximizeButtonHint", new QoreNode((int64)Qt::WindowMaximizeButtonHint));
   qt_ns->addConstant("WindowMinMaxButtonsHint",  new QoreNode((int64)Qt::WindowMinMaxButtonsHint));
   qt_ns->addConstant("WindowContextHelpButtonHint", new QoreNode((int64)Qt::WindowContextHelpButtonHint));
   qt_ns->addConstant("WindowShadeButtonHint",    new QoreNode((int64)Qt::WindowShadeButtonHint));
   qt_ns->addConstant("WindowStaysOnTopHint",     new QoreNode((int64)Qt::WindowStaysOnTopHint));
   qt_ns->addConstant("CustomizeWindowHint",      new QoreNode((int64)Qt::CustomizeWindowHint));

   // FocusPolicy enum
   qt_ns->addConstant("NoFocus",                  new QoreNode((int64)Qt::NoFocus));
   qt_ns->addConstant("TabFocus",                 new QoreNode((int64)Qt::TabFocus));
   qt_ns->addConstant("ClickFocus",               new QoreNode((int64)Qt::ClickFocus));
   qt_ns->addConstant("StrongFocus",              new QoreNode((int64)Qt::StrongFocus));
   qt_ns->addConstant("WheelFocus",               new QoreNode((int64)Qt::WheelFocus));

   // ConnectionType enum
   qt_ns->addConstant("AutoConnection",           new QoreNode((int64)Qt::AutoConnection));
   qt_ns->addConstant("DirectConnection",         new QoreNode((int64)Qt::DirectConnection));
   qt_ns->addConstant("QueuedConnection",         new QoreNode((int64)Qt::QueuedConnection));
   qt_ns->addConstant("AutoCompatConnection",     new QoreNode((int64)Qt::AutoCompatConnection));
   qt_ns->addConstant("BlockingQueuedConnection", new QoreNode((int64)Qt::BlockingQueuedConnection));

   // DateFormat enum
   qt_ns->addConstant("TextDate",                 new QoreNode((int64)Qt::TextDate));
   qt_ns->addConstant("ISODate",                  new QoreNode((int64)Qt::ISODate));
   qt_ns->addConstant("SystemLocaleDate",         new QoreNode((int64)Qt::SystemLocaleDate));
   qt_ns->addConstant("LocalDate",                new QoreNode((int64)Qt::LocalDate));
   qt_ns->addConstant("LocaleDate",               new QoreNode((int64)Qt::LocaleDate));

   // TimeSpec enum
   qt_ns->addConstant("LocalTime",                new QoreNode((int64)Qt::LocalTime));
   qt_ns->addConstant("UTC",                      new QoreNode((int64)Qt::UTC));

   // ScrollBarPolicy enum
   qt_ns->addConstant("ScrollBarAsNeeded",        new QoreNode((int64)Qt::ScrollBarAsNeeded));
   qt_ns->addConstant("ScrollBarAlwaysOff",       new QoreNode((int64)Qt::ScrollBarAlwaysOff));
   qt_ns->addConstant("ScrollBarAlwaysOn",        new QoreNode((int64)Qt::ScrollBarAlwaysOn));

   // CaseSensitivity enum
   qt_ns->addConstant("CaseInsensitive",          new QoreNode((int64)Qt::CaseInsensitive));
   qt_ns->addConstant("CaseSensitive",            new QoreNode((int64)Qt::CaseSensitive));

   // Corner enum
   qt_ns->addConstant("TopLeftCorner",            new QoreNode((int64)Qt::TopLeftCorner));
   qt_ns->addConstant("TopRightCorner",           new QoreNode((int64)Qt::TopRightCorner));
   qt_ns->addConstant("BottomLeftCorner",         new QoreNode((int64)Qt::BottomLeftCorner));
   qt_ns->addConstant("BottomRightCorner",        new QoreNode((int64)Qt::BottomRightCorner));

   // ShortcutContext enum
   qt_ns->addConstant("WidgetShortcut",           new QoreNode((int64)Qt::WidgetShortcut));
   qt_ns->addConstant("WindowShortcut",           new QoreNode((int64)Qt::WindowShortcut));
   qt_ns->addConstant("ApplicationShortcut",      new QoreNode((int64)Qt::ApplicationShortcut));

   // FillRule enum
   qt_ns->addConstant("OddEvenFill",              new QoreNode((int64)Qt::OddEvenFill));
   qt_ns->addConstant("WindingFill",              new QoreNode((int64)Qt::WindingFill));

   // MaskMode enum
   qt_ns->addConstant("MaskInColor",              new QoreNode((int64)Qt::MaskInColor));
   qt_ns->addConstant("MaskOutColor",             new QoreNode((int64)Qt::MaskOutColor));

   // ClipOperation enum
   qt_ns->addConstant("NoClip",                   new QoreNode((int64)Qt::NoClip));
   qt_ns->addConstant("ReplaceClip",              new QoreNode((int64)Qt::ReplaceClip));
   qt_ns->addConstant("IntersectClip",            new QoreNode((int64)Qt::IntersectClip));
   qt_ns->addConstant("UniteClip",                new QoreNode((int64)Qt::UniteClip));

   // LayoutDirection enum
   qt_ns->addConstant("LeftToRight",              new QoreNode((int64)Qt::LeftToRight));
   qt_ns->addConstant("RightToLeft",              new QoreNode((int64)Qt::RightToLeft));

   // ItemSelectionMode
   qt_ns->addConstant("ContainsItemShape",        new QoreNode((int64)Qt::ContainsItemShape));
   qt_ns->addConstant("IntersectsItemShape",      new QoreNode((int64)Qt::IntersectsItemShape));
   qt_ns->addConstant("ContainsItemBoundingRect", new QoreNode((int64)Qt::ContainsItemBoundingRect));
   qt_ns->addConstant("IntersectsItemBoundingRect", new QoreNode((int64)Qt::IntersectsItemBoundingRect));

   // TransformationMode enum
   qt_ns->addConstant("FastTransformation",       new QoreNode((int64)Qt::FastTransformation));
   qt_ns->addConstant("SmoothTransformation",     new QoreNode((int64)Qt::SmoothTransformation));

   // Axis enum
   qt_ns->addConstant("XAxis",                    new QoreNode((int64)Qt::XAxis));
   qt_ns->addConstant("YAxis",                    new QoreNode((int64)Qt::YAxis));
   qt_ns->addConstant("ZAxis",                    new QoreNode((int64)Qt::ZAxis));

}

static class QoreString *qt_module_init()
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
