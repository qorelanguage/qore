/*
  QoreHashNode.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2017 Qore Technologies, s.r.o.

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
#include <qore/minitest.hpp>
#include "qore/intern/QoreHashNodeIntern.h"
#include "qore/intern/QoreParseHashNode.h"
#include "qore/intern/QoreNamespaceIntern.h"
#include "qore/intern/ParserSupport.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/typed_hash_decl_private.h"

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <map>

#ifdef DEBUG_TESTS
#  include "tests/Hash_tests.cpp"
#endif

static const char* qore_hash_type_name = "hash";

void qore_hash_private::merge(const qore_hash_private& h, ExceptionSink* xsink) {
   for (auto& i : h.member_list) {
      setKeyValue(i->key, i->node ? i->node->refSelf() : nullptr, xsink);
   }
}

int qore_hash_private::getLValue(const char* key, LValueHelper& lvh, bool for_remove, ExceptionSink* xsink) {
   const QoreTypeInfo* memTypeInfo = nullptr;

   if (hashdecl) {
      const HashDeclMemberInfo* m = typed_hash_decl_private::get(*hashdecl)->findMember(key);
      if (!m) {
         xsink->raiseException("INVALID-MEMBER", "'%s' is not a registered member of hashdecl '%s'", key, hashdecl->getName());
         lvh.clearPtr();
         return -1;
      }

      memTypeInfo = m->getTypeInfo();
   }
   else if (complexTypeInfo)
      memTypeInfo = QoreTypeInfo::getUniqueReturnComplexHash(complexTypeInfo);

   hm_hm_t::const_iterator i = hm.find(key);
   HashMember* m;
   if (i == hm.end()) {
      if (for_remove)
         return -1;
      m = findCreateMember(key);
   }
   else
      m = (*(i->second));

   lvh.resetPtr(&m->node, memTypeInfo);
   return 0;
}

int qore_hash_private::parseInitHashInitialization(const QoreProgramLocation& loc, LocalVar* oflag, int pflag, int& lvids, QoreListNode* args, const QoreTypeInfo*& argTypeInfo, const AbstractQoreNode*& arg) {
    assert(!lvids);
    assert(!argTypeInfo);

    if (!args || args->empty())
        return -1;

    arg = nullptr;
    if (args->size() > 1) {
        parse_error(loc, "illegal arguments to typed hash initialization; a single hash argument is expected; %d arguments supplied instead", (int)args->size());
        return -1;
    }

    // initialize argument
    AbstractQoreNode** n = args->get_entry_ptr(0);
    (*n) = (*n)->parseInit(oflag, pflag & ~(PF_RETURN_VALUE_IGNORED), lvids, argTypeInfo);
    assert(*n);
    arg = *n;

    if (!QoreTypeInfo::parseReturns(argTypeInfo, NT_HASH)) {
        parse_error(loc, "illegal argument to typed hash initialization; a single hash argument is expected; got type '%s' instead", QoreTypeInfo::getName(argTypeInfo));
        return -1;
    }

    return 0;
}

int qore_hash_private::parseInitComplexHashInitialization(const QoreProgramLocation& loc, LocalVar *oflag, int pflag, QoreListNode* args, const QoreTypeInfo* vti) {

    int lvids = 0;
    const QoreTypeInfo* argTypeInfo = nullptr;
    const AbstractQoreNode* arg;
    if (!parseInitHashInitialization(loc, oflag, pflag, lvids, args, argTypeInfo, arg))
       parseCheckComplexHashInitialization(loc, vti, argTypeInfo, arg, "initialize", true);
    return lvids;
}

void qore_hash_private::parseCheckComplexHashInitialization(const QoreProgramLocation& loc, const QoreTypeInfo* valueTypeInfo, const QoreTypeInfo* argTypeInfo, const AbstractQoreNode* exp, const char* context_action, bool strict_check) {
    const TypedHashDecl* hd = QoreTypeInfo::getUniqueReturnHashDecl(argTypeInfo);
    if (hd)
        typed_hash_decl_private::get(*hd)->parseCheckComplexHashAssignment(loc, valueTypeInfo);
    else {
        const QoreTypeInfo* vti2 = QoreTypeInfo::getUniqueReturnComplexHash(argTypeInfo);
        if (vti2) {
            if (!QoreTypeInfo::parseAccepts(valueTypeInfo, vti2))
                parse_error(loc, "cannot %s 'hash<string, %s>' from a hash typed with incompatible value type '%s'", context_action, QoreTypeInfo::getName(valueTypeInfo),
                QoreTypeInfo::getName(vti2));
        }
        else
            parseCheckTypedAssignment(loc, exp, valueTypeInfo, context_action, strict_check);
    }
}

void qore_hash_private::parseCheckTypedAssignment(const QoreProgramLocation& loc, const AbstractQoreNode* arg, const QoreTypeInfo* vti, const char* context_action, bool strict_check) {
    switch (get_node_type(arg)) {
        case NT_HASH: {
            ConstHashIterator i(reinterpret_cast<const QoreHashNode*>(arg));
            while (i.next()) {
                const QoreTypeInfo* kti = getTypeInfoForValue(i.getValue());
                bool may_not_match = false;
                qore_type_result_e res = QoreTypeInfo::parseAccepts(vti, kti, may_not_match);
                if (res == QTI_IDENT
                    || ((res == QTI_AMBIGUOUS)
                        && (!strict_check || !may_not_match)))
                    continue;
                parse_error(loc, "cannot %s 'hash<string, %s>' from key '%s' of a hash with incompatible value type '%s'", context_action, QoreTypeInfo::getName(vti), i.getKey(), QoreTypeInfo::getName(kti));
            }
            break;
        }
        case NT_PARSE_HASH: {
            const QoreParseHashNode* phn = reinterpret_cast<const QoreParseHashNode*>(arg);
            const QoreParseHashNode::nvec_t& keys = phn->getKeys();
            const QoreParseHashNode::tvec_t& vtypes = phn->getValueTypes();
            assert(keys.size() == vtypes.size());

            for (unsigned i = 0; i < vtypes.size(); ++i) {
                const QoreTypeInfo* vti2 = vtypes[i];
                bool may_not_match = false;
                qore_type_result_e res = QoreTypeInfo::parseAccepts(vti, vti2, may_not_match);
                if (res == QTI_IDENT
                    || ((res == QTI_AMBIGUOUS)
                        && (!strict_check || !may_not_match)))
                    continue;
                const AbstractQoreNode* kn = keys[i];
                const QoreStringNode* key = get_node_type(kn) == NT_STRING ? reinterpret_cast<const QoreStringNode*>(kn) : nullptr;
                if (key)
                    parse_error(loc, "cannot %s 'hash<string, %s>' from key '%s' of a hash with incompatible value type '%s'", context_action, QoreTypeInfo::getName(vti), key->c_str(), QoreTypeInfo::getName(vti2));
                else
                    parse_error(loc, "cannot %s 'hash<string, %s>' from element value %d/%d of a hash with incompatible value type '%s'", context_action, QoreTypeInfo::getName(vti), (int)(i + 1), (int)vtypes.size(), QoreTypeInfo::getName(vti2));
            }
            break;
        }
        default:
            break;
    }
}

QoreHashNode* qore_hash_private::newComplexHash(const QoreTypeInfo* typeInfo, const QoreListNode* args, ExceptionSink* xsink) {
    assert(!args || args->empty() || args->size() == 1);

    QoreHashNode* init = nullptr;

    if (args && !args->empty()) {
        QoreNodeEvalOptionalRefHolder h(args->retrieve_entry(0), xsink);

        if (get_node_type(*h) != NT_HASH) {
            xsink->raiseException("HASH-INIT-ERROR", "typed hash initializer value must be a hash; got type '%s' instead", get_type_name(*h));
            return nullptr;
        }

        init = static_cast<QoreHashNode*>(h.getReferencedValue());
    }

    return newComplexHashFromHash(typeInfo, init, xsink);
}

QoreHashNode* qore_hash_private::newComplexHashFromHash(const QoreTypeInfo* typeInfo, QoreHashNode* init_hash, ExceptionSink* xsink) {
    ReferenceHolder<QoreHashNode> init(init_hash, xsink);

    // check member types
    if (init) {
        if (!init->is_unique())
            init = init->copy();
        HashIterator i(*init);
        const QoreTypeInfo* vti = QoreTypeInfo::getUniqueReturnComplexHash(typeInfo);
        assert(vti);
        while (i.next()) {
            // check types
            HashAssignmentHelper hah(i);
            QoreValue qv(hash_assignment_priv::get(hah)->swap(nullptr));
            QoreTypeInfo::acceptInputMember(vti, i.getKey(), qv, xsink);
            hash_assignment_priv::get(hah)->swap(qv.takeNode());
            if (*xsink)
                return nullptr;
        }
    }
    else
        init = new QoreHashNode;
    // mark new hash with new type
    assert(init->is_unique());
    init->priv->complexTypeInfo = typeInfo;
    return init.release();
}

QoreHashNode::QoreHashNode(bool ne) : AbstractQoreNode(NT_HASH, !ne, ne), priv(new qore_hash_private) {
}

QoreHashNode::QoreHashNode() : AbstractQoreNode(NT_HASH, true, false), priv(new qore_hash_private) {
}

QoreHashNode::~QoreHashNode() {
   delete priv;
}

AbstractQoreNode* QoreHashNode::realCopy() const {
   return copy();
}

// performs a lexical compare, return -1, 0, or 1 if the "this" value is less than, equal, or greater than
// the "val" passed
//DLLLOCAL virtual int compare(const AbstractQoreNode* val) const;
// the type passed must always be equal to the current type
bool QoreHashNode::is_equal_soft(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareSoft(reinterpret_cast<const QoreHashNode*>(v), xsink);
}

bool QoreHashNode::is_equal_hard(const AbstractQoreNode* v, ExceptionSink* xsink) const {
   if (!v || v->getType() != NT_HASH)
      return false;

   return !compareHard(reinterpret_cast<const QoreHashNode*>(v), xsink);
}

const char* QoreHashNode::getTypeName() const {
   return qore_hash_type_name;
}

const char* QoreHashNode::getFirstKey() const  {
   return priv->getFirstKey();
}

const char* QoreHashNode::getLastKey() const {
   return priv->getLastKey();
}

// deprecated
AbstractQoreNode** QoreHashNode::getKeyValuePtr(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return priv->getKeyValuePtr(tmp->getBuffer());
}

// deprecated
AbstractQoreNode** QoreHashNode::getKeyValuePtr(const char* key) {
   return priv->getKeyValuePtr(key);
}

int64 QoreHashNode::getKeyAsBigInt(const char* key, bool &found) const {
   return priv->getKeyAsBigInt(key, found);
}

bool QoreHashNode::getKeyAsBool(const char* key, bool &found) const {
   return priv->getKeyAsBool(key, found);
}

void QoreHashNode::deleteKey(const QoreString* key, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv->deleteKey(tmp->getBuffer(), xsink);
}

void QoreHashNode::removeKey(const QoreString* key, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv->removeKey(tmp->getBuffer(), xsink);
}

AbstractQoreNode* QoreHashNode::takeKeyValue(const QoreString* key, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return priv->takeKeyValue(tmp->getBuffer());
}

AbstractQoreNode* QoreHashNode::getKeyValueExistence(const QoreString* key, bool &exists, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getKeyValueExistence(tmp->getBuffer(), exists);
}

const AbstractQoreNode* QoreHashNode::getKeyValueExistence(const QoreString* key, bool &exists, ExceptionSink* xsink) const {
   return const_cast<QoreHashNode*>(this)->getKeyValueExistence(key, exists, xsink);
}

void QoreHashNode::setKeyValue(const QoreString* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (xsink && *xsink) {
      if (val)
	 val->deref(xsink);
      return;
   }

   setKeyValue(tmp->getBuffer(), val, xsink);
}

void QoreHashNode::setKeyValue(const QoreString& key, AbstractQoreNode* val, ExceptionSink* xsink) {
   setKeyValue(&key, val, xsink);
}

void QoreHashNode::setKeyValue(const char* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   hash_assignment_priv ha(*priv, key);
   ha.assign(val, xsink);
}

AbstractQoreNode* QoreHashNode::swapKeyValue(const QoreString* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink) {
      if (val)
	 val->deref(xsink);
      return 0;
   }

   hash_assignment_priv ha(*priv, tmp->getBuffer());
   return ha.swap(val);
}

AbstractQoreNode* QoreHashNode::swapKeyValue(const char* key, AbstractQoreNode* val) {
   //printd(0, "QoreHashNode::swapKeyValue() this=%p key=%s val=%p (%s) deprecated API called\n", this, key, val, get_node_type(val));
   //assert(false);
   hash_assignment_priv ha(*priv, key);
   return ha.swap(val);
}

AbstractQoreNode* QoreHashNode::swapKeyValue(const char* key, AbstractQoreNode* val, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   hash_assignment_priv ha(*priv, key);
   return ha.swap(val);
}

// deprecated
AbstractQoreNode** QoreHashNode::getExistingValuePtr(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getExistingValuePtr(tmp->getBuffer());
}

AbstractQoreNode* QoreHashNode::getKeyValue(const QoreString* key, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getKeyValue(tmp->getBuffer());
}

AbstractQoreNode* QoreHashNode::getKeyValue(const QoreString& key, ExceptionSink* xsink) {
   TempEncodingHelper tmp(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return 0;

   return getKeyValue(tmp->getBuffer());
}

const AbstractQoreNode* QoreHashNode::getKeyValue(const QoreString* key, ExceptionSink* xsink) const {
   return const_cast<QoreHashNode*>(this)->getKeyValue(key, xsink);
}

// retrieve keys in order they were inserted
QoreListNode* QoreHashNode::getKeys() const {
   return priv->getKeys();
}

// adds all elements (and references them) from the hash passed, leaves the
// hash passed alone
// order is maintained
void QoreHashNode::merge(const class QoreHashNode* h, ExceptionSink* xsink) {
   priv->merge(*h->priv, xsink);
}

// returns the same order
QoreHashNode* QoreHashNode::copy() const {
   return priv->copy();
}

QoreHashNode* QoreHashNode::hashRefSelf() const {
   ref();
   return const_cast<QoreHashNode*>(this);
}

// returns a hash with the same order
AbstractQoreNode* QoreHashNode::evalImpl(ExceptionSink* xsink) const {
   return priv->evalImpl(xsink);
}

// returns a hash with the same order
AbstractQoreNode* QoreHashNode::evalImpl(bool &needs_deref, ExceptionSink* xsink) const {
   if (value) {
      needs_deref = false;
      return const_cast<QoreHashNode*>(this);
   }

   needs_deref = true;
   return QoreHashNode::evalImpl(xsink);
}

int64 QoreHashNode::bigIntEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

int QoreHashNode::integerEvalImpl(ExceptionSink* xsink) const {
   return 0;
}

bool QoreHashNode::boolEvalImpl(ExceptionSink* xsink) const {
   return false;
}

double QoreHashNode::floatEvalImpl(ExceptionSink* xsink) const {
   return 0.0;
}

AbstractQoreNode* QoreHashNode::evalKeyValue(const QoreString* key, ExceptionSink* xsink) const {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return nullptr;

   hm_hm_t::const_iterator i = priv->hm.find(k->getBuffer());

   if (i != priv->hm.end() && (*i->second)->node)
      return (*i->second)->node->refSelf();

   return nullptr;
}

AbstractQoreNode* QoreHashNode::getReferencedKeyValue(const char* key) const {
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end() && (*i->second)->node)
      return (*i->second)->node->refSelf();

   return nullptr;
}

AbstractQoreNode* QoreHashNode::getReferencedKeyValue(const char* key, bool &exists) const {
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end()) {
      exists = true;
      if ((*i->second)->node)
         return (*i->second)->node->refSelf();

      return nullptr;
   }
   exists = false;
   return nullptr;
}

AbstractQoreNode* QoreHashNode::getKeyValue(const char* key) {
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end())
      return (*i->second)->node;

   return 0;
}

const AbstractQoreNode* QoreHashNode::getKeyValue(const char* key) const {
   return const_cast<QoreHashNode*>(this)->getKeyValue(key);
}

AbstractQoreNode* QoreHashNode::getKeyValueExistence(const char* key, bool &exists) {
   assert(key);

   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end()) {
      exists = true;
      return (*i->second)->node;
   }

   exists = false;
   return 0;
}

const AbstractQoreNode* QoreHashNode::getKeyValueExistence(const char* key, bool &exists) const {
   return const_cast<QoreHashNode*>(this)->getKeyValueExistence(key, exists);
}

// does a "soft" compare (values of different types are converted if necessary and then compared)
// 0 = equal, 1 = not equal
bool QoreHashNode::compareSoft(const QoreHashNode* h, ExceptionSink* xsink) const {
   if (h->priv->size() != priv->size())
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->priv->hm.find(hi.getKey());
      if (j == h->priv->hm.end())
         return 1;

      if (q_compare_soft(hi.getValue(), (*j->second)->node, xsink))
         return 1;
   }
   return 0;
}

// does a "hard" compare (types must be exactly the same)
// 0 = equal, 1 = not equal
bool QoreHashNode::compareHard(const QoreHashNode* h, ExceptionSink* xsink) const {
   if (h->priv->size() != priv->size())
      return 1;

   ConstHashIterator hi(this);
   while (hi.next()) {
      hm_hm_t::const_iterator j = h->priv->hm.find(hi.getKey());
      if (j == h->priv->hm.end())
         return 1;

      if (::compareHard(hi.getValue(), (*j->second)->node, xsink))
         return 1;
   }
   return 0;
}

// deprecated
AbstractQoreNode** QoreHashNode::getExistingValuePtr(const char* key) {
   hm_hm_t::const_iterator i = priv->hm.find(key);

   if (i != priv->hm.end())
      return &(*i->second)->node;

   return nullptr;
}

bool QoreHashNode::derefImpl(ExceptionSink* xsink) {
   return priv->derefImpl(xsink);
}

void QoreHashNode::clear(ExceptionSink* xsink, bool reverse) {
   assert(is_unique());
   priv->clear(xsink, reverse);
}

void QoreHashNode::deleteKey(const char* key, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   priv->deleteKey(key, xsink);
}

void QoreHashNode::removeKey(const char* key, ExceptionSink* xsink) {
   assert(reference_count() == 1);
   return priv->removeKey(key, xsink);
}

AbstractQoreNode* QoreHashNode::takeKeyValue(const char* key) {
   assert(reference_count() == 1);
   return priv->takeKeyValue(key);
}

qore_size_t QoreHashNode::size() const {
   return priv->size();
}

bool QoreHashNode::empty() const {
   return priv->empty();
}

bool QoreHashNode::existsKey(const char* key) const {
   return priv->existsKey(key);
}

bool QoreHashNode::existsKeyValue(const char* key) const {
   return priv->existsKeyValue(key);
}

void QoreHashNode::clearNeedsEval() {
   value = true;
   needs_eval_flag = false;
}

void QoreHashNode::setNeedsEval() {
   value = false;
   needs_eval_flag = true;
}

int QoreHashNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   QoreContainerHelper cch(this);
   if (!cch) {
      str.sprintf("{ERROR: recursive reference to hash %p}", this);
      return 0;
   }

   if (foff == FMT_YAML_SHORT) {
      str.concat('{');
      ConstHashIterator hi(this);
      while (hi.next()) {
	 str.sprintf("%s: ", hi.getKey());
	 const AbstractQoreNode* n = hi.getValue();
	 if (!n) n = &Nothing;
	 if (n->getAsString(str, foff, xsink))
	    return -1;
	 if (!hi.last())
	    str.concat(", ");
      }
      str.concat('}');
      return 0;
   }

   if (!size()) {
      str.concat(&EmptyHashString);
      return 0;
   }
   str.concat("hash: (");

   if (foff != FMT_NONE) {
      qore_size_t elements = size();
      str.sprintf("%lu member%s)\n", elements, elements == 1 ? "" : "s");
   }

   ConstHashIterator hi(this);
   while (hi.next()) {
      if (foff != FMT_NONE)
         str.addch(' ', foff + 2);

      str.sprintf("%s : ", hi.getKey());

      const AbstractQoreNode* n = hi.getValue();
      if (!n) n = &Nothing;
      if (n->getAsString(str, foff != FMT_NONE ? foff + 2 : foff, xsink))
         return -1;

      if (!hi.last()) {
         if (foff != FMT_NONE)
            str.concat('\n');
         else
            str.concat(", ");
      }
   }

   if (foff == FMT_NONE)
      str.concat(')');

   return 0;
}

QoreString* QoreHashNode::getAsString(bool &del, int foff, ExceptionSink* xsink) const {
   del = false;
   qore_size_t elements = size();
   if (!elements && foff != FMT_YAML_SHORT)
      return &EmptyHashString;

   TempString rv(new QoreString);
   if (getAsString(*(*rv), foff, xsink))
      return 0;

   del = true;
   return rv.release();
}

QoreHashNode* QoreHashNode::getSlice(const QoreListNode* value_list, ExceptionSink* xsink) const {
   ReferenceHolder<QoreHashNode> rv(new QoreHashNode, xsink);

   ConstListIterator li(value_list);
   while (li.next()) {
      QoreStringValueHelper key(li.getValue(), QCS_DEFAULT, xsink);
      if (*xsink)
	 return 0;

      bool exists;
      const AbstractQoreNode* v = getKeyValueExistence(key->getBuffer(), exists);
      if (!exists)
	 continue;
      rv->setKeyValue(key->getBuffer(), v ? v->refSelf() : 0, xsink);
      if (*xsink)
	 return 0;
   }
   return rv.release();
}

AbstractQoreNode* QoreHashNode::parseInit(LocalVar* oflag, int pflag, int &lvids, const QoreTypeInfo *&typeInfo) {
   typeInfo = hashTypeInfo;
   return this;
}

bool QoreHashNode::getAsBoolImpl() const {
   // check if we should do perl-style boolean evaluation
   if (runtime_check_parse_option(PO_STRICT_BOOLEAN_EVAL))
      return false;
   return !empty();
}

const TypedHashDecl* QoreHashNode::getHashDecl() const {
   return priv->getHashDecl();
}

const QoreTypeInfo* QoreHashNode::getValueTypeInfo() const {
   return priv->getValueTypeInfo();
}

const QoreTypeInfo* QoreHashNode::getTypeInfo() const {
   return priv->getTypeInfo();
}

HashIterator::HashIterator(QoreHashNode* qh) : h(qh), priv(new qhi_priv()) {
}

HashIterator::HashIterator(QoreHashNode& qh) : h(&qh), priv(new qhi_priv()) {
}

HashIterator::~HashIterator() {
   delete priv;
}

QoreHashNode* HashIterator::getHash() const {
   return h;
}

AbstractQoreNode* HashIterator::getReferencedValue() const {
   return !priv->valid() || !(*(priv->i))->node ? 0 : (*(priv->i))->node->refSelf();
}

QoreString* HashIterator::getKeyString() const {
   return !priv->valid() ? 0 : new QoreString((*(priv->i))->key);
}

bool HashIterator::next() {
   return h ? priv->next(h->priv->member_list) : false;
}

bool HashIterator::prev() {
   return h ? priv->prev(h->priv->member_list) : false;
}

const char* HashIterator::getKey() const {
   if (!priv->valid())
      return nullptr;

   return (*(priv->i))->key.c_str();
}

AbstractQoreNode* HashIterator::getValue() const {
   if (!priv->valid())
      return nullptr;

   return (*(priv->i))->node;
}

AbstractQoreNode* HashIterator::takeValueAndDelete() {
   if (!priv->valid())
      return nullptr;

   AbstractQoreNode* rv = (*(priv->i))->node;
   (*(priv->i))->node = 0;

   qhlist_t::iterator ni = priv->i;
   priv->prev(h->priv->member_list);

   // remove key from map before deleting hash member with key pointer
   hm_hm_t::iterator i = h->priv->hm.find((*ni)->key.c_str());
   assert(i != h->priv->hm.end());
   h->priv->hm.erase(i);
   h->priv->internDeleteKey(ni);

   return rv;
}

void HashIterator::deleteKey(ExceptionSink* xsink) {
   if (!priv->valid())
      return;

   discard((*(priv->i))->node, xsink);
   (*(priv->i))->node = nullptr;

   qhlist_t::iterator ni = priv->i;
   priv->prev(h->priv->member_list);

   hm_hm_t::iterator i = h->priv->hm.find((*ni)->key.c_str());
   assert(i != h->priv->hm.end());
   h->priv->hm.erase(i);
   h->priv->internDeleteKey(ni);
}

// deprecated
AbstractQoreNode** HashIterator::getValuePtr() const {
   if (!priv->valid())
      return nullptr;

   return &((*(priv->i))->node);
}

bool HashIterator::last() const {
   if (!priv->valid())
      return false;

   qhlist_t::const_iterator ni = priv->i;
   ++ni;
   return ni == h->priv->member_list.end() ? true : false;
}

bool HashIterator::first() const {
   if (!priv->valid())
      return false;

   return priv->i == h->priv->member_list.begin();
}

bool HashIterator::empty() const {
   return h->empty();
}

bool HashIterator::valid() const {
   return priv->valid();
}

ReverseHashIterator::ReverseHashIterator(QoreHashNode* h) : HashIterator(h) {
}

ReverseHashIterator::ReverseHashIterator(QoreHashNode& h) : HashIterator(h) {
}

ReverseHashIterator::~ReverseHashIterator() {
}

bool ReverseHashIterator::last() const {
   return HashIterator::first();
}

bool ReverseHashIterator::first() const {
   return HashIterator::last();
}

bool ReverseHashIterator::next() {
   return HashIterator::prev();
}

bool ReverseHashIterator::prev() {
   return HashIterator::next();
}

ConstHashIterator::ConstHashIterator(const QoreHashNode* qh) : h(qh), priv(new qhi_priv()) {
}

ConstHashIterator::ConstHashIterator(const QoreHashNode& qh) : h(&qh), priv(new qhi_priv()) {
}

ConstHashIterator::ConstHashIterator(const ConstHashIterator& old) : h(old.h->hashRefSelf()), priv(new qhi_priv(*old.priv)) {
}

ConstHashIterator::~ConstHashIterator() {
   delete priv;
}

const QoreHashNode* ConstHashIterator::getHash() const {
   return h;
}

AbstractQoreNode* ConstHashIterator::getReferencedValue() const {
   return !priv->valid() || !(*(priv->i))->node ? 0 : (*(priv->i))->node->refSelf();
}

QoreString* ConstHashIterator::getKeyString() const {
   return !priv->valid() ? 0 : new QoreString((*(priv->i))->key);
}

bool ConstHashIterator::next() {
   return h ? priv->next(h->priv->member_list) : false;
}

bool ConstHashIterator::prev() {
   return h ? priv->prev(h->priv->member_list) : false;
}

const char* ConstHashIterator::getKey() const {
   if (!priv->valid())
      return 0;
   return (*(priv->i))->key.c_str();
}

const AbstractQoreNode* ConstHashIterator::getValue() const {
   if (!priv->valid())
      return 0;

   return (*(priv->i))->node;
}

bool ConstHashIterator::last() const {
   if (!priv->valid())
      return false;

   qhlist_t::const_iterator ni = priv->i;
   ++ni;
   return ni == h->priv->member_list.end() ? true : false;
}

bool ConstHashIterator::first() const {
   if (!priv->valid())
      return false;

   return priv->i == h->priv->member_list.begin();
}

bool ConstHashIterator::empty() const {
   return h->empty();
}

bool ConstHashIterator::valid() const {
   return priv->valid();
}

void ConstHashIterator::reset() {
   priv->reset();
}

ReverseConstHashIterator::ReverseConstHashIterator(const QoreHashNode* h) : ConstHashIterator(h) {
}

ReverseConstHashIterator::ReverseConstHashIterator(const QoreHashNode& h) : ConstHashIterator(h) {
}

ReverseConstHashIterator::~ReverseConstHashIterator() {
}

bool ReverseConstHashIterator::last() const {
   return ConstHashIterator::first();
}

bool ReverseConstHashIterator::first() const {
   return ConstHashIterator::last();
}

bool ReverseConstHashIterator::next() {
   return ConstHashIterator::prev();
}

bool ReverseConstHashIterator::prev() {
   return ConstHashIterator::next();
}

hash_assignment_priv::hash_assignment_priv(qore_hash_private& n_h, const char* key, bool must_already_exist, qore_object_private* obj) : h(n_h), om(must_already_exist ? h.findMember(key) : h.findCreateMember(key)), o(obj) {
}

hash_assignment_priv::hash_assignment_priv(QoreHashNode& n_h, const char* key, bool must_already_exist) : h(*n_h.priv), om(must_already_exist ? h.findMember(key) : h.findCreateMember(key)) {
}

hash_assignment_priv::hash_assignment_priv(QoreHashNode& n_h, const std::string& key, bool must_already_exist) : h(*n_h.priv), om(must_already_exist ? h.findMember(key.c_str()) : h.findCreateMember(key.c_str())) {
}

hash_assignment_priv::hash_assignment_priv(ExceptionSink* xsink, QoreHashNode& n_h, const QoreString& key, bool must_already_exist) : h(*n_h.priv), om(0) {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   om = must_already_exist ? h.findMember(k->getBuffer()) : h.findCreateMember(k->getBuffer());
}

hash_assignment_priv::hash_assignment_priv(ExceptionSink* xsink, QoreHashNode& n_h, const QoreString* key, bool must_already_exist) : h(*n_h.priv), om(0) {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   om = must_already_exist ? h.findMember(k->getBuffer()) : h.findCreateMember(k->getBuffer());
}

void hash_assignment_priv::reassign(const char* key, bool must_already_exist) {
   om = must_already_exist ? h.findMember(key) : h.findCreateMember(key);
}

AbstractQoreNode* hash_assignment_priv::swapImpl(AbstractQoreNode* v) {
   assert(om);
   // before we can entirely get rid of QoreNothingNode, try to convert pointers to NOTHING to 0
   if (v == &Nothing)
      v = 0;
   AbstractQoreNode* old = om->node;
   om->node = v;

   bool before = needs_scan(old);
   bool after = needs_scan(v);
   if (before) {
      if (!after) {
	 if (o)
	    o->incScanCount(-1);
	 else
	    h.incScanCount(-1);
      }
   }
   else if (after) {
      if (o)
	 o->incScanCount(1);
      else
	 h.incScanCount(1);
   }

   return old;
}

void hash_assignment_priv::assign(AbstractQoreNode* v, ExceptionSink* xsink) {
   ReferenceHolder<> val(v, xsink);
   if (h.hashdecl) {
      if (typed_hash_decl_private::get(*h.hashdecl)->runtimeAssignKey(om->key.c_str(), val, xsink))
         return;
   }
   else if (h.complexTypeInfo) {
      QoreValue v(val.release());
      QoreTypeInfo::acceptInputKey(QoreTypeInfo::getUniqueReturnComplexHash(h.complexTypeInfo), om->key.c_str(), v, xsink);
      val = v.takeNode();
      if (*xsink)
         return;
   }

   AbstractQoreNode* old = swapImpl(val.release());
   if (old) {
      // "remove" logic here
      old->deref(xsink);
   }
}

AbstractQoreNode* hash_assignment_priv::getValueImpl() const {
   return om->node;
}

HashAssignmentHelper::HashAssignmentHelper(QoreHashNode& h, const char* key, bool must_already_exist) : priv(new hash_assignment_priv(*h.priv, key, must_already_exist)) {
}

HashAssignmentHelper::HashAssignmentHelper(QoreHashNode& h, const std::string& key, bool must_already_exist) : priv(new hash_assignment_priv(*h.priv, key.c_str(), must_already_exist)) {
}

HashAssignmentHelper::HashAssignmentHelper(ExceptionSink* xsink, QoreHashNode& h, const QoreString& key, bool must_already_exist) : priv(0) {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv = new hash_assignment_priv(*h.priv, k->getBuffer(), must_already_exist);
}

HashAssignmentHelper::HashAssignmentHelper(ExceptionSink* xsink, QoreHashNode& h, const QoreString* key, bool must_already_exist) : priv(0) {
   TempEncodingHelper k(key, QCS_DEFAULT, xsink);
   if (*xsink)
      return;

   priv = new hash_assignment_priv(*h.priv, k->getBuffer(), must_already_exist);
}

HashAssignmentHelper::HashAssignmentHelper(HashIterator &hi) : priv(new hash_assignment_priv(*hi.h->priv, *(hi.priv->i))) {
}

HashAssignmentHelper::~HashAssignmentHelper() {
   delete priv;
}

void HashAssignmentHelper::reassign(const char* key, bool must_already_exist) {
   priv->reassign(key);
}

void HashAssignmentHelper::reassign(const std::string& key, bool must_already_exist) {
   priv->reassign(key.c_str());
}

HashAssignmentHelper::operator bool() const {
   return priv;
}

void HashAssignmentHelper::assign(AbstractQoreNode* v, ExceptionSink* xsink) {
   assert(priv);
   priv->assign(v, xsink);
}

AbstractQoreNode* HashAssignmentHelper::swap(AbstractQoreNode* v, ExceptionSink* xsink) {
   assert(priv);
   return priv->swap(v);
}

AbstractQoreNode* HashAssignmentHelper::operator*() const {
   assert(priv);
   return **priv;
}

void QoreParseHashNode::doDuplicateWarning(const QoreProgramLocation& newloc, const char* key) {
   qore_program_private::makeParseWarning(getProgram(), newloc, QP_WARN_DUPLICATE_HASH_KEY, "DUPLICATE-HASH-KEY", "hash key '%s' has already been given in this hash; the value given in the last occurrence will be assigned to the hash; to avoid seeing this warning, remove the extraneous key definitions or turn off the warning by using '%%disable-warning duplicate-hash-key' in your code", key);
}

int QoreParseHashNode::getAsString(QoreString& str, int foff, ExceptionSink* xsink) const {
   str.sprintf("expression hash with %d member%s", (int)keys.size(), keys.size() == 1 ? "" : "s");
   return 0;
}

QoreString* QoreParseHashNode::getAsString(bool& del, int foff, ExceptionSink* xsink) const {
   del = true;
   QoreString* rv = new QoreString;
   getAsString(*rv, foff, xsink);
   return rv;
}

const char* QoreParseHashNode::getTypeName() const {
   return "hash";
}
