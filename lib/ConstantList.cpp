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

#include <string.h>
#include <stdlib.h>

void ConstantEntry::parseInit(const char *name) {
   //printd(5, "ConstantEntry::parseInit() this=%p %s init=%d node=%p (%s)\n", this, name, init, node, get_type_name(node));      
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

   // do not evaluate expression if any parse exception have been thrown
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
      }
   }
	       
   if (xsink.isEvent())
      pgm->addParseException(&xsink);
   else if (!node->is_value())
      parse_error("invalid expression of type '%s' assigned to constant '%s' (possible side effects)", get_type_name(node), name);      
}

void ConstantList::remove(hm_qn_t::iterator i) {
   if (i->second.node)
      i->second.node->deref(0);
   
   const char *c = i->first;
   hm.erase(i);
   free((char *)c);	 
}

ConstantList::~ConstantList() {
   //QORE_TRACE("ConstantList::~ConstantList()");
   deleteAll();
}

// NOTE: since constants cannot hold objects (only immediate values)
// FIXME: constants should be able to hold objects (and can and do hold 
//        system objects, but their destructors may not throw exceptions)
// there is no need for an exception handler with the dereference
void ConstantList::deleteAll() {
   hm_qn_t::iterator i = hm.begin();
   while (i != hm.end()) {
      remove(i);
      i = hm.begin();
   }
}

void ConstantList::reset() {
   deleteAll();
}

void ConstantList::parseAdd(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) {
   // first check if the constant has already been defined
   if (hm.find(name) != hm.end()) {
      parse_error("constant \"%s\" has already been defined", name);
      value->deref(0);
      return;
   }

   hm[strdup(name)] = ConstantEntry(value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value));
}

void ConstantList::add(const char *name, AbstractQoreNode *value, const QoreTypeInfo *typeInfo) {
#ifdef DEBUG
   if (hm.find(name) != hm.end()) {
      printd(0, "ConstantList::add() %s added twice!", name);
      assert(false);
   }
#endif
   hm[strdup(name)] = ConstantEntry(value, typeInfo || value->needs_eval() ? typeInfo : getTypeInfoForValue(value), true);
}

AbstractQoreNode *ConstantList::find(const char *name, const QoreTypeInfo *&constantTypeInfo) {
   hm_qn_t::iterator i = hm.find(name);
   if (i != hm.end()) {
      i->second.parseInit(i->first);
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

ConstantList *ConstantList::copy() {
   ConstantList *ncl = new ConstantList;
   
   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++) {
      // reference value for new constant definition
      if (i->second.node)
	 i->second.node->ref();
      ncl->add(i->first, i->second.node);
      //printd(5, "ConstantList::copy() this=%p copying %s (%p)\n", this, i->first, i->second.node);
   }
   
   return ncl;
}

// no duplicate checking is done here
void ConstantList::assimilate(ConstantList *n) {
   hm_qn_t::iterator i = n->hm.begin();
   while (i != n->hm.end()) {
      // "move" data to new list
      hm[i->first] = i->second;
      n->hm.erase(i);
      i = n->hm.begin();
   }
}

// duplicate checking is done here
void ConstantList::assimilate(ConstantList *n, ConstantList *otherlist, const char *nsname) {
   // assimilate target list
   hm_qn_t::iterator i = n->hm.begin();
   while (i != n->hm.end()) {
      hm_qn_t::iterator j = otherlist->hm.find(i->first);
      if (j != otherlist->hm.end()) {
	 parse_error("constant \"%s\" has already been defined in namespace \"%s\"", i->first, nsname);
	 n->remove(i);
      }
      else {      
	 j = hm.find(i->first);
	 if (j != hm.end()) {
	    parse_error("constant \"%s\" is already pending for namespace \"%s\"", i->first, nsname);
	    n->remove(i);
	 }
	 else {
	    // "move" data to new list
	    hm[i->first] = i->second;
	    n->hm.erase(i);
	 }
      }
      i = n->hm.begin();
   }
}

void ConstantList::parseInit() {
   //RootQoreNamespace *rns = getRootNS();
   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); ++i) {
      printd(5, "ConstantList::parseInit() %s %p\n", i->first, i->second.node);
      i->second.parseInit(i->first);
   }
}

QoreHashNode *ConstantList::getInfo() {
   QoreHashNode *h = new QoreHashNode();

   for (hm_qn_t::iterator i = hm.begin(); i != hm.end(); i++)
      h->setKeyValue(i->first, i->second.node->refSelf(), 0);

   return h;
}
