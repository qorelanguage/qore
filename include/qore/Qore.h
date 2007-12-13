/*
  Qore.h

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

#ifndef _QORE_QORE_H

#define _QORE_QORE_H

#include <qore/common.h>
#include <qore/QoreList.h>
#include <qore/QoreProgram.h>
#include <qore/ModuleManager.h>
#include <qore/QoreLib.h>
#include <qore/QoreNode.h>
#include <qore/QoreNet.h>
#include <qore/QoreFile.h>
#include <qore/QoreSocket.h>
#include <qore/QoreHash.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreObject.h>
#include <qore/QoreNamespace.h>
#include <qore/QoreException.h>
#include <qore/BinaryObject.h>
#include <qore/support.h>
#include <qore/QoreString.h>
#include <qore/DateTime.h>
#include <qore/QoreType.h>
#include <qore/charset.h>
#include <qore/params.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/qore_thread.h>
#include <qore/LockedObject.h>
#include <qore/QoreCondition.h>
#include <qore/DBI.h>
#include <qore/Datasource.h>
#include <qore/QoreClass.h>
#include <qore/ScopeGuard.h>
#include <qore/ReferenceHolder.h>
#include <qore/Environment.h>
#include <qore/AutoVLock.h>

// include private definitions if compiling the library
#ifdef _QORE_LIB_INTERN
#include <qore/intern/Function.h>
#include <qore/intern/AbstractStatement.h>
#include <qore/intern/QoreLibIntern.h>
#include <qore/Variable.h>
#include <qore/intern/NamedScope.h>
#include <qore/ScopedObjectCall.h>
#include <qore/intern/ClassRef.h>
#include <qore/intern/Context.h>
#include <qore/intern/Operator.h>
#include <qore/Tree.h>
#include <qore/Datasource.h>
#include <qore/VRMutex.h>
#include <qore/intern/VLock.h>

DLLLOCAL extern int qore_library_options;
#endif

DLLEXPORT extern char qore_version_string[];
DLLEXPORT extern int qore_version_major;
DLLEXPORT extern int qore_version_minor;
DLLEXPORT extern int qore_version_sub;
DLLEXPORT extern int qore_build_number;
DLLEXPORT extern int qore_target_bits;
DLLEXPORT extern char qore_target_os[];
DLLEXPORT extern char qore_target_arch[];

// qore library initialization options
#define QLO_NONE                       0
#define QLO_DISABLE_SIGNAL_HANDLING    1

DLLEXPORT void qore_init(char *def_charset = NULL, bool show_module_errors = false, int init_options = QLO_NONE);
DLLEXPORT void qore_cleanup();

#endif  // _QORE_QORE_H
