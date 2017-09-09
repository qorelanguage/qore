/*
  ConstantList.cpp

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
#include "qore/intern/ConstantList.h"
#include "qore/intern/QoreClassIntern.h"
#include "qore/intern/qore_program_private.h"
#include "qore/intern/QoreNamespaceIntern.h"

#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
const char* ClassNs::getName() const {
   return isNs() ? getNs()->name.c_str() : getClass()->name.c_str();
}
#endif

#if 0
// currently not used
/* the following functions find all objects that are directly reachable by a resolved
   constant value and dereference the QoreProgram object that the object has
   referenced (as long as its the same QoreProgram object that owns the constant)
   in order to break the circular reference
*/
static void check_constant_cycle(QoreProgram* pgm, AbstractQoreNode* n);

static void check_constant_cycle_list(QoreProgram* pgm, QoreListNode* l) {
   ListIterator li(l);
   while (li.next())
      check_constant_cycle(pgm, li.getValue());
}

static void check_constant_cycle_hash(QoreProgram* pgm, QoreHashNode* h) {
   HashIterator hi(h);
   while (hi.next())
      check_constant_cycle(pgm, hi.getValue());
}

static void check_constant_cycle(QoreProgram* pgm, AbstractQoreNode* n) {
   qore_type_t t = get_node_type(n);
   if (t == NT_LIST)
      check_constant_cycle_list(pgm, reinterpret_cast<QoreListNode*>(n));
   else if (t == NT_HASH)
      check_constant_cycle_hash(pgm, reinterpret_cast<QoreHashNode*>(n));
}
#endif

ConstantEntry::ConstantEntry(const QoreProgramLocation& loc, const char* n, AbstractQoreNode* v, const QoreTypeInfo* ti, bool n_pub, bool n_init, bool n_builtin, ClassAccess n_access)
   : saved_node(nullptr), access(n_access), loc(loc), name(n), typeInfo(ti), node(v), in_init(false), pub(n_pub),
     init(n_init), builtin(n_builtin) {
   QoreProgram* pgm = getProgram();
   if (pgm)
      pwo = qore_program_private::getParseWarnOptions(pgm);

   //printd(5, "ConstantEntry::ConstantEntry() this: %p '%s' ti: '%s' nti: '%s'\n", this, n, QoreTypeInfo::getName(typeInfo), QoreTypeInfo::getName(getTypeInfoForValue(v)));
}

ConstantEntry::ConstantEntry(const ConstantEntry& old) :
   saved_node(old.saved_node ? old.saved_node->refSelf() : nullptr),
   access(old.access),
   loc(old.loc), pwo(old.pwo), name(old.name),
   typeInfo(old.typeInfo), node(old.node ? old.node->refSelf() : nullptr),
   in_init(false), pub(old.builtin), init(true), builtin(old.builtin) {
   assert(!old.in_init);
   assert(old.init);
}

int ConstantEntry::scanValue(const AbstractQoreNode* n) const {
   switch (get_node_type(n)) {
      case NT_LIST: {
         ConstListIterator i(reinterpret_cast<const QoreListNode*>(n));
         while (i.next())
            if (scanValue(i.getValue()))
               return -1;
         return 0;
      }

      case NT_HASH: {
         ConstHashIterator i(reinterpret_cast<const QoreHashNode*>(n));
         while (i.next())
            if (scanValue(i.getValue()))
                return -1;
         return 0;
      }

      // do not allow any closure or structure containing a closure to be copied directly into the parse tree
      // since a recursive loop can be created: https://github.com/qorelanguage/qore/issues/44
      case NT_RUNTIME_CLOSURE:
      // could have any value and could change at runtime
      case NT_OBJECT:
      case NT_FUNCREF:
         //printd(5, "ConstantEntry::scanValue() this: %p n: %p nt: %d\n", this, n, get_node_type(n));
         return -1;
   }

   return 0;
}

void ConstantEntry::del(QoreListNode& l) {
   //printd(5, "ConstantEntry::del(l) this: %p '%s' node: %p (%d) %s %d (saved_node: %p)\n", this, name.c_str(), node, get_node_type(node), get_type_name(node), node->reference_count(), saved_node);
   if (saved_node) {
      node->deref(nullptr);
      l.push(saved_node);
#ifdef DEBUG
      node = nullptr;
      saved_node = nullptr;
#endif
   }
   else if (node) {
      l.push(node);
#ifdef DEBUG
      node = nullptr;
#endif
   }

   deref();
}

