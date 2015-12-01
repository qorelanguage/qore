/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  AbstractThreadResource.h

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
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
class AbstractThreadResource : public AbstractPrivateData {
public:
   //! the constructor is currently empty
   DLLEXPORT AbstractThreadResource();

   //! virtual destructor
   DLLEXPORT virtual ~AbstractThreadResource();

   //! this function is called when a thread terminates and a thread resource is still allocated to the thread
   virtual void cleanup(ExceptionSink* xsink) = 0;
};

#endif
