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

// FIXME: xxx add signature information, set parse location
static inline void duplicateSignatureException(const char *name, UserVariantBase *uvb) {
   parseException("DUPLICATE-SIGNATURE", "%s(%s) has already been declared with this signature", name, uvb->getUserSignature()->getSignatureText());
}

UserSignature::UserSignature(int n_first_line, int n_last_line, AbstractQoreNode *params, QoreParseTypeInfo *n_returnTypeInfo) : 
   parseReturnTypeInfo(n_returnTypeInfo), 
   first_line(n_first_line), last_line(n_last_line), parse_file(get_parse_file()),
   lv(0), argvid(0), selfid(0), resolved(false) {

   if (!params) {
      str = NO_TYPE_INFO;
      return;
   }

   int needs_types = getProgram()->getParseOptions() & PO_REQUIRE_TYPES;

   ReferenceHolder<AbstractQoreNode> param_holder(params, 0);

   if (params->getType() == NT_VARREF) {
      pushParam(reinterpret_cast<VarRefNode *>(params), needs_types);
      return;
   }

   if (params->getType() != NT_LIST) {
      param_error();
      return;
   }

   QoreListNode *l = reinterpret_cast<QoreListNode *>(params);

   parseTypeList.reserve(l->size());
   ListIterator li(l);
   while (li.next()) {
      AbstractQoreNode *n = li.getValue();
      qore_type_t t = n ? n->getType() : 0;
      if (t != NT_VARREF) {
	 if (n)
	    param_error();
	 break;
      }
	 
      pushParam(reinterpret_cast<VarRefNode *>(n), needs_types);
      // add a comma to the signature string if it's not the last parameter
      if (!li.last())
	 str.append(", ");
   }
}

void UserSignature::pushParam(VarRefNode *v, bool needs_types) {
   // check for duplicate name
   for (name_vec_t::iterator i = names.begin(), e = names.end(); i != e; ++i)
      if (*i == v->getName())
	 parse_error("duplicate variable '$%s' declared in parameter list", (*i).c_str());

   names.push_back(v->getName());

   QoreParseTypeInfo *pti = v->takeTypeInfo();
   if (needs_types && !pti)
      parse_error("function parameter '$%s' declared without type information, but parse options require all declarations to have type information", v->getName());
   parseTypeList.push_back(pti);
   if (pti->hasType())
      ++num_param_types;

   // add type name to signature
   pti->concatName(str);

   if (v->getType() == VT_LOCAL)
      parse_error("invalid local variable declaration in argument list; by default all variables declared in argument lists are local");
   else if (v->getType() == VT_GLOBAL)
      parse_error("invalid global variable declaration in argument list; by default all variables declared in argument lists are local");
}

void UserSignature::parseInitPushLocalVars(const QoreTypeInfo *classTypeInfo) {
   lv.reserve(parseTypeList.size());

   if (selfid)
      push_self_var(selfid);
   else if (classTypeInfo)
      selfid = push_local_var("self", classTypeInfo, false);
   
   // push $argv var on stack and save id
   // FIXME: push as list if hard typing enforced with parse options
   argvid = push_local_var("argv", 0, false);
   printd(5, "UserSignature::parseInitPushLocalVars() this=%p argvid=%p\n", this, argvid);

   resolve();

   // init param ids and push local param vars on stack
   for (unsigned i = 0; i < typeList.size(); ++i) {
      lv.push_back(push_local_var(names[i].c_str(), typeList[i]));
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
	 if (!paramTypeInfo[pi]->checkIdentical(sig->getParamTypeInfo(pi))) {
	    ok = false;
	    break;
	 }
      }
      if (ok)
	 return true;
   }
   return false;
}

