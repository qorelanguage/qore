/*
  ql_pwd.cpp
  
  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#include <errno.h>

static AbstractQoreNode *f_getpwuid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   int uid = (int)HARD_QORE_INT(params, 0);
   return q_getpwuid(uid);
#else
   missing_function_error("getpwuid", "UNIX_USERMGT", xsink);
xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getpwuid2(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   int uid = (int)HARD_QORE_INT(params, 0);
   errno = 0;
   QoreHashNode *h = q_getpwuid(uid);
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPPWUID2-ERROR", "uid %d not found", uid);
      else
         xsink->raiseException("GETPPWUID2-ERROR", q_strerror(errno));
   }
   return h;
#else
   missing_function_error("getpwuid2", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getpwnam(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   return q_getpwnam(name->getBuffer());
#else
   missing_function_error("getpwnam", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getpwnam2(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   errno = 0;
   QoreHashNode *h = q_getpwnam(name->getBuffer());
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPPWNAM2-ERROR", "user '%s' not found", name->getBuffer());
      else
         xsink->raiseException("GETPPWNAM2-ERROR", q_strerror(errno));
   }
   return h;
#else
   missing_function_error("getpwnam2", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getgrgid(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   int gid = (int)HARD_QORE_INT(params, 0);
   return q_getgrgid(gid);
#else
   missing_function_error("getgrgid", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getgrgid2(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   int gid = (int)HARD_QORE_INT(params, 0);
   errno = 0;
   QoreHashNode *h = q_getgrgid(gid);
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPGRGID2-ERROR", "gid %d not found", gid);
      else
         xsink->raiseException("GETPGRGID2-ERROR", q_strerror(errno));
   }
   return h;
#else
   missing_function_error("getgrgid2", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getgrnam(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   return q_getgrnam(name->getBuffer());
#else
   missing_function_error("getgrnam", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

static AbstractQoreNode *f_getgrnam2(const QoreListNode *params, ExceptionSink *xsink) {
#ifdef HAVE_PWD_H
   const QoreStringNode *name = HARD_QORE_STRING(params, 0);
   errno = 0;
   QoreHashNode *h = q_getgrnam(name->getBuffer());
   if (!h) {
      if (!errno)
         xsink->raiseException("GETPGRNAM2-ERROR", "group '%s' not found", name->getBuffer());
      else
         xsink->raiseException("GETPGRNAM2-ERROR", q_strerror(errno));
   }
   return h;
#else
   missing_function_error("getgrnam2", "UNIX_USERMGT", xsink);
   return 0;
#endif
}

void init_pwd_functions() {
   builtinFunctions.add2("getpwuid", f_noop, QC_RUNTIME_NOOP, QDOM_EXTERNAL_INFO, nothingTypeInfo);

   // *hash getpwuid(softint $uid)  
   builtinFunctions.add2("getpwuid", f_getpwuid, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // hash getpwuid2(softint $uid)  
   builtinFunctions.add2("getpwuid2", f_getpwuid2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   
   // *hash getpwnam(string $group)  
   builtinFunctions.add2("getpwnam", f_getpwnam, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // hash getpwnam2(string $group)  
   builtinFunctions.add2("getpwnam2", f_getpwnam2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // *hash getgrgid(softint $gid)  
   builtinFunctions.add2("getgrgid", f_getgrgid, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // hash getgrgid2(softint $gid)  
   builtinFunctions.add2("getgrgid2", f_getgrgid2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);
   
   // *hash getgrnam(string $group)  
   builtinFunctions.add2("getgrnam", f_getgrnam, QC_CONSTANT, QDOM_EXTERNAL_INFO, hashOrNothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // hash getgrnam2(string $group)  
   builtinFunctions.add2("getgrnam2", f_getgrnam2, QC_NO_FLAGS, QDOM_EXTERNAL_INFO, hashTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
}
