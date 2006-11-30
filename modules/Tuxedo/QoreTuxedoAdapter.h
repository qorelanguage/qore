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
#include <qore/Object.h>

#include <string>
#include <vector>
#include <utility>
#include <map>

#include <atmi.h>
#include <fml32.h>

class QoreEncoding;
class Hash;

//------------------------------------------------------------------------------
#ifdef DEBUG
// Values to be used as environment variables, names and passwords for tests.
// Undefine them to skip the tests that use them.
//

#define TUXDIR  "/opt/bea/tuxedo9.1"

// The simplest test with TOUPPER service on strings
#define TUXCONFIG_SIMPLE_TEST "tuxedo_tests/tuxedo_simple_app/tuxconfig"

// Queue sample
#define TUXCONFIG_QUEUE_TEST  "tuxedo_tests/qsample/tuxconfig"

#endif

//-----------------------------------------------------------------------------
#ifdef DEBUG
class QoreTuxedoTest : public AbstractPrivateData
{
public:
  virtual ~QoreTuxedoTest() { }
};

// test inheritance from builtin class
class QoreTuxedoTestBase : public AbstractPrivateData
{
public:
  virtual ~QoreTuxedoTestBase() {}
};
#endif

//------------------------------------------------------------------------------
class QoreTuxedoAdapter : public AbstractPrivateData
{
public:  // public for now so the QC_TuxedoAdapter has access to it

  // data used to connect Tuxedo server
  std::string m_username;
  std::string m_clientname;
  std::string m_groupname;
  std::string m_password;
  typedef std::vector<std::pair<std::string, std::string> > env_var_t;
  env_var_t m_env_variables;
  std::vector<char> m_binary_data;
  long m_connection_flags;
  bool m_Tuxedo_connection_initialized;

  TPCONTEXT_T m_context;

  QoreEncoding* m_string_encoding; // used to convert every string sent/received

  std::vector<int> m_pending_async_calls;
  TPQCTL m_queue_settings;

  std::map<int, TPTRANID> m_suspended_transactions; // tpsuspend/tpresume, maps id -> transaction
  int m_last_suspended_transaction_id; // used to generate unique handle for every suspended transcation in the map

  char* m_send_buffer;
  long m_send_buffer_size;

  // if these flags are set in TuxedoAdapter constructor they do not needs to be specified in given function
  long m_default_flags_for_call;
  bool m_default_flags_for_call_set;
  long m_default_flags_for_acall;
  bool m_default_flags_for_acall_set;
  long m_default_flags_for_getrply;
  bool m_default_flags_for_getrply_set;
  long m_default_flags_for_post_event;
  bool m_default_flags_for_post_event_set;
  long m_default_flags_for_connect;
  bool m_default_flags_for_connect_set;
  long m_default_flags_for_send;
  bool m_default_flags_for_send_set;
  long m_default_flags_for_receive;
  bool m_default_flags_for_receive_set;
  long m_default_flags_for_enqueue;
  bool m_default_flags_for_enqueue_set;
  long m_default_flags_for_dequeue;
  bool m_default_flags_for_dequeue_set;

  // if there's no explicit info whether we use FML or FML32 this will be the default
  bool m_default_is_fml32;
  bool m_default_is_fml32_set;

  // FML/FML32 descriptions are always set in constructor
  Hash* m_default_fml_description;
  Hash* m_default_fml32_description;
  
  int init();
  int close();
  int saveContext();

  void add_fml_value_into_send_buffer(char* value_name, FLDID32 id, int value_type, QoreNode* value, 
    bool is_fml32, char* err_name, ExceptionSink* xsink);
  void setFmlDataToSend(Hash* description_info, Hash* data, bool is_fml32, char* err_name, ExceptionSink* xsink);
  Hash* getFmlDataFromBuffer(Hash* description_info, bool is_fml32, ExceptionSink* xsink, char* buffer, long buffer_size, char* err_name);

  // FML/FML32 - load the description from tables (possibly generate the tables temporarily)
  Hash* loadFmlDescription(const std::vector<std::string>& files, bool is_fml32, ExceptionSink* xsink);
  Hash* loadFmlDescription(const std::string& file, bool is_fml32, ExceptionSink* xsink);
  Hash* generateFmlDescription(int base, Hash* typed_names, bool is_fml32, ExceptionSink* xsink);

  QoreNode* buffer2node(char* buffer, long buffer_size, char* err_name, ExceptionSink* xsink);
  bool allocate_send_buffer(const char* type, long size, char* err_name, ExceptionSink* xsink);

  std::pair<char*, long> allocate_out_buffer(char* default_type, Hash* settings, char* err_name, ExceptionSink* xsink);
  long get_flags(Hash* settings, long* pflags, long default_flags, bool default_flags_set, char* err_name, ExceptionSink* xsink);

public:
  QoreTuxedoAdapter(Hash* settings, ExceptionSink* xsink);
#ifdef DEBUG
  QoreTuxedoAdapter(); // just for testing
#endif
  virtual ~QoreTuxedoAdapter();

  int switchToSavedContext() const;
  void setStringEncoding(char* name);

  void setSendBuffer(QoreNode* n, Hash* settings, char* err_name, ExceptionSink* xsink);
  void freeSendBuffer();

  QoreNode* call(char* service_name, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* acall(char* service_name, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* get_reply(int handle, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* post_event(char* event_name, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* connect(char* service_name, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* send(int handle, Hash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* receive(int handle, Hash* call_settings, long* pflags, ExceptionSink* xsink); 
  QoreNode* enqueue(char* queue_space, char* queue_name, Hash* call_settings, long* pflags, ExceptionSink* xsink); 
  QoreNode* dequeue(char* queue_space, char* queue_name, Hash* call_settings, long* pflags, ExceptionSink* xsink);

  void remove_pending_async_call(int handle);

};

#endif

// EOF

