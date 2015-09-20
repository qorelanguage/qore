/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  VarRefNode.h

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

#ifndef _QORE_VARREFNODE_H
#define _QORE_VARREFNODE_H

#include <qore/intern/FunctionCallNode.h>

class VarRefNewObjectNode;
class LocalVar;
class LocalVarValue;
class Var;
struct ClosureVarValue;

class VarRefNode : public ParseNode {
protected:
   QoreProgramLocation loc;
   NamedScope name;
   qore_var_t type : 4;
   bool new_decl : 1;       // is this a new variable declaration
   bool explicit_scope : 1; // scope was explicitly provided

   DLLLOCAL ~VarRefNode() {
      //printd(5, "VarRefNode::~VarRefNode() deleting variable reference %p %s\n", this, name.ostr ? name.ostr : "<taken>");
      assert(type != VT_IMMEDIATE || !ref.cvv);
      assert(type != VT_IMMEDIATE || !ref.cvv);
   }

   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink) {
      if (type == VT_IMMEDIATE) {
         assert(false);
         ref.cvv->deref(xsink);
#ifdef DEBUG
         ref.cvv = 0;
#endif
      }
      return true;
   }

   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   DLLLOCAL void resolve(const QoreTypeInfo* typeInfo);
   DLLLOCAL AbstractQoreNode* parseInitIntern(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo* typeInfo, bool is_new = false);
   DLLLOCAL VarRefNewObjectNode* globalMakeNewCall(AbstractQoreNode* args);

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL virtual const QoreTypeInfo* parseGetTypeInfo() const {
      if (type == VT_LOCAL || type == VT_CLOSURE || type == VT_LOCAL_TS)
         return ref.id->parseGetTypeInfo();
      if (type == VT_GLOBAL)
         return ref.var->parseGetTypeInfo();
      return 0;
   }

   DLLLOCAL void setThreadSafeIntern() {
      ref.id->setClosureUse();
      type = VT_LOCAL_TS;
   }

   DLLLOCAL void setClosureIntern() {
      ref.id->setClosureUse();
      type = VT_CLOSURE;
   }

   DLLLOCAL VarRefNode(char* n, ClosureVarValue* cvv) : ParseNode(NT_VARREF, true, false), loc(RunTimeLocation), name(n), type(VT_IMMEDIATE), new_decl(false), explicit_scope(false) {
      ref.cvv = cvv;
      cvv->ref();
   }

   DLLLOCAL VarRefNode(const QoreProgramLocation& nloc, char* n, qore_var_t t, bool n_has_effect = false) : ParseNode(NT_VARREF, true, n_has_effect), loc(nloc), name(n), type(t), new_decl(t == VT_LOCAL), explicit_scope(false) {
      if (type == VT_LOCAL)
         ref.id = 0;
      assert(type != VT_GLOBAL);
   }

   DLLLOCAL VarRefNode(int sl, int el, char* n, qore_var_t t, bool n_has_effect = false) : ParseNode(NT_VARREF, true, n_has_effect), loc(sl, el), name(n), type(t), new_decl(t == VT_LOCAL), explicit_scope(false) {
      if (type == VT_LOCAL)
         ref.id = 0;
      assert(type != VT_GLOBAL);
   }

   DLLLOCAL VarRefNode(char* n, Var* n_var, bool n_has_effect = false, bool n_new_decl = true) : ParseNode(NT_VARREF, true, n_has_effect), loc(ParseLocation), name(n), type(VT_GLOBAL), new_decl(n_new_decl), explicit_scope(false) {
      ref.var = n_var;
   }

