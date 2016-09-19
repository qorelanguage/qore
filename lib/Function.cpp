/* -*- indent-tabs-mode: nil -*- */
/*
  Function.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2016 Qore Technologies, s.r.o.

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
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/qore_list_private.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <cmath>

// FIXME: xxx set parse location
static void duplicateSignatureException(const char* cname, const char* name, const AbstractFunctionSignature* sig) {
   parseException("DUPLICATE-SIGNATURE", "%s%s%s(%s) has already been declared", cname ? cname : "", cname ? "::" : "", name, sig->getSignatureText());
}

// FIXME: xxx set parse location
static void ambiguousDuplicateSignatureException(const char* cname, const char* name, const AbstractFunctionSignature* sig1, const AbstractFunctionSignature* sig2) {
   parseException("DUPLICATE-SIGNATURE", "%s%s%s(%s) matches already declared variant %s(%s)", cname ? cname : "", cname ? "::" : "", name, sig2->getSignatureText(), name, sig1->getSignatureText());
}

QoreFunction* IList::getFunction(const qore_class_private* class_ctx, const qore_class_private*& last_class, const_iterator aqfi, bool& internal_access, bool& stop) const {
   stop = internal_access && (*aqfi).access == Internal;

   QoreFunction* rv = (!last_class || ((*aqfi).access == Public) || stop
                       || (class_ctx && (*aqfi).access == Private)) ? (*aqfi).func : 0;

   if (rv) {
      const QoreClass* fc = rv->getClass();
      if (fc) {
         // get the function's class
         last_class = qore_class_private::get(*fc);
         if (last_class && class_ctx) {
            // set the internal access flag
            internal_access = class_ctx && last_class->is_equal(*class_ctx);
         }
      }
   }

   return rv;
}

bool AbstractFunctionSignature::operator==(const AbstractFunctionSignature& sig) const {
   if (num_param_types != sig.num_param_types || min_param_types != sig.min_param_types) {
      //printd(5, "AbstractFunctionSignature::operator==() pt: %d != %d || mpt %d != %d\n", num_param_types, sig.num_param_types, min_param_types, sig.min_param_types);
      return false;
   }

   if (!sig.returnTypeInfo->isOutputCompatible(returnTypeInfo)) {
      //printd(5, "AbstractFunctionSignature::operator==() rt: %s is not compatible with %s (%p %p)\n", returnTypeInfo->getName(), sig.returnTypeInfo->getName(), returnTypeInfo, sig.returnTypeInfo);
      return false;
   }

   for (unsigned i = 0; i < typeList.size(); ++i) {
      const QoreTypeInfo* ti = sig.typeList.size() <= i ? 0 : sig.typeList[i];
      if (!typeList[i]->isInputIdentical(ti)) {
         //printd(5, "AbstractFunctionSignature::operator==() param %d %s != %s\n", i, typeList[i]->getName(), sig.typeList[i]->getName());
         return false;
      }
   }

   //printd(5, "AbstractFunctionSignature::operator==() '%s' == '%s' TRUE\n", str.c_str(), sig.str.c_str());
   return true;
}

void AbstractQoreFunctionVariant::parseResolveUserSignature() {
   UserVariantBase* uvb = getUserVariantBase();
   if (uvb)
      uvb->getUserSignature()->resolve();
}

bool AbstractQoreFunctionVariant::hasBody() const {
   return is_user ? getUserVariantBase()->hasBody() : true;
}

static void do_call_name(QoreString &desc, const QoreFunction* func) {
   const char* class_name = func->className();
   if (class_name)
      desc.sprintf("%s::", class_name);
   desc.sprintf("%s(", func->getName());
}

static void add_args(QoreStringNode &desc, const QoreValueList* args) {
   if (!args || !args->size())
      return;

   for (unsigned i = 0; i < args->size(); ++i) {
      const QoreValue n = args->retrieveEntry(i);
      if (n.isNothing())
         desc.concat("NOTHING");
      else {
         qore_type_t t = n.getType();
         if (t == NT_OBJECT)
            desc.concat(n.get<const QoreObject>()->getClassName());
         else
            desc.concat(n.getTypeName());
      }
      if (i != (args->size() - 1))
         desc.concat(", ");
   }
}

CodeEvaluationHelper::CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func, const AbstractQoreFunctionVariant*& variant, const char* n_name, const QoreListNode* args, QoreObject* self, const qore_class_private* n_qc, qore_call_t n_ct, bool is_copy)
   : ct(n_ct), name(n_name), xsink(n_xsink), qc(n_qc), loc(RunTimeLocation), tmp(n_xsink), returnTypeInfo((const QoreTypeInfo* )-1), pgm(getProgram()), rtflags(0) {
   tmp.assignEval(args);

   if (*xsink)
      return;

   bool check_args = variant;
   if (!variant) {
      const qore_class_private* class_ctx = qc ? runtime_get_class() : 0;
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*qc->cls, class_ctx))
         class_ctx = 0;

      variant = func->runtimeFindVariant(getArgs(), false, class_ctx, xsink);
      if (!variant) {
         assert(*xsink);
         return;
      }

      // check for accessible variants
      if (qc) {
         const MethodVariant* mv = reinterpret_cast<const MethodVariant*>(variant);
         ClassAccess va = mv->getAccess();
         if ((va > Public && !class_ctx) || (va == Internal && mv->getClass() != qc->cls)) {
            xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s(%s) is not accessible in this context", mv->className(), func->getName(), mv->getSignature()->getSignatureText());
            return;
         }
      }
   }

   if (processDefaultArgs(func, variant, check_args, is_copy))
      return;

   setCallType(variant->getCallType());
   setReturnTypeInfo(variant->getReturnTypeInfo());
}

CodeEvaluationHelper::CodeEvaluationHelper(ExceptionSink* n_xsink, const QoreFunction* func, const AbstractQoreFunctionVariant*& variant, const char* n_name, const QoreValueList* args, QoreObject* self, const qore_class_private* n_qc, qore_call_t n_ct, bool is_copy)
   : ct(n_ct), name(n_name), xsink(n_xsink), qc(n_qc), loc(RunTimeLocation), tmp(n_xsink), returnTypeInfo((const QoreTypeInfo* )-1), pgm(getProgram()), rtflags(0) {
   tmp.assignEval(args);

   if (*xsink)
      return;

   bool check_args = variant;
   if (!variant) {
      const qore_class_private* class_ctx = qc ? runtime_get_class() : 0;
      if (class_ctx && !qore_class_private::runtimeCheckPrivateClassAccess(*qc->cls, class_ctx))
         class_ctx = 0;

      variant = func->runtimeFindVariant(getArgs(), false, class_ctx, xsink);
      if (!variant) {
         assert(*xsink);
         return;
      }

      // check for accessible variants
      if (qc) {
         const MethodVariant* mv = reinterpret_cast<const MethodVariant*>(variant);
         ClassAccess va = mv->getAccess();
         if ((va > Public && !class_ctx) || (va == Internal && mv->getClass() != qc->cls)) {
            xsink->raiseException("METHOD-IS-PRIVATE", "%s::%s(%s) is not accessible in this context", mv->className(), func->getName(), mv->getSignature()->getSignatureText());
            return;
         }
      }
   }

   if (processDefaultArgs(func, variant, check_args, is_copy))
      return;

   setCallType(variant->getCallType());
   setReturnTypeInfo(variant->getReturnTypeInfo());
}

CodeEvaluationHelper::~CodeEvaluationHelper() {
   if (returnTypeInfo != (const QoreTypeInfo*)-1)
      saveReturnTypeInfo(returnTypeInfo);
   if (ct != CT_UNUSED && xsink->isException())
      qore_es_private::addStackInfo(*xsink, ct, qc ? qc->name.c_str() : 0, name, loc);
}

int CodeEvaluationHelper::processDefaultArgs(const QoreFunction* func, const AbstractQoreFunctionVariant* variant, bool check_args, bool is_copy) {
   // get default argument list of variant
   AbstractFunctionSignature* sig = variant->getSignature();
   const arg_vec_t& defaultArgList = sig->getDefaultArgList();
   const type_vec_t& typeList = sig->getTypeList();

   unsigned max = QORE_MAX(defaultArgList.size(), typeList.size());
   for (unsigned i = 0; i < max; ++i) {
      if (i < defaultArgList.size() && defaultArgList[i] && (!tmp || tmp->retrieveEntry(i).isNothing())) {
         QoreValue& p = tmp.getEntryReference(i);
         p = defaultArgList[i]->evalValue(xsink);
         if (*xsink)
            return -1;

         // process default argument with accepting type's filter if necessary
         const QoreTypeInfo* paramTypeInfo = sig->getParamTypeInfo(i);
         if (paramTypeInfo->mayRequireFilter(p)) {
            paramTypeInfo->acceptInputParam(i, sig->getName(i), p, xsink);
            if (*xsink)
               return -1;
         }
      }
      else if (i < typeList.size()) {
         QoreValue n;
         if (tmp)
            n = tmp->retrieveEntry(i);

         if (is_copy && !i && n.isNothing())
            continue;

         const QoreTypeInfo* paramTypeInfo = sig->getParamTypeInfo(i);
         if (!paramTypeInfo)
            continue;

         // test for change or incompatibility
         if (check_args || paramTypeInfo->mayRequireFilter(n)) {
            QoreValue& p = tmp.getEntryReference(i);
            paramTypeInfo->acceptInputParam(i, sig->getName(i), p, xsink);
            if (*xsink)
               return -1;
         }
      }
   }

   // check for excess args exception
   unsigned nargs = tmp.size();
   if (!nargs)
      return 0;
   unsigned nparams = sig->numParams();

   //printd(5, "processDefaultArgs() %s nargs: %d nparams: %d flags: %lld po: %d\n", func->getName(), nargs, nparams, variant->getFlags(), (bool)(getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)));
   //if (nargs > nparams && (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS))) {
   if (nargs > nparams) {
      // use the target program (if different than the current pgm) to check for argument errors
      const UserVariantBase* uvb = variant->getUserVariantBase();
      int64 po;
      if (uvb)
         po = uvb->pgm->getParseOptions64();
      else
         po = runtime_get_parse_options();

      if (po & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
         int64 flags = variant->getFlags();

         if (!(flags & QC_USES_EXTRA_ARGS)) {
            for (unsigned i = nparams; i < nargs; ++i) {
               //printd(5, "processDefaultArgs() %s arg %d nothing: %d\n", func->getName(), i, is_nothing(tmp->retrieve_entry(i)));
               if (!tmp->retrieveEntry(i).isNothing()) {
                  QoreStringNode* desc = new QoreStringNode("call to ");
                  do_call_name(*desc, func);
                  if (nparams)
                     desc->concat(sig->getSignatureText());
                  desc->concat(") made as ");
                  do_call_name(*desc, func);
                  add_args(*desc, *tmp);
                  unsigned diff = nargs - nparams;
                  desc->sprintf(") with %d excess argument%s, which is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set", diff, diff == 1 ? "" : "s");
                  xsink->raiseException("CALL-WITH-TYPE-ERRORS", desc);
                  return -1;
               }
            }
         }
      }
   }

   return 0;
}

void AbstractFunctionSignature::addDefaultArgument(const AbstractQoreNode* arg) {
   assert(arg);
   str.append(" = ");
   qore_type_t t = arg->getType();
   if (t == NT_BAREWORD) {
      str.append(reinterpret_cast<const BarewordNode*>(arg)->str);
      return;
   }
   if (t == NT_CONSTANT) {
      str.append(reinterpret_cast<const ScopedRefNode*>(arg)->scoped_ref->getIdentifier());
      return;
   }
   if (arg->is_value()) {
      QoreNodeAsStringHelper sh(arg, FMT_NONE, 0);
      str.append(sh->getBuffer());
      return;
   }
   str.append("<exp>");
}

UserSignature::UserSignature(int first_line, int last_line, AbstractQoreNode* params, RetTypeInfo* retTypeInfo, int64 po) :
   AbstractFunctionSignature(retTypeInfo ? retTypeInfo->getTypeInfo() : 0),
   parseReturnTypeInfo(retTypeInfo ? retTypeInfo->takeParseTypeInfo() : 0),
   loc(first_line, last_line),
   lv(0), argvid(0), selfid(0), resolved(false) {

   bool needs_types = (bool)(po & (PO_REQUIRE_TYPES | PO_REQUIRE_PROTOTYPES));
   bool bare_refs = (bool)(po & PO_ALLOW_BARE_REFS);

   // assign no return type if return type declaration is missing and PO_REQUIRE_TYPES or PO_REQUIRE_PROTOTYPES is set
   if (!retTypeInfo && needs_types)
      returnTypeInfo = nothingTypeInfo;
   delete retTypeInfo;

   if (!params) {
      str = NO_TYPE_INFO;
      return;
   }

   ReferenceHolder<AbstractQoreNode> param_holder(params, 0);

   if (params->getType() == NT_VARREF) {
      pushParam(reinterpret_cast<VarRefNode*>(params), 0, needs_types);
      return;
   }

   if (params->getType() == NT_BAREWORD) {
      pushParam(reinterpret_cast<BarewordNode*>(params), needs_types, bare_refs);
      return;
   }

   if (params->getType() == NT_OPERATOR) {
      pushParam(reinterpret_cast<QoreOperatorNode*>(params), needs_types);
      return;
   }

   if (params->getType() != NT_LIST) {
      param_error();
      return;
   }

   QoreListNode* l = reinterpret_cast<QoreListNode*>(params);

   parseTypeList.reserve(l->size());
   typeList.reserve(l->size());
   defaultArgList.reserve(l->size());

   ListIterator li(l);
   while (li.next()) {
      AbstractQoreNode* n = li.getValue();
      qore_type_t t = n ? n->getType() : 0;
      if (t == NT_OPERATOR)
         pushParam(reinterpret_cast<QoreOperatorNode*>(n), needs_types);
      else if (t == NT_BAREWORD)
         pushParam(reinterpret_cast<BarewordNode*>(n), needs_types, bare_refs);
      else if (t == NT_VARREF)
         pushParam(reinterpret_cast<VarRefNode*>(n), 0, needs_types);
      else {
         if (n)
            param_error();
         break;
      }

      // add a comma to the signature string if it's not the last parameter
      if (!li.last())
         str.append(", ");
   }
}

void UserSignature::pushParam(QoreOperatorNode* t, bool needs_types) {
   QoreAssignmentOperatorNode* op = dynamic_cast<QoreAssignmentOperatorNode*>(t);
   if (!op) {
      parse_error("invalid expression with the '%s' operator in parameter list; only simple assignments to default values are allowed", t->getTypeName());
      return;
   }

   AbstractQoreNode* l = op->getLeft();
   if (l && l->getType() != NT_VARREF) {
      param_error();
      return;
   }
   VarRefNode* v = reinterpret_cast<VarRefNode*>(l);
   AbstractQoreNode* defArg = op->swapRight(0);
   pushParam(v, defArg, needs_types);
}

void UserSignature::pushParam(BarewordNode* b, bool needs_types, bool bare_refs) {
   names.push_back(b->str);
   parseTypeList.push_back(0);
   typeList.push_back(0);
   str.append(NO_TYPE_INFO);
   str.append(" ");
   str.append(b->str);
   defaultArgList.push_back(0);

   if (needs_types)
      parse_error(loc, "parameter '%s' declared without type information, but parse options require all declarations to have type information", b->str);

   //if (!(getProgram()->getParseOptions64() & PO_ALLOW_BARE_REFS))
   if (!bare_refs)
      parse_error("parameter '%s' declared without '$' prefix, but parse option 'allow-bare-defs' is not set", b->str);
   return;
}

void UserSignature::pushParam(VarRefNode* v, AbstractQoreNode* defArg, bool needs_types) {
   // check for duplicate name
   for (name_vec_t::iterator i = names.begin(), e = names.end(); i != e; ++i)
      if (*i == v->getName())
         parse_error(loc, "duplicate variable '%s' declared in parameter list", (*i).c_str());

   names.push_back(v->getName());

   bool is_decl = v->isDecl();
   if (needs_types && !is_decl)
      parse_error(loc, "parameter '%s' declared without type information, but parse options require all declarations to have type information", v->getName());

   // see if this is a new object call
   if (v->has_effect()) {
      // here we make 4 virtual function calls when 2 would be enough, but no need to optimize for speed for an exception
      parse_error(loc, "parameter '%s' may not be declared with new object syntax; instead use: '%s %s = new %s()'", v->getName(), v->getNewObjectClassName(), v->getName(), v->getNewObjectClassName());
   }

   if (is_decl) {
      VarRefDeclNode* vd = reinterpret_cast<VarRefDeclNode*>(v);
      QoreParseTypeInfo* pti = vd->takeParseTypeInfo();
      parseTypeList.push_back(pti);
      const QoreTypeInfo* ti = vd->getTypeInfo();
      typeList.push_back(ti);

      assert(!(pti && ti));

      if (pti || ti->hasType()) {
         ++num_param_types;
         // only increment min_param_types if there is no default argument
         if (!defArg)
            ++min_param_types;
      }

      // add type name to signature
      if (pti)
         pti->concatName(str);
      else
         ti->concatName(str);
   }
   else {
      parseTypeList.push_back(0);
      typeList.push_back(0);
      str.append(NO_TYPE_INFO);
   }

   str.append(" ");
   str.append(v->getName());

   defaultArgList.push_back(defArg);
   if (defArg)
      addDefaultArgument(defArg);

   if (v->explicitScope()) {
      if (v->getType() == VT_LOCAL)
         parse_error(loc, "invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
      else if (v->getType() == VT_GLOBAL)
         parse_error(loc, "invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
   }
}

void UserSignature::parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo) {
   lv.reserve(parseTypeList.size());

   if (selfid)
      push_local_var(selfid, loc);
   else if (classTypeInfo)
      selfid = push_local_var("self", loc, classTypeInfo, false, 1);

   // push $argv var on stack and save id
   argvid = push_local_var("argv", loc, listOrNothingTypeInfo, false, 1);
   printd(5, "UserSignature::parseInitPushLocalVars() this: %p argvid: %p\n", this, argvid);

   resolve();

   // init param ids and push local parameter vars on stack
   for (unsigned i = 0; i < typeList.size(); ++i) {
      // check for dups but do not check if the variables are referenced in the block
      lv.push_back(push_local_var(names[i].c_str(), loc, typeList[i], true, 1));
      printd(5, "UserSignature::parseInitPushLocalVars() registered local var %s (id: %p)\n", names[i].c_str(), lv[i]);
   }
}

void UserSignature::parseInitPopLocalVars() {
   // remove local variables from stack and unset the parse_assigned flag
   for (unsigned i = 0; i < typeList.size(); ++i)
      pop_local_var(true);

   // pop $argv param off stack
   pop_local_var();

   // pop $self off stack if present
   if (selfid)
      pop_local_var();
}

void UserSignature::resolve() {
   if (resolved)
      return;

   resolved = true;

   if (!returnTypeInfo) {
      returnTypeInfo = parseReturnTypeInfo->resolveAndDelete(loc);
      parseReturnTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseReturnTypeInfo);
#endif

   for (unsigned i = 0; i < parseTypeList.size(); ++i) {
      if (parseTypeList[i]) {
         assert(!typeList[i]);
         typeList[i] = parseTypeList[i]->resolveAndDelete(loc);
      }

      // initialize default arguments
      if (defaultArgList[i]) {
         int lvids = 0;
         const QoreTypeInfo* argTypeInfo = 0;
         defaultArgList[i] = defaultArgList[i]->parseInit(selfid, 0, lvids, argTypeInfo);
         if (lvids) {
            parse_error(loc, "illegal local variable declaration in default value expression in parameter '%s'", names[i].c_str());
            while (lvids--)
               pop_local_var();
         }
         // check type compatibility
         if (!typeList[i]->parseAccepts(argTypeInfo)) {
            QoreStringNode* desc = new QoreStringNode;
            desc->sprintf("parameter '%s' expects ", names[i].c_str());
            typeList[i]->getThisType(*desc);
            desc->concat(", but the default value is ");
            argTypeInfo->getThisType(*desc);
            desc->concat(" instead");
            qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
         }
      }
   }
   parseTypeList.clear();
}

bool QoreFunction::existsVariant(const type_vec_t& paramTypeInfo) const {
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature* sig = (*i)->getSignature();
      assert(sig);
      unsigned np = sig->numParams();
      if (np != paramTypeInfo.size())
         continue;
      if (!np)
         return true;
      bool ok = true;
      for (unsigned pi = 0; pi < np; ++pi) {
         if (!paramTypeInfo[pi]->isInputIdentical(sig->getParamTypeInfo(pi))) {
            ok = false;
            break;
         }
      }
      if (ok)
         return true;
   }
   return false;
}

static QoreStringNode* getNoopError(const QoreFunction* func, const QoreFunction* aqf, const AbstractQoreFunctionVariant* variant) {
   QoreStringNode* desc = new QoreStringNode;
   do_call_name(*desc, aqf);
   desc->sprintf("%s) is a variant that returns a constant value when incorrect data types are passed to the function", variant->getSignature()->getSignatureText());
   const QoreTypeInfo* rti = variant->getReturnTypeInfo();
   if (rti->hasType() && !variant->numParams()) {
      desc->concat(" and always returns ");
      if (rti->getUniqueReturnClass() || func->className()) {
         rti->getThisType(*desc);
      }
      else {
         // get actual value and include in warning
         ExceptionSink xsink;
         CodeEvaluationHelper ceh(&xsink, func, variant, "noop-dummy");
         ValueHolder v(variant->evalFunction(func->getName(), ceh, 0), 0);
         //ReferenceHolder<AbstractQoreNode> v(variant->evalFunction(func->getName(), ceh, 0), 0);
         if (v->isNothing())
            desc->concat("NOTHING");
         else {
            QoreNodeAsStringHelper vs(*v, FMT_NONE, 0);
            desc->sprintf("the following value: %s (", vs->getBuffer());
            rti->getThisType(*desc);
            desc->concat(')');
         }
      }
   }
   return desc;
}

static bool skip_method_variant(const AbstractQoreFunctionVariant* v, const qore_class_private* class_ctx, bool internal_access) {
   assert(dynamic_cast<const MethodVariantBase*>(v));
   ClassAccess va = reinterpret_cast<const MethodVariantBase*>(v)->getAccess();
   // skip if the variant is not accessible
   return ((!class_ctx && va > Public) || (va == Internal && !internal_access));
}

// finds a variant at runtime
const AbstractQoreFunctionVariant* QoreFunction::runtimeFindVariant(const QoreValueList* args, bool only_user, const qore_class_private* class_ctx, ExceptionSink* xsink) const {
   int match = -1;
   const AbstractQoreFunctionVariant* variant = 0;

   //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s%s%s() vlist: %d (pend: %d) ilist: %d args: %p (%d)\n", this, className() ? className() : "", className() ? "::" : "", getName(), vlist.size(), pending_vlist.size(), ilist.size(), args, args ? args->size() : 0);

   // perfect match score
   unsigned nargs = args ? args->size() : 0;
   int perfect = nargs * 2;

   const QoreFunction* aqf = 0;
   AbstractFunctionSignature* sig = 0;

   // parent class while iterating
   const qore_class_private* last_class = 0;
   bool internal_access = false;

   int cnt = 0;

   // iterate through inheritance list
   for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
      bool stop;
      aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
      if (!aqf)
         continue;
      aqf = (*aqfi).func;

      //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s::%s(...) size: %d\n", this, aqf->className(), getName(), ilist.size());

      for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
         // skip checking variant if we are only looking for user variants and this variant is builtin
         if (only_user && !(*i)->isUser())
            continue;

         // skip if the variant is not accessible
         if (last_class && skip_method_variant(*i, class_ctx, internal_access))
            continue;

         ++cnt;

         sig = (*i)->getSignature();
         assert(sig);

         //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) args: %p (%d) class: %s class_ctx: %p '%s'\n", this, getName(), sig->getSignatureText(), args, args ? args->size() : 0, aqf->className() ? aqf->className() : "n/a", class_ctx, class_ctx ? class_ctx->name.c_str() : "n/a");

         if (!variant && !sig->getParamTypes()) {
            match = 0;
            variant = *i;

            if (!perfect)
               break;
            continue;
         }

         // skip variants with signatures with fewer possible elements than the best match already
         if ((int)(sig->getParamTypes() * 2) > match) {
            int count = 0;
            bool ok = true;
            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
               const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
               QoreValue n;
               if (args)
                  n = args->retrieveEntry(pi);

               //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) i: %d param: %s arg: %s\n", this, getName(), sig->getSignatureText(), pi, t->getName(), n.typeName(n));

               int rc;
               if (n.isNothing() && sig->hasDefaultArg(pi))
                  rc = QTI_IGNORE;
               else {
                  rc = t->runtimeAcceptsValue(n);
                  //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) i: %d param: %s arg: %s rc: %d\n", this, getName(), sig->getSignatureText(), pi, t->getName(), n.getTypeName(), rc);
                  if (rc == QTI_NOT_EQUAL) {
                     ok = false;
                     break;
                  }
                  // do not count default matches with non-existent arguments
                  if (!args || pi >= args->size())
                     rc = QTI_IGNORE;
               }

               // only increment for actual type matches (t may be NULL)
               if (t && rc != QTI_IGNORE)
                  count += rc;
            }
            if (!ok)
               continue;

            if (count > match) {
               match = count;
               variant = *i;

               if (match == perfect)
                  break;
            }
         }
      }
      // issue 1229: continue searching the class hierarchy for a perfect match
      if (stop)
         break;
   }
   if (!variant && !only_user) {
      QoreStringNode* desc = new QoreStringNode("no variant matching '");
      const char* class_name = className();
      if (class_name)
         desc->sprintf("%s::", class_name);
      desc->sprintf("%s(", getName());
      add_args(*desc, args);
      desc->concat(") can be found; ");
      if (!cnt) {
         desc->concat("no variants were accessible in this execution context");
      }
      else {
         desc->concat("the following variants were tested:");

         last_class = 0;
         internal_access = false;

         // add variants tested
         // iterate through inheritance list
         for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
            bool stop;
            aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
            if (!aqf)
               continue;
            class_name = aqf->className();

            for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
               // skip if the variant is not accessible
               if (last_class && skip_method_variant(*i, class_ctx, internal_access))
                  continue;
               desc->concat("\n   ");
               if (class_name)
                  desc->sprintf("%s::", class_name);
               desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
            }
            if (stop)
               break;
         }
      }
      xsink->raiseException("RUNTIME-OVERLOAD-ERROR", desc);
   }
   else if (variant) {
      QoreProgram* pgm = getProgram();

      // pgm could be zero if called from a foreign thread with no current Program
      if (pgm) {
         // check parse options
         int64 po = runtime_get_parse_options();
         int64 vflags = variant->getFunctionality();
         // check restrictive flags
         //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s() returning %p %s(%s) vflags: " QLLD " po: " QLLD " neg: " QLLD "\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", (vflags & po & ~PO_POSITIVE_OPTIONS));
         if ((vflags & po & ~PO_POSITIVE_OPTIONS) || ((vflags & PO_POSITIVE_OPTIONS) && (((vflags & PO_POSITIVE_OPTIONS) & po) != (vflags & PO_POSITIVE_OPTIONS)))) {
            //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s(%s) getProgram(): %p getProgram()->getParseOptions64(): %x variant->getFunctionality(): %x\n", this, getName(), variant->getSignature()->getSignatureText(), getProgram(), getProgram()->getParseOptions64(), variant->getFunctionality());
            if (!only_user) {
               const char* class_name = className();
               xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin %s '%s%s%s(%s)'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", getName(), variant->getSignature()->getSignatureText());
            }
            return 0;
         }

         if (po & (PO_REQUIRE_TYPES | PO_STRICT_ARGS) && variant->getFlags() & QC_RUNTIME_NOOP) {
            QoreStringNode* desc = getNoopError(this, aqf, variant);
            desc->sprintf("; this variant is not accessible when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set (%llx)", variant->getFlags());
            xsink->raiseException("CALL-WITH-TYPE-ERRORS", desc);
         }
      }
   }

   //printd(5, "QoreFunction::runtimeFindVariant() this: %p %s() returning %p %s(%s) class: %s\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant && aqf && aqf->className() ? aqf->className() : "n/a");

   return variant;
}

static AbstractQoreFunctionVariant* doSingleVariantTypeException(const QoreProgramLocation &loc, int pi, const char* class_name, const char* name, const char* sig, const QoreTypeInfo* proto, const QoreTypeInfo* arg) {
   QoreStringNode* desc = new QoreStringNode("argument ");
   desc->sprintf("%d to '", pi);
   if (class_name)
      desc->sprintf("%s::", class_name);
   desc->sprintf("%s(%s)' expects ", name, sig);
   proto->getThisType(*desc);
   desc->concat(", but call supplies ");
   arg->getThisType(*desc);
   qore_program_private::makeParseException(getProgram(), loc, "PARSE-TYPE-ERROR", desc);
   return 0;
}

static void do_call_str(QoreString &desc, const QoreFunction* func, const type_vec_t& argTypeInfo) {
   unsigned num_args = argTypeInfo.size();
   do_call_name(desc, func);
   if (num_args)
      for (unsigned i = 0; i < num_args; ++i) {
         desc.concat(argTypeInfo[i]->getName());
         if (i != (num_args - 1))
            desc.concat(", ");
      }
   desc.concat(')');
}

static void warn_excess_args(QoreFunction* func, const type_vec_t& argTypeInfo, AbstractFunctionSignature* sig) {
   unsigned nargs = argTypeInfo.size();
   unsigned nparams = sig->numParams();

   QoreStringNode* desc = new QoreStringNode("call to ");
   desc->concat(func->className() ? "method " : "function ");
   do_call_name(*desc, func);
   if (nparams)
      desc->concat(sig->getSignatureText());
   desc->concat(") made as ");
   do_call_str(*desc, func, argTypeInfo);
   unsigned diff = nargs - nparams;
   desc->sprintf(" (with %d excess argument%s)", diff, diff == 1 ? "" : "s");
   // raise warning if require-types is not set
   //if (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
   if (parse_get_parse_options() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
      desc->concat("; this is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
      qore_program_private::makeParseException(getProgram(), "CALL-WITH-TYPE-ERRORS", desc);
   }
   else {
      // raise warning
      desc->concat("; excess arguments will be ignored; to disable this warning, use '%%disable-warning excess-args' in your code");
      qore_program_private::makeParseWarning(getProgram(), QP_WARN_EXCESS_ARGS, "EXCESS-ARGS", desc);
   }
}

// finds a variant at parse time
const AbstractQoreFunctionVariant* QoreFunction::parseFindVariant(const QoreProgramLocation& loc, const type_vec_t& argTypeInfo, const qore_class_private* class_ctx) {
   // the number of parameters * 2 matched to arguments (compatible but not perfect match = 1, perfect match = 2)
   int match = -1;
   // the number of possible matches at runtime (due to missing types at parse time); number of parameters
   int pmatch = -1;
   // the number of arguments matched perfectly in case of a tie score
   int nperfect = -1;
   // number of possible variants
   unsigned npv = 0;

   // pointer to the variant matched
   const AbstractQoreFunctionVariant* variant = 0;
   // pointer to the last possible variant matched
   const AbstractQoreFunctionVariant* pvariant = 0;
   unsigned num_args = argTypeInfo.size();

   //printd(5, "QoreFunction::parseFindVariant() this: %p %s() vlist: %d pend: %d ilist: %d num_args: %d\n", this, getName(), vlist.size(), pending_vlist.size(), ilist.size(), num_args);

   QoreFunction* aqf = 0;

   // parent class while iterating
   const qore_class_private* last_class = 0;
   bool internal_access = false;

   int cnt = 0;

   // iterate through inheritance list
   for (ilist_t::iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
      bool stop;
      aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
      if (!aqf)
         continue;
      aqf = (*aqfi).func;
      //printd(5, "QoreFunction::parseFindVariant() %p %s testing function %p\n", this, getName(), aqf);
      assert(!aqf->vlist.empty() || !aqf->pending_vlist.empty());

      // check committed list
      for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
         // skip if the variant is not accessible
         if (last_class && skip_method_variant(*i, class_ctx, internal_access))
            continue;
         AbstractFunctionSignature* sig = (*i)->getSignature();

         ++cnt;

         //printd(5, "QoreFunction::parseFindVariant() this: %p checking %s(%s) variant: %p sig->pt: %d sig->mpt: %d match: %d, args: %d\n", this, getName(), sig->getSignatureText(), variant, sig->getParamTypes(), sig->getMinParamTypes(), match, num_args);

         if (!variant && !sig->getParamTypes() && pmatch == -1) {
            match = pmatch = nperfect = 0;
            variant = *i;

            if (!npv)
               pvariant = variant;
            else
               pvariant = 0;

            ++npv;

            //printd(5, "QoreFunction::parseFindVariant() this: %p matched with no args %s(%s) variant: %p sig->pt: %d sig->mpt: %d match: %d, args: %d\n", this, getName(), sig->getSignatureText(), variant, sig->getParamTypes(), sig->getMinParamTypes(), match, num_args);

            continue;
         }

         // skip variants with signatures with fewer possible elements than the best match already
         if ((int)(sig->numParams() * 2) > match) {
            int variant_pmatch = 0;
            int count = 0;
            int variant_nperfect = 0;
            bool ok = true;
            bool variant_missing_types = false;

            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
               const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
               const QoreTypeInfo* a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;

               //printd(5, "QoreFunction::parseFindVariant() %s(%s) committed pi: %d t: %s (has type: %d) a: %s (%p) t->parseAccepts(a): %d\n", getName(), sig->getSignatureText(), pi, t->getName(), t->hasType(), a->getName(), a, t->parseAccepts(a));

               int rc = QTI_UNASSIGNED;
               if (t->hasType()) {
                  if (!a->hasType()) {
                     if (pi < num_args) {
                        variant_missing_types = true;
                        count += QTI_AMBIGUOUS;
                        ++variant_pmatch;
                        continue;
                     }
                     else if (sig->hasDefaultArg(pi))
                        rc = QTI_IGNORE;
                     else
                        a = nothingTypeInfo;
                  }
                  else if (a->isType(NT_NOTHING) && sig->hasDefaultArg(pi))
                     rc = QTI_IDENT;
               }

               if (rc == QTI_UNASSIGNED) {
                  bool may_not_match = false;
                  rc = t->parseAccepts(a, may_not_match);
                  //printd(5, "QoreFunction::parseFindVariant() %s(%s) rc: %d may_not_match: %d\n", getName(), sig->getSignatureText(), rc, may_not_match);
                  if (may_not_match && !variant_missing_types)
                     variant_missing_types = true;
                  if (rc == QTI_IDENT)
                     ++variant_nperfect;
               }

               if (rc == QTI_NOT_EQUAL) {
                  ok = false;
                  // raise a detailed parse exception immediately if there is only one variant
                  if (ilist.size() == 1 && aqf->pending_vlist.singular() && aqf->vlist.empty() && getProgram()->getParseExceptionSink())
                     return doSingleVariantTypeException(loc, pi + 1, aqf->className(), getName(), sig->getSignatureText(), t, a);
                  break;
               }
               // only increment for actual type matches (t may be NULL)
               //if (t) {
                  ++variant_pmatch;
                  if (rc != QTI_IGNORE)
                     count += rc;
                  //}
            }

            //printd(5, "QoreFunction::parseFindVariant() this: %p tested %s(%s) ok: %d count: %d match: %d variant_missing_types: %d variant_pmatch: %d variant_nperfect: %d nperfect: %d\n", this, getName(), sig->getSignatureText(), ok, count, match, variant_missing_types, variant_pmatch, variant_nperfect, nperfect);
            if (!ok)
               continue;

            // now check if additional args are present that could be NOTHING and count as partial matches xxx
            for (unsigned pi = sig->numParams(); pi < num_args; ++pi) {
               const QoreTypeInfo* a = argTypeInfo[pi];
               if (a->parseAcceptsReturns(NT_NOTHING)) {
                  ++variant_pmatch;
                  count += QTI_AMBIGUOUS;
               }
            }

            if (!npv)
               pvariant = variant;
            else
               pvariant = 0;

            ++npv;

            //if (count >= match && variant_nperfect > nperfect) {
            if (count > match || (count == match && variant_nperfect > nperfect)) {
               // if we could possibly match less than another variant
               // then we have to match at runtime
               if (variant_pmatch < pmatch)
                  variant = 0;
               else {
                  // only set variant if it's the longest absolute match and the
                  // longest potential match
                  pmatch = variant_pmatch;
                  match = count;
                  nperfect = variant_nperfect;
                  if (!variant_missing_types ) {
                     //printd(5, "QoreFunction::parseFindVariant() assigning variant %p %s(%s)\n", *i, getName(), sig->getSignatureText());
                     variant = *i;
                  }
                  else
                     variant = 0;
               }
            }
            else if (variant_pmatch >= pmatch) {
               // if we could possibly match less than another variant
               // then we have to match at runtime
               variant = 0;
               pmatch = variant_pmatch;
            }
         }
      }

      // check pending list
      for (vlist_t::iterator i = aqf->pending_vlist.begin(), e = aqf->pending_vlist.end(); i != e; ++i) {
         // skip if the variant is not accessible
         if (last_class && skip_method_variant(*i, class_ctx, internal_access))
            continue;

         ++cnt;

         UserVariantBase *uvb = (*i)->getUserVariantBase();
         UserSignature* sig = uvb->getUserSignature();
         // resolve types in signature if necessary
         sig->resolve();

         if (!variant && !sig->getParamTypes() && pmatch == -1) {
            match = pmatch = nperfect = 0;
            variant = *i;

            if (!npv)
               pvariant = variant;
            else
               pvariant = 0;

            ++npv;

            continue;
         }

         // skip variants with signatures with fewer possible elements than the best match already
         if ((int)(sig->numParams() * 2) > match) {
            int variant_pmatch = 0;
            int count = 0;
            int variant_nperfect = 0;
            bool ok = true;
            bool variant_missing_types = false;

            for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
               const QoreTypeInfo* t = sig->getParamTypeInfo(pi);
               const QoreTypeInfo* a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;

               //printd(5, "QoreFunction::parseFindVariant() %s(%s) uncommitted pi: %d t: %s (has type: %d) a: %s (%p) t->parseAccepts(a): %d\n", getName(), sig->getSignatureText(), pi, t->getName(), t->hasType(), a->getName(), a, t->parseAccepts(a));

               int rc = QTI_UNASSIGNED;
               if (t->hasType()) {
                  if (!a->hasType()) {
                     if (pi < num_args) {
                        variant_missing_types = true;
                        count += QTI_AMBIGUOUS;
                        ++variant_pmatch;
                        continue;
                     }
                     else if (sig->hasDefaultArg(pi))
                        rc = QTI_IGNORE;
                     else
                        a = nothingTypeInfo;
                  }
                  else if (a->isType(NT_NOTHING) && sig->hasDefaultArg(pi))
                     rc = QTI_IDENT;
               }

               if (rc == QTI_UNASSIGNED) {
                  bool may_not_match = false;
                  rc = t->parseAccepts(a, may_not_match);
                  if (may_not_match && !variant_missing_types)
                     variant_missing_types = true;
                  if (rc == QTI_IDENT)
                     ++variant_nperfect;
               }

               if (rc == QTI_NOT_EQUAL) {
                  ok = false;
                  // raise a detailed parse exception immediately if there is only one variant
                  if (ilist.size() == 1 && aqf->pending_vlist.singular() && aqf->vlist.empty() && getProgram()->getParseExceptionSink())
                     return doSingleVariantTypeException(loc, pi + 1, aqf->className(), getName(), sig->getSignatureText(), t, a);
                  break;
               }
               // only increment for actual type matches (t may be NULL)
               //if (t) {
                  ++variant_pmatch;
                  //printd(5, "QoreFunction::parseFindVariant() this: %p %s() variant: %p i: %d match (param %s == %s)\n", this, getName(), variant, pi, t->getName(), a->getName());
                  if (rc != QTI_IGNORE)
                     count += rc;
                  //}
            }
            if (!ok)
               continue;

            // now check if additional args are present that could be NOTHING and cound as partial matches xxx
            for (unsigned pi = sig->numParams(); pi < num_args; ++pi) {
               const QoreTypeInfo* a = argTypeInfo[pi];
               if (a->parseAcceptsReturns(NT_NOTHING)) {
                  ++variant_pmatch;
                  count += QTI_AMBIGUOUS;
               }
            }

            if (!npv)
               pvariant = variant;
            else
               pvariant = 0;

            ++npv;

            //if (count >= match && variant_nperfect > nperfect) {
            if (count > match || (count == match && variant_nperfect > nperfect)) {
               // if we could possibly match less than another variant
               // then we have to match at runtime
               if (variant_pmatch < pmatch)
                  variant = 0;
               else {
                  // only set variant if it's the longest absolute match and the
                  // longest potential match
                  pmatch = variant_pmatch;
                  match = count;
                  nperfect = variant_nperfect;
                  if (!variant_missing_types)
                     variant = *i;
                  else
                     variant = 0;
               }
            }
            else if (variant_pmatch >= pmatch) {
               // if we could possibly match less than another variant
               // then we have to match at runtime
               variant = 0;
               pmatch = variant_pmatch;
            }
         }
      }
      if (stop)
         break;
   }

   // if we only have one possible variant, then assign it, even it it's not a guaranteed match
   if (!variant && pvariant)
      variant = pvariant;
   else if (!variant && pmatch == -1 && getProgram()->getParseExceptionSink()) {
      QoreStringNode* desc = new QoreStringNode("no variant matching '");
      do_call_str(*desc, this, argTypeInfo);
      desc->concat(" can be found; ");
      if (!cnt) {
         desc->concat("no variants were accessible in this context");
      }
      else {
         desc->concat("the following variants were tested:");

         last_class = 0;
         internal_access = false;

         // add variants tested
         // iterate through inheritance list
         for (ilist_t::iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
            aqf = (*aqfi).func;
            bool stop;
            aqf = ilist.getFunction(class_ctx, last_class, aqfi, internal_access, stop);
            if (!aqf)
               continue;
            const char* class_name = aqf->className();

            for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
               // skip if the variant is not accessible
               //if (last_class && skip_method_variant(*i, class_ctx, internal_access))
               //   continue;
               desc->concat("\n   ");
               if (class_name)
                  desc->sprintf("%s::", class_name);
               desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
            }
            for (vlist_t::const_iterator i = aqf->pending_vlist.begin(), e = aqf->pending_vlist.end(); i != e; ++i) {
               // skip if the variant is not accessible
               //if (last_class && skip_method_variant(*i, class_ctx, internal_access))
               //   continue;
               desc->concat("\n   ");
               if (class_name)
                  desc->sprintf("%s::", class_name);
               desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
            }
            if (stop)
               break;
         }
      }
      qore_program_private::makeParseException(getProgram(), "PARSE-TYPE-ERROR", desc);
   }
   else if (variant) {
      int64 flags = variant->getFlags();
      if (flags & (QC_NOOP | QC_RUNTIME_NOOP)) {
         QoreStringNode* desc = getNoopError(this, aqf, variant);
         //if ((flags & QC_RUNTIME_NOOP) && (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES|PO_STRICT_ARGS))) {
         if ((flags & QC_RUNTIME_NOOP) && (parse_get_parse_options() & (PO_REQUIRE_TYPES|PO_STRICT_ARGS))) {
            desc->concat("; this variant is not accessible when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
            qore_program_private::makeParseException(getProgram(), "CALL-WITH-TYPE-ERRORS", desc);
         }
         else {
            desc->concat("; to disable this warning, use '%disable-warning invalid-operation' in your code");
            qore_program_private::makeParseWarning(getProgram(), QP_WARN_CALL_WITH_TYPE_ERRORS, "CALL-WITH-TYPE-ERRORS", desc);
         }
      }

      AbstractFunctionSignature* sig = variant->getSignature();
      if (!(flags & QC_USES_EXTRA_ARGS) && num_args > sig->numParams())
         warn_excess_args(this, argTypeInfo, sig);
   }

   //printd(5, "QoreFunction::parseFindVariant() this: %p %s%s%s() returning %p %s(%s) flags: %lld\n", this, className() ? className() : "", className() ? "::" : "", getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant ? variant->getFlags() : 0ll);
   return variant;
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL, then it is identified at run time
QoreValue QoreFunction::evalFunction(const AbstractQoreFunctionVariant* variant, const QoreListNode* args, QoreProgram *pgm, ExceptionSink* xsink) const {
   const char* fname = getName();
   CodeEvaluationHelper ceh(xsink, this, variant, fname, args);
   if (*xsink) return QoreValue();

   ProgramThreadCountContextHelper tch(xsink, pgm, true);
   if (*xsink) return QoreValue();
   return variant->evalFunction(fname, ceh, xsink);
}

// finds a variant and checks variant capabilities against current
// program parse options
QoreValue QoreFunction::evalDynamic(const QoreListNode* args, ExceptionSink* xsink) const {
   const char* fname = getName();
   const AbstractQoreFunctionVariant* variant = 0;
   CodeEvaluationHelper ceh(xsink, this, variant, fname, args);
   if (*xsink) return QoreValue();

   return variant->evalFunction(fname, ceh, xsink);
}

void QoreFunction::addBuiltinVariant(AbstractQoreFunctionVariant* variant) {
   assert(variant->getCallType() == CT_BUILTIN);
#ifdef DEBUG
   // FIXME: this algorithm is no longer valid due to default arguments
   // does not detect ambiguous signatures
   AbstractFunctionSignature* sig = variant->getSignature();
   // check for duplicate parameter signatures
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature* vs = (*i)->getSignature();
      unsigned tp = vs->numParams();
      if (tp != sig->numParams())
         continue;
      if (!tp) {
         printd(0, "BuiltinFunctionBase::addBuiltinVariant() this: %p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
         assert(false);
      }
      bool ok = false;
      for (unsigned pi = 0; pi < tp; ++pi) {
         if (vs->getParamTypeInfo(pi) != sig->getParamTypeInfo(pi)) {
            ok = true;
            break;
         }
      }
      if (!ok) {
         printd(0, "BuiltinFunctionBase::addBuiltinVariant() this: %p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
         assert(false);
      }
   }
#endif
   if (!has_builtin)
      has_builtin = true;
   addVariant(variant);
}

UserVariantExecHelper::~UserVariantExecHelper() {
   if (!uvb)
      return;
   UserSignature* sig = uvb->getUserSignature();
   // uninstantiate local vars from param list
   for (unsigned i = 0; i < sig->numParams(); ++i) {
      //printd(5, "UserVariantExecHelper::~UserVariantExecHelper() this: %p %s %d/%d %p lv: %s (%s)\n", this, sig->getSignatureText(), i, sig->numParams(), sig->lv[i], sig->lv[i]->getName(), sig->lv[i]->getValueTypeName());
      sig->lv[i]->uninstantiate(xsink);
   }
}

UserVariantBase::UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode* params, RetTypeInfo* rv, bool synced)
   : signature(n_sig_first_line, n_sig_last_line, params, rv, b ? b->pwo.parse_options : parse_get_parse_options()), statements(b), gate(synced ? new VRMutex() : 0),
     pgm(getProgram()), recheck(false), init(false) {
   //printd(5, "UserVariantBase::UserVariantBase() this: %p params: %p rv: %p b: %p synced: %d\n", params, rv, b, synced);
}

UserVariantBase::~UserVariantBase() {
   delete gate;
   delete statements;
}

void UserVariantBase::parseInitPushLocalVars(const QoreTypeInfo* classTypeInfo) {
   signature.parseInitPushLocalVars(classTypeInfo);
}

void UserVariantBase::parseInitPopLocalVars() {
   signature.parseInitPopLocalVars();
}

// instantiates arguments and sets up the argv variable
int UserVariantBase::setupCall(CodeEvaluationHelper *ceh, ReferenceHolder<QoreListNode> &argv, ExceptionSink* xsink) const {
   const QoreValueList* args = ceh ? ceh->getArgs() : 0;
   unsigned num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   unsigned num_params = signature.numParams();

   for (unsigned i = 0; i < num_params; ++i) {
      QoreValue np = args ? const_cast<QoreValueList*>(args)->retrieveEntry(i) : 0;
      //AbstractQoreNode* np = args ? const_cast<AbstractQoreNode*>(args->retrieve_entry(i)) : 0;
      //printd(5, "UserVariantBase::setupCall() eval %d: instantiating param lvar %p ('%s') (exp nt: %d %p '%s')\n", i, signature.lv[i], signature.lv[i]->getName(), get_node_type(np), np, get_type_name(np));
      signature.lv[i]->instantiate(np.refSelf());

      // the above if block will only instantiate the local variable if no
      // exceptions have occured. therefore here we cleanup the rest
      // of any already instantiated local variables if an exception does occur
      if (*xsink) {
         while (i) signature.lv[--i]->uninstantiate(xsink);
         return -1;
      }
   }

   // if there are more arguments than parameters
   printd(5, "UserVariantBase::setupCall() params: %d args: %d\n", num_params, num_args);

   if (num_params < num_args) {
      argv = new QoreListNode;

      for (unsigned i = 0; i < (num_args - num_params); i++) {
         // here we try to take the reference from args if possible
         QoreValue n = args ? const_cast<QoreValueList*>(args)->retrieveEntry(i + num_params) : 0;
         //AbstractQoreNode* n = args ? const_cast<AbstractQoreNode*>(args->get_referenced_entry(i + num_params)) : 0;
         argv->push(n.getReferencedValue());
      }
   }

   return 0;
}

QoreValue UserVariantBase::evalIntern(ReferenceHolder<QoreListNode> &argv, QoreObject *self, ExceptionSink* xsink) const {
   QoreValue val;
   if (statements) {
      // self might be 0 if instantiated by a constructor call
      if (self && signature.selfid)
         signature.selfid->instantiateSelf(self);

      // instantiate argv and push id on stack
      signature.argvid->instantiate(argv ? argv->refSelf() : 0);

      {
         ArgvContextHelper argv_helper(argv.release(), xsink);

         // enter gate if necessary
         if (!gate || (gate->enter(xsink) >= 0)) {
            // execute function
            val = statements->exec(xsink);

            // exit gate if necessary
            if (gate)
               gate->exit();
         }
      }

      // uninstantiate argv
      signature.argvid->uninstantiate(xsink);

      // if self then uninstantiate
      // self might be 0 if instantiated by a constructor call
      if (self && signature.selfid)
         signature.selfid->uninstantiateSelf();
   }
   else {
      argv = 0; // dereference argv now
   }

   // if return value is NOTHING; make sure it's valid; maybe there wasn't a return statement
   // only check if there isn't an active exception
   if (!*xsink && val.isNothing()) {
      const QoreTypeInfo* rt = signature.getReturnTypeInfo();

      // check return type
      rt->acceptAssignment("<block return>", val, xsink);
   }

   return val;
}

// primary function for executing user code
QoreValue UserVariantBase::eval(const char* name, CodeEvaluationHelper* ceh, QoreObject *self, ExceptionSink* xsink, const qore_class_private* qc) const {
   QORE_TRACE("UserVariantBase::eval()");
   //printd(5, "UserVariantBase::eval() this: %p '%s()' args: %p (size: %d) self: %p class: %p '%s'\n", this, name, ceh ? ceh->getArgs() : 0, ceh && ceh->getArgs() ? ceh->getArgs()->size() : 0, self, qc, qc ? qc->name.c_str() : "n/a");

   assert(!self || (ceh ? ceh->getClass() : qc));

   // if pgm is 0 or == the current pgm, then ProgramThreadCountContextHelper does nothing
   ProgramThreadCountContextHelper tch(xsink, pgm, true);
   if (*xsink) return QoreValue();

   UserVariantExecHelper uveh(this, ceh, xsink);
   if (!uveh)
      return QoreValue();

   CodeContextHelper cch(xsink, CT_USER, name, self, ceh ? ceh->getClass() : qc);

   return evalIntern(uveh.getArgv(), self, xsink);
}

int QoreFunction::parseCheckDuplicateSignatureCommitted(AbstractFunctionSignature* sig) {
   const AbstractFunctionSignature* vs = 0;
   int rc = parseCompareResolvedSignature(vlist, sig, vs);
   if (rc == QTI_NOT_EQUAL)
      return 0;

   if (rc == QTI_AMBIGUOUS)
      ambiguousDuplicateSignatureException(className(), getName(), vs, sig);
   else
      duplicateSignatureException(className(), getName(), sig);
   return -1;
}

// returns 0 for OK, -1 for error
// this is called after types have been resolved and the types must be rechecked
int QoreFunction::parseCompareResolvedSignature(const VList& vlist, const AbstractFunctionSignature* sig, const AbstractFunctionSignature*& vs) {
   unsigned vp = sig->getParamTypes();

   // now check already-committed variants
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      vs = (*i)->getSignature();
      // get the minimum number of parameters with type information that need to match
      unsigned mp = vs->getMinParamTypes();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp < mp || vp > tp)
         continue;

      bool dup = true;
      bool ambiguous = false;
      unsigned max = QORE_MAX(tp, vp);
      for (unsigned pi = 0; pi < max; ++pi) {
         const QoreTypeInfo* variantTypeInfo = vs->getParamTypeInfo(pi);
         bool variantHasDefaultArg = vs->hasDefaultArg(pi);

         const QoreTypeInfo* typeInfo = sig->getParamTypeInfo(pi);
         assert(!sig->getParseParamTypeInfo(pi));
         bool thisHasDefaultArg = sig->hasDefaultArg(pi);

         // check for ambiguous matches
         if (typeInfo) {
            if (!variantTypeInfo->hasType() && thisHasDefaultArg)
               ambiguous = true;
            else if (!typeInfo->isInputIdentical(variantTypeInfo)) {
               dup = false;
               break;
            }
         }
         else {
            if (variantTypeInfo->hasType() && variantHasDefaultArg)
               ambiguous = true;
            else if (!typeInfo->isInputIdentical(variantTypeInfo)) {
               dup = false;
               break;
            }
         }
      }
      if (dup)
         return ambiguous ? QTI_AMBIGUOUS : QTI_IDENT;
   }
   return QTI_NOT_EQUAL;
}

int QoreFunction::parseCheckDuplicateSignature(AbstractQoreFunctionVariant* variant) {
   AbstractFunctionSignature* sig = variant->getSignature();

   // check for duplicate parameter signatures
   unsigned vnp = sig->numParams();
   unsigned vtp = sig->getParamTypes();
   unsigned vmp = sig->getMinParamTypes();

   // first check pending variants
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserSignature* vs = reinterpret_cast<UserSignature*>((*i)->getSignature());
      assert(!vs->resolved);
      // get the minimum number of parameters with type information that need to match
      unsigned mp = vs->getMinParamTypes();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();

      //printd(5, "QoreFunction::parseCheckDuplicateSignature() adding %s(%s) checking %s(%s) vmp: %d vtp: %d mp: %d tp: %d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), vmp, vtp, mp, tp);

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vmp > tp || vtp < mp)
         continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
         duplicateSignatureException(className(), getName(), sig);
         return -1;
      }

      unsigned np = vs->numParams();

      bool dup = true;
      bool ambiguous = false;
      bool recheck = false;
      unsigned max = QORE_MAX(np, vnp);
      for (unsigned pi = 0; pi < max; ++pi) {
         const QoreTypeInfo* variantTypeInfo = vs->getParamTypeInfo(pi);
         const QoreParseTypeInfo* variantParseTypeInfo = variantTypeInfo ? 0 : vs->getParseParamTypeInfo(pi);
         bool variantHasDefaultArg = vs->hasDefaultArg(pi);

         const QoreTypeInfo* typeInfo = sig->getParamTypeInfo(pi);
         const QoreParseTypeInfo* parseTypeInfo = typeInfo ? 0 : sig->getParseParamTypeInfo(pi);
         bool thisHasDefaultArg = sig->hasDefaultArg(pi);

         // FIXME: this is a horribly-complicated if/then/else structure

         // check for ambiguous matches
         if (typeInfo || parseTypeInfo) {
            if (!variantTypeInfo->hasType() && !variantParseTypeInfo && thisHasDefaultArg)
               ambiguous = true;
            else {
               // check for real matches
               if (typeInfo) {
                  if (variantTypeInfo) {
                     if (!typeInfo->isInputIdentical(variantTypeInfo)) {
                        dup = false;
                        break;
                     }
                  }
                  else if (!variantParseTypeInfo->parseStageOneIdenticalWithParsed(typeInfo, recheck)) {
                     dup = false;
                     break;
                  }
               }
               else {
                  if (variantTypeInfo) {
                     if (!parseTypeInfo->parseStageOneIdenticalWithParsed(variantTypeInfo, recheck)) {
                        dup = false;
                        break;
                     }
                  }
                  else if (!parseTypeInfo->parseStageOneIdentical(variantParseTypeInfo)) {
                     dup = false;
                     break;
                  }
               }
            }
         }
         else {
            if ((variantTypeInfo->hasType() || variantParseTypeInfo) && variantHasDefaultArg)
               ambiguous = true;
            else if (variantTypeInfo) {
               if (!typeInfo->isInputIdentical(variantTypeInfo)) {
                  dup = false;
                  break;
               }
            }
            else if (!variantParseTypeInfo->parseStageOneIdenticalWithParsed(typeInfo, recheck)) {
               dup = false;
               break;
            }
         }
         //printd(5, "QoreFunction::parseCheckDuplicateSignature() %s(%s) == %s(%s) i: %d: %s <=> %s dup: %d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), pi, typeInfo->getName(), variantTypeInfo->getName(), dup);
      }
      if (dup) {
         if (ambiguous)
            ambiguousDuplicateSignatureException(className(), getName(), (*i)->getSignature(), sig);
         else
            duplicateSignatureException(className(), getName(), sig);
         return -1;
      }
      if (recheck)
         variant->setRecheck();
   }
   // now check already-committed variants
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature* uvsig = (*i)->getSignature();

      // get the minimum number of parameters with type information that need to match
      unsigned mp = uvsig->getMinParamTypes();
      // get total number of parameters with type information
      unsigned tp = uvsig->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vmp > tp || vtp < mp)
         continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
         duplicateSignatureException(className(), getName(), sig);
         return -1;
      }

      unsigned np = uvsig->numParams();

      bool dup = true;
      bool ambiguous = false;
      unsigned max = QORE_MAX(np, vnp);
      bool recheck = false;
      for (unsigned pi = 0; pi < max; ++pi) {
         const QoreTypeInfo* variantTypeInfo = uvsig->getParamTypeInfo(pi);
         bool variantHasDefaultArg = uvsig->hasDefaultArg(pi);

         const QoreTypeInfo* typeInfo = sig->getParamTypeInfo(pi);
         const QoreParseTypeInfo* parseTypeInfo = typeInfo ? 0 : sig->getParseParamTypeInfo(pi);
         bool thisHasDefaultArg = sig->hasDefaultArg(pi);

         // compare the to-be-committed types with resolved types in committed variants
         if (parseTypeInfo) {
            if (!variantTypeInfo && thisHasDefaultArg) {
               ambiguous = true;
            }
            else if (!parseTypeInfo->parseStageOneIdenticalWithParsed(variantTypeInfo, recheck)) {
               recheck = false;
               dup = false;
               break;
            }
         }
         else {
            if (!typeInfo && variantTypeInfo && variantHasDefaultArg) {
               ambiguous = true;
            }
            else if (typeInfo && !variantTypeInfo && thisHasDefaultArg) {
               ambiguous = true;
            }
            else if (!typeInfo->isInputIdentical(variantTypeInfo)) {
               dup = false;
               break;
            }
         }
      }
      if (dup) {
         if (ambiguous)
            ambiguousDuplicateSignatureException(className(), getName(), (*i)->getSignature(), sig);
         else
            duplicateSignatureException(className(), getName(), sig);
         return -1;
      }
      if (recheck)
         variant->setRecheck();
   }

   return 0;
}

AbstractFunctionSignature* QoreFunction::parseGetUniqueSignature() const {
   if (vlist.singular() && pending_vlist.empty())
      return first()->getSignature();

   if (pending_vlist.singular() && vlist.empty()) {
      assert(pending_first()->getUserVariantBase());
      UserSignature* sig = pending_first()->getUserVariantBase()->getUserSignature();
      sig->resolve();
      return sig;
   }

   return 0;
}

void QoreFunction::resolvePendingSignatures() {
   const QoreTypeInfo* ti = 0;

   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      assert((*i)->getUserVariantBase());
      UserSignature* sig = (*i)->getUserVariantBase()->getUserSignature();
      sig->resolve();

      if (same_return_type && parse_same_return_type) {
         const QoreTypeInfo* st = sig->getReturnTypeInfo();
         if (i != pending_vlist.begin() && !st->isInputIdentical(ti))
            parse_same_return_type = false;
         ti = st;
      }
   }
}

int QoreFunction::addPendingVariant(AbstractQoreFunctionVariant* variant) {
   parse_rt_done = false;
   parse_init_done = false;

   // check for duplicate signature with existing variants
   if (parseCheckDuplicateSignature(variant)) {
      variant->deref();
      return -1;
   }

   pending_vlist.push_back(variant);

   return 0;
}

void QoreFunction::parseCommit() {
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      vlist.push_back(*i);

      if ((*i)->isUser()) {
         if (!has_mod_pub && (*i)->isModulePublic())
            has_mod_pub = true;
         if (!has_user)
            has_user = true;
      }
      else if (!has_builtin)
         has_builtin = true;
   }
   pending_vlist.clear();

   if (!parse_same_return_type && same_return_type)
      same_return_type = false;

   parse_rt_done = true;
   parse_init_done = true;
}

void QoreFunction::parseRollback() {
   pending_vlist.del();

   if (!parse_same_return_type && same_return_type)
      parse_same_return_type = true;

   parse_rt_done = true;
   parse_init_done = true;
}

void QoreFunction::parseInit() {
   if (parse_init_done)
      return;
   parse_init_done = true;

   if (parse_same_return_type)
      parse_same_return_type = same_return_type;

   OptionalNamespaceParseContextHelper pch(ns);

   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      (*i)->parseInit(this);
   }
}

QoreValue UserClosureFunction::evalClosure(const QoreClosureBase& closure_base, QoreProgram* pgm, const QoreListNode* args, QoreObject *self, const qore_class_private* class_ctx, ExceptionSink* xsink) const {
   // closures cannot be overloaded
   assert(vlist.singular());

   const AbstractQoreFunctionVariant* variant = first();

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, this, variant, "<anonymous closure>", args, self, class_ctx, CT_USER);
   if (*xsink)
      return QoreValue();

   ProgramThreadCountContextHelper tch(xsink, pgm, true);
   if (*xsink)
      return QoreValue();

   ThreadSafeLocalVarRuntimeEnvironmentHelper ch(&closure_base);

   //printd(5, "UserClosureFunction::evalClosure() this: %p (%s) variant: %p args: %p self: %p\n", this, getName(), variant, args, self);
   return UCLOV_const(variant)->evalClosure(ceh, self, xsink);
}

void UserFunctionVariant::parseInit(QoreFunction* f) {
   signature.resolve();

   // resolve and push current return type on stack
   ParseCodeInfoHelper rtih(f->getName(), signature.getReturnTypeInfo());

   // can (and must) be called even if statements is NULL
   statements->parseInit(this);

   // recheck types against committed types if necessary
   if (recheck)
      f->parseCheckDuplicateSignatureCommitted(&signature);
}

void UserClosureVariant::parseInit(QoreFunction* f) {
   UserClosureFunction* cf = static_cast<UserClosureFunction*>(f);

   signature.resolve();

   // resolve and push current return type on stack
   ParseCodeInfoHelper rtih(f->getName(), signature.getReturnTypeInfo());

   statements->parseInitClosure(this, cf);

   // only one variant is possible, no need to recheck types
}
