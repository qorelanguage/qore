/*
  qt-core.cc
  
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

#include "QC_QByteArray.h"
#include "QC_QChar.h"
#include "QC_QChildEvent.h"
#include "QC_QDate.h"
#include "QC_QDateTime.h"
#include "QC_QTime.h"
#include "QC_QTimerEvent.h"
#include "QC_QVariant.h"
#include "QC_QEvent.h"
#include "QC_QLine.h"
#include "QC_QLineF.h"
#include "QC_QLocale.h"
#include "QC_QObject.h"
#include "QC_QPoint.h"
//#include "QC_QPointArray.h"
#include "QC_QPointF.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QRegExp.h"
#include "QC_QSize.h"
//#include "QC_QSizeF.h"
//#include "QC_QStringList.h"
#include "QC_QUrl.h"

typedef safe_dslist<return_qvariant_hook_t> qv_hook_list_t;
typedef safe_dslist<return_qobject_hook_t> qo_hook_list_t;
typedef safe_dslist<return_qevent_hook_t> qe_hook_list_t;

static qv_hook_list_t qvariant_hooks; 
static qo_hook_list_t qobject_hooks;
static qe_hook_list_t qevent_hooks;

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

QoreObject *return_object(const QoreClass *qclass, AbstractPrivateData *data)
{
   QoreObject *qore_object = new QoreObject(qclass, getProgram());
   qore_object->setPrivate(qclass->getID(), data);
   return qore_object;
}

AbstractQoreNode *return_qvariant(const QVariant &qv)
{
   QVariant::Type type = qv.type();
   switch (type) {
      case QVariant::Invalid:
	 return nothing();
      case QVariant::Bool:
	 return get_bool_node(qv.toBool());
      case QVariant::Date: 
	 return new DateTimeNode(qv.toDate().year(), qv.toDate().month(), qv.toDate().day());
      case QVariant::DateTime: {
	 QDate rv_d = qv.toDateTime().date();
	 QTime rv_t = qv.toDateTime().time();
	 return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
      }
      case QVariant::Double:
	 return new QoreFloatNode(qv.toDouble());
      case QVariant::Int:
	 return new QoreBigIntNode(qv.toInt());
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
      case QVariant::Point:
         return return_object(QC_QPoint, new QoreQPoint(qv.value<QPoint>()));
      //case QVariant::PointArray:
         //return return_object(QC_QPointArray, new QoreQPointArray(qv.value<QPointArray>()));
      case QVariant::PointF:
         return return_object(QC_QPointF, new QoreQPointF(qv.value<QPointF>()));
      case QVariant::Rect:
         return return_object(QC_QRect, new QoreQRect(qv.value<QRect>()));
      case QVariant::RectF:
         return return_object(QC_QRectF, new QoreQRectF(qv.value<QRectF>()));
      case QVariant::RegExp:
         return return_object(QC_QRegExp, new QoreQRegExp(qv.value<QRegExp>()));
      case QVariant::Size:
         return return_object(QC_QSize, new QoreQSize(qv.value<QSize>()));
      //case QVariant::SizeF:
         //return return_object(QC_QSizeF, new QoreQSizeF(qv.value<QSizeF>()));
      case QVariant::String:
	 return new QoreStringNode(qv.toString().toUtf8().data(), QCS_UTF8);
      //case QVariant::StringList:
         //return return_object(QC_QStringList, new QoreQStringList(qv.value<QStringList>()));
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
	 break;
   }

   // try return_qvariant hooks
   for (qv_hook_list_t::iterator i = qvariant_hooks.begin(), e = qvariant_hooks.end(); i != e; ++i) {
      AbstractQoreNode *rv = (*i)(qv);
      if (rv)
	 return rv;
   }

   return return_object(QC_QVariant, new QoreQVariant(qv));
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

   // try return_qobject hooks
   for (qo_hook_list_t::iterator i = qobject_hooks.begin(), e = qobject_hooks.end(); i != e; ++i) {
      QoreObject *rv = (*i)(o);
      if (rv)
	 return rv;
   }

   // assign as QObject
   qo = new QoreObject(QC_QObject, getProgram());
   qo->setPrivate(CID_QOBJECT, new QoreQtQObject(qo, o));
   return qo;
}

QoreObject *return_qevent(QEvent *event)
{
   if (!event)
      return 0;

   // the order is important here so the most specific subclass is checked before any base classes
   {
      QChildEvent *qce = dynamic_cast<QChildEvent *>(event);
      if (qce)
         return return_object(QC_QChildEvent, new QoreQChildEvent(*qce));
   }
   {
      QTimerEvent *qte = dynamic_cast<QTimerEvent *>(event);
      if (qte)
         return return_object(QC_QTimerEvent, new QoreQTimerEvent(*qte));
   }

   // try return_qevent hooks
   for (qe_hook_list_t::iterator i = qevent_hooks.begin(), e = qevent_hooks.end(); i != e; ++i) {
      QoreObject *rv = (*i)(event);
      if (rv)
	 return rv;
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
