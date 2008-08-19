/*
  sybase_connection.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library or FreeTDS' ct-lib

  Qore Programming Language

  Copyright (C) 2007 Qore Technolgoies sro

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

#include <assert.h>
#include <memory>

#include <ctpublic.h>

#include "connection.h"
#include "encoding_helpers.h"
#include "sybase_query.h"
#include "command.h"

static QoreString ver_str("begin tran select @@version commit tran");

//------------------------------------------------------------------------------
connection::connection()
   : m_context(0), m_connection(0), connected(false)
{
}

//------------------------------------------------------------------------------
connection::~connection()
{
   CS_RETCODE ret = CS_SUCCEED;

   if (m_connection) {
      if (connected)
      {
	 ret = ct_close(m_connection, CS_UNUSED);
	 if (ret != CS_SUCCEED)
	 {
	    ret = ct_close(m_connection, CS_FORCE_CLOSE);
	    assert(ret == CS_SUCCEED);
	 }
      }
      ret = ct_con_drop(m_connection);
      assert(ret == CS_SUCCEED);
   }

   if (m_context) {
      ret = ct_exit(m_context, CS_UNUSED);
      if (ret != CS_SUCCEED)
      {
	 ret = ct_exit(m_context, CS_FORCE_EXIT);
	 assert(ret == CS_SUCCEED);
      }
      ret = cs_ctx_drop(m_context);
      assert(ret == CS_SUCCEED);
   }
}

int connection::direct_execute(const char* sql_text, ExceptionSink* xsink)
{
   assert(sql_text && sql_text[0]);

   CS_COMMAND* cmd = 0;

   CS_RETCODE err = ct_cmd_alloc(m_connection, &cmd);
   if (err != CS_SUCCEED)
      return do_exception(xsink, "DBI:SYBASE:ERROR", "ct_cmd_alloc() failed");

   ON_BLOCK_EXIT(ct_cmd_drop, cmd);
   ScopeGuard canceller = MakeGuard(ct_cancel, (CS_CONNECTION*)0, cmd, CS_CANCEL_ALL);
   
   err = ct_command(cmd, CS_LANG_CMD, (CS_CHAR*)sql_text, strlen(sql_text), CS_END);
   if (err != CS_SUCCEED)
      return do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_command() failed");

   err = ct_send(cmd);
   if (err != CS_SUCCEED)
      return do_exception(xsink, "DBI-EXEC-EXCEPTION", "ct_send() failed");

   // no results expected
   CS_INT result_type;
   err = ct_results(cmd, &result_type);
   if (err != CS_SUCCEED)
      return do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() failed");
   
   if (result_type != CS_CMD_SUCCEED)
      return do_exception(xsink, "DBI:SYBASE:EXEC-ERROR", "ct_results() failed");

   while((err = ct_results(cmd, &result_type)) == CS_SUCCEED);
   canceller.Dismiss();

   return purge_messages(xsink);
}

AbstractQoreNode *connection::exec_intern(class QoreString *cmd_text, const QoreListNode *qore_args, bool need_list, ExceptionSink* xsink)
{
   sybase_query query;
   if (query.init(cmd_text, qore_args, xsink))
      return 0;

   printd(5, "connection::exec_intern() sql='%s'\n", cmd_text->getBuffer());
   command cmd(*this, xsink);
   if (xsink->isException())
      return 0;

   if (cmd.initiate_language_command(query.m_cmd->getBuffer(), xsink))
      return 0;

   if (!query.param_list.empty() && cmd.set_params(query, qore_args, xsink))
      return 0;

   if (cmd.send(xsink))
      return 0;

   ReferenceHolder<AbstractQoreNode> result(cmd.read_output(query.placeholder_list, need_list, xsink), xsink);
   if (*xsink)
      return 0;

   //printd(5, "execute_command_impl() result=%08p (%lld)\n", result, result && result->getType() == NT_INT ? result->getAsBigInt() : 0LL);
   return result.release();
}

AbstractQoreNode *connection::exec(const QoreString *cmd, const QoreListNode *parameters, ExceptionSink *xsink)
{
   // copy the string here for intrusive editing, convert encoding too if necessary
   class QoreString *query = cmd->convertEncoding(enc, xsink);
   if (!query)
      return 0;

   std::auto_ptr<QoreString> tmp(query);
   return exec_intern(query, parameters, false, xsink);
}

AbstractQoreNode *connection::exec_rows(const QoreString *cmd, const QoreListNode *parameters, ExceptionSink *xsink)
{
   // copy the string here for intrusive editing, convert encoding too if necessary
   class QoreString *query = cmd->convertEncoding(enc, xsink);
   if (!query)
      return 0;

   std::auto_ptr<QoreString> tmp(query);
   return exec_intern(query, parameters, true, xsink);
}

// returns 0=OK, -1=error (exception raised)                                                                                                                                      
int connection::commit(ExceptionSink *xsink)
{
   return direct_execute("commit", xsink);
}

// returns 0=OK, -1=error (exception raised)                                                                                                                                      
int connection::rollback(ExceptionSink *xsink)
{
   return direct_execute("rollback", xsink);
}

// Post-constructor initialization 
int connection::init(const char* username, const char* password, const char* dbname, const char *db_encoding, const QoreEncoding *n_enc, ExceptionSink* xsink)
{
  assert(!m_connection);
  assert(!m_context);

  printd(5, "connection::init() user=%s pass=%s dbname=%s, db_enc=%s\n", username, password ? password : "<n/a>", dbname, db_encoding ? db_encoding : "<n/a>");

  enc = n_enc;

  CS_RETCODE ret = cs_ctx_alloc(CS_VERSION_100, &m_context);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-CANNOT-ALLOCATE-ERROR", "cs_ctx_alloc() failed with error %d", ret);
    return -1;
  }

  ret = ct_init(m_context, CS_VERSION_100);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-INIT-FAILED", "ct_init() failed with error %d", ret);
    return -1;
  }

/*
  // add callbacks
  ret = ct_callback(m_context, 0, CS_SET, CS_CLIENTMSG_CB, (CS_VOID*)clientmsg_callback);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-CALLBACK", "ct_callback(CS_CLIENTMSG_CB) failed with error %d", ret);
    return -1;
  }
  ret = ct_callback(m_context, 0, CS_SET, CS_SERVERMSG_CB, (CS_VOID*)servermsg_callback);
  if (ret != CS_SUCCEED) {
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-CALLBACK", "ct_callback(CS_SERVERMSG_CB) failed with error %d", ret);
    return -1;
  }
*/  
   ret = ct_con_alloc(m_context, &m_connection);
   if (ret != CS_SUCCEED) {
      xsink->raiseException("DBI:SYBASE:CT-LIB-CREATE-CONNECTION", "ct_con_alloc() failed with error %d", ret);
      return -1;
   }

   // set inline message handling
   ret = ct_diag(m_connection, CS_INIT, CS_UNUSED, CS_UNUSED, 0);
   if (ret != CS_SUCCEED)
   {
      xsink->raiseException("DBI:SYBASE:CT-LIB-INIT-ERROR-HANDLING-ERROR", "ct_diag(CS_INIT) failed with error %d, unable to initialize inline error handling", ret);
      return -1;
   }

   ret = ct_con_props(m_connection, CS_SET, CS_USERNAME, (CS_VOID*)username, CS_NULLTERM, 0);
   if (ret != CS_SUCCEED) {
      xsink->raiseException("DBI:SYBASE:CT-LIB-SET-USERNAME", "ct_con_props(CS_USERNAME) failed with error %d", ret);
      return -1;
   }
   if (password && password[0]) {
      ret = ct_con_props(m_connection, CS_SET, CS_PASSWORD, (CS_VOID*)password, CS_NULLTERM, 0);
      if (ret != CS_SUCCEED) {
	 xsink->raiseException("DBI:SYBASE:CT-LIB-SET-PASSWORD", "ct_con_props(CS_PASSWORD) failed with error %d", ret);
	 return -1;
      }
   }

