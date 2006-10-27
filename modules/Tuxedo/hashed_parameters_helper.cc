/*
  modules/Tuxedo/hashed_parameters_helper.cc

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
#include <qore/Exception.h>
#include <qore/Hash.h>
#include <memory>

#include "hashed_parameters_helper.h"

// The task of Tuxedo_hashed_parameters is to process
// has used to passed hash, set environment variables
// and later reset them back and provide TPINIT structure
// for tpinit() call. All this should be MT safe since
// several threads may want to join Tuxedo and setting
// environment variables is not MT safe.
//

// Changed to the environment are serialized to avoid problems
// with changes made by the other threads.
LockedObject Tuxedo_hashed_parameters::m_mutex;

const int foff = 1; // not really sure what is this good for

//------------------------------------------------------------------------------
Tuxedo_hashed_parameters::Tuxedo_hashed_parameters()
: m_tpinit_data(0)
{
  m_mutex.lock(); 
}

//------------------------------------------------------------------------------
Tuxedo_hashed_parameters::~Tuxedo_hashed_parameters()
{
  // restore previous environment
  for (unsigned i = 0, n = m_old_environment.size(); i != n; ++i) {
    if (m_old_environment[i].second.empty()) {
      unsetenv(m_old_environment[i].first.c_str());
    } else {
      setenv(m_old_environment[i].first.c_str(), m_old_environment[i].second.c_str(), 1);
    }
  }
  m_mutex.unlock();

  if (m_tpinit_data) {
    tpfree((char*)m_tpinit_data);
  }
}

//------------------------------------------------------------------------------
void Tuxedo_hashed_parameters::process_hash(QoreNode* params, ExceptionSink* xsink)
{
  // inspect all relevant parameters
  std::string username, clientname, password, groupname;
  long flags = 0;

  Hash* hash = params->val.hash;

  if (!set_string("username", hash, xsink, username)) {
    return;
  }
  if (!set_string("clientname", hash, xsink, clientname)) {
    return;
  }
  if (!set_string("password", hash, xsink, password)) {
    return;
  }
  if (!set_string("groupname", hash, xsink, groupname)) {
    return;
  }

  // all flags
  if (!set_flag("TPU_SIG", TPU_SIG, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPU_DIP", TPU_DIP, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPU_THREAD", TPU_THREAD, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPU_IGN", TPU_IGN, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPSA_FASTPATH", TPSA_FASTPATH, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPSA_PROTECTED", TPSA_PROTECTED, hash, xsink, flags)) {
    return;
  }
  if (!set_flag("TPMULTICONTEXTS", TPMULTICONTEXTS, hash, xsink, flags)) {
    return;
  }


  // all environment variables
  if (!set_environment_variable("TUXCONFIG", hash, xsink)) {
    return;
  } 
  if (!set_environment_variable("WSENVFILE", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSNADDR", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSFADDR", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSFRANGE", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSDEVICE", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSTYPE", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("WSRPLYMAX", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("TMMINENCRYPTBITS", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("TMMAXENCRYPTBITS", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("TUXDIR", hash, xsink)) {
    return;
  }
  if (!set_environment_variable("MACHINE_NAME", hash, xsink)) {
    return;
  }

  // does the TPINIT structure need to be allocated?
  // (keep this at the end of the function)
  if (flags || username.size() || clientname.size() || password.size() || groupname.size()) {
    m_tpinit_data = (tpinfo_t*)tpalloc("TPINIT", 0, sizeof(tpinfo_t));
    if (!m_tpinit_data) {
      xsink->raiseException("tpalloc()", "tpalloc() failed with error %d.", tperrno);
      return;
    }

    strcpy(m_tpinit_data->usrname, username.c_str() ? username.c_str() : "" );
    strcpy(m_tpinit_data->cltname, clientname.c_str() ? clientname.c_str() : "" );
    strcpy(m_tpinit_data->passwd, password.c_str() ? password.c_str() : "");
    strcpy(m_tpinit_data->grpname, groupname.c_str() ? groupname.c_str() : "");
    m_tpinit_data->flags = flags;
    m_tpinit_data->datalen = 0;
    m_tpinit_data->data = 0;
  }
}

//------------------------------------------------------------------------------
bool Tuxedo_hashed_parameters::set_environment_variable(char* name, Hash* params, ExceptionSink* xsink)
{
  QoreNode* n = params->getKeyValue(name);
  if (!n) return true;
  std::auto_ptr<QoreString> s(n->getAsString(foff, xsink));
  if (xsink->isException()) {
    return false;
  }
  if (!s.get()) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR", 
      "The parameter '%s' cannot be converted into string.", name);
    return false;
  }
  std::string ss;
  if (s->getBuffer() && s->getBuffer()[0]) { // remove quotes
    if (s->getBuffer()[0] == '"') {
      ss = s->getBuffer() + 1;
    } else {
      ss = s->getBuffer();
    }
    if (!ss.empty() && *ss.rbegin()  == '"') {
      ss.resize(ss.size() - 1);
    }
  }

  const char* old = getenv(name);
  m_old_environment.push_back(std::make_pair(name, old ? old : ""));
  setenv(name, ss.empty() ? "" : ss.c_str(), 1);
  return true;
}

//------------------------------------------------------------------------------
bool Tuxedo_hashed_parameters::set_flag(char* name, unsigned flag, Hash* params, ExceptionSink* xsink, long& flags)
{
  QoreNode* n = params->getKeyValue(name);
  if (!n) return true;

  bool val = n->boolEval(xsink);
  if (xsink->isException()) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "Parameter %s'' cannot be converted into boolean.", name);
    return false;
  }
  if (val) {
    flags |= flag;
  }
  return true;
}

//------------------------------------------------------------------------------
bool Tuxedo_hashed_parameters::set_string(char* name, Hash* params, ExceptionSink* xsink, std::string& str)
{
  QoreNode*  n = params->getKeyValue(name);
  if (!n) return true;

  std::auto_ptr<QoreString> s(n->getAsString(foff, xsink));
  if (xsink->isException()) {
    return false;
  }
  if (!s.get()) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "The parameter '%s' cannot be converted into string.", name);
    return false;
  }

  const char* ss = s->getBuffer();
  if (ss) { // remove quotes
    if (*ss == '"') {
      str = ss + 1;
    } else {
      str = ss;
    }
    if (!str.empty() && *str.rbegin() == '"') {
      str.resize(str.size() - 1);
    }
  }  

  if (str.size() > MAXTIDENT) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "The parameter '%s' can have max %d characters, current value has %d.", name, MAXTIDENT, str.size());
    return false;
  }
  return true;  
}

// EOF

