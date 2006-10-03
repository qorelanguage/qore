/*
  Datasource.h

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

/* 
   FIXME: commit()s when autocommit=true should be made here, also after
          select()s (in case of a select for update, for example)
 */

#ifndef _QORE_DATASOURCE_H

#define _QORE_DATASOURCE_H

#include <qore/config.h>
#include <qore/Exception.h>
#include <qore/DBI.h>
#include <qore/qore_thread.h>

#include <stdlib.h>
#include <string.h>

class Datasource
{
   protected:
      bool in_transaction;
      bool isopen;
      bool autocommit;

      // for pending connection values
      char *p_username,  // for Oracle, MySQL
	 *p_password,  // for Oracle, MySQL
	 *p_dbname,    // for Oracle, MySQL
	 *p_charset,   // for Oracle
	 *p_hostname;  // for MySQL

      inline void freeConnectionValues();
      inline void setConnectionValues();

   public:
      class QoreEncoding *qorecharset;
      class DBIDriver *dsl;
      void *private_data;

      // actual connection values set by init()
      char *username,  // for Oracle, MySQL
	 *password,  // for Oracle, MySQL
	 *dbname,    // for Oracle, MySQL
	 *charset,   // for Oracle
	 *hostname;  // for MySQL

      inline Datasource(DBIDriver *);
      inline ~Datasource();
      inline class QoreNode *select(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      inline class QoreNode *exec(class QoreString *query_str, class List *args, ExceptionSink *xsink);
      //inline class Hash *describe(char *table_name, ExceptionSink *xsink);
      inline int commit(ExceptionSink *xsink);
      inline int rollback(ExceptionSink *xsink);
      inline int open(ExceptionSink *xsink);
      inline int close();
      inline void reset(ExceptionSink *xsink);
      inline List *getCapabilityList()
      {
	 return dsl->getCapList();
      }
      inline int getCapabilities()
      {
	 return dsl->getCaps();
      }
      inline void setUsername(char *u);
      inline void setPassword(char *p);
      inline void setDBName(char *d);
      inline void setCharset(char *c);
      inline void setHostName(char *h);
      inline QoreNode *getUsername();
      inline QoreNode *getPassword();
      inline QoreNode *getDBName();
      inline QoreNode *getCharset();
      inline QoreNode *getHostName();
      
      inline void setTransactionLockTimeout(int t);
      inline int getTransactionLockTimeout();
      // returns -1 for error, 0 for OK
      inline int beginTransaction(class ExceptionSink *xsink);
      inline bool isInTransaction() 
      { 
	 return in_transaction; 
      }
      inline void setAutoCommit(bool ac);
      inline bool getAutoCommit() 
      { 
	 return autocommit;
      }
      inline bool isOpen()
      { 
	 return isopen; 
      }

      inline Datasource *copy()
      {
	 class Datasource *nds = new Datasource(dsl);
	 
	 nds->p_username = p_username ? strdup(p_username) : NULL;
	 nds->p_password = p_password ? strdup(p_password) : NULL;
	 nds->p_dbname   = p_dbname   ? strdup(p_dbname)   : NULL;
	 nds->p_charset  = p_charset  ? strdup(p_charset)  : NULL;
	 nds->p_hostname = p_hostname ? strdup(p_hostname) : NULL;
	 nds->autocommit = autocommit;
	 return nds;
      }
};

#include <qore/charset.h>
#include <qore/QoreNode.h>
#include <qore/Exception.h>

inline Datasource::Datasource(DBIDriver *ndsl)
{
   dsl = ndsl;
   isopen = false;
   in_transaction = false;
   private_data = NULL;
   p_dbname = p_username = p_password = p_charset = p_hostname = NULL;
   dbname = username = password = charset = hostname = NULL;
   autocommit = false;
   qorecharset = QCS_DEFAULT;
}

inline Datasource::~Datasource()
{
   if (isopen)
      close();
#ifdef DEBUG
   if (private_data) 
      printe("ERROR: Datasource::~Datasource() private_data is not NULL\n");
#endif
   if (p_dbname)   free(p_dbname);
   if (p_username) free(p_username);
   if (p_password) free(p_password);
   if (p_charset)  free(p_charset);
   if (p_hostname) free(p_hostname);

   freeConnectionValues();
}

inline void Datasource::freeConnectionValues()
{
   if (dbname)   free(dbname);
   if (username) free(username);
   if (password) free(password);
   if (charset)  free(charset);
   if (hostname) free(hostname);
}

inline void Datasource::setConnectionValues()
{
   dbname   = p_dbname   ? strdup(p_dbname)   : NULL;
   username = p_username ? strdup(p_username) : NULL;
   password = p_password ? strdup(p_password) : NULL;
   charset  = p_charset  ? strdup(p_charset)  : NULL;
   hostname = p_hostname ? strdup(p_hostname) : NULL;
}

inline void Datasource::setAutoCommit(bool ac)
{
   autocommit = ac;
}

inline QoreNode *Datasource::select(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   return dsl->select(this, query_str, args, xsink);
}

inline QoreNode *Datasource::selectRows(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   return dsl->selectRows(this, query_str, args, xsink);
}

inline QoreNode *Datasource::exec(class QoreString *query_str, class List *args, ExceptionSink *xsink)
{
   class QoreNode *rv = dsl->execSQL(this, query_str, args, xsink);
   if (!autocommit && !in_transaction && !xsink->isException())
      in_transaction = true;

   return rv;
}

inline int Datasource::beginTransaction(class ExceptionSink *xsink)
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

inline int Datasource::commit(ExceptionSink *xsink)
{
   int rc = dsl->commit(this, xsink);
   in_transaction = false;
   return rc;
}

inline int Datasource::rollback(ExceptionSink *xsink)
{
   int rc = dsl->rollback(this, xsink);
   in_transaction = false;
   return rc;
}

inline int Datasource::open(ExceptionSink *xsink)
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

inline int Datasource::close()
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
inline void Datasource::reset(ExceptionSink *xsink)
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

inline void Datasource::setUsername(char *u)
{
   if (p_username)
      free(p_username);
   p_username = strdup(u);
}

inline void Datasource::setPassword(char *p)
{
   if (p_password)
      free(p_password);
   p_password = strdup(p);
}

inline void Datasource::setDBName(char *d)
{
   if (p_dbname)
      free(p_dbname);
   p_dbname = strdup(d);
}

inline void Datasource::setCharset(char *c)
{
   if (p_charset)
      free(p_charset);
   p_charset = strdup(c);
}

inline void Datasource::setHostName(char *h)
{
   if (p_hostname)
      free(p_hostname);
   p_hostname = strdup(h);
}

inline QoreNode *Datasource::getUsername()
{
   QoreNode *rv;
   if (p_username)
      rv = new QoreNode(p_username);
   else
      rv = NULL;
   return rv;
}

inline QoreNode *Datasource::getPassword()
{
   QoreNode *rv;
   if (p_password)
      rv = new QoreNode(p_password);
   else
      rv = NULL;
   return rv;
}

inline QoreNode *Datasource::getDBName()
{
   QoreNode *rv;
   if (p_dbname)
      rv = new QoreNode(p_dbname);
   else
      rv = NULL;
   return rv;
}

inline QoreNode *Datasource::getCharset()
{
   QoreNode *rv;
   if (p_charset)
      rv = new QoreNode(p_charset);
   else
      rv = NULL;
   return rv;
}

inline QoreNode *Datasource::getHostName()
{
   QoreNode *rv;
   if (p_hostname)
      rv = new QoreNode(p_hostname);
   else
      rv = NULL;
   return rv;
}

#endif // _QORE_DATASOURCE_H