void addArgs(QoreStringNode &desc, const QoreListNode *args) {
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

// finds a variant at runtime
const AbstractQoreFunctionVariant *AbstractQoreFunction::findVariant(const QoreListNode *args, ExceptionSink *xsink, const char *class_name) const {
   unsigned match = 0;
   const AbstractQoreFunctionVariant *variant = 0;

   //printd(5, "AbstractQoreFunction::findVariant() this=%p %s() vlist=%d (pend=%d) args=%p (%d)\n", this, getName(), vlist.size(), pending_vlist.size(), args, args ? args->size() : 0);

   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *sig = (*i)->getSignature();
      assert(sig);

      if (!variant && !sig->getParamTypes()) {
	 variant = *i;
	 continue;
      }

      //printd(5, "AbstractQoreFunction::findVariant() this=%p %s(%s) args=%p (%d)\n", this, getName(), sig->getSignatureText(), args, args ? args->size() : 0);

      // skip variants with signatures with fewer possible elements than the best match already
      if ((sig->getParamTypes() * 2) > match) {
	 unsigned count = 0;
	 bool ok = true;
	 for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	    const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	    const AbstractQoreNode *n = args ? args->retrieve_entry(pi) : 0;
	    int rc = t->testTypeCompatibility(n);
	    if (rc == QTI_NOT_EQUAL) {
	       ok = false;
	       break;
	    }

	    // only increment for actual type matches (t may be NULL)
	    if (t)
	       count += rc;
	 }
	 if (!ok)
	    continue;
	 if (count > match) {
	    match = count;
	    variant = *i;
	 }
      }
   }
   if (!variant) {
      QoreStringNode *desc = new QoreStringNode("no variant matching '");
      if (class_name)
	 desc->sprintf("%s::", class_name);
      desc->sprintf("%s(", getName());
      addArgs(*desc, args);
      desc->concat(") can be found; the following variants were tested:");

      // add variants tested
      for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
	 desc->concat("\n   ");
	 if (class_name)
	    desc->sprintf("%s::", class_name);
	 desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
      }

      xsink->raiseException("RUNTIME-OVERLOAD-ERROR", desc);
   }
   else {
      // check parse options
      if (variant->getFunctionality() & getProgram()->getParseOptions()) {
	 //printd(5, "AbstractQoreFunction::findVariant() this=%p %s(%s) getProgram()=%p getProgram()->getParseOptions()=%x variant->getFunctionality()=%x\n", this, getName(), variant->getSignature()->getSignatureText(), getProgram(), getProgram()->getParseOptions(), variant->getFunctionality());
	 xsink->raiseException("INVALID-FUNCTION-ACCESS", "parse options do not allow access to builtin function '%s(%s)'", getName(), variant->getSignature()->getSignatureText());
	 return 0;
      }
   }

   return variant;
}

