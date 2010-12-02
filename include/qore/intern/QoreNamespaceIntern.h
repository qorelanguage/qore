/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QoreNamespaceIntern.h

  Qore Programming Language

  Copyright (C) 2003 - 2010 David Nichols

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

#ifndef _QORE_QORENAMESPACEINTERN_H
#define _QORE_QORENAMESPACEINTERN_H

struct qore_ns_private {
   std::string name;

   QoreClassList      *classList;
   ConstantList       *constant;
   QoreNamespaceList  *nsl;

   // pending lists
   // FIXME: can be normal members
   QoreClassList      *pendClassList;
   ConstantList       *pendConstant;
   QoreNamespaceList  *pendNSL;

   const QoreNamespace *parent;
   q_ns_class_handler_t class_handler;   
   QoreNamespace *ns;

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n, QoreClassList *ocl, ConstantList *cl, QoreNamespaceList *nnsl) :
      name(n), 
      classList(ocl), constant(cl), nsl(nnsl), 
      pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList),
      parent(0), class_handler(0), ns(n_ns) {
      assert(classList);
      assert(constant);
      assert(nsl);
   }

   DLLLOCAL qore_ns_private(QoreNamespace *n_ns, const char *n) :
      name(n),
      classList(new QoreClassList), constant(new ConstantList), nsl(new QoreNamespaceList), 
      pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList),
      parent(0), class_handler(0), ns(n_ns) {
   }

   DLLLOCAL qore_ns_private(const qore_ns_private &old, QoreNamespace *n_ns, int64 po) : 
      name(old.name), classList(old.classList->copy(po)), constant(new ConstantList(*old.constant)),
      nsl(old.nsl->copy(po, n_ns)), 
      pendClassList(new QoreClassList), pendConstant(new ConstantList), pendNSL(new QoreNamespaceList),
      parent(0), class_handler(old.class_handler), ns(n_ns) {
   }		    

   DLLLOCAL ~qore_ns_private() {
      printd(5, "QoreNamespace::~QoreNamespace() this=%p '%s'\n", this, name.c_str());

      purge();
   }

   DLLLOCAL void purge() {
      delete constant;
      constant = 0;

      if (nsl)
	 nsl->deleteAllConstants();

      delete classList;
      classList = 0;

      delete nsl;
      nsl = 0;
	 
      delete pendConstant;
      pendConstant = 0;

      delete pendClassList;
      pendClassList = 0;

      delete pendNSL;
      pendNSL = 0;
   }

   // finds a local class in the committed class list, if not found executes the class handler
   DLLLOCAL QoreClass *findLoadClass(QoreNamespace *cns, const char *cname) {
      QoreClass *qc = classList->find(cname);
      if (!qc && class_handler)
	 qc = class_handler(cns, cname);
      return qc;
   }

   DLLLOCAL AbstractQoreNode *parseResolveBareword(const char *name, const QoreTypeInfo *&typeInfo) const {
      AbstractQoreNode *rv = constant->find(name, typeInfo);
      if (!rv) {
	 rv = pendConstant->find(name, typeInfo);
	 if (!rv) {
	    rv = classList->parseResolveBareword(name, typeInfo);
	    if (!rv) {
	       rv = pendClassList->parseResolveBareword(name, typeInfo);
	       if (!rv) {
		  rv = nsl->parseResolveBareword(name, typeInfo);
		  if (!rv)
		     rv = pendNSL->parseResolveBareword(name, typeInfo);
	       }
	    }
	 }
      }
      return rv;
   }
};

#endif
