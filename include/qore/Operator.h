/*
  Operator.h

  Qore flexible operator support

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

  FIXME: dynamic allocation of operators after initialization is not thread-safe

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

#ifndef _QORE_OPERATOR_H

#define _QORE_OPERATOR_H

#include <string.h>
#include <stdlib.h>

#include <qore/node_types.h>

// system default operators
extern class Operator *OP_ASSIGNMENT, *OP_LOG_AND, *OP_LOG_OR, *OP_LOG_LT, 
   *OP_LOG_GT, *OP_LOG_EQ, *OP_LOG_NE, *OP_LOG_LE, *OP_LOG_GE, *OP_MODULA, 
   *OP_BIN_AND, *OP_BIN_OR, *OP_BIN_NOT, *OP_BIN_XOR, *OP_MINUS, *OP_PLUS, 
   *OP_MULT, *OP_DIV, *OP_UNARY_MINUS, *OP_NOT, *OP_SHIFT_LEFT, *OP_SHIFT_RIGHT, 
   *OP_POST_INCREMENT, *OP_POST_DECREMENT, *OP_PRE_INCREMENT, *OP_PRE_DECREMENT, 
   *OP_LOG_CMP, *OP_PLUS_EQUALS, *OP_MINUS_EQUALS, *OP_AND_EQUALS, *OP_OR_EQUALS, 
   *OP_LIST_REF, *OP_OBJECT_REF, *OP_ELEMENTS, *OP_KEYS, *OP_QUESTION_MARK, 
   *OP_ABSOLUTE_EQ, *OP_ABSOLUTE_NE, *OP_REGEX_MATCH, *OP_REGEX_NMATCH, 
   *OP_OBJECT_FUNC_REF, *OP_NEW, *OP_SHIFT, *OP_POP, *OP_PUSH, *OP_EXISTS, 
   *OP_SINGLE_ASSIGN, *OP_UNSHIFT, *OP_REGEX_SUBST, *OP_LIST_ASSIGNMENT, 
   *OP_SPLICE, *OP_MODULA_EQUALS, *OP_MULTIPLY_EQUALS, *OP_DIVIDE_EQUALS,
   *OP_XOR_EQUALS, *OP_SHIFT_LEFT_EQUALS, *OP_SHIFT_RIGHT_EQUALS, *OP_INSTANCEOF,
   *OP_REGEX_TRANS, *OP_REGEX_EXTRACT;

class OperatorList {
   private:
      int num;
      class Operator *head, *tail;

   public:
      inline OperatorList()
      {
	 head = tail = NULL;
	 num = 0;
      }
      inline ~OperatorList();
      inline class Operator *add(class Operator *o);
      inline int size()
      {
	 return num;
      }
      class Operator *getHead() { return head; }
};

extern class OperatorList oplist;

void operatorsInit();

typedef class QoreNode *(* op_func_t)(class QoreNode *l, class QoreNode *r, class ExceptionSink *xsink);

class OperatorFunction {
   public:
      class QoreType *ltype;
      class QoreType *rtype;
      op_func_t op_func;
};

class Operator {
   private:
      int functionsAllocated;
      int (*opMatrix)[NUM_VALUE_TYPES];

      int findFunction(class QoreType *ltype, class QoreType *rtype);
      bool effect, lvalue;

   public:
      int args;
      char *name;
      char *description;
      int evalArgs;
      int numFunctions;
      class OperatorFunction *functions;
      class Operator *next;

      inline Operator(int arg, char *n, char *desc, int ev, bool eff, bool lv = false)
      {
	 numFunctions = 0;
	 functionsAllocated = 0;
	 functions = NULL;
	 args = arg;
	 name = n;
	 description = desc;
	 evalArgs = ev;
	 opMatrix = NULL;
	 next = NULL;
	 effect = eff;
	 lvalue = lv;
      }
      inline ~Operator()
      {
	 if (functions)
	    free(functions);
	 if (opMatrix)
	    delete [] opMatrix;
      }
      inline bool hasEffect() { return effect; }
      inline bool needsLValue() { return lvalue; }
      inline void addFunction(class QoreType *lt, class QoreType *rt, class QoreNode *(*f)(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink));
      //inline void removeFunction(int lt, int rt, class QoreNode *(*f)(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink));
      class QoreNode *eval(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink);
      void init();
};

#include <qore/common.h>
#include <qore/support.h>
#include <qore/QoreNode.h>

inline OperatorList::~OperatorList()
{
   while (head)
   {
      class Operator *w = head->next;
      delete head;
      head = w;
   }
}

inline class Operator *OperatorList::add(class Operator *o)
{
   if (!tail)
      head = o;
   else
      tail->next = o;
   tail = o;
   return o;
}

// if there is no exact match, the first partial match
// counts as a match
static inline int match(class QoreType *ntype, class QoreType *rtype)
{
   // if any type is OK, or an exact match
   if (rtype == NT_ALL || ntype == rtype || (rtype == NT_VARREF && ntype == NT_SELF_VARREF))
      return 1;
   // otherwise fail
   else
      return 0;
}

inline int Operator::findFunction(class QoreType *ltype, class QoreType *rtype)
{
   int i, m = -1;

   //tracein("Operator::findFunction()");
   //printd(5, "Operator::findFunction() %s: ltype=%d rtype=%d total=%d\n", description, ltype, rtype, numFunctions);
   // loop through all operator functions
   for (i = 0; i < numFunctions; i++)
   {
      // check for a match on the left side
      if (match(ltype, functions[i].ltype))
      {
	 /* if there is only one operator or there is also
	  * a match on the right side, return */
	 if ((args == 1) || 
	     ((args == 2) && match(rtype, functions[i].rtype)))
	    return i;
	 if (m == -1)
	    m = i;
	 continue;
      }
      if ((args == 2) && match(rtype, functions[i].rtype) 
	  && (m == -1))
	 m = i;
   }
/* if there is no match of any kind, take the highest priority function
 * (row 0), and try to convert the arguments, otherwise return the best 
 * partial match
 */
   //traceout("Operator::findFunction()");
   return m == -1 ? 0 : m;
}

#define OPFUNC_BLOCK 10
inline void Operator::addFunction(class QoreType *lt, class QoreType *rt, class QoreNode *(*f)(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink))
{
   // resize function array if necessary
   if (numFunctions == functionsAllocated)
   {
      functionsAllocated += OPFUNC_BLOCK;
      functions = (OperatorFunction *)realloc(functions, sizeof (OperatorFunction) * functionsAllocated);
   }
   functions[numFunctions].ltype = lt;
   functions[numFunctions].rtype = rt;
   functions[numFunctions++].op_func = f;
}

/*
inline void Operator::removeFunction(int lt, int rt, class QoreNode *(*f)(class QoreNode *l, class QoreNode *r, ExceptionSink *xsink))
{
   int i;

   for (i = 0; i < numFunctions; i++)
      if (functions[i].ltype == lt &&
	  functions[i].rtype == rt && 
	  functions[i].op_func == f)
      {
	 numFunctions--;
	 if (i == numFunctions)
	    return;
	 memmove(&functions[i], &functions[i+1], sizeof(OperatorFunction) * (numFunctions - i));
      }
#ifdef DEBUG
   run_time_error("Operator(%s)::removeFunction(%d, %d, %08x) cannot be found in the list!",
	  description, lt, rt, f);
#endif
}
*/

#endif