// finds a variant at parse time
const AbstractQoreFunctionVariant *AbstractQoreFunction::parseFindVariant(const type_vec_t &argTypeInfo, const char *class_name) {
   // the number of parameters * 2 matched to arguments (compatible but not perfect match = 1, perfect match = 2)
   unsigned match = 0;
   // the number of possible matches at runtime (due to missing types at parse time); number of parameters
   unsigned longest_pmatch = 0;
   // pointer to the variant matched
   const AbstractQoreFunctionVariant *variant = 0;
   unsigned num_args = argTypeInfo.size();

   //printd(5, "AbstractQoreFunction::parseFindVariant() this=%p %s() vlist=%d pend=%d num_args=%d\n", this, getName(), vlist.size(), pending_vlist.size(), num_args);

   assert(!vlist.empty() || !pending_vlist.empty());
   // check committed list
   for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *sig = (*i)->getSignature();

      if (!variant && !sig->getParamTypes()) {
	 variant = *i;
	 continue;
      }

      // skip variants with signatures with fewer possible elements than the best match already
      if ((sig->getParamTypes() * 2) > match) {
	 unsigned variant_longest_pmatch = 0;
	 unsigned count = 0;
	 bool ok = true;
	 for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	    const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	    const QoreTypeInfo *a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;
	    if (t->hasType() && !a) {
	       ++variant_longest_pmatch;
	       continue;
	    }

	    int rc = t->parseEqual(a);

	    if (rc == QTI_NOT_EQUAL) {
	       ok = false;

	       // raise a detailed parse exception immediately if there is only one variant
	       if (vlist.singular() && pending_vlist.empty() && getProgram()->getParseExceptionSink()) {
		  QoreStringNode *desc = new QoreStringNode("argument ");
                  desc->sprintf("%d to '%s(%s)' expects ", pi + 1, getName(), sig->getSignatureText());
                  t->getThisType(*desc);
                  desc->concat(", but call supplies ");
                  a->getThisType(*desc);
                  getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
		  return 0;
	       }

	       break;
	    }
	    // only increment for actual type matches (t may be NULL)
	    if (t) {
	       ++variant_longest_pmatch;
	       count += rc;
	    }
	 }
	 //printd(5, "AbstractQoreFunction::parseFindVariant() this=%p tested %s(%s) ok=%d count=%d match=%d\n", this, getName(), sig->getSignatureText(), ok, count, match);
	 if (!ok)
	    continue;
	 if (count > match) {
	    // if we could possibly match less than another variant
	    // then we have to match at runtime
	    if (variant_longest_pmatch <= longest_pmatch)
	       variant = 0;
	    else {
	       // only set variant if it's the longest absolute match and the
	       // longest potential match
	       longest_pmatch = variant_longest_pmatch;
	       match = count;
	       variant = *i;
	    }
	 }
	 else {
	    // if we could possibly match less than another variant
	    // then we have to match at runtime
	    if (variant_longest_pmatch >= longest_pmatch) {
	       variant = 0;
	       longest_pmatch = variant_longest_pmatch;
	    }
	 }
      }
   }

   // check pending list
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserVariantBase *uvb = (*i)->getUserVariantBase();
      UserSignature *sig = uvb->getUserSignature();
      // resolve types in signature if necessary
      sig->resolve();

      if (!variant && !sig->getParamTypes()) {
	 variant = *i;
	 continue;
      }

      // skip variants with signatures with fewer possible elements than the best match already
      if ((sig->getParamTypes() * 2) > match) {
	 unsigned variant_longest_pmatch = 0;
	 unsigned count = 0;
	 bool ok = true;
	 for (unsigned pi = 0; pi < sig->numParams(); ++pi) {
	    const QoreTypeInfo *t = sig->getParamTypeInfo(pi);
	    const QoreTypeInfo *a = (num_args && num_args > pi) ? argTypeInfo[pi] : 0;
	    if (t->hasType() && !a) {
	       ++variant_longest_pmatch;
	       continue;
	    }

	    int rc = t->parseEqual(a);

	    if (rc == QTI_NOT_EQUAL) {
	       ok = false;
	       
	       // raise a detailed parse exception immediately if there is only one variant
	       if (pending_vlist.singular() && vlist.empty() && getProgram()->getParseExceptionSink()) {
		  QoreStringNode *desc = new QoreStringNode("argument ");
                  desc->sprintf("%d to '%s(%s)' expects ", pi + 1, getName(), sig->getSignatureText());
                  t->getThisType(*desc);
                  desc->concat(", but call supplies ");
                  a->getThisType(*desc);
                  getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
		  return 0;
	       }

	       break;
	    }
	    // only increment for actual type matches (t may be NULL)
	    if (t) {
	       ++variant_longest_pmatch;
	       //printd(0, "AbstractQoreFunction::parseFindVariant() this=%p %s() variant=%p i=%d match (param %s == %s)\n", this, getName(), variant, pi, t->getName(), a->getName());
	       count += rc;
	    }
	 }
	 if (!ok)
	    continue;

	 if (count > match) {
	    // if we could possibly match less than another variant
	    // then we have to match at runtime
	    if (variant_longest_pmatch <= longest_pmatch)
	       variant = 0;
	    else {
	       // only set variant if it's the longest absolute match and the
	       // longest potential match
	       longest_pmatch = variant_longest_pmatch;
	       match = count;
	       variant = *i;
	    }
	 }
	 else {
	    // if we could possibly match less than another variant
	    // then we have to match at runtime
	    if (variant_longest_pmatch >= longest_pmatch) {
	       variant = 0;
	       longest_pmatch = variant_longest_pmatch;
	    }
	 }
      }
   }

   if (!variant && !longest_pmatch && getProgram()->getParseExceptionSink()) {
      QoreStringNode *desc = new QoreStringNode("no variant matching '");
      if (class_name)
	 desc->sprintf("%s::", class_name);
      desc->sprintf("%s(", getName());
      if (!num_args)
	 desc->concat(NO_TYPE_INFO);
      else
	 for (unsigned i = 0; i < num_args; ++i) {
	    desc->concat(argTypeInfo[i]->getName());
	    if (i != (num_args - 1))
	       desc->concat(", ");
	 }
      desc->concat(") can be found; the following variants were tested:");

      // add variants tested
      for (vlist_t::const_iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
	 desc->concat("\n   ");
	 if (class_name)
	    desc->sprintf("%s::", class_name);
	 desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
      }
      for (vlist_t::const_iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
	 desc->concat("\n   ");
	 if (class_name)
	    desc->sprintf("%s::", class_name);
	 desc->sprintf("%s(%s)", getName(), (*i)->getSignature()->getSignatureText());
      }
      getProgram()->makeParseException("PARSE-TYPE-ERROR", desc);
   }
   printd(5, "AbstractQoreFunction::parseFindVariant() this=%p %s() returning %p %s(%s)\n", this, getName(), variant, getName(), variant ? variant->getSignature()->getSignatureText() : "n/a");
   return variant;
}

