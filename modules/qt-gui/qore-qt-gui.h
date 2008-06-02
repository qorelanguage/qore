/*
 qore-qt.h
 
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

#ifndef _QORE_QORE_QT_H
#define _QORE_QORE_QT_H

#include "qt-core.h"

#include <QKeySequence>
#include <QBrush>

#include "BrushStyleNode.h"
#include "PenStyleNode.h"

DLLEXPORT extern AbstractQoreNode *C_Clipboard;

#include <map>

typedef std::map<int, const char *> qt_enum_map_t;

class QAbstractButton;
class QStyle;
class QStyleOption;
class QEvent;
class QAction;
class QWidget;

DLLEXPORT int get_qkeysequence(const AbstractQoreNode *n, QKeySequence &qks, ExceptionSink *xsink, bool suppress_exception = false);
DLLEXPORT int get_qbrush(const AbstractQoreNode *n, QBrush &brush, class ExceptionSink *xsink);

DLLEXPORT QoreObject *return_qabstractbutton(QAbstractButton *button);
DLLEXPORT QoreObject *return_qstyle(const QString &style, QStyle *qs, ExceptionSink *xsink);
DLLEXPORT QoreObject *return_qstyleoption(const QStyleOption *qso);
DLLEXPORT QoreObject *return_qaction(QAction *action);
DLLEXPORT QoreObject *return_qwidget(QWidget *widget, bool managed = true);

// hooks extending qore qt-core functionality
DLLEXPORT AbstractQoreNode *return_gui_qvariant(const QVariant &qv);
DLLEXPORT QoreObject *return_gui_qobject(QObject *o);
DLLEXPORT QoreObject *return_gui_qevent(QEvent *event);

#endif
