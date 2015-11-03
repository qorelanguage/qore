/*
  Qore.h

  Qore Programming Language

  Copyright 2003 - 2009 David Nichols

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

/** @file Qore.h
    the main header file for the Qore library.  All code using any part of the Qore library's functionality should include this file
*/

#include <qore/common.h>
#include <qore/QoreEncoding.h>
#include <qore/ReferenceHolder.h>
#include <qore/AbstractQoreNode.h>
#include <qore/QoreNodeEvalOptionalRefHolder.h>
#include <qore/QoreListNode.h>
#include <qore/QoreProgram.h>
#include <qore/ModuleManager.h>
#include <qore/QoreLib.h>
#include <qore/QoreStringNode.h>
#include <qore/DateTimeNode.h>
#include <qore/QoreHashNode.h>
#include <qore/QoreBigIntNode.h>
#include <qore/QoreBoolNode.h>
#include <qore/QoreFloatNode.h>
#include <qore/QoreNothingNode.h>
#include <qore/QoreNullNode.h>
#include <qore/QoreNet.h>
#include <qore/QoreURL.h>
#include <qore/QoreFile.h>
#include <qore/AbstractPrivateData.h>
#include <qore/QoreObject.h>
#include <qore/QoreNamespace.h>
#include <qore/ExceptionSink.h>
#include <qore/BinaryNode.h>
#include <qore/QoreString.h>
#include <qore/DateTime.h>
#include <qore/QoreType.h>
#include <qore/BuiltinFunctionList.h>
#include <qore/qore_thread.h>
#include <qore/QoreThreadLock.h>
#include <qore/QoreThreadLocalStorage.h>
#include <qore/QoreCondition.h>
#include <qore/DBI.h>
#include <qore/Datasource.h>
#include <qore/QoreClass.h>
#include <qore/ScopeGuard.h>
#include <qore/SystemEnvironment.h>
#include <qore/AutoVLock.h>
#include <qore/CallReferenceNode.h>
#include <qore/ReferenceNode.h>
#include <qore/params.h>
#include <qore/ReferenceHelper.h>
#include <qore/QoreEvents.h>
#include <qore/qore-version.h>

//! the complete version string of the qore library
DLLEXPORT extern const char *qore_version_string;

//! the major version number of the qore library
DLLEXPORT extern int qore_version_major;

//! the minor version number of the qore library
DLLEXPORT extern int qore_version_minor;

//! the version number below the minor version number of the qore library
DLLEXPORT extern int qore_version_sub;

//! the build number of the qore library
DLLEXPORT extern int qore_build_number;

//! the build target machine word size in bits (32 or 64 normally)
DLLEXPORT extern int qore_target_bits;

//! the build target Operating System name
DLLEXPORT extern const char *qore_target_os;

//! the build target machine architecture name
DLLEXPORT extern const char *qore_target_arch;

//! the qore module directory
DLLEXPORT extern const char *qore_module_dir;

//! the c++ compiler used to build qore
DLLEXPORT extern const char *qore_cplusplus_compiler;

//! the compiler flags used to build qore
DLLEXPORT extern const char *qore_cflags;

//! the linker flags used to link qore
DLLEXPORT extern const char *qore_ldflags;

//! information about the build host
DLLEXPORT extern const char *qore_build_host;

//! if the qore library includes debugging or not
DLLEXPORT bool qore_has_debug();

#define QLO_NONE                    0       //!< no options (default)
#define QLO_DISABLE_SIGNAL_HANDLING 1 << 0  //!< disable qore signal handling entirely

//! initializes the Qore library
/** @param license the license that the library will be used under; note that if the license type is QL_LGPL, then modules tagged with QL_GPL cannot be loaded 
    @param default_encoding the default character encoding for the library, if 0 then the environment variables QORE_CHARSET and LANG will be processed, in that order, to determine the default character encoding.  If no character encoding can be determined from either of these environment variables, UTF-8 will be used as the default.
    @param show_module_errors if true then any errors loading qore modules will be output to stdout
    @param init_options a binary "or" sum of the qore library options
    @note The openssl and libxml2 libraries are also initialized in this function.
    @note This function can only be called once and must be called before any other qore facilities are used.
    @note The license value must be QL_LGPL unless the program using Qore is a GPL program, in which case QL_GPL may be used (the default)
    @see qore_cleanup()
 */
DLLEXPORT void qore_init(qore_license_t license = QL_GPL, const char *default_encoding = 0, bool show_module_errors = false, int init_options = QLO_NONE);

//! frees all memory allocated by the library
/** @note The openssl and libxml2 libraries are cleaned up as well
    @note This function can only be called once and should be called when a program using the Qore library terminates.
    @see qore_init()
 */
DLLEXPORT void qore_cleanup();

// include private definitions if compiling the library
#ifdef _QORE_LIB_INTERN
#include <qore/intern/QoreLibIntern.h>
#endif

#include <qore/support.h>

#endif  // _QORE_QORE_H
