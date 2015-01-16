/*
  ConstantList.cpp
 
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

#include <qore/Qore.h>
#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreClassIntern.h>
#include <qore/intern/qore_program_private.h>
#include <qore/intern/QoreNamespaceIntern.h>

#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
const char* ClassNs::getName() const {
   return isNs() ? getNs()->name.c_str() : getClass()->name.c_str();
}
#endif

/* the following functions find all objects that are directly reachable by a resolved
   constant value and dereference the QoreProgram object that the object has 
   referenced (as long as its the same QoreProgram object that owns the constant)
   in order to break the circular reference
*/ 
static void check_constant_cycle(QoreProgram *pgm, AbstractQoreNode *n);

static void check_constant_cycle_list(QoreProgram *pgm, QoreListNode *l) {
   ListIterator li(l);
   while (li.next())
      check_constant_cycle(pgm, li.getValue());
}

static void check_constant_cycle_hash(QoreProgram *pgm, QoreHashNode *h) {
   HashIterator hi(h);
   while (hi.next())
      check_constant_cycle(pgm, hi.getValue());
}

static void check_constant_cycle(QoreProgram *pgm, AbstractQoreNode *n) {
   qore_type_t t = get_node_type(n);
   if (t == NT_LIST)
      check_constant_cycle_list(pgm, reinterpret_cast<QoreListNode *>(n));
   else if (t == NT_HASH)
      check_constant_cycle_hash(pgm, reinterpret_cast<QoreHashNode *>(n));
   else if (t == NT_OBJECT)
      qore_object_private::derefProgramCycle(reinterpret_cast<QoreObject *>(n), pgm);
   else if (t == NT_RUNTIME_CLOSURE) {
      //printd(5, "check_constant_cycle() closure=%p\n", n);
      reinterpret_cast<QoreClosureBase *>(n)->derefProgramCycle(pgm);
   }
}

ConstantEntry::ConstantEntry(const char* n, AbstractQoreNode* v, const QoreTypeInfo* ti, bool n_pub, bool n_init, bool n_builtin)
   : saved_node(0), loc(ParseLocation), name(n), typeInfo(ti), node(v), in_init(false), pub(n_pub), init(n_init), builtin(n_builtin)  {
   QoreProgram* pgm = getProgram();
   if (pgm)
      pwo = qore_program_private::getParseWarnOptions(pgm);
}

ConstantEntry::ConstantEntry(const ConstantEntry& old) : 
   saved_node(old.saved_node ? old.saved_node->refSelf() : 0), loc(old.loc), pwo(old.pwo), name(old.name),
   typeInfo(old.typeInfo), node(old.node ? old.node->refSelf() : 0),
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
	 
      // could have any value and could change at runtime
      case NT_OBJECT:
      case NT_FUNCREF:
	 return -1;
   }

   return 0;
}

void ConstantEntry::del(QoreListNode& l) {
   if (saved_node) {
      node->deref(0);
      l.push(saved_node);
#ifdef DEBUG
      node = 0;
      saved_node = 0;
#endif
   }
   else if (node) {
      l.push(node);
#ifdef DEBUG
      node = 0;
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
      parse_error("recursive constant reference found to constant '%s'", name.c_str());
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
      QoreParseClassHelper qpch(p ? p->cls : 0);

      // ensure that there is no accessible local variable state
      VariableBlockHelper vbh;

      // set parse options and warning mask for this statement
      ParseWarnHelper pwh(pwo);

      //printd(5, "ConstantEntry::parseInit() this: %p '%s' about to init node: %p '%s' class: %p '%s'\n", this, name.c_str(), node, get_type_name(node), p, p ? p->name.c_str() : "n/a");
      if (typeInfo)
         typeInfo = 0;

      node = node->parseInit((LocalVar*)0, PF_CONST_EXPRESSION, lvids, typeInfo);
   }

   //printd(5, "ConstantEntry::parseInit() this: %p %s initialized to node: %p (%s) value: %d\n", this, name.c_str(), node, get_type_name(node), node->is_value());

   if (node->is_value())
      return 0;

   // do not evaluate expression if any parse exceptions have been thrown
   QoreProgram* pgm = getProgram();
   if (pgm->parseExceptionRaised()) {
      discard(node, 0);
      node = 0;
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
	    check_constant_cycle(pgm, node); // address circular refs: pgm->const->pgm
	 }
      }
      else {
	 node->deref(&xsink);
	 node = 0;
	 typeInfo = nothingTypeInfo;
      }
   }

   if (xsink.isEvent())
      qore_program_private::addParseException(pgm, xsink, &loc);

   // scan for call references
   if (scanValue(node)) {
      saved_node = node;
      node = new RuntimeConstantRefNode(refSelf());
   }

   return 0;
}

