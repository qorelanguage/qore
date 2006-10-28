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
#include <qore/minitest.hpp>

#include <memory>

#ifdef DEBUG
#  define private public
#endif

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

  set_string("username", hash, xsink, username);
  if (xsink->isException()) {
    return;
  }
  set_string("clientname", hash, xsink, clientname);
  if (xsink->isException()) {
    return;
  }
  set_string("password", hash, xsink, password);
  if (xsink->isException()) {
    return;
  }
  set_string("groupname", hash, xsink, groupname);
  if (xsink->isException()) {
    return;
  }

  // all flags
  set_flag("TPU_SIG", TPU_SIG, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPU_DIP", TPU_DIP, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPU_THREAD", TPU_THREAD, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPU_IGN", TPU_IGN, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPSA_FASTPATH", TPSA_FASTPATH, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPSA_PROTECTED", TPSA_PROTECTED, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }
  set_flag("TPMULTICONTEXTS", TPMULTICONTEXTS, hash, xsink, flags);
  if (xsink->isException()) {
    return;
  }

  // all environment variables
  set_environment_variable("TUXCONFIG", hash, xsink);
  if (xsink->isException()) {
    return;
  } 
  set_environment_variable("WSENVFILE", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSNADDR", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSFADDR", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSFRANGE", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSDEVICE", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSTYPE", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("WSRPLYMAX", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("TMMINENCRYPTBITS", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("TMMAXENCRYPTBITS", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("TUXDIR", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("MACHINE_NAME", hash, xsink);
  if (xsink->isException()) {
    return;
  }
  set_environment_variable("TMNOTHREADS", hash, xsink);
  if (xsink->isException()) {
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
void Tuxedo_hashed_parameters::set_environment_variable(char* name, Hash* params, ExceptionSink* xsink)
{
  QoreNode* n = params->getKeyValue(name);
  if (!n) {
    return;
  }
  if (n->type != NT_STRING && n->type != NT_INT) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "The parameter '%s' needs to be either string or integer.", name);
    return;
  }

  std::string ss;
  if (n->type == NT_STRING) {
    char* s = n->val.String->getBuffer();
    if (!s) s = "";
    ss = s;
  } else
  if (n->type == NT_INT) {
    char buffer[10];
    sprintf(buffer, "%d", (int)n->val.intval);
    ss = buffer;
  }

  if (!ss.empty() && ss[0] == '"') { // remove quotes
    ss.erase(ss.begin());
    if (!ss.empty() && *ss.rbegin()  == '"') {
      ss.resize(ss.size() - 1);
    }
  }

  const char* old = getenv(name);
  m_old_environment.push_back(std::make_pair(name, old ? old : ""));
  setenv(name, ss.empty() ? "" : ss.c_str(), 1);
}

//------------------------------------------------------------------------------
void Tuxedo_hashed_parameters::set_flag(char* name, unsigned flag, Hash* params, ExceptionSink* xsink, long& flags)
{
  QoreNode* n = params->getKeyValue(name);
  if (!n) {
    return;
  }
  if (n->type != NT_BOOLEAN) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "Parameter %s'' needs to be a boolean.", name);
    return;

  }
  bool val = n->val.boolval;

  if (val) {
    flags |= flag;
  }
}

//------------------------------------------------------------------------------
void Tuxedo_hashed_parameters::set_string(char* name, Hash* params, ExceptionSink* xsink, std::string& str)
{
  QoreNode*  n = params->getKeyValue(name);
  if (!n) {
    return;
  }
  if (n->type != NT_STRING) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "The parameter '%s' needs to be a  string.", name);
    return;
  }
  char* s = n->val.String->getBuffer();
  if (!s) s = "";
  str = s;

  if (str.size() > MAXTIDENT) {
    xsink->raiseException("QORE-TUXEDO-CONNECTION-CONSTRUCTOR",
      "The parameter '%s' can have max %d characters, current value has %d.", name, MAXTIDENT, str.size());
  }
}

