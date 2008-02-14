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

#include <qore/Qore.h>
#include <qore/minitest.hpp>
#include <qore/Environment.h>

#include <limits.h>
#include <float.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>

#include <atmi.h>
#include <fml32.h>
#include <fml.h>
#include <tx.h>

#include <memory>
#include <stdio.h>
#include <unistd.h>

#ifdef DEBUG
#  define private public
#endif

#include "QoreTuxedoAdapter.h"

using std::string;
using std::vector;
using std::pair;
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
  Environment env;

  for (env_t::const_iterator it = new_env.begin(), end = new_env.end(); it != end; ++it) {
    QoreString* old = env.get(it->first.c_str());
    const char* s = old ? old->getBuffer() : "";
    m_old_env.push_back(std::pair<std::string, std::string>(it->first, s));

    if (it->second.empty()) {      
      env.unset(it->first.c_str());
    } else {
      env.set(it->first.c_str(), it->second.c_str(), 1);
    }
  }
}

LockedObject EnvironmentSetter::m_mutex;

EnvironmentSetter::~EnvironmentSetter()
{
  for (env_t::const_iterator it = m_old_env.begin(), end = m_old_env.end(); it != end; ++it) {
    Environment env;
    if (it->second.empty()) {
      env.unset(it->first.c_str());
    } else {
      env.set(it->first.c_str(), it->second.c_str(), 1);
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
    new_env.push_back(pair<std::string, std::string>(is_fml32 ? "FIELDTBLS32" : "FIELDTBLS", value));
    // all file names are expected to be absolute paths
    new_env.push_back(pair<std::string, std::string>(is_fml32 ? "FLDTBLDIR32" : "FLDTBLDIR", string())); 
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
    xsink->raiseException("FML-DESCRIPTION-TABLE-ERROR", "read_names_from_fml_description_file(): the file [ %s ] cannot be opened.", filename);
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
  envs.push_back(pair<std::string, std::string>("test_aaa", "xyz"));
  envs.push_back(pair<std::string, std::string>("test_bbb", ""));

  Environment env;
  env.set("test_aaa", "old aaa", 1);
  env.set("test_bbb", "old bbb", 1);

  { 
    EnvironmentSetter setter(envs);
    QoreString* s = env.get("test_aaa");
    assert(s);
    assert(!strcmp(s->getBuffer(), "xyz"));
    delete s;
    s = env.get("test_bbb");
    assert(!s);
  }
  QoreString* s =env.get("test_aaa");
  assert(s);
  assert(!strcmp(s->getBuffer(), "old aaa"));
  delete s;
  s = env.get("test_bbb");
  assert(s);
  assert(!strcmp(s->getBuffer(), "old bbb"));
  delete s;
}

TEST()
{
  printf("testing FmlEnvironmentSetter\n");
  { FmlEnvironmentSetter setter1("aaa", true); }
  { FmlEnvironmentSetter setter2("bbb", false); }
}
#endif

#ifdef DEBUG
QoreTuxedoAdapter::QoreTuxedoAdapter() // just for testing
: m_connection_flags(0),
  m_Tuxedo_connection_initialized(false),
  m_context(0),
  m_string_encoding(QCS_DEFAULT),
  m_last_suspended_transaction_id(0),
  m_send_buffer(0),
  m_send_buffer_size(0),
  m_default_flags_for_call(0),
  m_default_flags_for_call_set(false),
  m_default_flags_for_acall(0),
  m_default_flags_for_acall_set(false),
  m_default_flags_for_getrply(0),
  m_default_flags_for_getrply_set(false),
  m_default_flags_for_post_event(0),
  m_default_flags_for_post_event_set(false),
  m_default_flags_for_connect(0),
  m_default_flags_for_connect_set(false),
  m_default_flags_for_send(0),
  m_default_flags_for_send_set(false),
  m_default_flags_for_receive(0),
  m_default_flags_for_receive_set(false),
  m_default_flags_for_enqueue(0),
  m_default_flags_for_enqueue_set(false),
  m_default_flags_for_dequeue(0),
  m_default_flags_for_dequeue_set(false),
  m_default_is_fml32(true),
  m_default_is_fml32_set(false),
  m_default_fml_description(0),
  m_default_fml32_description(0)
{
}
#endif

//------------------------------------------------------------------------------
QoreTuxedoAdapter::QoreTuxedoAdapter(const QoreHashNode* settings, ExceptionSink* xsink)
: m_connection_flags(0),
  m_Tuxedo_connection_initialized(false),
  m_context(0),
  m_string_encoding(QCS_DEFAULT),
  m_last_suspended_transaction_id(0),
  m_send_buffer(0),
  m_send_buffer_size(0),
  m_default_flags_for_call(0),
  m_default_flags_for_call_set(false),
  m_default_flags_for_acall(0),
  m_default_flags_for_acall_set(false),
  m_default_flags_for_getrply(0),
  m_default_flags_for_getrply_set(false),
  m_default_flags_for_post_event(0),
  m_default_flags_for_post_event_set(false),
  m_default_flags_for_connect(0),
  m_default_flags_for_connect_set(false),
  m_default_flags_for_send(0),
  m_default_flags_for_send_set(false),
  m_default_flags_for_receive(0),
  m_default_flags_for_receive_set(false),
  m_default_flags_for_enqueue(0),
  m_default_flags_for_enqueue_set(false),
  m_default_flags_for_dequeue(0),
  m_default_flags_for_dequeue_set(false),
  m_default_is_fml32(true),
  m_default_is_fml32_set(false),
  m_default_fml_description(0),
  m_default_fml32_description(0)
{
  const char* err_name = "TUXEDO-ADAPTER-CONSTRUCTOR";

  long priority = 0;
  bool priority_set = false;
  bool finish_commit_after_data_logged = false;
  bool finish_commit_set = false;
  bool finish_Tx_commit_after_data_logged = false;
  bool finish_Tx_commit_set = false;
  bool Tx_transactions_chained = false;
  bool Tx_transactions_chained_set = false;

  ConstHashIterator iter(settings);
  while (iter.next()) {
    string key = iter.getKey();  
    for (unsigned i = 0, n = key.size(); i != n; ++i) key[i] = tolower(key[i]);

    //-------------------------------------------
    if (key == "username") {
       const AbstractQoreNode* n = iter.getValue();
       const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'UserName' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_username = s;
      continue;
    }
    //-------------------------------------------
    if (key == "password") {
       const AbstractQoreNode* n = iter.getValue();
       const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'Password' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_password = s;
      continue;
    }
    //-------------------------------------------
    if (key == "clientname") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'ClientName' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_clientname = s;
      continue;
    }
    //-------------------------------------------
    if (key == "groupname") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
	 xsink->raiseException(err_name, "Settings value 'GroupName' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_groupname = s;
      continue;
    }
    //-------------------------------------------
    if (key == "binaryconnectiondata") {
      const AbstractQoreNode* n = iter.getValue();
      const BinaryNode *bin = dynamic_cast<const BinaryNode *>(n);
      if (!bin) {
        xsink->raiseException(err_name, "Settings value 'BinaryConnectionData' needs to be a binary.");
        return;
      }
      if (bin->size()) {
        int size = bin->size();
        char* data = (char*)bin->getPtr();
        m_binary_data.clear();
        m_binary_data.insert(m_binary_data.end(), data, data + size);     
      }
      continue;
    }
    //-------------------------------------------
    if (key == "connectionflags") {
      const AbstractQoreNode* n = iter.getValue();
      long flags = (long)(n ? n->getAsBigInt() : 0);
      flags |= TPMULTICONTEXTS; // always use multicontext mode
      m_connection_flags = flags;
      continue;
    }
    //-------------------------------------------
    if (key == "tuxdir") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'TUXDIR' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      char* err_text = (char*)"Settings value 'TUXDIR' needs to be full path directory to Tuxedo installation.";
      if (!s || !s[0]) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      DIR* dir = opendir(s);
      if (dir) closedir(dir);
      if (!dir) {
        xsink->raiseException(err_name, err_text);
        return;
      } 
      m_env_variables.push_back(pair<std::string, std::string>("TUXDIR", s));
      continue;
    }
    //-------------------------------------------
    if (key == "tuxconfig") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'TUXCONFIG' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      char* err_text = (char*)"Settings value 'TUXCONFIG' needs to be full path to configuration file.";
      if (!s || !s[0]) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      FILE* f = fopen(s, "rb");
      if (f) fclose(f);
      if (!f) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      m_env_variables.push_back(pair<std::string, std::string>("TUXCONFIG", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsenvfile") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSENVFILE' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSENVFILE", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsnaddr") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSNADDR' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSNADDR", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsfaddr") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSFADDR' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSFADDR", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsfrange") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSFRANGE' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSFRANGE", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsdevice") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSDEVICE' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSDEVICE", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wstype") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSTYPE' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSTYPE", s));
      continue;
    }
    //-------------------------------------------
    if (key == "wsrplymax") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'WSRPLYMAX' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("WSRPLYMAX", s));
      continue;
    }
    //-------------------------------------------
    if (key == "tmminencryptbits") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'TMMINENCRYPTBITS' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("TMMINENCRYPTBITS", s));
      continue;
    }
    //-------------------------------------------
    if (key == "tmmaxencryptbits") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings value 'TMMAXENCRYPTBITS' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s) s = "";
      m_env_variables.push_back(pair<std::string, std::string>("TMMAXENCRYPTBITS", s));
      continue;
    }
    //-------------------------------------------
    if (key == "stringencoding") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings 'StringEncoding' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (!s || !s[0]) {
        s = "UTF8";
      }
      setStringEncoding(s);
      continue;
    } 
    //-------------------------------------------
    if (key == "defaultflagsforcall") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_call = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_call_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforasynccall") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_acall = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_acall_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforwaitforasyncreply") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_getrply = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_getrply_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforpostevent") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_post_event = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_post_event_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforjoinconversation") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_connect = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_connect_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforsendconversationdata") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_send = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_send_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforreceiveconversationdata") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_receive = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_receive_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsforenqueue") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_enqueue = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_enqueue_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultflagsfordequeue") {
      const AbstractQoreNode* n = iter.getValue();
      m_default_flags_for_dequeue = (long)(n ? n->getAsBigInt() : 0);
      m_default_flags_for_dequeue_set = true;
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfmltype") {
      const AbstractQoreNode* n = iter.getValue();
      char* err_text = (char*)"Settings 'DefaultFmlType' needs to be string \"FML\" or \"FML32\".";
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      const char* s = str->getBuffer();
      if (!s || !s[0]) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      if (strcmp(s, "FML") == 0) {
        m_default_is_fml32 = false;
        m_default_is_fml32_set = true;
      } else 
      if (strcmp(s, "FML32") == 0) {
        m_default_is_fml32 = true;
        m_default_is_fml32_set = true;
      } else {
        xsink->raiseException(err_name, err_text);
        return;
      }
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfmldescriptionfile") {
      if (m_default_fml_description) {
        xsink->raiseException(err_name, "Only one of 'DefaultFmlDescriptionFile' or 'DefaultFmlDescription' settings could be used.");
        return;
      }
      char* err_text = (char*)"Settings 'DefaultFmlDescriptionFile' needs to be complete file name.";
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      const char* s = str->getBuffer();
      if (!s || !s[0]) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      m_default_fml_description = loadFmlDescription(s, false, xsink);
      if (xsink->isException()) {
        xsink->raiseException(err_name, err_text); 
        return;
      }
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfml32descriptionfile") {
      if (m_default_fml32_description) {
        xsink->raiseException(err_name, "Only one of 'DefaultFml32DescriptionFile' or 'DefaultFml32Description' settings could be used.");
        return;
      }
      char* err_text = (char*)"Settings 'DefaultFml32DescriptionFile' needs to be complete file name.";
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      const char* s = str->getBuffer();
      if (!s || !s[0]) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      m_default_fml32_description = loadFmlDescription(s, true, xsink);
      if (xsink->isException()) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfmldescription") {
      if (m_default_fml_description) {
        xsink->raiseException(err_name, "Only one of 'DefaultFmlDescriptionFile' or 'DefaultFmlDescription' settings could beused.");
        return;
      }
      char* err_text = (char*)"Settings 'DefaultFmlDescription' needs to be a hash.";
      const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(iter.getValue());
      if (!h) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      // check whether base is set
      int base = 0;
      const AbstractQoreNode *n = settings->getKeyValue("DefaultFmlDescriptionBase");
      base = n ? n->getAsInt() : 0;

      m_default_fml_description = generateFmlDescription(base, h, false, xsink);
      if (xsink->isException()) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfmldescriptionbase") {
      continue;
    } 
    //-------------------------------------------
    if (key == "defaultfml32description") {
      if (m_default_fml32_description) {
        xsink->raiseException(err_name, "Only one of 'DefaultFml32DescriptionFile' or 'DefaultFml32Description' settings could be used.");
        return;
      }
      char* err_text = (char*)"Settings 'DefaultFml32Description' needs to be a hash.";

      const QoreHashNode *h = dynamic_cast<const QoreHashNode *>(iter.getValue());
      if (!h) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      // check whether base is set
      int base = 0;
      const AbstractQoreNode *n = settings->getKeyValue("DefaultFml32DescriptionBase");
      base = n ? n->getAsInt() : 0;

      m_default_fml32_description = generateFmlDescription(base, h, true, xsink);
      if (xsink->isException()) {
        xsink->raiseException(err_name, err_text);
        return;
      }
      continue;
    }
    //-------------------------------------------
    if (key == "defaultfml32descriptionbase") {
      continue;
    }
    //-------------------------------------------
    if (key == "priority") {
      const AbstractQoreNode* n = iter.getValue();
      priority_set = true;
      priority = (long)(n ? n->getAsBigInt() : 0);
      continue;
    }
    //-------------------------------------------
    if (key == "whencommitfinishes") {
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings 'WhenCommitFinishes' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (s && s[0]) {
        if (strcmp(s, "AfterDataAreLogged") == 0) {
          finish_commit_after_data_logged = true;
          finish_commit_set = true;
        } else
        if (strcmp(s, "AfterTwoPhaseCompletes") == 0) {
          finish_commit_after_data_logged = false;
          finish_commit_set = true;
        } else {
          xsink->raiseException(err_name, "Settings 'WhenCommitFinishes' needs to be either 'AfterDataAreLogged' or 'AfterTwoPhaseCompletes'.");
          return;
        }
      }
      continue;
    }
    //-------------------------------------------
    if (key == "whentxcommitfinishes") { // this is for Tx transactions
      const AbstractQoreNode* n = iter.getValue();
      const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
      if (!str) {
        xsink->raiseException(err_name, "Settings 'WhenTxCommitFinishes' needs to be a string.");
        return;
      }
      const char* s = str->getBuffer();
      if (s && s[0]) {
        if (strcmp(s, "AfterDataAreLogged") == 0) {
          finish_Tx_commit_after_data_logged = true;
          finish_Tx_commit_set = true;
        } else
        if (strcmp(s, "AfterTwoPhaseCompletes") == 0) {
          finish_Tx_commit_after_data_logged = false;
          finish_Tx_commit_set = true;
        } else {
          xsink->raiseException(err_name, "Settings 'WhenTxCommitFinishes' needs to be either 'AfterDataAreLogged' or 'AfterTwoPhaseCompletes'.");
          return;
        }
      }
      continue;
    }
    //-------------------------------------------
    if (key == "aretxtransactionschained") {
      const AbstractQoreNode* n = iter.getValue();
      Tx_transactions_chained = n ? n->getAsBool() : false;
      Tx_transactions_chained_set = true;
      continue;
    }

    //-------------------------------------------
    xsink->raiseException(err_name, "Settings value '%s' is not recognized.", key.c_str());
    return;
  } // while 

  err_name = (char*)"TUXEDO-ERROR";
  int res = init();
  if (res == -1) {
    xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpinit"), "tpinit() failed with error %d.", tperrno);
    return;
  } 
  m_Tuxedo_connection_initialized = true;
  switchToSavedContext();

  // apply the other settings
  if (priority_set) {
    int res = tpsprio(priority, TPABSOLUTE);
    if (res == -1) {
      xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpsprio"), "tpsprio() failed with error %d.", tperrno);
      return;
    }
  }

  if (finish_commit_set) {
    int res;
    if (finish_commit_after_data_logged) {
      res = tpscmt(TP_CMT_LOGGED);
    } else {
      res = tpscmt(TP_CMT_COMPLETE);
    }
    if (res == -1) {
      xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpscmt"), "tpscmt() failed with error %d.", tperrno);
      return;
    }
  }

  if (finish_Tx_commit_set) {
    int res;
    if (finish_Tx_commit_after_data_logged) {
      res = tx_set_commit_return(TX_COMMIT_DECISION_LOGGED);
    } else {
      res = tx_set_commit_return(TX_COMMIT_COMPLETED);
    }
    if (res != TX_OK) {
      xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tx_set_commit_return"), "tx_set_commit_return() failed with error %d.", tperrno);
      return;
    }
  }

  if (Tx_transactions_chained_set) {
    int res = tx_set_transaction_control(Tx_transactions_chained ? TX_CHAINED : TX_UNCHAINED);
    if (res != TX_OK) {
       xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tx_set_transaction_constrol"), "tx_set_transaction_control() failed with error %d.", res);
       return;
    }
  }

}

