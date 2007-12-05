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

#ifndef _QORE_QOREQTDYNAMICMETHOD_H

#define _QORE_QOREQTDYNAMICMETHOD_H

#define QORE_VIRTUAL_QOBJECT_METHODS \
   DLLLOCAL virtual int getSlotIndex(const QByteArray &theSlot, class ExceptionSink *xsink) {\
      return qobj->getSlotIndex(theSlot, xsink);				\
   } \
   DLLLOCAL virtual QoreQtDynamicSlot *getSlot(const char *sig, class ExceptionSink *xsink) {\
      return qobj->getSlot(sig, xsink); \
   } \
   DLLLOCAL virtual int getSignalIndex(const QByteArray &theSignal) const { \
      return qobj->getSignalIndex(theSignal);				\
   } \
   DLLLOCAL virtual int createSignal(const char *signal, class ExceptionSink *xsink) {	\
      return qobj->createDynamicSignal(signal, xsink);			\
   } \
   DLLLOCAL virtual int connectDynamic(QoreAbstractQObject *sender, const char *signal, const char *slot, class ExceptionSink *xsink) { \
      return qobj->connectDynamic(sender, signal, slot, xsink);		\
   } \
   DLLLOCAL virtual void emit_signal(const char *sig, QoreList *args) { return qobj->emit_signal(sig, args); } \
   DLLLOCAL virtual QObject *sender() const { return qobj->getSender(); } \
   DLLLOCAL virtual QoreObject *getQoreObject() const { return qobj->getQoreObject(); } \
   DLLLOCAL virtual void timerEvent(QTimerEvent * event) { qobj->parent_timerEvent(event); } \
   DLLLOCAL virtual void childEvent(QChildEvent * event) { qobj->parent_childEvent(event); }

#include <vector>

#define QQT_TYPE_UNKNOWN             -1
#define QQT_TYPE_VOID                 0
#define QQT_TYPE_INT                  1
#define QQT_TYPE_LONG                 2
#define QQT_TYPE_BOOL                 3
#define QQT_TYPE_FLOAT                4
#define QQT_TYPE_DOUBLE               5
#define QQT_TYPE_P_CHAR               6
#define QQT_TYPE_QDATE                7
#define QQT_TYPE_QFONT                8
#define QQT_TYPE_QSTRING              9
#define QQT_TYPE_P_QWIDGET           10
#define QQT_TYPE_P_QLISTWIDGETITEM   11

union qt_arg_u {
      int t_int;
      float t_float;
      double t_double;
      bool t_bool;
      QString *t_QString;
      QWidget *t_QWidget;
};

struct qt_arg {
      int type;
      union qt_arg_u data;

      DLLLOCAL qt_arg() : type(QQT_TYPE_VOID)
      {
      }
      DLLLOCAL ~qt_arg()
      {
	 if (type == QQT_TYPE_QSTRING)
	    delete data.t_QString;
      }
      DLLLOCAL void *set(int i)
      {
	 data.t_int = i; 
	 return reinterpret_cast<void *>(&data.t_int); 
     }
      DLLLOCAL void *set(float f)  
      { 
	 data.t_float = f; 
	 return reinterpret_cast<void *>(&data.t_float);
      }
      DLLLOCAL void *set(double f)
      { 
	 data.t_double = f; 
	 return reinterpret_cast<void *>(&data.t_double);
      }
      DLLLOCAL void *set(bool b)
      { 
	 data.t_bool = b; 
	 return reinterpret_cast<void *>(&data.t_bool);
      }
      DLLLOCAL void *set(const QString &str) 
      { 
	 data.t_QString = new QString(str); 
	 return reinterpret_cast<void *>(data.t_QString);
      }
      DLLLOCAL void *set(QWidget *qw) 
      { 
	 data.t_QWidget = qw; 
	 return reinterpret_cast<void *>(data.t_QWidget);
      }
};

typedef std::vector<int> type_list_t;

struct QoreQtDynamicMethod {
   protected:
      type_list_t type_list;

      DLLLOCAL static int get_type(const char *&p);

   public:
      virtual ~QoreQtDynamicMethod()
      {
      }
};

class QoreQtDynamicSlot : public QoreQtDynamicMethod
{
   private:
      QoreObject *qore_obj;
      Method *method;
      int return_type;

      DLLLOCAL static Method *resolveMethod(QoreObject *n_qore_obj, const char *name, class ExceptionSink *xsink);

   public:
      DLLLOCAL QoreQtDynamicSlot(QoreObject *n_qore_obj, const char *sig, ExceptionSink *xsink);

      DLLLOCAL virtual ~QoreQtDynamicSlot()
      {
      }

      DLLLOCAL virtual void call(void **arguments);
      DLLLOCAL virtual void call();
};

struct QoreQtDynamicSignal : public QoreQtDynamicMethod
{
   private:

   public:
      DLLLOCAL QoreQtDynamicSignal(const char *sig, ExceptionSink *xsink);
      DLLLOCAL virtual ~QoreQtDynamicSignal()
      {
      }
      DLLLOCAL void emit_signal(QObject *obj, int id, QoreList *args);
};

typedef std::vector<QoreQtDynamicMethod *> qore_qt_method_list_t;

class DynamicMethodMap : public qore_qt_method_list_t
{
   private:

   public:
      DLLLOCAL ~DynamicMethodMap()
      {
	 for (qore_qt_method_list_t::iterator i = begin(), e = end(); i != e; ++i)
	    delete *i;
      }
      DLLLOCAL int addMethod(QoreQtDynamicSlot *slot)
      {
	 int id = size();
	 push_back(slot);
	 return id;
      }
      DLLLOCAL int addMethod(QoreQtDynamicSignal *sig)
      {
	 int id = size();
	 push_back(sig);
	 return id;
      }
};

DLLLOCAL void emit_static_signal(QObject *sender, int signalId, const QMetaMethod &qmm, QoreList *args);

#endif
