/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ParseNode.h
  
  Qore Programming Language

  Copyright 2003 - 2013 David Nichols

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

#ifndef _QORE_PARSENODE_H

#define _QORE_PARSENODE_H

class ParseNode : public SimpleQoreNode {
private:
   // not implemented
   DLLLOCAL ParseNode& operator=(const ParseNode&);

protected:
   //! if the node has an effect when evaluated (changes something)
   bool effect : 1;

   //! if the return value is ignored
   bool ref_rv : 1;

   //! if the node has undergone "parse initialization"
   bool parse_init : 1;

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) = 0;
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const = 0;

public:
   DLLLOCAL ParseNode(qore_type_t t, bool n_needs_eval = true) : SimpleQoreNode(t, false, n_needs_eval), effect(n_needs_eval), ref_rv(true), parse_init(false) {
   }
   DLLLOCAL ParseNode(qore_type_t t, bool n_needs_eval, bool n_effect) : SimpleQoreNode(t, false, n_needs_eval), effect(n_effect), ref_rv(true), parse_init(false) {
   }
   DLLLOCAL ParseNode(const ParseNode &old) : SimpleQoreNode(old.type, false, old.needs_eval_flag), effect(old.effect), ref_rv(old.ref_rv), parse_init(false) {
   }
   // parse types should never be copied
   DLLLOCAL virtual AbstractQoreNode *realCopy() const {
      assert(false);
      return 0;
   }
   DLLLOCAL virtual bool is_equal_soft(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      assert(false);
      return false;
   }
   DLLLOCAL virtual bool is_equal_hard(const AbstractQoreNode *v, ExceptionSink *xsink) const {
      assert(false);
      return false;
   }
   DLLLOCAL void set_effect(bool n_effect) {
      effect = n_effect;
   }
   DLLLOCAL bool has_effect() const {
      return effect;
   }
   DLLLOCAL void ignore_rv() {
      ref_rv = false;
   }
   DLLLOCAL bool need_rv() const {
      return ref_rv;
   }

   DLLLOCAL virtual AbstractQoreNode *parseInit(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
      if (parse_init) {
         typeInfo = getTypeInfo();
         return this;
      }
      parse_init = true;
      return parseInitImpl(oflag, pflag, lvids, typeInfo);
   }
};

// these objects will never be copied or referenced therefore they can have 
// public destructors - the deref() functions just call "delete this;"
class ParseNoEvalNode : public ParseNode {
private:
   // not implemented
   DLLLOCAL ParseNoEvalNode& operator=(const ParseNoEvalNode&);

   DLLLOCAL virtual AbstractQoreNode *parseInitImpl(LocalVar *oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) = 0;
   DLLLOCAL virtual const QoreTypeInfo *getTypeInfo() const = 0;

protected:
   DLLLOCAL virtual int64 bigIntEvalImpl(ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }
   DLLLOCAL virtual int integerEvalImpl(ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }
   DLLLOCAL virtual bool boolEvalImpl(ExceptionSink *xsink) const {
      assert(false);
      return false;
   }
   DLLLOCAL virtual double floatEvalImpl(ExceptionSink *xsink) const {
      assert(false);
      return 0.0;
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }
   DLLLOCAL virtual AbstractQoreNode *evalImpl(bool &needs_deref, ExceptionSink *xsink) const {
      assert(false);
      return 0;
   }

public:
   DLLLOCAL ParseNoEvalNode(qore_type_t t) : ParseNode(t, false) {
   }

   DLLLOCAL ParseNoEvalNode(const ParseNoEvalNode &old) : ParseNode(old) {
   }
};

#endif