#if defined(SYBASE) || defined(FREETDS_LOCALE)
   CS_LOCALE *m_charset_locale = 0;

   ret = cs_loc_alloc(m_context, &m_charset_locale);
   if (ret != CS_SUCCEED) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_loc_alloc() returned error %d", (int)ret);
      return -1;
   }
   ret = cs_locale(m_context, CS_SET, m_charset_locale, CS_LC_ALL, 0, CS_NULLTERM, 0);
   if (ret != CS_SUCCEED) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_locale(CS_LC_ALL) returned error %d", (int)ret);
      return -1;
   }
   ret = cs_locale(m_context, CS_SET, m_charset_locale, CS_SYB_CHARSET, (CS_CHAR*)db_encoding, CS_NULLTERM, 0);
   if (ret != CS_SUCCEED) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_locale(CS_SYB_CHARSET, '%s') failed with error %d", db_encoding, (int)ret);
      return -1;
   }
   ret = ct_con_props(m_connection, CS_SET, CS_LOC_PROP, m_charset_locale, CS_UNUSED, 0);
   if (ret !=CS_SUCCEED) {
      xsink->raiseException("DBI-EXEC-EXCEPTION", "ct_con_props(CS_SET, CS_LOC_PROP) failed with error %d", (int)ret);
      return -1;
   }

   ret = cs_loc_drop(m_context, m_charset_locale);
   assert(ret == CS_SUCCEED);
