/*
 QoreQtDynamicMethod.cc
 
 Qore Programming Language
 
 Copyright 2003 - 2008 David Nichols
 
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

#include "qt-core.h"

#include "QoreQtDynamicMethod.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef safe_dslist<QoreQtAbstractDynamicTypeHelper *> qore_qt_type_list_t;

class QoreQtTypeList : public qore_qt_type_list_t
{
   public:
      DLLLOCAL QoreQtTypeList();

      DLLLOCAL QoreQtAbstractDynamicTypeHelper *identify(const char *&p)
      {
	 for (qore_qt_type_list_t::iterator i = begin(), e = end(); i != e; ++i) {
	    if ((*i)->identify(p)) {

	       // skip to next argument
	       while (*p && *p != ',' && *p != ')')
		  ++p;
	       if (*p) ++p;
	       while (*p && isblank(*p))
		  ++p;

	       return *i;
	    }
	 }
	 return 0;
      }
};

static QoreQtTypeList qqt_type_list;

class QoreQtVoid : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtVoid() : QoreQtAbstractDynamicTypeHelper("void")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 args.push(0);
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 ptr = 0;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
      }
};

DLLLOCAL QoreQtVoid qqt_void;

DLLLOCAL QoreQtInt qqt_int;

class QoreQtLong : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtLong() : QoreQtAbstractDynamicTypeHelper("long")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 long *ptr = reinterpret_cast<long *>(arg);
	 args.push(new QoreBigIntNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = (void *)(val ? (long)val->getAsBigInt() : 0);
	 ptr = &save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 long *ptr = reinterpret_cast<long *>(rv);
	 *ptr = val ? val->getAsInt() : 0;
      }
};

DLLLOCAL QoreQtLong qqt_long;

class QoreQtBool : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtBool() : QoreQtAbstractDynamicTypeHelper("bool")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 bool *ptr = reinterpret_cast<bool *>(arg);
	 args.push(get_bool_node(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = (void *)(val ? val->getAsBool() : 0);
	 ptr = &save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 bool *ptr = reinterpret_cast<bool *>(rv);
	 *ptr = val ? val->getAsBool() : 0;
      }
};

DLLLOCAL QoreQtBool qqt_bool;

class QoreQtFloat : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtFloat() : QoreQtAbstractDynamicTypeHelper("float")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 float *ptr = reinterpret_cast<float *>(arg);
	 args.push(new QoreFloatNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = malloc(sizeof(float));
	 float *p = (float *)save;
	 *p = val ? (float)val->getAsFloat() : 0.0;
	 ptr = save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
	 free(ptr);
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 float *ptr = reinterpret_cast<float *>(rv);
	 *ptr = val ? (float)val->getAsFloat() : 0;
      }
};

DLLLOCAL QoreQtFloat qqt_float;

class QoreQtDouble : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtDouble() : QoreQtAbstractDynamicTypeHelper("double")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 double *ptr = reinterpret_cast<double *>(arg);
	 args.push(new QoreFloatNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = malloc(sizeof(double));
	 double *p = (double *)save;
	 *p = val ? val->getAsFloat() : 0.0;
	 ptr = save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
	 free(ptr);
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 double *ptr = reinterpret_cast<double *>(rv);
	 *ptr = val ? val->getAsFloat() : 0;
      }
};

DLLLOCAL QoreQtDouble qqt_double;

class QoreQtQReal : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtQReal() : QoreQtAbstractDynamicTypeHelper("qreal")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 qreal *ptr = reinterpret_cast<qreal *>(arg);
	 args.push(new QoreFloatNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = malloc(sizeof(qreal));
	 qreal *p = (double *)save;
	 *p = val ? (qreal)val->getAsFloat() : 0.0;
	 ptr = save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
	 free(ptr);
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 qreal *ptr = reinterpret_cast<qreal *>(rv);
	 *ptr = val ? (qreal)val->getAsFloat() : 0;
      }
};

DLLLOCAL QoreQtQReal qqt_qreal;

struct string_saver
{
      QoreStringNode *v;
      char *p;

      DLLLOCAL string_saver(const AbstractQoreNode *val)
      {
	 QoreStringNodeValueHelper str(val);
	 v = str.getReferencedValue();
	 p = (char *)v->getBuffer();
      }
      DLLLOCAL ~string_saver() { v->deref(); }
      DLLLOCAL char *get_ptr() { return p; }
};

class QoreQtCharPtr : public QoreQtAbstractDynamicTypeHelper
{
   protected:
      DLLLOCAL QoreQtCharPtr(const char *n) : QoreQtAbstractDynamicTypeHelper(n)
      {
      }

   public:
      DLLLOCAL QoreQtCharPtr() : QoreQtAbstractDynamicTypeHelper("char*")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 char **ptr = reinterpret_cast<char **>(arg);
	 args.push(new QoreStringNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 string_saver *ss = new string_saver(val);
	 ptr = ss->get_ptr();
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
	 string_saver *ss = (string_saver *)(ptr);
	 delete ss;
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 assert(false);
	 //char **ptr = reinterpret_cast<char **>(rv);
	 // *ptr = (val && val->getType() == NT_STRING) ? reinterpret_cast<const QoreStringNode *
      }
};

DLLLOCAL QoreQtCharPtr qqt_char_ptr;

class QoreQtConstCharPtr : public QoreQtCharPtr
{
   public:
      DLLLOCAL QoreQtConstCharPtr() : QoreQtCharPtr("const char*")
      {
      }
};

DLLLOCAL QoreQtConstCharPtr qqt_const_char_ptr;

class QoreQtQString : public QoreQtAbstractDynamicTypeHelper
{
   protected:
      DLLLOCAL QoreQtQString(const char *n) : QoreQtAbstractDynamicTypeHelper(n)
      {
      }

   public:
      DLLLOCAL QoreQtQString() : QoreQtAbstractDynamicTypeHelper("QString")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 QString *qstr = reinterpret_cast<QString *>(arg);
	 //printd(5, "slot argument string: %08p: %d\n", qstr, qstr->length());
	 //printd(5, "slot argument string: '%s'\n", qstr->toUtf8().data());
	 args.push(new QoreStringNode(qstr->toUtf8().data(), QCS_UTF8));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 ExceptionSink xsink;

	 QString str;
	 get_qstring(val, str, &xsink);
         QString *p = new QString();
	 save = (void *)p;
	 ptr = save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
	 QString *p = (QString *)ptr;
	 delete p;
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 assert(false);
      }
};

DLLLOCAL QoreQtQString qqt_qstring;

class QoreQtConstQStringRef : public QoreQtQString
{
   public:
      DLLLOCAL QoreQtConstQStringRef() : QoreQtQString("const QString&")
      {
      }
};

DLLLOCAL QoreQtConstQStringRef qqt_const_qstring_ref;

class QoreQtQDate : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQtQDate() : QoreQtAbstractDynamicTypeHelper("QDate")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 QDate *qdate = reinterpret_cast<QDate *>(arg);
	 args.push(new DateTimeNode(qdate->year(), qdate->month(), qdate->day()));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 assert(false);
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 assert(false);
      }
};

DLLLOCAL QoreQtQDate qqt_qdate;

/*
class QoreQt : public QoreQtAbstractDynamicTypeHelper
{
   public:
      DLLLOCAL QoreQt() : QoreQtAbstractDynamicTypeHelper("")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
      }
};
*/

