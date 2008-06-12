/*
  QoreDir.h

  Directory object

  Qore Programming Language

  Copyright (C) 2005 David Nichols
  
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

#ifndef _QORE_QOREDIR_H

#define _QORE_QOREDIR_H

#include <dirent.h>

#include <string>
#include <vector>

#ifndef HAVE_DIRENT_D_TYPE
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef DT_DIR
#define DT_DIR 1
#endif
#endif

//! provides controlled access to file data through Qore data structures
/** Each file has a default character encoding associated with it.  String data
    read from the file will be tagged with this encoding.  String data written to
    the file will be converted to this encoding if necessary before written.
    @see QoreEncoding
 */
class QoreDir {
   private:
      //! private implementation
      struct qore_qd_private *priv;
      
      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreDir(const QoreDir&);

      //! this function is not implemented; it is here as a private function in order to prohibit it from being used
      DLLLOCAL QoreDir& operator=(const QoreDir&);
      
      // helper functions
      const std::string stripPath(const std::string&);
      void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiter);

   public:
      //! creates the object and sets the default encoding
      /**
	 the initial directory is the current directoy.
	 @param cs the encoding to use for this directory
	 @param xsink if an out of memory error occurs in the constructor
      */
      DLLEXPORT QoreDir(const QoreEncoding *cs, ExceptionSink *xsink);

      //! closes the directory and frees all memory allocated to the object
      DLLEXPORT ~QoreDir();

      //! changes the directory in relation to the current
      /**
	 @param dir the directory to change to. can include .. and .
	 @param xsink if the path is relative and no current directory is set, a qore-language exception is thrown
	 @return 0 for success
      */
      DLLEXPORT int chdir(const char* dir, ExceptionSink *xsink);
      
      DLLEXPORT int verifyDirectory(const char* dir); // check if the directory is accessable
      DLLEXPORT int checkPath(); // check if the stored path is accessable
      DLLEXPORT int exists(); // check if the directory exists
      DLLEXPORT int create(int, ExceptionSink*);

      DLLEXPORT const char* dirname() const;

      //! returns the encoding used for the directory
      DLLEXPORT const QoreEncoding *getEncoding() const;
      
      //!returns a new file in this directory
      /**
	 @param xsink if errors occur, qore-language exception information is added here
	 @param fn the name of the file to be opened
	 @param flags the flags to use when opening the file
	 @param mode the mode mask to use when opening the file
	 @param cs the encoding to use for the file
	 @return the File class, throws an exception on error
      */
      DLLEXPORT QoreFile openFile(ExceptionSink *xsink, const char* fn, int flags=O_RDONLY, int mode=0777, const QoreEncoding *cs=QCS_DEFAULT);

      // for internal use a wrapper for list
      DLLLOCAL QoreListNode* list(int, ExceptionSink *xsink, const QoreString *regex = 0, int regex_options = 0);
};

#endif  // _QORE_QOREDIR_H