#endif

   ret = ct_connect(m_connection, (CS_CHAR*)dbname,  strlen(dbname));
   if (ret != CS_SUCCEED) {
      do_exception(xsink, "DBI:SYBASE:CT-LIB-CONNECT", "ct_connect() failed with error %d", ret);
      //xsink->raiseException("DBI:SYBASE:CT-LIB-CONNECT", "ct_connect() failed with error %d", ret);
      return -1;
   }
   connected = true;
   
   // turn on chained transaction mode, this fits with Qore's transaction management approach
   // - in autocommit mode qore executes a commit after every request manually
   CS_BOOL cs_bool = CS_TRUE;
   ret = ct_options(m_connection, CS_SET, CS_OPT_CHAINXACTS, &cs_bool, CS_UNUSED, 0);
   if (ret != CS_SUCCEED)
      return do_exception(xsink, "DBI:SYBASE:INIT-ERROR", "ct_options(CS_OPT_CHAINXACTS) failed");

   // Set default type of string representation of DATETIME to long (like Jan 1 1990 12:32:55:0000 PM)
   // Without this some routines in conversions.cc would fail.
   CS_INT aux = CS_DATES_LONG;
   ret = cs_dt_info(m_context, CS_SET, NULL, CS_DT_CONVFMT, CS_UNUSED, (CS_VOID*)&aux, sizeof(aux), 0);
   if (ret != CS_SUCCEED)
      return do_exception(xsink, "DBI:SYBASE:INIT-ERROR", "cs_dt_info(CS_DT_CONVFMT) failed");

   return purge_messages(xsink);
}