QoreQtTypeList::QoreQtTypeList()
{
   push_back(&qqt_void);
   push_back(&qqt_int);
   push_back(&qqt_long);
   push_back(&qqt_bool);
   push_back(&qqt_float);
   push_back(&qqt_double);
   push_back(&qqt_qreal);
   push_back(&qqt_char_ptr);
   push_back(&qqt_const_char_ptr);
   push_back(&qqt_qstring);
   push_back(&qqt_qdate);
}

void register_qqt_dynamic_type(QoreQtAbstractDynamicTypeHelper *t)
{
   qqt_type_list.push_back(t);
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

QoreQtDynamicSlot::QoreQtDynamicSlot(QoreObject *n_qore_obj, const char *sig, ExceptionSink *xsink) : qore_obj(n_qore_obj), return_type(0)
{
   if (!sig)
      return;
   
   //printd(5, "QoreQtDynamicSlot::QoreQtDynamicSlot() processing signature '%s'\n", sig);

   // process signature
   const char *p = strchrs(sig, " (");
   if (!p)
      return;
   if (*p == '(') {
      return_type = &qqt_void;
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
      return_type = qqt_type_list.identify(tmp);
   }
   ++p;
   while (*p && isblank(*p))
      ++p;
   if (*p != ')') 
      while (*p) {
	 QoreQtAbstractDynamicTypeHelper *tc = qqt_type_list.identify(p);
	 if (!tc) {
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
      assert(type_list[i]);
      type_list[i]->add_qore_arg(*(*args), arguments[i + 1]);
   }
   ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*method, *args, &xsink), &xsink);

   if (return_type)
      return_type->do_return(arguments[0], *rv);
}

