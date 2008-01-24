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

#include <qore/Qore.h>

#include <string>
#include <vector>
#include <utility>
#include <map>

#include <atmi.h>
#include <fml32.h>

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
// empty class, used just to check whether the module works
class QoreTuxedoTest : public AbstractPrivateData
{
public:
  virtual ~QoreTuxedoTest() { }
};
#endif

static inline QoreHashNode *make_tuxedo_err_hash(int terrno, const char *errstr)
{
   QoreHashNode *h = new QoreHashNode();
   h->setKeyValue((char*)"error", new QoreNode((int64)terrno), 0);
   h->setKeyValue((char*)"Tuxedo call", new QoreStringNode(errstr), 0);
   return h;
}

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

  const QoreEncoding* m_string_encoding; // used to convert every string sent/received

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
  QoreHashNode *m_default_fml_description;
  QoreHashNode *m_default_fml32_description;
  
  int init();
  int close();
  int saveContext();

  void add_fml_value_into_send_buffer(const char* value_name, FLDID32 id, int value_type, QoreNode* value, 
    bool is_fml32, const char* err_name, ExceptionSink* xsink);
  void setFmlDataToSend(QoreHash* description_info, QoreHash* data, bool is_fml32, const char* err_name, ExceptionSink* xsink);
  QoreHashNode *getFmlDataFromBuffer(QoreHash* description_info, bool is_fml32, ExceptionSink* xsink, const char* buffer, long buffer_size, const char* err_name);

  // FML/FML32 - load the description from tables (possibly generate the tables temporarily)
  QoreHashNode* loadFmlDescription(const std::vector<std::string>& files, bool is_fml32, ExceptionSink* xsink);
  QoreHashNode* loadFmlDescription(const std::string& file, bool is_fml32, ExceptionSink* xsink);
  QoreHashNode* generateFmlDescription(int base, QoreHash* typed_names, bool is_fml32, ExceptionSink* xsink);

  QoreNode* buffer2node(char* buffer, long buffer_size, const char* err_name, ExceptionSink* xsink);
  bool allocate_send_buffer(const char* type, long size, const char* err_name, ExceptionSink* xsink);

  std::pair<char*, long> allocate_out_buffer(const char* default_type, QoreHash* settings, const char* err_name, ExceptionSink* xsink);
  long get_flags(QoreHash* settings, long* pflags, long default_flags, bool default_flags_set, const char* err_name, ExceptionSink* xsink);

public:
  QoreTuxedoAdapter(QoreHash* settings, ExceptionSink* xsink);
#ifdef DEBUG
  QoreTuxedoAdapter(); // just for testing
#endif
  virtual ~QoreTuxedoAdapter();

  int switchToSavedContext() const;
  void setStringEncoding(const char* name);

  void setSendBuffer(QoreNode* n, QoreHash* settings, const char* err_name, ExceptionSink* xsink);
  void freeSendBuffer();

  QoreNode* call(const char* service_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* acall(const char* service_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreHashNode* get_reply(int handle, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* post_event(const char* event_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* connect(const char* service_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreNode* send(int handle, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);
  QoreHashNode* receive(int handle, QoreHash* call_settings, long* pflags, ExceptionSink* xsink); 
  QoreHashNode* enqueue(const char* queue_space, const char* queue_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink); 
  QoreNode* dequeue(const char* queue_space, const char* queue_name, QoreHash* call_settings, long* pflags, ExceptionSink* xsink);

  void remove_pending_async_call(int handle);

};

#endif

// EOF