// purges all outstanding messages using ct_diag
int connection::purge_messages(ExceptionSink *xsink)
{
   int rc = 0;
   // make sure no messages have severity > 10
   int num;
   CS_RETCODE ret = ct_diag(m_connection, CS_STATUS, CS_CLIENTMSG_TYPE, CS_UNUSED, &num);
   assert(ret == CS_SUCCEED);
   CS_CLIENTMSG cmsg;
   for (int i = 1; i <= num; ++i)
   {
      ret = ct_diag(m_connection, CS_GET, CS_CLIENTMSG_TYPE, i, &cmsg);
      assert(ret == CS_SUCCEED);
      if (CS_SEVERITY(cmsg.msgnumber) > 10)
      {
	 QoreStringNode *desc = new QoreStringNode();
	 desc->sprintf("client message %d, severity %d: %s", CS_NUMBER(cmsg.msgnumber), CS_SEVERITY(cmsg.msgnumber), cmsg.msgstring);
	 if (cmsg.osstringlen)
	    desc->sprintf(": %s", cmsg.osstring);
	 xsink->raiseException("DBI:SYBASE:CLIENT-ERROR", desc);
	 rc = -1;
      }
#ifdef DEBUG
      printd(1, "client: severity:%d, n:%d: %s\n", CS_SEVERITY(cmsg.msgnumber), CS_NUMBER(cmsg.msgnumber), cmsg.msgstring);
      if (cmsg.osstringlen > 0)
	 printd(1, "Operating System Error: %s\n", cmsg.osstring);
#endif
   }
   ret = ct_diag(m_connection, CS_STATUS, CS_SERVERMSG_TYPE, CS_UNUSED, &num);
   assert(ret == CS_SUCCEED);
   CS_SERVERMSG smsg;
   for (int i = 1; i <= num; ++i)
   {
      ret = ct_diag(m_connection, CS_GET, CS_SERVERMSG_TYPE, i, &smsg);
      assert(ret == CS_SUCCEED);
      if (smsg.severity > 10)
      {
	 QoreStringNode *desc = new QoreStringNode();
	 desc->sprintf("server message %d, ", smsg.msgnumber);
	 if (smsg.line)
	    desc->sprintf("line %d, ", smsg.line);
	 desc->sprintf("severity %d: %s", smsg.severity, smsg.text);
	 desc->trim_trailing('\n');
	 xsink->raiseException("DBI:SYBASE:SERVER-ERROR", desc);
	 rc = -1;
      }
      printd(1, "server: line:%d, severity:%d, n:%d: %s", smsg.line, smsg.severity, smsg.msgnumber, smsg.text);
   }
   ret = ct_diag(m_connection, CS_CLEAR, CS_ALLMSG_TYPE, CS_UNUSED, 0);
   assert(ret == CS_SUCCEED);
   return rc;
}

int connection::do_exception(ExceptionSink *xsink, const char *err, const char *fmt, ...)
{
   class QoreStringNode *estr = new QoreStringNode();
   va_list args;
   while (fmt)
   {
      va_start(args, fmt);
      int rc = estr->vsprintf(fmt, args);
      va_end(args);
      if (!rc)
      {
	 estr->concat(": ");
         break;
      }
   }

   int count = 0;
   int num;
   CS_RETCODE ret = ct_diag(m_connection, CS_STATUS, CS_CLIENTMSG_TYPE, CS_UNUSED, &num);
   assert(ret == CS_SUCCEED);
   CS_CLIENTMSG cmsg;
   for (int i = 1; i <= num; ++i)
   {
      ret = ct_diag(m_connection, CS_GET, CS_CLIENTMSG_TYPE, i, &cmsg);
      assert(ret == CS_SUCCEED);
      int severity = CS_SEVERITY(cmsg.msgnumber);
      if (severity <= 10)
	 continue;

      if (count)
	 estr->concat(", ");
      estr->sprintf("client message %d: severity %d: %s", CS_NUMBER(cmsg.msgnumber), severity, cmsg.msgstring);
      estr->trim_trailing('.');
      if (cmsg.osstringlen > 0)
	 estr->sprintf(", OS error: %s", cmsg.osstring);
      count++;
   }
   ret = ct_diag(m_connection, CS_STATUS, CS_SERVERMSG_TYPE, CS_UNUSED, &num);
   assert(ret == CS_SUCCEED);
   CS_SERVERMSG smsg;
   for (int i = 1; i <= num; ++i)
   {
      ret = ct_diag(m_connection, CS_GET, CS_SERVERMSG_TYPE, i, &smsg);
      assert(ret == CS_SUCCEED);
      if (smsg.severity <= 10)
	 continue;
      
      if (count)
	 estr->concat(", ");
      estr->sprintf("server message %d, ", smsg.msgnumber);
      if (smsg.line)
	 estr->sprintf("line %d, ", smsg.line);
      estr->sprintf("severity %d: %s", smsg.severity, smsg.text);
      estr->trim_trailing("\n.");
      ++count;
   }
   ret = ct_diag(m_connection, CS_CLEAR, CS_ALLMSG_TYPE, CS_UNUSED, 0);
   assert(ret == CS_SUCCEED);   
   xsink->raiseException(err, estr);
   return -1;
}

