/*
  QorePseudoMethods.h

  Qore Programming Language

  Copyright 2003 - 2012 David Nichols

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

#ifndef _QORE_PSEUDOMETHODS_H

#define _QORE_PSEUDOMETHODS_H

DLLLOCAL void pseudo_classes_init();
DLLLOCAL void pseudo_classes_del();
DLLLOCAL AbstractQoreNode *pseudo_classes_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL int64 pseudo_classes_int64_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL int pseudo_classes_int_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL bool pseudo_classes_bool_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL double pseudo_classes_double_eval(const AbstractQoreNode *n, const char *name, const QoreListNode *args, ExceptionSink *xsink);
DLLLOCAL const QoreMethod *pseudo_classes_find_method(qore_type_t t, const char *mname, QoreClass *&qc);
DLLLOCAL const QoreMethod *pseudo_classes_find_method(const QoreTypeInfo *typeInfo, const char *mname, QoreClass *&qc, bool &possible_match);

#define NODE_ARRAY_LEN (NT_NUMBER + 1)
DLLLOCAL extern QoreBigIntNode *Node_NT_Array[NODE_ARRAY_LEN];

#endif
