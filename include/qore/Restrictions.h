/*
  Restrictions.h

  QORE programming language

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

#ifndef _QORE_RESTRICTIONS_H

#define _QORE_RESTRICTIONS_H

/** @file Restrictions.h
    defines parse restrictions and functional domains for builtin functions and qore class methods
*/

#define PO_DEFAULT                   0          //!< no restrictions (except parse option inheritance restrictions)
#define PO_NO_GLOBAL_VARS            (1 <<  0)    //!< cannot define new global variables
#define PO_NO_SUBROUTINE_DEFS        (1 <<  1)    //!< cannot define new user subroutines
#define PO_NO_THREAD_CONTROL         (1 <<  2)    //!< cannot launch new threads) use thread_exit) or access thread data
#define PO_NO_THREAD_CLASSES         (1 <<  3)    //!< no access to thread classes
#define PO_NO_TOP_LEVEL_STATEMENTS   (1 <<  4)    //!< cannot define new top-level statements (outside of sub or class defs)
#define PO_NO_CLASS_DEFS             (1 <<  5)    //!< cannot define new object classes
#define PO_NO_NAMESPACE_DEFS         (1 <<  6)    //!< cannot define new namespaces
#define PO_NO_CONSTANT_DEFS          (1 <<  7)    //!< cannot define new constants
#define PO_NO_NEW                    (1 <<  8)    //!< cannot use the new operator
#define PO_NO_SYSTEM_CLASSES         (1 <<  9)    //!< do not inherit system classes into this program space
#define PO_NO_USER_CLASSES           (1 << 10)    //!< do not inherit user classes into this program space
#define PO_NO_CHILD_PO_RESTRICTIONS  (1 << 11)    //!< turn off parse option inheritance restrictions
#define PO_NO_EXTERNAL_PROCESS       (1 << 12)    //!< do not allow backquote op) system()) exec()
#define PO_REQUIRE_OUR               (1 << 13)    //!< require "our" for global var declaration
#define PO_NO_PROCESS_CONTROL        (1 << 14)    //!< do not allow fork()) exec()) abort(). etc
#define PO_NO_NETWORK                (1 << 15)    //!< do not allow any network access (objs & subroutines)
#define PO_NO_FILESYSTEM             (1 << 16)    //!< do not allow any file access (objects & subroutines)
#define PO_LOCK_WARNINGS             (1 << 17)    //!< do not allow programs to change the warning mask
#define PO_NO_DATABASE               (1 << 18)    //!< do not allow database access
#define PO_NO_GUI                    (1 << 19)    //!< do not allow any GUI-relevant actions to be performed
#define PO_NO_TERMINAL_IO            (1 << 20)    //!< do not allow any terminal I/O to be performed

// combination options
//! cannot access any thread functionality
#define PO_NO_THREADS                (PO_NO_THREAD_CONTROL|PO_NO_THREAD_CLASSES)  

//! prohibits any external access
#define PO_NO_EXTERNAL_ACCESS        (PO_NO_PROCESS_CONTROL|PO_NO_NETWORK|PO_NO_FILESYSTEM|PO_NO_DATABASE) 

//! prohibits all terminal and file I/O and GUI operations
#define PO_NO_IO                     (PO_NO_GUI|PO_NO_TERMINAL_IO|PO_NO_FILESYSTEM) 

//! mask of all options allowing for more freedom (instead of less)
#define PO_POSITIVE_OPTIONS          (PO_NO_CHILD_PO_RESTRICTIONS)

#define QDOM_DEFAULT            PO_DEFAULT                //!< the default domain (no domain)
#define QDOM_PROCESS            PO_NO_PROCESS_CONTROL     //!< provides process control functionality (can affect or stop the current process)
#define QDOM_NETWORK            PO_NO_NETWORK             //!< provides network functionality
#define QDOM_EXTERNAL_PROCESS   PO_NO_EXTERNAL_PROCESS    //!< provides external process control functionality (can affect) start) or stop external processes)
#define QDOM_FILESYSTEM         PO_NO_FILESYSTEM          //!< provides access to the filesystem
#define QDOM_THREAD_CLASS       PO_NO_THREAD_CLASSES      //!< provides thread control functionality
#define QDOM_THREAD_CONTROL     PO_NO_THREAD_CONTROL      //!< provides the ability to check or manipulate threads (including starting new threads)
#define QDOM_DATABASE           PO_NO_DATABASE            //!< provides access to databases
#define QDOM_GUI                PO_NO_GUI                 //!< provides GUI functionality
#define QDOM_TERMINAL_IO        PO_NO_TERMINAL_IO         //!< provides terminal I/O functionality

#endif //_QORE_DOMAIN_H
