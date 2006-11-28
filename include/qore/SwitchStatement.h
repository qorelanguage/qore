/*
  SwitchStatement.h

  Qore Programming Language

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

#ifndef _QORE_SWITCHSTATEMENT_H

#define _QORE_SWITCHSTATEMENT_H

class CaseNode {
   private:
      virtual bool isCaseNodeImpl() const;
   
   public:
      class QoreNode *val;
      class StatementBlock *code;
      class CaseNode *next;

      DLLLOCAL inline CaseNode(class QoreNode *v, class StatementBlock *c)
      {
	 val = v;
	 code = c;
	 next = NULL;
      }
      DLLLOCAL virtual bool matches(QoreNode* lhs_value) {
        return !compareHard(lhs_value, val); // the ! is because of compareHard() semantics
      }
      DLLLOCAL bool isCaseNode() const
      {
	 return isCaseNodeImpl();
      }
      DLLLOCAL inline virtual ~CaseNode();
};

class SwitchStatement {
   private:
      class CaseNode *head, *tail;
      class QoreNode *sexp;
      class CaseNode *deflt;

   public:
      class LVList *lvars;

      DLLLOCAL inline SwitchStatement(class CaseNode *f)
      {
	 deflt = NULL;
	 head = tail = f;
	 sexp = NULL;
	 lvars = NULL;
      }
      DLLLOCAL inline ~SwitchStatement()
      {
	 while (head)
	 {
	    class CaseNode *w = head->next;
	    delete head;
	    head = w;
	 }
	 if (sexp)
	    sexp->deref(NULL);
	 if (lvars)
	    delete lvars;
      }
      DLLLOCAL inline void setSwitch(class QoreNode *s)
      {
	 sexp = s;
      }
      DLLLOCAL inline void addCase(class CaseNode *c)
      {
	 if (tail)
	    tail->next = c;
	 else
	    head = c;
	 tail = c;
	 if (!c->val)
	 {
	    if (deflt)
	       parse_error("multiple defaults in switch statement");
	    deflt = c;
	 }
      }
      DLLLOCAL inline void parseInit(lvh_t oflag, int pflag = 0);
      DLLLOCAL inline int exec(class QoreNode **return_value, class ExceptionSink *xsink);
};

inline CaseNode::~CaseNode()
{
   if (val)
      val->deref(NULL);
   if (code)
      delete code;
}

#endif
