/*
 qore-qt.h
 
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

#ifndef _QORE_QORE_QT_H

#define _QORE_QORE_QT_H

DLLLOCAL extern QoreType *NT_BRUSHSTYLE, *NT_PENSTYLE;

DLLLOCAL extern Object *C_ClipBoard;

#include <map>

typedef std::map<int, const char *> qt_enum_map_t;

static inline class QoreNode *make_enum(class QoreType *enum_type, int code)
{
   QoreNode *n = new QoreNode(enum_type);
   n->val.intval = code;
   return n;
}

DLLLOCAL int get_qdate(class QoreNode *n, QDate &date, class ExceptionSink *xsink);
DLLLOCAL int get_qtime(class QoreNode *n, QTime &time, class ExceptionSink *xsink);
DLLLOCAL int get_qdatetime(class QoreNode *n, QDateTime &dt, class ExceptionSink *xsink);
DLLLOCAL int get_qvariant(class QoreNode *n, QVariant &qv, class ExceptionSink *xsink, bool suppress_exception = false);
DLLLOCAL int get_qbytearray(class QoreNode *n, QByteArray &qba, class ExceptionSink *xsink);
DLLLOCAL int get_qchar(class QoreNode *n, QChar &c, class ExceptionSink *xsink);
DLLLOCAL int get_qstring(class QoreNode *n, QString &str, class ExceptionSink *xsink, bool suppress_exception = false);

DLLLOCAL class QoreNode *return_qvariant(QVariant &qv);

#endif
