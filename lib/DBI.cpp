/*
  DBI.cpp

  Database Independent SQL Layer

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#include <qore/Qore.h>

#include "qore/intern/qore_dbi_private.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

typedef safe_dslist<DBIDriver* > dbi_list_t;

// global qore library class for DBI driver management
DBIDriverList DBI;

struct dbi_cap_hash {
   int cap;
   const char* desc;
};

struct dbi_cap_hash dbi_cap_list[] =
{ { DBI_CAP_TIME_ZONE_SUPPORT,      "TimeZoneSupport" },
  { DBI_CAP_CHARSET_SUPPORT,        "CharsetSupport" },
  { DBI_CAP_TRANSACTION_MANAGEMENT, "TransactionManagement" },
  { DBI_CAP_STORED_PROCEDURES,      "StoredProcedures" },
  { DBI_CAP_LOB_SUPPORT,            "LargeObjectSupport" },
  { DBI_CAP_BIND_BY_VALUE,          "BindByValue" },
  { DBI_CAP_BIND_BY_PLACEHOLDER,    "BindByPlaceholder" },
  { DBI_CAP_HAS_EXECRAW,            "HasExecRaw" },
  { DBI_CAP_HAS_STATEMENT,          "HasStatementApi" },
  { DBI_CAP_HAS_SELECT_ROW,         "HasSelectRow" },
  { DBI_CAP_HAS_NUMBER_SUPPORT,     "HasNumberSupport" },
  { DBI_CAP_HAS_OPTION_SUPPORT,     "HasOptionSupport" },
  { DBI_CAP_SERVER_TIME_ZONE,       "ServerTimeZone" },
  { DBI_CAP_AUTORECONNECT,          "AutoReconnect" },
  { DBI_CAP_EVENTS,                 "Events" },
  { DBI_CAP_HAS_DESCRIBE,           "HasDescribe" },
  { DBI_CAP_HAS_ARRAY_BIND,         "HasArrayBind" },
};

#define NUM_DBI_CAPS (sizeof(dbi_cap_list) / sizeof(dbi_cap_hash))

struct qore_dbi_mlist_private {
   dbi_method_list_t l;
   dbi_opt_map_t omap;

   DLLLOCAL void registerOption(const char* name, const char* desc, const QoreTypeInfo* type = 0) {
      assert(omap.find(name) == omap.end());
      omap[name] = DbiOptInfo(desc, type);
   }

   DLLLOCAL static qore_dbi_mlist_private* get(const qore_dbi_method_list& ml) {
      return ml.priv;
   }
};

qore_dbi_method_list::qore_dbi_method_list() : priv(new qore_dbi_mlist_private) {
}

qore_dbi_method_list::~qore_dbi_method_list() {
   delete priv;
}

// covers open, commit, rollback, and begin transaction
void qore_dbi_method_list::add(int code, q_dbi_open_t method) {
   assert(code == QDBI_METHOD_OPEN
	  || code == QDBI_METHOD_COMMIT
	  || code == QDBI_METHOD_ROLLBACK
	  || code == QDBI_METHOD_BEGIN_TRANSACTION
	  || code == QDBI_METHOD_ABORT_TRANSACTION_START
      );
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// for close
void qore_dbi_method_list::add(int code, q_dbi_close_t method) {
   assert(code == QDBI_METHOD_CLOSE);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers select, select_rows, and exec
void qore_dbi_method_list::add(int code, q_dbi_select_t method) {
   assert(code == QDBI_METHOD_SELECT || code == QDBI_METHOD_SELECT_ROWS || code == QDBI_METHOD_EXEC || code == QDBI_METHOD_DESCRIBE);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers select_row
void qore_dbi_method_list::add(int code, q_dbi_select_row_t method) {
   assert(code == QDBI_METHOD_SELECT_ROW);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers execRaw
void qore_dbi_method_list::add(int code, q_dbi_execraw_t method) {
   assert(code == QDBI_METHOD_EXECRAW);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers get_server_version
void qore_dbi_method_list::add(int code, q_dbi_get_server_version_t method) {
   assert(code == QDBI_METHOD_GET_SERVER_VERSION);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers get_client_version
void qore_dbi_method_list::add(int code, q_dbi_get_client_version_t method) {
   assert(code == QDBI_METHOD_GET_CLIENT_VERSION);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt prepare
void qore_dbi_method_list::add(int code, q_dbi_stmt_prepare_t method) {
   assert(code == QDBI_METHOD_STMT_PREPARE);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt prepare_raw
void qore_dbi_method_list::add(int code, q_dbi_stmt_prepare_raw_t method) {
   assert(code == QDBI_METHOD_STMT_PREPARE_RAW);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt bind
void qore_dbi_method_list::add(int code, q_dbi_stmt_bind_t method) {
   assert(code == QDBI_METHOD_STMT_BIND || code == QDBI_METHOD_STMT_BIND_PLACEHOLDERS || code == QDBI_METHOD_STMT_BIND_VALUES);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt exec, close, define, and affectedRows
void qore_dbi_method_list::add(int code, q_dbi_stmt_exec_t method) {
   assert(code == QDBI_METHOD_STMT_EXEC || code == QDBI_METHOD_STMT_CLOSE || code == QDBI_METHOD_STMT_DEFINE || code == QDBI_METHOD_STMT_AFFECTED_ROWS || code == QDBI_METHOD_STMT_FREE);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt fetch_row, get_output and get_output_rows
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_row_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_ROW || code == QDBI_METHOD_STMT_GET_OUTPUT_ROWS || code == QDBI_METHOD_STMT_GET_OUTPUT || code == QDBI_METHOD_STMT_DESCRIBE);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt fetch_rows
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_rows_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_ROWS);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt fetch_columns
void qore_dbi_method_list::add(int code, q_dbi_stmt_fetch_columns_t method) {
   assert(code == QDBI_METHOD_STMT_FETCH_COLUMNS);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

// covers stmt next
void qore_dbi_method_list::add(int code, q_dbi_stmt_next_t method) {
   assert(code == QDBI_METHOD_STMT_NEXT);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

void qore_dbi_method_list::add(int code, q_dbi_option_set_t method) {
   assert(code == QDBI_METHOD_OPT_SET);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

void qore_dbi_method_list::add(int code, q_dbi_option_get_t method) {
   assert(code == QDBI_METHOD_OPT_GET);
   assert(priv->l.find(code) == priv->l.end());
   priv->l[code] = (void*)method;
}

void qore_dbi_method_list::registerOption(const char* name, const char* desc, const QoreTypeInfo* type) {
   priv->registerOption(name, desc, type);
}

DbiArgHelper::DbiArgHelper(const QoreListNode* ol, bool numeric, ExceptionSink* xs) : orig(ol), nl(0), xsink(xs) {
   if (numeric || !orig)
      return;

   // scan input list
   ConstListIterator li(orig);
   while (li.next()) {
      if (get_node_type(li.getValue()) == NT_NUMBER) {
         if (!nl) {
            nl = new QoreListNode;
            for (unsigned i = 0; i < li.index(); ++i)
               nl->push(orig->get_referenced_entry(i));
         }
         nl->push(new QoreFloatNode(li.getValue()->getAsFloat()));
         continue;
      }
      if (nl)
         nl->push(li.getReferencedValue());
   }
}

OptInputHelper::OptInputHelper(ExceptionSink* xs, const qore_dbi_private& driver, const char* opt, bool set, const AbstractQoreNode* v) : xsink(xs), val(const_cast<AbstractQoreNode*>(v)), tmp(false) {
   dbi_opt_map_t::const_iterator i = driver.omap.find(opt);
   if (i == driver.omap.end()) {
      xsink->raiseException("DBI-OPTION-ERROR", "driver '%s' does not support option '%s'", driver.name, opt);
      return;
   }
   if (!set)
      return;

   const QoreTypeInfo* ti = i->second.typeInfo;

   if (!QoreTypeInfo::mayRequireFilter(ti, v))
      return;

   tmp = true;
   val->ref();
   val = QoreTypeInfo::acceptInputParam(ti, -1, "<dbi driver option>", val, xsink);
}

qore_dbi_private::qore_dbi_private(const char* nme, const qore_dbi_mlist_private& methods, int cps) {
   // add methods to internal data structure
   for (dbi_method_list_t::const_iterator i = methods.l.begin(), e = methods.l.end(); i != e; ++i) {
      assert((*i).first > 0 && (*i).first <= QDBI_VALID_CODES);
      switch ((*i).first) {
         case QDBI_METHOD_OPEN:
            assert(!f.open);
            f.open = (q_dbi_open_t)(*i).second;
            break;
         case QDBI_METHOD_CLOSE:
            assert(!f.close);
            f.close = (q_dbi_close_t)(*i).second;
            break;
         case QDBI_METHOD_SELECT:
            assert(!f.select);
            f.select = (q_dbi_select_t)(*i).second;
            break;
         case QDBI_METHOD_SELECT_ROWS:
            assert(!f.selectRows);
            f.selectRows = (q_dbi_select_rows_t)(*i).second;
            break;
         case QDBI_METHOD_SELECT_ROW:
            assert(!f.selectRow);
            f.selectRow = (q_dbi_select_row_t)(*i).second;
            cps |= DBI_CAP_HAS_SELECT_ROW;
            break;
         case QDBI_METHOD_EXEC:
            assert(!f.execSQL);
            f.execSQL = (q_dbi_exec_t)(*i).second;
            break;
         case QDBI_METHOD_EXECRAW:
            assert(!f.execRawSQL);
            f.execRawSQL = (q_dbi_execraw_t)(*i).second;
            cps |= DBI_CAP_HAS_EXECRAW;
            break;
	 case QDBI_METHOD_DESCRIBE:
            assert(!f.describe);
            f.describe = (q_dbi_describe_t)(*i).second;
            cps |= DBI_CAP_HAS_DESCRIBE;
            break;
         case QDBI_METHOD_COMMIT:
            assert(!f.commit);
            f.commit = (q_dbi_commit_t)(*i).second;
            break;
         case QDBI_METHOD_ROLLBACK:
            assert(!f.rollback);
            f.rollback = (q_dbi_rollback_t)(*i).second;
            break;
         case QDBI_METHOD_BEGIN_TRANSACTION:
            assert(!f.begin_transaction);
            f.begin_transaction = (q_dbi_begin_transaction_t)(*i).second;
            break;
         case QDBI_METHOD_GET_SERVER_VERSION:
            assert(!f.get_server_version);
            f.get_server_version = (q_dbi_get_server_version_t)(*i).second;
            break;
         case QDBI_METHOD_GET_CLIENT_VERSION:
            assert(!f.get_client_version);
            f.get_client_version = (q_dbi_get_client_version_t)(*i).second;
            break;

         case QDBI_METHOD_STMT_PREPARE:
            assert(!f.stmt.prepare);
            f.stmt.prepare = (q_dbi_stmt_prepare_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_PREPARE_RAW:
            assert(!f.stmt.prepare_raw);
            f.stmt.prepare_raw = (q_dbi_stmt_prepare_raw_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_BIND:
            assert(!f.stmt.bind);
            f.stmt.bind = (q_dbi_stmt_bind_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_BIND_PLACEHOLDERS:
            assert(!f.stmt.bind_placeholders);
            f.stmt.bind_placeholders = (q_dbi_stmt_bind_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_BIND_VALUES:
            assert(!f.stmt.bind_values);
            f.stmt.bind_values = (q_dbi_stmt_bind_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_EXEC:
            assert(!f.stmt.exec);
            f.stmt.exec = (q_dbi_stmt_exec_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_FETCH_ROW:
            assert(!f.stmt.fetch_row);
            f.stmt.fetch_row = (q_dbi_stmt_fetch_row_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_FETCH_ROWS:
            assert(!f.stmt.fetch_rows);
            f.stmt.fetch_rows = (q_dbi_stmt_fetch_rows_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_FETCH_COLUMNS:
            assert(!f.stmt.fetch_columns);
            f.stmt.fetch_columns = (q_dbi_stmt_fetch_columns_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_DESCRIBE:
            assert(!f.stmt.describe);
            f.stmt.describe = (q_dbi_stmt_fetch_row_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_NEXT:
            assert(!f.stmt.next);
            f.stmt.next = (q_dbi_stmt_next_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_DEFINE:
            assert(!f.stmt.define);
            f.stmt.define = (q_dbi_stmt_define_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_CLOSE:
            assert(!f.stmt.close);
            f.stmt.close = (q_dbi_stmt_close_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_FREE:
            assert(!f.stmt.free);
            f.stmt.free = (q_dbi_stmt_close_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_AFFECTED_ROWS:
            assert(!f.stmt.affected_rows);
            f.stmt.affected_rows = (q_dbi_stmt_affected_rows_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_GET_OUTPUT:
            assert(!f.stmt.get_output);
            f.stmt.get_output = (q_dbi_stmt_get_output_t)(*i).second;
            break;
         case QDBI_METHOD_STMT_GET_OUTPUT_ROWS:
            assert(!f.stmt.get_output_rows);
            f.stmt.get_output_rows = (q_dbi_stmt_get_output_rows_t)(*i).second;
            break;

         case QDBI_METHOD_OPT_SET:
            assert(!f.opt.set);
            f.opt.set = (q_dbi_option_set_t)(*i).second;
            break;
         case QDBI_METHOD_OPT_GET:
            assert(!f.opt.get);
            f.opt.get = (q_dbi_option_get_t)(*i).second;
            break;

#ifdef DEBUG
	 // ignore unsupported method
	 case QDBI_METHOD_ABORT_TRANSACTION_START:
	    break;
         default:
            assert(false);
#endif
      }
   }
   // ensure minimum methods are defined
   assert(f.open);
   assert(f.close);
   assert(f.select);
   assert(f.selectRows);
   assert(f.execSQL);
   assert(f.commit);
   assert(f.rollback);

   // ensure either no or minimum stmt methods are defined
   assert(!f.stmt.prepare || (f.stmt.prepare_raw && f.stmt.bind && f.stmt.bind_values
         && f.stmt.exec && f.stmt.fetch_row && f.stmt.fetch_rows
         && f.stmt.fetch_columns && f.stmt.next && f.stmt.close
         && f.stmt.affected_rows && f.stmt.get_output
         && f.stmt.get_output_rows && f.stmt.define));

   // ensure either no or minimum opt methods are defined
   assert(!f.opt.set || (f.opt.set && f.opt.get));

   name = nme;
   caps = cps;
   if (f.stmt.prepare)
      caps |= DBI_CAP_HAS_STATEMENT;
#ifdef DEBUG
   else
      assert(!(caps & DBI_CAP_HAS_STATEMENT));
#endif

   if (f.opt.set)
      caps |= DBI_CAP_HAS_OPTION_SUPPORT;

   // copy options
   omap = methods.omap;
}

QoreListNode* qore_dbi_private::getCapList() const {
   QoreListNode* l = new QoreListNode;
   for (unsigned i = 0; i < NUM_DBI_CAPS; ++i)
      if (caps & dbi_cap_list[i].cap)
         l->push(new QoreStringNode(dbi_cap_list[i].desc));
   return l;
}

DBIDriver::DBIDriver(qore_dbi_private* p) : priv(p) {
}

DBIDriver::~DBIDriver() {
   delete priv;
}

const char* DBIDriver::getName() const {
   return priv->name;
}

bool DBIDriver::hasStatementAPI() const {
   return priv->hasStatementAPI();
}

QoreHashNode* DBIDriver::getOptionHash() const {
   return priv->getOptionHash();
}

/* it's not necessary to lock this object because it will only be written to in one thread at a time
   (within the module lock)
   note that a safe_dslist is used because it can be safely read in multiple threads while
   being written to (in the lock).  The list should never be that long so the penalty for searching
   a linked list with strcmp() against using a hash with explicit locking around all searches
   should be acceptable...
*/
struct qore_dbi_dlist_private {
   dbi_list_t l;

