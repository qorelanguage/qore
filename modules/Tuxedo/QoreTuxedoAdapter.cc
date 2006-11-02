/*
  modules/Tuxedo/QoreTuxedoAdapter.cc

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
#include <qore/Object.h>
#include <qore/minitest.hpp>
#include <qore/ScopeGuard.h>
#include <qore/LockedObject.h>
#include <qore/charset.h>

#include <atmi.h>

#include "QoreTuxedoAdapter.h"

//------------------------------------------------------------------------------
namespace {

typedef std::vector<std::pair<std::string, std::string> > env_t;

struct EnvironmentSetter
{
  static LockedObject m_mutex; // needs to be static
  env_t m_old_env;
  EnvironmentSetter(const env_t& new_env);
  ~EnvironmentSetter();
};

EnvironmentSetter::EnvironmentSetter(const env_t& new_env)
{
  m_mutex.lock();

  for (env_t::const_iterator it = new_env.begin(), end = new_env.end(); it != end; ++it) {
    char* old = getenv(it->first.c_str());
    if (!old) old = "";
    m_old_env.push_back(std::make_pair(it->first, old));

    if (it->second.empty()) {
      unsetenv(it->first.c_str());
    } else {
      setenv(it->first.c_str(), it->second.c_str(), 1);
    }
  }
}

LockedObject EnvironmentSetter::m_mutex;

EnvironmentSetter::~EnvironmentSetter()
{
  for (env_t::const_iterator it = m_old_env.begin(), end = m_old_env.end(); it != end; ++it) {
    if (it->second.empty()) {
      unsetenv(it->first.c_str());
    } else {
      setenv(it->first.c_str(), it->second.c_str(), 1);
    }
  }

  m_mutex.unlock();
}

} // namespace

#ifdef DEBUG
TEST()
{
  using namespace std;
  vector<pair<string, string> > envs;
  envs.push_back(make_pair("test_aaa", "xyz"));
  envs.push_back(make_pair("test_bbb", ""));

  setenv("test_aaa", "old aaa", 1);
  setenv("test_bbb", "old bbb", 1);

  { 
    EnvironmentSetter setter(envs);
    char* s = getenv("test_aaa");
    assert(s);
    assert(!strcmp(s, "xyz"));
    s = getenv("test_bbb");
    if (s) assert(!s[0]);
  }
  char* s = getenv("test_aaa");
  assert(s);
  assert(!strcmp(s, "old aaa"));
  s = getenv("test_bbb");
  assert(s);
  assert(!strcmp(s, "old bbb"));
}
#endif

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter()
: m_connection_flags(0),
  m_send_buffer(0),
  m_send_buffer_size(0),
  m_receive_buffer(0),
  m_receive_buffer_size(0),
  m_string_encoding(QCS_DEFAULT)
{
  memset(&m_context, 0, sizeof(m_context));
}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
  resetReceiveBuffer();
  resetDataToSend();
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::getNeededAuthentication(int& out_auth) const
{
  EnvironmentSetter setter(m_env_variables);
  int res = tpchkauth();
  if (res == -1) return tperrno;
  out_auth = res;
  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::init() 
{
  EnvironmentSetter setter(m_env_variables);

  if (m_username.empty() && m_clientname.empty() && m_groupname.empty() &&
      m_password.empty() && m_connection_flags == 0 && m_binary_data.empty()) {
    return tpinit(0) == -1 ? tperrno : 0;
  }
  int size = sizeof(TPINIT) + m_binary_data.size();
  TPINIT* buff = (TPINIT*)tpalloc("TPINIT", 0, size);
  if (!buff) return tperrno;
  ON_BLOCK_EXIT(tpfree, (char*)buff);

  memset(buff, 0, size);
  if (!m_username.empty())   strcpy(buff->usrname, m_username.c_str());
  if (!m_clientname.empty()) strcpy(buff->cltname, m_clientname.c_str());
  if (!m_groupname.empty())  strcpy(buff->grpname, m_groupname.c_str());
  if (!m_password.empty())   strcpy(buff->passwd, m_password.c_str());
  buff->flags = m_connection_flags;
  buff->datalen = m_binary_data.size();
  if (!m_binary_data.empty()) {
    const void* data = &m_binary_data[0];
    memcpy(&buff->data, data, m_binary_data.size());
  }

  if (tpinit(buff) == -1) return tperrno;
  return saveContext();
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::close() 
{
  // cancel all remaining async requests, if any
  for (std::vector<int>::const_iterator it =  m_pending_async_calls.begin(), end = m_pending_async_calls.end(); it != end; ++it) {
    tpcancel(*it);
  }
  return (tpterm() == -1) ? tperrno : 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::saveContext()
{
  return tpgetctxt(&m_context, 0) == -1 ? tperrno : 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::switchToSavedContext() const
{
  return tpsetctxt(m_context, 0) == -1 ? tperrno : 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::allocateReceiveBuffer(char* type, char* subtype, long size)
{
  resetReceiveBuffer();

  m_receive_buffer = tpalloc(type, subtype, size);
  if (!m_receive_buffer) return tperrno;
  m_receive_buffer_size = size;
  return 0;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::resetDataToSend()
{
  if (m_send_buffer) {
    tpfree(m_send_buffer);
    m_send_buffer = 0;
  }
  if (m_send_buffer_size) m_send_buffer_size = 0;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::resetReceiveBuffer()
{
  if (m_receive_buffer) {
    tpfree(m_receive_buffer);
    m_receive_buffer = 0;
  }
  if (m_receive_buffer_size) m_receive_buffer_size = 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::setDataToSend(void* data, int data_size, char* type, char* subtype)
{
  resetDataToSend();
  m_send_buffer = tpalloc(type, subtype, data_size);
  if (!m_send_buffer) return tperrno;
  m_send_buffer_size = data_size;
  memcpy(m_send_buffer, data, data_size);
  return 0;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::setStringEncoding(char* name)
{
  m_string_encoding = QEM.findCreate(name);
  if (!m_string_encoding) m_string_encoding = QCS_DEFAULT;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::remove_pending_async_call(int handle)
{
  for (std::vector<int>::iterator it = m_pending_async_calls.begin(), end = m_pending_async_calls.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_calls.erase(it);
      break;
    }
  }
}

// EOF

