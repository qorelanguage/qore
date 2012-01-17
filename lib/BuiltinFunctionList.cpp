/*
  BuiltinFunctionList.cpp

  Qore Programming language

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

#include <qore/intern/ql_io.h>
#include <qore/intern/ql_time.h>
#include <qore/intern/ql_lib.h>
#include <qore/intern/ql_math.h>
#include <qore/intern/ql_type.h>
#include <qore/intern/ql_env.h>
#include <qore/intern/ql_string.h>
#include <qore/intern/ql_pwd.h>
#include <qore/intern/ql_misc.h>
#include <qore/intern/ql_list.h>
#include <qore/intern/ql_thread.h>
#include <qore/intern/ql_crypto.h>
#include <qore/intern/ql_object.h>
#include <qore/intern/ql_file.h>
#include <qore/intern/ql_compression.h>

#ifdef DEBUG
#include <qore/intern/ql_debug.h>
#endif // DEBUG

#include <string.h>
#include <assert.h>

static bool library_init_done = false;

BuiltinFunctionList builtinFunctions;

typedef std::map<const char*, BuiltinFunction *, class ltstr> hm_bf_t;

void qore_process_params(unsigned num_params, type_vec_t &typeList, arg_vec_t &defaultArgList, va_list args) {
   typeList.reserve(num_params);
   defaultArgList.reserve(num_params);
   for (unsigned i = 0; i < num_params; ++i) {
      typeList.push_back(va_arg(args, const QoreTypeInfo *));
      defaultArgList.push_back(va_arg(args, AbstractQoreNode *));
      //printd(0, "qore_process_params() i=%d/%d typeInfo=%p (%s) defArg=%p\n", i, num_params, typeList[i], typeList[i]->getTypeName(), defaultArgList[i]);
   }
}

class BFLAutoLocker {
protected:
   QoreThreadLock *mutex;
public:
   DLLLOCAL BFLAutoLocker(QoreThreadLock *n_mutex) : mutex(n_mutex) { if (mutex) mutex->lock(); }
   DLLLOCAL ~BFLAutoLocker() { if (mutex) mutex->unlock(); }
};

class BuiltinFunctionListPrivate {
   friend class BuiltinFunctionListOptionalLockHelper;
protected:
   hm_bf_t hm;
   // the mutex is needed because the list is global and also searched at runtime
   mutable QoreThreadLock mutex;

   DLLLOCAL int add_intern(const char *name, AbstractQoreFunctionVariant *bfv) {
      BFLAutoLocker al(library_init_done ? &mutex : 0);

      hm_bf_t::iterator i = hm.find(name);
      BuiltinFunction *bf;
      if (i == hm.end()) {
	 bf = new BuiltinFunction(name);
	 hm[bf->getName()] = bf;
      }
      else 
	 bf = i->second;
      bf->addBuiltinVariant(bfv);
      
      //printd(0, "BuiltinFunctionListPrivate::add_intern('%s', %p) flags=%lld\n", name, bfv, bfv->getFlags());
      return 0;
   }

public:
   DLLLOCAL BuiltinFunctionListPrivate() {
   }

   DLLLOCAL void add(const char *name, q_func_t f, int64 functional_domain) {
      add_intern(name, new BuiltinFunctionVariant(f, QC_USES_EXTRA_ARGS, functional_domain));
   }

   template <typename T, class B>
   void add2_t(const char *name, T f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      //printd(0, "add2('%s', %p, flags=%lld) BEFORE\n", name, f, flags);
      type_vec_t typeList;
      arg_vec_t defaultArgList;
      if (num_params)
	 qore_process_params(num_params, typeList, defaultArgList, args);

      //printd(0, "add2('%s', %p, flags=%lld, domain=%lld, ret=%s, num_params=%d, ...)\n", name, f, flags, functional_domain, returnTypeInfo->getName(), num_params);
      add_intern(name, new B(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList));
   }

   void add2(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      add2_t<q_func_t, BuiltinFunctionVariant>(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   }

   void add2(const char *name, q_func_int64_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      add2_t<q_func_int64_t, BuiltinFunctionBigIntVariant>(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   }

   void add2(const char *name, q_func_bool_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      add2_t<q_func_bool_t, BuiltinFunctionBoolVariant>(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   }

   void add2(const char *name, q_func_double_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, va_list args) {
      add2_t<q_func_double_t, BuiltinFunctionFloatVariant>(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   }

   void add3(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &typeList, const arg_vec_t &defaultArgList) {
      add_intern(name, new BuiltinFunctionVariant(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList));
   }

   DLLLOCAL void clear() {
      //printd(5, "BuiltinFunctionListPrivate::~BuiltinFunctionListPrivate() this=%08p\n", this);
      hm_bf_t::iterator i = hm.begin();
      while (i != hm.end()) {
	 //printd(5, "BuiltinFunctionListPrivate::~BuiltinFunctionListPrivate() deleting '%s()'\n", i->first);
	 
	 // delete function
	 delete i->second;
	 
	 // erase hash entry
	 hm.erase(i);
	 
	 i = hm.begin();
      }
   }

   DLLLOCAL const BuiltinFunction *find(const char *name) const;
   DLLLOCAL int size() const;
};

class BuiltinFunctionListOptionalLockHelper {
protected:
   const BuiltinFunctionListPrivate *l;
public:
   DLLLOCAL BuiltinFunctionListOptionalLockHelper(const BuiltinFunctionListPrivate *n_l) : l(n_l) {
      if (library_init_done)
	 l->mutex.lock();
   }
   DLLLOCAL ~BuiltinFunctionListOptionalLockHelper() {
      if (library_init_done)
	 l->mutex.unlock();
   }
};

static BuiltinFunctionListPrivate bfl;

const BuiltinFunction *BuiltinFunctionListPrivate::find(const char *name) const {
   BuiltinFunctionListOptionalLockHelper ol(this);
   hm_bf_t::const_iterator i = hm.find(name);
   return i != hm.end() ? i->second : 0;
}

int BuiltinFunctionListPrivate::size() const {
   BuiltinFunctionListOptionalLockHelper ol(this);
   return hm.size();
}

BuiltinFunctionList::BuiltinFunctionList() {
}

BuiltinFunctionList::~BuiltinFunctionList() {
//   assert(hm.empty());
}

void BuiltinFunctionList::add(const char *name, q_func_t f, int functional_domain) {
   bfl.add(name, f, functional_domain);
}

void BuiltinFunctionList::add2(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   bfl.add2(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void BuiltinFunctionList::add2(const char *name, q_func_int64_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   bfl.add2(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void BuiltinFunctionList::add2(const char *name, q_func_bool_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   bfl.add2(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void BuiltinFunctionList::add2(const char *name, q_func_double_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   va_list args;
   va_start(args, num_params);
   bfl.add2(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void BuiltinFunctionList::add3(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &typeList, const arg_vec_t &defaultArgList) {
   bfl.add3(name, f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList);
}

void BuiltinFunctionList::clear() {
   bfl.clear();
}

const BuiltinFunction *BuiltinFunctionList::find(const char *name) {
   return bfl.find(name);
}

inline int BuiltinFunctionList::size() {
   return bfl.size();
}

void BuiltinFunctionList::init(QoreNamespace& qns) {
   QORE_TRACE("BuiltinFunctionList::init()");

   init_string_functions(qns);
   init_io_functions();
   init_time_functions();
   init_lib_functions(qns);
   init_misc_functions(qns);
   init_list_functions();
   init_type_functions();
   init_pwd_functions();
   init_math_functions();
   init_env_functions();
   init_dbi_functions();
   init_thread_functions(qns);
   init_crypto_functions(qns);
   init_object_functions();
   init_file_functions(qns);
   init_compression_functions(qns);
#ifdef DEBUG
   init_debug_functions();
#endif

   library_init_done = true;
}
