/*
  ql_pwd.cc
  
  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/ql_env.h>

#include <stdio.h>
#include <pwd.h>

// for the getpwuid function
static class LockedObject lck_getpwuid;

static inline void assign_value(QoreHash *h, char *key, char *val)
{
   h->setKeyValue(key, new QoreNode(val), NULL);
}

static inline void assign_value(QoreHash *h, char *key, int val)
{
   h->setKeyValue(key, new QoreNode(NT_INT, val), NULL);
}

static class QoreNode *f_getpwuid(class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *rv;

   tracein("f_getpwuid()");
   if (!(p0 = get_param(params, 0)))
   {
      traceout("f_getpwuid()");      
      return NULL;
   }

   lck_getpwuid.lock();
   
   struct passwd *pw = getpwuid(p0->getAsInt());
   if (pw)
   {
      QoreHash *h = new QoreHash();
      // assign values
      assign_value(h, "pw_name", pw->pw_name);
      assign_value(h, "pw_passwd", pw->pw_passwd);
      assign_value(h, "pw_gecos", pw->pw_gecos);
      assign_value(h, "pw_dir", pw->pw_dir);
      assign_value(h, "pw_shell", pw->pw_shell);
      assign_value(h, "pw_uid", pw->pw_uid);
      assign_value(h, "pw_gid", pw->pw_gid);
      rv = new QoreNode(h);
   }
   else
      rv = NULL;

   lck_getpwuid.unlock();

   traceout("f_getpwuid()");
   return rv;
}

void init_pwd_functions()
{
   builtinFunctions.add("getpwuid", f_getpwuid);
}
