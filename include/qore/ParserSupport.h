/*
  ParserSupport.h

  parsing support functions and objects

  Qore Programming language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_PARSER_SUPPORT_H

#define _QORE_PARSER_SUPPORT_H

#include <qore/LockedObject.h>
#include <qore/common.h>

#include <stdlib.h>

static inline class QoreNode *makeTree(class Operator *op, class QoreNode *left, class QoreNode *right);
static inline QoreNode *makeArgs(QoreNode *arg);
static inline void addNSNode(class Namespace *ns, struct NSNode *n);

#define HE_TAG_CONST        1
#define HE_TAG_SCOPED_CONST 2

class HashElement {
   public:
      char *key;
      class QoreNode *value;
      DLLLOCAL inline HashElement(class QoreNode *k, class QoreNode *v);
      DLLLOCAL inline HashElement(int tag, char *constant, class QoreNode *v);
      DLLLOCAL inline ~HashElement();
};

#include <qore/QoreNode.h>
#include <qore/List.h>
#include <qore/QoreType.h>
#include <qore/Namespace.h>
#include <qore/Operator.h>

#include <stdlib.h>

static inline class QoreNode *makeErrorTree(class Operator *op, class QoreNode *left, class QoreNode *right)
{
   return new QoreNode(left, op, right);
}

static class QoreNode *makeTree(class Operator *op, class QoreNode *left, class QoreNode *right)
{
   ExceptionSink xsink;

   //tracein("makeTree()");
   //printd(5, "makeTree(): l=%08p, r=%08p, op=%d\n", left, right, op);
   // if both nodes are constants, then evaluate immediately */
   if (is_value(left) && (!right || is_value(right)))
   {
      class QoreNode *n_node = op->eval(left, right, &xsink);
      //printd(5, "makeTree(): l=%08p (%s), r=%08p, op=%s, returning %08p\n", left, left->type->name, right, op->name, n_node);
      left->deref(NULL);
      if (right)
	 right->deref(NULL);

      if (xsink.isEvent())
	 getProgram()->addParseException(&xsink);

      //traceout("makeTree()");
      return n_node;
   }
   // otherwise, put nodes and operator into tree for runtime evaluation
   return new QoreNode(left, op, right);
}

static inline QoreNode *makeArgs(QoreNode *arg)
{
   if (!arg || arg->type == NT_LIST)
      return arg;
   List *l = new List(1);
   l->push(arg);
   return new QoreNode(l);
}

DLLLOCAL inline HashElement::HashElement(class QoreNode *k, class QoreNode *v)
{
   //tracein("HashElement::HashElement()");
   if (k->type != NT_STRING)
   {
      parse_error("object member name must be a string value!");
      key = strdup("");
   }
   else
      key = strdup(k->val.String->getBuffer());
   k->deref(NULL);
   value = v;
   //traceout("HashElement::HashElement()");
}

DLLLOCAL inline HashElement::HashElement(int tag, char *constant, class QoreNode *v)
{
   //tracein("HashElement::HashElement()");
   key = (char *)malloc(sizeof(char) * strlen(constant) + 2);
   key[0] = tag; // mark as constant
   strcpy(key + 1, constant);
   value = v;
   free(constant);
   //traceout("HashElement::HashElement()");
}

DLLLOCAL inline HashElement::~HashElement()
{
   free(key);
}

// for constant definitions
class ConstNode
{
   public:
      class NamedScope *name;
      class QoreNode *value;
      DLLLOCAL inline ConstNode(char *n, class QoreNode *v) { name = new NamedScope(n); value = v; }
      DLLLOCAL inline ~ConstNode() { delete name; }
};

class ObjClassDef
{
   public:
      class NamedScope *name;
      class QoreClass *oc;
      DLLLOCAL inline ObjClassDef(char *n, class QoreClass *o) { name = new NamedScope(n); oc = o; }
      DLLLOCAL inline ~ObjClassDef() { delete name; }
};

#define NSN_OCD   1
#define NSN_CONST 2
#define NSN_NS    3

struct NSNode
{
      int type;
      union {
	    class ObjClassDef *ocd;
	    class ConstNode  *cn;
	    class Namespace  *ns;
      } n;
      DLLLOCAL NSNode(class ObjClassDef *o) { type = NSN_OCD; n.ocd = o; }
      DLLLOCAL NSNode(class ConstNode  *c) { type = NSN_CONST; n.cn = c; }
      DLLLOCAL NSNode(class Namespace  *s) { type = NSN_NS; n.ns = s; }
};

static inline void addNSNode(class Namespace *ns, struct NSNode *n)
{
   switch (n->type)
   {
      case NSN_OCD:
	 ns->addClass(n->n.ocd->name, n->n.ocd->oc);
	 delete n->n.cn;
	 break;
      case NSN_CONST:
	 ns->addConstant(n->n.cn->name, n->n.cn->value);
	 delete n->n.cn;
	 break;
      case NSN_NS:
	 ns->addNamespace(n->n.ns);
	 break;
   }
   delete n;
}

#endif // _QORE_PARSER_SUPPORT_H
