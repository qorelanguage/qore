/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  VarRefNode.h

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

#ifndef _QORE_VARREFNODE_H
#define _QORE_VARREFNODE_H

#include <qore/intern/FunctionCallNode.h>

class GlobalVarRefNewObjectNode;
class LocalVar;
class Var;

class VarRefNode : public ParseNode {
   friend class VarRefNodeEvalOptionalRefHolder;

protected:
   char *name;
   qore_var_t type;
   bool new_decl;  // is this a new variable declaration
   bool explicit_scope; // scope was explicitly provided

   DLLLOCAL ~VarRefNode();

   // evalImpl(): return value requires a deref(xsink)
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const;
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const;

   DLLLOCAL void resolve(const QoreTypeInfo *typeInfo);
   DLLLOCAL AbstractQoreNode *parseInitIntern(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *typeInfo, bool is_new = false);
   DLLLOCAL GlobalVarRefNewObjectNode *globalMakeNewCall(AbstractQoreNode *args);

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      if (type == VT_LOCAL || type == VT_CLOSURE)
         return ref.id->getTypeInfo();
      if (type == VT_GLOBAL)
         return ref.var->getTypeInfo();
      return 0;
   }

   DLLLOCAL virtual const QoreTypeInfo *parseGetTypeInfo() const {
      if (type == VT_LOCAL || type == VT_CLOSURE)
         return ref.id->getTypeInfo();
      if (type == VT_GLOBAL)
         return ref.var->parseGetTypeInfo();
      return 0;
   }

public:
   union var_u {
      LocalVar *id;   // for local variables
      Var *var;       // for global variables
   } ref;

   // takes over memory for "n"
   DLLLOCAL VarRefNode(char *n, qore_var_t t, bool n_has_effect = false) : ParseNode(NT_VARREF, true, n_has_effect), name(n), type(t), new_decl(t == VT_LOCAL), explicit_scope(false) {
      if (type == VT_LOCAL)
         ref.id = 0;
      assert(type != VT_GLOBAL);
   }
   DLLLOCAL VarRefNode(char *n, Var *n_var, bool n_has_effect = false, bool n_new_decl = true) : ParseNode(NT_VARREF, true, n_has_effect), name(n), type(VT_GLOBAL), new_decl(n_new_decl), explicit_scope(false) {
      ref.var = n_var;
   }

   DLLLOCAL VarRefNode(char *n, LocalVar *n_id, bool in_closure) : ParseNode(NT_VARREF, true, false), name(n), new_decl(false), explicit_scope(false) {
      ref.id = n_id;
      if (in_closure) {
	 n_id->setClosureUse();
	 type = VT_CLOSURE;
      }
      else
         type = VT_LOCAL;
   }      
   
   DLLLOCAL virtual int getAsString(QoreString &str, int foff, ExceptionSink *xsink) const;
   DLLLOCAL virtual QoreString *getAsString(bool &del, int foff, ExceptionSink *xsink) const;

   // returns the type name as a c string
   DLLLOCAL virtual const char *getTypeName() const;

   DLLLOCAL virtual bool stayInTree() const {
      return !(type == VT_GLOBAL);
   }

   DLLLOCAL virtual bool parseIsDecl() const { return type != VT_UNRESOLVED; }
   DLLLOCAL virtual bool isDecl() const { return false; }
   DLLLOCAL bool explicitScope() const { return explicit_scope; }
   DLLLOCAL void setExplicitScope() { explicit_scope = true; }

   // will only be called on *VarRefNewObjectNode objects, but this is their common class
   DLLLOCAL virtual const char *getNewObjectClassName() const {
      assert(false);
      return 0;
   }

   // for checking for new object calls
   DLLLOCAL virtual AbstractQoreNode *makeNewCall(AbstractQoreNode *args);

   DLLLOCAL bool isGlobalDecl() const { return new_decl; }

   DLLLOCAL void setValue(AbstractQoreNode *val, ExceptionSink *xsink);
   DLLLOCAL AbstractQoreNode **getValuePtr(AutoVLock *vl, const QoreTypeInfo *&typeInfo, ObjMap &omap, ExceptionSink *xsink) const;
   DLLLOCAL qore_var_t getType() const { return type; }
   DLLLOCAL const char *getName() const { return name; }
   // called when a list of variables is declared
   DLLLOCAL void makeLocal() {
      assert(type != VT_GLOBAL); 
      type = VT_LOCAL; 
      new_decl = true;
      ref.id = 0;
   }
   // called when a list of variables is declared
   DLLLOCAL virtual void makeGlobal() {
      assert(type != VT_GLOBAL);
      assert(type == VT_UNRESOLVED || !ref.id);
      type = VT_GLOBAL;
      ref.var = getProgram()->addGlobalVarDef(name, 0);      
      new_decl = true;
   }

   // takes the name - caller owns the memory
   DLLLOCAL char *takeName();
};

