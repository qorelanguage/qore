/*
  BuiltinFunctionList.cpp

  Qore Programming language

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
#include <qore/intern/QoreNamespaceIntern.h>

#ifdef DEBUG
#include <qore/intern/ql_debug.h>
#endif // DEBUG

#include <qore/intern/QoreNamespaceIntern.h>

#include <string.h>
#include <assert.h>

void init_context_functions(QoreNamespace& ns);

//static bool library_init_done = false;

BuiltinFunctionList builtinFunctions;

//typedef std::map<const char*, QoreFunction*, class ltstr> hm_bf_t;

void qore_process_params(unsigned num_params, type_vec_t &typeList, arg_vec_t &defaultArgList, va_list args) {
   typeList.reserve(num_params);
   defaultArgList.reserve(num_params);
   for (unsigned i = 0; i < num_params; ++i) {
      typeList.push_back(va_arg(args, const QoreTypeInfo *));
      defaultArgList.push_back(va_arg(args, AbstractQoreNode *));
      //printd(0, "qore_process_params() i=%d/%d typeInfo=%p (%s) defArg=%p\n", i, num_params, typeList[i], typeList[i]->getTypeName(), defaultArgList[i]);
   }
}

void qore_process_params(unsigned num_params, type_vec_t &typeList, arg_vec_t &defaultArgList, name_vec_t& nameList, va_list args) {
   typeList.reserve(num_params);
   defaultArgList.reserve(num_params);
   nameList.reserve(num_params);
   for (unsigned i = 0; i < num_params; ++i) {
      typeList.push_back(va_arg(args, const QoreTypeInfo *));
      defaultArgList.push_back(va_arg(args, AbstractQoreNode *));
      nameList.push_back(va_arg(args, const char*));
      //printd(0, "qore_process_params() i=%d/%d typeInfo=%p (%s) defArg=%p\n", i, num_params, typeList[i], typeList[i]->getTypeName(), defaultArgList[i]);
   }
}

static int check_dup(QoreModuleContext& qmc, const char* name) {
   if (qmc.getRootNS()->getQore()->findAnyFunction(name)) {
      qmc.error("function '%s()' has already been declared in namespace 'Qore'", name);
      return -1;
   }
   return 0;
}

BuiltinFunctionList::BuiltinFunctionList() {
}

int BuiltinFunctionList::size() {
   return 0;
}

const QoreFunction* BuiltinFunctionList::find(const char *name) {
   QoreProgram* pgm = getProgram();
   if (!pgm)
      return 0;

   const qore_ns_private* ns = 0;
   return qore_root_ns_private::runtimeFindFunction(*(pgm->getRootNS()), name, ns);
}

void BuiltinFunctionList::add(const char *name, q_func_t f, int functional_domain) {
   QoreModuleContext* qmc = get_module_context();
   if (qmc && check_dup(*qmc, name))
      return;

   // qmc will be NULL when called from program initialization and not in a module
   // for backwards compatibility we load directly into the Qore namespace of the static system namespace
   BuiltinFunctionVariant* v = new BuiltinFunctionVariant(f, QC_USES_EXTRA_ARGS, functional_domain);
   if (!qmc)
      qore_root_ns_private::getQore(*staticSystemNamespace)->addBuiltinVariantIntern(name, v);
   else
      qmc->mcfl.push_back(ModuleContextFunctionCommit(qmc->getRootNS()->getQore(), name, v));

   //bfl.add(name, f, functional_domain);
}

void BuiltinFunctionList::add2(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, unsigned num_params, ...) {
   QoreModuleContext* qmc = get_module_context();
   if (qmc && check_dup(*qmc, name))
      return;

   // qmc will be NULL when called from program initialization and not in a module
   // for backwards compatibility we load directly into the Qore namespace of the static system namespace

   va_list args;
   va_start(args, num_params);

   type_vec_t typeList;
   arg_vec_t defaultArgList;
   if (num_params)
      qore_process_params(num_params, typeList, defaultArgList, args);

   // qmc will be NULL when called from program initialization and not in a module
   // for backwards compatibility we load directly into the Qore namespace of the static system namespace
   BuiltinFunctionVariant* v = new BuiltinFunctionVariant(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList);
   //printd(0, "add2('%s', %p, flags=%lld, domain=%lld, ret=%s, num_params=%d, ...)\n", name, f, flags, functional_domain, returnTypeInfo->getName(), num_params);
   if (!qmc)
      qore_root_ns_private::getQore(*staticSystemNamespace)->addBuiltinVariantIntern(name, v);
   else
      qmc->mcfl.push_back(ModuleContextFunctionCommit(qmc->getRootNS()->getQore(), name, v));

   //add_intern(name, new B(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList));
   //bfl.add2(name, f, flags, functional_domain, returnTypeInfo, num_params, args);
   va_end(args);
}

void BuiltinFunctionList::add3(const char *name, q_func_t f, int64 flags, int64 functional_domain, const QoreTypeInfo *returnTypeInfo, const type_vec_t &typeList, const arg_vec_t &defaultArgList) {
   QoreModuleContext* qmc = get_module_context();
   if (qmc && check_dup(*qmc, name))
      return;

   // qmc will be NULL when called from program initialization and not in a module
   // for backwards compatibility we load directly into the Qore namespace of the static system namespace
   BuiltinFunctionVariant* v = new BuiltinFunctionVariant(f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList);
   //printd(0, "add2('%s', %p, flags=%lld, domain=%lld, ret=%s, num_params=%d, ...)\n", name, f, flags, functional_domain, returnTypeInfo->getName(), num_params);
   if (!qmc)
      qore_root_ns_private::getQore(*staticSystemNamespace)->addBuiltinVariantIntern(name, v);
   else
      qmc->mcfl.push_back(ModuleContextFunctionCommit(qmc->getRootNS()->getQore(), name, v));

   //bfl.add3(name, f, flags, functional_domain, returnTypeInfo, typeList, defaultArgList);
}
