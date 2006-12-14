#/*
  QT_context.h

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

#ifndef _QORE_QT_CONTEXT_H

#define _QORE_QT_CONTEXT_H

DLLLOCAL class QoreNode *contextref_Eval(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *complexcontextref_Eval(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL class QoreNode *complexcontextref_Copy(class QoreNode *n, class ExceptionSink *xsink);
DLLLOCAL void complexcontextref_DeleteContents(class QoreNode *n);

#endif // _QORE_QT_CONTEXT_H
