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
#include "QC_QActionEvent.h"
#include "QC_QCloseEvent.h"
#include "QC_QContextMenuEvent.h"
#include "QC_QDropEvent.h"
#include "QC_QMimeData.h"
#include "QC_QDragMoveEvent.h"
#include "QC_QDragEnterEvent.h"
#include "QC_QDragLeaveEvent.h"
#include "QC_QFocusEvent.h"
#include "QC_QHideEvent.h"
#include "QC_QInputMethodEvent.h"
#include "QC_QShowEvent.h"
#include "QC_QTabletEvent.h"
#include "QC_QWheelEvent.h"
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
#include "QC_QMotifStyle.h"
#include "QC_QCDEStyle.h"
#include "QC_QWindowsStyle.h"
#include "QC_QCleanlooksStyle.h"
#include "QC_QPlastiqueStyle.h"
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

#include "qore-qt-events.h"

#include "QT_BrushStyle.h"
#include "QT_PenStyle.h"

#ifdef DARWIN
#include "QC_QMacStyle.h"
#endif

#ifdef WINDOWS
#include "QC_QWindowsXPStyle.h"
#endif

#include "qore-qt.h"

#include <QPalette>
#include <QToolTip>
#include <QStyleFactory>

#include <assert.h>

QoreType *NT_BRUSHSTYLE = 0, *NT_PENSTYLE = 0;

QoreNode *C_Clipboard = 0;

static class QoreString *qt_module_init();
static void qt_module_ns_init(class Namespace *rns, class Namespace *qns);
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
   if (n && n->type == NT_DATE) {
      DateTime *qdt = n->val.date_time;
      dt.setDate(QDate(qdt->getYear(), qdt->getMonth(), qdt->getDay()));
      dt.setTime(QTime(qdt->getHour(), qdt->getMinute(), qdt->getSecond(), qdt->getMillisecond()));
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
	       xsink->raiseException("DATETIME-ERROR", "class '%s' is not derived from QDate or QDateTime", n->val.object->getClass()->getName());
	    else
	       xsink->raiseException("DATETIME-ERROR", "cannot convert type '%s' to QDateTime", n ? n->type->getName() : "NOTHING");
	 }
	 return -1;
      }

      ReferenceHolder<QoreQDateTime> dtHolder(qdt, xsink);
      dt = *(static_cast<QDateTime *>(qdt));
      return 0;
   }

   ReferenceHolder<QoreQDate> dtHolder(qd, xsink);
   dt.setDate(*(static_cast<QDate *>(qd)));
   return 0;
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
   if (n && n->type == NT_STRING) {
      ba.clear();
      ba.append(n->val.String->getBuffer());
      return 0;
   }
   if (!suppress_exception)
      xsink->raiseException("QBYTEARRAY-ERROR", "cannot convert type '%s' to QByteArray", n ? n->type->getName() : "NOTHING");
   return -1;
}

int get_qvariant(const QoreNode *n, QVariant &qva, class ExceptionSink *xsink, bool suppress_exception)
{
   //printd(0, "get_variant() n=%08p %s\n", n, n ? n->type->getName() : "n/a");
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
      if (n->type == NT_STRING) {
	 QVariant n_qv(n->val.String->getBuffer());
	 qva = n_qv;
	 return 0;
      }
      if (n->type == NT_INT) {
	 if (n->val.intval <= 0x7fffffff)
	    qva.setValue((int)n->val.intval);
	 else
	    qva.setValue(n->val.intval);
	 //printd(0, "qvariant integer %d (%d)\n", (int)n->val.intval, qva.toInt());
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
   if (n && n->type == NT_STRING) {
      unsigned int unicode = n->val.String->getUnicodePoint(0, xsink);
      if (*xsink)
	 return -1;
      QChar tmp(unicode);
      c = tmp;
      return 0;
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
   if (n && n->type == NT_STRING) {
      if (n->val.String->getEncoding() == QCS_ISO_8859_1) {
	 str = QString::fromLatin1(n->val.String->getBuffer());
      }
      else if (n->val.String->getEncoding() == QCS_USASCII) {
	 str = QString::fromAscii(n->val.String->getBuffer());
      }
      else {
	 TempEncodingHelper estr(n->val.String, QCS_UTF8, xsink);
	 if (*xsink)
	    return -1;

	 str = QString::fromUtf8(estr->getBuffer());
      }
      return 0;
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

class QoreNode *return_object(QoreClass *qclass, AbstractPrivateData *data)
{
   Object *qore_object = new Object(qclass, getProgram());
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
   Object *obj;

   // try to determine what subclass the QStyle is if possible
   QCleanlooksStyle *qcls = dynamic_cast<QCleanlooksStyle *>(qs);
   if (qcls) {
      qc = QC_QCleanlooksStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQCleanlooksStyle(obj, qcls));
      return new QoreNode(obj);
   }

   QPlastiqueStyle *qps = dynamic_cast<QPlastiqueStyle *>(qs);
   if (qps) {
      qc = QC_QPlastiqueStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQPlastiqueStyle(obj, qps));
      return new QoreNode(obj);
   }

#ifdef WINDOWS
   QWindowsXPStyle *qxps = dynamic_cast<QWindowsXPStyle *>(qs);
   if (qxps) {
      qc = QC_QWindowsXPStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsXPStyle(obj, qxps));
      return new QoreNode(obj);
   }
#endif

#ifdef DARWIN
   QMacStyle *qms = dynamic_cast<QMacStyle *>(qs);
   if (qms) {
      qc = QC_QMacStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMacStyle(obj, qms));
      return new QoreNode(obj);
   }
#endif

   QWindowsStyle *qws = dynamic_cast<QWindowsStyle *>(qs);
   if (qws) {
      qc = QC_QWindowsStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQWindowsStyle(obj, qws));
      return new QoreNode(obj);
   }

   QCDEStyle *qcs = dynamic_cast<QCDEStyle *>(qs);
   if (qcs) {
      qc = QC_QCDEStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQCDEStyle(obj, qcs));
      return new QoreNode(obj);
   }

   QMotifStyle *qmts = dynamic_cast<QMotifStyle *>(qs);
   if (qmts) {
      qc = QC_QMotifStyle;
      obj = new Object(qc, getProgram());
      obj->setPrivate(qc->getID(), new QoreQtQMotifStyle(obj, qmts));
      return new QoreNode(obj);
   }

   // otherwise return a QStyle object
   obj = new Object(QC_QStyle, getProgram());
   obj->setPrivate(CID_QSTYLE, new QoreQtQStyle(obj, qs));
   return new QoreNode(obj);
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
      //case QVariant::RegExp:
         //return return_object(QC_QRegExp, new QoreQRegExp(qv.value<QRegExp>()));
      case QVariant::Region:
         return return_object(QC_QRegion, new QoreQRegion(qv.value<QRegion>()));
      case QVariant::Size:
         return return_object(QC_QSize, new QoreQSize(qv.value<QSize>()));
      //case QVariant::SizeF:
         //return return_object(QC_QSizeF, new QoreQSizeF(qv.value<QSizeF>()));
      //case QVariant::SizePolicy:
         //return return_object(QC_QSizePolicy, new QoreQSizePolicy(qv.value<QSizePolicy>()));
      case QVariant::String:
	 return new QoreNode(new QoreString(qv.toString().toUtf8().data(), QCS_UTF8));
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

// returns a QoreNode tagged as the appropriate QObject subclass
class QoreNode *return_qobject(QObject *o)
{
   if (!o)
      return 0;

   // see if it's an object created in Qore
   QVariant qv_ptr = o->property("qobject");
   Object *qo = reinterpret_cast<Object *>(qv_ptr.toULongLong());
   if (qo) {
      qo->ref();
      return new QoreNode(qo);
   }

   // see what subclass it is
   QWidget *qw = dynamic_cast<QWidget *>(o);
   if (qw) 
      return return_object(QC_QWidget, new QoreQtQWidget(qo, qw));

   // assign as QObject
   return return_object(QC_QObject, new QoreQtQObject(qo, o));
}

static class QoreNode *f_QObject_connect(class QoreNode *params, class ExceptionSink *xsink)
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

   p = test_param(params, NT_STRING, 1);
   if (!p)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing signal string as second argument");
      return 0;
   }
   const char *signal = p->val.String->getBuffer();

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
   p = test_param(params, NT_STRING, 3);
   if (!p)
   {
      xsink->raiseException("QOBJECT-CONNECT-ERROR", "missing slot as fourth argument");
      return 0;
   }
   const char *member = p->val.String->getBuffer();

   /*
   p = get_param(params, 4);
   int conn_type = is_nothing(p) ? Qt::AutoConnection : p->getAsInt();

   bool b = QObject::connect(sender->getQObject(), signal, receiver->getQObject(), member, (enum Qt::ConnectionType)conn_type);
   return new QoreNode(b);
   */
   receiver->connectDynamic(sender, signal, member, xsink);
   return 0;
}

static class QoreNode *f_SLOT(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("SLOT-ERROR", "missing slot name");
      return 0;
   }
   QoreString *str = new QoreString("1");
   str->concat(p->val.String->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return new QoreNode(str);
}

static class QoreNode *f_SIGNAL(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("SIGNAL-ERROR", "missing signal name");
      return 0;
   }
   QoreString *str = new QoreString("2");
   str->concat(p->val.String->getBuffer());
   const char *buf = str->getBuffer();
   int slen = str->strlen();
   if (slen < 3 || buf[slen - 1] != ')')
      str->concat("()");
   return new QoreNode(str);
}

static class QoreNode *f_TR(class QoreNode *params, class ExceptionSink *xsink)
{
   // get slot name
   QoreNode *p = test_param(params, NT_STRING, 0);
   if (!p || !p->val.String->strlen())
   {
      xsink->raiseException("TR-ERROR", "missing string argument to TR()");
      return 0;
   }
   return new QoreNode(new QoreString(QObject::tr(p->val.String->getBuffer()).toUtf8().data(), QCS_UTF8));
}

static class QoreNode *f_QAPP(class QoreNode *params, class ExceptionSink *xsink)
{
   return get_qore_qapp();
}

static class QoreNode *f_qDebug(class QoreNode *params, class ExceptionSink *xsink)
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

static class QoreNode *f_qWarning(class QoreNode *params, class ExceptionSink *xsink)
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

static class QoreNode *f_qCritical(class QoreNode *params, class ExceptionSink *xsink)
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

static class QoreNode *f_qFatal(class QoreNode *params, class ExceptionSink *xsink)
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

static class QoreNode *f_qRound(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   return new QoreNode((int64)qRound(p ? p->getAsFloat() : 0.0));
}

static class QoreNode *f_qsrand(class QoreNode *params, class ExceptionSink *xsink)
{
   class QoreNode *p = get_param(params, 0);
   qsrand(p ? p->getAsInt() : 0);
   return 0;
}

static class QoreNode *f_qrand(class QoreNode *params, class ExceptionSink *xsink)
{
   return new QoreNode((int64)qrand());
}

static QoreNode *f_QToolTip_font(QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qf = new Object(QC_QFont, getProgram());
   QoreQFont *q_qf = new QoreQFont(QToolTip::font());
   o_qf->setPrivate(CID_QFONT, q_qf);
   return new QoreNode(o_qf);
}

//void hideText ()
static QoreNode *f_QToolTip_hideText(QoreNode *params, ExceptionSink *xsink)
{
   QToolTip::hideText();
   return 0;
}

//QPalette palette ()
static QoreNode *f_QToolTip_palette(QoreNode *params, ExceptionSink *xsink)
{
   Object *o_qp = new Object(QC_QPalette, getProgram());
   QoreQPalette *q_qp = new QoreQPalette(QToolTip::palette());
   o_qp->setPrivate(CID_QPALETTE, q_qp);
   return new QoreNode(o_qp);
}

