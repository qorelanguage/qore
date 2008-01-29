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
#include "QC_QListWidgetItem.h"
#include "QC_QWidget.h"

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
      p += 7;
   }
   else if (!strncmp("QSystemTrayIcon::ActivationReason", p, 33)) {
      rt = QQT_TYPE_INT;
      p += 33;
   }
   else if (!strncmp("QListWidgetItem*", p, 16)) {
      rt = QQT_TYPE_P_QLISTWIDGETITEM;
      p += 16;
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

const QoreMethod *QoreQtDynamicSlot::resolveMethod(QoreObject *n_qore_obj, const char *name, class ExceptionSink *xsink)
{
   const QoreClass *qc = n_qore_obj->getClass();
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

   const QoreMethod *meth = qc->findMethod(tmp.getBuffer());
   if (!meth)
      xsink->raiseException("DYNAMIC-SLOT-ERROR", "method %s::%s() does not exist", qc->getName(), tmp.getBuffer());
	 
   //printd(5, "DynamicSlot::resolveMethod(%08p, '%s') search: %s::%s() resolved to %08p\n", n_qore_obj, name, qc->getName(), tmp.getBuffer(), meth);
   return meth;
}

QoreQtDynamicSlot::QoreQtDynamicSlot(QoreObject *n_qore_obj, const char *sig, ExceptionSink *xsink) : qore_obj(n_qore_obj), return_type(QQT_TYPE_UNKNOWN)
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
      const char *p1 = 0;
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
	    QoreStringNode *desc = new QoreStringNode("cannot resolve argument type '");
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
   ReferenceHolder<QoreListNode> args(type_list.empty() ? 0 : new QoreListNode(), &xsink);
   for (int i = 0, e = type_list.size(); i < e; ++i)
   {
      if (type_list[i] == QQT_TYPE_INT) {
	 int *ptr = reinterpret_cast<int *>(arguments[i + 1]);
	 args->push(new QoreBigIntNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_LONG) {
	 long *ptr = reinterpret_cast<long *>(arguments[i + 1]);
	 args->push(new QoreBigIntNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_BOOL) {
	 bool *ptr = reinterpret_cast<bool *>(arguments[i + 1]);
	 args->push(new QoreBoolNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_FLOAT) {
	 float *ptr = reinterpret_cast<float *>(arguments[i + 1]);
	 args->push(new QoreFloatNode((double)*ptr));
      }
      else if (type_list[i] == QQT_TYPE_DOUBLE) {
	 double *ptr = reinterpret_cast<double *>(arguments[i + 1]);
	 args->push(new QoreFloatNode((double)*ptr));
      }
      else if (type_list[i] == QQT_TYPE_P_CHAR) {
	 char **ptr = reinterpret_cast<char **>(arguments[i + 1]);
	 args->push(new QoreStringNode(*ptr));
      }
      else if (type_list[i] == QQT_TYPE_QSTRING) {
	 QString *qstr = reinterpret_cast<QString *>(arguments[i + 1]);
	 //printd(5, "slot argument string: %08p: %d\n", qstr, qstr->length());
	 //printd(5, "slot argument string: '%s'\n", qstr->toUtf8().data());
	 args->push(new QoreStringNode(qstr->toUtf8().data(), QCS_UTF8));
      }
      else if (type_list[i] == QQT_TYPE_QDATE) {
	 QDate *qdate = reinterpret_cast<QDate *>(arguments[i + 1]);
	 args->push(new DateTimeNode(qdate->year(), qdate->month(), qdate->day()));
      }
      else if (type_list[i] == QQT_TYPE_QFONT) {
	 QFont *qfont = reinterpret_cast<QFont *>(arguments[i + 1]);

	 QoreObject *o_qf = new QoreObject(QC_QFont, getProgram());
	 QoreQFont *q_qf = new QoreQFont(*qfont);
	 o_qf->setPrivate(CID_QFONT, q_qf);

	 args->push(o_qf);
      }
      else if (type_list[i] == QQT_TYPE_P_QLISTWIDGETITEM) {
	 QListWidgetItem *qlwi = *(reinterpret_cast<QListWidgetItem **>(arguments[i + 1]));
	 
	 QoreObject *o_qlwi = new QoreObject(QC_QListWidgetItem, getProgram());
	 QoreQListWidgetItem *q_qlwi = new QoreQListWidgetItem(qlwi);
	 o_qlwi->setPrivate(CID_QLISTWIDGETITEM, q_qlwi);

	 args->push(o_qlwi);
      }
      else {
	 printd(0, "QoreQtDynamicSlot::call() ignoring argument %d type %d\n", i, type_list[i]);
	 args->push(0);
      }
   }
   ReferenceHolder<QoreNode> rv(method->eval(qore_obj, *args, &xsink), &xsink);
   if (return_type == QQT_TYPE_INT) {
      int *ptr = reinterpret_cast<int *>(arguments[0]);
      *ptr = *rv ? rv->getAsInt() : 0;
   }
   else if (return_type == QQT_TYPE_BOOL) {
      bool *ptr = reinterpret_cast<bool *>(arguments[0]);
      *ptr = *rv ? rv->getAsBool() : 0;
   }
}

void QoreQtDynamicSlot::call()
{
   //printd(5, "DynamicSlot::call() sender=%08p\n", sender);

   ExceptionSink xsink;
   discard(method->eval(qore_obj, 0, &xsink), &xsink);
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
	 QoreStringNode *desc = new QoreStringNode("cannot resolve argument type '");
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

void QoreQtDynamicSignal::emit_signal(QObject *obj, int id, const QoreListNode *args)
{
   int num_args = type_list.size();
   void *sig_args[num_args + 1];
   qt_arg arg_list[num_args];

   // return return value to 0
   sig_args[0] = 0;

   // iterate through signal parameters to build argument list
   for (int i = 0; i < num_args; ++i)
   {
      // get argument QoreNode
      QoreNode *n = args ? args->retrieve_entry(i + 1) : 0;

      switch (type_list[i])
      {
	 case QQT_TYPE_INT:
	    sig_args[i + 1] = arg_list[i].set(n ? n->getAsInt() : 0);
	    break;
	 case QQT_TYPE_FLOAT:
	    sig_args[i + 1] = arg_list[i].set((float)(n ? n->getAsFloat() : 0.0));
	    break;
	 case QQT_TYPE_DOUBLE:
	    sig_args[i + 1] = arg_list[i].set((double)(n ? n->getAsFloat() : 0.0));
	    break;
	 case QQT_TYPE_BOOL:
	    sig_args[i + 1] = arg_list[i].set(n ? n->getAsBool() : false);
	    break;
	 case QQT_TYPE_QSTRING: {
	    ExceptionSink xsink;

	    QString str;
	    get_qstring(n, str, &xsink);
	    sig_args[i + 1] = arg_list[i].set(str);

	    break;
	 }
	 default:
	    printd(0, "QoreQtDynamicSignal::emit_signal() unsupported type=%d\n", type_list[i]);
	    assert(false);
      }
   }
   QMetaObject::activate(obj, id, id, sig_args);
}

void emit_static_signal(QObject *sender, int signalId, const QMetaMethod &qmm, const QoreListNode *args)
{
   QList<QByteArray> params = qmm.parameterTypes();

   int num_args = params.size();
   void *sig_args[num_args + 1];
   qt_arg arg_list[num_args];

   // return return value to 0
   sig_args[0] = 0;

   ExceptionSink xsink;

   // iterate through signal parameters to build argument list
   for (int i = 0; i < num_args; ++i)
   {
      // get argument QoreNode
      QoreNode *n = args ? args->retrieve_entry(i + 1) : 0;
      const char *str = params[i].data();
      
      if (!strcmp(str, "int"))
	 sig_args[i + 1] = arg_list[i].set(n ? n->getAsInt() : 0);
      else if (!strcmp(str, "float"))
	 sig_args[i + 1] = arg_list[i].set((float)(n ? n->getAsFloat() : 0.0));
      else if (!strcmp(str, "double"))
	 sig_args[i + 1] = arg_list[i].set((double)(n ? n->getAsFloat() : 0.0));
      else if (!strcmp(str, "bool"))
	 sig_args[i + 1] = arg_list[i].set(n ? n->getAsBool() : false);
      else if (!strcmp(str, "QString") || !strcmp(str, "const QString&"))
      {
	 QString str;
	 get_qstring(n, str, &xsink);
	 sig_args[i + 1] = arg_list[i].set(str);
      }
      else if (!strcmp(str, "QWidget*"))
      {
	 QoreQWidget *widget = (n && n->type == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<QoreObject *>(n))->getReferencedPrivateData(CID_QWIDGET, &xsink) : 0;
	 sig_args[i + 1] = arg_list[i].set(widget);
      }
      else {
	 printd(0, "emit_static_signal() i=%d, unsupported C++ type=%s\n", i, str);
	 assert(false);
      }
   }
	 
   QMetaObject::activate(sender, signalId, signalId, sig_args);
}

