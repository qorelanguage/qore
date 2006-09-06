/*
  QC_GetOpt.cc

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

#include <qore/config.h>
#include <qore/common.h>
#include <qore/thread.h>
#include <qore/QC_GetOpt.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/support.h>

int CID_GETOPT;

static void getGetOpt(void *obj)
{
   ((GetOpt *)obj)->ROreference();
}

static void releaseGetOpt(void *obj)
{
   ((GetOpt *)obj)->deref();
}

static inline int process_type(char *key, int &attributes, char *opt, class QoreType *&at, class ExceptionSink *xsink)
{
   // get type
   switch (*opt)
   {
      case 's':
	 at = NT_STRING;
	 break;
      case 'i':
	 at = NT_INT;
	 break;
      case 'f':
	 at = NT_FLOAT;
	 break;
      case 'b':
	 at = NT_BOOLEAN;
	 break;
      case 'd':
	 at = NT_DATE;
	 break;
      case 'h':
	 at = NT_HASH;
	 break;
      case '@':
	 at = NT_STRING;
	 attributes |= QGO_OPT_LIST;
	 break;
      case '+':
	 at = NT_INT;
	 attributes |= QGO_OPT_ADDITIVE;
	 break;
   }
   if (!at)
   {
      xsink->raiseException("GETOPT-OPTION-ERROR", "type '%c' for key '%s' is unknown", *opt, key);
      return -1;
   }
   if (!opt[1])
      return 0;

   if (opt[2])
   {
      xsink->raiseException("GETOPT-OPTION-ERROR", "invalid attributes in option '%s'", key);
      return -1;
   }

   // process modifiers
   if (opt[1] == '@')
   {
      if (attributes & QGO_OPT_LIST)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "list attribute doubled in option key '%s'", key);
	 return -1;
      }
      if (attributes & QGO_OPT_ADDITIVE)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' cannot have both additive and list attributes turned on", key);
	 return -1;
      }
      attributes |= QGO_OPT_LIST;
      return 0;
   }
   if (opt[1] == '+')
   {
      if (attributes & QGO_OPT_ADDITIVE)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "additive attribute doubled in option key '%s'", key);
	 return -1;
      }
      if (attributes & QGO_OPT_LIST)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' cannot have both additive and list attributes turned on", key);
	 return -1;
      }
      if (at != NT_INT && at != NT_FLOAT)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "additive attributes for type '%s' are not supported (option '%s')", at->name, key);
	 return -1;
      }
      attributes |= QGO_OPT_ADDITIVE;
      return 0;
   }

   xsink->raiseException("GETOPT-OPTION-ERROR", "unknown modifier '%c' in option '%s'", opt[1], key);
   return -1;
}

static void GETOPT_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = test_param(params, NT_HASH, 0);
   if (!p0)
   {
      xsink->raiseException("GETOPT-PARAMETER-ERROR", "expecting hash as first argument to GetOpt::constructor()");
      return;
   }

   class GetOpt *g = new GetOpt();

   class HashIterator hi(p0->val.hash);
   class QoreString vstr;
   while (hi.next())
   {
      char *k = hi.getKey();
      if (!strcmp(k, "_ERRORS_"))
      {
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "option key '%s' is reserved for errors in the output hash", k);
	 break;
      }

      class QoreNode *v = hi.getValue();
      if (!v || v->type != NT_STRING)
      {
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "value of option key '%s' is not a string (%s)", k,
			v ? v->type->name : "NOTHING");
	 break;
      }
      class QoreType *at = NULL;
      char *long_opt = NULL, short_opt = '\0';
      int attributes = QGO_OPT_NONE;

      // reset buffer
      vstr.terminate(0);
      vstr.concat(v->val.String->getBuffer());
      char *val = vstr.getBuffer();

      // get data type, if any
      char *tok = strchrs(val, "=:");
      if (tok)
      {
	 if (tok[1] && process_type(k, attributes, tok + 1, at, xsink))
	    break;
	 if ((*tok) == '=')
	    attributes |= QGO_OPT_MANDATORY;
	 else if (attributes & QGO_OPT_LIST)
	 {
	    xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' takes a list and therefore must have mandatory arguments", k);
	    break;
	 }
	 
	 (*tok) = '\0';
      }
      // get option names
      if (!val[0])
      {
	 //printd(5, "making exception key='%s' tok=%08x val=%08x val='%s'\n", k, tok, val, val);
	 xsink->raiseException("GETOPT-PARAMETER-ERROR", "value of option key '%s' has no option specifiers", k);
	 break;
      }
      tok = index(val, ',');
      if (tok)
      {
	 if (tok == (val + 1))
	 {
	    short_opt = val[0];
	    long_opt = val + 2;
	 }
	 else if (tok - val == (signed)(strlen(val) - 2))
	 {
	    (*tok) = 0;
	    short_opt = tok[1];
	    long_opt = val;
	 }
	 else // if the comma is not in the second or second-to-last position, then it's an error
	 {
	    xsink->raiseException("GETOPT-OPTION-ERROR", "user options can only be specified with one short option and one long option, however two long options were given for key '%s' (%s)", k, val);
	    break;
	 }
      }
      else if (val[1])
	 long_opt = val;
      else
	 short_opt = val[0];
      int rc = g->add(k, short_opt, long_opt, at, attributes);
      if (rc == QGO_ERR_DUP_SHORT_OPT)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "short option '%c' was duplicated in key '%s'", short_opt, k);
	 break;
      }
      if (rc == QGO_ERR_DUP_LONG_OPT)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "long option '%s' was duplicated in key '%s'", long_opt, k);
	 break;
      }
      if (rc == QGO_ERR_DUP_NAME)
      {
	 xsink->raiseException("GETOPT-OPTION-ERROR", "option '%s' was duplicated", k);
	 break;
      }
   }
   if (!xsink->isException())
      self->setPrivate(CID_GETOPT, g, getGetOpt, releaseGetOpt);
   else
      g->deref();
}

static void GETOPT_destructor(class Object *self, class GetOpt *g, ExceptionSink *xsink)
{
   g->deref();
}

static void GETOPT_copy(class Object *self, class Object *old, class GetOpt *g, class ExceptionSink *xsink)
{
   xsink->raiseException("GETOPT-COPY-ERROR", "copying GetOpt objects is not supported");
}

class QoreNode *GETOPT_parse(class Object *self, class GetOpt *g, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0 = get_param(params, 0);
   if (!p0)
      return NULL;

   class List *l;
   class VLock vl;
   bool modify = false;
   if (p0->type == NT_REFERENCE)
   {
      class QoreNode **vp = get_var_value_ptr(p0->val.lvexp, &vl, xsink);
      if (xsink->isEvent() || !(*vp) || (*vp)->type != NT_LIST)
	 return NULL;
      if ((*vp)->reference_count() > 1)
      {
	 class QoreNode *n = (*vp)->realCopy(xsink);
	 (*vp)->deref(xsink);
	 (*vp) = n;
      }
      l = (*vp)->val.list;
      modify = true;
   }
   else if (p0->type == NT_LIST)
      l = p0->val.list;
   else
      return NULL;

   class Hash *h = g->parse(l, modify, xsink);
   if (h)
      return new QoreNode(h);

   return NULL;
}

class QoreClass *initGetOptClass()
{
   tracein("initGetOptClass()");

   class QoreClass *QC_GETOPT = new QoreClass(strdup("GetOpt"));
   CID_GETOPT = QC_GETOPT->getID();
   QC_GETOPT->setConstructor(GETOPT_constructor);
   QC_GETOPT->setDestructor((q_destructor_t)GETOPT_destructor);
   QC_GETOPT->setCopy((q_copy_t)GETOPT_copy);
   QC_GETOPT->addMethod("parse",         (q_method_t)GETOPT_parse);

   traceout("initGetOptClass()");
   return QC_GETOPT;
}