//void setFont ( const QFont & font )
static QoreNode *f_QToolTip_setFont(QoreNode *params, ExceptionSink *xsink)
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
static QoreNode *f_QToolTip_setPalette(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPalette *palette = (p && p->type == NT_OBJECT) ? (QoreQPalette *)p->val.object->getReferencedPrivateData(CID_QPALETTE, xsink) : 0;
   if (!palette) {
      if (!xsink->isException())
         xsink->raiseException("QTOOLTIP-SETPALETTE-PARAM-ERROR", "expecting a QPalette object as first argument to QToolTip_setPalette()");
      return 0;
   }
   ReferenceHolder<QoreQPalette> paletteHolder(palette, xsink);
   QToolTip::setPalette(*(static_cast<QPalette *>(palette)));
   return 0;
}


//void showText ( const QPoint & pos, const QString & text, QWidget * w, const QRect & rect ) 
//void showText ( const QPoint & pos, const QString & text, QWidget * w = 0 )
static class QoreNode *f_QToolTip_showText(class QoreNode *params, class ExceptionSink *xsink)
{
   QoreNode *p = test_param(params, NT_OBJECT, 0);
   QoreQPoint *pos = p ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
	 xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "QToolTip_showText() was expecting a QPoint as the first argument");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   p = test_param(params, NT_STRING, 1);
   if (!p) {
      xsink->raiseException("QTOOLTIP-SHOWTEXT-PARAM-ERROR", "expecting a string as second argument to QToolTip_showText()");
      return 0;
   }
   const char *text = p->val.String->getBuffer();

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
static QoreNode *f_QStyleFactory_create(QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString key;
   if (get_qstring(p, key, xsink))
      return 0;

   return return_qstyle(key, QStyleFactory::create(key), xsink);
}

//QStringList keys ()
static QoreNode *f_QStyleFactory_keys(QoreNode *params, ExceptionSink *xsink)
{
   QStringList strlist_rv = QStyleFactory::keys();
   List *l = new List();
   for (QStringList::iterator i = strlist_rv.begin(), e = strlist_rv.end(); i != e; ++i)
      l->push(new QoreNode(new QoreString((*i).toUtf8().data(), QCS_UTF8)));
   return new QoreNode(l);
}


static class QoreString *qt_module_init()
{
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
   initQApplicationStaticFunctions();
   initQLocaleStaticFunctions();
   initQFontDatabaseStaticFunctions();
   initQMessageBoxStaticFunctions();
   initQPixmapStaticFunctions();
   initQFileDialogStaticFunctions();
   initQDirStaticFunctions();

   addBrushStyleType();
   addPenStyleType();

   return 0;
}