// if the variant was identified at parse time, then variant will not be NULL, otherwise if NULL then it is identified at run time
AbstractQoreNode *AbstractQoreFunction::evalFunction(const AbstractQoreFunctionVariant *variant, const QoreListNode *args, ExceptionSink *xsink) const {
   const char *fname = getName();
   CodeEvaluationHelper ceh(xsink, fname, args);
   if (*xsink) return 0;

   if (!variant) {
      variant = findVariant(ceh.getArgs(), xsink);
      if (!variant) {
	 assert(*xsink);
	 return 0;
      }
   }
   ceh.setCallType(variant->getCallType());

   return variant->evalFunction(fname, ceh.getArgs(), xsink);
}

// finds a variant and checks variant capabilities against current
// program parse options
AbstractQoreNode *AbstractQoreFunction::evalDynamic(const QoreListNode *args, ExceptionSink *xsink) const {
   const char *fname = getName();
   CodeEvaluationHelper ceh(xsink, fname, args);
   if (*xsink) return 0;

   const AbstractQoreFunctionVariant *variant = findVariant(ceh.getArgs(), xsink);
   if (!variant) {
      assert(*xsink);
      return 0;
   }
   ceh.setCallType(variant->getCallType());

   return variant->evalFunction(fname, ceh.getArgs(), xsink);
}

