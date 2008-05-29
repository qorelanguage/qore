/*
 QC_QVariant.h
 
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

#ifndef _QORE_QT_QC_QVARIANT_H

#define _QORE_QT_QC_QVARIANT_H

#include <QVariant>

DLLEXPORT extern qore_classid_t CID_QVARIANT;
DLLEXPORT extern class QoreClass *QC_QVariant;

DLLEXPORT class QoreClass *initQVariantClass();

class QoreQVariant : public AbstractPrivateData, public QVariant
{
   public:
      DLLLOCAL QoreQVariant() : QVariant()
      {
      }
      DLLLOCAL QoreQVariant(Type type) : QVariant(type)
      {
      }
      DLLLOCAL QoreQVariant(int typeOrUserType, const void* copy) : QVariant(typeOrUserType, copy)
      {
      }
      DLLLOCAL QoreQVariant(const QVariant& p) : QVariant(p)
      {
      }
      DLLLOCAL QoreQVariant(QDataStream& s) : QVariant(s)
      {
      }
      DLLLOCAL QoreQVariant(int int_val) : QVariant(int_val)
      {
      }
      DLLLOCAL QoreQVariant(bool val) : QVariant(val)
      {
      }
      DLLLOCAL QoreQVariant(double val) : QVariant(val)
      {
      }
      DLLLOCAL QoreQVariant(const QByteArray& qbytearray) : QVariant(qbytearray)
      {
      }
      DLLLOCAL QoreQVariant(const QBitArray& qbitarray) : QVariant(qbitarray)
      {
      }
      DLLLOCAL QoreQVariant(const QString& qstring) : QVariant(qstring)
      {
      }
      DLLLOCAL QoreQVariant(const QChar& qchar) : QVariant(qchar)
      {
      }
      DLLLOCAL QoreQVariant(const QLatin1String& qlatin1string) : QVariant(qlatin1string)
      {
      }
      DLLLOCAL QoreQVariant(const QStringList& qstringlist) : QVariant(qstringlist)
      {
      }
      DLLLOCAL QoreQVariant(const QDate& qdate) : QVariant(qdate)
      {
      }
      DLLLOCAL QoreQVariant(const QTime& qtime) : QVariant(qtime)
      {
      }
      DLLLOCAL QoreQVariant(const QDateTime& qdatetime) : QVariant(qdatetime)
      {
      }
      DLLLOCAL QoreQVariant(const QList<QVariant>& qvariantlist) : QVariant(qvariantlist)
      {
      }
/*
      DLLLOCAL QoreQVariant(const QMap<QString, QVariant>& qvariantmap) : QVariant(qmap<qstring, qvariantmap)
      {
      }
*/
      DLLLOCAL QoreQVariant(const QSize& qsize) : QVariant(qsize)
      {
      }
      DLLLOCAL QoreQVariant(const QSizeF& qsizef) : QVariant(qsizef)
      {
      }
      DLLLOCAL QoreQVariant(const QPoint& qpoint) : QVariant(qpoint)
      {
      }
      DLLLOCAL QoreQVariant(const QPointF& qpointf) : QVariant(qpointf)
      {
      }
      DLLLOCAL QoreQVariant(const QLine& qline) : QVariant(qline)
      {
      }
      DLLLOCAL QoreQVariant(const QLineF& qlinef) : QVariant(qlinef)
      {
      }
      DLLLOCAL QoreQVariant(const QRect& qrect) : QVariant(qrect)
      {
      }
      DLLLOCAL QoreQVariant(const QRectF& qrectf) : QVariant(qrectf)
      {
      }
      DLLLOCAL QoreQVariant(const QUrl& qurl) : QVariant(qurl)
      {
      }
      DLLLOCAL QoreQVariant(const QLocale& qlocale) : QVariant(qlocale)
      {
      }
      DLLLOCAL QoreQVariant(const QRegExp& regExp) : QVariant(regExp)
      {
      }
      DLLLOCAL QoreQVariant(Qt::GlobalColor color) : QVariant(color)
      {
      }
};

#endif // _QORE_QT_QC_QVARIANT_H
