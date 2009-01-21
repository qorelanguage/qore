/*
  QoreDir.h

  thread-safe Directory object with a private implementation

  Qore Programming Language

  Copyright (C) 2003 - 2009 David Nichols
  
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

#endif

//! provides controlled access to the filesystem through directories
/** Each QoreDir object has a default character encoding associated with it.  The class is thread-safe.
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
      
   public:
      //! creates the object and sets the default encoding
      /**
	 @param xsink if an out of memory error occurs in the constructor
	 @param cs the encoding to use for this directory
	 @param dir the initial directory; 0 = the current directory
      */
      DLLEXPORT QoreDir(ExceptionSink *xsink, const QoreEncoding *cs = QCS_DEFAULT, const char *dir = 0);

      //! copies the object
      /** @param xsink a Qore-language exception can be raised only on out of memory
	  @param old the old object to copy
       */
      DLLEXPORT QoreDir(ExceptionSink *xsink, const QoreDir &old);

      //! closes the directory and frees all memory allocated to the object
      DLLEXPORT ~QoreDir();

      //! changes the directory in relation to the current
      /**
	 @param dir the directory to change to. can include .. and .
	 @param xsink if the path is relative and no current directory is set, a qore-language exception is thrown
	 @return 0 for success
      */
      DLLEXPORT int chdir(const char* dir, ExceptionSink *xsink);

      //! creates a subdirectory of the current directory
      /** @param xsink if an error occurs, qore-language exception information is added here
	  @param subdir the subdirectory name to create
	  @param mode the mode of the directory to create (default = 0777)
	  @return 0 = OK, -1 = an exception was raised
      */
      DLLEXPORT int mkdir(ExceptionSink *xsink, const char *subdir, int mode = 0777) const;
      
      //! removes a subdirectory of the current directory
      /** @param subdir the subdirectory name to remove
	  @param xsink if errors occur, qore-language exception information is added here
	  @return 0 = OK, -1 = an exception was raised
      */
      DLLEXPORT int rmdir(const char *subdir, ExceptionSink *xsink) const;

      //! changes the mode of the current directory
      /** @param mode the moddoe to change the directory to
	  @param xsink if errors occur, qore-language exception information is added here
	  @return 0 = OK, -1 = an exception was raised
      */
      DLLEXPORT int chmod(int mode, ExceptionSink *xsink) const;

      //! changes the user and/or group owner for the current directory
      /** @param uid the UID to change to (-1 = leave the same)
	  @param gid the GID to change to (-1 = leave the same)
	  @param xsink if errors occur, qore-language exception information is added here
	  @return 0 = OK, -1 = an exception was raised
      */	  
      DLLEXPORT int chown(uid_t uid, gid_t gid, ExceptionSink *xsink) const;

      //! creates the current directory, including all parent directories, if they do not exist
      /** @param mode the mode for any directoreis created
	  @param xsink if errors occur, qore-language exception information is added here
	  @return the number of directories created, -1 = error
      */
      DLLEXPORT int create(int mode, ExceptionSink *xsink) const;

      //! returns 0 = OK (path exists), non-zero = errno returned by opendir()
      /** @return 0 = OK (path exists), non-zero = errno returned by opendir()
       */
      DLLEXPORT int checkPath() const;

      //! returns the current directory name or 0 if none is set
      /** @return the current directory name or 0 if none is set
       */
      DLLEXPORT QoreStringNode *dirname() const;

      //! returns a complete path with the argument appended to the current directory name
      /** @param sub the subdirectory name to append to the current directory name
	  @return a complete path with the argument appended to the current directory name
       */
      DLLEXPORT std::string getPath(const char *sub) const;

      //! returns the encoding used for the filesystem
      /** @return the encoding used for the filesystem
       */
      DLLEXPORT const QoreEncoding *getEncoding() const;
      
      //! returns a new file in this directory
      /**
	 @param xsink if errors occur, qore-language exception information is added here
	 @param fn the name of the file to be opened
	 @param flags the flags to use when opening the file
	 @param mode the mode mask to use when opening the file
	 @param cs the encoding to use for the file
	 @return the File class, throws an exception on error
      */
      DLLEXPORT QoreFile openFile(ExceptionSink *xsink, const char* fn, int flags = O_RDONLY, int mode = 0777, const QoreEncoding *cs = QCS_DEFAULT);

      //! returns a list of files in the current directory, taking an option regular expression filter
      /** @param xsink if errors occur, qore-language exception information is added here
	  @param stat_filter set to -1 to get everything, otherwise use S_* constants from stat() to filter for particular file types
	  @param regex an optional regular expression to filter the resulting list
	  @param regex_options optional regular expression options
	  @return a list of the results
       */
      DLLEXPORT QoreListNode* list(ExceptionSink *xsink, int stat_filter, const QoreString *regex = 0, int regex_options = 0) const;
};

#endif  // _QORE_QOREDIR_H