//-----------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  // test instantiation
  Tuxedo_hashed_parameters thp;
}

TEST()
{
  // test set_environment_variable()
  Tuxedo_hashed_parameters thp;
  ExceptionSink xsink;

  Hash* h = new Hash();
  h->setKeyValue("abc", new QoreNode("value abc"), &xsink);
  h->setKeyValue("defgh", new QoreNode("value defgh"), &xsink);
  h->setKeyValue("xyz", new QoreNode((int64)123), &xsink);
  h->setKeyValue("quoted", new QoreNode("\"quoted text\""), &xsink);
  h->setKeyValue("binary", new QoreNode(new BinaryObject()), &xsink);
  assert(!xsink.isException());

  setenv("abc", "xxx", 1);
  setenv("defgh", "xxx", 1);
  setenv("xyz", "xxx", 1);
  setenv("quoted", "xxx", 1);
  setenv("binary", "xxx", 1);

  thp.set_environment_variable("nonexistent", h, &xsink);
  assert(!xsink.isException()); // if value is not in hash (= not set by user) it won't be put into env

  char* s = getenv("nonexistent");
  assert(!s);

  thp.set_environment_variable("abc", h, &xsink);
  assert(!xsink.isException());

  s = getenv("abc");
  assert(strcmp(s, "value abc") == 0);

  thp.set_environment_variable("defgh", h, &xsink);
  assert(!xsink.isException());

  s = getenv("defgh");
  assert(strcmp(s, "value defgh") == 0); 

  thp.set_environment_variable("xyz", h, &xsink);
  assert(!xsink.isException());

  s = getenv("xyz");
  assert(strcmp(s, "123") == 0);

  thp.set_environment_variable("quoted", h, &xsink);
  assert(!xsink.isException());

  s = getenv("quoted");
  assert(strcmp(s, "quoted text") == 0); // needs to be unquoted

  thp.set_environment_variable("binary", h, &xsink);
  assert(xsink.isException());
  xsink.clear();

  s = getenv("binary");
  assert(strcmp(s, "xxx") == 0);

  // this strange trickery is needed because Hash cannot be deleted when it has items inside !?!
  QoreNode* aux = new QoreNode(h);
  aux->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test set_flag()
  Tuxedo_hashed_parameters thp;
  ExceptionSink xsink;

  Hash* h = new Hash();
  h->setKeyValue("abc", new QoreNode(true), &xsink);
  h->setKeyValue("defgh", new QoreNode(false), &xsink);
  h->setKeyValue("xyz", new QoreNode((int64)123), &xsink);
  assert(!xsink.isException());

  long flags = 0;
  thp.set_flag("abc", 123, h, &xsink, flags);
  assert(!xsink.isException());
  assert(flags == 123);
  
  flags = 0;
  thp.set_flag("defgh", 321, h, &xsink, flags);
  assert(!xsink.isException());
  assert(flags == 0); // because of false

  thp.set_flag("xyz", 11, h, &xsink, flags);
  assert(xsink.isException());
  xsink.clear();
  assert(flags == 0);

  thp.set_flag("nonexistent", 999, h, &xsink, flags);
  assert(!xsink.isException());
  assert(flags == 0);

  // this strange trickery is needed because Hash cannot be deleted when it has items inside !?!
  QoreNode* aux = new QoreNode(h);
  aux->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  // test set_string()
  Tuxedo_hashed_parameters thp;
  ExceptionSink xsink;

  Hash* h = new Hash();
  h->setKeyValue("abc", new QoreNode("abc"), &xsink);
  h->setKeyValue("defgh", new QoreNode("defgh"), &xsink);
  h->setKeyValue("xyz", new QoreNode((int64)123), &xsink);
  assert(!xsink.isException());

  std::string str;
  
  thp.set_string("abc", h, &xsink, str);
  assert(!xsink.isException());
  assert(str == "abc");

  thp.set_string("defgh", h, &xsink, str);
  assert(!xsink.isException());
  assert(str == "defgh"); 

  str = "";
  thp.set_string("xyz", h, &xsink, str);
  assert(xsink.isException());
  xsink.clear();
  assert(str == "");

  thp.set_string("nonexistent", h, &xsink, str);
  assert(!xsink.isException());
  assert(str == "");

  // this strange trickery is needed because Hash cannot be deleted when it has items inside !?!
  QoreNode* aux = new QoreNode(h);
  aux->deref(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  using std::string;

  // keep previous environment settings
  char* s = getenv("TUXCONFIG"); if (!s) s = "";
  string TUXCONFIG = s;
  s = getenv("WSENVFILE"); if (!s) s = "";
  string WSENVFILE = s;
  s = getenv("WSNADDR"); if (!s) s = "";
  string WSNADDR = s;
  s = getenv("WSFADDR"); if (!s) s = "";
  string WSFADDR = s;
  s = getenv("WSFRANGE"); if (!s) s = "";
  string WSFRANGE = s;
  s = getenv("WSDEVICE"); if (!s) s = "";
  string WSDEVICE = s;
  s = getenv("WSTYPE"); if (!s) s = "";
  string WSTYPE = s;
  s = getenv("WSRPLYMAX"); if (!s) s = "";
  string WSRPLYMAX = s;
  s = getenv("TMMINENCRYPTBITS"); if (!s) s = "";
  string TMMINENCRYPTBITS = s;
  s = getenv("TMMAXENCRYPTBITS"); if (!s) s = "";
  string TMMAXENCRYPTBITS = s;
  s = getenv("TUXDIR"); if (!s) s = "";
  string TUXDIR = s;
  s = getenv("MACHINE_NAME"); if (!s) s = "";
  string MACHINE_NAME = s;
  s = getenv("TMNOTHREADS"); if (!s) s = "";
  string TMNOTHREADS = s;
 
  {
    // test when all parameters are set
    Tuxedo_hashed_parameters thp;
    ExceptionSink xsink;

    Hash* h = new Hash();
    // flags
    h->setKeyValue("TPU_SIG", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPU_DIP", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPU_THREAD", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPU_IGN", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPU_SIG", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPSA_FASTPATH", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPSA_PROTECTED", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TPMULTICONTEXTS", new QoreNode(true), &xsink);
    assert(!xsink.isException());
    // tpinit values
    h->setKeyValue("username", new QoreNode("my username"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("clientname", new QoreNode("my clientname"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("password", new QoreNode("my password"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("groupname", new QoreNode("my groupname"), &xsink);
    assert(!xsink.isException());
    // environment variables
    h->setKeyValue("TUXDIR", new QoreNode("my TUXDIR"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSENVFILE", new QoreNode("my WSENVFILE"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSNADDR", new QoreNode("my WSNADDR"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSFADDR", new QoreNode("my WSFADDR"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSFRANGE", new QoreNode("my WSFRANGE"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSDEVICE", new QoreNode("my WSDEVICE"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSTYPE", new QoreNode("my WSTYPE"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("WSRPLYMAX", new QoreNode("my WSRPLYMAX"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TMMINENCRYPTBITS", new QoreNode("my TMMINENCRYPTBITS"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TMMAXENCRYPTBITS", new QoreNode("my TMMAXENCRYPTBITS"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TUXCONFIG", new QoreNode("my TUXCONFIG"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("MACHINE_NAME", new QoreNode("my MACHINE_NAME"), &xsink);
    assert(!xsink.isException());
    h->setKeyValue("TMNOTHREADS", new QoreNode("my TMNOTHREADS"), &xsink);
    assert(!xsink.isException());

    QoreNode* params = new QoreNode(h);

    thp.process_hash(params, &xsink);
    assert(!xsink.isException());
   
    params->deref(&xsink);
    assert(!xsink.isException());

    s = getenv("TUXDIR");
    assert(strcmp(s, "my TUXDIR") == 0);
    s = getenv("WSENVFILE");
    assert(strcmp(s, "my WSENVFILE") == 0);
    s = getenv("WSNADDR");
    assert(strcmp(s, "my WSNADDR") == 0);
    s = getenv("WSFADDR");
    assert(strcmp(s, "my WSFADDR") == 0);
    s = getenv("WSFRANGE");
    assert(strcmp(s, "my WSFRANGE") == 0);
    s = getenv("WSDEVICE"); 
    assert(strcmp(s, "my WSDEVICE") == 0);
    s = getenv("WSTYPE");
    assert(strcmp(s, "my WSTYPE") == 0);
    s = getenv("WSRPLYMAX");
    assert(strcmp(s, "my WSRPLYMAX") == 0);
    s = getenv("TMMINENCRYPTBITS");
    assert(strcmp(s, "my TMMINENCRYPTBITS") == 0);
    s = getenv("TMMAXENCRYPTBITS");
    assert(strcmp(s, "my TMMAXENCRYPTBITS") == 0);
    s = getenv("MACHINE_NAME");
    assert(strcmp(s, "my MACHINE_NAME") == 0);
    s = getenv("TMNOTHREADS");
    assert(strcmp(s, "my TMNOTHREADS") == 0);

    TPINIT* init = thp.get_tpinit_data();
    assert(init);
    assert(strcmp(init->usrname, "my username") == 0);
    assert(strcmp(init->cltname, "my clientname") == 0);
    assert(strcmp(init->passwd, "my password") == 0);
    assert(strcmp(init->grpname, "my groupname") == 0);

    if (!(init->flags & TPU_SIG)) {
      assert(false);
    }
    if (!(init->flags & TPU_DIP)) {
      assert(false);
    }
    if (!(init->flags & TPU_THREAD)) {
      assert(false);
    }
    if (!(init->flags & TPU_IGN)) {
      assert(false);
    }
    if (!(init->flags & TPSA_FASTPATH)) {
      assert(false);
    }    
    if (!(init->flags & TPSA_PROTECTED)) {
      assert(false);
    }
    if (!(init->flags & TPMULTICONTEXTS)) {
      assert(false);
    }
  }   

  { //-------------------------------------------------
    // test just some env is set
    Tuxedo_hashed_parameters thp;
    ExceptionSink xsink;

    Hash* h = new Hash();
    h->setKeyValue("MACHINE_NAME", new QoreNode("my MACHINE_NAME"), &xsink);
    assert(!xsink.isException());

    QoreNode* params = new QoreNode(h);

    thp.process_hash(params, &xsink);
    assert(!xsink.isException());

    params->deref(&xsink);
    assert(!xsink.isException());

    s = getenv("MACHINE_NAME");
    assert(strcmp(s, "my MACHINE_NAME") == 0);

    TPINIT* init = thp.get_tpinit_data();
    assert(!init); // no TPINIT values set
  }
 
  // put back original env variables, just in case
  setenv("TUXCONFIG", TUXCONFIG.c_str(), 1);
  setenv("WSENVFILE", WSENVFILE.c_str(), 1);
  setenv("WSNADDR", WSNADDR.c_str(), 1);
  setenv("WSFADDR", WSFADDR.c_str(), 1);
  setenv("WSFRANGE", WSFRANGE.c_str(), 1);
  setenv("WSDEVICE", WSDEVICE.c_str(), 1);
  setenv("WSTYPE", WSTYPE.c_str(), 1);
  setenv("WSRPLYMAX", WSRPLYMAX.c_str(), 1);
  setenv("TMMINENCRYPTBITS", TMMINENCRYPTBITS.c_str(), 1);
  setenv("TMMAXENCRYPTBITS", TMMAXENCRYPTBITS.c_str(), 1);
  setenv("TUXDIR", TUXDIR.c_str(), 1);
  setenv("MACHINE_NAME", MACHINE_NAME.c_str(), 1);
  setenv("TMNOTHREADS", TMNOTHREADS.c_str(), 1);
}

#endif // DEBUG

// EOF

