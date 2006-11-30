/*
  AbstractPrivateData.h

  abstract class for private data in objects

  Qore Programming Language

  Copyright (C) 2004, 2005, 2006 David Nichols

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

#include <qore/config.h>
#include <qore/ReferenceObject.h>

class AbstractPrivateData : public ReferenceObject
{
   protected:
      virtual ~AbstractPrivateData() {}

   public:
      void ref()
      {
	 ROreference();
      }
      virtual void deref(class ExceptionSink *xsink)
      {
	 if (ROdereference())
	    delete this;
      }
      virtual void deref()
      {
	 if (ROdereference())
	    delete this;
      }
};

#endif // _QORE_ABSTRACTPRIVATEDATA_H
