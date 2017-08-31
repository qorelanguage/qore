/*
  Environment.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

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

#include <qore/Qore.h>

#include <string.h>
#include <string>
#include <memory>

SystemEnvironment SysEnv;

#ifdef NEED_ENVIRON_LOCK
static QoreThreadLock lck_environ;
#endif

SystemEnvironment::SystemEnvironment() {
}

SystemEnvironment::~SystemEnvironment() {
}

int SystemEnvironment::set_intern(const char *name, const char *value, bool overwrite) {
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

int SystemEnvironment::get_intern(const char *name, QoreString &str) {
   char *p = getenv(name);
   //printd(5, "SystemEnvironment::get_intern(name='%s') val=%s\n", name, p ? p : "(null)");
   if (p)
      str.concat(p);
   return p ? 0 : -1;
}

QoreString *SystemEnvironment::get_intern(const char *name) {
   char *p = getenv(name);
   return p ? new QoreString(p) : 0;
}

QoreStringNode *SystemEnvironment::get_as_string_node_intern(const char *name) {
   char *p = getenv(name);
   return p ? new QoreStringNode(p) : 0;
}

int SystemEnvironment::unset_intern(const char *name) {
#ifdef HAVE_UNSETENV
   return unsetenv(name);
#else
   // FIXME: here we fake it - we don't actually remove the variable from the environment, but we set it to nothing...
   std::string str = name;
   str += "=";
   return putenv(strdup(str.c_str()));
#endif
}

int SystemEnvironment::set(const char *name, const char *value, bool overwrite) {
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return set_intern(name, value, overwrite);
}

int SystemEnvironment::get(const char *name, QoreString &str) {
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_intern(name, str);
}

QoreString *SystemEnvironment::get(const char *name) {
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_intern(name);
}

QoreStringNode *SystemEnvironment::getAsStringNode(const char *name) {
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return get_as_string_node_intern(name);
}

int SystemEnvironment::unset(const char *name) {
#ifdef NEED_ENVIRON_LOCK
   AutoLocker al(lck_environ);
#endif
   return unset_intern(name);
}

bool SystemEnvironment::valueExists(const char* name) {
  if (!name || !name[0]) return false;
  QoreString *s = get(name);
  if (!s) return false;
  std::unique_ptr<QoreString> holder(s);
  const char *str = s->getBuffer();
  return str && str[0];
}

AtomicEnvironmentSetter::AtomicEnvironmentSetter() {
#ifdef NEED_ENVIRON_LOCK
   lck_environ.lock();
#endif
}

AtomicEnvironmentSetter::~AtomicEnvironmentSetter() {
#ifdef NEED_ENVIRON_LOCK
   lck_environ.unlock();
#endif
}

int AtomicEnvironmentSetter::set(const char *name, const char *value, bool overwrite) {
   return SystemEnvironment::set_intern(name, value, overwrite);
}

int AtomicEnvironmentSetter::get(const char *name, QoreString &str) {
   return SystemEnvironment::get_intern(name, str);
}

QoreString *AtomicEnvironmentSetter::get(const char *name) {
   return SystemEnvironment::get_intern(name);
}

QoreStringNode *AtomicEnvironmentSetter::getAsStringNode(const char *name) {
   return SystemEnvironment::get_as_string_node_intern(name);
}

int AtomicEnvironmentSetter::unset(const char *name) {
   return SystemEnvironment::unset_intern(name);
}

bool AtomicEnvironmentSetter::valueExists(const char* name) {
  if (!name || !name[0]) return false;
  QoreString *s = SystemEnvironment::get(name);
  if (!s) return false;
  std::unique_ptr<QoreString> holder(s);
  const char *str = s->getBuffer();
  return str && str[0];
}