ConstantList::ConstantList(const ConstantList &old, ClassNs p) : ptr(p) {
   //printd(5, "ConstantList::ConstantList(old: %p, p: %s %s) this: %p cls: %p ns: %p\n", &old, p.getType(), p.getName(), this, ptr.getClass(), ptr.getNs());

   // DEBUG
   //fprintf(stderr, "XXX ConstantList::ConstantList() this=%p copy constructor from %p called\n", this, &old);
   cnemap_t::iterator last = cnemap.begin();
   for (cnemap_t::const_iterator i = old.cnemap.begin(), e = old.cnemap.end(); i != e; ++i) {
      assert(i->second->init);
      // only check the public flag if copying a constant list in a namespace
      if (p.isNs() && !i->second->pub)
         continue;

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

void ConstantList::clearIntern(ExceptionSink *xsink) {
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
      printd(5, "ConstantList::clear(l: %p) this: %p clearing %s type %s refs %d\n", &l, this, i->first, get_type_name(i->second->node), i->second->node ? i->second->node->reference_count() : 0);
      i->second->del(l);
   }

   cnemap.clear();
}

// called at runtime
void ConstantList::deleteAll(ExceptionSink *xsink) {
   clearIntern(xsink);
}

void ConstantList::parseDeleteAll() {
   ExceptionSink xsink;
   clearIntern(&xsink);

   if (xsink.isEvent())
      qore_program_private::addParseException(getProgram(), xsink);
}

cnemap_t::iterator ConstantList::parseAdd(const char* name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo, bool pub) {
   // first check if the constant has already been defined
   if (cnemap.find(name) != cnemap.end()) {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(0);
      return cnemap.end();
   }

   ConstantEntry* ce = new ConstantEntry(name, value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), pub);
   return cnemap.insert(cnemap_t::value_type(ce->getName(), ce)).first;
}

cnemap_t::iterator ConstantList::add(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) {
#ifdef DEBUG
   if (cnemap.find(name) != cnemap.end()) {
      printd(0, "ConstantList::add() %s added twice!", name);
      assert(false);
   }
#endif
   ConstantEntry* ce = new ConstantEntry(name, value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), true, true, true);
   return cnemap.insert(cnemap_t::value_type(ce->getName(), ce)).first;
}

ConstantEntry *ConstantList::findEntry(const char *name) {
   cnemap_t::iterator i = cnemap.find(name);
   return i == cnemap.end() ? 0 : i->second;
}

AbstractQoreNode *ConstantList::find(const char *name, const QoreTypeInfo *&constantTypeInfo) {
   cnemap_t::iterator i = cnemap.find(name);
   if (i != cnemap.end()) {
      if (!i->second->parseInit(ptr)) {
	 constantTypeInfo = i->second->typeInfo;
	 return i->second->node;
      }
      constantTypeInfo = nothingTypeInfo;
      return &Nothing;
   }

   constantTypeInfo = 0;
   return 0;
}

bool ConstantList::inList(const char *name) const {
   cnemap_t::const_iterator i = cnemap.find(name);
   return i != cnemap.end() ? true : false;
}

bool ConstantList::inList(const std::string &name) const {
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
void ConstantList::assimilate(ConstantList& n, ConstantList& otherlist, const char *name) {
   // assimilate target list
   for (cnemap_t::iterator i = n.cnemap.begin(), e = n.cnemap.end(); i != e; ++i) {
      if (inList(i->first)) {
	 parse_error("constant \"%s\" is already pending in namespace \"%s\"", i->first, name);
	 continue;
      }

      if (otherlist.inList(i->first)) {
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"", i->first, name);
	 continue;
      }

      cnemap[i->first] = i->second;
      i->second = 0;
   }

   n.parseDeleteAll();
}

int ConstantList::checkDup(const char* name, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   if (inList(name)) {
      parse_error("%s constant \"%s\" is already pending in class \"%s\"", privpub(priv), name, cname);
      return -1;
   }

   // see if constant already exists in committed list
   if (committed.inList(name)) {
      parse_error("%s constant \"%s\" has already been added to class \"%s\"", privpub(priv), name, cname);
      return -1;
   }

   // see if constant is in the other pending list
   if (otherPend.inList(name)) {
      parse_error("%s constant \"%s\" is already pending in class \"%s\" as a %s constant", privpub(priv), name, cname, privpub(!priv));
      return -1;
   }

   // see if constant is in the other committed list
   if (other.inList(name)) {
      parse_error("%s constant \"%s\" has already been added to class \"%s\" as a %s constant", privpub(priv), name, cname, privpub(!priv));
      return -1;
   }
   
   return 0;
}

void ConstantList::parseAdd(const std::string &name, AbstractQoreNode *val, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   if (checkDup(name.c_str(), committed, other, otherPend, priv, cname)) {
      if (val)
	 val->deref(0);
   }
   else {
      ConstantEntry* ce = new ConstantEntry(name.c_str(), val, getTypeInfoForValue(val));
      cnemap[ce->getName()] = ce;
   }
}

void ConstantList::assimilate(ConstantList &n, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   for (cnemap_t::iterator i = n.cnemap.begin(), e = n.cnemap.end(); i != e; ++i) {
      if (!checkDup(i->first, committed, other, otherPend, priv, cname)) {
	 cnemap[i->first] = i->second;
	 i->second = 0;
      }
   }

   n.parseDeleteAll();
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
