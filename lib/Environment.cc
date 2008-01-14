/*
  Environment.cc

  Qore Programming Language

  Copyright (C) 2003,2004,2005 David Nichols

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

#include <string.h>
#include <string>

class Environment Env;

#ifdef NEED_ENVIRON_LOCK
static class LockedObject lck_environ;
#endif

Environment::Environment()
{
}

Environment::~Environment()
{
}

int Environment::set(const char *name, const char *value, int overwrite)
{
   int rc;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
#ifdef HAVE_SETENV
   rc = setenv(name, value, overwrite);
#else
   std::string str = name;
   str += "=";
   str += value;
   rc = putenv(strdup(str.c_str()));
#endif
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
   return rc;
}

int Environment::get(const char *name, class QoreString *str)
{
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
   char *p = getenv(name);
   if (p)
      str->concat(p);
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
   return p ? 0 : -1;
}

class QoreString *Environment::get(const char *name)
{
   class QoreString *str;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
   char *p = getenv(name);
   str = p ? new QoreString(p) : NULL;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
   return str;
}

class QoreStringNode *Environment::getAsStringNode(const char *name)
{
   class QoreStringNode *str;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
   char *p = getenv(name);
   str = p ? new QoreStringNode(p) : NULL;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
   return str;
}

int Environment::unset(const char *name)
{
   int rc;
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
#ifdef HAVE_UNSETENV
   unsetenv(name);
   rc = 0;
#else
   // FIXME: here we fake it - we don't actually remove the variable from the environment, but we set it to nothing...
   std::string str = name;
   str += "=";
   rc = putenv(strdup(str.c_str()));
#endif
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
   return rc;
}

bool Environment::valueExists(const char* name)
{
  if (!name || !name[0]) return false;
  QoreString* s = get(name);
  if (!s) return false;
  const char* str = s->getBuffer();
  bool result = str && str[0];
  delete s;
  return result;
}