class GlobalVarRefNode : public VarRefNode {
protected:
public:
   DLLLOCAL GlobalVarRefNode(char *n, Var *v) : VarRefNode(n, v, false, false) {
      explicit_scope = true;
   }
};

class VarRefDeclNode : public VarRefNode {
protected:
   QoreParseTypeInfo *parseTypeInfo;
   const QoreTypeInfo *typeInfo;

   DLLLOCAL VarRefDeclNode(char *n, qore_var_t t, const QoreTypeInfo *n_typeInfo, QoreParseTypeInfo *n_parseTypeInfo, bool n_has_effect) : VarRefNode(n, t, n_has_effect), parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo=%p %s type=%d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
   }

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo);

public:
   DLLLOCAL VarRefDeclNode(char *n, qore_var_t t, const QoreTypeInfo *n_typeInfo) : VarRefNode(n, t), parseTypeInfo(0), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo=%p %s type=%d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
   }

   // takes over ownership of class_name
   DLLLOCAL VarRefDeclNode(char *n, qore_var_t t, char *class_name) : VarRefNode(n, t), parseTypeInfo(new QoreParseTypeInfo(class_name)), typeInfo(0) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() this=%p typeInfo=%p %s type=%d class=%s\n", this, typeInfo, n, type, class_name);
   }

   // takes over ownership of QoreParseTypeInfo ptr
   DLLLOCAL VarRefDeclNode(char *n, qore_var_t t, QoreParseTypeInfo *n_parseTypeInfo) : VarRefNode(n, t), parseTypeInfo(n_parseTypeInfo), typeInfo(0) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() this=%p typeInfo=%p %s type=%d class=%s\n", this, typeInfo, n, type, class_name);
   }

   // takes over ownership of QoreParseTypeInfo ptr
   DLLLOCAL VarRefDeclNode(char *n, qore_var_t t, const QoreTypeInfo *n_typeInfo, QoreParseTypeInfo *n_parseTypeInfo) : VarRefNode(n, t), parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo=%p %s type=%d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
   }

   DLLLOCAL ~VarRefDeclNode() {
      delete parseTypeInfo;
   }
   DLLLOCAL virtual bool parseIsDecl() const { 
      return true;
   }
   DLLLOCAL virtual bool isDecl() const {
      return true;
   }

   // for checking for new object calls
   DLLLOCAL virtual AbstractQoreNode *makeNewCall(AbstractQoreNode *args);

   DLLLOCAL QoreParseTypeInfo *takeParseTypeInfo() { 
      QoreParseTypeInfo *ti = parseTypeInfo;
      parseTypeInfo = 0;
      return ti;
   }
   DLLLOCAL QoreParseTypeInfo *getParseTypeInfo() {
      return parseTypeInfo;
   }
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const {
      assert(!parseTypeInfo);
      return typeInfo;
   }
   DLLLOCAL virtual void makeGlobal() {
      assert(type == VT_UNRESOLVED); 
      type = VT_GLOBAL;
      if (parseTypeInfo)
         ref.var = getProgram()->addGlobalVarDef(name, takeParseTypeInfo());
      else
         ref.var = getProgram()->addResolvedGlobalVarDef(name, typeInfo);
      new_decl = true;
   }

   void parseInitCommon(LocalVar *oflag, int pflag, int &lvids, bool is_new = false);
};

class VarRefFunctionCallBase : public FunctionCallBase {
protected:

public:
   DLLLOCAL VarRefFunctionCallBase(QoreListNode *n_args) : FunctionCallBase(n_args) {
   }
   DLLLOCAL void parseInitConstructorCall(LocalVar *oflag, int pflag, int &lvids, const QoreClass *qc);
};

class LocalVarRefNewObjectNode : public VarRefDeclNode, public VarRefFunctionCallBase {
protected:
   // evalImpl(): return value requires a deref(xsink)
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;
   
   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo);