void ConstantEntry::del(ExceptionSink* xsink) {
   if (saved_node) {
      node->deref(xsink);
      saved_node->deref(xsink);
#ifdef DEBUG
      node = 0;
      saved_node = 0;
#endif
   }
   else if (node) {
      // abort if an object is present and we are calling deref without an ExceptionSink object
      assert(get_node_type(node) != NT_OBJECT || xsink);
      node->deref(xsink);
#ifdef DEBUG
      node = 0;
#endif
   }

   deref();
}

int ConstantEntry::parseInit(ClassNs ptr) {
   //printd(5, "ConstantEntry::parseInit() this: %p '%s' pub: %d init: %d in_init: %d node: %p '%s' class context: %p '%s' ns: %p ('%s') pub: %d\n", this, name.c_str(), pub, init, in_init, node, get_type_name(node), ptr.getClass(), ptr.getClass() ? ptr.getClass()->name.c_str() : "<none>", ptr.getNs(), ptr.getNs() ? ptr.getNs()->name.c_str() : "<none>", ptr.getNs() ? ptr.getNs()->pub : 0);

   if (init)
      return 0;

   if (in_init) {
      parse_error(loc, "recursive constant reference found to constant '%s'", name.c_str());
      return 0;
   }

   ConstantEntryInitHelper ceih(*this);

   if (!node)
      return 0;

   int lvids = 0;

   {
      // set parse location in case of errors
      ParseLocationHelper plh(loc);

      // push parse class context
      qore_class_private* p = ptr.getClass();
      QoreParseClassHelper qpch(p ? p->cls : nullptr);

      // ensure that there is no accessible local variable state
      VariableBlockHelper vbh;

      // set parse options and warning mask for this statement
      ParseWarnHelper pwh(pwo);

      //printd(5, "ConstantEntry::parseInit() this: %p '%s' about to init node: %p '%s' class: %p '%s'\n", this, name.c_str(), node, get_type_name(node), p, p ? p->name.c_str() : "n/a");
      if (typeInfo)
         typeInfo = nullptr;

      node = node->parseInit((LocalVar*)0, PF_CONST_EXPRESSION, lvids, typeInfo);
   }

   //printd(5, "ConstantEntry::parseInit() this: %p %s initialized to node: %p (%s) value: %d type: '%s'\n", this, name.c_str(), node, get_type_name(node), node->is_value(), QoreTypeInfo::getName(typeInfo));

   if (node->is_value()) {
      if (!QoreTypeInfo::hasType(typeInfo))
         typeInfo = getTypeInfoForValue(node);
      return 0;
   }

   // do not evaluate expression if any parse exceptions have been thrown
   QoreProgram* pgm = getProgram();
   if (pgm->parseExceptionRaised()) {
      discard(node, nullptr);
      node = nullptr;
      typeInfo = nothingTypeInfo;
      return -1;
   }

   // evaluate expression
   ExceptionSink xsink;
   {
      ReferenceHolder<AbstractQoreNode> v(node->eval(&xsink), &xsink);

      //printd(5, "ConstantEntry::parseInit() this: %p %s evaluated to node: %p (%s)\n", this, name.c_str(), *v, get_type_name(*v));

      if (!xsink) {
         node->deref(&xsink);
         node = v.release();
         if (!node) {
            node = nothing();
            typeInfo = nothingTypeInfo;
         }
         else {
            typeInfo = getTypeInfoForValue(node);
            //printd(5, "ConstantEntry::parseInit() this: %p ti: '%s'\n", this, QoreTypeInfo::getName(typeInfo));
         }
      }
      else {
         node->deref(&xsink);
         node = nullptr;
         typeInfo = nothingTypeInfo;
      }
   }

   if (xsink.isEvent())
      qore_program_private::addParseException(pgm, xsink, &loc);

   // scan for call references
   if (scanValue(node)) {
      saved_node = node;
      node = new RuntimeConstantRefNode(loc, refSelf());
   }

   return 0;
}

