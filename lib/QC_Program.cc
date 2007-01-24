/*
  QC_Program.cc

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols

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
#include <qore/QC_Program.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>
#include <qore/Variable.h>

int CID_PROGRAM;

static void PROGRAM_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   int parse_opt;

   if ((p0 = get_param(params, 0)))
      parse_opt = p0->getAsInt();
   else
      parse_opt = 0;

   self->setPrivate(CID_PROGRAM, new QoreProgram(getProgram(), parse_opt));
}

static void PROGRAM_copy(class Object *self, class Object *old, class QoreProgram *p, class ExceptionSink *xsink)
{
   xsink->raiseException("PROGRAM-COPY-ERROR", "copying Program objects is currently unsupported");
}

static QoreNode *PROGRAM_parse(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parse()");
      return NULL;
   }

   // see if a warning mask was passed
   QoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask)
   {
      p->parse(p0->val.String, p1->val.String, xsink);
      return NULL;
   }
   ExceptionSink wsink;
   p->parse(p0->val.String, p1->val.String, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return NULL;

   class Exception *e = wsink.catchException();
   class QoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static QoreNode *PROGRAM_parsePending(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("PROGRAM-PARSE-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parsePending()");
      return NULL;
   }

   // see if a warning mask was passed
   QoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask)
   {
      p->parsePending(p0->val.String, p1->val.String, xsink);
      return NULL;
   }
   ExceptionSink wsink;
   p->parsePending(p0->val.String, p1->val.String, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return NULL;

   class Exception *e = wsink.catchException();
   class QoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static QoreNode *PROGRAM_parseCommit(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   // see if a warning mask was passed
   QoreNode *pt = get_param(params, 0);
   int warning_mask = pt ? pt->getAsInt() : 0;

   if (!warning_mask)
   {
      p->parseCommit(xsink);
      return NULL;
   }
   ExceptionSink wsink;
   p->parseCommit(xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return NULL;

   class Exception *e = wsink.catchException();
   class QoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static QoreNode *PROGRAM_parseRollback(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   p->parseRollback();
   return NULL;
}

static QoreNode *PROGRAM_callFunction(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunction()");
      return NULL;
   }

   QoreNode *args;
   if (params->val.list->size() > 1)
   {
      args = params->val.list->evalFrom(1, xsink);
      if (xsink->isEvent())
      {
	 if (args)
	    args->deref(xsink);
	 return NULL;
      }
   }
   else
      args = NULL;

   class QoreNode *rv = p->callFunction(p0->val.String->getBuffer(), args, xsink);

   if (args)
      args->deref(xsink);

   return rv;
}

static QoreNode *PROGRAM_callFunctionArgs(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunctionArgs()");
      return NULL;
   }
   QoreNode *args;
   p1 = get_param(params, 1);
   if (is_nothing(p1))
      args = NULL;
   else if (p1->type == NT_LIST)
      args = p1;
   else
   {
      args = new QoreNode(new List());
      args->val.list->push(p1);      
   }

   QoreNode *rv = p->callFunction(p0->val.String->getBuffer(), args, xsink);
   
   if (args && p1 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }

   return rv;
}

static QoreNode *PROGRAM_existsFunction(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   return new QoreNode(p->existsFunction(p0->val.String->getBuffer()));
}

static QoreNode *PROGRAM_run(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   return p->run(xsink);
}

static QoreNode *PROGRAM_importFunction(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::importUserFunction()");
      return NULL;
   }
   char *func = p0->val.String->getBuffer();

   getProgram()->exportUserFunction(func, p, xsink);
   return NULL;
}

static QoreNode *PROGRAM_importGlobalVariable(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;
   bool readonly;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-PARAMETER-ERROR", "expecting variable-name(string) as argument to QoreProgram::importUserFunction()");
      return NULL;
   }
   p1 = get_param(params, 1);
   if (p1)
      readonly = p1->getAsBool();
   else
      readonly = false;
      
   class Var *var = getProgram()->findVar(p0->val.String->getBuffer());
   if (var)
      p->importGlobalVariable(var, xsink, readonly);
   else
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", p0->val.String->getBuffer());
   return NULL;
}

static QoreNode *PROGRAM_getUserFunctionList(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(p->getUserFunctionList());
}

static class QoreClass *PROGRAM_setParseOptions(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   int opt;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      opt = p0->getAsInt();
   else
      opt = 0;

   p->setParseOptions(opt, xsink);
   return NULL;
}

static class QoreClass *PROGRAM_disableParseOptions(class Object *self, class QoreProgram *p, class QoreNode *params, ExceptionSink *xsink)
{
   int opt;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      opt = p0->getAsInt();
   else
      opt = 0;

   p->disableParseOptions(opt, xsink);
   return NULL;
}

class QoreClass *initProgramClass()
{
   tracein("initProgramClass()");

   class QoreClass *QC_PROGRAM = new QoreClass(strdup("Program"));
   CID_PROGRAM = QC_PROGRAM->getID();
   QC_PROGRAM->setConstructor(PROGRAM_constructor);
   QC_PROGRAM->setCopy((q_copy_t)PROGRAM_copy);
   QC_PROGRAM->addMethod("parse",                (q_method_t)PROGRAM_parse);
   QC_PROGRAM->addMethod("parsePending",         (q_method_t)PROGRAM_parsePending);
   QC_PROGRAM->addMethod("parseRollback",        (q_method_t)PROGRAM_parseRollback);
   QC_PROGRAM->addMethod("parseCommit",          (q_method_t)PROGRAM_parseCommit);
   QC_PROGRAM->addMethod("callFunction",         (q_method_t)PROGRAM_callFunction);
   QC_PROGRAM->addMethod("callFunctionArgs",     (q_method_t)PROGRAM_callFunctionArgs);
   QC_PROGRAM->addMethod("existsFunction",       (q_method_t)PROGRAM_existsFunction);
   QC_PROGRAM->addMethod("run",                  (q_method_t)PROGRAM_run);
   QC_PROGRAM->addMethod("importFunction",       (q_method_t)PROGRAM_importFunction);
   QC_PROGRAM->addMethod("importGlobalVariable", (q_method_t)PROGRAM_importGlobalVariable);
   QC_PROGRAM->addMethod("getUserFunctionList",  (q_method_t)PROGRAM_getUserFunctionList);
   QC_PROGRAM->addMethod("setParseOptions",      (q_method_t)PROGRAM_setParseOptions);
   QC_PROGRAM->addMethod("disableParseOptions",  (q_method_t)PROGRAM_disableParseOptions);

   traceout("initProgramClass()");
   return QC_PROGRAM;
}