/*
CS_RETCODE connection::clientmsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_CLIENTMSG* errmsg)
{
#ifdef DEBUG
  // This will print out description about an Sybase error. Most of the information
  // can be ignored but if the application crashes the last error may be very informative.
  // Comment it out if this output is not required.
  if ((CS_NUMBER(errmsg->msgnumber) == 211) || (CS_NUMBER(errmsg->msgnumber) == 212)) { // acc. to the docs
    return CS_SUCCEED;
  }
  fprintf(stderr, "-------------------------------------------------------------");
  fprintf(stderr, "\nOpen Client Message:\n");
  fprintf(stderr, "Message number: LAYER = (%d) ORIGIN = (%d) ",
    (int)CS_LAYER(errmsg->msgnumber), (int)CS_ORIGIN(errmsg->msgnumber));
  fprintf(stderr, "SEVERITY = (%d) NUMBER = (%d)\n",
    (int)CS_SEVERITY(errmsg->msgnumber), (int)CS_NUMBER(errmsg->msgnumber));
  fprintf(stderr, "Message String: %s\n", errmsg->msgstring);
  if (errmsg->osstringlen > 0) {
    fprintf(stderr, "Operating System Error: %s\n", errmsg->osstring);
  }
  fprintf(stderr, "--------------------------------------------------\n");
  fflush(stderr);  
#endif
  return CS_SUCCEED;
}

CS_RETCODE connection::servermsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_SERVERMSG* svrmsg)
{
#ifdef DEBUG
  // This will print out description about an Sybase error. Most of the information
  // can be ignored but if the application crashes the last error may be very informative.
  // Comment it out if this output is not required.
  fprintf(stderr, "-------------------------------------------------------------");
  fprintf(stderr, "\nOpen Server Message:\n");
  fprintf(stderr, "Message number = %d, severity = %d\n", (int)svrmsg->msgnumber, (int)svrmsg->severity);
  fprintf(stderr, "State = %d, line = %d\n", (int)svrmsg->state, (int)svrmsg->line);
  if (svrmsg->svrnlen) {
    fprintf(stderr, "Server: %s\n", svrmsg->svrname);
  }
  if (svrmsg->proclen) {
    fprintf(stderr, "Procedure: %s\n", svrmsg->proc);
  }
  fprintf(stderr, "Message string: %s\n", svrmsg->text);
  fprintf(stderr, "--------------------------------------------------\n");
  fflush(stderr);
#endif
  return CS_SUCCEED;
}
*/

// get client version
#define CLIENT_VER_LEN 240
QoreStringNode *connection::get_client_version(ExceptionSink *xsink)
{
   char *buf = (char *)malloc(sizeof(char) * CLIENT_VER_LEN);
   CS_INT olen;
   CS_RETCODE ret = ct_config(m_context, CS_GET, CS_VER_STRING, buf, CLIENT_VER_LEN, &olen);
   //printd(0, "olen=%d, ret=%d\n", olen, ret);
   if (ret != CS_SUCCEED) {
      do_exception(xsink, "DBI:SYBASE:GET-CLIENT-VERSION-ERROR", "ct_config(CS_VER_STRING) failed with error %d", (int)ret);
      return 0;
   }  
   //printd(5, "client version=%s\n", buf);
   return new QoreStringNode(buf);
}

AbstractQoreNode *connection::get_server_version(ExceptionSink *xsink)
{
   AbstractQoreNode *res = exec_intern(&ver_str, 0, true, xsink);
   if (!res)
      return 0;
   assert(res->getType() == NT_HASH);
   HashIterator hi(reinterpret_cast<QoreHashNode *>(res));
   hi.next();
   AbstractQoreNode *rv = hi.takeValueAndDelete();
   res->deref(xsink);

   QoreStringNode *str = dynamic_cast<QoreStringNode *>(rv);
   if (str)
      str->trim_trailing('\n');
   
   return rv;
}

#ifdef DEBUG
//#  include "tests/connection_tests.cc"
//#  include "tests/direct_execute_tests.cc"
//#  include "tests/executor_tests.cc" 
//#  include "tests/executor_rpc_tests.cc"
#endif

// EOF

