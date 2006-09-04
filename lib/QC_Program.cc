/*
  QC_Program.cc

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
#include <qore/QC_Program.h>
#include <qore/QoreClass.h>
#include <qore/params.h>
#include <qore/Exception.h>
#include <qore/support.h>
#include <qore/Object.h>
#include <qore/Variable.h>

int CID_PROGRAM;

static inline void *getProgram(void *obj)
{
   ((QoreProgram *)obj)->ref();
   return obj;
}

static QoreNode *PROGRAM_constructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   class QoreNode *p0;
   int parse_opt;

   if ((p0 = get_param(params, 0)))
      parse_opt = p0->getAsInt();
   else
      parse_opt = 0;

   self->setPrivate(CID_PROGRAM, new QoreProgram(getProgram(), parse_opt), getProgram);
   return NULL;
}

static QoreNode *PROGRAM_destructor(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreProgram *p = (QoreProgram *)self->getAndClearPrivateData(CID_PROGRAM);
   if (p)
      p->deref(xsink);
   else
      alreadyDeleted(xsink, "Program::destructor");
   return NULL;
}

static QoreNode *PROGRAM_copy(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   xsink->raiseException("PROGRAM-COPY-ERROR", "copying Program objects is currently unsupported");
   return NULL;
}

static QoreNode *PROGRAM_parse(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parse()");
      return NULL;
   }

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   if (p)
   {
      p->parse(p0->val.String, p1->val.String, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::parse");
   return NULL;
}

static QoreNode *PROGRAM_parsePending(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0, *p1;

   if (!(p0 = test_param(params, NT_STRING, 0)) ||
       !(p1 = test_param(params, NT_STRING, 1)))
   {
      xsink->raiseException("PROGRAM-PARSE-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parsePending()");
      return NULL;
   }

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   if (p)
   {
      p->parsePending(p0->val.String, p1->val.String, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::parsePending");
   return NULL;
}

static QoreNode *PROGRAM_parseCommit(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   
   if (p)
   {
      p->parseCommit(xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::parseCommit");
    return NULL;
}

static QoreNode *PROGRAM_parseRollback(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   
   if (p)
   {
      p->parseRollback();
      p->deref();
   }
   else
       alreadyDeleted(xsink, "Program::parseRollback");
    return NULL;
}

static QoreNode *PROGRAM_callFunction(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   QoreNode *rv = NULL;

   if (p)
   {
      rv = p->callFunction(p0->val.String->getBuffer(), args, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::callFunction");

   if (args)
      args->deref(xsink);
   return rv;
}

static QoreNode *PROGRAM_callFunctionArgs(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   QoreNode *rv = NULL;

   if (p)
   {
      rv = p->callFunction(p0->val.String->getBuffer(), args, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::callFunctionArgs");
   
   if (args && p1 != args)
   {
      args->val.list->shift();
      args->deref(xsink);
   }

   return rv;
}

static QoreNode *PROGRAM_existsFunction(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
      return NULL;

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   QoreNode *rv;
   
   if (p)
   {
      rv = new QoreNode(NT_INT,  p->existsFunction(p0->val.String->getBuffer()));
      p->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Program::existsFunction");
   }
   return rv;
}

static QoreNode *PROGRAM_run(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   QoreNode *rv;
   
   if (p)
   {
      rv = p->run(xsink);
      p->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Program::run");
   }
   return rv;
}

static QoreNode *PROGRAM_importFunction(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p0;

   if (!(p0 = test_param(params, NT_STRING, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::importUserFunction()");
      return NULL;
   }
   char *func = p0->val.String->getBuffer();
   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   if (p)
   {
      getProgram()->exportUserFunction(func, p, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::importFunction");
   return NULL;
}

static QoreNode *PROGRAM_importGlobalVariable(class Object *self, class QoreNode *params, ExceptionSink *xsink)
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
   {
      class QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
      if (p)
      {
	 p->importGlobalVariable(var, xsink, readonly);
	 p->deref();
      }
      else
	 alreadyDeleted(xsink, "Program::importGlobalVariable");
   }
   else
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", p0->val.String->getBuffer());
   return NULL;
}

static QoreNode *PROGRAM_getUserFunctionList(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   QoreNode *rv;
   
   if (p)
   {
      rv = new QoreNode(p->getUserFunctionList());
      p->deref();
   }
   else
   {
      rv = NULL;
      alreadyDeleted(xsink, "Program::getUserFunctionList");
   }
   return rv;
}

class QoreClass *PROGRAM_setParseOptions(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int opt;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      opt = p0->getAsInt();
   else
      opt = 0;

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   
   if (p)
   {
      p->setParseOptions(opt, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::setParseOptions");

   return NULL;
}

class QoreClass *PROGRAM_disableParseOptions(class Object *self, class QoreNode *params, ExceptionSink *xsink)
{
   int opt;
   QoreNode *p0 = get_param(params, 0);
   if (p0)
      opt = p0->getAsInt();
   else
      opt = 0;

   QoreProgram *p = (QoreProgram *)self->getReferencedPrivateData(CID_PROGRAM);
   
   if (p)
   {
      p->disableParseOptions(opt, xsink);
      p->deref();
   }
   else
      alreadyDeleted(xsink, "Program::disableParseOptions");

   return NULL;
}

class QoreClass *initProgramClass()
{
   tracein("initProgramClass()");

   class QoreClass *QC_PROGRAM = new QoreClass(strdup("Program"));
   CID_PROGRAM = QC_PROGRAM->getID();
   QC_PROGRAM->addMethod("constructor",          PROGRAM_constructor);
   QC_PROGRAM->addMethod("destructor",           PROGRAM_destructor);
   QC_PROGRAM->addMethod("copy",                 PROGRAM_copy);
   QC_PROGRAM->addMethod("parse",                PROGRAM_parse);
   QC_PROGRAM->addMethod("parsePending",         PROGRAM_parsePending);
   QC_PROGRAM->addMethod("parseRollback",        PROGRAM_parseRollback);
   QC_PROGRAM->addMethod("parseCommit",          PROGRAM_parseCommit);
   QC_PROGRAM->addMethod("callFunction",         PROGRAM_callFunction);
   QC_PROGRAM->addMethod("callFunctionArgs",     PROGRAM_callFunctionArgs);
   QC_PROGRAM->addMethod("existsFunction",       PROGRAM_existsFunction);
   QC_PROGRAM->addMethod("run",                  PROGRAM_run);
   QC_PROGRAM->addMethod("importFunction",       PROGRAM_importFunction);
   QC_PROGRAM->addMethod("importGlobalVariable", PROGRAM_importGlobalVariable);
   QC_PROGRAM->addMethod("getUserFunctionList",  PROGRAM_getUserFunctionList);

   traceout("initProgramClass()");
   return QC_PROGRAM;
}
