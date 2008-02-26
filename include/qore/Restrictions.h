/*
  Restrictions.h

  QORE programming language

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

#ifndef _QORE_RESTRICTIONS_H

#define _QORE_RESTRICTIONS_H

//! parse options bitfield enum type
enum qore_restrictions_t {
   PO_INVALID                   = -1,         //!< invalid restriction code
   PO_DEFAULT                   = 0,          //!< no restrictions (except parse option inheritance restrictions)
   PO_NO_GLOBAL_VARS            = (1 <<  1),  //!< cannot define new global variables
   PO_NO_SUBROUTINE_DEFS        = (1 <<  2),  //!< cannot define new user subroutines
   PO_NO_THREAD_CONTROL         = (1 <<  3),  //!< cannot launch new threads, use thread_exit, or access thread data
   PO_NO_THREAD_CLASSES         = (1 <<  4),  //!< no access to thread classes
   PO_NO_TOP_LEVEL_STATEMENTS   = (1 <<  5),  //!< cannot define new top-level statements (outside of sub or class defs)
   PO_NO_CLASS_DEFS             = (1 <<  6),  //!< cannot define new object classes
   PO_NO_NAMESPACE_DEFS         = (1 <<  7),  //!< cannot define new namespaces
   PO_NO_CONSTANT_DEFS          = (1 <<  8),  //!< cannot define new constants
   PO_NO_NEW                    = (1 <<  9),  //!< cannot use the new operator
   PO_NO_SYSTEM_CLASSES         = (1 << 10),  //!< do not inherit system classes into this program space
   PO_NO_USER_CLASSES           = (1 << 11),  //!< do not inherit user classes into this program space
   PO_NO_CHILD_PO_RESTRICTIONS  = (1 << 12),  //!< turn off parse option inheritance restrictions
   PO_NO_EXTERNAL_PROCESS       = (1 << 13),  //!< do not allow backquote op, system(), exec()
   PO_REQUIRE_OUR               = (1 << 14),  //!< require "our" for global var declaration
   PO_NO_PROCESS_CONTROL        = (1 << 15),  //!< do not allow fork(), exec(), abort(). etc
   PO_NO_NETWORK                = (1 << 16),  //!< do not allow any network access (objs & subroutines)
   PO_NO_FILESYSTEM             = (1 << 17),  //!< do not allow any file access (objects & subroutines)
   PO_LOCK_WARNINGS             = (1 << 18),  //!< do not allow programs to change the warning mask
   PO_NO_DATABASE               = (1 << 19),  //!< do not allow database access
   PO_NO_GUI                    = (1 << 20),  //!< do not allow any GUI-relevant actions to be performed
   PO_NO_TERMINAL_IO            = (1 << 21),  //!< do not allow any terminal I/O to be performed

   // combination options
   PO_NO_THREADS                = (PO_NO_THREAD_CONTROL|PO_NO_THREAD_CLASSES),  //!< cannot access any thread functionality

   PO_NO_EXTERNAL_ACCESS        = (PO_NO_PROCESS_CONTROL|PO_NO_NETWORK|PO_NO_FILESYSTEM|PO_NO_DATABASE), //!< prohibits any external access

   PO_NO_IO                     = (PO_NO_GUI|PO_NO_TERMINAL_IO|PO_NO_FILESYSTEM),  //!< prohibits all terminal and file I/O and GUI operations

   //! mask of all options allowing for more freedom (instead of less)
   PO_POSITIVE_OPTIONS          = (PO_NO_CHILD_PO_RESTRICTIONS),
};

//! functional domains for builtin functions and classes
enum qore_domain_t {
   QDOM_DEFAULT            = PO_DEFAULT,                //!< the default domain (no domain)
   QDOM_PROCESS            = PO_NO_PROCESS_CONTROL,     //!< provides process control functionality (can affect or stop the current process)
   QDOM_NETWORK            = PO_NO_NETWORK,             //!< provides network functionality
   QDOM_EXTERNAL_PROCESS   = PO_NO_EXTERNAL_PROCESS,    //!< provides external process control functionality (can affect, start, or stop external processes)
   QDOM_FILESYSTEM         = PO_NO_FILESYSTEM,          //!< provides access to the filesystem
   QDOM_THREAD_CLASS       = PO_NO_THREAD_CLASSES,      //!< provides thread control functionality
   QDOM_THREAD_CONTROL     = PO_NO_THREAD_CONTROL,      //!< provides the ability to check or manipulate threads (including starting new threads)
   QDOM_DATABASE           = PO_NO_DATABASE,            //!< provides access to databases
   QDOM_GUI                = PO_NO_GUI,                 //!< provides GUI functionality
   QDOM_TERMINAL_IO        = PO_NO_TERMINAL_IO,         //!< provides terminal I/O functionality
};

#endif //_QORE_RESTRICTIONS_H
