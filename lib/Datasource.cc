/*
 Datasource.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006 David Nichols
 
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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/Datasource.h>
#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Exception.h>
#include <qore/DBI.h>
#include <qore/qore_thread.h>

#include <stdlib.h>
#include <string.h>

Datasource::Datasource(DBIDriver *ndsl)
{
   dsl = ndsl;
   isopen = false;
   in_transaction = false;
   private_data = NULL;
   p_dbname = p_username = p_password = p_db_encoding = p_hostname = NULL;
   dbname = username = password = db_encoding = hostname = NULL;
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
   if (p_dbname)      free(p_dbname);
   if (p_username)    free(p_username);
   if (p_password)    free(p_password);
   if (p_db_encoding) free(p_db_encoding);
   if (p_hostname)    free(p_hostname);
   
   freeConnectionValues();
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
   
   nds->p_username    = p_username    ? strdup(p_username)    : NULL;
   nds->p_password    = p_password    ? strdup(p_password)    : NULL;
   nds->p_dbname      = p_dbname      ? strdup(p_dbname)      : NULL;
   nds->p_db_encoding = p_db_encoding ? strdup(p_db_encoding) : NULL;
   nds->p_hostname    = p_hostname    ? strdup(p_hostname)    : NULL;
   nds->autocommit    = autocommit;
   return nds;
}

void Datasource::freeConnectionValues()
{
   if (dbname)      free(dbname);
   if (username)    free(username);
   if (password)    free(password);
   if (db_encoding) free(db_encoding);
   if (hostname)    free(hostname);
}

void Datasource::setConnectionValues()
{
   dbname      = p_dbname      ? strdup(p_dbname)      : NULL;
   username    = p_username    ? strdup(p_username)    : NULL;
   password    = p_password    ? strdup(p_password)    : NULL;
   hostname    = p_hostname    ? strdup(p_hostname)    : NULL;
   db_encoding = p_db_encoding ? strdup(p_db_encoding) : NULL;
}

void Datasource::setAutoCommit(bool ac)
{
   autocommit = ac;
}

QoreNode *Datasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   return dsl->select(this, query_str, args, xsink);
}

QoreNode *Datasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   return dsl->selectRows(this, query_str, args, xsink);
}

QoreNode *Datasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv = dsl->execSQL(this, query_str, args, xsink);
   if (!autocommit && !in_transaction && !xsink->isException())
      in_transaction = true;
   
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
   
   in_transaction = true;
   return 0;
}

int Datasource::commit(ExceptionSink *xsink)
{
   int rc = dsl->commit(this, xsink);
   in_transaction = false;
   return rc;
}

int Datasource::rollback(ExceptionSink *xsink)
{
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
      freeConnectionValues();
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

void Datasource::setPendingUsername(char *u)
{
   if (p_username)
      free(p_username);
   p_username = strdup(u);
}

void Datasource::setPendingPassword(char *p)
{
   if (p_password)
      free(p_password);
   p_password = strdup(p);
}

void Datasource::setPendingDBName(char *d)
{
   if (p_dbname)
      free(p_dbname);
   p_dbname = strdup(d);
}

void Datasource::setPendingDBEncoding(char *c)
{
   if (p_db_encoding)
      free(p_db_encoding);
   p_db_encoding = strdup(c);
}

void Datasource::setPendingHostName(char *h)
{
   if (p_hostname)
      free(p_hostname);
   p_hostname = strdup(h);
}

char *Datasource::getUsername() const
{
   return username;
}

char *Datasource::getPassword() const
{
   return password;
}

char *Datasource::getDBName() const
{
   return dbname;
}

char *Datasource::getDBEncoding() const
{
   return db_encoding;
}

char *Datasource::getOSEncoding() const
{
   return qorecharset ? qorecharset->code : NULL;
}

char *Datasource::getHostName() const
{
   return hostname;
}

class QoreEncoding *Datasource::getQoreEncoding() const
{
   return qorecharset;
}

void Datasource::setDBEncoding(char *name)
{
   if (db_encoding)
      free(db_encoding);
   db_encoding = strdup(name);
}

void Datasource::setQoreEncoding(char *name)
{
   qorecharset = QEM.findCreate(name);
}

void Datasource::setQoreEncoding(class QoreEncoding *enc)
{
   qorecharset = enc;
}

QoreNode *Datasource::getPendingUsername() const
{
   QoreNode *rv;
   if (p_username)
      rv = new QoreNode(p_username);
   else
      rv = NULL;
   return rv;
}

QoreNode *Datasource::getPendingPassword() const
{
   QoreNode *rv;
   if (p_password)
      rv = new QoreNode(p_password);
   else
      rv = NULL;
   return rv;
}

QoreNode *Datasource::getPendingDBName() const
{
   QoreNode *rv;
   if (p_dbname)
      rv = new QoreNode(p_dbname);
   else
      rv = NULL;
   return rv;
}

QoreNode *Datasource::getPendingDBEncoding() const
{
   QoreNode *rv;
   if (p_db_encoding)
      rv = new QoreNode(p_db_encoding);
   else
      rv = NULL;
   return rv;
}

QoreNode *Datasource::getPendingHostName() const
{
   QoreNode *rv;
   if (p_hostname)
      rv = new QoreNode(p_hostname);
   else
      rv = NULL;
   return rv;
}
