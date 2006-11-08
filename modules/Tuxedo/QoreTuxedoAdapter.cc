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

#include <limits.h>
#include <float.h>

#include <atmi.h>
#include <fml32.h>
#include <fml.h>

#include <memory>
#include <stdio.h>

#ifdef DEBUG
#  define private public
#endif

#include "QoreTuxedoAdapter.h"

using std::string;
using std::vector;
using std::pair;
using std::make_pair;
using std::auto_ptr;

namespace {

typedef vector<pair<string, string> > env_t;

//------------------------------------------------------------------------------
// Set given environment variables and return back to original state in destructor.
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
// Set environment variables needed to parse FML definitions, return back to original in destructor.
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
  FILE* f = fopen(filename, "r");
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
  printf("testing EnvironmentSetter\n");
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

TEST()
{
  printf("testing FmlEnvironmentSetter\n");
  { FmlEnvironmentSetter setter1("aaa", true); }
  { FmlEnvironmentSetter setter2("bbb", false); }
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
    list->push(new QoreNode((int64)id));
    list->push(new QoreNode((int64)type));
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
  ON_BLOCK_EXIT(unlink, tmpfile);
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
static void do_test(bool is_fml32)
{
  printf("testing generateFml[32]Description()\n");
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
  Hash* res = adapter.generateFmlDescription(500, &typed_names, is_fml32, &xsink);
  if (xsink) {
    Exception* e = xsink.catchException();
    printf("File %s, line %d threw\n", e->file, e->line);
  }
  assert(res);

  HashIterator it(res);
  int counter = 0;
  while (it.next()) {
    char* key = it.getKey();
    QoreNode* val = it.getValue();

    assert(val->type == NT_LIST);
    List* l = val->val.list;
    assert(l->size() == 2);
    QoreNode* id = l->retrieve_entry(0);
    assert(id->type == NT_INT);
    FLDID32 id_val = (FLDID32)id->val.intval;
    QoreNode* type = l->retrieve_entry(1);
    assert(type->type == NT_INT);
    int type_val = (int)type->val.intval;
    
    if (is_fml32) {
      assert(Fldtype32(id_val) == type_val);
    } else {
      assert(Fldtype(id_val) == type_val);
    }

    switch (counter) {
    case 0: 
      assert(!strcmp(key, "a_short")); 
      assert(type_val == FLD_SHORT);
      break;
    case 1: 
      assert(!strcmp(key, "a_long")); 
      assert(type_val == FLD_LONG);
      break;
    case 2: 
      assert(!strcmp(key, "a_char")); 
      assert(type_val == FLD_CHAR);
      break;
    case 3: 
      assert(!strcmp(key, "a_float")); 
      assert(type_val == FLD_FLOAT);
      break;
    case 4: 
      assert(!strcmp(key, "a_double")); 
      assert(type_val == FLD_DOUBLE);
      break;
    case 5: 
      assert(!strcmp(key, "a_string")); 
      assert(type_val == FLD_STRING);
      break;
    case 6: 
      assert(!strcmp(key, "a_carray")); 
      assert(type_val == FLD_CARRAY);
      break;
    default: assert(false);
    }
    ++counter;
  }
 
  QoreNode* aux = new QoreNode(res);
  aux->deref(&xsink);

  typed_names.deleteKey("a_short", &xsink);
  typed_names.deleteKey("a_long", &xsink);
  typed_names.deleteKey("a_char", &xsink);
  typed_names.deleteKey("a_float", &xsink);
  typed_names.deleteKey("a_double", &xsink);
  typed_names.deleteKey("a_string", &xsink);
  typed_names.deleteKey("a_carray", &xsink);
}

TEST()
{
  do_test(false);
  do_test(true);
}
#endif

//------------------------------------------------------------------------------
// helper
static int reallocate_buffer(char** buffer, long* buffer_size, long new_size)
{
  char* res = tprealloc(*buffer, new_size);
  if (!res) return tperrno;
  *buffer = res;
  *buffer_size = new_size;
  return 0;
}

//-----------------------------------------------------------------------------
// Find out ID + type from FML name, helper
static pair<FLDID32, int>  fml_name2id(char* name, Hash* description_info, ExceptionSink* xsink, char* func_name)
{
  pair<FLDID32, int> result(0, 0);
  QoreNode* n = description_info->getKeyValueExistence(name);
  if (!n || n == (QoreNode*)-1) {
    xsink->raiseException(func_name, "Field [ %s ] does not exist in FML[32] settings. Please check possible typos.");
    return result;
  }
  // already known to be list with (int, int)
  assert(n->type == NT_LIST);
  List* l = n->val.list;
  assert(l->size() == 2);
  n = l->retrieve_entry(0);
  result.first = (FLDID32)n->val.intval;
  n = l->retrieve_entry(1);
  result.second = (int)n->val.intval;
  return result;
}

//------------------------------------------------------------------------------
// Find out type from FML name, helper
static int fml_name2type(char* name, Hash* description_info, ExceptionSink* xsink, char* func_name)
{
  QoreNode* n = description_info->getKeyValue(name);
  if (!n) {
    xsink->raiseException(func_name, "Field [ %s ] does not exist in FML[32] settings. Please check possible typos.");
    return 0;
  }
  // already known to be list with (int, int)
  List* l = n->val.list;
  n = l->retrieve_entry(1);
  return (FLDID32)n->val.intval;
}

//------------------------------------------------------------------------------
// Find FML name and type from ID, helper
static pair<string, int> fml_id2name(FLDID32 id, Hash* description_info, ExceptionSink* xsink, char* func_name)
{
  HashIterator it(description_info);
  while (it.next()) {
    QoreNode* n = it.getValue();
    assert(n->type == NT_LIST);
    List* l = n->val.list;
    assert(l->size() == 2);
    n = l->retrieve_entry(0);
    assert(n->type == NT_INT);
    FLDID32 this_id = (FLDID32)n->val.intval;
    if (this_id != id) continue;

    pair<string, int> result;
    result.first = it.getKey();
    n = l->retrieve_entry(1);
    assert(n->type == NT_INT);
    result.second = (int)n->val.intval;
    return result;
  }
  xsink->raiseException(func_name, "A FML[32] ID not found in description table.");
  return make_pair("", 0);
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::add_fml_value_into_send_buffer(char* value_name, FLDID32 id, int value_type, QoreNode* value, bool is_fml32, ExceptionSink* xsink)
{
  char* err_name = "TuxedoAdapter::setFml[32]DataToSend";

  char* ptr_val;
  FLDLEN32 value_length;

  union {
    short short_value;
    long long_value;
    char char_value;
    float float_value;
    double double_value;
  };
  
  switch (value_type) {
    case FLD_SHORT: 
    {
      if (value->type != NT_INT) {
         xsink->raiseException(err_name, "Value [ %s ] needs to be integer.", value_name);
        return 0;
      }
      int64 val = value->val.intval;
      if (val < SHRT_MIN || val > SHRT_MAX) {
        xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting short type.", value_name);
        return 0;
      }
      short_value = (short)val;
      ptr_val = (char*)&short_value;
      value_length = sizeof(short);
      break;      
    }

    case FLD_LONG:
    {
      if (value->type != NT_INT) {
        xsink->raiseException(err_name, "Value [ %s ] needs to be integer.", value_name);
        return 0;
      }
      int64 val = value->val.intval;
      if (val < LONG_MIN || val > LONG_MAX) {
        xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting long type.", value_name);
        return 0;
      }
      long_value = (long)val;
      ptr_val = (char*)&long_value;
      value_length = sizeof(long);
      break;
    } 

    case FLD_CHAR: 
    {
      if (value->type != NT_INT) {
         xsink->raiseException(err_name, "Value [ %s ] needs to be integer.", value_name);
        return 0;
      }
      int64 val = value->val.intval;
      if (val < CHAR_MIN || val > CHAR_MAX) {
        xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting char type.", value_name);
        return 0;
      }
      char_value = (char)val;
      ptr_val = &char_value;
      value_length = sizeof(char);
      break;
    }

    case FLD_FLOAT: 
    {
      if (value->type != NT_FLOAT) {
         xsink->raiseException(err_name, "Value [ %s ] needs to be float.", value_name);
        return 0;
      }
      double val = value->val.floatval;
      if (val < FLT_MIN || val > FLT_MAX) {
        xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting float type.", value_name);
        return 0;
      }
      float_value = (float)val;
      ptr_val = (char*)&float_value;
      value_length = sizeof(float);
      break;
    }

    case FLD_DOUBLE: 
    {
      if (value->type != NT_FLOAT) {
         xsink->raiseException(err_name, "Value [ %s ] needs to be float.", value_name);
        return 0;
      }
      double_value = value->val.floatval;
      ptr_val = (char*)&double_value;
      value_length = sizeof(double);
      break;
    }

    case FLD_STRING:
    {
      if (value->type != NT_STRING) {
        xsink->raiseException(err_name, "Value [ %s ] needs to be a string.", value_name);
        return 0;
      }
      char* s = value->val.String->getBuffer();
      if (!s) s = "";
      ptr_val = s;
      value_length = strlen(s) + 1; 
      break;
    } 

    case FLD_CARRAY:
    {
      if (value->type != NT_BINARY) {
        xsink->raiseException(err_name, "Value [ %s ] needs to be a binary.", value_name);
        return 0;
      }
      BinaryObject* bin = value->val.bin;
      ptr_val = (char*)bin->getPtr();
      if (!ptr_val) ptr_val = "";
      value_length = bin->size();
      break;
    }

    case FLD_PTR:
    case FLD_FML32:
    case FLD_VIEW32:
    case FLD_MBSTRING:
    default:
      xsink->raiseException(err_name, "Value [ %s ] has unrecognized type %d.", value_name, value_type);
      return 0;
  }

  for (;;) {
    int res;
    if (is_fml32) {
      res = Fappend32((FBFR32*)m_send_buffer, id, ptr_val, value_length);
    } else {
      res = Fappend((FBFR*)m_send_buffer, id, ptr_val, value_length);
    }
    if (res != -1) {
      break;
    }
    if (Ferror != FNOSPACE) {
      xsink->raiseException(err_name, "Value [ %s ] cannot be appended into FML[32] buffer. Error %d.", value_name, (int)Ferror);
      return 0;
    }
    // the buffer needs to be resized
    int increment = m_send_buffer_size;
    if (increment > 64 * 1024) {
      increment = 64 * 1024; // guesed value
    }
    res = reallocate_buffer(&m_send_buffer, &m_send_buffer_size, m_send_buffer_size + increment);
    if (res) return res;
  }

  return 0;
}

//------------------------------------------------------------------------------
int QoreTuxedoAdapter::setFmlDataToSend(Hash* description_info, List* data, bool is_fml32, ExceptionSink* xsink)
{
  resetDataToSend();
  char* err_name = "TuxedoAdapter::setFml[32]DataToSend";
  
  long buffer_size = 4096;
  char* type = (char*)(is_fml32 ? "FML32" : "FML");
  m_send_buffer = tpalloc(type, 0, buffer_size);
  if (!m_send_buffer) return tperrno;
  m_send_buffer_size = buffer_size;
  int res;
  if (is_fml32) {
    res = Finit32((FBFR32*)m_send_buffer, m_send_buffer_size);
    assert(Fielded32((FBFR32*)m_send_buffer));
  } else {
    res = Finit((FBFR*)m_send_buffer, m_send_buffer_size);
    assert(Fielded((FBFR*)m_send_buffer));
  }
  if (res == -1) {
    xsink->raiseException(err_name, "Finit[32] failed with error %d.", (int)Ferror);
    return 0;
  }
  // append every item in the buffer
  for (int i = 0, cnt = data->size(); i != cnt; ++i) {
    QoreNode* n = data->retrieve_entry(i); // known as list of (string type, value)
    assert(n->type == NT_LIST);
    List* l = n->val.list;
    assert(l->size() == 2);    
    n = l->retrieve_entry(0);
    assert(n->type == NT_STRING);
    char* name = n->val.String->getBuffer();
    if (!name || !name[0]) {
      xsink->raiseException(err_name, "Item name # %d is empty.", i + 1);
      return 0;
    }
    pair<FLDID32, int> id_type = fml_name2id(name, description_info, xsink, err_name);
    if (*xsink) {
      return 0;
    }
    n = l->retrieve_entry(1);
    int res = add_fml_value_into_send_buffer(name, id_type.first, id_type.second, n, is_fml32, xsink);
    if (*xsink) return 0;
    if (res) return res;    
  }

  // switch off from append mode
  if (is_fml32) {
    res = Findex32((FBFR32*)m_send_buffer, 0);
    assert(Fielded32((FBFR32*)m_send_buffer));
  } else {
    res = Findex((FBFR*)m_send_buffer, 0);
    assert(Fielded((FBFR*)m_send_buffer));
  }
  if (res == -1) {
    xsink->raiseException(err_name, "Findex[32] failed with error %d.", (int)Ferror);
    return 0;
  }

  // set up proper buffer size 
  long result_size;
  if (is_fml32) {
    result_size = Fsizeof32((FBFR32*)m_send_buffer);
  } else {
    result_size = Fsizeof((FBFR*)m_send_buffer);
  }
  if (result_size == -1) {
    xsink->raiseException(err_name, "Fsizeof[32] failed with error %d.", (int)Ferror);
    return 0;
  }
  assert(result_size);
  m_send_buffer_size = (int)result_size;

  return 0;
}

//------------------------------------------------------------------------------
List* QoreTuxedoAdapter::getFmlDataFromBuffer(Hash* description_info, bool is_fml32, 
  ExceptionSink* xsink, char* buffer, long buffer_size, char* err_name)
{
 
  if (!buffer) {
    xsink->raiseException(err_name, "The typed buffer is empty.");
    return 0;
  }
  char type[20];
  char subtype[20];
  if (tptypes(buffer, type, subtype) == -1) {
    xsink->raiseException(err_name, "tptypes() of typed buffer failed with error %d.", tperrno);
    return 0;
  }
  const char* expected_type = is_fml32 ? "FML32" : "FML";
  if (strcmp(type, expected_type)) {
    xsink->raiseException(err_name, "Unexpected type of data in typed buffer: %s expected, %s found.", expected_type, type);
    return 0;
  }
  // is the buffer actually FML[32], i.e. fielded?
  bool fielded;
  if (is_fml32) {
    fielded = Fielded32((FBFR32*)buffer);
  } else {
    fielded = Fielded((FBFR*)buffer);
  }
  if (!fielded) {
    xsink->raiseException(err_name, "The typed buffer doesn't contain valid FML/FML32 data.");
    return 0;
  }

  auto_ptr<List> out_list(new List);

  FLDID32 fldid32 = FIRSTFLDID;
  FLDID   fldid = FIRSTFLDID;
  FLDOCC32 fldocc32;
  FLDOCC   fldocc;

  unsigned value_buffer_size = 8 * 1024;
  char* value_buffer = (char*)malloc(value_buffer_size);
  if (!value_buffer) {
    xsink->outOfMemory();
    return 0;
  }
  ON_BLOCK_EXIT(free, value_buffer);
  unsigned actual_value_length;

  // read item by item, resize buffer as needed
  for (;;) {
    int res;
    if (is_fml32) {
      FLDLEN32 len = value_buffer_size;
      for (;;) {
        res = Fnext32((FBFR32*)buffer, &fldid32, &fldocc32, value_buffer, &len);
        if (res != -1) {
          actual_value_length = len;
          break;         
        }
        if (Ferror != FNOSPACE) {
          break;
        }
        char* aux = (char*)realloc(value_buffer, 2 * value_buffer_size);
        if (!aux) {
          xsink->outOfMemory();
          return 0;
        }
        value_buffer = aux;
        value_buffer_size *= 2;
      }      
    } else { 
      FLDLEN len = value_buffer_size;
      res = Fnext((FBFR*)buffer, &fldid, &fldocc, value_buffer, &len);
      if (res != -1) {
        actual_value_length = len;
      } else {
        if (Ferror != FNOSPACE) {
          break;
        } 
        const unsigned MaxLen = 64 * 1024 - 1; // FLDLEN is ushort
        if (value_buffer_size >= MaxLen) break; 
        value_buffer_size *= 2;
        if (value_buffer_size >= MaxLen) value_buffer_size = MaxLen;
        char* aux = (char*)realloc(value_buffer, value_buffer_size);
        if (!aux) {
          xsink->outOfMemory();
          return 0;
        }
        value_buffer = aux;
        continue;
      }
    }
    if (res == 0) {
      break; // no more
    }

    if (res == -1) {
      xsink->raiseException(err_name, "Failed to extract FML[32] field from buffer, Ferror = %d.", (int)Ferror);
      return 0;
    }
    // Fname[32] cannot be used as it loads description from file(s)
    pair<string, int> item_info;
    if (is_fml32) {
      item_info  = fml_id2name(fldid32, description_info, xsink, err_name);
    } else {
      item_info = fml_id2name(fldid, description_info, xsink, err_name);
    }
    if (xsink->isException()) {
      return 0;
    }
    assert(!item_info.first.empty());
    QoreNode* result_value = 0;
    
    switch (item_info.second) {
    case FLD_SHORT:
    {
      if (actual_value_length != sizeof(short)) {
        xsink->raiseException(err_name, "FML[32] type short, invalid data length %d.", actual_value_length);
        return 0;
      }
      short val;
      memcpy(&val, value_buffer, sizeof(short));
      result_value = new QoreNode((int64)val);
      break;
    }

    case FLD_LONG: 
    {
      if (actual_value_length != sizeof(long)) {
        xsink->raiseException(err_name, "FML[32] type long, invalid data length %d.", actual_value_length);
        return 0;
      }
      long val;
      memcpy(&val, value_buffer, sizeof(long));
      result_value = new QoreNode((int64)val);
      break;
    }

    case FLD_CHAR: 
    {
      if (actual_value_length != sizeof(char)) {
        xsink->raiseException(err_name, "FML[32] type char, invalid data length %d.", actual_value_length);
        return 0;
      }
      result_value = new QoreNode((int64)value_buffer[0]);
      break;
    }

    case FLD_FLOAT: 
    {
      if (actual_value_length != sizeof(float)) {
        xsink->raiseException(err_name, "FML[32] type float, invalid data length %d.", actual_value_length);
        return 0;
      }
      float val;
      memcpy(&val, value_buffer, sizeof(float));
      result_value = new QoreNode(val);
      break;
    }

    case FLD_DOUBLE: 
    {
      if (actual_value_length != sizeof(double)) {
        xsink->raiseException(err_name, "FML[32] type double, invalid data length %d.", actual_value_length);
        return 0;
      }
      double val;
      memcpy(&val, value_buffer, sizeof(double));
      result_value = new QoreNode(val);
      break;
    }

    case FLD_STRING: 
    {
      assert(value_buffer[actual_value_length] == 0);
      result_value = new QoreNode(value_buffer);
      break;
    }

    case FLD_CARRAY: 
    {
      if (actual_value_length == 0) {
        result_value = new QoreNode(new BinaryObject);
      } else {
        char* copy = (char*)malloc(actual_value_length);
        if (!copy) {
          xsink->outOfMemory();
          return 0;
        }
        result_value = new QoreNode(new BinaryObject(copy, actual_value_length));
      }  
      break;
    }

    case FLD_PTR:
    case FLD_FML32:
    case FLD_VIEW32:
    case FLD_MBSTRING:
      xsink->raiseException(err_name, "FML[32] type %d is not yet supported.", item_info.second);
      return 0;
    default:
      xsink->raiseException(err_name, "Internal error: uknown data type %d in FML description hash.", item_info.second);
      return 0;
    } // switch

    List* sublist = new List;
    sublist->push(new QoreNode((char*)item_info.first.c_str()));
    sublist->push(result_value);
    out_list->push(new QoreNode(sublist));

  } // for
  
  return out_list.release();
}

//------------------------------------------------------------------------------
#ifdef DEBUG
static void do_test2(bool is_fml32)
{
  printf("testing setFml[32]DataToSend()/getFmlDataFromBuffer()\n");
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
  Hash* res = adapter.generateFmlDescription(500, &typed_names, is_fml32, &xsink);
  if (xsink) {
    Exception* e = xsink.catchException();
    printf("File %s, line %d threw\n", e->file, e->line);
  }
  assert(res);

  List* data = new List;
  List* sublist = new List;
  sublist->push(new QoreNode("a_long"));
  sublist->push(new QoreNode((int64)12345678));
  data->push(new QoreNode(sublist));
  sublist = new List;
  sublist->push(new QoreNode("a_string"));
  sublist->push(new QoreNode("string1"));
  data->push(new QoreNode(sublist));
  sublist = new List;
  sublist->push(new QoreNode("a_string"));
  sublist->push(new QoreNode("string2"));
  data->push(new QoreNode(sublist));

  assert(!xsink.isException());
  int x = adapter.setFmlDataToSend(res, data, is_fml32, &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(x == 0);
  assert(adapter.m_send_buffer);

  // read the data back
  List* extracted_data = adapter.getFmlDataFromBuffer(res, is_fml32, &xsink,
    adapter.m_send_buffer, adapter.m_send_buffer_size, "a test routine");
  if (xsink.isException()) {
    assert(false);
  }
  assert(extracted_data);

  assert(extracted_data->size() == 3);
  // value 1
  QoreNode* n = extracted_data->retrieve_entry(0);
  assert(n->type == NT_LIST);
  sublist = n->val.list;
  assert(sublist->size() == 2);
  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  char* s = n->val.String->getBuffer();
  if (strcmp(s, "a_long") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_INT);
  assert(n->val.intval == 12345678);

  // value 2
  n = extracted_data->retrieve_entry(1);
  assert(n->type == NT_LIST);
  sublist = n->val.list;
  assert(sublist->size() == 2);

  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  s = n->val.String->getBuffer();
  if (strcmp(s, "a_string") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_STRING);
  s = n->val.String->getBuffer();
  if (strcmp(s, "string1") != 0) {
    assert(false);
  }

  // value 3
  n = extracted_data->retrieve_entry(2);
  assert(n->type == NT_LIST);
  sublist = n->val.list;
  assert(sublist->size() == 2);

  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  s = n->val.String->getBuffer();
  if (strcmp(s, "a_string") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_STRING);
  s = n->val.String->getBuffer();
  if (strcmp(s, "string2") != 0) {
    assert(false);
  }
  
  delete extracted_data;

  // cleanup
  QoreNode* aux = new QoreNode(res);
  aux->deref(&xsink);
  aux = new QoreNode(data);
  aux->deref(&xsink);

  typed_names.deleteKey("a_short", &xsink);
  typed_names.deleteKey("a_long", &xsink);
  typed_names.deleteKey("a_char", &xsink);
  typed_names.deleteKey("a_float", &xsink);
  typed_names.deleteKey("a_double", &xsink);
  typed_names.deleteKey("a_string", &xsink);
  typed_names.deleteKey("a_carray", &xsink);
}

TEST()
{
  do_test2(false);
  do_test2(true);
  do_test2(false);
  do_test2(true);
}
#endif

// EOF

