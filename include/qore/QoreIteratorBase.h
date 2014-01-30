/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreIteratorBase.h

  abstract class for private data for iterators in objects

  Qore Programming Language

  Copyright 2003 - 2014 David Nichols

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

#ifndef _QORE_QOREITERATORBASE_H

#define _QORE_QOREITERATORBASE_H

#include <qore/AbstractPrivateData.h>

DLLEXPORT extern QoreClass* QC_ABSTRACTITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTBIDIRECTIONALITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTQUANTIFIEDBIDIRECTIONALITERATOR;
DLLEXPORT extern QoreClass* QC_ABSTRACTQUANTIFIEDITERATOR;

class QoreAbstractIteratorBase {
protected:
   int tid;

public:
   //! creates the object and marks it as owned by the current thread
   DLLEXPORT QoreAbstractIteratorBase();

   DLLEXPORT virtual ~QoreAbstractIteratorBase();

   //! checks for a valid operation, returns 0 if OK, -1 if not (exception thrown)
   DLLEXPORT int check(ExceptionSink* xsink) const;

   //! returns the name of the current iterator class
   DLLEXPORT virtual const char* getName() const = 0;
};

//! abstract base class for iterator private data
class QoreIteratorBase : public AbstractPrivateData, public QoreAbstractIteratorBase {
public:
   //! creates the object and marks it as owned by the current thread
   DLLEXPORT QoreIteratorBase();
};

#endif