public:
   union var_u {
      LocalVar* id;         // for local variables
      Var* var;             // for global variables
      ClosureVarValue* cvv; // for immediate values; used with references
   } ref;

   // takes over memory for "n"
   DLLLOCAL VarRefNode(char* n, qore_var_t t, bool n_has_effect = false) : ParseNode(NT_VARREF, true, n_has_effect), loc(ParseLocation), name(n), type(t), new_decl(t == VT_LOCAL), explicit_scope(false) {
      if (type == VT_LOCAL)
         ref.id = 0;
      assert(type != VT_GLOBAL);
   }

   DLLLOCAL VarRefNode(char* n, LocalVar* n_id, bool in_closure) : ParseNode(NT_VARREF, true, false), loc(ParseLocation), name(n), new_decl(false), explicit_scope(false) {
      ref.id = n_id;
      if (in_closure)
         setClosureIntern();
      else
         type = VT_LOCAL;
   }

   DLLLOCAL virtual int getAsString(QoreString& str, int foff, ExceptionSink* xsink) const;
   DLLLOCAL virtual QoreString* getAsString(bool& del, int foff, ExceptionSink* xsink) const;

   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      if (type == VT_LOCAL || type == VT_CLOSURE || type == VT_LOCAL_TS)
         return ref.id->getTypeInfo();
      if (type == VT_GLOBAL)
         return ref.var->getTypeInfo();
      return 0;
   }

   // returns the type name as a c string
   DLLLOCAL virtual const char* getTypeName() const;

   DLLLOCAL virtual bool stayInTree() const {
      return !(type == VT_GLOBAL);
   }

   DLLLOCAL virtual bool parseIsDecl() const { return type != VT_UNRESOLVED; }
   DLLLOCAL virtual bool isDecl() const { return false; }
   DLLLOCAL bool explicitScope() const { return explicit_scope; }
   DLLLOCAL void setExplicitScope() { explicit_scope = true; }

   // will only be called on *VarRefNewObjectNode objects, but this is their common class
   DLLLOCAL virtual const char* getNewObjectClassName() const {
      assert(false);
      return 0;
   }

   // for checking for new object calls
   DLLLOCAL virtual AbstractQoreNode* makeNewCall(AbstractQoreNode* args);

   DLLLOCAL bool isGlobalDecl() const { return new_decl; }

   DLLLOCAL bool isGlobalVar() const { return type == VT_GLOBAL; }

   //DLLLOCAL VarRefNode* isOptimized(const QoreTypeInfo*& typeInfo) const;
   DLLLOCAL int getLValue(LValueHelper& lvh, bool for_remove) const;

   DLLLOCAL bool isRef() const {
      if (type == VT_LOCAL)
         return ref.id->isRef();
      if (type == VT_IMMEDIATE)
         return true;
      assert(type == VT_GLOBAL);
      return ref.var->isRef();
   }

   DLLLOCAL void remove(LValueRemoveHelper& lvrh);

   DLLLOCAL qore_var_t getType() const { return type; }
   DLLLOCAL const char* getName() const { return name.ostr; }
   // called when a list of variables is declared
   DLLLOCAL void makeLocal() {
      assert(type != VT_GLOBAL);
      type = VT_LOCAL;
      new_decl = true;
      ref.id = 0;
   }
   // called when a list of variables is declared
   DLLLOCAL virtual void makeGlobal();

   // takes the name - caller owns the memory
   DLLLOCAL char* takeName() {
      assert(name.ostr);
      return name.takeName();
   }

   DLLLOCAL void setThreadSafe() {
      if (type == VT_LOCAL)
         setThreadSafeIntern();
   }

   DLLLOCAL void setClosure() {
      if (type == VT_LOCAL || type == VT_LOCAL_TS)
         setClosureIntern();
   }

   DLLLOCAL void setPublic() {
      assert(type == VT_GLOBAL);
      ref.var->setPublic();
   }

   DLLLOCAL void parseAssigned() {
      assert(type != VT_IMMEDIATE);
      if (type == VT_LOCAL || type == VT_CLOSURE || type == VT_LOCAL_TS)
         ref.id->parseAssigned();
   }
};

class GlobalVarRefNode : public VarRefNode {
protected:
public:
   DLLLOCAL GlobalVarRefNode(char* n, Var* v) : VarRefNode(n, v, false, false) {
      explicit_scope = true;
   }

   DLLLOCAL GlobalVarRefNode(char* n, const QoreTypeInfo* typeInfo = 0);
   DLLLOCAL GlobalVarRefNode(char* n, QoreParseTypeInfo* parseTypeInfo);

   DLLLOCAL void reg();
};

class VarRefDeclNode : public VarRefNode {
protected:
   QoreParseTypeInfo* parseTypeInfo;
   const QoreTypeInfo* typeInfo;

   DLLLOCAL VarRefDeclNode(const QoreProgramLocation& nloc, char* n, qore_var_t t, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo, bool n_has_effect) :
      VarRefNode(nloc, n, t, n_has_effect), parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo: %p %s type: %d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
   }

   DLLLOCAL VarRefDeclNode(char* n, Var* var, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo) :
      VarRefNode(n, var, true), parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
   }

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& typeInfo);

   DLLLOCAL VarRefDeclNode(char* n, ClosureVarValue* cvv, const QoreTypeInfo* n_typeInfo) : VarRefNode(n, cvv), parseTypeInfo(0), typeInfo(n_typeInfo) {
   }

