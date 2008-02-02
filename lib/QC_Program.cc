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

#include <qore/Qore.h>
#include <qore/intern/QC_Program.h>

int CID_PROGRAM;

static void PROGRAM_constructor(class QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   class AbstractQoreNode *p0;
   int parse_opt;

   if ((p0 = get_param(params, 0)))
      parse_opt = p0->getAsInt();
   else
      parse_opt = 0;

   self->setPrivate(CID_PROGRAM, new QoreProgram(getProgram(), parse_opt));
}

static void PROGRAM_copy(class QoreObject *self, class QoreObject *old, class QoreProgram *p, class ExceptionSink *xsink)
{
   xsink->raiseException("PROGRAM-COPY-ERROR", "copying Program objects is currently unsupported");
}

static AbstractQoreNode *PROGRAM_parse(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)) || !(p1 = test_string_param(params, 1)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parse()");
      return NULL;
   }

   // see if a warning mask was passed
   AbstractQoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask)
   {
      p->parse(p0, p1, xsink);
      return NULL;
   }
   ExceptionSink wsink;
   p->parse(p0, p1, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return NULL;

   class QoreException *e = wsink.catchException();
   class AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parsePending(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)) || !(p1 = test_string_param(params, 1)))
   {
      xsink->raiseException("PROGRAM-PARSE-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parsePending()");
      return NULL;
   }

   // see if a warning mask was passed
   AbstractQoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask)
   {
      p->parsePending(p0, p1, xsink);
      return NULL;
   }
   ExceptionSink wsink;
   p->parsePending(p0, p1, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return NULL;

   class QoreException *e = wsink.catchException();
   class AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parseCommit(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   // see if a warning mask was passed
   AbstractQoreNode *pt = get_param(params, 0);
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

   class QoreException *e = wsink.catchException();
   class AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parseRollback(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->parseRollback();
   return NULL;
}

static AbstractQoreNode *PROGRAM_callFunction(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunction()");
      return 0;
   }

   ReferenceHolder<QoreListNode> args(xsink);
   if (params->size() > 1)
   {
      args = params->evalFrom(1, xsink);
      if (*xsink)
	 return 0;
   }

   return p->callFunction(p0->getBuffer(), *args, xsink);
}

static AbstractQoreNode *PROGRAM_callFunctionArgs(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   AbstractQoreNode *p1;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunctionArgs()");
      return NULL;
   }

   QoreListNode *args;

   p1 = get_param(params, 1);
   if (is_nothing(p1))
      args = NULL;
   else if (!(args = dynamic_cast<QoreListNode *>(p1)))
   {
      args = new QoreListNode();
      args->push(p1);
   }

   AbstractQoreNode *rv = p->callFunction(p0->getBuffer(), args, xsink);
   
   if (args && p1 != args)
   {
      args->shift();
      args->deref(xsink);
   }

   return rv;
}

static AbstractQoreNode *PROGRAM_existsFunction(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return NULL;

   return new QoreBoolNode(p->existsFunction(p0->getBuffer()));
}

static AbstractQoreNode *PROGRAM_run(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->run(xsink);
}

static AbstractQoreNode *PROGRAM_importFunction(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::importUserFunction()");
      return NULL;
   }
   const char *func = p0->getBuffer();

   getProgram()->exportUserFunction(func, p, xsink);
   return NULL;
}

static AbstractQoreNode *PROGRAM_importGlobalVariable(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreStringNode *p0;
   AbstractQoreNode *p1;
   bool readonly;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-PARAMETER-ERROR", "expecting variable-name(string) as argument to QoreProgram::importUserFunction()");
      return NULL;
   }

   p1 = get_param(params, 1);
   if (p1)
      readonly = p1->getAsBool();
   else
      readonly = false;
      
   class Var *var = getProgram()->findVar(p0->getBuffer());
   if (var)
      p->importGlobalVariable(var, xsink, readonly);
   else
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", p0->getBuffer());
   return NULL;
}

static AbstractQoreNode *PROGRAM_getUserFunctionList(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->getUserFunctionList();
}

static class QoreClass *PROGRAM_setParseOptions(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   int opt;
   AbstractQoreNode *p0 = get_param(params, 0);
   if (p0)
      opt = p0->getAsInt();
   else
      opt = 0;

   p->setParseOptions(opt, xsink);
   return NULL;
}

static class QoreClass *PROGRAM_disableParseOptions(class QoreObject *self, class QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   int opt;
   AbstractQoreNode *p0 = get_param(params, 0);
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

   class QoreClass *QC_PROGRAM = new QoreClass("Program");
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
