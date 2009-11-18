/*
 Datasource.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2009 David Nichols
 
 NOTE that 2 copies of connection values are kept in case
 the values are changed while a connection is in use
 
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

#include <stdlib.h>
#include <string.h>

struct qore_ds_private {
   bool in_transaction;
   bool isopen;
   bool autocommit;
   bool connection_aborted;
   mutable class DBIDriver *dsl;
   const QoreEncoding *qorecharset;
   void *private_data;               // driver private data per connection
      
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

   DLLLOCAL qore_ds_private(DBIDriver *ndsl) : in_transaction(false), isopen(false), autocommit(false), connection_aborted(false), dsl(ndsl), qorecharset(QCS_DEFAULT), private_data(0), p_port(0), port(0) { }
      
   DLLLOCAL ~qore_ds_private() {
      assert(!private_data);
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
};

Datasource::Datasource(DBIDriver *ndsl) : priv(new qore_ds_private(ndsl)) {
}

Datasource::~Datasource() {
   if (priv->isopen)
      close();

   delete priv;
}

void Datasource::setPendingConnectionValues(const Datasource *other) {
   priv->setPendingConnectionValues(other->priv);
}

void Datasource::setTransactionStatus(bool t) {
   //printd(5, "Datasource::setTS(%d) this=%08p\n", t, this);
   priv->in_transaction = t;
}

QoreListNode *Datasource::getCapabilityList() const {
   return priv->dsl->getCapList();
}

int Datasource::getCapabilities() const {
   return priv->dsl->getCaps();
}

bool Datasource::isInTransaction() const { 
   return priv->in_transaction; 
}

bool Datasource::getAutoCommit() const { 
   return priv->autocommit;
}

bool Datasource::isOpen() const { 
   return priv->isopen; 
}

Datasource *Datasource::copy() const {
   class Datasource *nds = new Datasource(priv->dsl);
   nds->priv->setPendingConnectionValues(priv);

   return nds;
}

void Datasource::setConnectionValues() {
   priv->setConnectionValues();
}

void Datasource::setAutoCommit(bool ac) {
   priv->autocommit = ac;
}

AbstractQoreNode *Datasource::select(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink) {
   AbstractQoreNode *rv = priv->dsl->select(this, query_str, args, xsink);
   if (priv->autocommit && !priv->connection_aborted)
      priv->dsl->autoCommit(this, xsink);

   return rv;
}

AbstractQoreNode *Datasource::selectRows(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink) {
   AbstractQoreNode *rv = priv->dsl->selectRows(this, query_str, args, xsink);
   if (priv->autocommit && !priv->connection_aborted)
      priv->dsl->autoCommit(this, xsink);

   return rv;
}

AbstractQoreNode *Datasource::exec(const QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink) {
   if (!priv->autocommit && !priv->in_transaction && beginImplicitTransaction(xsink))
      return 0;

   AbstractQoreNode *rv = priv->dsl->execSQL(this, query_str, args, xsink);
   //printd(5, "Datasource::exec() this=%08p, autocommit=%d, in_transaction=%d, xsink=%d\n", this, priv->autocommit, priv->in_transaction, xsink->isException());

   if (priv->connection_aborted) {
      assert(*xsink);
      assert(!rv);
      return 0;
   }

   if (priv->autocommit)
      priv->dsl->autoCommit(this, xsink);
   else if (!priv->in_transaction) {
      if (xsink->isException()) {
	 priv->dsl->abortTransactionStart(this, xsink);
      }
      else
	 priv->in_transaction = true;	 
   }
   
   return rv;
}

int Datasource::beginImplicitTransaction(ExceptionSink *xsink) {
   //printd(5, "Datasource::beginImplicitTransaction() autocommit=%s\n", autocommit ? "true" : "false");
   if (priv->autocommit) {
      xsink->raiseException("AUTOCOMMIT-ERROR", "transaction management is not available because autocommit is enabled for this Datasource");
      return -1;
   }
   return priv->dsl->beginTransaction(this, xsink);
}

int Datasource::beginTransaction(ExceptionSink *xsink) {
   int rc = beginImplicitTransaction(xsink);
   if (!rc)
      priv->in_transaction = true;
   return rc;
}

int Datasource::commit(ExceptionSink *xsink) {
   if (!priv->in_transaction && beginImplicitTransaction(xsink))
      return -1;

   int rc = priv->dsl->commit(this, xsink);
   priv->in_transaction = false;
   return rc;
}

int Datasource::rollback(ExceptionSink *xsink) {
   if (!priv->in_transaction && beginImplicitTransaction(xsink))
      return -1;

   int rc = priv->dsl->rollback(this, xsink);
   priv->in_transaction = false;
   return rc;
}

int Datasource::open(ExceptionSink *xsink) {
   int rc;
   
   if (!priv->isopen) {
      // copy pending connection values to connection values
      setConnectionValues();
      
      priv->connection_aborted = false;

      rc = priv->dsl->init(this, xsink);
      if (!xsink->isEvent())
	 priv->isopen = true;
   }
   else
      rc = 0;
   
   return rc;
}

int Datasource::close() {
   if (priv->isopen) {
      priv->dsl->close(this);
      priv->isopen = false;
      priv->in_transaction = false;
      return 0;
   }
   return -1;
}

void Datasource::connectionAborted() {
   assert(priv->isopen);

   priv->connection_aborted = true;
   close();
}

bool Datasource::wasConnectionAborted() const {
   return priv->connection_aborted;
}

// forces a close and open to reset a database connection
void Datasource::reset(ExceptionSink *xsink) {
   if (priv->isopen) {
      // close the Datasource
      priv->dsl->close(this);
      priv->isopen = false;
      
      // open the connection
      open(xsink);
      
      // close any open transaction(s)
      priv->in_transaction = false;
   }
}

void *Datasource::getPrivateData() const {
   return priv->private_data;
}

void Datasource::setPrivateData(void *data) {
   priv->private_data = data;
}

void Datasource::setPendingUsername(const char *u) {
   priv->p_username = u;
}

void Datasource::setPendingPassword(const char *p) {
   priv->p_password = p;
}

void Datasource::setPendingDBName(const char *d) {
   priv->p_dbname = d;
}

void Datasource::setPendingDBEncoding(const char *c) {
   priv->p_db_encoding = c;
}

void Datasource::setPendingHostName(const char *h) {
   priv->p_hostname = h;
}

void Datasource::setPendingPort(int port) {
   priv->p_port = port;
}

const std::string &Datasource::getUsernameStr() const {
   return priv->username;
}

const std::string &Datasource::getPasswordStr() const {
   return priv->password;
}

const std::string &Datasource::getDBNameStr() const {
   return priv->dbname;
}

const std::string &Datasource::getDBEncodingStr() const {
   return priv->db_encoding;
}

const std::string &Datasource::getHostNameStr() const {
   return priv->hostname;
}

const char *Datasource::getUsername() const {
   return priv->username.empty() ? 0 : priv->username.c_str();
}

const char *Datasource::getPassword() const {
   return priv->password.empty() ? 0 : priv->password.c_str();
}

const char *Datasource::getDBName() const {
   return priv->dbname.empty() ? 0 : priv->dbname.c_str();
}

const char *Datasource::getDBEncoding() const {
   return priv->db_encoding.empty() ? 0 : priv->db_encoding.c_str();
}

const char *Datasource::getOSEncoding() const {
   return priv->qorecharset ? priv->qorecharset->getCode() : 0;
}

const char *Datasource::getHostName() const {
   return priv->hostname.empty() ? 0 : priv->hostname.c_str();
}

int Datasource::getPort() const {
   return priv->port;
}

const QoreEncoding *Datasource::getQoreEncoding() const {
   return priv->qorecharset;
}

void Datasource::setDBEncoding(const char *name) {
   priv->db_encoding = name;
}

void Datasource::setQoreEncoding(const char *name) {
   priv->qorecharset = QEM.findCreate(name);
}

void Datasource::setQoreEncoding(const QoreEncoding *enc) {
   priv->qorecharset = enc;
}

QoreStringNode *Datasource::getPendingUsername() const {
   return priv->p_username.empty() ? 0 : new QoreStringNode(priv->p_username.c_str());
}

QoreStringNode *Datasource::getPendingPassword() const {
   return priv->p_password.empty() ? 0 : new QoreStringNode(priv->p_password.c_str());
}

QoreStringNode *Datasource::getPendingDBName() const {
   return priv->p_dbname.empty() ? 0 : new QoreStringNode(priv->p_dbname.c_str());
}

QoreStringNode *Datasource::getPendingDBEncoding() const {
   return priv->p_db_encoding.empty() ? 0 : new QoreStringNode(priv->p_db_encoding.c_str());
}

QoreStringNode *Datasource::getPendingHostName() const {
   return priv->p_hostname.empty() ? 0 : new QoreStringNode(priv->p_hostname.c_str());
}

int Datasource::getPendingPort() const {
   return priv->p_port;
}

const char *Datasource::getDriverName() const {
   return priv->dsl->getName();
}

const DBIDriver *Datasource::getDriver() const {
   return priv->dsl;
}

AbstractQoreNode *Datasource::getServerVersion(ExceptionSink *xsink) {
   return priv->dsl->getServerVersion(this, xsink);
}

AbstractQoreNode *Datasource::getClientVersion(ExceptionSink *xsink) const {
   return priv->dsl->getClientVersion(this, xsink);
}
