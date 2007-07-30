/*
 QoreAbstractQObject.h
 
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

#ifndef _QORE_QOREABSTRACTQOBJECT_H

#define _QORE_QOREABSTRACTQOBJECT_H

#include <qore/Qore.h>

#include <QPointer>
#include <QObject>
#include <QHash>
#include <QList>

#include <map>
#include <vector>

#include <string.h>
#include <ctype.h>

#define QORE_VIRTUAL_QOBJECT_METHODS \
   DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink) {\
      return qobj->getSlotIndex(theSlot, xsink);				\
   } \
   DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal, bool &is_dynamic) const { \
      return qobj->getSignalIndex(theSignal, is_dynamic);				\
   } \
   DLLLOCAL virtual int createSignal(const char *signal) { \
      return qobj->createDynamicSignal(signal);	\
   } \
   DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink) { \
      return qobj->connectDynamic(sender, signal, slot, xsink);		\
   }

#define QQT_TYPE_UNKNOWN  -1
#define QQT_TYPE_VOID      0
#define QQT_TYPE_INT       1
#define QQT_TYPE_LONG      2
#define QQT_TYPE_BOOL      3
#define QQT_TYPE_FLOAT     4
#define QQT_TYPE_DOUBLE    5
#define QQT_TYPE_P_CHAR    6

typedef std::vector<int> type_list_t;

class DynamicSlot 
{
   private:
      Object *qore_obj;
      Method *method;
      int return_type;
      type_list_t type_list;

      DLLLOCAL static Method *resolveMethod(Object *n_qore_obj, const char *name, class ExceptionSink *xsink)
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

      DLLLOCAL static int get_type(const char *&p)
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
	 else if (!strncmp("char*", p, 5) || !strncmp("const char*", p, 5)) {
	    rt = QQT_TYPE_P_CHAR;	
	    p += 5;
	 }
	 else
	    rt = QQT_TYPE_UNKNOWN;
	 
	 while (isblank(*p) || *p == ',' || *p == ')')
	    p++;
	 
	 //printd(5, "get_type(%s) returning %d\n", op, rt);
	 return rt;
      }

   public:
      DLLLOCAL DynamicSlot(Object *n_qore_obj, const char *sig, ExceptionSink *xsink) : qore_obj(n_qore_obj), return_type(QQT_TYPE_UNKNOWN)
      {
	 if (!sig)
	    return;

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
	 while (p && *p)
	    type_list.push_back(get_type(p));
      }

      DLLLOCAL void call(QObject *sender, void **arguments)
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
	    else
	       args->push(0);
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
};

struct QoreQtDynamicMethod {
   public:
      virtual ~QoreQtDynamicMethod() = 0;
      virtual DynamicSlot *getSlot() const = 0;
};

struct QoreQtDynamicSlot : QoreQtDynamicMethod
{
   private:
      DynamicSlot *slot;

   public:
      DLLLOCAL QoreQtDynamicSlot(DynamicSlot *n_slot) : slot(n_slot)
      {
      }
      DLLLOCAL virtual ~QoreQtDynamicSlot()
      {
	 delete slot;
      }
      DLLLOCAL virtual DynamicSlot *getSlot() const
      {
	 return slot;
      }
};

struct QoreQtDynamicSignal : QoreQtDynamicMethod
{
   private:
      
   public:

      DLLLOCAL QoreQtDynamicSignal()
      {
      }
      DLLLOCAL virtual ~QoreQtDynamicSignal()
      {
      }
      DLLLOCAL virtual DynamicSlot *getSlot() const
      {
	 return 0;
      }
};

typedef std::map<int, QoreQtDynamicMethod *> qore_qt_method_map_t;

class DynamicMethodMap : public qore_qt_method_map_t
{
   private:

   public:
      DLLLOCAL ~DynamicMethodMap()
      {
	 for (qore_qt_method_map_t::iterator i = begin(), e = end(); i != e; ++i)
	    delete i->second;
      }
      DLLLOCAL int addMethod(DynamicSlot *slot)
      {
	 int id = size();
	 insert(std::make_pair(id, new QoreQtDynamicSlot(slot)));
	 return id;
      }
      DLLLOCAL int addMethod()
      {
	 int id = size();
	 insert(std::make_pair(id, new QoreQtDynamicSignal()));
	 return id;
      }
};

class QoreAbstractQObject : public AbstractPrivateData
{
   private:

   public:

      DLLLOCAL virtual class QObject *getQObject() const = 0;

      // for dynamic signals and slots
      DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink) = 0;
      DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal, bool &is_dynamic) const = 0;
      DLLLOCAL virtual int createSignal(const char *signal) = 0;
      DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink) = 0;
};

#endif