void AbstractQoreFunction::addBuiltinVariant(AbstractQoreFunctionVariant *variant) {
   assert(variant->getCallType() == CT_BUILTIN);
#ifdef DEBUG
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

UserVariantBase::UserVariantBase(StatementBlock *b, int n_sig_first_line, int n_sig_last_line, AbstractQoreNode *params, QoreParseTypeInfo *rv, bool synced) 
   : statements(b), signature(n_sig_first_line, n_sig_last_line, params, rv), synchronized(synced), gate(synced ? new VRMutex() : 0),
     recheck(false) {
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
      const QoreTypeInfo *paramTypeInfo = signature.getParamTypeInfo(i);
      //printd(5, "UserVariantBase::setupCall() eval %d: instantiating param lvar %p (%s) (exp nt=%d %p %s)\n", i, signature.lv[i], signature.lv[i]->getName(), get_node_type(np), np, get_type_name(np));
      if (!is_nothing(np)) {
	 if (np->getType() == NT_REFERENCE) {
	    const ReferenceNode *r = reinterpret_cast<const ReferenceNode *>(np);
	    //printd(5, "UserVariantBase::setupCall() eval %d: instantiating (%s) as reference (ref exp nt=%d %p %s)\n", i, signature.lv[i]->getName(), get_node_type(r->getExpression()), r->getExpression(), get_type_name(r->getExpression()));
	    bool is_self_ref = false;
	    n = doPartialEval(r->getExpression(), &is_self_ref, xsink);
	    if (!*xsink) {
	       n = paramTypeInfo->checkTypeInstantiation(signature.getName(i), n, xsink);
	       if (!*xsink)
		  signature.lv[i]->instantiate(n, is_self_ref ? getStackObject() : 0);
	    }
	 }
	 else {
	    n = paramTypeInfo->checkTypeInstantiation(signature.getName(i), np->refSelf(), xsink);
	    if (!*xsink)
	       signature.lv[i]->instantiate(n);
	 }
      }
      else {
	 n = paramTypeInfo->checkTypeInstantiation(signature.getName(i), 0, xsink);
	 if (!*xsink)
	    signature.lv[i]->instantiate(n);
      }

      // the above if block will only instantiate the local variable if no
      // exceptions have occurred. therefore here we cleanup the rest
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
      if (self)
         signature.selfid->instantiate_object(self);

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
      if (self)
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
int AbstractQoreFunction::parseCheckDuplicateSignatureCommitted(UserVariantBase *variant) {
   UserSignature *sig = variant->getUserSignature();

   unsigned vp = sig->getParamTypes();

   // now check already-committed variants
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *vs = (*i)->getSignature();
      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();
      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp != tp)
	 continue;

      // we have already checked for duplicates with no signature, so we can assume
      // this is not the case here
      assert(tp);

      bool dup = true;
      unsigned max = QORE_MAX(tp, vp);
      for (unsigned pi = 0; pi < max; ++pi) {
	 // compare the unresolved type with resolved types in committed variants
	 if (!sig->getParseParamTypeInfo(pi)->checkIdentical(vs->getParamTypeInfo(pi))) {
	    dup = false;
	    break;
	 }
      }
      if (dup) {
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

   unsigned vp = sig->getParamTypes();
   // first check pending variants
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      UserSignature *vs = reinterpret_cast<UserSignature *>((*i)->getSignature());
      assert(!vs->resolved);

      // get number of parameters with type information
      unsigned tp = vs->getParamTypes();
      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp != tp)
	 continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }

      bool dup = true;
      unsigned max = QORE_MAX(tp, vp);
      for (unsigned pi = 0; pi < max; ++pi) {
	 if (!sig->getParseParamTypeInfo(pi)->parseStageOneIdentical(vs->getParseParamTypeInfo(pi))) {
	    dup = false;
	    break;
	 }
      }
      if (dup) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }
   }
   // now check already-committed variants
   for (vlist_t::iterator i = vlist.begin(), e = vlist.end(); i != e; ++i) {
      AbstractFunctionSignature *uvsig = (*i)->getSignature();

      // get number of parameters with type information
      unsigned tp = uvsig->getParamTypes();

      // shortcut: if the two variants have different numbers of parameters with type information, then they do not match
      if (vp != tp)
	 continue;

      // the 2 signatures have the same number of parameters with type information
      if (!tp) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }

      bool dup = true;
      unsigned max = QORE_MAX(tp, vp);
      bool recheck = false;
      for (unsigned pi = 0; pi < max; ++pi) {
	 // compare the unresolved type with resolved types in committed variants
	 if (!sig->getParseParamTypeInfo(pi)->parseStageOneIdenticalWithParsed(uvsig->getParamTypeInfo(pi), recheck)) {
	    recheck = false;
	    dup = false;
	    break;
	 }
      }
      if (dup) {
	 duplicateSignatureException(getName(), variant);
	 return -1;
      }
      if (recheck)
	 variant->setRecheck();
   }

   return 0;
}

int AbstractQoreFunction::parseAddVariant(AbstractQoreFunctionVariant *variant) {
   // check for duplicate signature with existing variants
   if (parseCheckDuplicateSignature(variant->getUserVariantBase())) {
      delete variant;
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
}

void AbstractQoreFunction::parseRollback() {
   pending_vlist.del();
}

void UserFunction::parseInit() {
   for (vlist_t::iterator i = pending_vlist.begin(), e = pending_vlist.end(); i != e; ++i) {
      assert((*i)->getUserVariantBase());
      UFV(*i)->parseInit();

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

void UserFunctionVariant::parseInit() {
   signature.resolve();

   // resolve and push current return type on stack
   ReturnTypeInfoHelper rtih(signature.getReturnTypeInfo());
   
   // can (and must) be called even if statements is NULL
   statements->parseInit(&signature);
}

void UserClosureVariant::parseInitClosure(const QoreTypeInfo *classTypeInfo, lvar_set_t *vlist) {
   statements->parseInitClosure(&signature, classTypeInfo, vlist);
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

      SimpleRefHolder<QoreTreeNode> t(new QoreTreeNode(doPartialEval(tree->left, is_self_ref, xsink), tree->op, nn ? nn.release() : nothing()));
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