static void qt_module_ns_init(class Namespace *rns, class Namespace *qns)
{
   class Namespace *qt = new Namespace("Qt");

    // the order is sensitive here as child classes need the parent IDs
   class QoreClass *qobject, *qwidget, *qlayout, *qframe, *qboxlayout, *qpaintdevice, *qpixmap, *qabstractslider;
   qt->addSystemClass((qobject = initQObjectClass()));
   qt->addSystemClass(initQApplicationClass(qobject));
   qt->addSystemClass(initQMovieClass(qobject));
   qt->addSystemClass(initQActionClass(qobject));
   qt->addSystemClass(initQActionGroupClass(qobject));
   qt->addSystemClass(initQShortcutClass(qobject));

   qt->addSystemClass((qpaintdevice = initQPaintDeviceClass()));
   qt->addSystemClass(initQPictureClass(qpaintdevice));
   qt->addSystemClass(initQImageClass(qpaintdevice));

   qt->addSystemClass((qpixmap = initQPixmapClass(qpaintdevice)));
   qt->addSystemClass(initQBitmapClass(qpixmap));

   qt->addSystemClass((qwidget = initQWidgetClass(qobject, qpaintdevice)));

   qt->addSystemClass((qabstractslider = initQAbstractSliderClass(qwidget)));
   qt->addSystemClass(initQSliderClass(qabstractslider));

   qt->addSystemClass((qframe = initQFrameClass(qwidget)));
   qt->addSystemClass(initQLCDNumberClass(qframe));
   qt->addSystemClass(initQLabelClass(qframe));

   qt->addSystemClass(initQTimerClass(qobject));

   qt->addSystemClass(initQRectClass());
   qt->addSystemClass(initQRectFClass());
   qt->addSystemClass(initQBrushClass());
   qt->addSystemClass(initQColorClass());
   qt->addSystemClass(initQPaletteClass());
   qt->addSystemClass(initQRegionClass());
   qt->addSystemClass(initQPointClass());
   qt->addSystemClass(initQSizeClass());

   qt->addSystemClass(initQDateTimeClass());
   qt->addSystemClass(initQDateClass());
   qt->addSystemClass(initQTimeClass());

   qt->addSystemClass(initQKeySequenceClass());
   qt->addSystemClass(initQFontClass());
   qt->addSystemClass(initQMatrixClass());

   class QoreClass *qevent, *qinputevent;
   qt->addSystemClass((qevent = initQEventClass()));
   qt->addSystemClass(initQPaintEventClass(qevent));
   qt->addSystemClass(initQMoveEventClass(qevent));
   qt->addSystemClass(initQResizeEventClass(qevent));

   qt->addSystemClass((qinputevent = initQInputEventClass(qevent)));
   qt->addSystemClass(initQKeyEventClass(qinputevent));
   qt->addSystemClass(initQMouseEventClass(qinputevent));

   Namespace *qlayout_ns = new Namespace("QLayout");

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

   qt->addInitialNamespace(qlayout_ns);

   Namespace *qsizepolicy = new Namespace("QSizePolicy");

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

   qt->addInitialNamespace(qsizepolicy);

   Namespace *qicon = new Namespace("QIcon");

   // Mode enum
   qicon->addConstant("Normal",                   new QoreNode((int64)QIcon::Normal));
   qicon->addConstant("Disabled",                 new QoreNode((int64)QIcon::Disabled));
   qicon->addConstant("Active",                   new QoreNode((int64)QIcon::Active));
   qicon->addConstant("Selected",                 new QoreNode((int64)QIcon::Selected));

   // State enum
   qicon->addConstant("On",                       new QoreNode((int64)QIcon::On));
   qicon->addConstant("Off",                      new QoreNode((int64)QIcon::Off));

   qicon->addSystemClass(initQIconClass());
   qt->addInitialNamespace(qicon);

   Namespace *qpalette = new Namespace("QPalette");

   // ColorGroup enum
   qpalette->addConstant("Active",                   new QoreNode((int64)QPalette::Active));
   qpalette->addConstant("Disabled",                 new QoreNode((int64)QPalette::Disabled));
   qpalette->addConstant("Inactive",                 new QoreNode((int64)QPalette::Inactive));
   qpalette->addConstant("NColorGroups",             new QoreNode((int64)QPalette::NColorGroups));
   qpalette->addConstant("Current",                  new QoreNode((int64)QPalette::Current));
   qpalette->addConstant("All",                      new QoreNode((int64)QPalette::All));
   qpalette->addConstant("Normal",                   new QoreNode((int64)QPalette::Normal));

   // ColorRole enum
   qpalette->addConstant("WindowText",               new QoreNode((int64)QPalette::WindowText));
   qpalette->addConstant("Button",                   new QoreNode((int64)QPalette::Button));
   qpalette->addConstant("Light",                    new QoreNode((int64)QPalette::Light));
   qpalette->addConstant("Midlight",                 new QoreNode((int64)QPalette::Midlight));
   qpalette->addConstant("Dark",                     new QoreNode((int64)QPalette::Dark));
   qpalette->addConstant("Mid",                      new QoreNode((int64)QPalette::Mid));
   qpalette->addConstant("Text",                     new QoreNode((int64)QPalette::Text));
   qpalette->addConstant("BrightText",               new QoreNode((int64)QPalette::BrightText));
   qpalette->addConstant("ButtonText",               new QoreNode((int64)QPalette::ButtonText));
   qpalette->addConstant("Base",                     new QoreNode((int64)QPalette::Base));
   qpalette->addConstant("Window",                   new QoreNode((int64)QPalette::Window));
   qpalette->addConstant("Shadow",                   new QoreNode((int64)QPalette::Shadow));
   qpalette->addConstant("Highlight",                new QoreNode((int64)QPalette::Highlight));
   qpalette->addConstant("HighlightedText",          new QoreNode((int64)QPalette::HighlightedText));
   qpalette->addConstant("Link",                     new QoreNode((int64)QPalette::Link));
   qpalette->addConstant("LinkVisited",              new QoreNode((int64)QPalette::LinkVisited));
   qpalette->addConstant("AlternateBase",            new QoreNode((int64)QPalette::AlternateBase));
   qpalette->addConstant("NoRole",                   new QoreNode((int64)QPalette::NoRole));
   qpalette->addConstant("NColorRoles",              new QoreNode((int64)QPalette::NColorRoles));
   qpalette->addConstant("Foreground",               new QoreNode((int64)QPalette::Foreground));
   qpalette->addConstant("Background",               new QoreNode((int64)QPalette::Background));

   qt->addInitialNamespace(qpalette);

   Namespace *qpainter_ns = new Namespace("QPainter");
   
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

   qt->addInitialNamespace(qpainter_ns);

   QoreClass *qabstractbutton, *qtextformat, *qtextframeformat, *qtextcharformat,
      *qstyleoption, *qstyleoptionviewitem, *qabstractitemdelegate,
      *qabstractspinbox, *qdatetimeedit, *qabstractscrollarea, *qdropevent, 
      *qdragmoveevent, *qcombobox, *qstyleoptioncomplex, *qstyle, *qmotifstyle,
      *qwindowsstyle, *qabstractitemview, *qtableview, *qdialog;

   Namespace *qstyle_ns = new Namespace("QStyle");

   qstyle_ns->addSystemClass((qstyle = initQStyleClass(qobject)));
   qstyle_ns->addSystemClass((qmotifstyle = initQMotifStyleClass(qstyle)));
   qstyle_ns->addSystemClass(initQCDEStyleClass(qmotifstyle));
   qstyle_ns->addSystemClass((qwindowsstyle = initQWindowsStyleClass(qstyle)));
   qstyle_ns->addSystemClass(initQCleanlooksStyleClass(qwindowsstyle));
   qstyle_ns->addSystemClass(initQPlastiqueStyleClass(qwindowsstyle));
#ifdef DARWIN
   qstyle_ns->addSystemClass(initQMacStyleClass(qwindowsstyle));
#endif
#ifdef WINDOWS
   qstyle_ns->addSystemClass(initQWindowsXPStyleClass(qwindowsstyle));
#endif
   qstyle_ns->addConstant("PM_ButtonMargin",          new QoreNode((int64)QStyle::PM_ButtonMargin));
   qstyle_ns->addConstant("PM_ButtonDefaultIndicator", new QoreNode((int64)QStyle::PM_ButtonDefaultIndicator));
   qstyle_ns->addConstant("PM_MenuButtonIndicator",   new QoreNode((int64)QStyle::PM_MenuButtonIndicator));
   qstyle_ns->addConstant("PM_ButtonShiftHorizontal", new QoreNode((int64)QStyle::PM_ButtonShiftHorizontal));
   qstyle_ns->addConstant("PM_ButtonShiftVertical",   new QoreNode((int64)QStyle::PM_ButtonShiftVertical));
   qstyle_ns->addConstant("PM_DefaultFrameWidth",     new QoreNode((int64)QStyle::PM_DefaultFrameWidth));
   qstyle_ns->addConstant("PM_SpinBoxFrameWidth",     new QoreNode((int64)QStyle::PM_SpinBoxFrameWidth));
   qstyle_ns->addConstant("PM_ComboBoxFrameWidth",    new QoreNode((int64)QStyle::PM_ComboBoxFrameWidth));
   qstyle_ns->addConstant("PM_MaximumDragDistance",   new QoreNode((int64)QStyle::PM_MaximumDragDistance));
   qstyle_ns->addConstant("PM_ScrollBarExtent",       new QoreNode((int64)QStyle::PM_ScrollBarExtent));
   qstyle_ns->addConstant("PM_ScrollBarSliderMin",    new QoreNode((int64)QStyle::PM_ScrollBarSliderMin));
   qstyle_ns->addConstant("PM_SliderThickness",       new QoreNode((int64)QStyle::PM_SliderThickness));
   qstyle_ns->addConstant("PM_SliderControlThickness", new QoreNode((int64)QStyle::PM_SliderControlThickness));
   qstyle_ns->addConstant("PM_SliderLength",          new QoreNode((int64)QStyle::PM_SliderLength));
   qstyle_ns->addConstant("PM_SliderTickmarkOffset",  new QoreNode((int64)QStyle::PM_SliderTickmarkOffset));
   qstyle_ns->addConstant("PM_SliderSpaceAvailable",  new QoreNode((int64)QStyle::PM_SliderSpaceAvailable));
   qstyle_ns->addConstant("PM_DockWidgetSeparatorExtent", new QoreNode((int64)QStyle::PM_DockWidgetSeparatorExtent));
   qstyle_ns->addConstant("PM_DockWidgetHandleExtent", new QoreNode((int64)QStyle::PM_DockWidgetHandleExtent));
   qstyle_ns->addConstant("PM_DockWidgetFrameWidth",  new QoreNode((int64)QStyle::PM_DockWidgetFrameWidth));
   qstyle_ns->addConstant("PM_TabBarTabOverlap",      new QoreNode((int64)QStyle::PM_TabBarTabOverlap));
   qstyle_ns->addConstant("PM_TabBarTabHSpace",       new QoreNode((int64)QStyle::PM_TabBarTabHSpace));
   qstyle_ns->addConstant("PM_TabBarTabVSpace",       new QoreNode((int64)QStyle::PM_TabBarTabVSpace));
   qstyle_ns->addConstant("PM_TabBarBaseHeight",      new QoreNode((int64)QStyle::PM_TabBarBaseHeight));
   qstyle_ns->addConstant("PM_TabBarBaseOverlap",     new QoreNode((int64)QStyle::PM_TabBarBaseOverlap));
   qstyle_ns->addConstant("PM_ProgressBarChunkWidth", new QoreNode((int64)QStyle::PM_ProgressBarChunkWidth));
   qstyle_ns->addConstant("PM_SplitterWidth",         new QoreNode((int64)QStyle::PM_SplitterWidth));
   qstyle_ns->addConstant("PM_TitleBarHeight",        new QoreNode((int64)QStyle::PM_TitleBarHeight));
   qstyle_ns->addConstant("PM_MenuScrollerHeight",    new QoreNode((int64)QStyle::PM_MenuScrollerHeight));
   qstyle_ns->addConstant("PM_MenuHMargin",           new QoreNode((int64)QStyle::PM_MenuHMargin));
   qstyle_ns->addConstant("PM_MenuVMargin",           new QoreNode((int64)QStyle::PM_MenuVMargin));
   qstyle_ns->addConstant("PM_MenuPanelWidth",        new QoreNode((int64)QStyle::PM_MenuPanelWidth));
   qstyle_ns->addConstant("PM_MenuTearoffHeight",     new QoreNode((int64)QStyle::PM_MenuTearoffHeight));
   qstyle_ns->addConstant("PM_MenuDesktopFrameWidth", new QoreNode((int64)QStyle::PM_MenuDesktopFrameWidth));
   qstyle_ns->addConstant("PM_MenuBarPanelWidth",     new QoreNode((int64)QStyle::PM_MenuBarPanelWidth));
   qstyle_ns->addConstant("PM_MenuBarItemSpacing",    new QoreNode((int64)QStyle::PM_MenuBarItemSpacing));
   qstyle_ns->addConstant("PM_MenuBarVMargin",        new QoreNode((int64)QStyle::PM_MenuBarVMargin));
   qstyle_ns->addConstant("PM_MenuBarHMargin",        new QoreNode((int64)QStyle::PM_MenuBarHMargin));
   qstyle_ns->addConstant("PM_IndicatorWidth",        new QoreNode((int64)QStyle::PM_IndicatorWidth));
   qstyle_ns->addConstant("PM_IndicatorHeight",       new QoreNode((int64)QStyle::PM_IndicatorHeight));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorWidth", new QoreNode((int64)QStyle::PM_ExclusiveIndicatorWidth));
   qstyle_ns->addConstant("PM_ExclusiveIndicatorHeight", new QoreNode((int64)QStyle::PM_ExclusiveIndicatorHeight));
   qstyle_ns->addConstant("PM_CheckListButtonSize",   new QoreNode((int64)QStyle::PM_CheckListButtonSize));
   qstyle_ns->addConstant("PM_CheckListControllerSize", new QoreNode((int64)QStyle::PM_CheckListControllerSize));
   qstyle_ns->addConstant("PM_DialogButtonsSeparator", new QoreNode((int64)QStyle::PM_DialogButtonsSeparator));
   qstyle_ns->addConstant("PM_DialogButtonsButtonWidth", new QoreNode((int64)QStyle::PM_DialogButtonsButtonWidth));
   qstyle_ns->addConstant("PM_DialogButtonsButtonHeight", new QoreNode((int64)QStyle::PM_DialogButtonsButtonHeight));
   qstyle_ns->addConstant("PM_MdiSubWindowFrameWidth", new QoreNode((int64)QStyle::PM_MdiSubWindowFrameWidth));
   qstyle_ns->addConstant("PM_MDIFrameWidth",         new QoreNode((int64)QStyle::PM_MDIFrameWidth));
   qstyle_ns->addConstant("PM_MdiSubWindowMinimizedWidth", new QoreNode((int64)QStyle::PM_MdiSubWindowMinimizedWidth));
   qstyle_ns->addConstant("PM_MDIMinimizedWidth",     new QoreNode((int64)QStyle::PM_MDIMinimizedWidth));
   qstyle_ns->addConstant("PM_HeaderMargin",          new QoreNode((int64)QStyle::PM_HeaderMargin));
   qstyle_ns->addConstant("PM_HeaderMarkSize",        new QoreNode((int64)QStyle::PM_HeaderMarkSize));
   qstyle_ns->addConstant("PM_HeaderGripMargin",      new QoreNode((int64)QStyle::PM_HeaderGripMargin));
   qstyle_ns->addConstant("PM_TabBarTabShiftHorizontal", new QoreNode((int64)QStyle::PM_TabBarTabShiftHorizontal));
   qstyle_ns->addConstant("PM_TabBarTabShiftVertical", new QoreNode((int64)QStyle::PM_TabBarTabShiftVertical));
   qstyle_ns->addConstant("PM_TabBarScrollButtonWidth", new QoreNode((int64)QStyle::PM_TabBarScrollButtonWidth));
   qstyle_ns->addConstant("PM_ToolBarFrameWidth",     new QoreNode((int64)QStyle::PM_ToolBarFrameWidth));
   qstyle_ns->addConstant("PM_ToolBarHandleExtent",   new QoreNode((int64)QStyle::PM_ToolBarHandleExtent));
   qstyle_ns->addConstant("PM_ToolBarItemSpacing",    new QoreNode((int64)QStyle::PM_ToolBarItemSpacing));
   qstyle_ns->addConstant("PM_ToolBarItemMargin",     new QoreNode((int64)QStyle::PM_ToolBarItemMargin));
   qstyle_ns->addConstant("PM_ToolBarSeparatorExtent", new QoreNode((int64)QStyle::PM_ToolBarSeparatorExtent));
   qstyle_ns->addConstant("PM_ToolBarExtensionExtent", new QoreNode((int64)QStyle::PM_ToolBarExtensionExtent));
   qstyle_ns->addConstant("PM_SpinBoxSliderHeight",   new QoreNode((int64)QStyle::PM_SpinBoxSliderHeight));
   qstyle_ns->addConstant("PM_DefaultTopLevelMargin", new QoreNode((int64)QStyle::PM_DefaultTopLevelMargin));
   qstyle_ns->addConstant("PM_DefaultChildMargin",    new QoreNode((int64)QStyle::PM_DefaultChildMargin));
   qstyle_ns->addConstant("PM_DefaultLayoutSpacing",  new QoreNode((int64)QStyle::PM_DefaultLayoutSpacing));
   qstyle_ns->addConstant("PM_ToolBarIconSize",       new QoreNode((int64)QStyle::PM_ToolBarIconSize));
   qstyle_ns->addConstant("PM_ListViewIconSize",      new QoreNode((int64)QStyle::PM_ListViewIconSize));
   qstyle_ns->addConstant("PM_IconViewIconSize",      new QoreNode((int64)QStyle::PM_IconViewIconSize));
   qstyle_ns->addConstant("PM_SmallIconSize",         new QoreNode((int64)QStyle::PM_SmallIconSize));
   qstyle_ns->addConstant("PM_LargeIconSize",         new QoreNode((int64)QStyle::PM_LargeIconSize));
   qstyle_ns->addConstant("PM_FocusFrameVMargin",     new QoreNode((int64)QStyle::PM_FocusFrameVMargin));
   qstyle_ns->addConstant("PM_FocusFrameHMargin",     new QoreNode((int64)QStyle::PM_FocusFrameHMargin));
   qstyle_ns->addConstant("PM_ToolTipLabelFrameWidth", new QoreNode((int64)QStyle::PM_ToolTipLabelFrameWidth));
   qstyle_ns->addConstant("PM_CheckBoxLabelSpacing",  new QoreNode((int64)QStyle::PM_CheckBoxLabelSpacing));
   qstyle_ns->addConstant("PM_TabBarIconSize",        new QoreNode((int64)QStyle::PM_TabBarIconSize));
   qstyle_ns->addConstant("PM_SizeGripSize",          new QoreNode((int64)QStyle::PM_SizeGripSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleMargin", new QoreNode((int64)QStyle::PM_DockWidgetTitleMargin));
   qstyle_ns->addConstant("PM_MessageBoxIconSize",    new QoreNode((int64)QStyle::PM_MessageBoxIconSize));
   qstyle_ns->addConstant("PM_ButtonIconSize",        new QoreNode((int64)QStyle::PM_ButtonIconSize));
   qstyle_ns->addConstant("PM_DockWidgetTitleBarButtonMargin", new QoreNode((int64)QStyle::PM_DockWidgetTitleBarButtonMargin));
   qstyle_ns->addConstant("PM_RadioButtonLabelSpacing", new QoreNode((int64)QStyle::PM_RadioButtonLabelSpacing));
   qstyle_ns->addConstant("PM_LayoutLeftMargin",      new QoreNode((int64)QStyle::PM_LayoutLeftMargin));
   qstyle_ns->addConstant("PM_LayoutTopMargin",       new QoreNode((int64)QStyle::PM_LayoutTopMargin));
   qstyle_ns->addConstant("PM_LayoutRightMargin",     new QoreNode((int64)QStyle::PM_LayoutRightMargin));
   qstyle_ns->addConstant("PM_LayoutBottomMargin",    new QoreNode((int64)QStyle::PM_LayoutBottomMargin));
   qstyle_ns->addConstant("PM_LayoutHorizontalSpacing", new QoreNode((int64)QStyle::PM_LayoutHorizontalSpacing));
   qstyle_ns->addConstant("PM_LayoutVerticalSpacing", new QoreNode((int64)QStyle::PM_LayoutVerticalSpacing));
   qstyle_ns->addConstant("PM_CustomBase",            new QoreNode((int64)QStyle::PM_CustomBase));

   qt->addInitialNamespace(qstyle_ns);

   // automatically added classes
   qt->addSystemClass(initQPointFClass());
   qt->addSystemClass(initQPolygonClass());
   qt->addSystemClass(initQPolygonFClass());
   qt->addSystemClass(initQLineClass());
   qt->addSystemClass(initQLineFClass());
   qt->addSystemClass((qabstractbutton = initQAbstractButtonClass(qwidget)));
   qt->addSystemClass(initQPushButtonClass(qabstractbutton));
   qt->addSystemClass(initQMenuClass(qwidget));
   qt->addSystemClass(initQToolButtonClass(qabstractbutton));
   qt->addSystemClass((qdialog = initQDialogClass(qwidget)));
   qt->addSystemClass(initQLineEditClass(qwidget));
   qt->addSystemClass(initQTextLengthClass());
   qt->addSystemClass((qtextformat = initQTextFormatClass()));
   qt->addSystemClass(initQTextBlockFormatClass(qtextformat));
   qt->addSystemClass((qtextcharformat = initQTextCharFormatClass(qtextformat)));
   qt->addSystemClass(initQPenClass());
   qt->addSystemClass((qtextframeformat = initQTextFrameFormatClass(qtextformat)));
   qt->addSystemClass(initQTextTableFormatClass(qtextframeformat));
   qt->addSystemClass(initQTextListFormatClass(qtextformat));
   qt->addSystemClass(initQTextImageFormatClass(qtextcharformat));
   qt->addSystemClass((qstyleoption = initQStyleOptionClass()));
   qt->addSystemClass(initQModelIndexClass());
   qt->addSystemClass((qstyleoptionviewitem = initQStyleOptionViewItemClass(qstyleoption)));
   qt->addSystemClass(initQStyleOptionViewItemV2Class(qstyleoptionviewitem));
   qt->addSystemClass(initQAbstractItemModelClass(qobject));
   qt->addSystemClass((qabstractitemdelegate = initQAbstractItemDelegateClass(qobject)));
   qt->addSystemClass(initQItemDelegateClass(qabstractitemdelegate));
   qt->addSystemClass((qcombobox = initQComboBoxClass(qwidget)));
   qt->addSystemClass(initQCheckBoxClass(qabstractbutton));
   qt->addSystemClass((qabstractspinbox = initQAbstractSpinBoxClass(qwidget)));
   qt->addSystemClass((qdatetimeedit = initQDateTimeEditClass(qabstractspinbox)));
   qt->addSystemClass(initQByteArrayClass());
   qt->addSystemClass(initQUrlClass());
   qt->addSystemClass(initQVariantClass());
   qt->addSystemClass(initQGroupBoxClass(qwidget));
   qt->addSystemClass(initQDateEditClass(qdatetimeedit));
   qt->addSystemClass(initQFontMetricsClass());
   qt->addSystemClass(initQFontDatabaseClass());
   qt->addSystemClass(initQFontInfoClass());
   qt->addSystemClass(initQScrollBarClass(qabstractslider));
   qt->addSystemClass((qabstractscrollarea = initQAbstractScrollAreaClass(qframe)));
   qt->addSystemClass(initQScrollAreaClass(qabstractscrollarea));
   qt->addSystemClass(initQActionEventClass(qevent));
   qt->addSystemClass(initQCloseEventClass(qevent));
   qt->addSystemClass(initQContextMenuEventClass(qinputevent));
   qt->addSystemClass((qdropevent = initQDropEventClass(qevent)));
   qt->addSystemClass(initQMimeDataClass(qobject));
   qt->addSystemClass((qdragmoveevent = initQDragMoveEventClass(qdropevent)));
   qt->addSystemClass(initQDragEnterEventClass(qdragmoveevent));
   qt->addSystemClass(initQDragLeaveEventClass(qevent));
   qt->addSystemClass(initQFocusEventClass(qevent));
   qt->addSystemClass(initQHideEventClass(qevent));
   qt->addSystemClass(initQInputMethodEventClass(qevent));
   qt->addSystemClass(initQShowEventClass(qevent));
   qt->addSystemClass(initQTabletEventClass(qinputevent));
   qt->addSystemClass(initQWheelEventClass(qinputevent));
   qt->addSystemClass(initQFontComboBoxClass(qcombobox));
   qt->addSystemClass(initQMainWindowClass(qwidget));
   qt->addSystemClass(initQRadioButtonClass(qabstractbutton));
   qt->addSystemClass((qstyleoptioncomplex = initQStyleOptionComplexClass(qstyleoption)));
   qt->addSystemClass(initQStyleOptionComboBoxClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionGroupBoxClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionSizeGripClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionSliderClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionSpinBoxClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionTitleBarClass(qstyleoptioncomplex));
   qt->addSystemClass(initQStyleOptionToolButtonClass(qstyleoptioncomplex));
   qt->addSystemClass(initQSpinBoxClass(qwidget));
   qt->addSystemClass(initQTableWidgetItemClass());
   qt->addSystemClass(initQStyleOptionMenuItemClass(qstyleoption));
   qt->addSystemClass(initQMessageBoxClass(qdialog));
   qt->addSystemClass(initQStyleOptionButtonClass(qstyleoption));
   qt->addSystemClass(initQFileDialogClass(qdialog));
   qt->addSystemClass(initQDirClass());
   qt->addSystemClass(initQMetaObjectClass());
   qt->addSystemClass(initQMenuBarClass(qwidget));
   qt->addSystemClass(initQPrinterClass(qpaintdevice));
   qt->addSystemClass(initQPrintDialogClass(qwidget));

   // add QBoxLayout namespace and constants
   class Namespace *qbl = new Namespace("QBoxLayout");

   // Direction enum
   qbl->addConstant("LeftToRight",    new QoreNode((int64)QBoxLayout::LeftToRight));
   qbl->addConstant("RightToLeft",    new QoreNode((int64)QBoxLayout::RightToLeft));
   qbl->addConstant("TopToBottom",    new QoreNode((int64)QBoxLayout::TopToBottom));
   qbl->addConstant("BottomToTop",    new QoreNode((int64)QBoxLayout::BottomToTop));

   qt->addInitialNamespace(qbl);

   Namespace *qabstractitemview_ns = new Namespace("QAbstractItemView");
   
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
   
   qt->addInitialNamespace(qabstractitemview_ns);

   Namespace *qheaderview = new Namespace("QHeaderView");

   // ResizeMode enum
   qheaderview->addConstant("Interactive",              new QoreNode((int64)QHeaderView::Interactive));
   qheaderview->addConstant("Stretch",                  new QoreNode((int64)QHeaderView::Stretch));
   qheaderview->addConstant("Fixed",                    new QoreNode((int64)QHeaderView::Fixed));
   qheaderview->addConstant("ResizeToContents",         new QoreNode((int64)QHeaderView::ResizeToContents));
   qheaderview->addConstant("Custom",                   new QoreNode((int64)QHeaderView::Custom));

   qheaderview->addSystemClass(initQHeaderViewClass(qabstractitemview));

   qt->addInitialNamespace(qheaderview);


   Namespace *qclipboard = new Namespace("QClipboard");
   
   // Mode enum
   qclipboard->addConstant("Clipboard",                new QoreNode((int64)QClipboard::Clipboard));
   qclipboard->addConstant("Selection",                new QoreNode((int64)QClipboard::Selection));
   qclipboard->addConstant("FindBuffer",               new QoreNode((int64)QClipboard::FindBuffer));
   qclipboard->addConstant("LastMode",                 new QoreNode((int64)QClipboard::LastMode));

   qclipboard->addSystemClass(initQClipboardClass(qobject));

   qt->addInitialNamespace(qclipboard);

   Namespace *qchar = new Namespace("QChar");
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

   qt->addInitialNamespace(qchar);

   Namespace *qcalendarwidget = new Namespace("QCalendarWidget");

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

   qt->addInitialNamespace(qcalendarwidget);

   Namespace *qlocale = new Namespace("QLocale");
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

   qt->addInitialNamespace(qlocale);

   // add QFont namespaces and constants
   class Namespace *qframens = new Namespace("QFrame");
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

   qt->addInitialNamespace(qframens);

   // add QFont namespaces and constants
   class Namespace *qf = new Namespace("QFont");
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

   qt->addInitialNamespace(qf);

   // add QLCDNumber namespace and constants
   class Namespace *qlcdn = new Namespace("QLCDNumber");
   qlcdn->addConstant("Outline",   new QoreNode((int64)QLCDNumber::Outline));
   qlcdn->addConstant("Filled",    new QoreNode((int64)QLCDNumber::Filled));
   qlcdn->addConstant("Flat",      new QoreNode((int64)QLCDNumber::Flat));
   qlcdn->addConstant("Hex",       new QoreNode((int64)QLCDNumber::Hex));
   qlcdn->addConstant("Dec",       new QoreNode((int64)QLCDNumber::Dec));
   qlcdn->addConstant("Oct",       new QoreNode((int64)QLCDNumber::Oct));
   qlcdn->addConstant("Bin",       new QoreNode((int64)QLCDNumber::Bin));
   qt->addInitialNamespace(qlcdn);

   // add QAbstractSlider namespace and constants
   class Namespace *qas = new Namespace("QAbstractSlider");
   qas->addConstant("SliderNoAction",        new QoreNode((int64)QAbstractSlider::SliderNoAction));
   qas->addConstant("SliderSingleStepAdd",   new QoreNode((int64)QAbstractSlider::SliderSingleStepAdd));
   qas->addConstant("SliderSingleStepSub",   new QoreNode((int64)QAbstractSlider::SliderSingleStepSub));
   qas->addConstant("SliderPageStepAdd",     new QoreNode((int64)QAbstractSlider::SliderPageStepAdd));
   qas->addConstant("SliderPageStepSub",     new QoreNode((int64)QAbstractSlider::SliderPageStepSub));
   qas->addConstant("SliderToMinimum",       new QoreNode((int64)QAbstractSlider::SliderToMinimum));
   qas->addConstant("SliderToMaximum",       new QoreNode((int64)QAbstractSlider::SliderToMaximum));
   qas->addConstant("SliderMove",            new QoreNode((int64)QAbstractSlider::SliderMove));
   qt->addInitialNamespace(qas);

   // CheckState enum
   qt->addConstant("Unchecked",                new QoreNode((int64)Qt::Unchecked));
   qt->addConstant("PartiallyChecked",         new QoreNode((int64)Qt::PartiallyChecked));
   qt->addConstant("Checked",                  new QoreNode((int64)Qt::Checked));

   // add connection enum values
   qt->addConstant("AutoConnection",           new QoreNode((int64)Qt::AutoConnection));
   qt->addConstant("DirectConnection",         new QoreNode((int64)Qt::DirectConnection));
   qt->addConstant("QueuedConnection",         new QoreNode((int64)Qt::QueuedConnection));
   qt->addConstant("AutoCompatConnection",     new QoreNode((int64)Qt::AutoCompatConnection));
   qt->addConstant("BlockingQueuedConnection", new QoreNode((int64)Qt::BlockingQueuedConnection));

   // orientation enum values
   qt->addConstant("Vertical",        new QoreNode((int64)Qt::Vertical));
   qt->addConstant("Horizontal",      new QoreNode((int64)Qt::Horizontal));

   // GlobalColor enum
   qt->addConstant("color0",            new QoreNode((int64)Qt::color0));
   qt->addConstant("color1",            new QoreNode((int64)Qt::color1));
   qt->addConstant("black",             new QoreNode((int64)Qt::black));
   qt->addConstant("white",             new QoreNode((int64)Qt::white));
   qt->addConstant("darkGray",          new QoreNode((int64)Qt::darkGray));
   qt->addConstant("gray",              new QoreNode((int64)Qt::gray));
   qt->addConstant("lightGray",         new QoreNode((int64)Qt::lightGray));
   qt->addConstant("red",               new QoreNode((int64)Qt::red));
   qt->addConstant("green",             new QoreNode((int64)Qt::green));
   qt->addConstant("blue",              new QoreNode((int64)Qt::blue));
   qt->addConstant("cyan",              new QoreNode((int64)Qt::cyan));
   qt->addConstant("magenta",           new QoreNode((int64)Qt::magenta));
   qt->addConstant("yellow",            new QoreNode((int64)Qt::yellow));
   qt->addConstant("darkRed",           new QoreNode((int64)Qt::darkRed));
   qt->addConstant("darkGreen",         new QoreNode((int64)Qt::darkGreen));
   qt->addConstant("darkBlue",          new QoreNode((int64)Qt::darkBlue));
   qt->addConstant("darkCyan",          new QoreNode((int64)Qt::darkCyan));
   qt->addConstant("darkMagenta",       new QoreNode((int64)Qt::darkMagenta));
   qt->addConstant("darkYellow",        new QoreNode((int64)Qt::darkYellow));
   qt->addConstant("transparent",       new QoreNode((int64)Qt::transparent));

   // BrushStyle enum
   qt->addConstant("NoBrush",                  make_enum(NT_BRUSHSTYLE, (int)Qt::NoBrush));
   qt->addConstant("SolidPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::SolidPattern));
   qt->addConstant("Dense1Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense1Pattern));
   qt->addConstant("Dense2Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense2Pattern));
   qt->addConstant("Dense3Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense3Pattern));
   qt->addConstant("Dense4Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense4Pattern));
   qt->addConstant("Dense5Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense5Pattern));
   qt->addConstant("Dense6Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense6Pattern));
   qt->addConstant("Dense7Pattern",            make_enum(NT_BRUSHSTYLE, (int)Qt::Dense7Pattern));
   qt->addConstant("HorPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::HorPattern));
   qt->addConstant("VerPattern",               make_enum(NT_BRUSHSTYLE, (int)Qt::VerPattern));
   qt->addConstant("CrossPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::CrossPattern));
   qt->addConstant("BDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::BDiagPattern));
   qt->addConstant("FDiagPattern",             make_enum(NT_BRUSHSTYLE, (int)Qt::FDiagPattern));
   qt->addConstant("DiagCrossPattern",         make_enum(NT_BRUSHSTYLE, (int)Qt::DiagCrossPattern));
   qt->addConstant("LinearGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::LinearGradientPattern));
   qt->addConstant("RadialGradientPattern",    make_enum(NT_BRUSHSTYLE, (int)Qt::RadialGradientPattern));
   qt->addConstant("ConicalGradientPattern",   make_enum(NT_BRUSHSTYLE, (int)Qt::ConicalGradientPattern));
   qt->addConstant("TexturePattern",           make_enum(NT_BRUSHSTYLE, (int)Qt::TexturePattern));

   // PenStyle enum
   qt->addConstant("NoPen",             make_enum(NT_PENSTYLE, (int)Qt::NoPen));
   qt->addConstant("SolidLine",         make_enum(NT_PENSTYLE, (int)Qt::SolidLine));
   qt->addConstant("DashLine",          make_enum(NT_PENSTYLE, (int)Qt::DashLine));
   qt->addConstant("DotLine",           make_enum(NT_PENSTYLE, (int)Qt::DotLine));
   qt->addConstant("DashDotLine",       make_enum(NT_PENSTYLE, (int)Qt::DashDotLine));
   qt->addConstant("DashDotDotLine",    make_enum(NT_PENSTYLE, (int)Qt::DashDotDotLine));
   qt->addConstant("CustomDashLine",    make_enum(NT_PENSTYLE, (int)Qt::CustomDashLine));

   // AlignmentFlag enum
   qt->addConstant("AlignLeft",                new QoreNode((int64)Qt::AlignLeft));
   qt->addConstant("AlignLeading",             new QoreNode((int64)Qt::AlignLeading));
   qt->addConstant("AlignRight",               new QoreNode((int64)Qt::AlignRight));
   qt->addConstant("AlignTrailing",            new QoreNode((int64)Qt::AlignTrailing));
   qt->addConstant("AlignHCenter",             new QoreNode((int64)Qt::AlignHCenter));
   qt->addConstant("AlignJustify",             new QoreNode((int64)Qt::AlignJustify));
   qt->addConstant("AlignAbsolute",            new QoreNode((int64)Qt::AlignAbsolute));
   qt->addConstant("AlignHorizontal_Mask",     new QoreNode((int64)Qt::AlignHorizontal_Mask));
   qt->addConstant("AlignTop",                 new QoreNode((int64)Qt::AlignTop));
   qt->addConstant("AlignBottom",              new QoreNode((int64)Qt::AlignBottom));
   qt->addConstant("AlignVCenter",             new QoreNode((int64)Qt::AlignVCenter));
   qt->addConstant("AlignVertical_Mask",       new QoreNode((int64)Qt::AlignVertical_Mask));
   qt->addConstant("AlignCenter",              new QoreNode((int64)Qt::AlignCenter));

   // MouseButton enum
   qt->addConstant("NoButton",                 new QoreNode((int64)Qt::NoButton));
   qt->addConstant("LeftButton",               new QoreNode((int64)Qt::LeftButton));
   qt->addConstant("RightButton",              new QoreNode((int64)Qt::RightButton));
   qt->addConstant("MidButton",                new QoreNode((int64)Qt::MidButton));
   qt->addConstant("XButton1",                 new QoreNode((int64)Qt::XButton1));
   qt->addConstant("XButton2",                 new QoreNode((int64)Qt::XButton2));
   qt->addConstant("MouseButtonMask",          new QoreNode((int64)Qt::MouseButtonMask));

   // Modifier enum
   qt->addConstant("META",                     new QoreNode((int64)Qt::META));
   qt->addConstant("SHIFT",                    new QoreNode((int64)Qt::SHIFT));
   qt->addConstant("CTRL",                     new QoreNode((int64)Qt::CTRL));
   qt->addConstant("ALT",                      new QoreNode((int64)Qt::ALT));
   qt->addConstant("MODIFIER_MASK",            new QoreNode((int64)Qt::MODIFIER_MASK));
   qt->addConstant("UNICODE_ACCEL",            new QoreNode((int64)Qt::UNICODE_ACCEL));

   // DayOfWeek
   qt->addConstant("Monday",                   new QoreNode((int64)Qt::Monday));
   qt->addConstant("Tuesday",                  new QoreNode((int64)Qt::Tuesday));
   qt->addConstant("Wednesday",                new QoreNode((int64)Qt::Wednesday));
   qt->addConstant("Thursday",                 new QoreNode((int64)Qt::Thursday));
   qt->addConstant("Friday",                   new QoreNode((int64)Qt::Friday));
   qt->addConstant("Saturday",                 new QoreNode((int64)Qt::Saturday));
   qt->addConstant("Sunday",                   new QoreNode((int64)Qt::Sunday));

   // ContextMenuPolicy enum
   qt->addConstant("NoContextMenu",            new QoreNode((int64)Qt::NoContextMenu));
   qt->addConstant("DefaultContextMenu",       new QoreNode((int64)Qt::DefaultContextMenu));
   qt->addConstant("ActionsContextMenu",       new QoreNode((int64)Qt::ActionsContextMenu));
   qt->addConstant("CustomContextMenu",        new QoreNode((int64)Qt::CustomContextMenu));
   qt->addConstant("PreventContextMenu",       new QoreNode((int64)Qt::PreventContextMenu));

   // Key enum
   qt->addConstant("Key_Escape",               new QoreNode((int64)Qt::Key_Escape));
   qt->addConstant("Key_Tab",                  new QoreNode((int64)Qt::Key_Tab));
   qt->addConstant("Key_Backtab",              new QoreNode((int64)Qt::Key_Backtab));
   qt->addConstant("Key_Backspace",            new QoreNode((int64)Qt::Key_Backspace));
   qt->addConstant("Key_Return",               new QoreNode((int64)Qt::Key_Return));
   qt->addConstant("Key_Enter",                new QoreNode((int64)Qt::Key_Enter));
   qt->addConstant("Key_Insert",               new QoreNode((int64)Qt::Key_Insert));
   qt->addConstant("Key_Delete",               new QoreNode((int64)Qt::Key_Delete));
   qt->addConstant("Key_Pause",                new QoreNode((int64)Qt::Key_Pause));
   qt->addConstant("Key_Print",                new QoreNode((int64)Qt::Key_Print));
   qt->addConstant("Key_SysReq",               new QoreNode((int64)Qt::Key_SysReq));
   qt->addConstant("Key_Clear",                new QoreNode((int64)Qt::Key_Clear));
   qt->addConstant("Key_Home",                 new QoreNode((int64)Qt::Key_Home));
   qt->addConstant("Key_End",                  new QoreNode((int64)Qt::Key_End));
   qt->addConstant("Key_Left",                 new QoreNode((int64)Qt::Key_Left));
   qt->addConstant("Key_Up",                   new QoreNode((int64)Qt::Key_Up));
   qt->addConstant("Key_Right",                new QoreNode((int64)Qt::Key_Right));
   qt->addConstant("Key_Down",                 new QoreNode((int64)Qt::Key_Down));
   qt->addConstant("Key_PageUp",               new QoreNode((int64)Qt::Key_PageUp));
   qt->addConstant("Key_PageDown",             new QoreNode((int64)Qt::Key_PageDown));
   qt->addConstant("Key_Shift",                new QoreNode((int64)Qt::Key_Shift));
   qt->addConstant("Key_Control",              new QoreNode((int64)Qt::Key_Control));
   qt->addConstant("Key_Meta",                 new QoreNode((int64)Qt::Key_Meta));
   qt->addConstant("Key_Alt",                  new QoreNode((int64)Qt::Key_Alt));
   qt->addConstant("Key_CapsLock",             new QoreNode((int64)Qt::Key_CapsLock));
   qt->addConstant("Key_NumLock",              new QoreNode((int64)Qt::Key_NumLock));
   qt->addConstant("Key_ScrollLock",           new QoreNode((int64)Qt::Key_ScrollLock));
   qt->addConstant("Key_F1",                   new QoreNode((int64)Qt::Key_F1));
   qt->addConstant("Key_F2",                   new QoreNode((int64)Qt::Key_F2));
   qt->addConstant("Key_F3",                   new QoreNode((int64)Qt::Key_F3));
   qt->addConstant("Key_F4",                   new QoreNode((int64)Qt::Key_F4));
   qt->addConstant("Key_F5",                   new QoreNode((int64)Qt::Key_F5));
   qt->addConstant("Key_F6",                   new QoreNode((int64)Qt::Key_F6));
   qt->addConstant("Key_F7",                   new QoreNode((int64)Qt::Key_F7));
   qt->addConstant("Key_F8",                   new QoreNode((int64)Qt::Key_F8));
   qt->addConstant("Key_F9",                   new QoreNode((int64)Qt::Key_F9));
   qt->addConstant("Key_F10",                  new QoreNode((int64)Qt::Key_F10));
   qt->addConstant("Key_F11",                  new QoreNode((int64)Qt::Key_F11));
   qt->addConstant("Key_F12",                  new QoreNode((int64)Qt::Key_F12));
   qt->addConstant("Key_F13",                  new QoreNode((int64)Qt::Key_F13));
   qt->addConstant("Key_F14",                  new QoreNode((int64)Qt::Key_F14));
   qt->addConstant("Key_F15",                  new QoreNode((int64)Qt::Key_F15));
   qt->addConstant("Key_F16",                  new QoreNode((int64)Qt::Key_F16));
   qt->addConstant("Key_F17",                  new QoreNode((int64)Qt::Key_F17));
   qt->addConstant("Key_F18",                  new QoreNode((int64)Qt::Key_F18));
   qt->addConstant("Key_F19",                  new QoreNode((int64)Qt::Key_F19));
   qt->addConstant("Key_F20",                  new QoreNode((int64)Qt::Key_F20));
   qt->addConstant("Key_F21",                  new QoreNode((int64)Qt::Key_F21));
   qt->addConstant("Key_F22",                  new QoreNode((int64)Qt::Key_F22));
   qt->addConstant("Key_F23",                  new QoreNode((int64)Qt::Key_F23));
   qt->addConstant("Key_F24",                  new QoreNode((int64)Qt::Key_F24));
   qt->addConstant("Key_F25",                  new QoreNode((int64)Qt::Key_F25));
   qt->addConstant("Key_F26",                  new QoreNode((int64)Qt::Key_F26));
   qt->addConstant("Key_F27",                  new QoreNode((int64)Qt::Key_F27));
   qt->addConstant("Key_F28",                  new QoreNode((int64)Qt::Key_F28));
   qt->addConstant("Key_F29",                  new QoreNode((int64)Qt::Key_F29));
   qt->addConstant("Key_F30",                  new QoreNode((int64)Qt::Key_F30));
   qt->addConstant("Key_F31",                  new QoreNode((int64)Qt::Key_F31));
   qt->addConstant("Key_F32",                  new QoreNode((int64)Qt::Key_F32));
   qt->addConstant("Key_F33",                  new QoreNode((int64)Qt::Key_F33));
   qt->addConstant("Key_F34",                  new QoreNode((int64)Qt::Key_F34));
   qt->addConstant("Key_F35",                  new QoreNode((int64)Qt::Key_F35));
   qt->addConstant("Key_Super_L",              new QoreNode((int64)Qt::Key_Super_L));
   qt->addConstant("Key_Super_R",              new QoreNode((int64)Qt::Key_Super_R));
   qt->addConstant("Key_Menu",                 new QoreNode((int64)Qt::Key_Menu));
   qt->addConstant("Key_Hyper_L",              new QoreNode((int64)Qt::Key_Hyper_L));
   qt->addConstant("Key_Hyper_R",              new QoreNode((int64)Qt::Key_Hyper_R));
   qt->addConstant("Key_Help",                 new QoreNode((int64)Qt::Key_Help));
   qt->addConstant("Key_Direction_L",          new QoreNode((int64)Qt::Key_Direction_L));
   qt->addConstant("Key_Direction_R",          new QoreNode((int64)Qt::Key_Direction_R));
   qt->addConstant("Key_Space",                new QoreNode((int64)Qt::Key_Space));
   qt->addConstant("Key_Any",                  new QoreNode((int64)Qt::Key_Any));
   qt->addConstant("Key_Exclam",               new QoreNode((int64)Qt::Key_Exclam));
   qt->addConstant("Key_QuoteDbl",             new QoreNode((int64)Qt::Key_QuoteDbl));
   qt->addConstant("Key_NumberSign",           new QoreNode((int64)Qt::Key_NumberSign));
   qt->addConstant("Key_Dollar",               new QoreNode((int64)Qt::Key_Dollar));
   qt->addConstant("Key_Percent",              new QoreNode((int64)Qt::Key_Percent));
   qt->addConstant("Key_Ampersand",            new QoreNode((int64)Qt::Key_Ampersand));
   qt->addConstant("Key_Apostrophe",           new QoreNode((int64)Qt::Key_Apostrophe));
   qt->addConstant("Key_ParenLeft",            new QoreNode((int64)Qt::Key_ParenLeft));
   qt->addConstant("Key_ParenRight",           new QoreNode((int64)Qt::Key_ParenRight));
   qt->addConstant("Key_Asterisk",             new QoreNode((int64)Qt::Key_Asterisk));
   qt->addConstant("Key_Plus",                 new QoreNode((int64)Qt::Key_Plus));
   qt->addConstant("Key_Comma",                new QoreNode((int64)Qt::Key_Comma));
   qt->addConstant("Key_Minus",                new QoreNode((int64)Qt::Key_Minus));
   qt->addConstant("Key_Period",               new QoreNode((int64)Qt::Key_Period));
   qt->addConstant("Key_Slash",                new QoreNode((int64)Qt::Key_Slash));
   qt->addConstant("Key_0",                    new QoreNode((int64)Qt::Key_0));
   qt->addConstant("Key_1",                    new QoreNode((int64)Qt::Key_1));
   qt->addConstant("Key_2",                    new QoreNode((int64)Qt::Key_2));
   qt->addConstant("Key_3",                    new QoreNode((int64)Qt::Key_3));
   qt->addConstant("Key_4",                    new QoreNode((int64)Qt::Key_4));
   qt->addConstant("Key_5",                    new QoreNode((int64)Qt::Key_5));
   qt->addConstant("Key_6",                    new QoreNode((int64)Qt::Key_6));
   qt->addConstant("Key_7",                    new QoreNode((int64)Qt::Key_7));
   qt->addConstant("Key_8",                    new QoreNode((int64)Qt::Key_8));
   qt->addConstant("Key_9",                    new QoreNode((int64)Qt::Key_9));
   qt->addConstant("Key_Colon",                new QoreNode((int64)Qt::Key_Colon));
   qt->addConstant("Key_Semicolon",            new QoreNode((int64)Qt::Key_Semicolon));
   qt->addConstant("Key_Less",                 new QoreNode((int64)Qt::Key_Less));
   qt->addConstant("Key_Equal",                new QoreNode((int64)Qt::Key_Equal));
   qt->addConstant("Key_Greater",              new QoreNode((int64)Qt::Key_Greater));
   qt->addConstant("Key_Question",             new QoreNode((int64)Qt::Key_Question));
   qt->addConstant("Key_At",                   new QoreNode((int64)Qt::Key_At));
   qt->addConstant("Key_A",                    new QoreNode((int64)Qt::Key_A));
   qt->addConstant("Key_B",                    new QoreNode((int64)Qt::Key_B));
   qt->addConstant("Key_C",                    new QoreNode((int64)Qt::Key_C));
   qt->addConstant("Key_D",                    new QoreNode((int64)Qt::Key_D));
   qt->addConstant("Key_E",                    new QoreNode((int64)Qt::Key_E));
   qt->addConstant("Key_F",                    new QoreNode((int64)Qt::Key_F));
   qt->addConstant("Key_G",                    new QoreNode((int64)Qt::Key_G));
   qt->addConstant("Key_H",                    new QoreNode((int64)Qt::Key_H));
   qt->addConstant("Key_I",                    new QoreNode((int64)Qt::Key_I));
   qt->addConstant("Key_J",                    new QoreNode((int64)Qt::Key_J));
   qt->addConstant("Key_K",                    new QoreNode((int64)Qt::Key_K));
   qt->addConstant("Key_L",                    new QoreNode((int64)Qt::Key_L));
   qt->addConstant("Key_M",                    new QoreNode((int64)Qt::Key_M));
   qt->addConstant("Key_N",                    new QoreNode((int64)Qt::Key_N));
   qt->addConstant("Key_O",                    new QoreNode((int64)Qt::Key_O));
   qt->addConstant("Key_P",                    new QoreNode((int64)Qt::Key_P));
   qt->addConstant("Key_Q",                    new QoreNode((int64)Qt::Key_Q));
   qt->addConstant("Key_R",                    new QoreNode((int64)Qt::Key_R));
   qt->addConstant("Key_S",                    new QoreNode((int64)Qt::Key_S));
   qt->addConstant("Key_T",                    new QoreNode((int64)Qt::Key_T));
   qt->addConstant("Key_U",                    new QoreNode((int64)Qt::Key_U));
   qt->addConstant("Key_V",                    new QoreNode((int64)Qt::Key_V));
   qt->addConstant("Key_W",                    new QoreNode((int64)Qt::Key_W));
   qt->addConstant("Key_X",                    new QoreNode((int64)Qt::Key_X));
   qt->addConstant("Key_Y",                    new QoreNode((int64)Qt::Key_Y));
   qt->addConstant("Key_Z",                    new QoreNode((int64)Qt::Key_Z));
   qt->addConstant("Key_BracketLeft",          new QoreNode((int64)Qt::Key_BracketLeft));
   qt->addConstant("Key_Backslash",            new QoreNode((int64)Qt::Key_Backslash));
   qt->addConstant("Key_BracketRight",         new QoreNode((int64)Qt::Key_BracketRight));
   qt->addConstant("Key_AsciiCircum",          new QoreNode((int64)Qt::Key_AsciiCircum));
   qt->addConstant("Key_Underscore",           new QoreNode((int64)Qt::Key_Underscore));
   qt->addConstant("Key_QuoteLeft",            new QoreNode((int64)Qt::Key_QuoteLeft));
   qt->addConstant("Key_BraceLeft",            new QoreNode((int64)Qt::Key_BraceLeft));
   qt->addConstant("Key_Bar",                  new QoreNode((int64)Qt::Key_Bar));
   qt->addConstant("Key_BraceRight",           new QoreNode((int64)Qt::Key_BraceRight));
   qt->addConstant("Key_AsciiTilde",           new QoreNode((int64)Qt::Key_AsciiTilde));
   qt->addConstant("Key_nobreakspace",         new QoreNode((int64)Qt::Key_nobreakspace));
   qt->addConstant("Key_exclamdown",           new QoreNode((int64)Qt::Key_exclamdown));
   qt->addConstant("Key_cent",                 new QoreNode((int64)Qt::Key_cent));
   qt->addConstant("Key_sterling",             new QoreNode((int64)Qt::Key_sterling));
   qt->addConstant("Key_currency",             new QoreNode((int64)Qt::Key_currency));
   qt->addConstant("Key_yen",                  new QoreNode((int64)Qt::Key_yen));
   qt->addConstant("Key_brokenbar",            new QoreNode((int64)Qt::Key_brokenbar));
   qt->addConstant("Key_section",              new QoreNode((int64)Qt::Key_section));
   qt->addConstant("Key_diaeresis",            new QoreNode((int64)Qt::Key_diaeresis));
   qt->addConstant("Key_copyright",            new QoreNode((int64)Qt::Key_copyright));
   qt->addConstant("Key_ordfeminine",          new QoreNode((int64)Qt::Key_ordfeminine));
   qt->addConstant("Key_guillemotleft",        new QoreNode((int64)Qt::Key_guillemotleft));
   qt->addConstant("Key_notsign",              new QoreNode((int64)Qt::Key_notsign));
   qt->addConstant("Key_hyphen",               new QoreNode((int64)Qt::Key_hyphen));
   qt->addConstant("Key_registered",           new QoreNode((int64)Qt::Key_registered));
   qt->addConstant("Key_macron",               new QoreNode((int64)Qt::Key_macron));
   qt->addConstant("Key_degree",               new QoreNode((int64)Qt::Key_degree));
   qt->addConstant("Key_plusminus",            new QoreNode((int64)Qt::Key_plusminus));
   qt->addConstant("Key_twosuperior",          new QoreNode((int64)Qt::Key_twosuperior));
   qt->addConstant("Key_threesuperior",        new QoreNode((int64)Qt::Key_threesuperior));
   qt->addConstant("Key_acute",                new QoreNode((int64)Qt::Key_acute));
   qt->addConstant("Key_mu",                   new QoreNode((int64)Qt::Key_mu));
   qt->addConstant("Key_paragraph",            new QoreNode((int64)Qt::Key_paragraph));
   qt->addConstant("Key_periodcentered",       new QoreNode((int64)Qt::Key_periodcentered));
   qt->addConstant("Key_cedilla",              new QoreNode((int64)Qt::Key_cedilla));
   qt->addConstant("Key_onesuperior",          new QoreNode((int64)Qt::Key_onesuperior));
   qt->addConstant("Key_masculine",            new QoreNode((int64)Qt::Key_masculine));
   qt->addConstant("Key_guillemotright",       new QoreNode((int64)Qt::Key_guillemotright));
   qt->addConstant("Key_onequarter",           new QoreNode((int64)Qt::Key_onequarter));
   qt->addConstant("Key_onehalf",              new QoreNode((int64)Qt::Key_onehalf));
   qt->addConstant("Key_threequarters",        new QoreNode((int64)Qt::Key_threequarters));
   qt->addConstant("Key_questiondown",         new QoreNode((int64)Qt::Key_questiondown));
   qt->addConstant("Key_Agrave",               new QoreNode((int64)Qt::Key_Agrave));
   qt->addConstant("Key_Aacute",               new QoreNode((int64)Qt::Key_Aacute));
   qt->addConstant("Key_Acircumflex",          new QoreNode((int64)Qt::Key_Acircumflex));
   qt->addConstant("Key_Atilde",               new QoreNode((int64)Qt::Key_Atilde));
   qt->addConstant("Key_Adiaeresis",           new QoreNode((int64)Qt::Key_Adiaeresis));
   qt->addConstant("Key_Aring",                new QoreNode((int64)Qt::Key_Aring));
   qt->addConstant("Key_AE",                   new QoreNode((int64)Qt::Key_AE));
   qt->addConstant("Key_Ccedilla",             new QoreNode((int64)Qt::Key_Ccedilla));
   qt->addConstant("Key_Egrave",               new QoreNode((int64)Qt::Key_Egrave));
   qt->addConstant("Key_Eacute",               new QoreNode((int64)Qt::Key_Eacute));
   qt->addConstant("Key_Ecircumflex",          new QoreNode((int64)Qt::Key_Ecircumflex));
   qt->addConstant("Key_Ediaeresis",           new QoreNode((int64)Qt::Key_Ediaeresis));
   qt->addConstant("Key_Igrave",               new QoreNode((int64)Qt::Key_Igrave));
   qt->addConstant("Key_Iacute",               new QoreNode((int64)Qt::Key_Iacute));
   qt->addConstant("Key_Icircumflex",          new QoreNode((int64)Qt::Key_Icircumflex));
   qt->addConstant("Key_Idiaeresis",           new QoreNode((int64)Qt::Key_Idiaeresis));
   qt->addConstant("Key_ETH",                  new QoreNode((int64)Qt::Key_ETH));
   qt->addConstant("Key_Ntilde",               new QoreNode((int64)Qt::Key_Ntilde));
   qt->addConstant("Key_Ograve",               new QoreNode((int64)Qt::Key_Ograve));
   qt->addConstant("Key_Oacute",               new QoreNode((int64)Qt::Key_Oacute));
   qt->addConstant("Key_Ocircumflex",          new QoreNode((int64)Qt::Key_Ocircumflex));
   qt->addConstant("Key_Otilde",               new QoreNode((int64)Qt::Key_Otilde));
   qt->addConstant("Key_Odiaeresis",           new QoreNode((int64)Qt::Key_Odiaeresis));
   qt->addConstant("Key_multiply",             new QoreNode((int64)Qt::Key_multiply));
   qt->addConstant("Key_Ooblique",             new QoreNode((int64)Qt::Key_Ooblique));
   qt->addConstant("Key_Ugrave",               new QoreNode((int64)Qt::Key_Ugrave));
   qt->addConstant("Key_Uacute",               new QoreNode((int64)Qt::Key_Uacute));
   qt->addConstant("Key_Ucircumflex",          new QoreNode((int64)Qt::Key_Ucircumflex));
   qt->addConstant("Key_Udiaeresis",           new QoreNode((int64)Qt::Key_Udiaeresis));
   qt->addConstant("Key_Yacute",               new QoreNode((int64)Qt::Key_Yacute));
   qt->addConstant("Key_THORN",                new QoreNode((int64)Qt::Key_THORN));
   qt->addConstant("Key_ssharp",               new QoreNode((int64)Qt::Key_ssharp));
   qt->addConstant("Key_division",             new QoreNode((int64)Qt::Key_division));
   qt->addConstant("Key_ydiaeresis",           new QoreNode((int64)Qt::Key_ydiaeresis));
   qt->addConstant("Key_AltGr",                new QoreNode((int64)Qt::Key_AltGr));
   qt->addConstant("Key_Multi_key",            new QoreNode((int64)Qt::Key_Multi_key));
   qt->addConstant("Key_Codeinput",            new QoreNode((int64)Qt::Key_Codeinput));
   qt->addConstant("Key_SingleCandidate",      new QoreNode((int64)Qt::Key_SingleCandidate));
   qt->addConstant("Key_MultipleCandidate",    new QoreNode((int64)Qt::Key_MultipleCandidate));
   qt->addConstant("Key_PreviousCandidate",    new QoreNode((int64)Qt::Key_PreviousCandidate));
   qt->addConstant("Key_Mode_switch",          new QoreNode((int64)Qt::Key_Mode_switch));
   qt->addConstant("Key_Kanji",                new QoreNode((int64)Qt::Key_Kanji));
   qt->addConstant("Key_Muhenkan",             new QoreNode((int64)Qt::Key_Muhenkan));
   qt->addConstant("Key_Henkan",               new QoreNode((int64)Qt::Key_Henkan));
   qt->addConstant("Key_Romaji",               new QoreNode((int64)Qt::Key_Romaji));
   qt->addConstant("Key_Hiragana",             new QoreNode((int64)Qt::Key_Hiragana));
   qt->addConstant("Key_Katakana",             new QoreNode((int64)Qt::Key_Katakana));
   qt->addConstant("Key_Hiragana_Katakana",    new QoreNode((int64)Qt::Key_Hiragana_Katakana));
   qt->addConstant("Key_Zenkaku",              new QoreNode((int64)Qt::Key_Zenkaku));
   qt->addConstant("Key_Hankaku",              new QoreNode((int64)Qt::Key_Hankaku));
   qt->addConstant("Key_Zenkaku_Hankaku",      new QoreNode((int64)Qt::Key_Zenkaku_Hankaku));
   qt->addConstant("Key_Touroku",              new QoreNode((int64)Qt::Key_Touroku));
   qt->addConstant("Key_Massyo",               new QoreNode((int64)Qt::Key_Massyo));
   qt->addConstant("Key_Kana_Lock",            new QoreNode((int64)Qt::Key_Kana_Lock));
   qt->addConstant("Key_Kana_Shift",           new QoreNode((int64)Qt::Key_Kana_Shift));
   qt->addConstant("Key_Eisu_Shift",           new QoreNode((int64)Qt::Key_Eisu_Shift));
   qt->addConstant("Key_Eisu_toggle",          new QoreNode((int64)Qt::Key_Eisu_toggle));
   qt->addConstant("Key_Hangul",               new QoreNode((int64)Qt::Key_Hangul));
   qt->addConstant("Key_Hangul_Start",         new QoreNode((int64)Qt::Key_Hangul_Start));
   qt->addConstant("Key_Hangul_End",           new QoreNode((int64)Qt::Key_Hangul_End));
   qt->addConstant("Key_Hangul_Hanja",         new QoreNode((int64)Qt::Key_Hangul_Hanja));
   qt->addConstant("Key_Hangul_Jamo",          new QoreNode((int64)Qt::Key_Hangul_Jamo));
   qt->addConstant("Key_Hangul_Romaja",        new QoreNode((int64)Qt::Key_Hangul_Romaja));
   qt->addConstant("Key_Hangul_Jeonja",        new QoreNode((int64)Qt::Key_Hangul_Jeonja));
   qt->addConstant("Key_Hangul_Banja",         new QoreNode((int64)Qt::Key_Hangul_Banja));
   qt->addConstant("Key_Hangul_PreHanja",      new QoreNode((int64)Qt::Key_Hangul_PreHanja));
   qt->addConstant("Key_Hangul_PostHanja",     new QoreNode((int64)Qt::Key_Hangul_PostHanja));
   qt->addConstant("Key_Hangul_Special",       new QoreNode((int64)Qt::Key_Hangul_Special));
   qt->addConstant("Key_Dead_Grave",           new QoreNode((int64)Qt::Key_Dead_Grave));
   qt->addConstant("Key_Dead_Acute",           new QoreNode((int64)Qt::Key_Dead_Acute));
   qt->addConstant("Key_Dead_Circumflex",      new QoreNode((int64)Qt::Key_Dead_Circumflex));
   qt->addConstant("Key_Dead_Tilde",           new QoreNode((int64)Qt::Key_Dead_Tilde));
   qt->addConstant("Key_Dead_Macron",          new QoreNode((int64)Qt::Key_Dead_Macron));
   qt->addConstant("Key_Dead_Breve",           new QoreNode((int64)Qt::Key_Dead_Breve));
   qt->addConstant("Key_Dead_Abovedot",        new QoreNode((int64)Qt::Key_Dead_Abovedot));
   qt->addConstant("Key_Dead_Diaeresis",       new QoreNode((int64)Qt::Key_Dead_Diaeresis));
   qt->addConstant("Key_Dead_Abovering",       new QoreNode((int64)Qt::Key_Dead_Abovering));
   qt->addConstant("Key_Dead_Doubleacute",     new QoreNode((int64)Qt::Key_Dead_Doubleacute));
   qt->addConstant("Key_Dead_Caron",           new QoreNode((int64)Qt::Key_Dead_Caron));
   qt->addConstant("Key_Dead_Cedilla",         new QoreNode((int64)Qt::Key_Dead_Cedilla));
   qt->addConstant("Key_Dead_Ogonek",          new QoreNode((int64)Qt::Key_Dead_Ogonek));
   qt->addConstant("Key_Dead_Iota",            new QoreNode((int64)Qt::Key_Dead_Iota));
   qt->addConstant("Key_Dead_Voiced_Sound",    new QoreNode((int64)Qt::Key_Dead_Voiced_Sound));
   qt->addConstant("Key_Dead_Semivoiced_Sound", new QoreNode((int64)Qt::Key_Dead_Semivoiced_Sound));
   qt->addConstant("Key_Dead_Belowdot",        new QoreNode((int64)Qt::Key_Dead_Belowdot));
   qt->addConstant("Key_Dead_Hook",            new QoreNode((int64)Qt::Key_Dead_Hook));
   qt->addConstant("Key_Dead_Horn",            new QoreNode((int64)Qt::Key_Dead_Horn));
   qt->addConstant("Key_Back",                 new QoreNode((int64)Qt::Key_Back));
   qt->addConstant("Key_Forward",              new QoreNode((int64)Qt::Key_Forward));
   qt->addConstant("Key_Stop",                 new QoreNode((int64)Qt::Key_Stop));
   qt->addConstant("Key_Refresh",              new QoreNode((int64)Qt::Key_Refresh));
   qt->addConstant("Key_VolumeDown",           new QoreNode((int64)Qt::Key_VolumeDown));
   qt->addConstant("Key_VolumeMute",           new QoreNode((int64)Qt::Key_VolumeMute));
   qt->addConstant("Key_VolumeUp",             new QoreNode((int64)Qt::Key_VolumeUp));
   qt->addConstant("Key_BassBoost",            new QoreNode((int64)Qt::Key_BassBoost));
   qt->addConstant("Key_BassUp",               new QoreNode((int64)Qt::Key_BassUp));
   qt->addConstant("Key_BassDown",             new QoreNode((int64)Qt::Key_BassDown));
   qt->addConstant("Key_TrebleUp",             new QoreNode((int64)Qt::Key_TrebleUp));
   qt->addConstant("Key_TrebleDown",           new QoreNode((int64)Qt::Key_TrebleDown));
   qt->addConstant("Key_MediaPlay",            new QoreNode((int64)Qt::Key_MediaPlay));
   qt->addConstant("Key_MediaStop",            new QoreNode((int64)Qt::Key_MediaStop));
   qt->addConstant("Key_MediaPrevious",        new QoreNode((int64)Qt::Key_MediaPrevious));
   qt->addConstant("Key_MediaNext",            new QoreNode((int64)Qt::Key_MediaNext));
   qt->addConstant("Key_MediaRecord",          new QoreNode((int64)Qt::Key_MediaRecord));
   qt->addConstant("Key_HomePage",             new QoreNode((int64)Qt::Key_HomePage));
   qt->addConstant("Key_Favorites",            new QoreNode((int64)Qt::Key_Favorites));
   qt->addConstant("Key_Search",               new QoreNode((int64)Qt::Key_Search));
   qt->addConstant("Key_Standby",              new QoreNode((int64)Qt::Key_Standby));
   qt->addConstant("Key_OpenUrl",              new QoreNode((int64)Qt::Key_OpenUrl));
   qt->addConstant("Key_LaunchMail",           new QoreNode((int64)Qt::Key_LaunchMail));
   qt->addConstant("Key_LaunchMedia",          new QoreNode((int64)Qt::Key_LaunchMedia));
   qt->addConstant("Key_Launch0",              new QoreNode((int64)Qt::Key_Launch0));
   qt->addConstant("Key_Launch1",              new QoreNode((int64)Qt::Key_Launch1));
   qt->addConstant("Key_Launch2",              new QoreNode((int64)Qt::Key_Launch2));
   qt->addConstant("Key_Launch3",              new QoreNode((int64)Qt::Key_Launch3));
   qt->addConstant("Key_Launch4",              new QoreNode((int64)Qt::Key_Launch4));
   qt->addConstant("Key_Launch5",              new QoreNode((int64)Qt::Key_Launch5));
   qt->addConstant("Key_Launch6",              new QoreNode((int64)Qt::Key_Launch6));
   qt->addConstant("Key_Launch7",              new QoreNode((int64)Qt::Key_Launch7));
   qt->addConstant("Key_Launch8",              new QoreNode((int64)Qt::Key_Launch8));
   qt->addConstant("Key_Launch9",              new QoreNode((int64)Qt::Key_Launch9));
   qt->addConstant("Key_LaunchA",              new QoreNode((int64)Qt::Key_LaunchA));
   qt->addConstant("Key_LaunchB",              new QoreNode((int64)Qt::Key_LaunchB));
   qt->addConstant("Key_LaunchC",              new QoreNode((int64)Qt::Key_LaunchC));
   qt->addConstant("Key_LaunchD",              new QoreNode((int64)Qt::Key_LaunchD));
   qt->addConstant("Key_LaunchE",              new QoreNode((int64)Qt::Key_LaunchE));
   qt->addConstant("Key_LaunchF",              new QoreNode((int64)Qt::Key_LaunchF));
   qt->addConstant("Key_MediaLast",            new QoreNode((int64)Qt::Key_MediaLast));
   qt->addConstant("Key_Select",               new QoreNode((int64)Qt::Key_Select));
   qt->addConstant("Key_Yes",                  new QoreNode((int64)Qt::Key_Yes));
   qt->addConstant("Key_No",                   new QoreNode((int64)Qt::Key_No));
   qt->addConstant("Key_Cancel",               new QoreNode((int64)Qt::Key_Cancel));
   qt->addConstant("Key_Printer",              new QoreNode((int64)Qt::Key_Printer));
   qt->addConstant("Key_Execute",              new QoreNode((int64)Qt::Key_Execute));
   qt->addConstant("Key_Sleep",                new QoreNode((int64)Qt::Key_Sleep));
   qt->addConstant("Key_Play",                 new QoreNode((int64)Qt::Key_Play));
   qt->addConstant("Key_Zoom",                 new QoreNode((int64)Qt::Key_Zoom));
   qt->addConstant("Key_Context1",             new QoreNode((int64)Qt::Key_Context1));
   qt->addConstant("Key_Context2",             new QoreNode((int64)Qt::Key_Context2));
   qt->addConstant("Key_Context3",             new QoreNode((int64)Qt::Key_Context3));
   qt->addConstant("Key_Context4",             new QoreNode((int64)Qt::Key_Context4));
   qt->addConstant("Key_Call",                 new QoreNode((int64)Qt::Key_Call));
   qt->addConstant("Key_Hangup",               new QoreNode((int64)Qt::Key_Hangup));
   qt->addConstant("Key_Flip",                 new QoreNode((int64)Qt::Key_Flip));
   qt->addConstant("Key_unknown",              new QoreNode((int64)Qt::Key_unknown));

   // MatchFlag enum
   qt->addConstant("MatchExactly",             new QoreNode((int64)Qt::MatchExactly));
   qt->addConstant("MatchContains",            new QoreNode((int64)Qt::MatchContains));
   qt->addConstant("MatchStartsWith",          new QoreNode((int64)Qt::MatchStartsWith));
   qt->addConstant("MatchEndsWith",            new QoreNode((int64)Qt::MatchEndsWith));
   qt->addConstant("MatchRegExp",              new QoreNode((int64)Qt::MatchRegExp));
   qt->addConstant("MatchWildcard",            new QoreNode((int64)Qt::MatchWildcard));
   qt->addConstant("MatchFixedString",         new QoreNode((int64)Qt::MatchFixedString));
   qt->addConstant("MatchCaseSensitive",       new QoreNode((int64)Qt::MatchCaseSensitive));
   qt->addConstant("MatchWrap",                new QoreNode((int64)Qt::MatchWrap));
   qt->addConstant("MatchRecursive",           new QoreNode((int64)Qt::MatchRecursive));

   // ItemDataRole enum
   qt->addConstant("DisplayRole",              new QoreNode((int64)Qt::DisplayRole));
   qt->addConstant("DecorationRole",           new QoreNode((int64)Qt::DecorationRole));
   qt->addConstant("EditRole",                 new QoreNode((int64)Qt::EditRole));
   qt->addConstant("ToolTipRole",              new QoreNode((int64)Qt::ToolTipRole));
   qt->addConstant("StatusTipRole",            new QoreNode((int64)Qt::StatusTipRole));
   qt->addConstant("WhatsThisRole",            new QoreNode((int64)Qt::WhatsThisRole));
   qt->addConstant("FontRole",                 new QoreNode((int64)Qt::FontRole));
   qt->addConstant("TextAlignmentRole",        new QoreNode((int64)Qt::TextAlignmentRole));
   qt->addConstant("BackgroundColorRole",      new QoreNode((int64)Qt::BackgroundColorRole));
   qt->addConstant("BackgroundRole",           new QoreNode((int64)Qt::BackgroundRole));
   qt->addConstant("TextColorRole",            new QoreNode((int64)Qt::TextColorRole));
   qt->addConstant("ForegroundRole",           new QoreNode((int64)Qt::ForegroundRole));
   qt->addConstant("CheckStateRole",           new QoreNode((int64)Qt::CheckStateRole));
   qt->addConstant("AccessibleTextRole",       new QoreNode((int64)Qt::AccessibleTextRole));
   qt->addConstant("AccessibleDescriptionRole", new QoreNode((int64)Qt::AccessibleDescriptionRole));
   qt->addConstant("SizeHintRole",             new QoreNode((int64)Qt::SizeHintRole));
   qt->addConstant("UserRole",                 new QoreNode((int64)Qt::UserRole));

   // ItemFlag enum
   qt->addConstant("ItemIsSelectable",         new QoreNode((int64)Qt::ItemIsSelectable));
   qt->addConstant("ItemIsEditable",           new QoreNode((int64)Qt::ItemIsEditable));
   qt->addConstant("ItemIsDragEnabled",        new QoreNode((int64)Qt::ItemIsDragEnabled));
   qt->addConstant("ItemIsDropEnabled",        new QoreNode((int64)Qt::ItemIsDropEnabled));
   qt->addConstant("ItemIsUserCheckable",      new QoreNode((int64)Qt::ItemIsUserCheckable));
   qt->addConstant("ItemIsEnabled",            new QoreNode((int64)Qt::ItemIsEnabled));
   qt->addConstant("ItemIsTristate",           new QoreNode((int64)Qt::ItemIsTristate));
	
   // AspectRatioMode enum
   qt->addConstant("IgnoreAspectRatio",        new QoreNode((int64)Qt::IgnoreAspectRatio));
   qt->addConstant("KeepAspectRatio",          new QoreNode((int64)Qt::KeepAspectRatio));
   qt->addConstant("KeepAspectRatioByExpanding", new QoreNode((int64)Qt::KeepAspectRatioByExpanding));

   // TextFormat enum
   qt->addConstant("PlainText",                new QoreNode((int64)Qt::PlainText));
   qt->addConstant("RichText",                 new QoreNode((int64)Qt::RichText));
   qt->addConstant("AutoText",                 new QoreNode((int64)Qt::AutoText));
   qt->addConstant("LogText",                  new QoreNode((int64)Qt::LogText));

   // CursorShape enum
   qt->addConstant("ArrowCursor",              new QoreNode((int64)Qt::ArrowCursor));
   qt->addConstant("UpArrowCursor",            new QoreNode((int64)Qt::UpArrowCursor));
   qt->addConstant("CrossCursor",              new QoreNode((int64)Qt::CrossCursor));
   qt->addConstant("WaitCursor",               new QoreNode((int64)Qt::WaitCursor));
   qt->addConstant("IBeamCursor",              new QoreNode((int64)Qt::IBeamCursor));
   qt->addConstant("SizeVerCursor",            new QoreNode((int64)Qt::SizeVerCursor));
   qt->addConstant("SizeHorCursor",            new QoreNode((int64)Qt::SizeHorCursor));
   qt->addConstant("SizeBDiagCursor",          new QoreNode((int64)Qt::SizeBDiagCursor));
   qt->addConstant("SizeFDiagCursor",          new QoreNode((int64)Qt::SizeFDiagCursor));
   qt->addConstant("SizeAllCursor",            new QoreNode((int64)Qt::SizeAllCursor));
   qt->addConstant("BlankCursor",              new QoreNode((int64)Qt::BlankCursor));
   qt->addConstant("SplitVCursor",             new QoreNode((int64)Qt::SplitVCursor));
   qt->addConstant("SplitHCursor",             new QoreNode((int64)Qt::SplitHCursor));
   qt->addConstant("PointingHandCursor",       new QoreNode((int64)Qt::PointingHandCursor));
   qt->addConstant("ForbiddenCursor",          new QoreNode((int64)Qt::ForbiddenCursor));
   qt->addConstant("WhatsThisCursor",          new QoreNode((int64)Qt::WhatsThisCursor));
   qt->addConstant("BusyCursor",               new QoreNode((int64)Qt::BusyCursor));
   qt->addConstant("OpenHandCursor",           new QoreNode((int64)Qt::OpenHandCursor));
   qt->addConstant("ClosedHandCursor",         new QoreNode((int64)Qt::ClosedHandCursor));
   qt->addConstant("LastCursor",               new QoreNode((int64)Qt::LastCursor));
   qt->addConstant("BitmapCursor",             new QoreNode((int64)Qt::BitmapCursor));
   qt->addConstant("CustomCursor",             new QoreNode((int64)Qt::CustomCursor));

   // AnchorAttribute enum
   qt->addConstant("AnchorName",               new QoreNode((int64)Qt::AnchorName));
   qt->addConstant("AnchorHref",               new QoreNode((int64)Qt::AnchorHref));

   // DockWidgetArea enum
   qt->addConstant("LeftDockWidgetArea",       new QoreNode((int64)Qt::LeftDockWidgetArea));
   qt->addConstant("RightDockWidgetArea",      new QoreNode((int64)Qt::RightDockWidgetArea));
   qt->addConstant("TopDockWidgetArea",        new QoreNode((int64)Qt::TopDockWidgetArea));
   qt->addConstant("BottomDockWidgetArea",     new QoreNode((int64)Qt::BottomDockWidgetArea));
   qt->addConstant("DockWidgetArea_Mask",      new QoreNode((int64)Qt::DockWidgetArea_Mask));
   qt->addConstant("AllDockWidgetAreas",       new QoreNode((int64)Qt::AllDockWidgetAreas));
   qt->addConstant("NoDockWidgetArea",         new QoreNode((int64)Qt::NoDockWidgetArea));

   // DockWidgetAreaSizes enum
   qt->addConstant("NDockWidgetAreas",         new QoreNode((int64)Qt::NDockWidgetAreas));

   // ToolBarArea enum
   qt->addConstant("LeftToolBarArea",          new QoreNode((int64)Qt::LeftToolBarArea));
   qt->addConstant("RightToolBarArea",         new QoreNode((int64)Qt::RightToolBarArea));
   qt->addConstant("TopToolBarArea",           new QoreNode((int64)Qt::TopToolBarArea));
   qt->addConstant("BottomToolBarArea",        new QoreNode((int64)Qt::BottomToolBarArea));
   qt->addConstant("ToolBarArea_Mask",         new QoreNode((int64)Qt::ToolBarArea_Mask));
   qt->addConstant("AllToolBarAreas",          new QoreNode((int64)Qt::AllToolBarAreas));
   qt->addConstant("NoToolBarArea",            new QoreNode((int64)Qt::NoToolBarArea));

   // ToolBarSizes enum
   qt->addConstant("NToolBarAreas",            new QoreNode((int64)Qt::NToolBarAreas));

   qns->addInitialNamespace(qt);
}

static void qt_module_delete()
{
   if (C_Clipboard) {
      ExceptionSink xsink;
      C_Clipboard->deref(&xsink);
   }
}