   DLLLOCAL ~qore_dbi_dlist_private() {
      dbi_list_t::iterator i;
      while ((i = l.begin()) != l.end()) {
	 DBIDriver* driv = *i;
	 l.erase(i);
	 delete driv;
      }
   }

   DLLLOCAL DBIDriver* find_intern(const char* name) const {
      for (dbi_list_t::const_iterator i = l.begin(); i != l.end(); i++)
	 if (!strcmp(name, (*i)->getName()))
	    return *i;

      return 0;
   }

   DLLLOCAL QoreListNode* getDriverList() const {
      if (l.empty())
	 return 0;

      QoreListNode* lst = new QoreListNode;

      for (dbi_list_t::const_iterator i = l.begin(); i != l.end(); i++)
	 lst->push(new QoreStringNode((*i)->getName()));

      return lst;
   }
};

DBIDriverList::DBIDriverList() : priv(new qore_dbi_dlist_private) {
}

DBIDriverList::~DBIDriverList() {
   delete priv;
}

DBIDriver* DBIDriverList::find_intern(const char* name) const {
   return priv->find_intern(name);
}

DBIDriver* DBIDriverList::find(const char* name) const {
   DBIDriver* d = priv->find_intern(name);
   if (d)
      return d;

   // to to load the driver if it doesn't exist
   // ignore any exceptions
   ExceptionSink xs;
   MM.runTimeLoadModule(name, &xs);
   xs.clear();

   return priv->find_intern(name);
}