ConstantList::ConstantList(const ConstantList& old, int64 po, ClassNs p) : ptr(p) {
   //printd(5, "ConstantList::ConstantList(old: %p, p: %s %s) this: %p cls: %p ns: %p\n", &old, p.getType(), p.getName(), this, ptr.getClass(), ptr.getNs());

   // DEBUG
   //fprintf(stderr, "XXX ConstantList::ConstantList() this=%p copy constructor from %p called\n", this, &old);
   cnemap_t::iterator last = cnemap.begin();
   for (cnemap_t::const_iterator i = old.cnemap.begin(), e = old.cnemap.end(); i != e; ++i) {
      assert(i->second->init);
      // only check copying criteria when copying a constant list in a namespace
      if (p.isNs()) {
         // check the public flag
         if (!i->second->pub)
            continue;
         if (po & PO_NO_INHERIT_USER_CONSTANTS && i->second->isUser())
            continue;
         if (po & PO_NO_INHERIT_SYSTEM_CONSTANTS && i->second->isSystem())
            continue;
      }

      ConstantEntry* ce = new ConstantEntry(*(i->second));

      last = cnemap.insert(last, cnemap_t::value_type(ce->getName(), ce));
      //printd(5, "ConstantList::ConstantList(old=%p) this=%p copying %s (%p)\n", &old, this, i->first, i->second->node);
   }
}

ConstantList::~ConstantList() {
   //QORE_TRACE("ConstantList::~ConstantList()");
   // for non-debug mode with old modules: clear constants here
   //fprintf(stderr, "XXX ConstantList::~ConstantList() this=%p size=%d\n", this, cnemap.size());

   reset();
}

void ConstantList::reset() {
   if (!cnemap.empty())
      clearIntern(0);
}

void ConstantList::clearIntern(ExceptionSink* xsink) {
   for (cnemap_t::iterator i = cnemap.begin(), e = cnemap.end(); i != e; ++i) {
      if (!i->second)
         continue;
      printd(5, "ConstantList::clearIntern() this: %p clearing %s type %s refs %d\n", this, i->first, get_type_name(i->second->node), i->second->node ? i->second->node->reference_count() : 0);
      i->second->del(xsink);
   }

   cnemap.clear();
}

// called at runtime
void ConstantList::clear(QoreListNode& l) {
   for (cnemap_t::iterator i = cnemap.begin(), e = cnemap.end(); i != e; ++i) {
      if (!i->second)
         continue;
      //printd(5, "ConstantList::clear(l: %p) this: %p clearing %s type %s refs %d\n", &l, this, i->first, get_type_name(i->second->node), i->second->node ? i->second->node->reference_count() : 0);
      i->second->del(l);
   }

   cnemap.clear();
}

// called at runtime
void ConstantList::deleteAll(ExceptionSink* xsink) {
   clearIntern(xsink);
}

void ConstantList::parseDeleteAll() {
   ExceptionSink xsink;
   clearIntern(&xsink);

   if (xsink.isEvent())
      qore_program_private::addParseException(getProgram(), xsink);
}

cnemap_t::iterator ConstantList::parseAdd(const QoreProgramLocation& loc, const char* name, AbstractQoreNode* value, const QoreTypeInfo* typeInfo, bool pub, ClassAccess access) {
   // first check if the constant has already been defined
   if (cnemap.find(name) != cnemap.end()) {
      parse_error(loc, "constant \"%s\" has already been defined", name);
      value->deref(0);
      return cnemap.end();
   }

   ConstantEntry* ce = new ConstantEntry(loc, name, value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), pub, false, false, access);
   return cnemap.insert(cnemap_t::value_type(ce->getName(), ce)).first;
}

cnemap_t::iterator ConstantList::add(const char* name, AbstractQoreNode* value, const QoreTypeInfo* typeInfo, ClassAccess access) {
#ifdef DEBUG
   if (cnemap.find(name) != cnemap.end()) {
      printd(0, "ConstantList::add() %s added twice!", name);
      assert(false);
   }
#endif
   ConstantEntry* ce = new ConstantEntry(QoreProgramLocation(), name, value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), true, true, true, access);
   return cnemap.insert(cnemap_t::value_type(ce->getName(), ce)).first;
}

ConstantEntry *ConstantList::findEntry(const char* name) {
   cnemap_t::iterator i = cnemap.find(name);
   return i == cnemap.end() ? 0 : i->second;
}

AbstractQoreNode* ConstantList::parseFind(const char* name, const QoreTypeInfo*& constantTypeInfo, ClassAccess& access) {
   cnemap_t::iterator i = cnemap.find(name);
   if (i != cnemap.end()) {
      if (!i->second->parseInit(ptr)) {
         constantTypeInfo = i->second->typeInfo;
         access = i->second->getAccess();
         return i->second->node;
      }
      constantTypeInfo = nothingTypeInfo;
      return &Nothing;
   }

   constantTypeInfo = nullptr;
   return 0;
}

