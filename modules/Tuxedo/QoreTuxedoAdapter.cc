/*
  modules/Tuxedo/QoreTuxedoAdapter.cc

  Tuxedo integration to QORE

  Qore Programming Language

  Copyright (C) 2006 Qore Technologies

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
#include <qore/Exception.h>
#include <atmi.h>

#include "QoreTuxedoAdapter.h"
#include "tpalloc_helper.h"
#include "handle_error.h"

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter(const char* name, Hash* params, char* err, ExceptionSink* xsink)
: m_name(name && name[0] ? name : "<unnamed>")
{
}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
  ExceptionSink dummy;
  close_adapter("", &dummy);
}

//------------------------------------------------------------------------------
// Try to cancel all pending requests. Provide error details 
// on the last failed one.
void QoreTuxedoAdapter::close_adapter(char* err, ExceptionSink* xsink)
{
  unsigned failed_operations = 0;
  // discard all remaining asynchronous requests
  int async_tperrnum = 0;
  for (std::list<int>::const_iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    int res = tpcancel(*it);
    if (res == -1) {
      async_tperrnum = tperrno;
      ++failed_operations;
    }
  }
  m_pending_async_requests.clear();

  // forcibly close all opened conversations
  int conversation_tperrnum = 0;
  for (std::list<int>::const_iterator it = m_active_conversations.begin(), end = m_active_conversations.end(); it != end; ++it) {
    int res = tpdiscon(*it);
    if (res == -1) {
      conversation_tperrnum = tperrno;
      ++failed_operations;
    }
  }
  m_active_conversations.clear();

  if (!failed_operations) {
    return;
  }

  std::string func_name;
  if (async_tperrnum) {
    func_name = "tpcancel() of connection [";
  } else {
    func_name = "tpdiscon() of connection [";
  }
  func_name += m_name;
  func_name += "] (total number of failures = ";
  {
    char buffer[10];
    sprintf(buffer, "%u)", failed_operations);
    func_name += buffer;
  }
  handle_error(async_tperrnum ? async_tperrnum : conversation_tperrnum, err, func_name.c_str(), xsink);
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::call(char* service_name, List* params, long flags, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> in_buffer = list2buffer(params, err, xsink);
  if (xsink->isException()) {
    if (in_buffer.first) {
      tpfree(in_buffer.first);
    }
    return 0;
  }
  long out_size = 4096;
  char* out_buffer = tpalloc_helper("STRING", 0, out_size, err, xsink);
  if (xsink->isException()) {
    if (in_buffer.first) {
      tpfree(in_buffer.first);
    }
    if (out_buffer) {
      tpfree(out_buffer);
    }
    return 0;
  }
  int res = tpcall(service_name, in_buffer.first, in_buffer.second, &out_buffer, &out_size, flags);
  if (in_buffer.first) {
    tpfree(in_buffer.first);
  }

  if (res != -1) {
    List* result = buffer2list(out_buffer, out_size, err, xsink);
    if (out_buffer) {
      tpfree(out_buffer);
    }  
    return result;
  }
  // an error ocured
  std::string func_name = "tpcall() of connection [";
  func_name += m_name;
  func_name += "]";
  handle_error(tperrno, err, func_name.c_str(), xsink);
  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::async_call(char* service_name, List* params, long flags, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> buffer = list2buffer(params, err, xsink);
  if (xsink->isException()) {
    return 0;
  }
  int res = tpacall(service_name, buffer.first, buffer.second, flags);

  if (res != -1) {
    if (buffer.first) {
      tpfree(buffer.first);
    }

    if (res) { // 0 means no response 
      m_pending_async_requests.push_back(res);
    }
    return res;
  }
  // error occured
  std::string func_name = "tpacall() of connection [";
  func_name += m_name;
  func_name += "]";
  handle_error(tperrno, err, func_name.c_str(), xsink);
  return 0;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::cancel_async(int handle, char* err, ExceptionSink* xsink)
{
  for (std::list<int>::iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_requests.erase(it);
      if (tpcancel(handle) != -1) {
        return;
      }
      std::string func_name = "tpcancel() of connection [";
      func_name += m_name;
      func_name += "]";
      handle_error(tperrno, err, func_name.c_str(), xsink);
      return;
    }
  }
  xsink->raiseException(err, "Invalid handle value for cancel_async().");
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::get_async_result(int handle, long flags, char* err, ExceptionSink* xsink)
{
  long size = 4096;
  char* out_buffer = tpalloc_helper("STRING", 0, size, err, xsink);
  if (xsink->isException()) {
    if (out_buffer) {
      tpfree(out_buffer);
    }
    return 0;
  }
  // remove the handle from those to be cabceled when the object gets destroyed
  for (std::list<int>::iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_requests.erase(it);
      break;
    } 
  }

  int res = tpgetrply(&handle, &out_buffer, &size, flags);
  if (res != -1) {
    List* result = buffer2list(out_buffer, size, err, xsink);
    if (out_buffer) {
      tpfree(out_buffer);
    }
    if (xsink->isException()) {
      delete result;
      return 0;
    }
    return result;
  }
  // error occured
  std::string func_name = "tpgetrply() of connection [";
  func_name += m_name;
  func_name += "]";
  handle_error(tperrno, err, func_name.c_str(), xsink);

  if (out_buffer) {
    tpfree(out_buffer);
  }
  return 0;
}

//-----------------------------------------------------------------------------
int QoreTuxedoAdapter::connect(char* service_name, List* initial_data, long flags, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> buffer = std::make_pair((char*)0, 0);
  if (initial_data) {
    buffer = list2buffer(initial_data, err, xsink);
    if (xsink->isException()) {
      return 0;
    }
  }
  int res = tpconnect(service_name, buffer.first, buffer.second, flags);
  if (res == -1) {
    std::string func_name = "tpconnect(\"";
    func_name += service_name ? service_name : "";
    func_name += "\") of connection [";
    func_name += m_name;
    func_name += "]";
    handle_error(tperrno, err, func_name.c_str(), xsink);
    return 0;
  }

  m_active_conversations.push_back(res);
  return res;
}

//-----------------------------------------------------------------------------
void QoreTuxedoAdapter::forced_disconnect(int handle, char* err, ExceptionSink* xsink)
{
  for (std::list<int>::iterator it = m_active_conversations.begin(), end = m_active_conversations.end(); it != end; ++it) {
    if (*it == handle) {
      m_active_conversations.erase(it);
      break;
    }
  }
  int res = tpdiscon(handle);
  if (res != -1) {
     return;
  }
  handle_error(tperrno, err, "tpdicson()", xsink);
}

//-----------------------------------------------------------------------------
std::string QoreTuxedoAdapter::conversation_event2string(long event)
{
  switch (event) {
  case 0: return std::string();
  case TPEV_DISCONIMM: return "(event TPEV_DISCONIMM - server disconnected abruptly)";
  case TPEV_SVCERR: return "(event TPEV_SVCERR - server error)";
  case TPEV_SVCFAIL: return "(event TPEV_SVCFAIL - server failure)";
  case TPEV_SVCSUCC: return "(event TPEV_SVCSUCC - conversation closed OK)";
  case TPEV_SENDONLY: return "(event TPEV_SENDONLY - not in receive mode)";
  default:
    {
      char buffer[100];
      sprintf(buffer, "(unknwon event %ld)", event);
      return std::string(buffer);
    }
    break;
  }
}
//-----------------------------------------------------------------------------
bool QoreTuxedoAdapter::send(int handle, List* data, long flags, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> buffer = list2buffer(data, err, xsink);
  if (xsink->isException()) {
    if (buffer.first) {
      tpfree(buffer.first);
    }
    return true;
  }
  long event = 0;
  int res = tpsend(handle, buffer.first, buffer.second, flags, &event);
  int tperrnum = tperrno;
  if (buffer.first) {
    tpfree(buffer.first);
  }

  if (res != -1 && (event == 0 || event == TPEV_SVCSUCC)) {
    return event == TPEV_SVCSUCC;
  }
  
  std::string func_name = "tpsend()";
  if (event != 0) {
    func_name += conversation_event2string(event);
  }
  func_name += " of connection [";
  func_name += m_name;
  func_name += "]";
  handle_error(tperrnum, err, func_name.c_str(), xsink);
  return true;
}

//-----------------------------------------------------------------------------
std::pair<bool, List*> QoreTuxedoAdapter::recv(int handle, long flags, char* err, ExceptionSink* xsink)
{
  // TBD
  return std::make_pair(false, (List*)0);
}

//-----------------------------------------------------------------------------
std::pair<char*, long> QoreTuxedoAdapter::list2buffer(List* list, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> result = std::make_pair((char*)0, 0);
  int sz = list->size();
  if (sz < 2) {
    xsink->raiseException(err, "Parameter list must have at least two elements (it has %d now).", sz);
    return result;
  }
  QoreNode* pname = list->retrieve_entry(0);
  if (pname->type != NT_STRING) {
    xsink->raiseException(err, "The first parameter must be a type of data (in form of string), Example: \"XML\".");
    return result;
  }
  std::string name = pname->val.String->getBuffer() ? pname->val.String->getBuffer() : "";
  // according to the type process the rest of the string
  if (name == "string") {
    return string_list2buffer(list, err, xsink);
  }
  
  xsink->raiseException(err, "Uknown type of data (the first parameter): [%s].", name.c_str() ? name.c_str() : "");  
  return result;
}

//------------------------------------------------------------------------------
std::pair<char*, long> QoreTuxedoAdapter::string_list2buffer(List* list, char* err, ExceptionSink* xsink)
{
  std::pair<char*, long> result = std::make_pair((char*)0, 0);
  int sz = list->size();
  if (sz != 2) {
    xsink->raiseException(err, "Parameter list with string type must have exactly two parameters.");
    return result;
  }
  int len = 0;
  char* data = 0;
  
  QoreNode* pdata = list->retrieve_entry(1);
  if (pdata->type == NT_STRING) {
    len = pdata->val.String->strlen() + 1;
    data = pdata->val.String->getBuffer();
  } else 
  if (pdata->type == NT_BINARY) {
    len = pdata->val.bin->size();
    data = (char*)pdata->val.bin->getPtr();
  } else {
    xsink->raiseException(err, "The second parameter needs tobe either string or binary.");
    return result;
  }

  char* out_buffer = tpalloc_helper("STRING", 0, len, err, xsink);
  if (xsink->isException()) {
    if (out_buffer) {
      tpfree(out_buffer);
    }
    return result;
  }
  memcpy(out_buffer, data, len);

  result.first = out_buffer;
  result.second = len;

  return result;
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::buffer2list(char* buffer, long size, char* err, ExceptionSink* xsink)
{
  char type[20];
  char subtype[20];
  long res = tptypes(buffer, type, subtype);
  if (res == -1) {
    xsink->raiseException(err, "tptypes() failed with tperrno = %d. Internal error of Tuxedo interface.\n", tperrno);
    return 0;
  }

  // handle each type

  if (strcmp(type, "STRING") == 0) {
    QoreNode* head = new QoreNode("string");
    QoreString* s = new QoreString(buffer, size, QCS_DEFAULT);
    QoreNode* tail = new QoreNode(s);
    List* result = new List();
    result->push(head);
    result->push(tail);
    return result;
  }

  xsink->raiseException(err, "Received data type [%s] is not handled.\n", type);
  return 0;
}

// EOF