//------------------------------------------------------------------------------
QoreTuxedoAdapter::~QoreTuxedoAdapter()
{
  close();
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
  freeSendBuffer();

  if (m_default_fml_description) {
     m_default_fml_description->deref(0);
  }
  if (m_default_fml32_description) {
     m_default_fml32_description->deref(0);
  }

  if (!m_Tuxedo_connection_initialized) {
    return 0;
  }
  switchToSavedContext();
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
void QoreTuxedoAdapter::setStringEncoding(const char* name)
{
  m_string_encoding = QEM.findCreate(name);
  if (!m_string_encoding) m_string_encoding = QCS_DEFAULT;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::freeSendBuffer()
{
  if (m_send_buffer) {
    tpfree(m_send_buffer);
    m_send_buffer = 0;
  }
  m_send_buffer_size = 0;
}

//------------------------------------------------------------------------------
bool QoreTuxedoAdapter::allocate_send_buffer(const char* type, long size, const char* err_name, ExceptionSink* xsink)
{
  m_send_buffer = tpalloc((char*)type, 0, size);
  if (m_send_buffer) {
    m_send_buffer_size = size;
    return true;
  }
  xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpcall"), "tpcall(\"%s\", %ld) failed with error %d.", type, size, tperrno);
  return false;
}

//-----------------------------------------------------------------------------
// Allocate buffer. Get type either from passed settings hash or use the default.
pair<char*, long> QoreTuxedoAdapter::allocate_out_buffer(const char* default_type, const QoreHashNode* settings, const char* err_name, ExceptionSink* xsink)
{
  char type[20] = "STRING";
  bool type_set = false;

  if (settings) {
     const AbstractQoreNode* n = settings->getKeyValue("DefaultReturnedDataType");
    if (n) {
       const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(n);
       if (!str) {
	  xsink->raiseException(err_name, "Settings 'DefaultReturnedDataType' needs to be a string.");
	  return pair<char*, long>((char*)0, 0);
       }
       const char* s = str->getBuffer();
       if (!s && strcmp(s, "CARRAY") && strcmp(s, "STRING") && strcmp(s, "FML") && strcmp(s, "FML32")) {
	  xsink->raiseException(err_name, "Settings 'DefaultReturnedDataType': supported values are 'CARRAY', 'STRING', 'FML', 'FML32'.");
	  return pair<char*, long>((char*)0, 0);
       }
       strcpy(type, s);
       type_set = true;
    }
  }
  if (!type_set && default_type && default_type[0]) {
    strcpy(type, default_type);
  }

  pair<char*, long> result;
  result.first = tpalloc(type, 0, 4096);
  if (!result.first) {
    xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpalloc"), "tpalloc(\"%s\", 4096) failed with error %d.", type, tperrno);
    return result;
  }
  result.second = 4096;
  return result;
}

//-----------------------------------------------------------------------------
// Get flags either from passed hash (or integer) or from the default value
long QoreTuxedoAdapter::get_flags(const QoreHashNode* settings, long* pflags, long default_flags, bool default_flags_set, const char* err_name, ExceptionSink* xsink)
{
  if (pflags) return *pflags;

  if (settings) {
     const AbstractQoreNode* n = settings->getKeyValue("flags");
     return (long)(n ? n->getAsBigInt() : 0);
  }

  if (default_flags_set) return default_flags;

  xsink->raiseException(err_name, "Value of 'flags' is missing (no default in constructor, no explicit value passed).");
  return 0;
}

//------------------------------------------------------------------------------
// Return true if FML32 should be generated, false for FML
static bool is_fml32_requested(const QoreHashNode* settings, bool default_is_fml32, bool default_is_fml32_set, const char* err_name, ExceptionSink* xsink)
{
   const AbstractQoreNode* n = settings->getKeyValue((char*)"use_fml32");
   if (n)
      return n->getAsBool();
   if (default_is_fml32_set) return default_is_fml32;
   xsink->raiseException(err_name, "Neither settings 'use_fml32' nor 'DefaultFmlType' constructor parameter were specified.");
   return false;
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::setSendBuffer(const AbstractQoreNode* n, const QoreHashNode* settings, const char* err_name, ExceptionSink* xsink)
{
  freeSendBuffer();
  
  if (is_nothing(n))
    return;

  const QoreType *ntype = n->getType();

  if (ntype == NT_BINARY) {
     const BinaryNode* bin = reinterpret_cast<const BinaryNode *>(n);
     if (bin->size() == 0) return;
     if (!allocate_send_buffer("CARRAY", bin->size(), err_name, xsink)) return;
     memcpy(m_send_buffer, bin->getPtr(), bin->size());
     return;
  }

  if (ntype == NT_STRING) {
     const QoreStringNode *str = reinterpret_cast<const QoreStringNode *>(n);
     const char* s = str->getBuffer();
     if (!s || !s[0]) return;
     if (str->getEncoding() != m_string_encoding) {
	QoreString aux(s, m_string_encoding);
	if (!allocate_send_buffer("STRING", strlen(aux.getBuffer()) + 1, err_name, xsink)) return;
	strcpy(m_send_buffer, aux.getBuffer());
     } else {
	if (!allocate_send_buffer("STRING", strlen(s) + 1, err_name, xsink)) return;
	strcpy(m_send_buffer, s);
     }
     return;
  }

  if (ntype == NT_HASH) {
     const QoreHashNode *h = reinterpret_cast<const QoreHashNode *>(n);
     bool is_fml32 = is_fml32_requested(settings, m_default_is_fml32, m_default_is_fml32_set, err_name, xsink);
     if (xsink->isException()) return;
     if (!allocate_send_buffer(is_fml32 ? "FML32" : "FML", 4096, err_name, xsink)) return;
     QoreHashNode *description = is_fml32 ? m_default_fml32_description : m_default_fml_description;
     if (!description) {
	xsink->raiseException(err_name, "%s description was not specified as a constructor parameter ('DefaultFml[32]DescriptionFile' or 'DefaultFml[32]Description').", is_fml32 ? "FML32" : "FML");
	return;
     }
     setFmlDataToSend(settings, h, is_fml32, err_name, xsink);
     return;
  }

  xsink->raiseException(err_name, "Type of data to be sent is not recognized.");
}

//-----------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::buffer2node(char* buffer, long buffer_size, const char* err_name, ExceptionSink* xsink)
{
  if (!buffer || buffer_size == 0)
     return 0;

  char type[20];
  char subtype[20];
  int res = tptypes(buffer, type, subtype);
  if (res == -1) {
    QoreHashNode* h = new QoreHashNode;
    h->setKeyValue((char*)"error", new QoreBigIntNode(tperrno), xsink);
    h->setKeyValue((char*)"Tuxedo call", new QoreStringNode("tptypes"), xsink);
    return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tptypes"), "tptypes() failed with error %d.", tperrno);
  }
  
  if (!strcmp(type, "CARRAY") || !strcmp(type, "X_OCTET")) {
    void* copy = malloc(buffer_size);
    if (!copy) {
      xsink->outOfMemory();
      return 0;
    }
    BinaryNode* bin = new BinaryNode(copy, buffer_size);
    return bin;
  }

  if (!strcmp(type, "STRING")) {
    QoreString aux(buffer, m_string_encoding);
    return new QoreStringNode(aux.getBuffer());    
  }

  if (!strcmp(type, "FML32")) {
    if (!m_default_fml32_description) {
      return xsink->raiseException(err_name, "FML32 description was not specified as a constructor parameter ('DefaultFml32DescriptionFile' or 'DefaultFml32Description').");
    }

    ReferenceHolder<QoreHashNode> h(getFmlDataFromBuffer(m_default_fml32_description, true, xsink, buffer, buffer_size, err_name), xsink);
    if (*xsink) return 0;
    return h.release();
  }

  if (!strcmp(type, "FML")) {
    if (!m_default_fml_description) {
      return xsink->raiseException(err_name, "FML description was not specified as a constructor parameter ('DefaultFmlDescriptionFile' or 'DefaultFmlDescription').");
    }

    ReferenceHolder<QoreHashNode> h(getFmlDataFromBuffer(m_default_fml_description, false, xsink, buffer, buffer_size, err_name), xsink);
    if (*xsink) return 0;
    return h.release();
  }

  return xsink->raiseException(err_name, "Type [ %s ] of data is not yet supported.", type);
}

//------------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::call(const char* service_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ERROR";
  char type[20] = "";
  char subtype[20];

  if (m_send_buffer) {
    int res = tptypes(m_send_buffer, type, subtype);
    if (res == -1) {
       return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tptypes"), "tptypes() failed with error %d.", tperrno);
    }
  }

  pair<char*, long> out = allocate_out_buffer(type, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  ON_BLOCK_EXIT(tpfree, out.first);
  long flags = get_flags(call_settings, pflags, m_default_flags_for_call, m_default_flags_for_call_set, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  int res = tpcall((char *)service_name, m_send_buffer, m_send_buffer_size, &out.first, &out.second, flags);
  freeSendBuffer();
  if (res == -1) {
    return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpcall"), "tpcall() failed with error %d.", tperrno);
  }

  AbstractQoreNode* ret = buffer2node(out.first, out.second, err_name, xsink);
  if (xsink->isException()) return 0;
  return ret;
}

//------------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::acall(const char* service_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ERROR";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_acall, m_default_flags_for_acall_set, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  int res = tpacall((char *)service_name, m_send_buffer, m_send_buffer_size, flags);
  if (res == -1) {
    return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tpacall"), "tpacall() failed with error %d.", tperrno);
  }
  if (res != 0) { // 0 == no reply expected
    m_pending_async_calls.push_back(res);
  }
  return new QoreBigIntNode(res);
}

//-----------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::post_event(const char* event_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ERROR";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_post_event, m_default_flags_for_post_event_set, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }

  int res = tppost((char *)event_name, m_send_buffer, m_send_buffer_size, flags);
  if (res != -1) return 0;

  return xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tppost"), "tppost() failed with error %d.", tperrno);
}

//------------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::get_reply(int handle, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ADAPTER-WAIT-FOR-ASYNC_REPLY";

  pair<char*, long> out = allocate_out_buffer(0, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  ON_BLOCK_EXIT(tpfree, out.first);

  long flags = get_flags(call_settings, pflags, m_default_flags_for_getrply, m_default_flags_for_getrply_set, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }

  int aux_handle = handle;
  int res = tpgetrply(&aux_handle, &out.first, &out.second, flags);
  if (res == -1) {
     xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpgetrply"), "tpgetrply() failed with error %d.", tperrno);
     return 0;
  }

  AbstractQoreNode* ret = buffer2node(out.first, out.second, err_name, xsink);
  if (xsink->isException()) return 0;

  QoreHashNode *h = new QoreHashNode;
  h->setKeyValue((char*)"data", ret, xsink); 
  h->setKeyValue((char*)"handle", new QoreBigIntNode(aux_handle), xsink);
  return h;
}

//------------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::connect(const char* service_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ADAPTER-JOIN-CONVERSATION";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_connect, m_default_flags_for_connect_set, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  int res = tpconnect((char *)service_name, m_send_buffer, m_send_buffer_size, flags);
  if (res != -1) {
    return new QoreBigIntNode(res); // descriptor of the connection
  }
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpconnect"), "tpconnect() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::send(int handle, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ERROR-SEND-CONVERSATION-DATA";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_send, m_default_flags_for_send_set, err_name, xsink);
  if (xsink->isException()) return 0;
  long event = TPEV_SVCSUCC;
  int res = tpsend(handle, m_send_buffer, m_send_buffer_size, flags, &event);
  if (res != -1) {
    return new QoreBigIntNode(event);
  }
  return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpsend"), "tpsend() failed with error %d.", tperrno);
}

//-----------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::receive(int handle, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ADAPTER-RECEIVE-CONVERSATION-DATA";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_send, m_default_flags_for_send_set, err_name, xsink);
  if (xsink->isException()) return 0;
  long event = TPEV_SVCSUCC;

  pair<char*, long> out = allocate_out_buffer(0, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  ON_BLOCK_EXIT(tpfree, out.first);

  err_name = (char*)"TUXEDO-ERROR";
  int res = tprecv(handle, &out.first, &out.second, flags, &event);
  if (res == -1) {
     xsink->raiseExceptionArg(err_name, make_tuxedo_err_hash(tperrno, "tprecv"), "tprecv() failed with error %d.", tperrno);
     return 0;
  } 
    
  AbstractQoreNode* ret = buffer2node(out.first, out.second, err_name, xsink);
  if (xsink->isException()) return 0;

  QoreHashNode *h = new QoreHashNode;
  h->setKeyValue((char*)"data", ret, xsink);
  h->setKeyValue((char*)"event", new QoreBigIntNode(event), xsink);
  return h;
}

static const AbstractQoreNode* get_val(const QoreHashNode* hash, const char* name)
{
  if (!hash) return 0;
  return hash->getKeyValue(name);
}

static const QoreStringNode *get_string_val(const QoreHashNode* hash, const char* name)
{
   if (!hash) return 0;
   return dynamic_cast<const QoreStringNode *>(hash->getKeyValue(name));
}

static const BinaryNode *get_bin_val(const QoreHashNode* hash, const char* name)
{
   if (!hash) return 0;
   return dynamic_cast<const BinaryNode *>(hash->getKeyValue(name));
}

//-----------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::enqueue(const char* queue_space, const char* queue_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ADAPTER-ENQUEUE";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_enqueue, m_default_flags_for_enqueue_set, err_name, xsink);
  if (xsink->isException()) return 0;

  TPQCTL ctl;
  memset(&ctl, 0, sizeof(ctl));
  ctl.flags = TPNOFLAGS;

  if (call_settings) {
    // set queue control parameters
     const AbstractQoreNode* n = get_val(call_settings, "queue_control_flags");
    if (n) ctl.flags = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_deq_time");
    if (n) ctl.deq_time = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_priority");
    if (n) ctl.priority = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_exp_time");
    if (n) ctl.exp_time = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_delivery_qos");
    if (n) ctl.delivery_qos = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_reply_qos");
    if (n) ctl.reply_qos = (long)n->getAsBigInt();
    n = get_val(call_settings, "queue_control_urcode");
    if (n) ctl.urcode = (long)n->getAsBigInt();
    const BinaryNode *bin = get_bin_val(call_settings, "queue_control_msgid");

    if (bin) {
      unsigned sz = sizeof(ctl.msgid);
      if (bin->size() == sz) memcpy(&ctl.msgid, bin->getPtr(), sz);
    }
    bin = get_bin_val(call_settings, "queue_control_corrid");
    if (bin) {
      unsigned sz = sizeof(ctl.corrid);
      if (bin->size() == sz) memcpy(&ctl.corrid, bin->getPtr(), sz);
    }
    const QoreStringNode *str = get_string_val(call_settings, "queue_control_replyqueue");
    if (str) strcpy(ctl.replyqueue, str->getBuffer());
    str = get_string_val(call_settings, "queue_control_failurequeue");
    if (str) strcpy(ctl.failurequeue, str->getBuffer());
  }

  int res = tpenqueue((char *)queue_space, (char *)queue_name, &ctl, m_send_buffer, m_send_buffer_size, flags);
  if (res == -1) {
     xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpenqueue"), "tpenqueue() failed with error %d.", tperrno);
     return 0;
  }
  // create hash with relevant out settings
  ReferenceHolder<QoreHashNode> out(new QoreHashNode(), xsink);
  out->setKeyValue((char*)"queue_control_flags", new QoreBigIntNode(ctl.flags), xsink);

  int sz = sizeof(ctl.msgid);
  void* copy = malloc(sz);
  if (!copy) {
    xsink->outOfMemory();
    return 0;
  }
  memcpy(copy, &ctl.msgid, sz);
  BinaryNode* bin = new BinaryNode(copy, sz);
  out->setKeyValue((char*)"queue_control_msgid", bin, xsink);
  out->setKeyValue((char*)"queue_control_diagnostic", new QoreBigIntNode(ctl.diagnostic), xsink);

  if (xsink->isException())
    return 0;

  return out.release();
}

//-----------------------------------------------------------------------------
AbstractQoreNode* QoreTuxedoAdapter::dequeue(const char* queue_space, const char* queue_name, const QoreHashNode* call_settings, long* pflags, ExceptionSink* xsink)
{
  const char* err_name = (char*)"TUXEDO-ADAPTER-DEQUEUE";
  long flags = get_flags(call_settings, pflags, m_default_flags_for_dequeue, m_default_flags_for_dequeue_set, err_name, xsink);
  if (xsink->isException()) return 0;

  pair<char*, long> out = allocate_out_buffer(0, call_settings, err_name, xsink);
  if (xsink->isException()) {
    return 0;
  }
  ON_BLOCK_EXIT(tpfree, out.first);

  TPQCTL ctl;
  memset(&ctl, 0, sizeof(ctl));
  ctl.flags = TPNOFLAGS;

  if (call_settings) {
    // set queue control parameters
     const AbstractQoreNode* n = get_val(call_settings, "queue_control_flags");
    if (n) ctl.flags = (long)n->getAsBigInt();
    const BinaryNode *bin = get_bin_val(call_settings, "queue_control_msgid");
    if (bin) {
      unsigned sz = sizeof(ctl.msgid);
      if (bin->size() == sz) memcpy(&ctl.msgid, bin->getPtr(), sz);
    }
    bin = get_bin_val(call_settings, "queue_control_corrid");
    if (bin) {
      unsigned sz = sizeof(ctl.corrid);
      if (bin->size() == sz) memcpy(&ctl.corrid, bin->getPtr(), sz);
    }
  }

  int x = tpdequeue((char *)queue_space, (char *)queue_name, &ctl, &out.first, &out.second, flags);
  if (x == -1) {
    return xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tpdequeue"), "tpdequeue() failed with error %d.", tperrno);
  }
  AbstractQoreNode* retval = buffer2node(out.first, out.second, err_name, xsink);
  if (xsink->isException()) return 0;

  QoreHashNode *res = new QoreHashNode;
  res->setKeyValue((char*)"data", retval, xsink);
  
  res->setKeyValue((char*)"queue_control_flags", new QoreBigIntNode(ctl.flags), xsink);
  res->setKeyValue((char*)"queue_control_priority", new QoreBigIntNode(ctl.priority), xsink);

  int sz = sizeof(ctl.msgid);
  void* copy = malloc(sz);
  if (!copy) {
    xsink->outOfMemory();
    res->deref(xsink);
    return 0;
  }
  memcpy(copy, &ctl.msgid, sz);
  BinaryNode* bin = new BinaryNode(copy, sz);
  res->setKeyValue((char*)"queue_control_msgid", bin, xsink);

  sz = sizeof(ctl.corrid);
  copy = malloc(sz);
  if (!copy) {
    xsink->outOfMemory();
    res->deref(xsink);
    return 0;
  }
  memcpy(copy, &ctl.corrid, sz);
  bin = new BinaryNode(copy, sz);
  res->setKeyValue((char*)"queue_control_corrid", bin, xsink);

  res->setKeyValue((char*)"queue_control_delivery_qos", new QoreBigIntNode(ctl.delivery_qos), xsink);
  res->setKeyValue((char*)"queue_control_reply_qos", new QoreBigIntNode(ctl.reply_qos), xsink);
  res->setKeyValue((char*)"queue_control_replyqueue", new QoreStringNode((char*)ctl.replyqueue), xsink);
  res->setKeyValue((char*)"queue_control_failurequeue", new QoreStringNode((char*)ctl.failurequeue), xsink);
  res->setKeyValue((char*)"queue_control_diagnostic", new QoreBigIntNode(ctl.diagnostic), xsink);
  res->setKeyValue((char*)"queue_control_appkey", new QoreBigIntNode(ctl.appkey), xsink);
  res->setKeyValue((char*)"queue_control_urcode", new QoreBigIntNode(ctl.urcode), xsink);

  sz = sizeof(ctl.cltid);
  copy = malloc(sz);
  if (!copy) {
    xsink->outOfMemory();
    res->deref(xsink);
    return 0;
  }
  memcpy(copy, &ctl.cltid, sz);
  bin = new BinaryNode(copy, sz);
  res->setKeyValue((char*)"queue_control_cltid", bin, xsink);

  if (xsink->isException()) {
    res->deref(xsink);
    return 0;
  }
  return res;
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
QoreHashNode *QoreTuxedoAdapter::loadFmlDescription(const vector<string>& files, bool is_fml32, ExceptionSink* xsink)
{
  vector<string> all_names = read_names_from_all_fml_description_files(files,xsink);
  if (*xsink) return 0;
  // before returning the old variables back free the tables from memory
  // (assumption: Fldid[32] is idempotent)
  ON_BLOCK_EXIT((is_fml32 ? &Fidnm_unload : &Fidnm_unload32));

  FmlEnvironmentSetter setter(files, is_fml32);
  const char* err_name = (char*)"LOAD-FML-DESCRIPTION-ERROR";

  QoreHashNode *result = new QoreHashNode;

  for (unsigned i = 0, n = all_names.size(); i != n; ++i) {
    char* name = (char*)all_names[i].c_str();
    FLDID32 id;
    if (is_fml32) {
      id = Fldid32(name);
    } else {
      id = Fldid(name);
    }
    if (id == BADFLDID) {
      result->deref(xsink);
      xsink->raiseException(err_name, "Fldid[32](\"%s\") failed. Ferror = %d.", name, Ferror);
      return 0;
    }
    int type;
    if (is_fml32) {
      type = Fldtype32(id);
    } else {
      type = Fldtype(id);
    }

    QoreListNode* list = new QoreListNode();
    list->push(new QoreBigIntNode(id));
    list->push(new QoreBigIntNode(type));
    result->setKeyValue(name, list, xsink);
    if (xsink->isException()) {
      result->deref(xsink);
      return 0;
    }
  }
  return result;
}

//------------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::loadFmlDescription(const string& file, bool is_fml32, ExceptionSink* xsink)
{
  vector<string> files;
  files.push_back(file);
  return loadFmlDescription(files, is_fml32, xsink);
}

//------------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::generateFmlDescription(int base, const QoreHashNode* typed_names, bool is_fml32, ExceptionSink* xsink)
{
  const char* err_name = (char*)"LOAD-FML-DESCRIPTION-ERROR";

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

  ConstHashIterator iter(typed_names);
  int counter = 0;

  while (iter.next()) {
    const char* name = iter.getKey();
    const AbstractQoreNode* value = iter.getValue();
    int type = value ? value->getAsInt() : 0;
    char* type_name;

    switch (type) {
    case FLD_SHORT: type_name = (char*)"short"; break;    
    case FLD_LONG: type_name = (char*)"long"; break;
    case FLD_CHAR: type_name = (char*)"char"; break;
    case FLD_FLOAT: type_name = (char*)"float"; break;
    case FLD_DOUBLE: type_name = (char*)"double"; break;
    case FLD_STRING: type_name = (char*)"string"; break;
    case FLD_CARRAY: type_name = (char*)"carray"; break;

    case FLD_PTR:
    case FLD_FML32:
    case FLD_VIEW32:
    case FLD_MBSTRING:
      return (QoreHashNode*)xsink->raiseException(err_name, "Input hash: value of [ %s ], support for this type is not yet implemented.", name);
    default:
      return (QoreHashNode*)xsink->raiseException(err_name, "Input hash: value of [ %s ] is not recognized as a type.", name);
    }

    fprintf(f, "%s %d %s - \n", name, ++counter, type_name);
  }
  g.Dismiss();
  if (fclose(f)) {
    return (QoreHashNode*)xsink->raiseException(err_name, "Failed to create a temporary file.");
  }
  return loadFmlDescription(tmpfile, is_fml32, xsink);
}

//------------------------------------------------------------------------------
#ifdef DEBUG
static void do_test(bool is_fml32)
{
  printf("testing generateFml[32]Description()\n");
  ExceptionSink xsink;
  QoreHashNode *typed_names = new QoreHashNode();

  typed_names->setKeyValue("a_short", new QoreBigIntNode(FLD_SHORT), &xsink);
  typed_names->setKeyValue("a_long", new QoreBigIntNode(FLD_LONG), &xsink);
  typed_names->setKeyValue("a_char", new QoreBigIntNode(FLD_CHAR), &xsink);
  typed_names->setKeyValue("a_float", new QoreBigIntNode(FLD_FLOAT), &xsink);
  typed_names->setKeyValue("a_double", new QoreBigIntNode(FLD_DOUBLE), &xsink);
  typed_names->setKeyValue("a_string", new QoreBigIntNode(FLD_STRING), &xsink);
  typed_names->setKeyValue("a_carray", new QoreBigIntNode(FLD_CARRAY), &xsink);

  QoreHashNode *empty = new QoreHashNode();
  QoreTuxedoAdapter adapter(empty, &xsink);
  QoreHashNode *res = adapter.generateFmlDescription(500, typed_names, is_fml32, &xsink);
  if (xsink) {
    assert(false);
  }
  assert(res);

  HashIterator it(res);
  int counter = 0;
  while (it.next()) {
    const char* key = it.getKey();
    AbstractQoreNode* val = it.getValue();

    assert(val->type == NT_LIST);
    QoreListNode* l = reinterpret_cast<QoreListNode *>(val);
    assert(l->size() == 2);
    AbstractQoreNode* id = l->retrieve_entry(0);
    assert(id->type == NT_INT);
    FLDID32 id_val = (FLDID32)id->getAsInt();
    AbstractQoreNode* type = l->retrieve_entry(1);
    assert(type->type == NT_INT);
    int type_val = (int)type->getAsInt();
    
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
 
  res->deref(&xsink);
  assert(!xsink.isException());

  empty->derefAndDelete(&xsink);
  typed_names->derefAndDelete(&xsink);
  assert(!xsink.isException());
}

TEST()
{
  do_test(false);
  do_test(true);
}
#endif


//------------------------------------------------------------------------------
// helper
static void reallocate_buffer(char** buffer, long* buffer_size, long new_size, const char* err_name, ExceptionSink* xsink)
{
  char* res = tprealloc(*buffer, new_size);
  if (!res) {
    xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "tprealloc"), "tprealloc() failed with error %d.", tperrno);
    return;
  }
  *buffer = res;
  *buffer_size = new_size;
}

//-----------------------------------------------------------------------------
// Find out ID + type from FML name, helper
static pair<FLDID32, int> fml_name2id(const char* name, const QoreHashNode* description_info, ExceptionSink* xsink, const char* func_name)
{
  pair<FLDID32, int> result(0, 0);
  const AbstractQoreNode* n = description_info->getKeyValue(name);
  if (!n) {
     xsink->raiseException(func_name, "Field [ %s ] does not exist in FML[32] settings. Please check for typos.");
     return result;
  }
  // already known to be list with (int, int)
  assert(n->type == NT_LIST);
  const QoreListNode* l = reinterpret_cast<const QoreListNode *>(n);
  assert(l->size() == 2);
  const QoreBigIntNode *b = reinterpret_cast<const QoreBigIntNode *>(l->retrieve_entry(0));
  result.first = (FLDID32)b->val;
  b = reinterpret_cast<const QoreBigIntNode *>(l->retrieve_entry(1));
  result.second = (int)b->val;
  return result;
}

//------------------------------------------------------------------------------
// Find FML name and type from ID, helper
static pair<string, int> fml_id2name(FLDID32 id, const QoreHashNode* description_info, ExceptionSink* xsink, char* func_name)
{
   ConstHashIterator it(description_info);
   while (it.next()) {
      const AbstractQoreNode* n = it.getValue();
      assert(n->type == NT_LIST);
      const QoreListNode* l = reinterpret_cast<const QoreListNode *>(n);
      assert(l->size() == 2);
      n = l->retrieve_entry(0);
      assert(n->type == NT_INT);
      FLDID32 this_id = (FLDID32)n->getAsInt();
      if (this_id != id) continue;
      
      pair<string, int> result;
      result.first = it.getKey();
      n = l->retrieve_entry(1);
      assert(n->type == NT_INT);
      result.second = (int)n->getAsInt();
      return result;
   }
   xsink->raiseException(func_name, "A FML[32] ID not found in description table.");
   return pair<std::string, int>("", 0);
}

//------------------------------------------------------------------------------
void QoreTuxedoAdapter::add_fml_value_into_send_buffer(const char* value_name, FLDID32 id, int value_type, const AbstractQoreNode* value, bool is_fml32, const char* err_name, ExceptionSink* xsink)
{
  char* ptr_val;
  FLDLEN32 value_length;

  union {
    short short_value;
    long long_value;
    char char_value;
    float float_value;
    double double_value;
  };
  auto_ptr<QoreString> encoded_string;
  
  switch (value_type) {
    case FLD_SHORT: 
    {
       const QoreBigIntNode *b = dynamic_cast<const QoreBigIntNode *>(value);
       if (!b) {
	  xsink->raiseException(err_name, "Value [ %s ] needs to be integer.", value_name);
	  return;
       }
       int64 val = b->val;
       if (val < SHRT_MIN || val > SHRT_MAX) {
	  xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting short type.", value_name);
	  return;
       }
       short_value = (short)val;
       ptr_val = (char*)&short_value;
       value_length = sizeof(short);
       break;      
    }

    case FLD_LONG:
    {
       int64 val = value ? value->getAsBigInt() : 0;
       if (val < LONG_MIN || val > LONG_MAX) {
	  xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting long type.", value_name);
	  return;
       }
       long_value = (long)val;
       ptr_val = (char*)&long_value;
       value_length = sizeof(long);
       break;
    } 

    case FLD_CHAR: 
    {
       int val = value ? value->getAsInt() : 0;
       if (val < CHAR_MIN || val > CHAR_MAX) {
	  xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting char type.", value_name);
	  return;
       }
       char_value = (char)val;
       ptr_val = &char_value;
       value_length = sizeof(char);
       break;
    }

    case FLD_FLOAT: 
    {
       double val = value ? value->getAsFloat() : 0.0;
       if (val < FLT_MIN || val > FLT_MAX) {
	  xsink->raiseException(err_name, "Value [ %s ] doesn't contain value fitting float type.", value_name);
	  return;
       }
       float_value = (float)val;
       ptr_val = (char*)&float_value;
       value_length = sizeof(float);
       break;
    }

     case FLD_DOUBLE: 
     {
	double_value = value ? value->getAsFloat() : 0.0;
	ptr_val = (char*)&double_value;
	value_length = sizeof(double);
	break;
     }

    case FLD_STRING:
    {
       const QoreStringNode *str = dynamic_cast<const QoreStringNode *>(value);
       
       if (!str) {
	  xsink->raiseException(err_name, "Value [ %s ] needs to be a string.", value_name);
	  return;
       }
       const char* s = str->getBuffer();
       if (s && s[0] && str->getEncoding() != m_string_encoding) {
	  encoded_string.reset(new QoreString(s, m_string_encoding));
	  s = encoded_string->getBuffer();
       }
       if (!s) s = "";
       ptr_val = (char *)s;
       value_length = strlen(s) + 1; 
       break;
    }

    case FLD_CARRAY:
    {
       const BinaryNode *bin = dynamic_cast<const BinaryNode *>(value);
       if (!bin) {
	  xsink->raiseException(err_name, "Value [ %s ] needs to be a binary.", value_name);
	  return;
       }
       ptr_val = (char*)bin->getPtr();
       if (!ptr_val) ptr_val = (char*)"";
       value_length = bin->size();
       break;
    }

    case FLD_PTR:
    case FLD_FML32:
    case FLD_VIEW32:
    case FLD_MBSTRING:
    default:
      xsink->raiseException(err_name, "Value [ %s ] has unrecognized type %d.", value_name, value_type);
      return;
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
	xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "Fappend[32]"), "Value '%s' cannot be appended into FML[32] buffer. Error %d.", value_name, (int)Ferror);
	return;
     }
     // the buffer needs to be resized
     int increment = m_send_buffer_size;
     if (increment > 64 * 1024) {
	increment = 64 * 1024; // guesed value
     }
     reallocate_buffer(&m_send_buffer, &m_send_buffer_size, m_send_buffer_size + increment, err_name, xsink);
     if (xsink->isException()) return;
  }
}


