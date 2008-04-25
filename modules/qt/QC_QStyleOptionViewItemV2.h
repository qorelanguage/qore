/*
 QC_QStyleOptionViewItemV2.h
 
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

#ifndef _QORE_QT_QC_QSTYLEOPTIONVIEWITEMV2_H

#define _QORE_QT_QC_QSTYLEOPTIONVIEWITEMV2_H

#include <QStyleOptionViewItemV2>

DLLLOCAL extern qore_classid_t CID_QSTYLEOPTIONVIEWITEMV2;
DLLLOCAL extern class QoreClass *QC_QStyleOptionViewItemV2;

DLLLOCAL class QoreClass *initQStyleOptionViewItemV2Class(QoreClass *);

class QoreQStyleOptionViewItemV2 : public AbstractPrivateData, public QStyleOptionViewItemV2
{
   public:
      DLLLOCAL QoreQStyleOptionViewItemV2() : QStyleOptionViewItemV2()
      {
      }
      DLLLOCAL QoreQStyleOptionViewItemV2(const QStyleOptionViewItemV2 &sovi) : QStyleOptionViewItemV2(sovi)
      {
      }
      DLLLOCAL QoreQStyleOptionViewItemV2(const QStyleOptionViewItem& other) : QStyleOptionViewItemV2(other)
      {
      }
};

#endif // _QORE_QT_QC_QSTYLEOPTIONVIEWITEMV2_H
