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
DLLLOCAL QoreClass *initDebugProgramClass(QoreNamespace& ns);

#include <qore/QoreDebugProgram.h>
#include <qore/ReferenceArgumentHelper.h>


// class needed to handle calls from C++ to Qore script instance
class QoreDebugProgramWithQoreObject: public QoreDebugProgram {
private:
   QoreObject* qo;
   typedef std::map<QoreProgram*, QoreObject*> qore_program_to_object_map_t;
   qore_program_to_object_map_t qore_program_to_object_map;

   DLLLOCAL void callMethod(const char* name, QoreProgram *pgm, int paramCount, AbstractQoreNode** params, ThreadDebugEnum &sb, ExceptionSink* xsink, ExceptionSink &xsink2) {
      QoreListNode* l = new QoreListNode();
      qore_program_to_object_map_t::iterator i = qore_program_to_object_map.find(pgm);
      if (i == qore_program_to_object_map.end()) {
         return;
      }
      i->second->ref();
      l->push(i->second);
      //l->push(QoreProgram);
      for (int i=0; i<paramCount; i++) {
         printd(5, "QoreDebugProgramWithCoreObject::callMethod(%s) this: %p, param: %d/%d, type: %s\n", name, this, i, paramCount, params[i]?params[i]->getTypeName():"n/a");
         if (params[i])
               params[i]->ref();
         l->push(params[i]);
      }
      QoreBigIntNode *rc = new QoreBigIntNode(sb);
      ReferenceArgumentHelper rah(rc, &xsink2);
      l->push(rah.getArg()); // caller owns ref

      printd(5, "QoreDebugProgramWithCoreObject::callMethod(%s) this: %p, pgm: %p, param#: %d, sb: %d, xsink2: %d\n", name, this, pgm, paramCount, sb, xsink2.isEvent());
      qo->evalMethod(name, l, &xsink2);
//      AbstractQoreNode* rc = rah.getOutputValue();  // caller owns ref
//      sb = (ThreadDebugEnum) rc->getAsInt();
      QoreValue v(rah.getOutputQoreValue());
      sb = (ThreadDebugEnum) v.getAsBigInt();
      //rc->deref();
      printd(5, "QoreDebugProgramWithCoreObject::callMethod(%s) this: %p, pgm: %p, sb: %d\n", name, this, pgm, sb);
      l->deref(&xsink2);
      /* catch all exceptions from defbug code, optionally we could assimilate on demand or create exception handler
       * but developer can try/catch by himself to handle it
       */
      // xsink->assimilate(xsink2);
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
public:
   DLLLOCAL QoreDebugProgramWithQoreObject(QoreObject* n_qo): qo(n_qo) {
      printd(5, "QoreDebugProgramWithCoreObject::QoreDebugProgramWithCoreObject() this: %p, qo: %p\n", this, n_qo);
   }

   // hide parent methods
   DLLLOCAL void addProgram(QoreProgram *pgm) = delete;
   DLLLOCAL void addProgram(QoreProgram *pgm, QoreObject* o) {
      // for backcall we need link to QoreObject
      //AutoLocker al(tlock);
      printd(5, "QoreDebugProgramWithCoreObject::addProgram() this: %p, pgm: %p, o: %p\n", this, pgm, o);
      qore_program_to_object_map_t::iterator i = qore_program_to_object_map.find(pgm);
      if (i != qore_program_to_object_map.end()) {
         assert(i->second == o);
         return;  // already exists
      }
      qore_program_to_object_map.insert(qore_program_to_object_map_t::value_type(pgm, o));
      QoreDebugProgram::addProgram(pgm);
   }
   DLLLOCAL virtual void onAttach(QoreProgram *pgm, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      printd(5, "QoreDebugProgramWithCoreObject::onAttach() this: %p, pgm: %p\n", this, pgm);
      ExceptionSink xsink2;
      callMethod("onAttach", pgm, 0, 0, sb, xsink, xsink2);
   }
   DLLLOCAL virtual void onDetach(QoreProgram *pgm, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      printd(5, "QoreDebugProgramWithCoreObject::onDetach() this: %p, pgm: %p\n", this, pgm);
      ExceptionSink xsink2;
      callMethod("onDetach", pgm, 0, 0, sb, xsink, xsink2);
   }
   DLLLOCAL virtual void onStep(QoreProgram *pgm, const StatementBlock *blockStatement, const AbstractStatement *statement, int &retCode, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      ExceptionSink xsink2;
      AbstractQoreNode* params[3];
      params[0] = getLocation(blockStatement);
      params[1] = statement ? getLocation(statement) : 0;
      QoreBigIntNode *rc = new QoreBigIntNode(retCode);
      ReferenceArgumentHelper rah(rc, &xsink2);
      params[2] = rah.getArg(); // caller owns ref
      callMethod("onStep", pgm, 3, params, sb, xsink, xsink2);
//      AbstractQoreNode* rc = rah.getOutputValue();  // caller owns ref
//      retCode = rc->getAsInt();
      QoreValue v(rah.getOutputQoreValue());
      retCode = v.getAsBigInt();
      //rc->deref();
   }
   DLLLOCAL virtual void onFunctionEnter(QoreProgram *pgm, const StatementBlock *blockStatement, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      ExceptionSink xsink2;
      AbstractQoreNode* params[1];
      params[0] = getLocation(blockStatement);
      callMethod("onFunctionEnter", pgm, 1, params, sb, xsink, xsink2);
   }
   DLLLOCAL virtual void onFunctionExit(QoreProgram *pgm, const StatementBlock *blockStatement, QoreValue& returnValue, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      ExceptionSink xsink2;
      AbstractQoreNode* params[2];
      params[0] = getLocation(blockStatement);
      printd(5, "QoreDebugProgramWithCoreObject::onFunctionExit() getRetValue#0: type: %d, in: %p\n", returnValue.type, returnValue.getInternalNode());
      if (returnValue.getInternalNode()) {
         printd(5, "QoreDebugProgramWithCoreObject::onFunctionExit() getRetValue#1: QVtype: %d, refCount: %d\n", returnValue.type, returnValue.getInternalNode()->reference_count());
      }
      ReferenceArgumentHelper rah(returnValue.takeNode(), &xsink2);
      if (returnValue.getInternalNode()) {
         printd(5, "QoreDebugProgramWithCoreObject::onFunctionExit() getRetValue#2: refCount: %d\n", returnValue.getInternalNode()->reference_count());
      }
      params[1] = rah.getArg(); // caller owns ref
      callMethod("onFunctionExit", pgm, 2, params, sb, xsink, xsink2);
      returnValue = rah.getOutputValue();  // caller owns ref
      printd(5, "QoreDebugProgramWithCoreObject::onFunctionExit() getRetValue#3: type: %d, in: %p\n", returnValue.type, returnValue.getInternalNode());
      if (returnValue.getInternalNode()) {
         printd(5, "QoreDebugProgramWithCoreObject::onFunctionExit() getRetValue#4: refCount: %d\n", returnValue.getInternalNode()->reference_count());
      }
   }
   DLLLOCAL virtual void onException(QoreProgram *pgm, const AbstractStatement *statement, ThreadDebugEnum &sb, ExceptionSink* xsink) {
      ExceptionSink xsink2;
      AbstractQoreNode* params[3];
      params[0] = getLocation(statement);
      QoreException* except = xsink->getException();
      params[1] = except->makeExceptionObject();
      ReferenceArgumentHelper rah(get_bool_node(false), &xsink2);
      params[2] = rah.getArg(); // caller owns ref
      callMethod("onException", pgm, 3, params, sb, xsink, xsink2);
      QoreValue v(rah.getOutputQoreValue());
      if (v.getAsBool()) {
         xsink->clear();  // dismiss exception
      }
   }

};


#endif // _QORE_CLASS_DEBUGPROGRAM_H