public:
   DLLLOCAL VarRefDeclNode(int sl, int el, char* n, qore_var_t t, const QoreTypeInfo* n_typeInfo) :
      VarRefNode(sl, el, n, t), parseTypeInfo(0), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo: %p %s type: %d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
   }

   // takes over ownership of class_name
   DLLLOCAL VarRefDeclNode(int sl, int el, char* n, qore_var_t t, char* class_name) :
      VarRefNode(sl, el, n, t), parseTypeInfo(new QoreParseTypeInfo(class_name)), typeInfo(0) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() this: %p typeInfo: %p %s type: %d class: %s\n", this, typeInfo, n, type, class_name);
   }

   // takes over ownership of QoreParseTypeInfo ptr
   DLLLOCAL VarRefDeclNode(int sl, int el, char* n, qore_var_t t, QoreParseTypeInfo* n_parseTypeInfo) :
      VarRefNode(sl, el, n, t), parseTypeInfo(n_parseTypeInfo), typeInfo(0) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() this: %p typeInfo: %p %s type: %d class: %s\n", this, typeInfo, n, type, class_name);
   }

   // takes over ownership of QoreParseTypeInfo ptr
   DLLLOCAL VarRefDeclNode(int sl, int el, char* n, qore_var_t t, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo) :
      VarRefNode(sl, el, n, t), parseTypeInfo(n_parseTypeInfo), typeInfo(n_typeInfo) {
      //printd(5, "VarRefDeclNode::VarRefDeclNode() typeInfo: %p %s type: %d (%s)\n", typeInfo, n, n_qt, getBuiltinTypeName(n_qt));
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
   DLLLOCAL virtual AbstractQoreNode* makeNewCall(AbstractQoreNode* args);

   DLLLOCAL QoreParseTypeInfo* takeParseTypeInfo() {
      QoreParseTypeInfo* ti = parseTypeInfo;
      parseTypeInfo = 0;
      return ti;
   }
   DLLLOCAL QoreParseTypeInfo* getParseTypeInfo() {
      return parseTypeInfo;
   }
   DLLLOCAL virtual const QoreTypeInfo* getTypeInfo() const {
      assert(!parseTypeInfo);
      return typeInfo;
   }
   DLLLOCAL virtual void makeGlobal();

   void parseInitCommon(LocalVar* oflag, int pflag, int& lvids, bool is_new = false);
};

class VarRefImmediateNode : public VarRefDeclNode {
private:
   DLLLOCAL void deref() {
      assert(false);
      if (ROdereference())
         delete this;
   }

protected:
   DLLLOCAL virtual bool derefImpl(ExceptionSink* xsink) {
      //printd(5, "VarRefImmediateNode::derefImpl() this: %p '%s' cvv: %p\n", this, name.ostr, ref.cvv);
      ref.cvv->deref(xsink);
#ifdef DEBUG
      ref.cvv = 0;
#endif
      return true;
   }

public:
   DLLLOCAL VarRefImmediateNode(char* n, ClosureVarValue* cvv, const QoreTypeInfo* n_typeInfo) : VarRefDeclNode(n, cvv, n_typeInfo) {
      //printd(5, "VarRefImmediateNode::VarRefImmediateNode() this: %p '%s' cvv: %p\n", this, name.ostr, cvv);
   }

   DLLLOCAL virtual ~VarRefImmediateNode() {
      //printd(5, "VarRefImmediateNode::~VarRefImmediateNode() this: %p '%s'\n", this, name.ostr);
      assert(!ref.cvv);
   }
};

// special thread-local variables with global scope used to handle module loading errors, created by the %try-module parse directive if module loading fails
class VarRefTryModuleErrorNode : public VarRefDeclNode {
public:
   DLLLOCAL VarRefTryModuleErrorNode(int sl, int el, char* n) : VarRefDeclNode(sl, el, n, VT_LOCAL, hashTypeInfo) {
   }

   DLLLOCAL virtual ~VarRefTryModuleErrorNode() {
   }
};

class VarRefFunctionCallBase : public FunctionCallBase {
protected:

public:
   DLLLOCAL VarRefFunctionCallBase(QoreListNode* n_args) : FunctionCallBase(n_args) {
   }
   DLLLOCAL void parseInitConstructorCall(const QoreProgramLocation& loc, LocalVar* oflag, int pflag, int& lvids, const QoreClass* qc);
};

class VarRefNewObjectNode : public VarRefDeclNode, public VarRefFunctionCallBase {
protected:
   DLLLOCAL virtual QoreValue evalValueImpl(bool& needs_deref, ExceptionSink* xsink) const;

   // initializes during parsing
   DLLLOCAL virtual AbstractQoreNode* parseInitImpl(LocalVar* oflag, int pflag, int& lvids, const QoreTypeInfo*& outTypeInfo);

public:
   DLLLOCAL VarRefNewObjectNode(const QoreProgramLocation& loc, char* n, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo, QoreListNode* n_args, qore_var_t t) :
               VarRefDeclNode(loc, n, t, n_typeInfo, n_parseTypeInfo, true), VarRefFunctionCallBase(n_args) {
   }

   DLLLOCAL VarRefNewObjectNode(char* n, Var* var, QoreListNode* n_args, const QoreTypeInfo* n_typeInfo, QoreParseTypeInfo* n_parseTypeInfo) :
               VarRefDeclNode(n, var, n_typeInfo, n_parseTypeInfo), VarRefFunctionCallBase(n_args) {
   }

   /*
   DLLLOCAL virtual ~VarRefNewObjectNode() {
      //printd(5, "VarRefNewObjectNode::~VarRefNewObjectNode() this: %p (%s)\n", this, getName());
   }
   */

   DLLLOCAL virtual bool stayInTree() const {
      return true;
   }

   // will only be called on *VarRefNewObjectNode objects, but this is their common class
   DLLLOCAL virtual const char* getNewObjectClassName() const {
      if (typeInfo) {
         assert(typeInfo->getUniqueReturnClass());
         return typeInfo->getUniqueReturnClass()->getName();
      }
      return parseTypeInfo->cscope->getIdentifier();
   }
};

#endif
