/*
 QoreQtDynamicMethod.h
 
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

#ifndef _QORE_QOREQTDYNAMICMETHOD_H

#define _QORE_QOREQTDYNAMICMETHOD_H

#include <QString>
#include <QObject>
#include <QDate>

#include <string>
#include <vector>

/*
      DLLEXPORT void *set(QWidget *qw) 
      { 
	 data.t_QWidget = qw; 
	 return reinterpret_cast<void *>(data.t_QWidget);
      }
*/

class QoreQtAbstractDynamicTypeHelper {
   protected:
      std::string type_name;

   public:
      DLLLOCAL QoreQtAbstractDynamicTypeHelper(const char *name) : type_name(name)
      {
      }
      virtual ~QoreQtAbstractDynamicTypeHelper() {}
      virtual void add_qore_arg(QoreListNode &args, void *arg) = 0;
      virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val) = 0;
      virtual void del_arg(void *ptr) = 0;
      virtual void do_return(void *rv, const AbstractQoreNode *val) = 0;

      DLLLOCAL bool identify(const char *&p)
      {
	 if (!strncmp(type_name.c_str(), p, type_name.size())) {
	    p += type_name.size();
	    return true;
	 }
	 return false;
      }

      DLLLOCAL const char *get_name() const { return type_name.c_str(); }
};

class QoreQtInt : public QoreQtAbstractDynamicTypeHelper
{
   protected:
      DLLLOCAL QoreQtInt(const char *n) : QoreQtAbstractDynamicTypeHelper(n)
      {
      }

   public:
      DLLLOCAL QoreQtInt() : QoreQtAbstractDynamicTypeHelper("int")
      {
      }
      DLLLOCAL virtual void add_qore_arg(QoreListNode &args, void *arg)
      {
	 int *ptr = reinterpret_cast<int *>(arg);
	 args.push(new QoreBigIntNode(*ptr));
      }
      DLLLOCAL virtual void add_qt_arg(void *&ptr, void *&save, const AbstractQoreNode *val)
      {
	 save = (void *)(val ? val->getAsInt() : 0);
	 ptr = &save;
      }
      DLLLOCAL virtual void del_arg(void *ptr)
      {
      }
      DLLLOCAL virtual void do_return(void *rv, const AbstractQoreNode *val)
      {
	 int *ptr = reinterpret_cast<int *>(rv);
	 *ptr = val ? val->getAsInt() : 0;
      }
};

typedef std::vector<QoreQtAbstractDynamicTypeHelper *> type_list_t;

struct QoreQtDynamicMethod {
   protected:
      type_list_t type_list;

      DLLEXPORT static int get_type(const char *&p);

   public:
      virtual ~QoreQtDynamicMethod()
      {
      }
};

class QoreQtDynamicSlot : public QoreQtDynamicMethod
{
   private:
      QoreObject *qore_obj;
      const QoreMethod *method;
      QoreQtAbstractDynamicTypeHelper *return_type;

      DLLEXPORT static const QoreMethod *resolveMethod(QoreObject *n_qore_obj, const char *name, class ExceptionSink *xsink);

   public:
      DLLEXPORT QoreQtDynamicSlot(QoreObject *n_qore_obj, const char *sig, ExceptionSink *xsink);

      DLLEXPORT virtual ~QoreQtDynamicSlot()
      {
      }

      DLLEXPORT virtual void call(void **arguments);
      DLLEXPORT virtual void call();
};

struct QoreQtDynamicSignal : public QoreQtDynamicMethod
{
   private:

   public:
      DLLEXPORT QoreQtDynamicSignal(const char *sig, ExceptionSink *xsink);
      DLLEXPORT virtual ~QoreQtDynamicSignal()
      {
      }
      DLLEXPORT void emit_signal(QObject *obj, int id, const QoreListNode *args);
};

typedef std::vector<QoreQtDynamicMethod *> qore_qt_method_list_t;

class DynamicMethodMap : public qore_qt_method_list_t
{
   private:

   public:
      DLLEXPORT ~DynamicMethodMap()
      {
	 for (qore_qt_method_list_t::iterator i = begin(), e = end(); i != e; ++i)
	    delete *i;
      }
      DLLEXPORT int addMethod(QoreQtDynamicSlot *slot)
      {
	 int id = size();
	 push_back(slot);
	 return id;
      }
      DLLEXPORT int addMethod(QoreQtDynamicSignal *sig)
      {
	 int id = size();
	 push_back(sig);
	 return id;
      }
};

DLLEXPORT void emit_static_signal(QObject *sender, int signalId, const QMetaMethod &qmm, const QoreListNode *args);

#endif
