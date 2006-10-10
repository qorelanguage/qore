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
#include <utility>

class ExceptionSink;

//------------------------------------------------------------------------------
class QoreTuxedoAdapter : public ReferenceObject
{
  std::string m_name; // identification of the connection, could be empty
  std::list<int> m_pending_async_requests;
  std::list<int> m_active_conversations;

  std::pair<char*, long> list2buffer(List* list, char* err, ExceptionSink* xsink);
  std::pair<char*, long> string_list2buffer(List* list, char* err, ExceptionSink* xsink);
  std::pair<char*, long> binary_list2buffer(List* list, char* err, ExceptionSink* xsink);
  std::pair<char*, long> xml_list2buffer(List* list, char* err, ExceptionSink* xsink);

  List* buffer2list(char* buffer, long size, char* err, ExceptionSink* xsink);

  std::string conversation_event2string(long event);
  void remove_active_conversation(int handle);

public:
  QoreTuxedoAdapter(const char* name, Hash* params, char* err, ExceptionSink* xsink);
  ~QoreTuxedoAdapter();
  const char* get_name() const { return m_name.c_str(); }
  void close_adapter(char* err, ExceptionSink* xsink);

  List* call(char* service_name, List* params, long flags, char* err, ExceptionSink* xsink);
  int async_call(char* service_name, List* params, long flags, char* err, ExceptionSink* xsink);
  void cancel_async(int handle, char* err, ExceptionSink* xsink);
  List* get_async_result(int handle, long flags, char* err, ExceptionSink* xsink);

  int connect(char* service_name, List* initial_data, long flags, char* err, ExceptionSink* xsink);
  void forced_disconnect(int handle, char* err, ExceptionSink* xsink);
  // if true is returned then the conversation ended 
  bool send(int handle, List* data, long flags, char* err, ExceptionSink* xsink);
  // if the first item is true then the conversation ended, second item is received data (as list)
  std::pair<bool, List*> recv(int handle, long flags, char* err, ExceptionSink* xsink);

  void deref() { 
    if (ROdereference()) {
      delete this;
    }
  }
};

#endif

// EOF

