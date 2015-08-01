/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  qore_ds_private.h

  Qore Programming Language
 
  Copyright (C) 2003 - 2014 David Nichols
 
  The Datasource class provides the low-level interface to Qore DBI drivers.
  
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

#ifndef _QORE_DS_PRIVATE_H

#define _QORE_DS_PRIVATE_H

struct qore_ds_private {
   Datasource* ds;
   bool in_transaction;
   bool active_transaction;
   bool isopen;
   bool autocommit;
   bool connection_aborted;
   mutable DBIDriver* dsl;
   const QoreEncoding* qorecharset;
   void* private_data;               // driver private data per connection
      
   // for pending connection values
   std::string p_username,
      p_password,
      p_dbname,
      p_db_encoding, // database-specific name for the encoding for the connection
      p_hostname;
   int p_port;       // pending port number (0 = default port)

   // actual connection values set by init() before the datasource is opened
   std::string username,
      password,
      db_encoding,   // database-specific name for the encoding for the connection
      dbname,
      hostname;
   int port; // port number (0 = default port)

   // options per connection
   QoreHashNode* opt;
   // DBI event queue
   Queue* event_queue;
   // DBI Event queue argument
   AbstractQoreNode* event_arg;

   DLLLOCAL qore_ds_private(Datasource* n_ds, DBIDriver* ndsl) : ds(n_ds), in_transaction(false), active_transaction(false), isopen(false), autocommit(false), connection_aborted(false), dsl(ndsl), qorecharset(QCS_DEFAULT), private_data(0), p_port(0), port(0), opt(new QoreHashNode), event_queue(0), event_arg(0) {
   }

   DLLLOCAL qore_ds_private(const qore_ds_private& old, Datasource* n_ds) :
      ds(n_ds), in_transaction(false), active_transaction(false), isopen(false),
      autocommit(old.autocommit), connection_aborted(false), dsl(old.dsl),
      qorecharset(QCS_DEFAULT), private_data(0),
      p_username(old.p_username), p_password(old.p_password),
      p_dbname(old.p_dbname), p_db_encoding(old.p_db_encoding),
      p_hostname(old.p_hostname), p_port(old.p_port),
      port(0), 
      //opt(old.opt->copy()) {
      opt(old.getCurrentOptionHash(true)), 
      event_queue(old.event_queue ? old.event_queue->queueRefSelf() : 0),
      event_arg(old.event_arg ? old.event_arg->refSelf() : 0) {
   }

   DLLLOCAL ~qore_ds_private() {
      assert(!private_data);
      ExceptionSink xsink;
      if (opt)
         opt->deref(&xsink);
      if (event_arg)
         event_arg->deref(&xsink);
      if (event_queue)
         event_queue->deref(&xsink);
   }

   DLLLOCAL void setPendingConnectionValues(const qore_ds_private *other) {
      p_username    = other->p_username;
      p_password    = other->p_password;
      p_dbname      = other->p_dbname;
      p_hostname    = other->p_hostname;
      p_db_encoding = other->p_db_encoding;
      autocommit    = other->autocommit;
      p_port        = other->p_port;
   }

   DLLLOCAL void setConnectionValues() {
      dbname      = p_dbname;
      username    = p_username;
      password    = p_password;
      hostname    = p_hostname;
      db_encoding = p_db_encoding;
      port        = p_port;
   }

   // returns true if a new transaction was started
   DLLLOCAL bool statementExecuted(int rc, ExceptionSink *xsink);

   DLLLOCAL void copyOptions(const Datasource* ods);

   DLLLOCAL void setOption(const char* name, const AbstractQoreNode* v, ExceptionSink* xsink) {
      opt->setKeyValue(name, v ? v->refSelf() : 0, xsink);
   }

   DLLLOCAL QoreHashNode* getOptionHash() const {
      return private_data ? qore_dbi_private::get(*dsl)->getOptionHash(ds) : opt->hashRefSelf();
   }

   DLLLOCAL QoreHashNode* getCurrentOptionHash(bool ensure_hash = false) const {
      QoreHashNode* options = 0;
      
      ReferenceHolder<QoreHashNode> opts(getOptionHash(), 0);
      ConstHashIterator hi(*opts);
      while (hi.next()) {
         const QoreHashNode* ov = reinterpret_cast<const QoreHashNode*>(hi.getValue());
         const AbstractQoreNode* v = ov->getKeyValue("value");
         if (!v || v == &False)
            continue;

         if (!options)
            options = new QoreHashNode;

         options->setKeyValue(hi.getKey(), v->refSelf(), 0);
      }

      if (ensure_hash && !options)
         options = new QoreHashNode;

      return options;
   }

   DLLLOCAL void setEventQueue(Queue* q, AbstractQoreNode* arg, ExceptionSink* xsink) {
      if (event_queue)
         event_queue->deref(xsink);
      if (event_arg)
         event_arg->deref(xsink);
      event_queue = q;
      event_arg = arg;
   }

   DLLLOCAL QoreHashNode* getEventQueueHash(Queue*& q, int event_code) const {
      q = event_queue;
      if (!q)
         return 0;
      QoreHashNode* h = new QoreHashNode;
      if (!username.empty())
         h->setKeyValue("user", new QoreStringNode(username), 0);
      if (!dbname.empty())
         h->setKeyValue("db", new QoreStringNode(dbname), 0);
      h->setKeyValue("eventtype", new QoreBigIntNode(event_code), 0);
      if (event_arg)
         h->setKeyValue("arg", event_arg->refSelf(), 0);
      return h;
   }
};

#endif
