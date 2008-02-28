/*
 QC_QVariant.cc
 
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

#include "QC_QVariant.h"
#include "QC_QPoint.h"
#include "QC_QPointF.h"
#include "QC_QSize.h"
//#include "QC_QSizeF.h"
#include "QC_QLine.h"
#include "QC_QLineF.h"
#include "QC_QRect.h"
#include "QC_QRectF.h"
#include "QC_QLocale.h"
#include "QC_QByteArray.h"
#include "QC_QDateTime.h"
#include "QC_QDate.h"
#include "QC_QTime.h"
#include "QC_QChar.h"
#include "QC_QUrl.h"

#include "qore-qt.h"

qore_classid_t CID_QVARIANT;
class QoreClass *QC_QVariant = 0;

////QVariant ()
////QVariant ( Type type )
////QVariant ( int typeOrUserType, const void * copy )
////QVariant ( const QVariant & p )
////QVariant ( QDataStream & s )
////QVariant ( int int_val )
////QVariant ( bool val )
////QVariant ( double val )
////QVariant ( const QByteArray & qbytearray )
////QVariant ( const QBitArray & qbitarray )
////QVariant ( const QString & qstring )
////QVariant ( const QLatin1String & qlatin1string )
////QVariant ( const QStringList & qstringlist )
////QVariant ( const QDate & qdate )
////QVariant ( const QTime & qtime )
////QVariant ( const QDateTime & qdatetime )
////QVariant ( const QList<QVariant> & qvariantlist )
////QVariant ( const QMap<QString, QVariant> & qvariantmap )
////QVariant ( const QSize & qsize )
////QVariant ( const QSizeF & qsizef )
////QVariant ( const QPoint & qpoint )
////QVariant ( const QPointF & qpointf )
////QVariant ( const QLine & qline )
////QVariant ( const QLineF & qlinef )
////QVariant ( const QRect & qrect )
////QVariant ( const QRectF & qrectf )
////QVariant ( const QUrl & qurl )
////QVariant ( const QLocale & qlocale )
////QVariant ( const QRegExp & regExp )
////QVariant ( Qt::GlobalColor color )
static void QVARIANT_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   if (is_nothing(p)) {
      self->setPrivate(CID_QVARIANT, new QoreQVariant());
      return;
   }
   if (p && p->getType() == NT_OBJECT) {
      QoreQPoint *qpoint = (QoreQPoint *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINT, xsink);
      if (!qpoint) {
         QoreQPointF *qpointf = (QoreQPointF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QPOINTF, xsink);
         if (!qpointf) {
            QoreQLine *qline = (QoreQLine *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QLINE, xsink);
            if (!qline) {
               QoreQLineF *qlinef = (QoreQLineF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QLINEF, xsink);
               if (!qlinef) {
                  QoreQRect *qrect = (QoreQRect *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECT, xsink);
                  if (!qrect) {
                     QoreQRectF *qrectf = (QoreQRectF *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QRECTF, xsink);
                     if (!qrectf) {
                        QoreQUrl *qurl = (QoreQUrl *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QURL, xsink);
                        if (!qurl) {
                           QoreQLocale *qlocale = (QoreQLocale *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QLOCALE, xsink);
                           if (!qlocale) {
                              QoreQSize *qsize = (QoreQSize *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QSIZE, xsink);
                              if (!qsize) {
				 QoreQByteArray *qba = (QoreQByteArray *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QBYTEARRAY, xsink);
				 if (!qba) {
				    QoreQDateTime *qdt = (QoreQDateTime *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QDATETIME, xsink);
				    if (!qdt) {
				       QoreQDate *qd = (QoreQDate *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QDATE, xsink);
				       if (!qd) {
					  QoreQTime *qt = (QoreQTime *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QTIME, xsink);
					  if (!qt) {
					     QoreQChar *qchar = (QoreQChar *)(reinterpret_cast<const QoreObject *>(p))->getReferencedPrivateData(CID_QCHAR, xsink);
					     if (!qchar) {
						if (!xsink->isException())
						   xsink->raiseException("QVARIANT-CONSTRUCTOR-PARAM-ERROR", "QVariant::constructor() does not know how to handle arguments of class '%s' as passed as the first argument", (reinterpret_cast<const QoreObject *>(p))->getClassName());
						return;
					     }
					     ReferenceHolder<QoreQChar> qcharHolder(qchar, xsink);
					     self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QChar *>(qchar))));
					     return;
					  }
					  ReferenceHolder<QoreQTime> qtHolder(qt, xsink);
					  self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QTime *>(qt))));
					  return;
				       }
				       ReferenceHolder<QoreQDate> qdHolder(qd, xsink);
				       self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QDate *>(qd))));
				       return;
				    }
				    ReferenceHolder<QoreQDateTime> qdtHolder(qdt, xsink);
				    self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QDateTime *>(qdt))));
				    return;
				 }
				 ReferenceHolder<QoreQByteArray> qbaHolder(qba, xsink);
				 self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QByteArray *>(qba))));
				 return;
                              }
                              ReferenceHolder<QoreQSize> qsizeHolder(qsize, xsink);
                              self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QSize *>(qsize))));
                              return;
                           }
                           ReferenceHolder<QoreQLocale> qlocaleHolder(qlocale, xsink);
                           self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QLocale *>(qlocale))));
                           return;
                        }
                        ReferenceHolder<QoreQUrl> qurlHolder(qurl, xsink);
                        self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QUrl *>(qurl))));
                        return;
                     }
                     ReferenceHolder<QoreQRectF> qrectfHolder(qrectf, xsink);
                     self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QRectF *>(qrectf))));
                     return;
                  }
                  ReferenceHolder<QoreQRect> qrectHolder(qrect, xsink);
                  self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QRect *>(qrect))));
                  return;
               }
               ReferenceHolder<QoreQLineF> qlinefHolder(qlinef, xsink);
               self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QLineF *>(qlinef))));
               return;
            }
            ReferenceHolder<QoreQLine> qlineHolder(qline, xsink);
            self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QLine *>(qline))));
            return;
         }
         ReferenceHolder<QoreQPointF> qpointfHolder(qpointf, xsink);
         self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QPointF *>(qpointf))));
         return;
      }
      ReferenceHolder<QoreQPoint> qpointHolder(qpoint, xsink);
      self->setPrivate(CID_QVARIANT, new QoreQVariant(*(static_cast<QPoint *>(qpoint))));
      return;
   }
//   if (p && p->getType() == NT_???) {
//      ??? QDataStream s = p;
//      self->setPrivate(CID_QVARIANT, new QoreQVariant(s));
//      return;
//   }
   if (p && p->getType() == NT_STRING) {
      QString qstring;
      
      get_qstring(p, qstring, xsink);
      self->setPrivate(CID_QVARIANT, new QoreQVariant(qstring));
      return;
   }
//   if (p && p->getType() == NT_???) {
//      ??? QLatin1String qlatin1string = p;
//      self->setPrivate(CID_QVARIANT, new QoreQVariant(qlatin1string));
//      return;
//   }
//   if (p && p->getType() == NT_???) {
//      ??? QStringList qstringlist = p;
//      self->setPrivate(CID_QVARIANT, new QoreQVariant(qstringlist));
//      return;
//   }
   if (p && p->getType() == NT_DATE) {
      QDateTime qdatetime;
      if (get_qdatetime(p, qdatetime, xsink))
         return;
      self->setPrivate(CID_QVARIANT, new QoreQVariant(qdatetime));
      return;
   }
   if (p && p->getType() == NT_INT) {
      int int_val = p ? p->getAsInt() : 0;
      self->setPrivate(CID_QVARIANT, new QoreQVariant(int_val));
      return;
   }
   if (p && p->getType() == NT_BOOLEAN) {
      bool val = p ? p->getAsBool() : false;
      self->setPrivate(CID_QVARIANT, new QoreQVariant(val));
      return;
   }
//   if (p && p->getType() == NT_INT) {
//      QVariant::QMap<QString qmap<qstring = (QVariant::QMap<QString)(p ? p->getAsInt() : 0);
//   p = get_param(params, 1);
//   }
//   if (p && p->getType() == NT_INT) {
//      Qt::GlobalColor color = (Qt::GlobalColor)(p ? p->getAsInt() : 0);
//      self->setPrivate(CID_QVARIANT, new QoreQVariant(color));
//      return;
//   }
   if (p && p->getType() == NT_FLOAT) {
      double val = p ? p->getAsFloat() : 0.0;
      self->setPrivate(CID_QVARIANT, new QoreQVariant(val));
      return;
   }
   xsink->raiseException("QVARIANT-CONSTRUCTOR-ERROR", "don't know how to handle arguments of type '%s'", p ? p->getTypeName() : "NOTHING");
   return;
}

static void QVARIANT_copy(class QoreObject *self, class QoreObject *old, class QoreQVariant *qv, ExceptionSink *xsink)
{
   self->setPrivate(CID_QVARIANT, new QoreQVariant(*qv));
}

//bool canConvert ( Type t ) const
////bool canConvert () const
static AbstractQoreNode *QVARIANT_canConvert(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QVariant::Type t = (QVariant::Type)(p ? p->getAsInt() : 0);
   return get_bool_node(qv->canConvert(t));
}

//void clear ()
static AbstractQoreNode *QVARIANT_clear(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   qv->clear();
   return 0;
}

//bool convert ( Type t )
static AbstractQoreNode *QVARIANT_convert(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p = get_param(params, 0);
   QVariant::Type t = (QVariant::Type)(p ? p->getAsInt() : 0);
   return get_bool_node(qv->convert(t));
}

////DataPtr & data_ptr ()
//static AbstractQoreNode *QVARIANT_data_ptr(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->data_ptr());
//}

//bool isNull () const
static AbstractQoreNode *QVARIANT_isNull(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qv->isNull());
}

//bool isValid () const
static AbstractQoreNode *QVARIANT_isValid(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qv->isValid());
}

////void setValue ( const T & value )
//static AbstractQoreNode *QVARIANT_setValue(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? T value = p;
//   qv->setValue(value);
//   return 0;
//}

////QBitArray toBitArray () const
//static AbstractQoreNode *QVARIANT_toBitArray(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toBitArray());
//}

//bool toBool () const
static AbstractQoreNode *QVARIANT_toBool(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return get_bool_node(qv->toBool());
}

//QByteArray toByteArray () const
static AbstractQoreNode *QVARIANT_toByteArray(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qba = new QoreObject(QC_QByteArray, getProgram());
   if (!o_qba)
      return 0;
   QoreQByteArray *q_qba = new QoreQByteArray(qv->toByteArray());
   o_qba->setPrivate(CID_QBYTEARRAY, q_qba);
   return o_qba;
}

//QChar toChar () const
static AbstractQoreNode *QVARIANT_toChar(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *rv_str = new QoreStringNode(QCS_UTF8);
   QChar rv_qc = qv->toChar();
   rv_str->concatUTF8FromUnicode(rv_qc.unicode());
   return rv_str;
}

//QDate toDate () const
static AbstractQoreNode *QVARIANT_toDate(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QDate rv_date = qv->toDate();
   return new DateTimeNode(rv_date.year(), rv_date.month(), rv_date.day());
}

//QDateTime toDateTime () const
static AbstractQoreNode *QVARIANT_toDateTime(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QDateTime rv_dt = qv->toDateTime();
   QDate rv_d = rv_dt.date();
   QTime rv_t = rv_dt.time();
   return new DateTimeNode(rv_d.year(), rv_d.month(), rv_d.day(), rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

//double toDouble ( bool * ok = 0 ) const
static AbstractQoreNode *QVARIANT_toDouble(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   bool ok = false;
   double rc = qv->toDouble(&ok);
   if (!ok) {
      xsink->raiseException("QVARIANT-TODOUBLE-ERROR", "conversion to double failed");
      return 0;      
   }
   return new QoreFloatNode((double)rc);
}

//int toInt ( bool * ok = 0 ) const
static AbstractQoreNode *QVARIANT_toInt(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   bool ok = false;
   int64 rc = qv->toInt(&ok);
   if (!ok) {
      xsink->raiseException("QVARIANT-TOINT-ERROR", "conversion to integer failed");
      return 0;      
   }
   return new QoreBigIntNode(rc);
}

//QLine toLine () const
static AbstractQoreNode *QVARIANT_toLine(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLine, getProgram());
   if (!o_ql)
      return 0;
   QoreQLine *q_ql = new QoreQLine(qv->toLine());
   o_ql->setPrivate(CID_QLINE, q_ql);
   return o_ql;
}

//QLineF toLineF () const
static AbstractQoreNode *QVARIANT_toLineF(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qlf = new QoreObject(QC_QLineF, getProgram());
   if (!o_qlf)
      return 0;
   QoreQLineF *q_qlf = new QoreQLineF(qv->toLineF());
   o_qlf->setPrivate(CID_QLINEF, q_qlf);
   return o_qlf;
}

////QList<QVariant> toList () const
//static AbstractQoreNode *QVARIANT_toList(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toList());
//}

//QLocale toLocale () const
static AbstractQoreNode *QVARIANT_toLocale(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_ql = new QoreObject(QC_QLocale, getProgram());
   if (!o_ql)
      return 0;
   QoreQLocale *q_ql = new QoreQLocale(qv->toLocale());
   o_ql->setPrivate(CID_QLOCALE, q_ql);
   return o_ql;
}

////qlonglong toLongLong ( bool * ok = 0 ) const
//static AbstractQoreNode *QVARIANT_toLongLong(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   ??? return new QoreBigIntNode(qv->toLongLong(ok));
//}

////QMap<QString, QVariant> toMap () const
//static AbstractQoreNode *QVARIANT_toMap(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toMap());
//}

//QPoint toPoint () const
static AbstractQoreNode *QVARIANT_toPoint(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qp = new QoreObject(QC_QPoint, getProgram());
   QoreQPoint *q_qp = new QoreQPoint(qv->toPoint());
   o_qp->setPrivate(CID_QPOINT, q_qp);
   return o_qp;
}

//QPointF toPointF () const
static AbstractQoreNode *QVARIANT_toPointF(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qpf = new QoreObject(QC_QPointF, getProgram());
   QoreQPointF *q_qpf = new QoreQPointF(qv->toPointF());
   o_qpf->setPrivate(CID_QPOINTF, q_qpf);
   return o_qpf;
}

//QRect toRect () const
static AbstractQoreNode *QVARIANT_toRect(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qv->toRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//QRectF toRectF () const
static AbstractQoreNode *QVARIANT_toRectF(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qrf = new QoreObject(QC_QRectF, getProgram());
   QoreQRectF *q_qrf = new QoreQRectF(qv->toRectF());
   o_qrf->setPrivate(CID_QRECTF, q_qrf);
   return o_qrf;
}

////QRegExp toRegExp () const
//static AbstractQoreNode *QVARIANT_toRegExp(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toRegExp());
//}

//QSize toSize () const
static AbstractQoreNode *QVARIANT_toSize(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   if (!o_qs)
      return 0;
   QoreQSize *q_qs = new QoreQSize(qv->toSize());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return o_qs;
}

////QSizeF toSizeF () const
//static AbstractQoreNode *QVARIANT_toSizeF(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toSizeF());
//}

//QString toString () const
static AbstractQoreNode *QVARIANT_toString(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qv->toString().toUtf8().data(), QCS_UTF8);
}

////QStringList toStringList () const
//static AbstractQoreNode *QVARIANT_toStringList(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qv->toStringList());
//}

//QTime toTime () const
static AbstractQoreNode *QVARIANT_toTime(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QTime rv_t = qv->toTime();
   return new DateTimeNode(1970, 1, 1, rv_t.hour(), rv_t.minute(), rv_t.second(), rv_t.msec());
}

////uint toUInt ( bool * ok = 0 ) const
//static AbstractQoreNode *QVARIANT_toUInt(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   return new QoreBigIntNode(qv->toUInt(ok));
//}

////qulonglong toULongLong ( bool * ok = 0 ) const
//static AbstractQoreNode *QVARIANT_toULongLong(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
//{
//   const AbstractQoreNode *p = get_param(params, 0);
//   ??? bool* ok = p;
//   ??? return new QoreBigIntNode(qv->toULongLong(ok));
//}

//QUrl toUrl () const
static AbstractQoreNode *QVARIANT_toUrl(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qu = new QoreObject(QC_QUrl, getProgram());
   if (!o_qu)
      return 0;
   QoreQUrl *q_qu = new QoreQUrl(qv->toUrl());
   o_qu->setPrivate(CID_QURL, q_qu);
   return o_qu;
}

//Type type () const
static AbstractQoreNode *QVARIANT_type(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qv->type());
}

//const char * typeName () const
static AbstractQoreNode *QVARIANT_typeName(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   const char *c_rv = qv->typeName();
   if (!c_rv)
      return 0;
   return new QoreStringNode(c_rv);
}

//int userType () const
static AbstractQoreNode *QVARIANT_userType(QoreObject *self, QoreQVariant *qv, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qv->userType());
}

QoreClass *initQVariantClass()
{
   QC_QVariant = new QoreClass("QVariant", QDOM_GUI);
   CID_QVARIANT = QC_QVariant->getID();

   QC_QVariant->setConstructor(QVARIANT_constructor);
   QC_QVariant->setCopy((q_copy_t)QVARIANT_copy);

   QC_QVariant->addMethod("canConvert",                  (q_method_t)QVARIANT_canConvert);
   QC_QVariant->addMethod("clear",                       (q_method_t)QVARIANT_clear);
   QC_QVariant->addMethod("convert",                     (q_method_t)QVARIANT_convert);
   //QC_QVariant->addMethod("data_ptr",                    (q_method_t)QVARIANT_data_ptr);
   QC_QVariant->addMethod("isNull",                      (q_method_t)QVARIANT_isNull);
   QC_QVariant->addMethod("isValid",                     (q_method_t)QVARIANT_isValid);
   //QC_QVariant->addMethod("setValue",                    (q_method_t)QVARIANT_setValue);
   //QC_QVariant->addMethod("toBitArray",                  (q_method_t)QVARIANT_toBitArray);
   QC_QVariant->addMethod("toBool",                      (q_method_t)QVARIANT_toBool);
   QC_QVariant->addMethod("toByteArray",                 (q_method_t)QVARIANT_toByteArray);
   QC_QVariant->addMethod("toChar",                      (q_method_t)QVARIANT_toChar);
   QC_QVariant->addMethod("toDate",                      (q_method_t)QVARIANT_toDate);
   QC_QVariant->addMethod("toDateTime",                  (q_method_t)QVARIANT_toDateTime);
   QC_QVariant->addMethod("toDouble",                    (q_method_t)QVARIANT_toDouble);
   QC_QVariant->addMethod("toInt",                       (q_method_t)QVARIANT_toInt);
   QC_QVariant->addMethod("toLine",                      (q_method_t)QVARIANT_toLine);
   QC_QVariant->addMethod("toLineF",                     (q_method_t)QVARIANT_toLineF);
   //QC_QVariant->addMethod("toList",                      (q_method_t)QVARIANT_toList);
   QC_QVariant->addMethod("toLocale",                    (q_method_t)QVARIANT_toLocale);
   //QC_QVariant->addMethod("toLongLong",                  (q_method_t)QVARIANT_toLongLong);
   //QC_QVariant->addMethod("toMap",                       (q_method_t)QVARIANT_toMap);
   QC_QVariant->addMethod("toPoint",                     (q_method_t)QVARIANT_toPoint);
   QC_QVariant->addMethod("toPointF",                    (q_method_t)QVARIANT_toPointF);
   QC_QVariant->addMethod("toRect",                      (q_method_t)QVARIANT_toRect);
   QC_QVariant->addMethod("toRectF",                     (q_method_t)QVARIANT_toRectF);
   //QC_QVariant->addMethod("toRegExp",                    (q_method_t)QVARIANT_toRegExp);
   QC_QVariant->addMethod("toSize",                      (q_method_t)QVARIANT_toSize);
   //QC_QVariant->addMethod("toSizeF",                     (q_method_t)QVARIANT_toSizeF);
   QC_QVariant->addMethod("toString",                    (q_method_t)QVARIANT_toString);
   //QC_QVariant->addMethod("toStringList",                (q_method_t)QVARIANT_toStringList);
   QC_QVariant->addMethod("toTime",                      (q_method_t)QVARIANT_toTime);
   //QC_QVariant->addMethod("toUInt",                      (q_method_t)QVARIANT_toUInt);
   //QC_QVariant->addMethod("toULongLong",                 (q_method_t)QVARIANT_toULongLong);
   QC_QVariant->addMethod("toUrl",                       (q_method_t)QVARIANT_toUrl);
   QC_QVariant->addMethod("type",                        (q_method_t)QVARIANT_type);
   QC_QVariant->addMethod("typeName",                    (q_method_t)QVARIANT_typeName);
   QC_QVariant->addMethod("userType",                    (q_method_t)QVARIANT_userType);

   return QC_QVariant;
}