AbstractQoreNode* ConstantList::find(const char* name, const QoreTypeInfo*& constantTypeInfo, ClassAccess& access) {
   cnemap_t::iterator i = cnemap.find(name);
   if (i != cnemap.end()) {
      constantTypeInfo = i->second->typeInfo;
      access = i->second->getAccess();
      return i->second->node;
   }

   constantTypeInfo = nullptr;
   return nullptr;
}

bool ConstantList::inList(const char* name) const {
   cnemap_t::const_iterator i = cnemap.find(name);
   return i != cnemap.end() ? true : false;
}

bool ConstantList::inList(const std::string& name) const {
   cnemap_t::const_iterator i = cnemap.find(name.c_str());
   return i != cnemap.end() ? true : false;
}

void ConstantList::mergeUserPublic(const ConstantList& src) {
   for (cnemap_t::const_iterator i = src.cnemap.begin(), e = src.cnemap.end(); i != e; ++i) {
      if (!i->second->isUserPublic())
         continue;

      assert(!inList(i->first));

      ConstantEntry* n = new ConstantEntry(*i->second);
      cnemap[n->getName()] = n;
   }
}

int ConstantList::importSystemConstants(const ConstantList& src, ExceptionSink* xsink) {
   for (cnemap_t::const_iterator i = src.cnemap.begin(), e = src.cnemap.end(); i != e; ++i) {
      if (!i->second->isSystem())
         continue;

      if (inList(i->first)) {
         xsink->raiseException("IMPORT-SYSTEM-API-ERROR", "cannot import system constant %s due to an existing constant with the same name in the target namespace", i->first);
         return -1;
      }

      ConstantEntry* n = new ConstantEntry(*i->second);
      cnemap[n->getName()] = n;
   }
   return 0;
}

// no duplicate checking is done here
void ConstantList::assimilate(ConstantList& n) {
   for (cnemap_t::iterator i = n.cnemap.begin(), e = n.cnemap.end(); i != e; ++i) {
      assert(!inList(i->first));
      // "move" data to new list
      cnemap[i->first] = i->second;
      i->second = 0;
   }

   n.parseDeleteAll();
}

// duplicate checking is done here
void ConstantList::assimilate(ConstantList& n, ConstantList& otherlist, const char* type, const char* name) {
   // assimilate target list
   for (cnemap_t::iterator i = n.cnemap.begin(), e = n.cnemap.end(); i != e; ++i) {
      if (inList(i->first)) {
         parse_error(i->second->loc, "constant \"%s\" is already pending in %s \"%s\"", i->first, type, name);
         continue;
      }

      if (otherlist.inList(i->first)) {
         parse_error(i->second->loc, "constant \"%s\" has already been defined in %s \"%s\"", i->first, type, name);
         continue;
      }

      cnemap[i->first] = i->second;
      i->second = 0;
   }

   n.parseDeleteAll();
}

void ConstantList::parseAdd(const QoreProgramLocation& loc, const std::string& name, AbstractQoreNode* val, ConstantList& committed, ClassAccess access, const char* cname) {
   if (inList(name)) {
      parse_error(loc, "constant \"%s\" is already pending in class \"%s\"", name.c_str(), cname);
      discard(val, 0);
      return;
   }

   if (committed.inList(name)) {
      parse_error(loc, "constant \"%s\" has already been defined in class \"%s\"", name.c_str(), cname);
      discard(val, 0);
      return;
   }

   ConstantEntry* ce = new ConstantEntry(loc, name.c_str(), val, getTypeInfoForValue(val), false, false, false, access);
   cnemap[ce->getName()] = ce;
}

void ConstantList::parseInit() {
   for (cnemap_t::iterator i = cnemap.begin(), e = cnemap.end(); i != e; ++i) {
      //printd(5, "ConstantList::parseInit() this: %p '%s' %p (class: %p '%s' ns: %p '%s')\n", this, i->first, i->second->node, ptr.getClass(), ptr.getClass() ? ptr.getClass()->name.c_str() : "n/a", ptr.getNs(), ptr.getNs() ? ptr.getNs()->name.c_str() : "n/a");
      i->second->parseInit(ptr);
   }
}

QoreHashNode* ConstantList::getInfo() {
   QoreHashNode* h = new QoreHashNode;

   for (cnemap_t::iterator i = cnemap.begin(), e = cnemap.end(); i != e; ++i)
      h->setKeyValue(i->first, i->second->node->refSelf(), 0);

   return h;
}
