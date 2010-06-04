/*
  Function.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QoreClassIntern.h>

#include <stdio.h>
#include <ctype.h>
#include <assert.h>

// FIXME: needed for the log10() call as a hack in float_minus_infinity_noop() below
#include <math.h>

// FIXME: xxx set parse location
static inline void duplicateSignatureException(const char *name, UserVariantBase *uvb) {
   parseException("DUPLICATE-SIGNATURE", "%s(%s) has already been declared", name, uvb->getUserSignature()->getSignatureText());
}

// FIXME: xxx set parse location
static inline void ambiguousDuplicateSignatureException(const char *name, AbstractQoreFunctionVariant *uvb1, UserVariantBase *uvb2) {
   parseException("DUPLICATE-SIGNATURE", "%s(%s) matches already declared variant %s(%s)", name, uvb2->getUserSignature()->getSignatureText(), name, uvb1->getSignature()->getSignatureText());
}

static void do_call_name(QoreString &desc, const AbstractQoreFunction *func) {
   const char *class_name = func->className();
   if (class_name)
      desc.sprintf("%s::", class_name);
   desc.sprintf("%s(", func->getName());
}

static void addArgs(QoreStringNode &desc, const QoreListNode *args) {
   if (!args || !args->size()) {
      desc.concat(NO_TYPE_INFO);
      return;
   }   
   for (unsigned i = 0; i < args->size(); ++i) {
      const AbstractQoreNode *n = args->retrieve_entry(i);
      if (is_nothing(n))
	 desc.concat("NOTHING");
      else {
	 qore_type_t t = n ? n->getType() : NT_NOTHING;
	 if (t == NT_OBJECT)
	    desc.concat(reinterpret_cast<const QoreObject *>(n)->getClassName());
	 else
	    desc.concat(n->getTypeName());
      }
      if (i != (args->size() - 1))
	 desc.concat(", ");
   }
}

int CodeEvaluationHelper::processDefaultArgs(const AbstractQoreFunction *func, const AbstractQoreFunctionVariant *variant, ExceptionSink *xsink) {
   bool edit_done = false;

   // get default argument list of variant
   AbstractFunctionSignature *sig = variant->getSignature();
   const arg_vec_t &defaultArgList = sig->getDefaultArgList();
   const type_vec_t &typeList = sig->getTypeList();

   unsigned max = QORE_MAX(defaultArgList.size(), typeList.size());
   for (unsigned i = 0; i < max; ++i) {
      if (i < defaultArgList.size() && defaultArgList[i] && (!tmp || is_nothing(tmp->retrieve_entry(i)))) {
	 // edit the current argument list
	 if (!edit_done) {
	    tmp.edit();
	    edit_done = true;
	 }

	 AbstractQoreNode **p = const_cast<QoreListNode *>(*tmp)->get_entry_ptr(i);
	 *p = defaultArgList[i]->eval(xsink);
	 if (*xsink)
	    return -1;
      }
      else if (i < typeList.size()) {
	 const AbstractQoreNode *n = tmp ? tmp->retrieve_entry(i) : 0;
	 const QoreTypeInfo *paramTypeInfo = sig->getParamTypeInfo(i);
	 // test for change or incompatibility
	 if (paramTypeInfo->hasInputFilter()) {
	    if (!edit_done && paramTypeInfo->mayRequireFilter(n)) {
	       // edit the current argument list
	       tmp.edit();
	       edit_done = true;
	    }

	    AbstractQoreNode **p = const_cast<QoreListNode *>(*tmp)->get_entry_ptr(i);
	    *p = paramTypeInfo->acceptInputParam(i, sig->getName(i), *p, xsink);
	    if (*xsink)
	       return -1;
	 }
      }
   }

   // check for excess args exception
   unsigned nargs = tmp ? tmp->size() : 0;
   if (!nargs)
      return 0;
   unsigned nparams = sig->numParams();

   //printd(5, "processDefaultArgs() %s nargs=%d nparams=%d flags=%lld po=%d\n", func->getName(), nargs, nparams, variant->getFlags(), (bool)(getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)));
   if (nargs > nparams && (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS))) {
      int64 flags = variant->getFlags();

      if (!(flags & QC_USES_EXTRA_ARGS)) {
	 for (unsigned i = nparams; i < nargs; ++i) {
	    //printd(5, "processDefaultArgs() %s arg %d nothing=%d\n", func->getName(), i, is_nothing(tmp->retrieve_entry(i)));
	    if (!is_nothing(tmp->retrieve_entry(i))) {
	       QoreStringNode *desc = new QoreStringNode("call to ");
	       do_call_name(*desc, func);
	       if (nparams)
		  desc->concat(sig->getSignatureText());
	       desc->concat(") made as "); 
	       do_call_name(*desc, func);
	       addArgs(*desc, *tmp);
	       unsigned diff = nargs - nparams;
	       desc->sprintf(") with %d excess argument%s, which is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set", diff, diff == 1 ? "" : "s");
	       xsink->raiseException("CALL-WITH-TYPE-ERRORS", desc);
	       return -1;
	    }
	 }
      }
   }

   return 0;
}

void AbstractFunctionSignature::addDefaultArgument(const AbstractQoreNode *arg) {
   assert(arg);
   str.append(" = ");
   qore_type_t t = arg->getType();
   if (t == NT_BAREWORD) {
      str.append(reinterpret_cast<const BarewordNode *>(arg)->str);
      return;
   }
   if (t == NT_CONSTANT) {
      str.append(reinterpret_cast<const ConstantNode *>(arg)->scoped_ref->getIdentifier());
      return;
   }
   if (arg->is_value()) {
      QoreNodeAsStringHelper sh(arg, FMT_NONE, 0);
      str.append(sh->getBuffer());
      return;
   }
   str.append("<exp>");
}

UserSignature::UserSignature(int n_first_line, int n_last_line, AbstractQoreNode *params, RetTypeInfo *retTypeInfo) : 
   AbstractFunctionSignature(retTypeInfo ? retTypeInfo->getTypeInfo() : 0), 
   parseReturnTypeInfo(retTypeInfo ? retTypeInfo->takeParseTypeInfo() : 0), 
   first_line(n_first_line), last_line(n_last_line), parse_file(get_parse_file()),
   lv(0), argvid(0), selfid(0), resolved(false) {
   // assign no return type if return type declaration is missing and PO_REQUIRE_TYPES or PO_REQUIRE_PROTOTYPES is set
   if (!retTypeInfo && (getProgram()->getParseOptions() & (PO_REQUIRE_TYPES | PO_REQUIRE_PROTOTYPES)))
      returnTypeInfo = nothingTypeInfo;
   delete retTypeInfo;

   if (!params) {
      str = NO_TYPE_INFO;
      return;
   }

   int needs_types = getProgram()->getParseOptions() & (PO_REQUIRE_TYPES | PO_REQUIRE_PROTOTYPES);

   ReferenceHolder<AbstractQoreNode> param_holder(params, 0);

   if (params->getType() == NT_VARREF) {
      pushParam(reinterpret_cast<VarRefNode *>(params), 0, needs_types);
      return;
   }

   if (params->getType() == NT_TREE) {
      pushParam(reinterpret_cast<QoreTreeNode *>(params), needs_types);
      return;
   }

   if (params->getType() != NT_LIST) {
      param_error();
      return;
   }

   QoreListNode *l = reinterpret_cast<QoreListNode *>(params);

   parseTypeList.reserve(l->size());
   typeList.reserve(l->size());
   defaultArgList.reserve(l->size());

   ListIterator li(l);
   while (li.next()) {
      AbstractQoreNode *n = li.getValue();
      qore_type_t t = n ? n->getType() : 0;
      if (t == NT_TREE)
	 pushParam(reinterpret_cast<QoreTreeNode *>(n), needs_types);
      else if (t == NT_VARREF)
	 pushParam(reinterpret_cast<VarRefNode *>(n), 0, needs_types);
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

void UserSignature::pushParam(QoreTreeNode *t, bool needs_types) {
   if (t->getOp() != OP_ASSIGNMENT)
      parse_error("invalid expression with the '%s' operator in parameter list; only simple assignments to default values are allowed", t->getOp()->getName());
   else if (t->left && t->left->getType() != NT_VARREF)
      param_error();
   else {
      VarRefNode *v = reinterpret_cast<VarRefNode *>(t->left);
      AbstractQoreNode *defArg = t->right;
      t->right = 0;
      pushParam(v, defArg, needs_types);
   }
}

void UserSignature::pushParam(VarRefNode *v, AbstractQoreNode *defArg, bool needs_types) {
   // check for duplicate name
   for (name_vec_t::iterator i = names.begin(), e = names.end(); i != e; ++i)
      if (*i == v->getName())
	 parse_error(parse_file, first_line, last_line, "duplicate variable '$%s' declared in parameter list", (*i).c_str());

   names.push_back(v->getName());

   bool is_decl = v->isDecl();
   if (needs_types && !is_decl)
      parse_error(parse_file, first_line, last_line, "parameter '$%s' declared without type information, but parse options require all declarations to have type information", v->getName());

   // see if this is a new object call
   if (v->has_effect()) {
      // here we make 4 virtual function calls when 2 would be enough, but no need to optimize for speed for an exception
      parse_error(parse_file, first_line, last_line, "parameter '$%s' may not be declared with new object syntax; instead use: '%s $%s = new %s()'", v->getName(), v->getNewObjectClassName(), v->getName(), v->getNewObjectClassName());
   }

   if (is_decl) {
      VarRefDeclNode *vd = reinterpret_cast<VarRefDeclNode *>(v);
      QoreParseTypeInfo *pti = vd->takeParseTypeInfo();
      parseTypeList.push_back(pti);
      const QoreTypeInfo *ti = vd->getTypeInfo();
      typeList.push_back(ti);

      if (ti == nothingTypeInfo)
	 parse_error(parse_file, first_line, last_line, "parameter '$%s' may not be declared as type 'nothing'", v->getName());

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
   defaultArgList.push_back(defArg);
   if (defArg)
      addDefaultArgument(defArg);

   if (v->getType() == VT_LOCAL)
      parse_error(parse_file, first_line, last_line, "invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
   else if (v->getType() == VT_GLOBAL)
      parse_error(parse_file, first_line, last_line, "invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
}

void UserVariantBase::parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo) {
   signature.parseInitPushLocalVars(classTypeInfo);
}

void UserVariantBase::parseInitPopLocalVars() {
   signature.parseInitPopLocalVars();
}

void UserSignature::parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo) {
   lv.reserve(parseTypeList.size());
   
   if (selfid)
      push_self_var(selfid);
   else if (classTypeInfo)
      selfid = push_local_var("self", classTypeInfo, false, 1);
   
   // push $argv var on stack and save id
   argvid = push_local_var("argv", 0, false, 1);
   printd(5, "UserSignature::parseInitPushLocalVars() this=%p argvid=%p\n", this, argvid);

   resolve();

   // init param ids and push local parameter vars on stack
   for (unsigned i = 0; i < typeList.size(); ++i) {
      // check for dups but do not check if the variables are referenced in the block
      lv.push_back(push_local_var(names[i].c_str(), typeList[i], true, 1));
      printd(5, "UserSignature::parseInitPushLocalVars() registered local var %s (id=%p)\n", names[i].c_str(), lv[i]);
   }
}

void UserSignature::parseInitPopLocalVars() {
   for (unsigned i = 0; i < typeList.size(); ++i)
      pop_local_var();
   
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
      returnTypeInfo = parseReturnTypeInfo->resolveAndDelete();
      parseReturnTypeInfo = 0;
   }
#ifdef DEBUG
   else assert(!parseReturnTypeInfo);
#endif

   for (unsigned i = 0; i < parseTypeList.size(); ++i) {
      if (parseTypeList[i]) {
	 assert(!typeList[i]);
	 typeList[i] = parseTypeList[i]->resolveAndDelete();
      }

      // initialize default arguments
      if (defaultArgList[i]) {
	 int lvids = 0;
	 const QoreTypeInfo *argTypeInfo = 0;
	 defaultArgList[i] = defaultArgList[i]->parseInit(selfid, 0, lvids, argTypeInfo);
	 if (lvids) {
	    // FIXME: set parse position?
	    parse_error("illegal local variable declaration in default value expression in parameter '$%s'", names[i].c_str());
	    while (lvids--)
	       pop_local_var();
	 }
	 // check type compatibility
	 if (!typeList[i]->parseAccepts(argTypeInfo)) {
	    QoreStringNode *desc = new QoreStringNode;
	    desc->sprintf("parameter '$%s' expects ", names[i].c_str());
	    typeList[i]->getThisType(*desc);
	    desc->concat(", but the default value is ");
	    argTypeInfo->getThisType(*desc);
	    desc->concat(" instead");
	    getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
	 }
      }
   }
   parseTypeList.clear();
}

bool AbstractQoreFunction::existsVariant(const type_vec_t &paramTypeInfo) const {
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *sig = (*i)->getSignature();
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

static QoreStringNode *getNoopError(const AbstractQoreFunction *func, const AbstractQoreFunction *aqf, const AbstractQoreFunctionVariant *variant) {
   QoreStringNode *desc = new QoreStringNode;
   do_call_name(*desc, aqf);
   desc->sprintf("%s) is a variant that returns a constant value when incorrect data types are passed to the function", variant->getSignature()->getSignatureText());
   const QoreTypeInfo *rti = variant->getReturnTypeInfo();
   if (rti->hasType() && !variant->numParams()) {
      desc->concat(" and always returns ");
      if (rti->getUniqueReturnClass() || func->className()) {
	 rti->getThisType(*desc);
      }
      else {
	 // get actual value and include in warning
	 ReferenceHolder<AbstractQoreNode> v(variant->evalFunction(func->getName(), 0, 0), 0);
	 if (is_nothing(*v))
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

// finds a variant at runtime
const AbstractQoreFunctionVariant *AbstractQoreFunction::findVariant(const QoreListNode *args, bool only_user, ExceptionSink *xsink) const {
   int match = -1;
   const AbstractQoreFunctionVariant *variant = 0;

   //printd(0, "AbstractQoreFunction::findVariant() this=%p %s%s%s() vlist=%d (pend=%d) ilist=%d args=%p (%d)\n", this, className() ? className() : "", className() ? "::" : "", getName(), vlist.size(), pending_vlist.size(), ilist.size(), args, args ? args->size() : 0);

   // perfect match score
   unsigned nargs = args ? args->size() : 0;
   int perfect = nargs * 2;

   const AbstractQoreFunction *aqf = 0;
   AbstractFunctionSignature *sig = 0;

   // iterate through inheritance list
   for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
      aqf = *aqfi;

      for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
	 // skip checking variant if we are only looking for user variants and this variant is builtin
	 if (only_user && !(*i)->isUser())
	    continue;

	 sig = (*i)->getSignature();
	 assert(sig);

	 //printd(5, "AbstractQoreFunction::findVariant() this=%p %s(%s) args=%p (%d) class=%s\n", this, getName(), sig->getSignatureText(), args, args ? args->size() : 0, aqf->className() ? aqf->className() : "n/a");

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
	       const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	       const AbstractQoreNode *n = args ? args->retrieve_entry(pi) : 0;

	       //printd(5, "AbstractQoreFunction::findVariant() this=%p %s(%s) i=%d param=%s arg=%s\n", this, getName(), sig->getSignatureText(), pi, t->getName(), get_type_name(n));
	    
	       int rc;
	       if (is_nothing(n) && sig->hasDefaultArg(pi))
		  rc = QTI_IGNORE;
	       else {
		  rc = t->runtimeAcceptsValue(n);
		  if (rc == QTI_NOT_EQUAL) {
		     ok = false;
		     break;
		  }
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
      if (match == perfect)
	 break;
   }
   if (!variant && !only_user) {
      QoreStringNode *desc = new QoreStringNode("no variant matching '");
      const char *class_name = className();
      if (class_name)
	 desc->sprintf("%s::", class_name);
      desc->sprintf("%s(", getName());
      addArgs(*desc, args);
      desc->concat(") can be found; the following variants were tested:");

      // add variants tested
      // iterate through inheritance list
      for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
	 aqf = *aqfi;
	 class_name = aqf->className();

	 for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
	    desc->concat("\n   ");
	    if (class_name)
	       desc->sprintf("%s::", class_name);
	    desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
	 }
      }
      xsink->raiseException("RUNTIME-OVERLOAD-ERROR", desc);
   }
   else if (variant) {
      int64 po = getProgram()->getParseOptions64();

      // check parse options
      if (variant->getFunctionality() & po) {
	 //printd(5, "AbstractQoreFunction::findVariant() this=%p %s(%s) getProgram()=%p getProgram()->getParseOptions()=%x variant->getFunctionality()=%x\n", this, getName(), variant->getSignature()->getSignatureText(), getProgram(), getProgram()->getParseOptions(), variant->getFunctionality());
	 if (!only_user) {
	    const char *class_name = className();
	    xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin %s '%s%s%s(%s)'", class_name ? "method" : "function", class_name ? class_name : "", class_name ? "::" : "", getName(), variant->getSignature()->getSignatureText());
	 }
	 return 0;
      }

      if (po & (PO_REQUIRE_TYPES | PO_STRICT_ARGS) && variant->getFlags() & QC_RUNTIME_NOOP) {
	 QoreStringNode *desc = getNoopError(this, aqf, variant);
	 desc->concat("; this variant is not accessible when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
	 xsink->raiseException("CALL-WITH-TYPE-ERRORS", desc);
      }
   }

   //printd(5, "AbstractQoreFunction::findVariant() this=%p %s() returning %p %s(%s) class=%s\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant && aqf && aqf->className() ? aqf->className() : "n/a");

   return variant;
}

static AbstractQoreFunctionVariant *doSingleVariantTypeException(int pi, const char *name, const char *sig, const QoreTypeInfo *proto, const QoreTypeInfo *arg) {
   QoreStringNode *desc = new QoreStringNode("argument ");
   desc->sprintf("%d to '%s(%s)' expects ", pi, name, sig);
   proto->getThisType(*desc);
   desc->concat(", but call supplies ");
   arg->getThisType(*desc);
   getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
   return 0;
}

// finds a variant at runtime
const AbstractQoreFunctionVariant *AbstractQoreFunction::runtimeFindVariant(const type_vec_t &argTypeInfo, bool only_user) const {
   // the number of parameters * 2 matched to arguments (compatible but not perfect match = 1, perfect match = 2)
   int match = -1;
   // pointer to the variant matched
   const AbstractQoreFunctionVariant *variant = 0;
   unsigned num_args = argTypeInfo.size();

   //printd(5, "AbstractQoreFunction::runtimeFindVariant() this=%p %s() vlist=%d pend=%d ilist=%d num_args=%d\n", this, getName(), vlist.size(), pending_vlist.size(), ilist.size(), num_args);

   AbstractQoreFunction *aqf = 0;
   
   // iterate through inheritance list
   for (ilist_t::const_iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
      aqf = *aqfi;
      //printd(5, "AbstractQoreFunction::runtimeFindVariant() %p %s testing function %p\n", this, getName(), aqf);
      assert(!aqf->vlist.empty());

      // check committed list only
      for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
	 if (only_user && !(*i)->isUser())
	    continue;

	 AbstractFunctionSignature *sig = (*i)->getSignature();

	 //printd(5, "AbstractQoreFunction::runtimeFindVariant() this=%p checking %s(%s) variant=%p sig->pt=%d sig->mpt=%d match=%d, args=%d\n", this, getName(), sig->getSignatureText(), variant, sig->getParamTypes(), sig->getMinParamTypes(), match, num_args);
	 
	 if (!variant && !sig->getParamTypes()) {
	    match = 0;
	    variant = *i;
	    continue;
	 }

	 // skip variants with signatures with fewer possible elements than the best match already
	 if ((int)(sig->getParamTypes() * 2) <= match)
	    continue;

	 int count = 0;
	 bool ok = true;
	 for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	    const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	    const QoreTypeInfo *a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;
	    // must have type information
	    assert(a || pi >= num_args);

	    //printd(5, "AbstractQoreFunction::runtimeFindVariant() %s(%s) pi=%d t=%s (has type: %d) a=%s (%p) t->parseEqual(a)=%d\n", getName(), sig->getSignatureText(), pi, t->getName(), t->hasType(), a->getName(), a, t->parseEqual(a));
	  
	    int rc;
	    if (t->hasType() && !a->hasType() && sig->hasDefaultArg(pi))
	       rc = QTI_IGNORE;
	    else {
	       rc = t->parseAccepts(a);
	       if (rc == QTI_NOT_EQUAL) {
		  ok = false;
		  break;
	       }
	    }

	    // only increment for actual type matches (t may be NULL)
	    if (t && rc != QTI_IGNORE)
	       count += rc;
	 }
	 if (!ok)
	    continue;

	 //printd(5, "AbstractQoreFunction::runtimeFindVariant() this=%p tested %s(%s) ok=%d count=%d match=%d variant_pmatch=%d\n", this, getName(), sig->getSignatureText(), ok, count, match, variant_pmatch);
	 if (count > match) {
	    match = count;
	    variant = *i;
	 }
      }
   }

   //printd(5, "AbstractQoreFunction::runtimeFindVariant() this=%p %s%s%s() returning %p %s(%s)\n", this, className() ? className() : "", className() ? "::" : "", getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a");
   return variant;
}

static void do_call_str(QoreString &desc, const AbstractQoreFunction *func, const type_vec_t &argTypeInfo) {
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

static void warn_excess_args(AbstractQoreFunction *func, const type_vec_t &argTypeInfo, AbstractFunctionSignature *sig) {
   unsigned nargs = argTypeInfo.size();
   unsigned nparams = sig->numParams();

   QoreStringNode *desc = new QoreStringNode("call to ");
   desc->concat(func->className() ? "method " : "function ");
   do_call_name(*desc, func);
   if (nparams)
      desc->concat(sig->getSignatureText());
   desc->concat(") made as ");
   do_call_str(*desc, func, argTypeInfo);
   unsigned diff = nargs - nparams;
   desc->sprintf(" (with %d excess argument%s)", diff, diff == 1 ? "" : "s");
   // raise warning if require-types is not set
   if (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES | PO_STRICT_ARGS)) {
      desc->concat("; this is an error when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
      getProgram()->makeParseException("CALL-WITH-TYPE-ERRORS", desc);
   }
   else {
      // raise warning
      desc->concat("; excess arguments will be ignored; to disable this warning, use '%%disable-warning excess-args' in your code");
      getProgram()->makeParseWarning(QP_WARN_EXCESS_ARGS, "EXCESS-ARGS", desc);
   }
}

// finds a variant at parse time
const AbstractQoreFunctionVariant *AbstractQoreFunction::parseFindVariant(const type_vec_t &argTypeInfo) {
   // the number of parameters * 2 matched to arguments (compatible but not perfect match = 1, perfect match = 2)
   int match = -1;
   // the number of possible matches at runtime (due to missing types at parse time); number of parameters
   int pmatch = -1;
   // the number of arguments matched perfectly in case of a tie score
   int nperfect = -1;
   // pointer to the variant matched
   const AbstractQoreFunctionVariant *variant = 0;
   unsigned num_args = argTypeInfo.size();

   printd(5, "AbstractQoreFunction::parseFindVariant() this=%p %s() vlist=%d pend=%d ilist=%d num_args=%d\n", this, getName(), vlist.size(), pending_vlist.size(), ilist.size(), num_args);

   AbstractQoreFunction *aqf = 0;
   
   // iterate through inheritance list
   for (ilist_t::iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
      aqf = *aqfi;
      //printd(5, "AbstractQoreFunction::parseFindVariant() %p %s testing function %p\n", this, getName(), aqf);
      assert(!aqf->vlist.empty() || !aqf->pending_vlist.empty());

      // check committed list
      for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
	 AbstractFunctionSignature *sig = (*i)->getSignature();

	 printd(5, "AbstractQoreFunction::parseFindVariant() this=%p checking %s(%s) variant=%p sig->pt=%d sig->mpt=%d match=%d, args=%d\n", this, getName(), sig->getSignatureText(), variant, sig->getParamTypes(), sig->getMinParamTypes(), match, num_args);
	 
	 if (!variant && !sig->getParamTypes() && pmatch == -1) {
	    match = pmatch = nperfect = 0;
	    variant = *i;
	    continue;
	 }

	 // skip variants with signatures with fewer possible elements than the best match already
	 if ((int)(sig->getParamTypes() * 2) > match) {
	    int variant_pmatch = 0;
	    int count = 0;
	    int variant_nperfect = 0;
	    bool ok = true;
	    bool variant_missing_types = false;
	    
	    for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	       const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	       const QoreTypeInfo *a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;
	       
	       printd(5, "AbstractQoreFunction::parseFindVariant() %s(%s) pi=%d t=%s (has type: %d) a=%s (%p) t->parseAccepts(a)=%d\n", getName(), sig->getSignatureText(), pi, t->getName(), t->hasType(), a->getName(), a, t->parseAccepts(a));
	       
	       int rc = QTI_UNASSIGNED;
	       if (t->hasType()) {
		  if (!a->hasType()) {
		     if (pi < num_args) {
			variant_missing_types = true;
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
		  rc = t->parseAccepts(a);
		  if (rc == QTI_IDENT)
		     ++variant_nperfect;
	       }

	       if (rc == QTI_NOT_EQUAL) {
		  ok = false;
		  // raise a detailed parse exception immediately if there is only one variant
		  if (ilist.size() == 1 && aqf->pending_vlist.singular() && aqf->vlist.empty() && getProgram()->getParseExceptionSink())
		     return doSingleVariantTypeException(pi + 1, getName(), sig->getSignatureText(), t, a);
		  break;
	       }
	       // only increment for actual type matches (t may be NULL)
	       if (t) {
		  ++variant_pmatch;
		  if (rc != QTI_IGNORE)
		     count += rc;
	       }
	    }
	    printd(5, "AbstractQoreFunction::parseFindVariant() this=%p tested %s(%s) ok=%d count=%d match=%d variant_missing_types=%d variant_pmatch=%d\n", this, getName(), sig->getSignatureText(), ok, count, match, variant_missing_types, variant_pmatch);
	    if (!ok)
	       continue;

	    if (count > match || (count == match && variant_nperfect > nperfect)) {
	       // if we could possibly match less than another variant
	       // then we have to match at runtime
	       if (variant_pmatch <= pmatch)
		  variant = 0;
	       else {
		  // only set variant if it's the longest absolute match and the
		  // longest potential match
		  pmatch = variant_pmatch;
		  match = count;
		  nperfect = variant_nperfect;
		  if (!variant_missing_types) {
		     printd(5, "AbstractQoreFunction::parseFindVariant() assigning variant %p %s(%s)\n", *i, getName(), sig->getSignatureText());
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
	 UserVariantBase *uvb = (*i)->getUserVariantBase();
	 UserSignature *sig = uvb->getUserSignature();
	 // resolve types in signature if necessary
	 sig->resolve();

	 if (!variant && !sig->getParamTypes() && pmatch == -1) {
	    match = pmatch = nperfect = 0;
	    variant = *i;
	    continue;
	 }

	 // skip variants with signatures with fewer possible elements than the best match already
	 if ((int)(sig->getParamTypes() * 2) > match) {
	    int variant_pmatch = 0;
	    int count = 0;
	    int variant_nperfect = 0;
	    bool ok = true;
	    bool variant_missing_types = false;

	    for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	       const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	       const QoreTypeInfo *a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;

	       int rc = QTI_UNASSIGNED;
	       if (t->hasType()) {
		  if (!a->hasType()) {
		     if (pi < num_args) {
			variant_missing_types = true;
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
		  rc = t->parseAccepts(a);
		  if (rc == QTI_IDENT)
		     ++variant_nperfect;
	       }

	       if (rc == QTI_NOT_EQUAL) {
		  ok = false;
		  // raise a detailed parse exception immediately if there is only one variant
		  if (ilist.size() == 1 && aqf->pending_vlist.singular() && aqf->vlist.empty() && getProgram()->getParseExceptionSink())
		     return doSingleVariantTypeException(pi + 1, getName(), sig->getSignatureText(), t, a);
		  break;
	       }
	       // only increment for actual type matches (t may be NULL)
	       if (t) {
		  ++variant_pmatch;
		  //printd(0, "AbstractQoreFunction::parseFindVariant() this=%p %s() variant=%p i=%d match (param %s == %s)\n", this, getName(), variant, pi, t->getName(), a->getName());
		  if (rc != QTI_IGNORE)
		     count += rc;
	       }
	    }
	    if (!ok)
	       continue;

	    if (count > match || (count == match && variant_nperfect > nperfect)) {
	       // if we could possibly match less than another variant
	       // then we have to match at runtime
	       if (variant_pmatch <= pmatch)
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
   }

   if (!variant && pmatch == -1 && getProgram()->getParseExceptionSink()) {
      QoreStringNode *desc = new QoreStringNode("no variant matching '");
      do_call_str(*desc, this, argTypeInfo);
      desc->concat(" can be found; the following variants were tested:");

      // add variants tested
      // iterate through inheritance list
      for (ilist_t::iterator aqfi = ilist.begin(), aqfe = ilist.end(); aqfi != aqfe; ++aqfi) {
	 aqf = *aqfi;	 
	 const char *class_name = aqf->className();
	 
	 for (vlist_t::const_iterator i = aqf->vlist.begin(), e = aqf->vlist.end(); i != e; ++i) {
	    desc->concat("\n   ");
	    if (class_name)
	       desc->sprintf("%s::", class_name);
	    desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
	 }
	 for (vlist_t::const_iterator i = aqf->pending_vlist.begin(), e = aqf->pending_vlist.end(); i != e; ++i) {
	    desc->concat("\n   ");
	    if (class_name)
	       desc->sprintf("%s::", class_name);
	    desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
	 }
      }
      getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
   }
   else if (variant) {
      int64 flags = variant->getFlags();
      if (flags & (QC_NOOP | QC_RUNTIME_NOOP)) {
	 QoreStringNode *desc = getNoopError(this, aqf, variant);
	 if ((flags & QC_RUNTIME_NOOP) && (getProgram()->getParseOptions64() & (PO_REQUIRE_TYPES|PO_STRICT_ARGS))) {
	    desc->concat("; this variant is not accessible when PO_REQUIRE_TYPES or PO_STRICT_ARGS is set");
	    getProgram()->makeParseException("CALL-WITH-TYPE-ERRORS", desc);
	 }
	 else {
	    desc->concat("; to disable this warning, use '%disable-warning invalid-operation' in your code");
	    getProgram()->makeParseWarning(QP_WARN_CALL_WITH_TYPE_ERRORS, "CALL-WITH-TYPE-ERRORS", desc);
	 }
      }

      AbstractFunctionSignature *sig = variant->getSignature();
      if (!(flags & QC_USES_EXTRA_ARGS) && num_args > sig->numParams())
	 warn_excess_args(this, argTypeInfo, sig);
   }
   printd(5, "AbstractQoreFunction::parseFindVariant() this=%p %s%s%s() returning %p %s(%s) flags=%lld\n", this, className() ? className() : "", className() ? "::" : "", getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a", variant ? variant->getFlags() : 0ll);
   return variant;
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode *AbstractQoreFunction::evalFunction(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const {
   const char *fname = getName();
   CodeEvaluationHelper ceh(xsink, fname, args);
   if (*xsink) return 0;

   if (!variant) {
      variant = findVariant(ceh.getArgs(), false, xsink);
      if (!variant) {
	 assert(*xsink);
	 return 0;
      }
   }
   if (ceh.processDefaultArgs(this, variant, xsink))
      return 0;

   ceh.setCallType(variant->getCallType());
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   return variant->evalFunction(fname, ceh.getArgs(), xsink);
}

// finds a variant and checks variant capabilities against current
// program parse options
AbstractQoreNode *AbstractQoreFunction::evalDynamic(const QoreListNode *args, ExceptionSink *xsink) const {
   const char *fname = getName();
   CodeEvaluationHelper ceh(xsink, fname, args);
   if (*xsink) return 0;

   const AbstractQoreFunctionVariant *variant = findVariant(ceh.getArgs(), false, xsink);
   if (!variant) {
      assert(*xsink);
      return 0;
   }
   if (ceh.processDefaultArgs(this, variant, xsink))
      return 0;

   ceh.setCallType(variant->getCallType());
   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   return variant->evalFunction(fname, ceh.getArgs(), xsink);
}

void AbstractQoreFunction::addBuiltinVariant(AbstractQoreFunctionVariant *variant) {
   assert(variant->getCallType() == CT_BUILTIN);
#ifdef DEBUG
   // FIXME: this algorithm is no longer valid due to default arguments
   // does not detect ambiguous signatures
   AbstractFunctionSignature *sig = variant->getSignature();
   // check for duplicate parameter signatures
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *vs = (*i)->getSignature();
      unsigned tp = vs->numParams();
      if (tp != sig->numParams())
	 continue;
      if (!tp) {
	 printd(0, "BuiltinFunctionBase::addBuiltinVariant() this=%p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
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
	 printd(0, "BuiltinFunctionBase::addBuiltinVariant() this=%p %s(%s) added twice: %p, %p\n", this, getName(), sig->getSignatureText(), *i, variant);
	 assert(false);
      }
   }
#endif
   addVariant(variant);
}

UserVariantExecHelper::~UserVariantExecHelper() {
   if (!uvb)
      return;
   UserSignature *sig = uvb->getUserSignature();
   // uninstantiate local vars from param list
   for (unsigned i = 0; i < sig->numParams(); ++i)
      sig->lv[i]->uninstantiate(xsink);
}

UserVariantBase::UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, RetTypeInfo *rv, bool synced, int64 n_flags) 
   : statements(b), signature(n_sig_first_line, n_sig_last_line, params, rv), synchronized(synced), gate(synced ? new VRMutex() : 0),
     recheck(false), flags(n_flags) {
   printd(5, "UserVariantBase::UserVariantBase() params=%p rv=%p b=%p synced=%d\n", params, rv, b, synced);
}

UserVariantBase::~UserVariantBase() {
   delete gate;
   delete statements;
}

// evaluates arguments and sets up the argv variable
int UserVariantBase::setupCall(const QoreListNode *args, ReferenceHolder<QoreListNode> &argv, ExceptionSink *xsink) const {
   unsigned num_args = args ? args->size() : 0;
   // instantiate local vars from param list
   unsigned num_params = signature.numParams();

   for (unsigned i = 0; i < num_params; ++i) {
      AbstractQoreNode *np = args ? const_cast<AbstractQoreNode *>(args->retrieve_entry(i)) : 0;
      AbstractQoreNode *n = 0;
      //printd(5, "UserVariantBase::setupCall() eval %d: instantiating param lvar %p (%s) (exp nt=%d %p %s)\n", i, signature.lv[i], signature.lv[i]->getName(), get_node_type(np), np, get_type_name(np));
      if (!is_nothing(np)) {
	 if (np->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(np);
	    //printd(5, "UserVariantBase::setupCall() eval %d: instantiating (%s) as reference (ref exp nt=%d %p %s)\n", i, signature.lv[i]->getName(), get_node_type(r->getExpression()), r->getExpression(), get_type_name(r->getExpression()));
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    if (!*xsink)
	       signature.lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	 }
	 else
	    signature.lv[i]->instantiate(np->refSelf());
      }
      else
	 signature.lv[i]->instantiate(n);

      // the above if block will only instantiate the local variable if no
      // exceptions have occured. therefore here we cleanup the rest
      // of any already instantiated local variables if an exception does occur
      if (*xsink) {
	 if (n) n->deref(xsink);
	 while (i) signature.lv[--i]->uninstantiate(xsink);
	 return -1;
      }
   }

   // if there are more arguments than parameters
   printd(5, "UserVariantBase::setupCall() params=%d args=%d\n", num_params, num_args);

   if (num_params < num_args) {
      argv = new QoreListNode;

      for (unsigned i = 0; i < (num_args - num_params); i++) {
	 // here we try to take the reference from args if possible
	 AbstractQoreNode *n = args ? const_cast<AbstractQoreNode *>(args->get_referenced_entry(i + num_params)) : 0;
	 argv->push(n);
      }
   }

   return 0;
}

AbstractQoreNode *UserVariantBase::evalIntern(ReferenceHolder<QoreListNode> &argv, QoreObject *self, ExceptionSink *xsink, const char *class_name) const {
   AbstractQoreNode *val = 0;
   if (statements) {      
      if (signature.selfid) {
	 assert(self);
         signature.selfid->instantiate_object(self);
      }

      // instantiate argv and push id on stack
      signature.argvid->instantiate(argv ? argv->refSelf() : 0);

      {
	 ArgvContextHelper argv_helper(argv.release(), xsink);

	 // enter gate if necessary
	 if (!synchronized || (gate->enter(xsink) >= 0)) {
	    // execute function
	    val = statements->exec(xsink);

	    // exit gate if necessary
	    if (synchronized)
	       gate->exit();
	 }
      }

      // uninstantiate argv
      signature.argvid->uninstantiate(xsink);

      // if self then uninstantiate
      if (signature.selfid)
         signature.selfid->uninstantiate(xsink);
   }
   else
      argv = 0; // dereference argv now

   return val;
}

// primary function for executing user code
AbstractQoreNode *UserVariantBase::eval(const char *name, const QoreListNode *args, QoreObject *self, ExceptionSink *xsink, const char *class_name) const {
   QORE_TRACE("UserVariantBase::eval()");
   printd(5, "UserVariantBase::eval() this=%p name=%s() args=%p (size=%d) self=%p\n", this, name, args, args ? args->size() : 0, self);

   UserVariantExecHelper uveh(this, args, xsink);
   if (!uveh)
      return 0;

   CODE_CONTEXT_HELPER(CT_USER, name, self, xsink);
     
   return evalIntern(uveh.getArgv(), self, xsink, class_name);
}

UserFunction::UserFunction(char *n_name) : name(n_name) {
   printd(5, "UserFunction::UserFunction(%s)\n", n_name ? n_name : "(null)");
}

UserFunction::~UserFunction() {
   printd(5, "UserFunction::~UserFunction() deleting %s\n", name);
   if (name)
      free(name);
}

// returns 0 for OK, -1 for error
// this is called after types have been resolved and the types must be rechecked
int AbstractQoreFunction::parseCheckDuplicateSignatureCommitted(UserVariantBase *variant) {
   UserSignature *sig = variant->getUserSignature();

   unsigned vp = sig->getParamTypes();

   // now check already-committed variants
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *vs = (*i)->getSignature();
      // get the minimum number of parameters with type information that need to match
      unsigned mp = vs->getMinParamTypes();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp < mp || vp > tp)
	 continue;

      // we have already checked for duplicates with no signature, so we can assume
      // this is not the case here
      assert(tp);

      bool dup = true;
      bool ambiguous = false;
      unsigned max = QORE_MAX(tp, vp);
      for (unsigned pi = 0; pi < max; ++pi) {
	 const QoreTypeInfo *variantTypeInfo = vs->getParamTypeInfo(pi);
	 bool variantHasDefaultArg = vs->hasDefaultArg(pi);

	 const QoreTypeInfo *typeInfo = sig->getParamTypeInfo(pi);
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
      if (dup) {
	 if (ambiguous)
	    ambiguousDuplicateSignatureException(getName(), *i, variant);
	 else
	    duplicateSignatureException(getName(), variant);
	 return -1;
      }
   }
   return 0;
}

int AbstractQoreFunction::parseCheckDuplicateSignature(UserVariantBase *variant) {
   // check for duplicate parameter signatures
   UserSignature *sig = variant->getUserSignature();
   assert(!sig->resolved);

   unsigned vnp = sig->numParams();
   unsigned vtp = sig->getParamTypes();
   unsigned vmp = sig->getMinParamTypes();

   // first check pending variants
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserSignature *vs = reinterpret_cast<UserSignature *>((*i)->getSignature());
      assert(!vs->resolved);
      // get the minimum number of parameters with type information that need to match
      unsigned mp = vs->getMinParamTypes();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();

      //printd(5, "AbstractQoreFunction::parseCheckDuplicateSignature() adding %s(%s) checking %s(%s) vmp=%d vtp=%d mp=%d tp=%d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), vmp, vtp, mp, tp);

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vmp > tp || vtp < mp)
	 continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }

      unsigned np = vs->numParams();

      bool dup = true;
      bool ambiguous = false;
      bool recheck = false;
      unsigned max = QORE_MAX(np, vnp);
      for (unsigned pi = 0; pi < max; ++pi) {
	 const QoreTypeInfo *variantTypeInfo = vs->getParamTypeInfo(pi);
	 const QoreParseTypeInfo *variantParseTypeInfo = vs->getParseParamTypeInfo(pi);
	 bool variantHasDefaultArg = vs->hasDefaultArg(pi);

	 const QoreTypeInfo *typeInfo = sig->getParamTypeInfo(pi);
	 const QoreParseTypeInfo *parseTypeInfo = sig->getParseParamTypeInfo(pi);
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
	 //printd(5, "AbstractQoreFunction::parseCheckDuplicateSignature() %s(%s) == %s(%s) i=%d: %s <=> %s dup=%d\n", getName(), sig->getSignatureText(), getName(), vs->getSignatureText(), pi, typeInfo->getName(), variantTypeInfo->getName(), dup);
      }
      if (dup) {
	 if (ambiguous)
	    ambiguousDuplicateSignatureException(getName(), *i, variant);
	 else
	    duplicateSignatureException(getName(), variant);
	 return -1;
      }
      if (recheck)
	 variant->setRecheck();
   }
   // now check already-committed variants
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *uvsig = (*i)->getSignature();

      // get the minimum number of parameters with type information that need to match
      unsigned mp = uvsig->getMinParamTypes();
      // get total number of parameters with type information
      unsigned tp = uvsig->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vmp > tp || vtp < mp)
	 continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }

      unsigned np = uvsig->numParams();

      bool dup = true;
      bool ambiguous = false;
      unsigned max = QORE_MAX(np, vnp);
      bool recheck = false;
      for (unsigned pi = 0; pi < max; ++pi) {
	 const QoreTypeInfo *variantTypeInfo = uvsig->getParamTypeInfo(pi);
	 bool variantHasDefaultArg = uvsig->hasDefaultArg(pi);

	 const QoreTypeInfo *typeInfo = sig->getParamTypeInfo(pi);
	 const QoreParseTypeInfo *parseTypeInfo = sig->getParseParamTypeInfo(pi);
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
	    ambiguousDuplicateSignatureException(getName(), *i, variant);
	 else
	    duplicateSignatureException(getName(), variant);
	 return -1;
      }
      if (recheck)
	 variant->setRecheck();
   }

   return 0;
}

AbstractFunctionSignature *AbstractQoreFunction::parseGetUniqueSignature() const {
   if (vlist.singular() && pending_vlist.empty())
      return first()->getSignature();

   if (pending_vlist.singular() && vlist.empty()) {
      assert(pending_first()->getUserVariantBase());
      UserSignature *sig = pending_first()->getUserVariantBase()->getUserSignature();
      sig->resolve();
      return sig;
   }

   return 0;
}

void AbstractQoreFunction::resolvePendingSignatures() {
   const QoreTypeInfo *ti = 0;

   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      assert((*i)->getUserVariantBase());
      UserSignature *sig = (*i)->getUserVariantBase()->getUserSignature();
      sig->resolve();
      
      if (same_return_type && parse_same_return_type) {
	 const QoreTypeInfo *st = sig->getReturnTypeInfo();
	 if (i != pending_vlist.begin() && !st->isInputIdentical(ti))
	    parse_same_return_type = false;
	 ti = st;
      }
   }
}

int AbstractQoreFunction::parseAddVariant(AbstractQoreFunctionVariant *variant) {
   parse_rt_done = false;

   // check for duplicate signature with existing variants
   if (parseCheckDuplicateSignature(variant->getUserVariantBase())) {
      variant->deref();
      return -1;
   }

   pending_vlist.push_back(variant);

   return 0;
}

void AbstractQoreFunction::parseCommit() {
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      vlist.push_back(*i);
   }
   pending_vlist.clear();

   if (!parse_same_return_type && same_return_type)
      same_return_type = false;

   parse_rt_done = true;
}

void AbstractQoreFunction::parseRollback() {
   pending_vlist.del();

   if (!parse_same_return_type && same_return_type)
      parse_same_return_type = true;

   parse_rt_done = true;
}

void UserFunction::parseInit() {
   parse_same_return_type = same_return_type;

   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      assert((*i)->getUserVariantBase());
      UFV(*i)->parseInit(name);

      // recheck types against committed types if necessary
      if (UFV(*i)->getRecheck())
	 parseCheckDuplicateSignatureCommitted(UFV(*i));
   }
}

AbstractQoreNode *UserClosureFunction::evalClosure(const QoreListNode *args, QoreObject *self, ExceptionSink *xsink) const {
   // closures cannot be overloaded
   assert(vlist.singular());

   const UserClosureVariant *variant = UCLOV_const(first());

   // setup call, save runtime position
   CodeEvaluationHelper ceh(xsink, "<anonymous closure>", args, 0, CT_USER);
   if (ceh.processDefaultArgs(this, variant, xsink))
      return 0;

   ceh.setReturnTypeInfo(variant->getReturnTypeInfo());

   //printd(0, "UserClosureFunction::evalClosure() this=%p (%s) variant=%p args=%p self=%p\n", this, getName(), variant, args, self);

   return variant->evalClosure(ceh.getArgs(), self, xsink);
}

void UserClosureFunction::parseInitClosure(const QoreTypeInfo *classTypeInfo, lvar_set_t *lvlist) {
   // closures cannot be overloaded
   assert(pending_vlist.singular());
   UserClosureVariant *v = UCLOV(pending_first());
   v->parseInitClosure(classTypeInfo, lvlist);
   // we can commit the code now because if there are any errors, the entire closure will be deleted
   parseCommit();
}

void UserFunctionVariant::parseInit(const char *fname) {
   signature.resolve();

   // resolve and push current return type on stack
   ParseCodeInfoHelper rtih(fname, signature.getReturnTypeInfo());
   
   // can (and must) be called even if statements is NULL
   statements->parseInit(this);
}

void UserClosureVariant::parseInitClosure(const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist) {
   signature.resolve();

   // resolve and push current return type on stack
   ParseCodeInfoHelper rtih("<anonymous closure>", signature.getReturnTypeInfo());

   statements->parseInitClosure(this, classTypeInfo, vlist);
}

// this will only be called with lvalue expressions
AbstractQoreNode *doPartialEval(AbstractQoreNode *n, bool *is_self_ref, ExceptionSink *xsink) {
   AbstractQoreNode *rv = 0;
   qore_type_t ntype = n->getType();
   if (ntype == NT_TREE) {
      QoreTreeNode *tree = reinterpret_cast<QoreTreeNode *>(n);
      ReferenceHolder<AbstractQoreNode> nn(tree->right->eval(xsink), xsink);
      if (*xsink)
	 return 0;

      SimpleRefHolder<QoreTreeNode> t(new QoreTreeNode(doPartialEval(tree->left, is_self_ref, xsink), tree->getOp(), nn ? nn.release() : nothing()));
      if (t->left)
	 rv = t.release();
   }
   else {
      rv = n->refSelf();
      if (ntype == NT_SELF_VARREF)
	 (*is_self_ref) = true;
   }
   return rv;
}

// this function does nothing - it's here for backwards-compatibility for functions
// that accept invalid arguments and return nothing
AbstractQoreNode *f_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return 0;
}

// this function does nothing - it's here for backwards-compatibility for functions
// that accept invalid arguments and return boolean False
AbstractQoreNode *f_bool_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return &False;
}

AbstractQoreNode *f_bool_true_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return &True;
}

// this function does nothing - it's here for backwards-compatibility for functions
// that accept invalid arguments and return an empty string
AbstractQoreNode *f_string_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return null_string();
}

AbstractQoreNode *f_float_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return zero_float();
}

AbstractQoreNode *f_float_one_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreFloatNode(1.0);
}

AbstractQoreNode *f_float_minus_infinity_noop(const QoreListNode *args, ExceptionSink *xsink) {
   // FIXME: how to specify -inf as an immediate value?
   return new QoreFloatNode(log10((double)0.0));
}

AbstractQoreNode *f_int_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return zero();
}

AbstractQoreNode *f_int_minus_one_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(-1);
}

AbstractQoreNode *f_int_one_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new QoreBigIntNode(1);
}

AbstractQoreNode *f_list_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return empty_list();
}

AbstractQoreNode *f_date_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return zero_date();
}

AbstractQoreNode *f_reldate_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new DateTimeNode(true);
}

AbstractQoreNode *f_binary_noop(const QoreListNode *args, ExceptionSink *xsink) {
   return new BinaryNode;
}
