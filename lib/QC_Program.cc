/*
  QC_Program.cc

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

qore_classid_t CID_PROGRAM;

static void PROGRAM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0;
   int parse_opt;

   if ((p0 = get_param(params, 0)))
      parse_opt = p0->getAsInt();
   else
      parse_opt = PO_DEFAULT;

   self->setPrivate(CID_PROGRAM, new QoreProgram(getProgram(), parse_opt));
}

static void PROGRAM_copy(QoreObject *self, QoreObject *old, QoreProgram *p, ExceptionSink *xsink)
{
   xsink->raiseException("PROGRAM-COPY-ERROR", "copying Program objects is currently unsupported");
}

static AbstractQoreNode *PROGRAM_parse(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)) || !(p1 = test_string_param(params, 1))) {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parse()");
      return 0;
   }

   // format label as "<run-time-loaded: xxx>"
   QoreString label("<run-time-loaded: ");
   label.concat(p1, xsink);
   if (*xsink)
      return 0;
   label.concat('>');

   // see if a warning mask was passed
   const AbstractQoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask) {
      p->parse(p0, &label, xsink);
      return 0;
   }

   ExceptionSink wsink;
   p->parse(p0, &label, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parsePending(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0, *p1;

   if (!(p0 = test_string_param(params, 0)) || !(p1 = test_string_param(params, 1))) {
      xsink->raiseException("PROGRAM-PARSE-PARAMETER-ERROR", "expecting code(string), label(string) as arguments to Program::parsePending()");
      return 0;
   }

   // format label as "<run-time-loaded: xxx>"
   QoreString label("<run-time-loaded: ");
   label.concat(p1, xsink);
   if (*xsink)
      return 0;
   label.concat('>');

   // see if a warning mask was passed
   const AbstractQoreNode *p2 = get_param(params, 2);
   int warning_mask = p2 ? p2->getAsInt() : 0;

   if (!warning_mask) {
      p->parsePending(p0, &label, xsink);
      return 0;
   }
   ExceptionSink wsink;
   p->parsePending(p0, &label, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parseCommit(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   // see if a warning mask was passed
   const AbstractQoreNode *pt = get_param(params, 0);
   int warning_mask = pt ? pt->getAsInt() : 0;

   if (!warning_mask) {
      p->parseCommit(xsink);
      return 0;
   }
   ExceptionSink wsink;
   p->parseCommit(xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   AbstractQoreNode *rv = e->makeExceptionObjectAndDelete(xsink);
   return rv;
}

static AbstractQoreNode *PROGRAM_parseRollback(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->parseRollback();
   return 0;
}

static AbstractQoreNode *PROGRAM_callFunction(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunction()");
      return 0;
   }

   ReferenceHolder<QoreListNode> args(xsink);
   if (params->size() > 1)
      args = params->copyListFrom(1);

   return p->callFunction(p0->getBuffer(), *args, xsink);
}

static AbstractQoreNode *PROGRAM_callFunctionArgs(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;

   if (!(p0 = test_string_param(params, 0))) {
      xsink->raiseException("PROGRAM-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::callFunctionArgs()");
      return 0;
   }

   QoreListNode *args;

   p1 = get_param(params, 1);
   if (is_nothing(p1))
      args = 0;
   else if (!(args = const_cast<QoreListNode *>(dynamic_cast<const QoreListNode *>(p1)))) {
      args = new QoreListNode();
      args->push(const_cast<AbstractQoreNode *>(p1));
   }

   AbstractQoreNode *rv = p->callFunction(p0->getBuffer(), args, xsink);
   
   if (args && p1 != args) {
      args->shift();
      args->deref(xsink);
   }

   return rv;
}

static AbstractQoreNode *PROGRAM_existsFunction(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
      return 0;

   return get_bool_node(p->existsFunction(p0->getBuffer()));
}

static AbstractQoreNode *PROGRAM_run(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->run(xsink);
}

static AbstractQoreNode *PROGRAM_importFunction(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTFUNCTION-PARAMETER-ERROR", "expecting function-name(string) as argument to QoreProgram::importUserFunction()");
      return 0;
   }
   const char *func = p0->getBuffer();

   getProgram()->exportUserFunction(func, p, xsink);
   return 0;
}

static AbstractQoreNode *PROGRAM_importGlobalVariable(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0;
   const AbstractQoreNode *p1;

   if (!(p0 = test_string_param(params, 0)))
   {
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-PARAMETER-ERROR", "expecting variable-name(string) as argument to QoreProgram::importUserFunction()");
      return 0;
   }

   p1 = get_param(params, 1);
   bool readonly = p1 ? p1->getAsBool() : false;
      
   class Var *var = getProgram()->findGlobalVar(p0->getBuffer());
   if (var)
      p->importGlobalVariable(var, xsink, readonly);
   else
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", p0->getBuffer());
   return 0;
}

static AbstractQoreNode *PROGRAM_getUserFunctionList(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->getUserFunctionList();
}

static AbstractQoreNode *PROGRAM_setParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int opt = p0 ? p0->getAsInt() : PO_DEFAULT;

   p->setParseOptions(opt, xsink);
   return 0;
}

static AbstractQoreNode *PROGRAM_getParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(p->getParseOptions());
}

static AbstractQoreNode *PROGRAM_disableParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const AbstractQoreNode *p0 = get_param(params, 0);
   int opt = p0 ? p0->getAsInt() : PO_DEFAULT;

   p->disableParseOptions(opt, xsink);
   return 0;
}

static AbstractQoreNode *PROGRAM_setScriptPath(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   const QoreStringNode *p0 = test_string_param(params, 0);

   p->setScriptPath(p0 ? p0->getBuffer() : 0);
   return 0;
}

static AbstractQoreNode *PROGRAM_getScriptDir(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->getScriptDir();
}

static AbstractQoreNode *PROGRAM_getScriptName(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->getScriptName();
}

static AbstractQoreNode *PROGRAM_getScriptPath(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   return p->getScriptPath();
}

static AbstractQoreNode *PROGRAM_lockOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink)
{
   p->lockOptions();
   return 0;
}

static AbstractQoreNode *PROGRAM_getGlobalVariable(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
    const QoreStringNode *str = test_string_param(params, 0);
    if (!str) {
	xsink->raiseException("PROGRAM-GET-GLOBAL-VARIABLE-ERROR", "missing string argument giving the variable name");
	return 0;
    }

    TempEncodingHelper t(str, QCS_DEFAULT, xsink);
    if (!t)
	return 0;

    const ReferenceNode *ref = test_reference_param(params, 1);

    bool found;
    ReferenceHolder<AbstractQoreNode> rv(p->getGlobalVariableValue(t->getBuffer(), found), xsink);

    if (ref) {
	AutoVLock vl(xsink);
	ReferenceHelper r(ref, vl, xsink);
	if (!r)
	    return 0;

	if (r.assign(get_bool_node(found), xsink))
	    return 0;
    }

    return rv.release();
}

QoreClass *initProgramClass() {
   QORE_TRACE("initProgramClass()");

   QoreClass *QC_PROGRAM = new QoreClass("Program");
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
   QC_PROGRAM->addMethod("getParseOptions",      (q_method_t)PROGRAM_getParseOptions);
   QC_PROGRAM->addMethod("setParseOptions",      (q_method_t)PROGRAM_setParseOptions);
   QC_PROGRAM->addMethod("disableParseOptions",  (q_method_t)PROGRAM_disableParseOptions);
   QC_PROGRAM->addMethod("setScriptPath",        (q_method_t)PROGRAM_setScriptPath);
   QC_PROGRAM->addMethod("getScriptDir",         (q_method_t)PROGRAM_getScriptDir);
   QC_PROGRAM->addMethod("getScriptName",        (q_method_t)PROGRAM_getScriptName);
   QC_PROGRAM->addMethod("getScriptPath",        (q_method_t)PROGRAM_getScriptPath);
   QC_PROGRAM->addMethod("lockOptions",          (q_method_t)PROGRAM_lockOptions);
   QC_PROGRAM->addMethod("getGlobalVariable",    (q_method_t)PROGRAM_getGlobalVariable);

   return QC_PROGRAM;
}
