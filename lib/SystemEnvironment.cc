/*
  Environment.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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
#include <memory>

SystemEnvironment SysEnv;

#ifdef NEED_ENVIRON_LOCK
static QoreThreadLock lck_environ;
#endif

SystemEnvironment::SystemEnvironment()
{
}

SystemEnvironment::~SystemEnvironment()
{
}

int SystemEnvironment::set_intern(const char *name, const char *value, bool overwrite)
{
#ifdef HAVE_SETENV
   return setenv(name, value, (int)overwrite);
#else
   if (!overwrite && getenv(name))
      return -1;
   std::string str = name;
   str += "=";
   str += value;
   return putenv(strdup(str.c_str()));
#endif
}

int SystemEnvironment::get_intern(const char *name, QoreString &str)
{
   char *p = getenv(name);
   if (p)
      str.concat(p);
   return p ? 0 : -1;
}

QoreString *SystemEnvironment::get_intern(const char *name)
{
   char *p = getenv(name);
   return p ? new QoreString(p) : 0;
}

QoreStringNode *SystemEnvironment::get_as_string_node_intern(const char *name)
{
   char *p = getenv(name);
   return p ? new QoreStringNode(p) : 0;
}

int SystemEnvironment::unset_intern(const char *name)
{
#ifdef HAVE_UNSETENV
   unsetenv(name);
   return 0;
#else
   // FIXME: here we fake it - we don't actually remove the variable from the environment, but we set it to nothing...
   std::string str = name;
   str += "=";
   return putenv(strdup(str.c_str()));
#endif
}

int SystemEnvironment::set(const char *name, const char *value, bool overwrite)
{
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return set_intern(name, value, overwrite);
}

int SystemEnvironment::get(const char *name, QoreString &str)
{
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_intern(name, str);
}

class QoreString *SystemEnvironment::get(const char *name)
{
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_intern(name);
}

QoreStringNode *SystemEnvironment::getAsStringNode(const char *name)
{
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_as_string_node_intern(name);
}

int SystemEnvironment::unset(const char *name)
{
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return unset_intern(name);
}

bool SystemEnvironment::valueExists(const char* name)
{
  if (!name || !name[0]) return false;
  QoreString *s = get(name);
  if (!s) return false;
  std::auto_ptr<QoreString> holder(s);
  const char *str = s->getBuffer();
  return str && str[0];
}

AtomicEnvironmentSetter::AtomicEnvironmentSetter()
{
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
}

AtomicEnvironmentSetter::~AtomicEnvironmentSetter()
{
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
}

int AtomicEnvironmentSetter::set(const char *name, const char *value, bool overwrite)
{
   return SystemEnvironment::set_intern(name, value, overwrite);
}

int AtomicEnvironmentSetter::get(const char *name, QoreString &str)
{
   return SystemEnvironment::get_intern(name, str);
}

class QoreString *AtomicEnvironmentSetter::get(const char *name)
{
   return SystemEnvironment::get_intern(name);
}

QoreStringNode *AtomicEnvironmentSetter::getAsStringNode(const char *name)
{
   return SystemEnvironment::get_as_string_node_intern(name);
}

int AtomicEnvironmentSetter::unset(const char *name)
{
   return SystemEnvironment::unset_intern(name);
}

bool AtomicEnvironmentSetter::valueExists(const char* name)
{
  if (!name || !name[0]) return false;
  QoreString *s = SystemEnvironment::get(name);
  if (!s) return false;
  std::auto_ptr<QoreString> holder(s);
  const char *str = s->getBuffer();
  return str && str[0];
}
