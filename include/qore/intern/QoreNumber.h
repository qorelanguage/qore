/*
  QoreNumber.h

  QoreNumber Class Definition for Arbitrary-precision numeric support

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

#ifndef _QORE_QORENUMBER_H

#define _QORE_QORENUMBER_H

#include <qore/common.h>

class QoreNumber {
   public:
      QoreNumber();
      QoreNumber(int);
      QoreNumber(int64);
      QoreNumber(double);
      QoreNumber(char *);
      ~QoreNumber();
      void add(QoreNumber *);
      void add(int);
      void add(double);
      void add(int64);
      void multiply(QoreNumber *);
      void multiply(int);
      void multiply(double);
      void multiply(int64);      
      void divide_by(QoreNumber *);
      void divide_by(int);
      void divide_by(double);
      void divide_by(int64);      
};

#include <qore/support.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

inline QoreNumber::QoreNumber()
{
}

inline QoreNumber::~QoreNumber()
{
}

inline QoreNumber::QoreNumber(char *str)
{
}

inline QoreNumber::QoreNumber(int i)
{
}

inline QoreNumber::QoreNumber(double f)
{
}

inline QoreNumber::QoreNumber(int64 i)
{
}

inline QoreNumber::~QoreNumber()
{
}

inline void QoreNumber::add(QoreNumber *v)
{
}

inline void QoreNumber::add(int i)
{
}

inline void QoreNumber::add(double d)
{
}

inline void QoreNumber::add(int64 i)
{
}

inline void QoreNumber::multiply(QoreNumber *v)
{
}

inline void QoreNumber::multiply(int i)
{
}

inline void QoreNumber::multiply(double d)
{
}

inline void QoreNumber::multiply(int64 i)
{
}

inline void QoreNumber::divide_by(QoreNumber *v)
{
}

inline void QoreNumber::divide_by(int i)
{
}

inline void QoreNumber::divide_by(double d)
{
}

inline void QoreNumber::divide_by(int64 i)
{
}

#endif
