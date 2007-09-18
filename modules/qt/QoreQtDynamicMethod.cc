/*
 QoreQtDynamicMethod.cc
 
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

#include "QoreQtDynamicMethod.h"

#include "QC_QFont.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>

int QoreQtDynamicMethod::get_type(const char *&p)
{
   int rt;
   if (!strncmp("int", p, 3)) {
      rt = QQT_TYPE_INT;
      p += 3;  
   }
   else if (!strncmp("bool", p, 4)) {
      rt = QQT_TYPE_BOOL;
      p += 4;
   }
   else if (!strncmp("float", p, 5)) {
      rt = QQT_TYPE_FLOAT;
      p += 5;
   }
   else if (!strncmp("double", p, 5)) {
      rt = QQT_TYPE_DOUBLE;
      p += 5;
   }
   else if (!strncmp("char*", p, 5)) {
      rt = QQT_TYPE_P_CHAR;	
      p += 5;
   }
   else if (!strncmp("const char*", p, 11)) {
      rt = QQT_TYPE_P_CHAR;	
      p += 11;
   }
   else if (!strncmp("QDate", p, 5)) {
      rt = QQT_TYPE_QDATE;
      p += 5;
   }
   else if (!strncmp("QFont", p, 5)) {
      rt = QQT_TYPE_QFONT;
      p += 5;
   }
   else if (!strncmp("QString", p, 7)) {
      rt = QQT_TYPE_QSTRING;
      p += 5;
   }
   else {
      //printd(5, "QoreQtDynamicMethod::get_type(%s) unknown type error!\n", p);
      return QQT_TYPE_UNKNOWN;
   }

   while (*p && *p != ',' && *p != ')')
      ++p;
   if (*p) ++p;
   while (*p && isblank(*p))
      ++p;
	 
   //printd(5, "get_type(%s) returning %d\n", op, rt);
   return rt;
}

Method *QoreQtDynamicSlot::resolveMethod(Object *n_qore_obj, const char *name, class ExceptionSink *xsink)
{
   QoreClass *qc = n_qore_obj->getClass();
   const char *c = strchr(name, '(');
   QoreString tmp;
   if (c)
      tmp.concat(name, c - name);
   else
      tmp.concat(name);
   // do not allow special methods to be matched
   if (!tmp.compare("constructor") || !tmp.compare("destructor") || !tmp.compare("copy")) {
      xsink->raiseException("DYNAMIC-SLOT-ERROR", "cannot assign special method %s::%s() to a Qt slot!", qc->getName(), tmp.getBuffer());
      return 0;
   }

   Method *meth = qc->findMethod(tmp.getBuffer());
   if (!meth)
      xsink->raiseException("DYNAMIC-SLOT-ERROR", "method %s::%s() does not exist", qc->getName(), tmp.getBuffer());
	 
   //printd(5, "DynamicSlot::resolveMethod(%08p, '%s') search: %s::%s() resolved to %08p\n", n_qore_obj, name, qc->getName(), tmp.getBuffer(), meth);
   return meth;
}

QoreQtDynamicSlot::QoreQtDynamicSlot(Object *n_qore_obj, const char *sig, ExceptionSink *xsink) : qore_obj(n_qore_obj), return_type(QQT_TYPE_UNKNOWN)
{
   if (!sig)
      return;
   
   //printd(5, "QoreQtDynamicSlot::QoreQtDynamicSlot() processing signature '%s'\n", sig);

   // process signature
   const char *p = strchrs(sig, " (");
   if (!p)
      return;
   if (*p == '(') {
      return_type = QQT_TYPE_VOID;
      method = resolveMethod(qore_obj, sig, xsink);
      if (!method)
	 return;
   }
   else {
      const char *p1;
      while (p && *p && (*p) != '(') {
	 p1 = p;
	 p = strchrs(p, " (");
      }
      if (!p || !*p) {
	 xsink->raiseException("DYNAMIC-SLOT-ERROR", "cannot find slot function name in '%s'", sig);
	 return;
      }

      // resolve slot signature to method
      method = resolveMethod(qore_obj, p1, xsink);
      if (!method)
	 return;

      const char *tmp = sig;
      return_type = get_type(tmp);
   }
   ++p;
   while (*p && isblank(*p))
      ++p;
   if (*p != ')') 
      while (*p) {
	 int tc = get_type(p);
	 if (tc == QQT_TYPE_UNKNOWN) {
	    QoreString *desc = new QoreString("cannot resolve argument type '");
	    char *x = strchrs(p, " ,)");
	    if (x)
	       desc->concat(p, x - p);
	    else
	       desc->concat("unknown");
	    desc->sprintf("' in '%s'", sig);
	    xsink->raiseException("DYNAMIC-SLOT-ERROR", desc);
	    break;
	 }
	 type_list.push_back(tc);
      }
}

void QoreQtDynamicSlot::call(void **arguments)
{
   //printd(5, "DynamicSlot::call(%08p, %08p)\n", sender, arguments);
   //printd(5, "0=%08p, 1=%08p, 2=%08p\n", arguments[0], arguments[1], arguments[2]);

   ExceptionSink xsink;
   List *args = type_list.empty() ? 0 : new List();
   for (int i = 0, e = type_list.size(); i < e; ++i)
   {
      if (type_list[i] == QQT_TYPE_INT) {
	 int *ptr = reinterpret_cast<int *>(arguments[i + 1]);
	 args->push(new QoreNode((int64)*ptr));
      }
      else if (type_list[i] == QQT_TYPE_LONG) {
	 long *ptr = reinterpret_cast<long *>(arguments[i + 1]);
	 args->push(new QoreNode((int64)*ptr));
      }
      else if (type_list[i] == QQT_TYPE_BOOL) {
	 bool *ptr = reinterpret_cast<bool *>(arguments[i + 1]);
	 args->push(new QoreNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_FLOAT) {
	 float *ptr = reinterpret_cast<float *>(arguments[i + 1]);
	 args->push(new QoreNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_DOUBLE) {
	 double *ptr = reinterpret_cast<double *>(arguments[i + 1]);
	 args->push(new QoreNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_P_CHAR) {
	 char **ptr = reinterpret_cast<char **>(arguments[i + 1]);
	 args->push(new QoreNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_QSTRING) {
	 QString *qstr = reinterpret_cast<QString *>(arguments[i + 1]);
	 //printd(5, "slot argument string: %08p: %d\n", qstr, qstr->length());
	 //printd(5, "slot argument string: '%s'\n", qstr->toUtf8().data());
	 args->push(new QoreNode(new QoreString(qstr->toUtf8().data(), QCS_UTF8)));
      }
      else if (type_list[i] == QQT_TYPE_QDATE) {
	 QDate *qdate = reinterpret_cast<QDate *>(arguments[i + 1]);
	 args->push(new QoreNode(new DateTime(qdate->year(), qdate->month(), qdate->day())));
      }
      else if (type_list[i] == QQT_TYPE_QFONT) {
	 QFont *qfont = reinterpret_cast<QFont *>(arguments[i + 1]);

	 Object *o_qf = new Object(QC_QFont, getProgram());
	 QoreQFont *q_qf = new QoreQFont(*qfont);
	 o_qf->setPrivate(CID_QFONT, q_qf);

	 args->push(new QoreNode(o_qf));
      }
      else {
	 printd(0, "QoreQtDynamicSlot::call() ignoring argument %d type %d\n", i, type_list[i]);
	 args->push(0);
      }
   }
   QoreNode *node_args = args ? new QoreNode(args) : 0;
   QoreNode *rv = method->eval(qore_obj, node_args, &xsink);
   if (node_args)
      node_args->deref(&xsink);
   if (return_type == QQT_TYPE_INT) {
      int *ptr = reinterpret_cast<int *>(arguments[0]);
      *ptr = rv ? rv->getAsInt() : 0;
   }
   else if (return_type == QQT_TYPE_BOOL) {
      bool *ptr = reinterpret_cast<bool *>(arguments[0]);
      *ptr = rv ? rv->getAsBool() : 0;
   }
	 
   discard(rv, &xsink);
}

void QoreQtDynamicSlot::call()
{
   //printd(5, "DynamicSlot::call() sender=%08p\n", sender);

   ExceptionSink xsink;
   QoreNode *rv = method->eval(qore_obj, 0, &xsink);
   discard(rv, &xsink);
}

QoreQtDynamicSignal::QoreQtDynamicSignal(const char *sig, ExceptionSink *xsink) 
{
   assert(sig);

   // process signature
   const char *p = strchr(sig, '(');

   if (!p) {
      xsink->raiseException("DYNAMIC-SIGNAL-ERROR", "invalid signal signature '%s'", sig);
      return;
   }

   ++p;
   while (*p && isblank(*p))
      ++p;
   if (*p != ')') 

   while (*p) {
      int at = get_type(p);
      if (at == QQT_TYPE_UNKNOWN) {
	 QoreString *desc = new QoreString("cannot resolve argument type '");
	 char *x = strchrs(p, " ,)");
	 if (x)
	    desc->concat(p, x - p);
	 else
	    desc->concat("unknown");
	 desc->sprintf("' in '%s'", sig);
	 xsink->raiseException("DYNAMIC-SIGNAL-ERROR", desc);
	 break;
      }
      type_list.push_back(at);
   }
}

void QoreQtDynamicSignal::emit_signal(QObject *obj, int id, List *args)
{
   int num_args = type_list.size();
   void *sig_args[num_args + 1];
   qt_arg_u arg_list[num_args];

   // return return value to 0
   sig_args[0] = 0;

   bool need_destructor = false;

   // iterate through signal parameters to build argument list
   for (int i = 0; i < num_args; ++i)
   {
      // get argument QoreNode
      QoreNode *n = args ? args->retrieve_entry(i + 1) : 0;

      switch (type_list[i])
      {
	 case QQT_TYPE_INT:
	    arg_list[i].set(n ? n->getAsInt() : 0);
	    sig_args[i + 1] = reinterpret_cast<void *>(&arg_list[i].t_int);
	    break;
	 case QQT_TYPE_FLOAT:
	    arg_list[i].set((float)(n ? n->getAsFloat() : 0.0));
	    sig_args[i + 1] = reinterpret_cast<void *>(&arg_list[i].t_float);
	    break;
	 case QQT_TYPE_DOUBLE:
	    arg_list[i].set((double)(n ? n->getAsFloat() : 0.0));
	    sig_args[i + 1] = reinterpret_cast<void *>(&arg_list[i].t_double);
	    break;
	 case QQT_TYPE_BOOL:
	    arg_list[i].set(n ? n->getAsBool() : false);
	    sig_args[i + 1] = reinterpret_cast<void *>(&arg_list[i].t_bool);
	    break;
	 case QQT_TYPE_QSTRING: {
	    ExceptionSink xsink;

	    QString str;
	    get_qstring(n, str, &xsink);
	    arg_list[i].set(str);

	    need_destructor = true;

	    sig_args[i + 1] = reinterpret_cast<void *>(arg_list[i].t_QString);

	    //printd(5, "creating QString argument: %08p, %s\n", n, n->type->getName());
	    //printd(5, "QString: '%s'\n", arg_list[i].t_QString->toUtf8().data());

	    break;
	 }
	 default:
	    assert(false);
      }
   }
   QMetaObject::activate(obj, id, id, sig_args);

   if (need_destructor)
      // run destructors as necessary
      for (int i = 0; i < num_args; ++i)
	 switch (type_list[i])
	 {
	    case QQT_TYPE_QSTRING:
	       delete arg_list[i].t_QString;
	       break;
	 }
}
