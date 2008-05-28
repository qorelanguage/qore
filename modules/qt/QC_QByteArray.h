/*
 QC_QByteArray.h
 
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

#ifndef _QORE_QT_QC_QBYTEARRAY_H

#define _QORE_QT_QC_QBYTEARRAY_H

#include <QByteArray>

DLLEXPORT extern qore_classid_t CID_QBYTEARRAY;
DLLEXPORT extern class QoreClass *QC_QByteArray;

DLLEXPORT class QoreClass *initQByteArrayClass();

class QoreQByteArray : public AbstractPrivateData, public QByteArray
{
   public:
      DLLLOCAL QoreQByteArray() : QByteArray()
      {
      }
      DLLLOCAL QoreQByteArray(const char* str) : QByteArray(str)
      {
      }
      DLLLOCAL QoreQByteArray(const char* data, int size) : QByteArray(data, size)
      {
      }
      DLLLOCAL QoreQByteArray(int size, char ch) : QByteArray(size, ch)
      {
      }
      DLLLOCAL QoreQByteArray(const QByteArray& other) : QByteArray(other)
      {
      }
};

#endif // _QORE_QT_QC_QBYTEARRAY_H
