/*
  glob.h

  Qore Programming Language

  Copyright 2003 - 2011 David Nichols

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

#ifndef _QORE_INTERN_GLOB_H
#define _QORE_INTERN_GLOB_H

#include <assert.h>

#include <string>
#include <vector>

typedef int (*glob_error_t)(const char *, int);

#ifdef _WIN32
class QoreGlobWin {
protected:
   typedef std::vector<std::string> names_t;
   names_t names;

public:
   size_t gl_pathc;
   const char **gl_pathv;

   DLLLOCAL QoreGlobWin() : gl_pathc(0), gl_pathv(0) {
   }

   DLLLOCAL ~QoreGlobWin() {
      reset();
   }

   DLLLOCAL int set(const char *pattern, int flags, glob_error_t errfunc) {
      assert(!flags);
      assert(!errfunc);
      reset();

      char *dirp = q_dirname(pattern);
      unsigned len = strlen(dirp);
      QoreString dir(dirp, len, len + 1, QCS_DEFAULT); 

      // set the pattern to get all files in the directory, and then match according to glob() rules
      dir.concat("\\*.*");
      WIN32_FIND_DATA pfd;
      HANDLE h = ::FindFirstFile(dir.getBuffer(), &pfd);
      ON_BLOCK_EXIT(::FindClose, h);

      while (FindNextFile(h, &pfd)) {
	 printd(0, "QoreGlobWin::set(pattern='%s') %s\n", pattern, pfd.cFileName);
      }

      return 0;
   }

   DLLLOCAL int reset() {
      names.clear();
      gl_pathc = 0;
      if (gl_pathv) {
	 free(gl_pathv);
	 gl_pathv = 0;
      }
      return 0;
   }
};

typedef QoreGlobWin glob_t;
#else
#error no glob implementation for this platform
#endif

// check prototypes
DLLLOCAL int glob(const char *pattern, int flags, glob_error_t errfunc, glob_t *buf);
DLLLOCAL int globfree(glob_t *buf);

#endif
