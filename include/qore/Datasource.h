/*
  Datasource.h

  Qore Programming Language
 
  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
  The Datasource class provides the low-level interface to Qore DBI drivers.
 
  NOTE that this class is *not* thread-safe.  To use this class in a multi-
  threaded context, per-thread connection locking must be done at a level
  above this class...
 
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

#ifndef _QORE_DATASOURCE_H

#define _QORE_DATASOURCE_H

#include <qore/LockedObject.h>

#include <string>

struct qore_ds_private;

class Datasource
{
   private:
      struct qore_ds_private *priv; // private implementation

      // not implemented
      DLLLOCAL Datasource(const Datasource&);
      DLLLOCAL Datasource& operator=(const Datasource&);
      
   protected:
      DLLEXPORT void freeConnectionValues();
      DLLEXPORT void setConnectionValues();
      DLLEXPORT void setTransactionStatus(bool);
      DLLEXPORT void setPendingConnectionValues(const Datasource *other);
      DLLEXPORT int beginImplicitTransaction(class ExceptionSink *xsink);

   public:
      DLLEXPORT Datasource(class DBIDriver *);
      DLLEXPORT virtual ~Datasource();
      DLLEXPORT bool getAutoCommit() const;
      DLLEXPORT const char *getUsername() const;
      DLLEXPORT const char *getPassword() const;
      DLLEXPORT const char *getDBName() const;
      DLLEXPORT const char *getDBEncoding() const;
      DLLEXPORT const char *getOSEncoding() const;
      DLLEXPORT const char *getHostName() const;
      DLLEXPORT void *getPrivateData() const;
      DLLEXPORT void setPrivateData(void *data);
      DLLEXPORT void setDBEncoding(const char *name);
      DLLEXPORT const class QoreEncoding *getQoreEncoding() const;
      DLLEXPORT void setQoreEncoding(const class QoreEncoding *enc);
      DLLEXPORT void setQoreEncoding(const char *name);
      DLLEXPORT void setPendingUsername(const char *u);
      DLLEXPORT void setPendingPassword(const char *p);
      DLLEXPORT void setPendingDBName(const char *d);
      DLLEXPORT void setPendingDBEncoding(const char *c);
      DLLEXPORT void setPendingHostName(const char *h);
      DLLEXPORT void setAutoCommit(bool ac);
      DLLEXPORT int open(ExceptionSink *xsink);
      // not "const" to allow for reconnects
      DLLEXPORT class AbstractQoreNode *select(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
      // not "const" to allow for reconnects
      DLLEXPORT class AbstractQoreNode *selectRows(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
      // not "const" to allow for reconnects and also to change transaction status
      DLLEXPORT class AbstractQoreNode *exec(const class QoreString *query_str, const QoreListNode *args, ExceptionSink *xsink);
      // not "const" to allow for reconnects
      DLLEXPORT int commit(ExceptionSink *xsink);
      // not "const" to allow for reconnects
      DLLEXPORT int rollback(ExceptionSink *xsink);
      DLLEXPORT int close();
      DLLEXPORT void reset(ExceptionSink *xsink);
      // caller owns the pointer returned
      DLLEXPORT QoreListNode *getCapabilityList() const;
      DLLEXPORT int getCapabilities() const;
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT AbstractQoreNode *getPendingUsername() const;
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT AbstractQoreNode *getPendingPassword() const;
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT AbstractQoreNode *getPendingDBName() const;
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT AbstractQoreNode *getPendingDBEncoding() const;
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT AbstractQoreNode *getPendingHostName() const;
      DLLEXPORT void setTransactionLockTimeout(int t);
      DLLEXPORT int getTransactionLockTimeout() const;
      // returns -1 for error, 0 for OK
      DLLEXPORT int beginTransaction(class ExceptionSink *xsink);
      DLLEXPORT bool isInTransaction() const;
      DLLEXPORT bool isOpen() const;
      DLLEXPORT Datasource *copy() const;
      DLLEXPORT const char *getDriverName() const;
      // caller owns the AbstractQoreNode reference returned; not "const" to allow for reconnects
      DLLEXPORT class AbstractQoreNode *getServerVersion(class ExceptionSink *xsink);
      // caller owns the AbstractQoreNode reference returned
      DLLEXPORT class AbstractQoreNode *getClientVersion(class ExceptionSink *xsink) const;

      DLLEXPORT const class DBIDriver *getDriver() const;
};

#endif // _QORE_DATASOURCE_H
