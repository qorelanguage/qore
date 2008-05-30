/*
 qt-core.h
 
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

#ifndef _QORE_QT_CORE_H
#define _QORE_QT_CORE_H

#include <QDate>
#include <QDateTime>
#include <QObject>
#include <QList>
#include <QStringList>

DLLEXPORT int get_qdate(const AbstractQoreNode *n, QDate &date, class ExceptionSink *xsink);
DLLEXPORT int get_qtime(const AbstractQoreNode *n, QTime &time, class ExceptionSink *xsink);
DLLEXPORT int get_qdatetime(const AbstractQoreNode *n, QDateTime &dt, class ExceptionSink *xsink);
DLLEXPORT int get_qbrush(const AbstractQoreNode *n, QBrush &brush, class ExceptionSink *xsink);
DLLEXPORT int get_qvariant(const AbstractQoreNode *n, QVariant &qv, class ExceptionSink *xsink, bool suppress_exception = false);
DLLEXPORT int get_qbytearray(const AbstractQoreNode *n, QByteArray &qba, class ExceptionSink *xsink, bool suppress_exception = false);
DLLEXPORT int get_qchar(const AbstractQoreNode *n, QChar &c, class ExceptionSink *xsink, bool suppress_exception = false);
DLLEXPORT int get_qstring(const AbstractQoreNode *n, QString &str, class ExceptionSink *xsink, bool suppress_exception = false);

DLLEXPORT QoreObject *return_object(const QoreClass *qclass, AbstractPrivateData *data);
DLLEXPORT QoreObject *return_qobject(QObject *o);
DLLEXPORT QoreObject *return_qevent(QEvent *event);

DLLEXPORT AbstractQoreNode *return_qvariant(const QVariant &qv);
DLLEXPORT QoreListNode *return_qstringlist(const QStringList &l);

typedef AbstractQoreNode *(*return_qvariant_hook_t)(const QVariant &qv);
typedef QoreObject *(*return_qobject_hook_t)(QObject *o);
typedef QoreObject *(*return_qevent_hook_t)(QEvent *e);

DLLEXPORT void register_return_qvariant_hook(return_qvariant_hook_t hook);
DLLEXPORT void register_return_qobject_hook(return_qobject_hook_t hook);
DLLEXPORT void register_return_qevent_hook(return_qevent_hook_t hook);

#endif