DBIDriver* DBIDriverList::find(const char* name, ExceptionSink* xsink) const {
   DBIDriver* d = priv->find_intern(name);
   if (d)
      return d;

   // to to load the driver if it doesn't exist
   if (MM.runTimeLoadModule(name, xsink))
      return 0;

   return priv->find_intern(name);
}

DBIDriver* DBIDriverList::registerDriver(const char* name, const qore_dbi_method_list& methods, int caps) {
   assert(!priv->find_intern(name));

   DBIDriver* dd = new DBIDriver(new qore_dbi_private(name, *qore_dbi_mlist_private::get(methods), caps));
   priv->l.push_back(dd);
   return dd;
}

QoreListNode* DBIDriverList::getDriverList() const {
   return priv->getDriverList();
}

void DBI_concat_numeric(QoreString* str, const AbstractQoreNode* v) {
   if (is_nothing(v) || is_null(v)) {
      str->concat("null");
      return;
   }

   qore_type_t t = v->getType();
   if (t == NT_FLOAT || (t == NT_STRING && strchr((reinterpret_cast<const QoreStringNode*>(v))->c_str(), '.'))) {
      size_t offset = str->size();
      str->sprintf("%g", v->getAsFloat());
      // issue 1556: external modules that call setlocale() can change
      // the decimal point character used here from '.' to ','
      // only search the double added, QoreString::sprintf() concatenates
      q_fix_decimal(str, offset);
      return;
   }
   else if (t == NT_NUMBER) {
      reinterpret_cast<const QoreNumberNode*>(v)->getStringRepresentation(*str);
      return;
   }
   str->sprintf("%lld", v->getAsBigInt());
}

