/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  ConstantList.h

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

  constants can only be defined when parsing
  constants values will be substituted during the 2nd parse phase

  this structure can be safely read at all times, and writes are
  wrapped under the program-level parse lock

  NOTE: constants can only hold immediate values (no objects)

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

#ifndef _QORE_CONSTANTLIST_H

#define _QORE_CONSTANTLIST_H

#ifdef _QORE_LIB_INTERN

#include <qore/common.h>

#include <map>

class LocalVar;

class ConstantEntry {
public:
   const QoreTypeInfo *typeInfo;
   AbstractQoreNode *node;
   bool init;

   DLLLOCAL ConstantEntry() : typeInfo(0), node(0), init(false) {}
   DLLLOCAL ConstantEntry(AbstractQoreNode *v, const QoreTypeInfo *ti = 0) : typeInfo(ti), node(v), init(false) {}
   DLLLOCAL void parseInit(const char *name) {
      //printd(5, "ConstantEntry::parseInit() this=%p %s init=%d node=%p (%s)\n", this, name, init, node, get_type_name(node));
      
      if (init)
         return;
      init = true;
      if (!node)
         return;

      int lvids = 0;
      node = node->parseInit((LocalVar *)0, 0, lvids, typeInfo);

      //printd(5, "ConstantEntry::parseInit() this=%p %s initialized to node=%p (%s)\n", this, name, node, get_type_name(node));

      if (lvids) {
         parse_error("illegal local variable declaration in assignment expression for constant '%s'", name);
         while (lvids--)
            pop_local_var();
         return;
      }

      if (node->is_value())
         return;

      ParseNode *pn = dynamic_cast<ParseNode *>(node);
      if (pn && !pn->is_const_ok()) {
         parse_error("invalid expression assigned to constant '%s' (possible side effects)", name);
         return;
      }

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
         getProgram()->addParseException(&xsink);
      else if (!node->is_value())
         parse_error("invalid expression of type '%s' assigned to constant '%s' (possible side effects)", get_type_name(node), name);      
   }
};

typedef std::map<const char*, ConstantEntry, class ltstr> hm_qn_t;

class ConstantList {
private:
   hm_qn_t hm;

   DLLLOCAL void remove(hm_qn_t::iterator i);

public:
   DLLLOCAL ~ConstantList();
   DLLLOCAL void add(const char *name, AbstractQoreNode *val, const QoreTypeInfo *typeInfo = 0);
   DLLLOCAL AbstractQoreNode *find(const char *name, const QoreTypeInfo *&constantTypeInfo);
   DLLLOCAL bool inList(const char *name) const;
   DLLLOCAL ConstantList *copy();
   DLLLOCAL void reset();
   DLLLOCAL void assimilate(ConstantList *n, ConstantList *otherlist, const char *nsname);
   DLLLOCAL void assimilate(ConstantList *n);
   DLLLOCAL void parseInit();
   DLLLOCAL QoreHashNode *getInfo();
   DLLLOCAL void deleteAll();
};

#endif

#endif // _QORE_CONSTANTLIST_H