void QoreQtDynamicSlot::call()
{
   //printd(5, "DynamicSlot::call() sender=%08p\n", sender);

   ExceptionSink xsink;
   discard(qore_obj->evalMethod(*method, 0, &xsink), &xsink);
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
	 QoreQtAbstractDynamicTypeHelper *at = qqt_type_list.identify(p);
	 if (!at) {
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
	 //printd(0, "resolved type '%s' in '%s'\n", at->get_name(), sig);
	 type_list.push_back(at);
      }
}

void QoreQtDynamicSignal::emit_signal(QObject *obj, int id, const QoreListNode *args)
{
   int num_args = type_list.size();
   void *sig_args[num_args + 1];
   void *save_args[num_args];

   // return return value to 0
   sig_args[0] = 0;

   // iterate through signal parameters to build argument list
   for (int i = 0; i < num_args; ++i) {
      // get argument QoreNode
      const AbstractQoreNode *n = args ? args->retrieve_entry(i + 1) : 0;

#ifdef DEBUG
      if (!type_list[i])
	 printd(0, "QoreQtDynamicSignal::emit_signal() unsupported type\n");
#endif
      assert(type_list[i]);

      type_list[i]->add_qt_arg(sig_args[i + 1], save_args[i], n);
   }
   QMetaObject::activate(obj, id, id, sig_args);

   // iterate through signal parameters to delete temporary values
   for (int i = 0; i < num_args; ++i)
      type_list[i]->del_arg(save_args[i]);
}

void emit_static_signal(QObject *sender, int signalId, const QMetaMethod &qmm, const QoreListNode *args)
{
   QList<QByteArray> params = qmm.parameterTypes();

   int num_args = params.size();
   void *sig_args[num_args + 1];
   void *save_args[num_args];

   QoreQtAbstractDynamicTypeHelper *tlist[num_args];

   // return return value to 0
   sig_args[0] = 0;

   ExceptionSink xsink;

   // iterate through signal parameters to build argument list
   for (int i = 0; i < num_args; ++i)
   {
      // get argument QoreNode
      const AbstractQoreNode *n = args ? args->retrieve_entry(i + 1) : 0;
      const char *str = params[i].data();

      // get type
      tlist[i] = qqt_type_list.identify(str);

#ifdef DEBUG
      if (!tlist[i])
	 printd(0, "QoreQtDynamicSignal::emit_signal() unsupported type='%s'\n", str);
#endif
      assert(tlist[i]);

      tlist[i]->add_qt_arg(sig_args[i + 1], save_args[i], n);

/*      
      if (!strcmp(str, "QWidget*"))
      {
	 QoreQWidget *widget = (n && n->getType() == NT_OBJECT) ? (QoreQWidget *)(reinterpret_cast<const QoreObject *>(n))->getReferencedPrivateData(CID_QWIDGET, &xsink) : 0;
	 sig_args[i + 1] = arg_list[i].set(widget);
      }
*/
   }
	 
   QMetaObject::activate(sender, signalId, signalId, sig_args);

   // iterate through signal parameters to delete temporary values
   for (int i = 0; i < num_args; ++i)
      tlist[i]->del_arg(save_args[i]);
}

