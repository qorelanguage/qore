/*
  QT_list.h

  Qore Programming Language

  Copyright (C) 2003, 2004, 2005, 2006 David Nichols

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

#ifndef _QORE_QT_LIST_H

#define _QORE_QT_LIST_H

DLLLOCAL class QoreNode *list_DefaultValue();
DLLLOCAL class QoreNode *list_ConvertTo(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *list_Eval(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *list_eval_opt_deref(bool &needs_deref, class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *list_Copy(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL bool list_Compare(class QoreNode *l, class QoreNode *r);
DLLLOCAL class QoreString *list_MakeString(class QoreNode *n, int format, class ExceptionSink *xsink);
DLLLOCAL void list_DeleteContents(class QoreNode *n);

#endif // _QORE_QT_LIST_H
