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
#include <qore/QoreType.h>
#include <qore/QoreNode.h>

#include <atmi.h>
#include <fml32.h>
#include <fml.h>

#include <memory>
#include <stdio.h>

#include "QoreTuxedoAdapter.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::auto_ptr;

namespace {

typedef vector<pair<string, string> > env_t;

//------------------------------------------------------------------------------
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
    m_old_env.push_back(make_pair(it->first, old));

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

//------------------------------------------------------------------------------
// Set environment variables needed to parse FML definitions.
class FmlEnvironmentSetter
{
private:
  auto_ptr<EnvironmentSetter> m_setter;

  void set(const string& value, bool is_fml32) {
    env_t new_env;
    new_env.push_back(make_pair(is_fml32 ? "FIELDTBLS32" : "FIELDTBLS", value));
    // all file names are expected to be absolute paths
    new_env.push_back(make_pair(is_fml32 ? "FLDTBLDIR32" : "FLDTBLDIR", string())); 
    m_setter.reset(new EnvironmentSetter(new_env));
  }

public:
  FmlEnvironmentSetter(const vector<string>& files, bool is_fml32) {
    string joined_files;
    for (unsigned i = 0, n = files.size(); i != n; ++i) {
      joined_files += files[i];
      if (i + 1 != n) joined_files += ","; // that's the Tuxedo way
    }
    set(joined_files, is_fml32);
  }
  FmlEnvironmentSetter(const string& file, bool is_fml32) {
    set(file, is_fml32);
  }
};

//------------------------------------------------------------------------------
// Extract all names from given table description file.
// See http://edocs.bea.com/tuxedo/tux91/fml/fml04.htm#1010346
static vector<string> read_names_from_fml_description_file(const char* filename, ExceptionSink* xsink)
{
   vector<string> result;
  FILE* f = fopen(filename, "rt");
  if (!f) {
    xsink->raiseException("FML[32]_process_description_tables", "read_names_from_fml_description_file(): the file [ %s ] cannot be opened.", filename);
    return result;
  }
  ON_BLOCK_EXIT(fclose, f);

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    char* thumb = line;
    while (isspace(*thumb)) ++thumb;
    if (!*thumb) continue;
    if (strstr(thumb, "*base") == thumb) continue;
    if (*thumb == '#') continue;

     char* name_end = thumb;
     while (!isspace(*name_end) && *name_end) ++name_end;
     *name_end = 0;

     if (name_end == thumb) continue; // ???
     result.push_back(thumb);
  }
  return result;
}