//------------------------------------------------------------------------------
void QoreTuxedoAdapter::setFmlDataToSend(const QoreHashNode* description_info, const QoreHashNode* data, bool is_fml32, const char* err_name, ExceptionSink* xsink)
{
  assert(m_send_buffer);
  int res;
  if (is_fml32) {
    res = Finit32((FBFR32*)m_send_buffer, m_send_buffer_size);
    assert(Fielded32((FBFR32*)m_send_buffer));
  } else {
    res = Finit((FBFR*)m_send_buffer, m_send_buffer_size);
    assert(Fielded((FBFR*)m_send_buffer));
  }
  if (res == -1) {
    xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "Finit[32]"), "Finit[32] failed with error %d.", (int)Ferror);
    return;
  }
  // append every item in the buffer
  ConstHashIterator iter(data);
  int index = 0;
  while (iter.next()) {
    ++index;
    const char* complete_name = iter.getKey();
    if (!complete_name || !complete_name[0]) {
      xsink->raiseException(err_name, "Name of item # %d is empty.", index);
      return;
    }
    // get rid of caret
    string name(complete_name);
    char* thumb = strchr((char*)name.c_str(), '^');
    if (thumb) *thumb = 0;
    
    const AbstractQoreNode* value = iter.getValue();

    pair<FLDID32, int> id_type = fml_name2id((char*)name.c_str(), description_info, xsink, err_name);
    if (xsink->isException()) {
      return;
    }

    const QoreListNode *l = dynamic_cast<const QoreListNode *>(value);
    if (l) {
      // explode the list members
      for (int i = 0, cnt = l->size(); i != cnt; ++i) {
	 const AbstractQoreNode* subvalue = l->retrieve_entry(i);
	 add_fml_value_into_send_buffer((char*)name.c_str(), id_type.first, id_type.second, subvalue, is_fml32, err_name, xsink);
	 if (xsink->isException()) return;
      }
    } else {
       add_fml_value_into_send_buffer((char*)name.c_str(), id_type.first, id_type.second, value, is_fml32, err_name, xsink);
       if (xsink->isException()) return;
    }
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
    xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "Findex[32]"), "Findex[32] failed with error %d.", (int)Ferror);
    return;
  }

  // set up proper buffer size 
  long result_size;
  if (is_fml32) {
    result_size = Fsizeof32((FBFR32*)m_send_buffer);
  } else {
    result_size = Fsizeof((FBFR*)m_send_buffer);
  }
  if (result_size == -1) {
    xsink->raiseExceptionArg("TUXEDO-ERROR", make_tuxedo_err_hash(tperrno, "Fsizeof[32]"), "Fsizeof[32] failed with error %d.", (int)Ferror);
    return;
  }
  assert(result_size);
  m_send_buffer_size = (int)result_size;
}


