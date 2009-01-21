/*
  AbstractThreadResource.h

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

#ifndef _QORE_ABSTRACTTHREADRESOURCE_H

#define _QORE_ABSTRACTTHREADRESOURCE_H

#include <qore/AbstractPrivateData.h>

//! base class for saving data using Qore's thread resource management system
/** Thread resources are resources that are tied to a particular thread.  Qore provides the ability to 
    call the object's "cleanup()" function if the resource is still allocated to the thread when the
    thread terminates.  For example, the Datasource transaction lock is implemented as a thread resource.
    If the used does not commit or rollback an open transaction before the thread terminates,
    ManagedDatasource::cleanup() is run, which will throw an exception, rollback the transaction, and 
    release the transaction lock.  When a thread commits or rolls back a transaction, the thread resource
    is removed.
    Use the set_thread_resource() to set and remove_thread_resource() to remove thread resources.
    @see set_thread_resource()
    @see remove_thread_resource()
 */
class AbstractThreadResource : public AbstractPrivateData
{
   public:
      //! virtual destructor
      DLLEXPORT virtual ~AbstractThreadResource()
      {
      }
      //! this function is called when a thread terminates and a thread resource is still allocated to the thread
      virtual void cleanup(class ExceptionSink *xsink) = 0;
};

#endif
