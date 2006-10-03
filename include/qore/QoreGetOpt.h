/*
  QoreGetOpt.h

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

#ifndef _QORE_QOREGETOPT_H

#define _QORE_QOREGETOPT_H

#define QGO_ERR_DUP_SHORT_OPT -1
#define QGO_ERR_DUP_LONG_OPT  -2
#define QGO_ERR_DUP_NAME      -3
#define QGO_ERR_NO_NAME       -4
#define QGO_ERR_NO_OPTION     -5

#define QGO_OPT_NONE      0
#define QGO_OPT_ADDITIVE  1
#define QGO_OPT_LIST      2
#define QGO_OPT_MANDATORY 4

#define QGO_OPT_LIST_OR_ADD (QGO_OPT_ADDITIVE|QGO_OPT_LIST)

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class QoreGetOptNode {
   public:
      char *name;
      char short_opt;
      char *long_opt;
      class QoreType *argtype;
      int option;

      class QoreGetOptNode *next;

      inline QoreGetOptNode(char *n, char so, char *lo, class QoreType *at = NULL, int o = QGO_OPT_NONE)
      {
	 name = n  ? strdup(n) : NULL;
	 short_opt = so;
	 long_opt = lo ? strdup(lo) : NULL;
	 argtype = at;
	 option = o;
      }
      inline ~QoreGetOptNode();
};

class QoreGetOpt {
   private:
      class QoreGetOptNode *head;
      
      class QoreNode *parseDate(char *val);
      void processLongArg(char *arg, class List *l, class Hash *h, int &i, bool modify);
      int processShortArg(char *arg, class List *l, class Hash *h, int &i, int &j, bool modify);
      inline QoreGetOptNode *find(char *opt)
      {
	 class QoreGetOptNode *w = head;
	 while (w)
	 {
	    if (w->long_opt && !strcmp(w->long_opt, opt))
	       return w;
	    w = w->next;
	 }
	 return NULL;
      }
      inline class QoreGetOptNode *find(char opt)
      {
	 class QoreGetOptNode *w = head;
	 while (w)
	 {
	    //printd(5, "QGO::find(%c) w=%08p %c next=%08p\n", opt, w, w->short_opt, w->next);
	    if (w->short_opt == opt)
	       return w;
	    w = w->next;
	 }
	 return NULL;
      }
      void doOption(class QoreGetOptNode *n, class Hash *h, char *val);
      char *getNextArgument(class List *l, class Hash *h, int &i, char *lopt, char sopt);

   public:
      inline QoreGetOpt()
      {
	 head = NULL;
      }
      inline ~QoreGetOpt()
      {
	 while (head)
	 {
	    class QoreGetOptNode *w = head->next;
	    delete head;
	    head = w;
	 }
      }
      // returns 0 for OK
      int add(char *name, char short_opt, char *long_opt, class QoreType *argtype = NULL, int option = QGO_OPT_NONE);
      class Hash *parse(class List *l, bool ml, class ExceptionSink *xsink);
};

#include <qore/QoreNode.h>

inline QoreGetOptNode::~QoreGetOptNode()
{
   if (name)
      free(name);
   if (long_opt)
      free(long_opt);
}

#endif // _QORE_QOREGETOPT_H
