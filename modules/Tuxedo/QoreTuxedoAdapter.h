#ifndef QORE_TUXEDO_ADAPTER_IMPL_H_
#define QORE_TUXEDO_ADAPTER_IMPL_H_

/*
  modules/Tuxedo/QoreTuxedoAdapter.h

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 QoreTechnologies

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

#include <qore/common.h>
#include <qore/support.h>
#include <qore/ReferenceObject.h>
#include <string>
#include <list>

class ExceptionSink;

//------------------------------------------------------------------------------
class QoreTuxedoAdapter : public ReferenceObject
{
  std::string m_name; // identification of the connection, could be empty
  std::list<int> m_pending_async_requests;

  void handle_tpcancel_error(char* err, int tperrnum, int handle, ExceptionSink* xsink);
  char* hash2buffer(Hash* hash, ExceptionSink* xsink);
  Hash* buffer2hash(char* buffer, ExceptionSink* xsink);

public:
  QoreTuxedoAdapter(const char* name, Hash* params, ExceptionSink* xsink);
  ~QoreTuxedoAdapter();
  const char* name() const { return m_name.c_str(); }
  void close_adapter(ExceptionSink* xsink);

  Hash* call(char* service_name, Hash* params, long flags, ExceptionSink* xsink);
  int async_call(char* service_name, Hash* params, long flags, ExceptionSink* xsink);
  void cancel_async(int handle, ExceptionSink* xsink);
  Hash* get_async_result(int handle, Hash* out, long flags, ExceptionSink* xsink);

  void deref() { 
    if (ROdereference()) {
      delete this;
    }
  }
};

#endif

// EOF

