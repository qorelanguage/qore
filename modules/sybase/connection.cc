/*
  sybase_connection.cc

  Sybase DB layer for QORE
  uses Sybase OpenClient C library

  Qore Programming language

  Copyright (C) 2007

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

#include <qore/config.h>
#include <qore/support.h>

#include <ctpublic.h>
#include <assert.h>
#include <qore/minitest.hpp>

#include "connection.h"

//------------------------------------------------------------------------------
connection::connection()
: m_context(0), m_connection(0), m_charset_locale(0)
{
}

//------------------------------------------------------------------------------
connection::~connection()
{
  CS_RETCODE ret = CS_SUCCEED;

  if (m_charset_locale) {
    ret = cs_loc_drop(getContext(), m_charset_locale);
    assert(ret == CS_SUCCEED);
  }

  if (m_connection) {
    ret = ct_close(m_connection, CS_UNUSED);
    if (ret != CS_SUCCEED) {
// commented out since it returns CS_BUSY. No idea at the time of writing what to do with it.
//      assert(false); // not much can be done here
    }
  }
  if (m_context) {
   CS_INT exit_type = ret == CS_SUCCEED ? CS_UNUSED : CS_FORCE_EXIT;
    ret = ct_exit(m_context, exit_type);
    if (ret != CS_SUCCEED) {
      assert(false); // not much can be done here
    }
    ret = cs_ctx_drop(m_context);
    if (ret != CS_SUCCEED) {
      assert(false); // not much can be done here
    }
  }
}

//------------------------------------------------------------------------------
// Post-constructor initialization 
void connection::init(const char* username, const char* password, const char* dbname, const char *db_encoding, ExceptionSink* xsink)
{
  assert(!m_connection);
  assert(!m_context);

  //printd(0, "connection::init() user=%s pass=%s dbname=%s, db_enc=%s\n", username, password, dbname, db_encoding);

  CS_RETCODE ret = cs_ctx_alloc(CS_VERSION_100, &m_context);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-CANNOT-ALLOCATE-ERROR", "cs_ctx_alloc() failed with error %d", ret);
    return;
  }

  ret = ct_init(m_context, CS_VERSION_100);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-INIT-FAILED", "ct_init() failed with error %d", ret);
    return;
  }

  // add callbacks
  ret = ct_callback(m_context, 0, CS_SET, CS_CLIENTMSG_CB, (CS_VOID*)clientmsg_callback);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-CALLBACK", "ct_callback(CS_CLIENTMSG_CB) failed with error %d", ret);
    return;
  }
  ret = ct_callback(m_context, 0, CS_SET, CS_SERVERMSG_CB, (CS_VOID*)servermsg_callback);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-CALLBACK", "ct_callback(CS_SERVERMSG_CB) failed with error %d", ret);
    return;
  }
  ret = ct_con_alloc(m_context, &m_connection);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-CREATE-CONNECTION", "ct_con_alloc() failed with error %d", ret);
    return;
  }

  ret = ct_con_props(m_connection, CS_SET, CS_USERNAME, (CS_VOID*)username, CS_NULLTERM, 0);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-USERNAME", "ct_con_props(CS_USERNAME) failed with error %d", ret);
    return;
  }
  if (password && password[0]) {
    ret = ct_con_props(m_connection, CS_SET, CS_PASSWORD, (CS_VOID*)password, CS_NULLTERM, 0);
    if (ret != CS_SUCCEED) {
      assert(false);
      xsink->raiseException("DBI:SYBASE:CT-LIB-SET-PASSWORD", "ct_con_props(CS_PASSWORD) failed with error %d", ret);
      return;
    }
  }

  ret = cs_loc_alloc(getContext(), &m_charset_locale);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_loc_alloc() returned error %d", (int)ret);
    return;
  }
  ret = cs_locale(getContext(), CS_SET, m_charset_locale, CS_SYB_CHARSET, (CS_CHAR*)db_encoding, CS_NULLTERM, 0);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_locale(CS_SYB_CHARSET, \"%s\") failed with error %d", (int)ret);
    return;
  }

  //printd(0, "about to call ct_connect()\n");
  ret = ct_connect(m_connection, (CS_CHAR*)dbname,  strlen(dbname));
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-CONNECT", "ct_connect() failed with error %d", ret);
    return;
  }
  //printd(0, "returned from ct_connect()\n");

  // Transaction management is done by the driver (docs says it is by default)
  CS_BOOL chained_transactions = CS_FALSE;
  ret = ct_options(m_connection, CS_SET, CS_OPT_CHAINXACTS, &chained_transactions, CS_UNUSED, NULL);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-TRANSACTION-CHAINING", "ct_options(CS_OPT_CHAINXACTS) failed with error %d", ret);
    return;
  }

  // Set default type of string representation of DATETIME to long (like Jan 1 1990 12:32:55:0000 PM)
  // Without this some routines in conversions.cc would fail.
  CS_INT aux = CS_DATES_LONG;
  ret = cs_dt_info(m_context, CS_SET, NULL, CS_DT_CONVFMT, CS_UNUSED, (CS_VOID*)&aux, sizeof(aux), 0);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "Sybase call cs_dt_info(CS_DT_CONVFMT) failed with error %d", (int)ret);
    return;
  }
}

//------------------------------------------------------------------------------
void connection::set_charset(const char* charset_name, ExceptionSink* xsink)
{
  if (m_charset_locale) {
    assert(false); // called twice?
    return;
  }
  CS_RETCODE err = cs_loc_alloc(getContext(), &m_charset_locale);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_loc_alloc() returned error %d", (int)err);
    return;
  }
  err = cs_locale(getContext(), CS_SET, m_charset_locale, CS_SYB_CHARSET, (CS_CHAR*)charset_name, CS_NULLTERM, 0);
  if (err != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI-EXEC-EXCEPTION", "cs_locale(CS_SYB_CHARSET, \"%s\") failed with error %d", (int)err);
    return;
  }
}

//------------------------------------------------------------------------------
CS_RETCODE connection::clientmsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_CLIENTMSG* errmsg)
{
#ifdef DEBUG
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

//------------------------------------------------------------------------------
CS_RETCODE connection::servermsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_SERVERMSG* svrmsg)
{
#ifdef DEBUG
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

#ifdef DEBUG
#  include "tests/connection_tests.cc"
#endif

// EOF

