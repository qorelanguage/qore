/*
  AbstractPrivateData.h

  abstract class for private data in objects

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

#ifndef _QORE_ABSTRACTPRIVATEDATA_H

#define _QORE_ABSTRACTPRIVATEDATA_H

#include <qore/QoreReferenceCounter.h>

//! the base class for all data to be used as private data of Qore objects
/** C++ constructor code for Qore classes must set private data of the class
    against the class ID using QoreObject::setPrivate()
 */
class AbstractPrivateData : public QoreReferenceCounter
{
   protected:
      //! as these objects are reference counted, the destructor should be called only when the reference count = 0 and not manually
      DLLEXPORT virtual ~AbstractPrivateData() {}

   public:
      //! increments the reference count of the object
      DLLEXPORT void ref()
      {
	 ROreference();
      }

      //! decrements the reference count of the object
      /**
	 @param xsink any Qore-language exception information is stored here
       */
      DLLEXPORT virtual void deref(class ExceptionSink *xsink)
      {
	 if (ROdereference())
	    delete this;
      }

      //! decrements the reference count of the object without the possibility of throwing a Qore-language exception
      DLLEXPORT virtual void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

#endif // _QORE_ABSTRACTPRIVATEDATA_H
