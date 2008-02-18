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
#include <qore/intern/ql_env.h>

#include <stdio.h>
#include <pwd.h>

// for the getpwuid function
static class QoreThreadLock lck_getpwuid;

static inline void assign_value(QoreHashNode *h, char *key, char *val)
{
   h->setKeyValue(key, new QoreStringNode(val), NULL);
}

static inline void assign_value(QoreHashNode *h, char *key, int val)
{
   h->setKeyValue(key, new QoreBigIntNode(val), NULL);
}

static AbstractQoreNode *f_getpwuid(const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;

   if (!(p0 = get_param(params, 0)))
      return NULL;

   AutoLocker al(&lck_getpwuid);
   
   struct passwd *pw = getpwuid(p0->getAsInt());
   if (!pw)
      return 0;

   QoreHashNode *h = new QoreHashNode();
   // assign values
   assign_value(h, "pw_name", pw->pw_name);
   assign_value(h, "pw_passwd", pw->pw_passwd);
   assign_value(h, "pw_gecos", pw->pw_gecos);
   assign_value(h, "pw_dir", pw->pw_dir);
   assign_value(h, "pw_shell", pw->pw_shell);
   assign_value(h, "pw_uid", pw->pw_uid);
   assign_value(h, "pw_gid", pw->pw_gid);
   return h;
}

void init_pwd_functions()
{
   builtinFunctions.add("getpwuid", f_getpwuid);
}
