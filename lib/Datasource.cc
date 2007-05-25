/*
 Datasource.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
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

Datasource::Datasource(DBIDriver *ndsl)
{
   dsl = ndsl;
   isopen = false;
   in_transaction = false;
   private_data = NULL;
   autocommit = false;
   qorecharset = QCS_DEFAULT;
}

Datasource::~Datasource()
{
   if (isopen)
      close();
#ifdef DEBUG
   if (private_data) 
      printe("ERROR: Datasource::~Datasource() private_data is not NULL\n");
#endif
}

List *Datasource::getCapabilityList() const
{
   return dsl->getCapList();
}

int Datasource::getCapabilities() const
{
   return dsl->getCaps();
}

bool Datasource::isInTransaction() const
{ 
   return in_transaction; 
}

bool Datasource::getAutoCommit() const
{ 
   return autocommit;
}

bool Datasource::isOpen() const
{ 
   return isopen; 
}

Datasource *Datasource::copy() const
{
   class Datasource *nds = new Datasource(dsl);
   
   nds->p_username    = p_username;
   nds->p_password    = p_password;
   nds->p_dbname      = p_dbname;
   nds->p_db_encoding = p_db_encoding;
   nds->p_hostname    = p_hostname;
   nds->autocommit    = autocommit;
   return nds;
}

void Datasource::setConnectionValues()
{
   dbname      = p_dbname;
   username    = p_username;
   password    = p_password;
   hostname    = p_hostname;
   db_encoding = p_db_encoding;
}

void Datasource::setAutoCommit(bool ac)
{
   autocommit = ac;
}

QoreNode *Datasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   QoreNode *rv = dsl->select(this, query_str, args, xsink);
   if (autocommit)
      dsl->autoCommit(this, xsink);
   return rv;
}

QoreNode *Datasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   QoreNode *rv = dsl->selectRows(this, query_str, args, xsink);
   if (autocommit)
      dsl->autoCommit(this, xsink);
   return rv;
}

QoreNode *Datasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   if (!autocommit && !in_transaction && beginTransaction(xsink))
      return NULL;

   class QoreNode *rv = dsl->execSQL(this, query_str, args, xsink);
   if (autocommit)
      dsl->autoCommit(this, xsink);
   else if (!in_transaction)
   {
      if (xsink->isException())
	 dsl->abortTransactionStart(this, xsink);
      else
	 in_transaction = true;	 
   }
   
   return rv;
}

int Datasource::beginTransaction(class ExceptionSink *xsink)
{
   //printd(0, "Datasource::beginTransaction() autocommit=%s\n", autocommit ? "true" : "false");
   if (autocommit)
   {
      xsink->raiseException("AUTOCOMMIT-ERROR", "transaction management is not available because autocommit is enabled for this Datasource");
      return -1;
   }
   int rc = dsl->beginTransaction(this, xsink);
   if (!rc)
      in_transaction = true;
   return rc;
}

int Datasource::commit(ExceptionSink *xsink)
{
   if (!in_transaction && beginTransaction(xsink))
      return -1;

   int rc = dsl->commit(this, xsink);
   in_transaction = false;
   return rc;
}

int Datasource::rollback(ExceptionSink *xsink)
{
   if (!in_transaction && beginTransaction(xsink))
      return -1;

   int rc = dsl->rollback(this, xsink);
   in_transaction = false;
   return rc;
}

int Datasource::open(ExceptionSink *xsink)
{
   int rc;
   
   if (!isopen)
   {
      // copy pending connection values to connection values
      setConnectionValues();
      
      rc = dsl->init(this, xsink);
      if (!xsink->isEvent())
	 isopen = true;
   }
   else
      rc = 0;
   
   return rc;
}

int Datasource::close()
{
   if (isopen)
   {
      dsl->close(this);
      isopen = false;
      in_transaction = false;
      return 0;
   }
   return -1;
}

// forces a close and open to reset a database connection
void Datasource::reset(ExceptionSink *xsink)
{
   if (isopen)
   {
      // close the Datasource
      dsl->close(this);
      isopen = false;
      
      // open the connection
      open(xsink);
      
      // close any open transaction(s)
      in_transaction = false;
   }
}

void *Datasource::getPrivateData() const
{
   return private_data;
}

void Datasource::setPrivateData(void *data)
{
   private_data = data;
}

void Datasource::setPendingUsername(const char *u)
{
   p_username = u;
}

void Datasource::setPendingPassword(const char *p)
{
   p_password = p;
}

void Datasource::setPendingDBName(const char *d)
{
   p_dbname = d;
}

void Datasource::setPendingDBEncoding(const char *c)
{
   p_db_encoding = c;
}

void Datasource::setPendingHostName(const char *h)
{
   p_hostname = h;
}

const char *Datasource::getUsername() const
{
   return username.empty() ? NULL : username.c_str();
}

const char *Datasource::getPassword() const
{
   return password.empty() ? NULL : password.c_str();
}

const char *Datasource::getDBName() const
{
   return dbname.empty() ? NULL : dbname.c_str();
}

const char *Datasource::getDBEncoding() const
{
   return db_encoding.empty() ? NULL : db_encoding.c_str();
}

const char *Datasource::getOSEncoding() const
{
   return qorecharset ? qorecharset->getCode() : NULL;
}

const char *Datasource::getHostName() const
{
   return hostname.empty() ? NULL : hostname.c_str();
}

class QoreEncoding *Datasource::getQoreEncoding() const
{
   return qorecharset;
}

void Datasource::setDBEncoding(const char *name)
{
   db_encoding = name;
}

void Datasource::setQoreEncoding(const char *name)
{
   qorecharset = QEM.findCreate(name);
}

void Datasource::setQoreEncoding(class QoreEncoding *enc)
{
   qorecharset = enc;
}

QoreNode *Datasource::getPendingUsername() const
{
   return p_username.empty() ? NULL : new QoreNode(p_username.c_str());
}

QoreNode *Datasource::getPendingPassword() const
{
   return p_password.empty() ? NULL : new QoreNode(p_password.c_str());
}

QoreNode *Datasource::getPendingDBName() const
{
   return p_dbname.empty() ? NULL : new QoreNode(p_dbname.c_str());
}

QoreNode *Datasource::getPendingDBEncoding() const
{
   return p_db_encoding.empty() ? NULL : new QoreNode(p_db_encoding.c_str());
}

QoreNode *Datasource::getPendingHostName() const
{
   return p_hostname.empty() ? NULL : new QoreNode(p_hostname.c_str());
}

const char *Datasource::getDriverName() const
{
   return dsl->getName();
}

class DBIDriver *Datasource::getDriver() const
{
   return dsl;
}

class QoreNode *Datasource::getServerVersion(class ExceptionSink *xsink)
{
   return dsl->getServerVersion(this, xsink);
}

class QoreNode *Datasource::getClientVersion()
{
   return dsl->getClientVersion();
}
