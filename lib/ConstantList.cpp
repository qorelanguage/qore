/*
 ConstantList.cpp
 
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
#include <qore/intern/ConstantList.h>
#include <qore/intern/QoreClassIntern.h>

#include <string.h>
#include <stdlib.h>

ConstantCycleHelper::ConstantCycleHelper(ConstantEntry *n_ce, const char *name) : ce(n_ce) {
   if (set_constant(n_ce)) {
      //printd(5, "ConstantCycleHelper::ConstantCycleHelper(%p, %s) already present\n", ce, name);
      parse_error("recursive constant reference found to constant '%s'", name);
      ce = 0;
      return;
   }
   //printd(5, "ConstantCycleHelper::ConstantCycleHelper(%p, %s) OK\n", ce, name);
}

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

void ConstantEntry::parseInit(const char *name) {
   //printd(5, "ConstantEntry::parseInit() this=%p %s init=%d node=%p (%s)\n", this, name, init, node, get_type_name(node));

   ConstantCycleHelper cch(this, name);
   if (!cch) {
      assert(init);
      return;
   }

   if (init)
      return;
   init = true;

   if (!node)
      return;

   int lvids = 0;
   node = node->parseInit((LocalVar *)0, PF_CONST_EXPRESSION, lvids, typeInfo);

   //printd(5, "ConstantEntry::parseInit() this=%p %s initialized to node=%p (%s)\n", this, name, node, get_type_name(node));

   if (lvids) {
      parse_error("illegal local variable declaration in assignment expression for constant '%s'", name);
      while (lvids--)
	 pop_local_var();
      return;
   }

   if (node->is_value())
      return;

   // do not evaluate expression if any parse exceptions have been thrown
   QoreProgram *pgm = getProgram();
   if (pgm->parseExceptionRaised())
      return;

   // evaluate expression
   ExceptionSink xsink;
   {
      // FIXME: set location?
      ReferenceHolder<AbstractQoreNode> v(node->eval(&xsink), &xsink);

      //printd(5, "ConstantEntry::parseInit() this=%p %s evaluated to node=%p (%s)\n", this, name, *v, get_type_name(*v));

      if (!xsink) {
	 node->deref(&xsink);
	 node = v.release();
	 if (!node) {
	    node = nothing();
	    typeInfo = nothingTypeInfo;
	 }
	 else
	    check_constant_cycle(pgm, node); // address circular refs: pgm->const->pgm
      }
   }

   if (xsink.isEvent())
      pgm->addParseException(&xsink);
}

ConstantList::ConstantList(const ConstantList &old) {
   for (hm_qn_t::const_iterator i = old.hm.begin(), e = old.hm.end(); i != e; ++i) {
      assert(i->second.init);

      // reference value for new constant definition
      if (i->second.node)
	 i->second.node->ref();

      hm[i->first] = ConstantEntry(i->second.node, i->second.typeInfo, true);
      //printd(5, "ConstantList::ConstantList(old=%p) this=%p copying %s (%p)\n", &old, this, i->first.c_str(), i->second.node);
   }
}

ConstantList::~ConstantList() {
   //QORE_TRACE("ConstantList::~ConstantList()");
   // for non-debug mode with old modules: clear constants here
   if (!hm.empty())
      deleteAll(0);
}

void ConstantList::clearIntern(ExceptionSink *xsink) {
   for (hm_qn_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      if (i->second.node) {
	 // abort if an object is present and we are calling deref without an ExceptionSink object
	 assert(get_node_type(i->second.node) != NT_OBJECT || xsink);
	 i->second.node->deref(xsink);
      }
   }

   hm.clear();
}

// called at runtime
void ConstantList::deleteAll(ExceptionSink *xsink) {
   clearIntern(xsink);
}

void ConstantList::parseDeleteAll() {
   ExceptionSink xsink;
   clearIntern(&xsink);

   if (xsink.isEvent())
      getProgram()->addParseException(&xsink);
}

void ConstantList::parseAdd(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) {
   // first check if the constant has already been defined
   if (hm.find(name) != hm.end()) {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(0);
      return;
   }

   hm[name] = ConstantEntry(value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value));
}

void ConstantList::add(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) {
#ifdef DEBUG
   if (hm.find(name) != hm.end()) {
      printd(0, "ConstantList::add() %s added twice!", name);
      assert(false);
   }
#endif
   hm[name] = ConstantEntry(value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), true);
}

AbstractQoreNode *ConstantList::find(const char *name, const QoreTypeInfo *&constantTypeInfo) {
   hm_qn_t::iterator i = hm.find(name);
   if (i != hm.end()) {
      i->second.parseInit(i->first.c_str());
      constantTypeInfo = i->second.typeInfo;
      return i->second.node;
   }

   constantTypeInfo = 0;
   return 0;
}

bool ConstantList::inList(const char *name) const {
   hm_qn_t::const_iterator i = hm.find(name);
   return i != hm.end() ? true : false;
}

bool ConstantList::inList(const std::string &name) const {
   hm_qn_t::const_iterator i = hm.find(name);
   return i != hm.end() ? true : false;
}

// no duplicate checking is done here
void ConstantList::assimilate(ConstantList *n) {
   for (hm_qn_t::iterator i = n->hm.begin(), e = n->hm.end(); i != e; ++i) {
      assert(!inList(i->first));
      // "move" data to new list
      hm[i->first] = i->second;
      i->second = 0;
   }
   
   n->parseDeleteAll();
}

// duplicate checking is done here
void ConstantList::assimilate(ConstantList *n, ConstantList *otherlist, const char *name) {
   // assimilate target list
   for (hm_qn_t::iterator i = n->hm.begin(), e = n->hm.end(); i != e; ++i) {
      if (inList(i->first)) {
	 parse_error("constant \"%s\" is already pending in namespace \"%s\"", i->first.c_str(), name);
	 continue;
      }

      if (otherlist->inList(i->first)) {
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"", i->first.c_str(), name);
	 continue;
      }

      hm[i->first] = i->second;
      i->second = 0;
   }

   n->parseDeleteAll();
}

int ConstantList::checkDup(const std::string &name, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   if (inList(name)) {
      parse_error("%s constant \"%s\" is already pending in class \"%s\"", privpub(priv), name.c_str(), cname);
      return -1;
   }

   // see if constant already exists in committed list
   if (committed.inList(name)) {
      parse_error("%s constant \"%s\" has already been added to class \"%s\"", privpub(priv), name.c_str(), cname);
      return -1;
   }

   // see if constant is in the other pending list
   if (otherPend.inList(name)) {
      parse_error("%s constant \"%s\" is already pending in class \"%s\" as a %s constant", privpub(priv), name.c_str(), cname, privpub(!priv));
      return -1;
   }

   // see if constant is in the other committed list
   if (other.inList(name)) {
      parse_error("%s constant \"%s\" has already been added to class \"%s\" as a %s constant", privpub(priv), name.c_str(), cname, privpub(!priv));
      return -1;
   }
   
   return 0;
}

void ConstantList::parseAdd(const std::string &name, AbstractQoreNode *val, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   if (checkDup(name, committed, other, otherPend, priv, cname))
      val->deref(0);
   else
      hm[name] = ConstantEntry(val, getTypeInfoForValue(val));
}

void ConstantList::assimilate(ConstantList &n, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   for (hm_qn_t::iterator i = n.hm.begin(), e = n.hm.end(); i != e; ++i) {
      if (!checkDup(i->first, committed, other, otherPend, priv, cname)) {
	 hm[i->first] = i->second;
	 i->second = 0;
      }
   }

   n.parseDeleteAll();
}

void ConstantList::parseInit() {
   //RootQoreNamespace *rns = getRootNS();
   for (hm_qn_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      printd(5, "ConstantList::parseInit() %s %p\n", i->first.c_str(), i->second.node);
      i->second.parseInit(i->first.c_str());
   }
}

QoreHashNode *ConstantList::getInfo() {
   QoreHashNode *h = new QoreHashNode;

   for (hm_qn_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i)
      h->setKeyValue(i->first.c_str(), i->second.node->refSelf(), 0);

   return h;
}