//-----------------------------------------------------------------------------
// Extract all names from all given table description file.
static vector<string> read_names_from_all_fml_description_files(const vector<string>& files, ExceptionSink* xsink)
{
   vector<string> result;
   for (unsigned i = 0; i < files.size(); ++i) {
     vector<string> aux = read_names_from_fml_description_file(files[i].c_str(), xsink);
     if (xsink->isException()) {
       return vector<string>();
     }
     result.insert(result.end(), aux.begin(), aux.end());
   }
   return result;
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
  m_context(0),
  m_send_buffer(0),
  m_send_buffer_size(0),
  m_receive_buffer(0),
  m_receive_buffer_size(0),
  m_string_encoding(QCS_DEFAULT),
  m_last_suspended_transaction_id(0)
{
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
  for (vector<int>::const_iterator it =  m_pending_async_calls.begin(), end = m_pending_async_calls.end(); it != end; ++it) {
    tpcancel(*it);
  }
  return (tpterm() == -1) ? tperrno : 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::saveContext()
{
  int res = tpgetctxt(&m_context, 0);
  if (res == -1) return tperrno;
  if (m_context == TPINVALIDCONTEXT) return TPINVALIDCONTEXT;
  if (m_context == TPNULLCONTEXT) return TPNULLCONTEXT;
  return 0;
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
  for (vector<int>::iterator it = m_pending_async_calls.begin(), end = m_pending_async_calls.end(); it != end; ++it) {
    if (*it == handle) {
      m_pending_async_calls.erase(it);
      break;
    }
  }
}

//------------------------------------------------------------------------------
static QoreNode* get_val(Hash* hash, char* name, QoreType* type)
{
  if (!hash) return 0;
  QoreNode* n = hash->getKeyValueExistence(name);
  if (!n || n == (QoreNode*)-1) return 0;  // -1 could be returned by the getKeyExistence() !!!
  if (n->type != type) return 0;
  return n;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::enqueue(char* queue_space, char* queue_name, long flags, Hash* settings, Hash*& out_settings)
{
  memset(&m_queue_settings, 0, sizeof(m_queue_settings));
  m_queue_settings.flags = TPNOFLAGS; // by default do not usea

  // set queue control parameters
  QoreNode* n = get_val(settings, "flags", NT_INT);
  if (n) m_queue_settings.flags = (long)n->val.intval;
  n = get_val(settings, "deq_time", NT_INT);
  if (n) m_queue_settings.deq_time = (long)n->val.intval;
  n = get_val(settings, "priority", NT_INT);
  if (n) m_queue_settings.priority = (long)n->val.intval;
  n = get_val(settings, "exp_time", NT_INT);
  if (n) m_queue_settings.exp_time = (long)n->val.intval;
  n = get_val(settings, "delivery_qos", NT_INT);
  if (n) m_queue_settings.delivery_qos = (long)n->val.intval;
  n = get_val(settings, "reply_qos", NT_INT);
  if (n) m_queue_settings.reply_qos = (long)n->val.intval;
  n = get_val(settings, "urcode", NT_INT);
  if (n) m_queue_settings.urcode = (long)n->val.intval;
  n = get_val(settings, "msgid", NT_BINARY);
  if (n) {
    BinaryObject* bin = n->val.bin;
    int sz = sizeof(m_queue_settings.msgid);
    if (bin->size() == sz) memcpy(&m_queue_settings.msgid, bin->getPtr(), sz);
  }
  n = get_val(settings, "corrid", NT_BINARY);
  if (n) {
    BinaryObject* bin = n->val.bin;
    int sz = sizeof(m_queue_settings.corrid);
    if (bin->size() == sz) memcpy(&m_queue_settings.corrid, bin->getPtr(), sz);
  }
  n = get_val(settings, "replyqueue", NT_STRING);
  if (n) strcpy(m_queue_settings.replyqueue, n->val.String->getBuffer());
  n = get_val(settings, "failurequeue", NT_STRING);
  if (n) strcpy(m_queue_settings.failurequeue, n->val.String->getBuffer());

  int res = tpenqueue(queue_space, queue_name, &m_queue_settings, m_send_buffer, m_send_buffer_size, flags);
  if (res == -1) return tperrno;

 // create hash with relevant out settings
  ExceptionSink xsink;
  auto_ptr<Hash> out(new Hash);
  out->setKeyValue("flags", new QoreNode((int64)m_queue_settings.flags), &xsink);

  int sz = sizeof(m_queue_settings.msgid);
  void* copy = malloc(sz);
  if (!copy) return TPEOS;
  memcpy(copy, &m_queue_settings.msgid, sz);
  BinaryObject* bin = new BinaryObject(copy, sz);
  out->setKeyValue("msgid", new QoreNode(bin), &xsink);

  out->setKeyValue("diagnostic", new QoreNode((int64)m_queue_settings.diagnostic), &xsink);

  if (xsink) return TPEINVAL;
  out_settings = out.release();
  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::dequeue(char* queue_space, char* queue_name, long flags, Hash* settings, Hash*& out_settings)
{
  memset(&m_queue_settings, 0, sizeof(m_queue_settings));
  m_queue_settings.flags = TPNOFLAGS; // by default do not use

  // set queue control parameters
  QoreNode* n = get_val(settings, "flags", NT_INT);
  if (n) m_queue_settings.flags = (long)n->val.intval;
  n = get_val(settings, "msgid", NT_BINARY);
  if (n) {
    BinaryObject* bin = n->val.bin;
    int sz = sizeof(m_queue_settings.msgid);
    if (bin->size() == sz) memcpy(&m_queue_settings.msgid, bin->getPtr(), sz);
  }
  n = get_val(settings, "corrid", NT_BINARY);
  if (n) {
    BinaryObject* bin = n->val.bin;
    int sz = sizeof(m_queue_settings.corrid);
    if (bin->size() == sz) memcpy(&m_queue_settings.corrid, bin->getPtr(), sz);
  }

  int res = tpdequeue(queue_space, queue_name, &m_queue_settings, &m_receive_buffer, &m_receive_buffer_size, flags);
  if (res == -1) return tperrno;

  // create hash with relevant out settings
  ExceptionSink xsink;
  auto_ptr<Hash> out(new Hash);
  out->setKeyValue("flags", new QoreNode((int64)m_queue_settings.flags), &xsink);
  out->setKeyValue("priority", new QoreNode((int64)m_queue_settings.priority), &xsink);

  int sz = sizeof(m_queue_settings.msgid);
  void* copy = malloc(sz);
  if (!copy) return TPEOS;
  memcpy(copy, &m_queue_settings.msgid, sz);
  BinaryObject* bin = new BinaryObject(copy, sz);
  out->setKeyValue("msgid", new QoreNode(bin), &xsink);

  sz = sizeof(m_queue_settings.corrid);
  copy = malloc(sz);
  if (!copy) return TPEOS;
  memcpy(copy, &m_queue_settings.corrid, sz);
  bin = new BinaryObject(copy, sz);
  out->setKeyValue("corrid", new QoreNode(bin), &xsink);

  out->setKeyValue("delivery_qos", new QoreNode((int64)m_queue_settings.delivery_qos), &xsink);
  out->setKeyValue("reply_qos", new QoreNode((int64)m_queue_settings.reply_qos), &xsink);
  out->setKeyValue("replyqueue", new QoreNode((char*)m_queue_settings.replyqueue), &xsink);
  out->setKeyValue("failurequeue", new QoreNode((char*)m_queue_settings.failurequeue), &xsink);
  out->setKeyValue("diagnostic", new QoreNode((int64)m_queue_settings.diagnostic), &xsink);
  out->setKeyValue("appkey", new QoreNode((int64)m_queue_settings.appkey), &xsink);
  out->setKeyValue("urcode", new QoreNode((int64)m_queue_settings.urcode), &xsink);
 
  sz = sizeof(m_queue_settings.cltid);
  copy = malloc(sz);
  if (!copy) return TPEOS;
  memcpy(copy, &m_queue_settings.cltid, sz);
  bin = new BinaryObject(copy, sz);
  out->setKeyValue("cltid", new QoreNode(bin), &xsink);

  if (xsink) return TPEINVAL;
  out_settings = out.release();
  return 0;
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::loadFmlDescription(const vector<string>& files, bool is_fml32, ExceptionSink* xsink)
{
  vector<string> all_names = read_names_from_all_fml_description_files(files,xsink);
  if (*xsink) return 0;

  // before returning the old variables back free the tables from memory
  // (assumption: Fldid[32] is idempotent)
  ON_BLOCK_EXIT((is_fml32 ? &Fidnm_unload : &Fidnm_unload32));

  FmlEnvironmentSetter setter(files, is_fml32);
  char* err_name = "TuxedoAdapter::loadFml[32]Description";

  auto_ptr<Hash> result(new Hash);

  for (unsigned i = 0, n = all_names.size(); i != n; ++i) {
    char* name = (char*)all_names[i].c_str();
    FLDID32 id;
    if (is_fml32) {
      id = Fldid32(name);
    } else {
      id = Fldid(name);
    }
    if (id == BADFLDID) {
      xsink->raiseException(err_name, "Fldid[32](\"%s\") failed. Ferror = %d.", name, Ferror);
      return 0;
    }
    int type;
    if (is_fml32) {
      type = Fldtype32(id);
    } else {
      type = Fldtype(id);
    }
    List* list = new List();
    list->insert(new QoreNode((int64)id));
    list->insert(new QoreNode((int64)type));
    result->setKeyValue(name, new QoreNode(list), xsink);
    if (xsink->isException()) {
      return 0;
    }
  }
  return result.release();
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::loadFmlDescription(const string& file, bool is_fml32, ExceptionSink* xsink)
{
  vector<string> files;
  files.push_back(file);
  return loadFmlDescription(files, is_fml32, xsink);
}

//------------------------------------------------------------------------------
Hash* QoreTuxedoAdapter::generateFmlDescription(int base, Hash* typed_names, bool is_fml32, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::generateFml[32]Description";

  char buffer[256];
  char* tmpfile = tmpnam(buffer);
  FILE* f = fopen(tmpfile, "wt");
  if (!f) {
    unlink(tmpfile); // just in case
    xsink->raiseException(err_name, "Failed to create temporary file. Please check directory for temporary files.");
    return 0;
  }
  unlink(tmpfile);
  ScopeGuard g = MakeGuard(fclose, f);

  if (base > 0) fprintf(f, "*base %d\n", base);

  HashIterator iter(typed_names);
  int counter = 0;

  while (iter.next()) {
    char* name = iter.getKey();
    QoreNode* value = iter.getValue();
    if (value->type != NT_INT) (Hash*)xsink->raiseException(err_name, "Input hash: value of [ %s ] needs to be an integer.", name);
    int type = (int)value->val.intval;
    char* type_name;

    switch (type) {
    case FLD_SHORT: type_name = "short"; break;    
    case FLD_LONG: type_name = "long"; break;
    case FLD_CHAR: type_name = "char"; break;
    case FLD_FLOAT: type_name = "float"; break;
    case FLD_DOUBLE: type_name = "double"; break;
    case FLD_STRING: type_name = "string"; break;
    case FLD_CARRAY: type_name = "carray"; break;

    case FLD_PTR:
    case FLD_FML32:
    case FLD_VIEW32:
    case FLD_MBSTRING:
      return (Hash*)xsink->raiseException(err_name, "Input hash: value of [ %s ], support for this type is not yet implemented.", name);
    default:
      return (Hash*)xsink->raiseException(err_name, "Input hash: value of [ %s ] is not recognized as a type.", name);
    }

    fprintf(f, "%s %d %s - \n", name, ++counter, type_name);
  }
  g.Dismiss();
  if (fclose(f)) {
    return (Hash*)xsink->raiseException(err_name, "Failed to create a temporary file.");
  }
  return loadFmlDescription(tmpfile, is_fml32, xsink);
}

//------------------------------------------------------------------------------
#ifdef DEBUG
TEST()
{
  ExceptionSink xsink;
  Hash typed_names;

  typed_names.setKeyValue("a_short", new QoreNode((int64)FLD_SHORT), &xsink);
  typed_names.setKeyValue("a_long", new QoreNode((int64)FLD_LONG), &xsink);
  typed_names.setKeyValue("a_char", new QoreNode((int64)FLD_CHAR), &xsink);
  typed_names.setKeyValue("a_float", new QoreNode((int64)FLD_FLOAT), &xsink);
  typed_names.setKeyValue("a_double", new QoreNode((int64)FLD_DOUBLE), &xsink);
  typed_names.setKeyValue("a_string", new QoreNode((int64)FLD_STRING), &xsink);
  typed_names.setKeyValue("a_carray", new QoreNode((int64)FLD_CARRAY), &xsink);

  QoreTuxedoAdapter adapter;
  Hash* res = adapter.generateFmlDescription(500, &typed_names, true, &xsink);
  assert(!xsink);
  assert(res);


  typed_names.deleteKey("a_short", &xsink);
  typed_names.deleteKey("a_long", &xsink);
  typed_names.deleteKey("a_char", &xsink);
  typed_names.deleteKey("a_float", &xsink);
  typed_names.deleteKey("a_double", &xsink);
  typed_names.deleteKey("a_string", &xsink);
  typed_names.deleteKey("a_carray", &xsink);
}
#endif

// EOF

