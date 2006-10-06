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

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter(const char* name, Hash* params, ExceptionSink* xsink)
: m_name(name && name[0] ? name : "<unnamed>")
{
}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
  ExceptionSink dummy;
  close_adapter(&dummy);
}

//------------------------------------------------------------------------------
// Try to cancel all pending requests. Provide error details 
// on the last failed one.
void QoreTuxedoAdapter::close_adapter(ExceptionSink* xsink)
{
  bool some_cancel_failed = false;
  int tperrnum;
  int handle;
  for (std::list<int>::const_iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    int res = tpcancel(*it);
    if (res == -1) {
      some_cancel_failed = true;
      tperrnum = tperrno;
      handle = *it;
    }
  }
  m_pending_async_requests.clear();
  if (some_cancel_failed) {
    handle_tpcancel_error("QORE-TUXEDO-ADAPTER-DESTRUCTOR", tperrnum, handle, xsink);
  }
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::call(char* service_name, List* params, long flags, ExceptionSink* xsink)
{
  
  char* err = "QORE-TUXEDO-ADAPTER-CALL";
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
printf("### calling tpcall() now\n");
  int res = tpcall(service_name, in_buffer.first, in_buffer.second, &out_buffer, &out_size, flags);
  if (in_buffer.first) {
    tpfree(in_buffer.first);
  }

  if (res != -1) {
printf("### some data retrieved\n");
    List* result = buffer2list(out_buffer, out_size, err, xsink);
    if (out_buffer) {
      tpfree(out_buffer);
    }  
    return result;
  }
  // an error ocured
  switch (tperrno) {
  case TPEINVAL:
    xsink->raiseException(err, "tpcall() returned TPEINVAL (invalid arguments).");
    break;
  case TPENOENT:
    xsink->raiseException(err, "tpcall() returned TPENOENT (invalid or non-existent service).");
    break;
  case TPEITYPE:
    xsink->raiseException(err, "tpcall() returned TPEITYPE (invalid input type or subtype).");
    break;
  case TPEOTYPE:
    xsink->raiseException(err, "tpcall() returned TPEOTYPE (invalid output type or subtype).");
    break;
  case TPETRAN:
    xsink->raiseException(err, "tpcall() returned TPETRAN (transaction not supported).");
    break;
  case TPETIME:
    xsink->raiseException(err, "tpcall() returned TPETIME (timeout or transcation rolled back).");
    break;
  case TPESVCFAIL:
    xsink->raiseException(err, "tpcall() returned TPESVCFAIL (service failed).");
    break;
  case TPESVCERR:
    xsink->raiseException(err, "tpcall() returned TPESVCERR (service error).");
    break;
  case TPEBLOCK:
    xsink->raiseException(err, "tpcall() returned TPEBLOCK (blocking condition).");
    break;
  case TPGOTSIG:
    xsink->raiseException(err, "tpcall() returned TPGOTSIG (signal received).");
    break;
  case TPEPROTO:
    xsink->raiseException(err, "tpcall() returned TPEPROTO (called improperly).");
    break;
  case TPESYSTEM:
    xsink->raiseException(err, "tpcall() returned TPESYSTEM (Tuxedo error, see Tuxedo log for details).");
    break;
  case TPEOS:
    xsink->raiseException(err, "tpcall() returned TPEOS (OS error). errno = %d.", errno);
    break;
  default:
    xsink->raiseException(err, "tpcall() returned unknown error %d.", tperrno);
    break;
  }

  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::async_call(char* service_name, List* params, long flags, ExceptionSink* xsink)
{
  std::pair<char*, long> buffer = list2buffer(params, "QORE-TUXEDO-ADAPTER-ASYNC_CALL", xsink);
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
printf("#### tpacall returns handle %d.\n", res);
    return res;
  }

  char* err = "QORE-TUXEDO-ADAPTER-ASYNC_CALL";
  switch (tperrno) {
  case TPEINVAL:
    xsink->raiseException(err, "tpacall() returned TPEINVAL error: invalid arguments.");
    break;
  case TPENOENT:
    xsink->raiseException(err, "tpacall() returned TPENOENT error: non-existent or conversational service.");
    break;
  case TPEITYPE:
    xsink->raiseException(err, "tpacall() returned TPEITYPE error: invalid type or subtype");
    break;
  case TPELIMIT:
    xsink->raiseException(err, "tpacall() returned TPELIMIT error: too many outstanding async requests.");
    break;
  case TPETRAN:
    xsink->raiseException(err, "tpacall() returned TPETRAN error: service doesn't support transactions.");
    break;
  case TPETIME:
    xsink->raiseException(err,"tpacall() returned TPETIME error: timeout or transaction already rolled back.");
    break;
  case TPEBLOCK:
    xsink->raiseException(err, "tpacall() returned TPEBLOCK error: the call would block.");
    break;
  case TPGOTSIG:
    xsink->raiseException(err, "tpacall() returned TPGOTSIG error: a signal was received.");
    break;
  case TPEPROTO:
    xsink->raiseException(err, "tpacall() returned TPEPROTO error: called improperly.");
    break;
  case TPESYSTEM:
    xsink->raiseException(err, "tpacall() returned TPESYSTEM error: Tuxedo problem, check Tuxedo log for details.");
    break;
  case TPEOS:
    xsink->raiseException(err, "tpacall() returned TPEOS error: OS problem, errno = %d.", errno);
    break;
  default:
    xsink->raiseException(err, "tpacall() returned unrecognized error %d.", tperrno);
    break;
  }

  if (buffer.first) {
    tpfree(buffer.first);
  }

  return 0;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::cancel_async(int handle, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-CANCEL_ASYNC";

  for (std::list<int>::iterator it = m_pending_async_requests.begin(), end = m_pending_async_requests.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_requests.erase(it);
      if (tpcancel(handle) != -1) {
        return;
      }
      handle_tpcancel_error(err, tperrno, handle, xsink);
    }
  }
  xsink->raiseException(err, "Invalid handle value.");
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::handle_tpcancel_error(char* err, int tperrnum, int handle, ExceptionSink* xsink)
{
  switch (tperrnum) {
  case TPEBADDESC:
    xsink->raiseException(err, "tpcancel() returned TPEBADDESC error. Value %d is not valid handle.", handle);
    return;
  case TPETRAN:
    xsink->raiseException(err, "tpcancel() returned TPETRAC error: the handle is associated with Tuxedo transaction.");
    return;
  case TPEPROTO:
    xsink->raiseException(err, "tpcancel() returned TPEPROTO error: called improperly.");
    return;
  case TPESYSTEM:
    xsink->raiseException(err, "tpcancel() returned TPESYSTEM error: Tuxedo problem, see Tuxedo log for details.");
    return;
  case TPEOS:
    xsink->raiseException(err, "tpcancel() returned TPEOS error: operating system error, errno = %d", errno);
    return;
  default:
    xsink->raiseException(err, "tpcancel() returned unknown error %d.", tperrnum);
    return;
  }
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::get_async_result(int handle, long flags, ExceptionSink* xsink)
{
  char* err = "QORE-TUXEDO-ADAPTER-GET_ASYNC_RESULT";
  long size = 4096;
  char* out_buffer = tpalloc_helper("STRING", 0, size, err, xsink);
  if (xsink->isException()) {
    if (out_buffer) {
      tpfree(out_buffer);
    }
    return 0;
  }
printf("### buffer = %p\n", out_buffer);
  int res = tpgetrply(&handle, &out_buffer, &size, flags);
printf("##### tpgetrply returned %d, tperrno = %d\n", res, tperrno);
printf("#### detail = %d\n", tperrordetail(0));
printf("### handle = %d\n", handle);
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

  switch (tperrno) {
  case TPEINVAL: 
    xsink->raiseException(err, "tpgetrply() returned TPEINVAL (invalid arguments).");
    break;
  case TPEOTYPE:
    xsink->raiseException(err, "tpgetrply() returned TPEOTYPE (invalid type or subtype).");
    break;
  case TPEBADDESC:
    xsink->raiseException(err, "tpgetrply() returned TPEBADDESC (invalid handle).");
    break;
  case TPETIME:
    xsink->raiseException(err, "tpgetrply() returned TPETIME (timeout or transaction rolled back).");
    break;
  case TPESVCFAIL:
    xsink->raiseException(err, "tpgetrply() returned TPESVCFAIL (service failed).");
    break;
  case TPESVCERR:
    xsink->raiseException(err, "tpgetrply() returned TPESVCERR (service error).");
    break;
  case TPEBLOCK:
    xsink->raiseException(err, "tpgetrply() returned TPEBLOCK (blocking condition).");
    break;
  case TPGOTSIG:
    xsink->raiseException(err, "tpgetrply() returned TPGOTSIG (signal received).");
    break; 
  case TPEPROTO:
    xsink->raiseException(err, "tpgetrply() returned TPEPROTO (called improperly).");
    break;
  case TPESYSTEM:
    xsink->raiseException(err, "tpgetrply() returned TPESYSTEM (Tuxedo error, see Tuxedo log for details).");
    break;
  case TPEOS:
    xsink->raiseException(err, "tpgetrply() returned TPEOS (OR error). errno = %d.", errno);
    break;
  default:
    xsink->raiseException(err, "tpgetrply() returned unknown error %d.", tperrno);
    break;
  }

 // an error occured
  if (out_buffer) {
    tpfree(out_buffer);
  }

  return 0;
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
printf("### string read [%s]\n", buffer);
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

