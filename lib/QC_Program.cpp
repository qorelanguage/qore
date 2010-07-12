/*
  QC_Program.cpp

  Qore Programming Language

  Copyright 2003 - 2010 David Nichols

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
#include <qore/intern/QC_TimeZone.h>

qore_classid_t CID_PROGRAM;

static void PROGRAM_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink) {
   self->setPrivate(CID_PROGRAM, new QoreProgram(getProgram(), HARD_QORE_INT(params, 0)));
}

static void PROGRAM_copy(QoreObject *self, QoreObject *old, QoreProgram *p, ExceptionSink *xsink) {
   xsink->raiseException("PROGRAM-COPY-ERROR", "copying Program objects is currently unsupported");
}

static AbstractQoreNode *PROGRAM_parse_str_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(params, 1);

   // format label as "<run-time-loaded: xxx>"
   QoreString label("<run-time-loaded: ");
   label.concat(p1, xsink);
   if (*xsink)
      return 0;
   label.concat('>');

   int warning_mask = get_int_param(params, 2);
   if (!warning_mask) {
      p->parse(p0, &label, xsink);
      return 0;
   }

   ExceptionSink wsink;
   p->parse(p0, &label, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   return e->makeExceptionObjectAndDelete(xsink);
}

static AbstractQoreNode *PROGRAM_parsePending_str_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(params, 1);

   // format label as "<run-time-loaded: xxx>"
   QoreString label("<run-time-loaded: ");
   label.concat(p1, xsink);
   if (*xsink)
      return 0;
   label.concat('>');

   int warning_mask = get_int_param(params, 2);
   if (!warning_mask) {
      p->parsePending(p0, &label, xsink);
      return 0;
   }
   ExceptionSink wsink;
   p->parsePending(p0, &label, xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   return e->makeExceptionObjectAndDelete(xsink);
}

static AbstractQoreNode *PROGRAM_parseCommit(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   // see if a warning mask was passed
   int warning_mask = get_int_param(params, 0);

   if (!warning_mask) {
      p->parseCommit(xsink);
      return 0;
   }
   ExceptionSink wsink;
   p->parseCommit(xsink, &wsink, warning_mask);
   if (!wsink.isException())
      return 0;

   QoreException *e = wsink.catchException();
   return e->makeExceptionObjectAndDelete(xsink);
}

static AbstractQoreNode *PROGRAM_parseRollback(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   p->parseRollback();
   return 0;
}

static AbstractQoreNode *PROGRAM_callFunction(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   ReferenceHolder<QoreListNode> args(xsink);
   if (params->size() > 1)
      args = params->copyListFrom(1);

   return p->callFunction(p0->getBuffer(), *args, xsink);
}

static AbstractQoreNode *PROGRAM_callFunctionArgs_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return p->callFunction(p0->getBuffer(), 0, xsink);
}

static AbstractQoreNode *PROGRAM_callFunctionArgs_str_something(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const AbstractQoreNode *p1 = get_param(params, 1);
   assert(p1);

   ReferenceHolder<QoreListNode> args(new QoreListNode, xsink);
   args->push(p1->refSelf());

   return p->callFunction(p0->getBuffer(), *args, xsink);
}

static AbstractQoreNode *PROGRAM_callFunctionArgs_str_list(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreListNode *args = HARD_QORE_LIST(params, 1);
   return p->callFunction(p0->getBuffer(), args, xsink);
}

static AbstractQoreNode *PROGRAM_existsFunction(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   return get_bool_node(p->existsFunction(p0->getBuffer()));
}

static AbstractQoreNode *PROGRAM_run(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return p->run(xsink);
}

// Program::importFunction(string $func) returns nothing
static AbstractQoreNode *PROGRAM_importFunction_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   getProgram()->exportUserFunction(p0->getBuffer(), p, xsink);
   return 0;
}

// Program::importFunction(string $func, string $new_name) returns nothing
static AbstractQoreNode *PROGRAM_importFunction_str_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   const QoreStringNode *p1 = HARD_QORE_STRING(params, 1);
   getProgram()->exportUserFunction(p0->getBuffer(), p1->getBuffer(), p, xsink);
   return 0;
}

// Program::importGlobalVariable(string $var, bool $readonly = False) returns nothing
static AbstractQoreNode *PROGRAM_importGlobalVariable(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   bool readonly = HARD_QORE_BOOL(params, 1);
      
   class Var *var = getProgram()->findGlobalVar(p0->getBuffer());
   if (var)
      p->importGlobalVariable(var, xsink, readonly);
   else
      xsink->raiseException("PROGRAM-IMPORTGLOBALVARIABLE-EXCEPTION", "there is no global variable \"%s\"", p0->getBuffer());
   return 0;
}

// Program::getUserFunctionList() returns list
static AbstractQoreNode *PROGRAM_getUserFunctionList(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return p->getUserFunctionList();
}

// Program::getParseOptions() returns int
static AbstractQoreNode *PROGRAM_getParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreBigIntNode(p->getParseOptions());
}

// Program::setParseOptions(int $opt = PO_DEFAULT) returns nothing
static AbstractQoreNode *PROGRAM_setParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   int opt = (int)HARD_QORE_INT(params, 0);
   p->setParseOptions(opt, xsink);
   return 0;
}

// Program::disableParseOptions(int $opt = PO_DEFAULT) returns nothing
static AbstractQoreNode *PROGRAM_disableParseOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   int opt = (int)HARD_QORE_INT(params, 0);
   p->disableParseOptions(opt, xsink);
   return 0;
}

// Program::setScriptPath() returns nothing
static AbstractQoreNode *PROGRAM_setScriptPath_nothing(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   p->setScriptPath(0);
   return 0;
}

// Program::setScriptPath(string $path) returns nothing
static AbstractQoreNode *PROGRAM_setScriptPath_str(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *p0 = HARD_QORE_STRING(params, 0);
   p->setScriptPath(p0->getBuffer());
   return 0;
}

// Program::getScriptDir() returns nothing|string
static AbstractQoreNode *PROGRAM_getScriptDir(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return p->getScriptDir();
}

// Program::getScriptName() returns nothing|string
static AbstractQoreNode *PROGRAM_getScriptName(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return p->getScriptName();
}

// Program::getScriptPath() returns nothing|string
static AbstractQoreNode *PROGRAM_getScriptPath(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return p->getScriptPath();
}

// Program::lockOptions() returns nothing
static AbstractQoreNode *PROGRAM_lockOptions(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   p->lockOptions();
   return 0;
}

// Program::getGlobalVariable(string $varname) returns any
// Program::getGlobalVariable(string $varname, reference $ref) returns any
static AbstractQoreNode *PROGRAM_getGlobalVariable(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   TempEncodingHelper t(str, QCS_DEFAULT, xsink);
   if (!t)
      return 0;

    const ReferenceNode *ref = test_reference_param(params, 1);

    bool found;
    ReferenceHolder<AbstractQoreNode> rv(p->getGlobalVariableValue(t->getBuffer(), found), xsink);

    if (ref) {
	AutoVLock vl(xsink);
	QoreTypeSafeReferenceHelper r(ref, vl, xsink);
	if (!r)
	    return 0;

	if (r.assign(get_bool_node(found), xsink))
	    return 0;
    }

    return rv.release();
}

// Program::setTimeZoneRegion(string $region) returns nothing
static AbstractQoreNode *PROGRAM_setTimeZoneRegion(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   const QoreStringNode *str = HARD_QORE_STRING(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findLoadRegion(str->getBuffer(), xsink);
   if (*xsink)
      return 0;

   p->setTZ(zone);
   return 0;
}

// Program::setTimeZoneUTCOffset(softint $seconds_east) returns nothing
static AbstractQoreNode *PROGRAM_setTimeZoneUTCOffset(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   int seconds_east = (int)HARD_QORE_INT(params, 0);
   const AbstractQoreZoneInfo *zone = QTZM.findCreateOffsetZone(seconds_east);
   p->setTZ(zone);
   return 0;
}

// Program::setTimeZone(TimeZone $zone) returns nothing
static AbstractQoreNode *PROGRAM_setTimeZone(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   HARD_QORE_OBJ_DATA(zone, TimeZoneData, params, 0, CID_TIMEZONE, "TimeZone", "Program::setTimeZone", xsink);
   if (*xsink)
      return 0;
   p->setTZ(zone->get());
   return 0;
}

// Program::getTimeZone() returns TimeZone
static AbstractQoreNode *PROGRAM_getTimeZone(QoreObject *self, QoreProgram *p, const QoreListNode *params, ExceptionSink *xsink) {
   return new QoreObject(QC_TIMEZONE, 0, new TimeZoneData(p->currentTZ()));
}

QoreClass *initProgramClass() {
   QORE_TRACE("initProgramClass()");

   QoreClass *QC_PROGRAM = new QoreClass("Program");
   CID_PROGRAM = QC_PROGRAM->getID();

   QC_PROGRAM->setConstructorExtended(PROGRAM_constructor, false, QC_NO_FLAGS, QDOM_DEFAULT, 1, softBigIntTypeInfo, new QoreBigIntNode(PO_DEFAULT));

   QC_PROGRAM->setCopy((q_copy_t)PROGRAM_copy);

   // Program::parse(string $code, string $label) returns nothing
   QC_PROGRAM->addMethodExtended("parse",                (q_method_t)PROGRAM_parse_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Program::parse(string $code, string $label, softint $warning_mask) returns nothing|hash
   QC_PROGRAM->addMethodExtended("parse",                (q_method_t)PROGRAM_parse_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // Program::parsePending(string $code, string $label) returns nothing
   QC_PROGRAM->addMethodExtended("parsePending",         (q_method_t)PROGRAM_parsePending_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Program::parsePending(string $code, string $label, softint $warning_mask) returns nothing|hash
   QC_PROGRAM->addMethodExtended("parsePending",         (q_method_t)PROGRAM_parsePending_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 3, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // Program::parseCommit() returns nothing
   QC_PROGRAM->addMethodExtended("parseCommit",          (q_method_t)PROGRAM_parseCommit, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   // Program::parseCommit(int $arning_mask) returns nothing|hash
   QC_PROGRAM->addMethodExtended("parseCommit",          (q_method_t)PROGRAM_parseCommit, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // Programm::parseRollback() returns nothing
   QC_PROGRAM->addMethodExtended("parseRollback",        (q_method_t)PROGRAM_parseRollback, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // Program::callFunction(string $name, ...) returns any
   QC_PROGRAM->addMethodExtended("callFunction",         (q_method_t)PROGRAM_callFunction, false, QC_USES_EXTRA_ARGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   QC_PROGRAM->addMethodExtended("callFunctionArgs",     (q_method_t)PROGRAM_callFunctionArgs_str_list, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, listTypeInfo, QORE_PARAM_NO_ARG);
   QC_PROGRAM->addMethodExtended("callFunctionArgs",     (q_method_t)PROGRAM_callFunctionArgs_str, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   QC_PROGRAM->addMethodExtended("callFunctionArgs",     (q_method_t)PROGRAM_callFunctionArgs_str_something, false, QC_NO_FLAGS, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, somethingTypeInfo, QORE_PARAM_NO_ARG);

   // Program::existsFunction() returns nothing
   QC_PROGRAM->addMethodExtended("existsFunction",       (q_method_t)class_noop, false, QC_RUNTIME_NOOP, QDOM_DEFAULT, nothingTypeInfo);
   // Program::existsFunction(string $str) returns bool
   QC_PROGRAM->addMethodExtended("existsFunction",       (q_method_t)PROGRAM_existsFunction, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, boolTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // Program::run() returns any
   QC_PROGRAM->addMethodExtended("run",                  (q_method_t)PROGRAM_run);

   // Program::importFunction() returns nothing
   QC_PROGRAM->addMethodExtended("importFunction",       (q_method_t)PROGRAM_importFunction_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Program::importFunction(string $func, string $new_name) returns nothing
   QC_PROGRAM->addMethodExtended("importFunction",       (q_method_t)PROGRAM_importFunction_str_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, stringTypeInfo, QORE_PARAM_NO_ARG);

   // Program::importGlobalVariable(string $var, bool $readonly = False) returns nothing
   QC_PROGRAM->addMethodExtended("importGlobalVariable", (q_method_t)PROGRAM_importGlobalVariable, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 2, stringTypeInfo, QORE_PARAM_NO_ARG, boolTypeInfo, &False);

   // Program::getUserFunctionList() returns list
   QC_PROGRAM->addMethodExtended("getUserFunctionList",  (q_method_t)PROGRAM_getUserFunctionList, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, listTypeInfo);

   // Program::getParseOptions() returns int
   QC_PROGRAM->addMethodExtended("getParseOptions",      (q_method_t)PROGRAM_getParseOptions, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, bigIntTypeInfo);

   // Program::setParseOptions(int $opt = PO_DEFAULT) returns nothing
   QC_PROGRAM->addMethodExtended("setParseOptions",      (q_method_t)PROGRAM_setParseOptions, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, bigIntTypeInfo, new QoreBigIntNode(PO_DEFAULT));

   // Program::disableParseOptions(int $opt = PO_DEFAULT) returns nothing
   QC_PROGRAM->addMethodExtended("disableParseOptions",  (q_method_t)PROGRAM_disableParseOptions, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, bigIntTypeInfo, new QoreBigIntNode(PO_DEFAULT));

   // Program::setScriptPath() returns nothing
   QC_PROGRAM->addMethodExtended("setScriptPath",        (q_method_t)PROGRAM_setScriptPath_nothing, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);
   // Program::setScriptPath(string $path) returns nothing
   QC_PROGRAM->addMethodExtended("setScriptPath",        (q_method_t)PROGRAM_setScriptPath_str, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // Program::getScriptDir() returns nothing|string
   QC_PROGRAM->addMethodExtended("getScriptDir",         (q_method_t)PROGRAM_getScriptDir, false, QC_RET_VALUE_ONLY);

   // Program::getScriptName() returns nothing|string
   QC_PROGRAM->addMethodExtended("getScriptName",        (q_method_t)PROGRAM_getScriptName, false, QC_RET_VALUE_ONLY);

   // Program::getScriptPath() returns nothing|string
   QC_PROGRAM->addMethodExtended("getScriptPath",        (q_method_t)PROGRAM_getScriptPath, false, QC_RET_VALUE_ONLY);

   // Program::lockOptions() returns nothing
   QC_PROGRAM->addMethodExtended("lockOptions",          (q_method_t)PROGRAM_lockOptions, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo);

   // Program::getGlobalVariable(string $varname) returns any
   QC_PROGRAM->addMethodExtended("getGlobalVariable",    (q_method_t)PROGRAM_getGlobalVariable, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, 0, 1, stringTypeInfo, QORE_PARAM_NO_ARG);
   // Program::getGlobalVariable(string $varname, reference $ref) returns any
   QC_PROGRAM->addMethodExtended("getGlobalVariable",    (q_method_t)PROGRAM_getGlobalVariable, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, 0, 2, stringTypeInfo, QORE_PARAM_NO_ARG, referenceTypeInfo, QORE_PARAM_NO_ARG);

   // Program::setTimeZoneRegion(string $region) returns nothing
   QC_PROGRAM->addMethodExtended("setTimeZoneRegion",    (q_method_t)PROGRAM_setTimeZoneRegion, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, stringTypeInfo, QORE_PARAM_NO_ARG);

   // Program::setTimeZoneOffset(softint $minutes_east) returns nothing
   QC_PROGRAM->addMethodExtended("setTimeZoneUTCOffset", (q_method_t)PROGRAM_setTimeZoneUTCOffset, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, softBigIntTypeInfo, QORE_PARAM_NO_ARG);

   // Program::setTimeZone(TimeZone $zone) returns nothing
   QC_PROGRAM->addMethodExtended("setTimeZone",          (q_method_t)PROGRAM_setTimeZone, false, QC_NO_FLAGS, QDOM_DEFAULT, nothingTypeInfo, 1, QC_TIMEZONE->getTypeInfo(), QORE_PARAM_NO_ARG);

   // Program::getTimeZone() returns TimeZone
   QC_PROGRAM->addMethodExtended("getTimeZone",          (q_method_t)PROGRAM_getTimeZone, false, QC_RET_VALUE_ONLY, QDOM_DEFAULT, QC_TIMEZONE->getTypeInfo());

   return QC_PROGRAM;
}
