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
   deleteAll();
}

void ConstantList::clearIntern(ExceptionSink *xsink) {
   for (hm_qn_t::iterator i = hm.begin(), e = hm.end(); i != e; ++i) {
      if (i->second.node)
	 i->second.node->deref(xsink);
   }

   hm.clear();
}

// called at runtime
void ConstantList::deleteAll() {
   ExceptionSink xsink;
   clearIntern(&xsink);
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

void ConstantList::assimilate(ConstantList &n, ConstantList &committed, ConstantList &other, ConstantList &otherPend, bool priv, const char *cname) {
   for (hm_qn_t::iterator i = n.hm.begin(), e = n.hm.end(); i != e; ++i) {
      // see if constant already exists in this list
      if (inList(i->first)) {
	 parse_error("%s constant \"%s\" is already pending in class \"%s\"", privpub(priv), i->first.c_str(), cname);
	 continue;
      }

      // see if constant already exists in committed list
      if (committed.inList(i->first)) {
	 parse_error("%s constant \"%s\" has already been added to class \"%s\"", privpub(priv), i->first.c_str(), cname);
	 continue;
      }

      // see if constant is in the other pending list
      if (otherPend.inList(i->first)) {
	 parse_error("%s constant \"%s\" is already pending in class \"%s\" as a %s constant", privpub(priv), i->first.c_str(), cname, privpub(!priv));
	 continue;
      }

      // see if constant is in the other committed list
      if (other.inList(i->first)) {
	 parse_error("%s constant \"%s\" has already been added to class \"%s\" as a %s constant", privpub(priv), i->first.c_str(), cname, privpub(!priv));
	 continue;
      }

      // "move" data to new list
      hm[i->first] = i->second;
      i->second = 0;
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