int DBI_concat_string(QoreString* str, const AbstractQoreNode* v, ExceptionSink* xsink) {
   assert(xsink);
   if (is_nothing(v) || is_null(v))
      return 0;

   QoreStringValueHelper tstr(v, str->getEncoding(), xsink);
   if (*xsink)
      return -1;

   str->concat(*tstr, xsink);
   return *xsink;
}

/*
  parseDatasource()
  parses strings of the form: driver:user/pass@db(encoding)%host:post{option1,option2=value}
  everything except "@db" is optional
*/
QoreHashNode* parseDatasource(const char* ds, ExceptionSink* xsink) {
   static const char* DATASOURCE_PARSE_ERROR = "DATASOURCE-PARSE-ERROR";

   if (!ds || !ds[0]) {
      xsink->raiseException(DATASOURCE_PARSE_ERROR, "empty text passed to parseDatasource()");
      return 0;
   }

   // use a QoreString to create a temporary buffer
   QoreString tmp(ds);
   tmp.trim();
   char* str = const_cast<char*>(tmp.getBuffer());

   ReferenceHolder<QoreHashNode> h(new QoreHashNode, xsink);
   char* p = strchr(str, ':');
   // make sure this is the driver name and not the port at the end
   if (p) {
      *p = '\0';
      if (strchrs(str, "@/%"))
         // this is the port at the end, so skip it
         *p = ':';
      else {
         h->setKeyValue("type", new QoreStringNode(str), 0);
         str = p + 1;
      }
   }

   bool has_pass = false;
   p = strchr(str, '/');
   if (p) {
      *p = '\0';
      if (*str)
	 h->setKeyValue("user", new QoreStringNode(str), 0);
      str = p + 1;
      has_pass = true;
   }

   // take last '@' sign in string in case there's one in the password
   p = strrchr(str, '@');
   if (!p) {
      xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing database name delimited by '@' in '%s'", ds);
      return 0;
   }

   *p = '\0';
   if (p != str) {
      if (has_pass)
	 h->setKeyValue("pass", new QoreStringNode(str), 0);
      else
	 h->setKeyValue("user", new QoreStringNode(str), 0);
   }
   str = p + 1;

   char* db = str;
   p = strchr(str, '(');
   if (p) {
      char* end = strchr(p, ')');
      if (!end) {
	 xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing closing parenthesis in charset specification in '%s'", ds);
	 return 0;
      }
      *p = '\0';  // terminate for db
      *end = '\0';  // terminate charset
      p++;
      h->setKeyValue("charset", new QoreStringNode(p), 0);
      str = end + 1;
   }

   // get dbname
   p = strchrs(str, "%:{");
   if (!p)
      h->setKeyValue("db", new QoreStringNode(db), 0);
   else {
      char tok = *p;
      *p = '\0';

      h->setKeyValue("db", new QoreStringNode(db), 0);
      str = p + 1;

      if (tok == '%') {
	 p = strchrs(str, ":{");

	 if ((p == str) || !*str) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing hostname string after '%%' delimeter in '%s'", ds);
	    return 0;
	 }

	 const char* hstr = str;

	 if (p) {
	    tok = *p;
	    *p = '\0';
	    str = p + 1;
	 }
	 else
	    str += strlen(str);

	 h->setKeyValue("host", new QoreStringNode(hstr), 0);
      }

      if (tok == ':') {
	 int port = atoi(str);
	 if (!port) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "invalid port number in datasource string");
	    return 0;
	 }
	 h->setKeyValue("port", new QoreBigIntNode(port), 0);

	 const char* pstr = str;
	 p = strchr(str, '{');
	 if (p) {
	    tok = *p;
	    *p = '\0';
	    str = p + 1;
	 }
	 else
	    str += strlen(str);

	 while (isdigit(*pstr))
	    ++pstr;

	 if (*pstr) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "invalid characters present in port number in '%s'", ds);
	    return 0;
	 }
      }

      if (tok == '{') {
	 char* end = strchr(str, '}');
	 if (!end) {
	    xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing closing curly bracket '}' in option specification in '%s'", ds);
	    return 0;
	 }
	 *end = '\0';
	 p = str;
	 str = end + 1;

	 // parse option hash
	 ReferenceHolder<QoreHashNode> opt(new QoreHashNode, xsink);

	 while (true) {
	    if (!*p)
	       break;
	    char* eq = strchr(p, '=');
	    char* oend = strchr(p, ',');
	    qore_size_t len = 0;
	    // if there is only an option left with no more options and no value
	    if (!eq && !oend) {
	       opt->setKeyValue(p, &True, 0);
               p += strlen(p);
	    }
	    else {
	       // if there is more than one option and the next option to be parsed has no value
	       if (oend && (!eq || oend < eq)) {
                  len = oend - p;
                  QoreString tmp(p, len);
	          opt->setKeyValue(tmp.getBuffer(), &True, 0);
	          p += len;
	       }
	       else {
	          // here we must have an equals sign
	          assert(eq);
	          if (eq == p) {
	             xsink->raiseException(DATASOURCE_PARSE_ERROR, "missing value after '=' in option specification in '%s'", ds);
	             return 0;
	          }
	          *eq = '\0';
	          ++eq;
	          len = oend ? oend - eq : strlen(eq);
	          if (opt->existsKey(p)) {
	             xsink->raiseException(DATASOURCE_PARSE_ERROR, "option '%s' repeated in '%s'", p, ds);
	             return 0;
	          }

	          QoreString key(p);
	          key.trim();

	          QoreString value(eq, len);
	          value.trim();

	          opt->setKeyValue(key.getBuffer(), new QoreStringNode(value), 0);

	          p = eq + len;
	       }
	    }
            if (oend)
               ++p;
	 }

	 h->setKeyValue("options", opt.release(), 0);
      }

      if (*str) {
	 xsink->raiseException(DATASOURCE_PARSE_ERROR, "unrecognized characters at end of datasource definition in '%s'", ds);
	 return 0;
      }
   }

   return h.release();
}