public:
   DLLLOCAL LocalVarRefNewObjectNode(char *n, const QoreTypeInfo *n_typeInfo, QoreParseTypeInfo *n_parseTypeInfo, QoreListNode *n_args) : VarRefDeclNode(n, VT_LOCAL, n_typeInfo, n_parseTypeInfo, true), VarRefFunctionCallBase(n_args) {
   }
   /*
   DLLLOCAL virtual ~LocalVarRefNewObjectNode() {
      //printd(0, "VarRefNewObjectNode::~VarRefNewObjectNode() this=%p (%s)\n", this, getName());
   }
   */

   DLLLOCAL virtual bool stayInTree() const {
      return true;
   }

   // will only be called on *VarRefNewObjectNode objects, but this is their common class
   DLLLOCAL virtual const char *getNewObjectClassName() const {
      if (typeInfo) {
         assert(typeInfo->getUniqueReturnClass());
         return typeInfo->getUniqueReturnClass()->getName();
      }
      return parseTypeInfo->cscope->getIdentifier();
   }
};

class GlobalVarRefNewObjectNode : public VarRefNode, public VarRefFunctionCallBase {
protected:
   // evalImpl(): return value requires a deref(xsink)
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const;

   //! optionally evaluates the argument
   /** return value requires a deref(xsink) if needs_deref is true
       @see AbstractQoreNode::eval()
   */
   DLLLOCAL AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const;

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&outTypeInfo);

public:
   // for making a reference to a global variable
   DLLLOCAL GlobalVarRefNewObjectNode(char *n, Var *var, QoreListNode *n_args) : VarRefNode(n, var, true), VarRefFunctionCallBase(n_args) {
   }

   DLLLOCAL virtual bool stayInTree() const {
      return true;
   }

   // will only be called on *VarRefNewObjectNode objects, but this is their common class
   DLLLOCAL virtual const char *getNewObjectClassName() const {
      return ref.var->getClassName();
   }
};

class VarRefNodeEvalOptionalRefHolder {
private:
   AbstractQoreNode *val;
   ExceptionSink *xsink;
   bool needs_deref;

   DLLLOCAL void discard_intern() {
      if (needs_deref && val)
	 val->deref(xsink);
   }

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL VarRefNodeEvalOptionalRefHolder(const VarRefNodeEvalOptionalRefHolder&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   DLLLOCAL VarRefNodeEvalOptionalRefHolder& operator=(const VarRefNodeEvalOptionalRefHolder&);

   //! this function is not implemented; it is here as a private function in order to prohibit it from being used
   /** this function is not implemented in order to require objects of this type to be allocated on the stack.
    */
   DLLLOCAL void *operator new(size_t);

public:
   //! constructor with a value that will call the class' eval(needs_deref) method
   DLLLOCAL VarRefNodeEvalOptionalRefHolder(const VarRefNode *exp, ExceptionSink *n_xsink) : xsink(n_xsink) {
      if (exp)
	 val = exp->VarRefNode::evalImpl(needs_deref, xsink);
      else {
	 val = 0;
	 needs_deref = false;
      }	    
   }

   //! discards any temporary value evaluated by the constructor or assigned by "assign()"
   DLLLOCAL ~VarRefNodeEvalOptionalRefHolder() {
      discard_intern();
   }
      
   //! discards any temporary value evaluated by the constructor or assigned by "assign()"
   DLLLOCAL void discard() {
      discard_intern();
      needs_deref = false;
      val = 0;
   }

   //! assigns a new value to this holder object
   DLLLOCAL void assign(bool n_needs_deref, AbstractQoreNode *n_val) {
      discard_intern();
      needs_deref = n_needs_deref;
      val = n_val;
   }

   //! returns a referenced value - the caller will own the reference
   DLLLOCAL AbstractQoreNode *getReferencedValue() {
      if (needs_deref)
	 needs_deref = false;
      else if (val)
	 val->ref();
      return val;
   }

   //! returns the object being managed
   DLLLOCAL const AbstractQoreNode *operator->() const { return val; }

   //! returns the object being managed
   DLLLOCAL const AbstractQoreNode *operator*() const { return val; }

   //! returns true if a value is being held
   DLLLOCAL operator bool() const { return val != 0; }
};

#endif