//------------------------------------------------------------------------------
QoreHashNode *QoreTuxedoAdapter::getFmlDataFromBuffer(const QoreHashNode* description_info, bool is_fml32, 
						      ExceptionSink* xsink, const char* buffer, long buffer_size, const char* err_name)
{
 
  if (!buffer) {
    xsink->raiseException(err_name, "The typed buffer is empty.");
    return 0;
  }
  char type[20];
  char subtype[20];
  if (tptypes((char *)buffer, type, subtype) == -1) {
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

  ReferenceHolder<QoreHashNode> out_hash(new QoreHashNode(), xsink);

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
       item_info  = fml_id2name(fldid32, description_info, xsink, (char *)err_name);
    } else {
       item_info = fml_id2name(fldid, description_info, xsink, (char *)err_name);
    }
    if (xsink->isException())
      return 0;

    assert(!item_info.first.empty());
    AbstractQoreNode* result_value = 0;
    
    switch (item_info.second) {
    case FLD_SHORT:
    {
      if (actual_value_length != sizeof(short)) {
        xsink->raiseException(err_name, "FML[32] type short, invalid data length %d.", actual_value_length);
        return 0;
      }
      short val;
      memcpy(&val, value_buffer, sizeof(short));
      result_value = new QoreBigIntNode(val);
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
      result_value = new QoreBigIntNode(val);
      break;
    }

    case FLD_CHAR: 
    {
      if (actual_value_length != sizeof(char)) {
        xsink->raiseException(err_name, "FML[32] type char, invalid data length %d.", actual_value_length);
        return 0;
      }
      result_value = new QoreBigIntNode(value_buffer[0]);
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
      result_value = new QoreFloatNode(val);
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
      result_value = new QoreFloatNode(val);
      break;
    }

    case FLD_STRING: 
    {
      assert(value_buffer[actual_value_length] == 0);
      result_value = new QoreStringNode(value_buffer);
      break;
    }

    case FLD_CARRAY: 
    {
      if (actual_value_length == 0) {
        result_value = new BinaryNode;
      } else {
        char* copy = (char*)malloc(actual_value_length);
        if (!copy) {
          xsink->outOfMemory();
          return 0;
        }
        result_value = new BinaryNode(copy, actual_value_length);
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

    out_hash->setKeyValue((char*)item_info.first.c_str(), result_value, xsink);
    if (xsink->isException())
       return 0;
  } // for
  
  return out_hash.release();
}

//------------------------------------------------------------------------------
#ifdef DEBUG
static void do_test2(bool is_fml32)
{
  printf("testing setFml[32]DataToSend()/getFmlDataFromBuffer()\n");
  ExceptionSink xsink;
  QoreHashNode *typed_names = new QoreHashNode();

  typed_names->setKeyValue("a_short", new QoreBigIntNode(FLD_SHORT), &xsink);
  typed_names->setKeyValue("a_long", new QoreBigIntNode(FLD_LONG), &xsink);
  typed_names->setKeyValue("a_char", new QoreBigIntNode(FLD_CHAR), &xsink);
  typed_names->setKeyValue("a_float", new QoreBigIntNode(FLD_FLOAT), &xsink);
  typed_names->setKeyValue("a_double", new QoreBigIntNode(FLD_DOUBLE), &xsink);
  typed_names->setKeyValue("a_string", new QoreBigIntNode(FLD_STRING), &xsink);
  typed_names->setKeyValue("a_carray", new QoreBigIntNode(FLD_CARRAY), &xsink);

  QoreTuxedoAdapter adapter;
  QoreHashNode* res = adapter.generateFmlDescription(500, typed_names, is_fml32, &xsink);
  if (xsink) {
    assert(false);
  }
  assert(res);

  QoreHashNode* data = new QoreHashNode;
  data->setKeyValue((char*)"a_long", new QoreBigIntNode(12345678), &xsink);
  data->setKeyValue((char*)"a_string", new QoreStringNode("string1"), &xsink);
  data->setKeyValue((char*)"a_string", new QoreStringNode("string2"), &xsink);
  assert(!xsink.isException());

  char* type = (char*)(is_fml32 ? "FML32" : "FML");
  adapter.m_send_buffer = tpalloc(type, 0, 4096);
  assert(adapter.m_send_buffer);
  adapter.m_send_buffer_size = 4096;
/*
  adapter.setFmlDataToSend(res, data, is_fml32, (char*)"a test routine", &xsink);
  if (xsink.isException()) {
    assert(false);
  }
  assert(adapter.m_send_buffer);

  // read the data back
  QoreHashNode *extracted_data = adapter.getFmlDataFromBuffer(res, is_fml32, &xsink,
    adapter.m_send_buffer, adapter.m_send_buffer_size, "a test routine");
  if (xsink.isException()) {
    assert(false);
  }
  assert(extracted_data);

  assert(extracted_data->size() == 3);
  // value 1
  AbstractQoreNode* n = extracted_data->retrieve_entry(0);
  assert(n->type == NT_LIST);
  sublist = reinterpret_cast<QoreListNode *>(n);
  assert(sublist->size() == 2);
  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  char* s = n->getBuffer();
  if (strcmp(s, "a_long") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_INT);
  assert(n->getAsBigInt() == 12345678);

  // value 2
  n = extracted_data->retrieve_entry(1);
  assert(n->type == NT_LIST);
  sublist = reinterpret_cast<QoreListNode *>(n);
  assert(sublist->size() == 2);

  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  s = n->getBuffer();
  if (strcmp(s, "a_string") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_STRING);
  s = n->getBuffer();
  if (strcmp(s, "string1") != 0) {
    assert(false);
  }

  // value 3
  n = extracted_data->retrieve_entry(2);
  assert(n->type == NT_LIST);
  sublist = reinterpret_cast<QoreListNode *>(n);
  assert(sublist->size() == 2);

  n = sublist->retrieve_entry(0);
  assert(n->type == NT_STRING);
  s = n->getBuffer();
  if (strcmp(s, "a_string") != 0) {
    assert(false);
  }
  n = sublist->retrieve_entry(1);
  assert(n->type == NT_STRING);
  s = n->getBuffer();
  if (strcmp(s, "string2") != 0) {
    assert(false);
  }
  extracted_data->deref(&xsink);

  // cleanup
  res->deref(&xsink);
  data->derefAndDelete(&xsink);
*/
  typed_names->derefAndDelete(&xsink);
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

