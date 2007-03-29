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

#include "sybase_connection.h"

//------------------------------------------------------------------------------
sybase_connection::sybase_connection()
: m_context(0), m_connection(0)
{
}

//------------------------------------------------------------------------------
sybase_connection::~sybase_connection()
{
  CS_RETCODE ret = CS_SUCCEED;
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
void sybase_connection::init(const char* username, const char* password, const char* dbname, ExceptionSink* xsink)
{
  assert(!m_connection);
  assert(!m_context);

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
/* not sure what this means, no docs for CS_MESSAGE_CB
  ret = cs_config(m_context, CS_SET, CS_MESSAGE_CB, (CS_VOID*)message_callback, CS_UNUSED, NULL);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-CALLBACK", "ct_config(CS_MESSAGE_CB) failed with error %d", ret);
    return;
  }
*/
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

  ret = ct_connect(m_connection, (CS_CHAR*)dbname,  strlen(dbname));
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-CONNECT", "ct_connect() failed with error %d", ret);
    return;
  }

  // transaction management is done by the driver (docs says it is by default)
  CS_BOOL chained_transactions = CS_FALSE;
  ret = ct_options(m_connection, CS_SET, CS_OPT_CHAINXACTS, &chained_transactions, CS_UNUSED, NULL);
  if (ret != CS_SUCCEED) {
    assert(false);
    xsink->raiseException("DBI:SYBASE:CT-LIB-SET-TRANSACTION-CHAINING", "ct_options(CS_OPT_CHAINXACTS) failed with error %d", ret);
    return;
  }
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::message_callback()
{
  return CS_SUCCEED;
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::clientmsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_CLIENTMSG* errmsg)
{
#ifdef DEBUG
  if ((CS_NUMBER(errmsg->msgnumber) == 211) || (CS_NUMBER(errmsg->msgnumber) == 212)) {
    return CS_SUCCEED;
  }
printf("#### ******************* client error output here: *****************************\n");
  fprintf(stderr, "\nOpen Client Message:\n");
  fprintf(stderr, "Message number: LAYER = (%d) ORIGIN = (%d) ",
    (int)CS_LAYER(errmsg->msgnumber), (int)CS_ORIGIN(errmsg->msgnumber));
  fprintf(stderr, "SEVERITY = (%d) NUMBER = (%d)\n",
    (int)CS_SEVERITY(errmsg->msgnumber), (int)CS_NUMBER(errmsg->msgnumber));
  fprintf(stderr, "Message String: %s\n", errmsg->msgstring);
  if (errmsg->osstringlen > 0) {
    fprintf(stderr, "Operating System Error: %s\n", errmsg->osstring);
  }
  fflush(stderr);  
#endif
  return CS_SUCCEED;
}

//------------------------------------------------------------------------------
CS_RETCODE sybase_connection::servermsg_callback(CS_CONTEXT* ctx, CS_CONNECTION* conn, CS_SERVERMSG* svrmsg)
{
#ifdef DEBUG
printf("#### ******************* server error output here: *****************************\n");
#endif
  return CS_SUCCEED;
}

#ifdef DEBUG
#  include "tests/sybase_connection_tests.cc"
#endif

// EOF

