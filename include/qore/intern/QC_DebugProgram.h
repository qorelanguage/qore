/* -*- mode: c++; indent-tabs-mode: nil -*- */
/*
  QC_DebugProgram.h
  
  Qore Programming Language

  Copyright (C) 2003 - 2015 David Nichols

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  Note that the Qore library is released under a choice of three open-source
  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
  information.
*/

#ifndef _QORE_CLASS_DEBUGPROGRAM_H

#define _QORE_CLASS_DEBUGPROGRAM_H

DLLEXPORT extern qore_classid_t CID_DEBUGPROGRAM;
DLLLOCAL extern QoreClass* QC_DEBUGPROGRAM;
DLLLOCAL QoreClass *initProgramClass(QoreNamespace& ns);

#include <qore/QoreDebugProgram.h>
#include <qore/ReferenceArgumentHelper.h>


// class needed to handle calls from C++ to Qore script instance
class QoreDebugProgramWithQoreObject: public QoreDebugProgram {
private:
   QoreObject* qo;
   typedef std::map<QoreProgram*, QoreObject*> qore_program_to_object_map_t;
   qore_program_to_object_map_t qore_program_to_object_map;

   DLLLOCAL ThreadDebugEnum callMethod(const char* name, QoreProgram *pgm, int paramCount, AbstractQoreNode** params, ExceptionSink* xsink) {
      int rv;
      QoreListNode* l = new QoreListNode();
      qore_program_to_object_map_t::iterator i = qore_program_to_object_map.find(pgm);
      if (i == qore_program_to_object_map.end()) {
         return DBG_SB_RUN;
      }
      l->push(i->second);
      //l->push(QoreProgram);
      for (int i=0; i<paramCount; i++) {
         l->push(params[i]);
      }
      rv = qo->intEvalMethod(name, l, xsink);
      for (int i=0; i<paramCount; i++) {
         params[i]->deref(xsink);
      }
      l->deref(xsink);
      return (ThreadDebugEnum) rv;
   }

   DLLLOCAL QoreHashNode* getLocation(const AbstractStatement *s) const {
      QoreHashNode* h = new QoreHashNode;
      h->setKeyValue("file", new QoreStringNode(s->loc.file), 0);
      h->setKeyValue("source", new QoreStringNode(s->loc.source), 0);
      h->setKeyValue("offset", new QoreBigIntNode(s->loc.offset), 0);
      h->setKeyValue("start_line", new QoreBigIntNode(s->loc.start_line), 0);
      h->setKeyValue("end_line", new QoreBigIntNode(s->loc.end_line), 0);
      return h;
   }
   // hide parent methods
   //DLLEXPORT void addProgram(QoreProgram *pgm);
public:
   DLLLOCAL QoreDebugProgramWithQoreObject(QoreObject* n_qo): qo(n_qo) {}

   DLLLOCAL void addProgram(QoreProgram *pgm, QoreObject* o) {
      // for backcall we need link to QoreObject
      //AutoLocker al(tlock);
      qore_program_to_object_map_t::iterator i = qore_program_to_object_map.find(pgm);
      if (i != qore_program_to_object_map.end()) {
         assert(i->second == o);
         return;  // already exists
      }
      qore_program_to_object_map.insert(qore_program_to_object_map_t::value_type(pgm, o));
      QoreDebugProgram::addProgram(pgm);
   }
   DLLLOCAL ThreadDebugEnum onAttach(QoreProgram *pgm, ExceptionSink* xsink) {
      return callMethod("onAttach", pgm, 0, 0, xsink);
   }
   DLLLOCAL ThreadDebugEnum onDetach(QoreProgram *pgm, ExceptionSink* xsink) {
      return callMethod("onDetach", pgm, 0, 0, xsink);
   }
   DLLLOCAL virtual ThreadDebugEnum onStep(QoreProgram *pgm, const StatementBlock *blockStatement, const AbstractStatement *statement, int &retCode, ExceptionSink* xsink) {
      AbstractQoreNode* params[3];
      params[0] = getLocation(blockStatement);
      params[1] = getLocation(statement);
      ReferenceArgumentHelper rah(new QoreBigIntNode(retCode), xsink);
      params[2] = rah.getArg(); // caller owns ref
      ThreadDebugEnum rv = callMethod("onStep", pgm, 3, params, xsink);
      AbstractQoreNode* rc = rah.getOutputValue();  // caller owns ref
      retCode = rc->getAsInt();
      rc->deref(xsink);
      return rv;
   }
   DLLLOCAL virtual ThreadDebugEnum onFunctionEnter(QoreProgram *pgm, const StatementBlock *blockStatement, ExceptionSink* xsink) {
      AbstractQoreNode* params[1];
      params[0] = getLocation(blockStatement);
      ThreadDebugEnum rv = callMethod("onFunctionEnter", pgm, 1, params, xsink);
      return rv;
   }
   DLLLOCAL virtual ThreadDebugEnum onFunctionExit(QoreProgram *pgm, const StatementBlock *blockStatement, QoreValue& returnValue, ExceptionSink* xsink) {
      AbstractQoreNode* params[2];
      params[0] = getLocation(blockStatement);
      ReferenceArgumentHelper rah(returnValue.getInternalNode(), xsink);
      params[1] = rah.getArg(); // caller owns ref
      ThreadDebugEnum rv = callMethod("onFunctionExit", pgm, 2, params, xsink);
      AbstractQoreNode* rc = rah.getOutputValue();  // caller owns ref
      returnValue.assign(rc);  // takes reference
      return rv;
   }
   DLLLOCAL virtual ThreadDebugEnum onException(QoreProgram *pgm, const AbstractStatement *statement, ExceptionSink* xsink) {
      AbstractQoreNode* params[1];
      params[0] = getLocation(statement);
      ThreadDebugEnum rv = callMethod("onException", pgm, 1, params, xsink);
      return rv;
   }

};


#endif // _QORE_CLASS_DEBUGPROGRAM_H
