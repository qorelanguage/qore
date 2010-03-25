/*
  main.cc

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

#include <qore/Qore.h>

#include <qore/DBI.h>
#include <qore/intern/QoreSignal.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <libxml/parser.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef DARWIN
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif

int qore_trace = 0;
int debug = 0;
int qore_library_options = QLO_NONE;

qore_license_t qore_license;

void qore_init(qore_license_t license, const char *def_charset, bool show_module_errors, int n_qore_library_options) {
   qore_license = license;

   qore_library_options = n_qore_library_options;

   // initialize libxml2 library
   LIBXML_TEST_VERSION

   // initialize openssl library
   SSL_load_error_strings();
   OpenSSL_add_all_algorithms();
   SSL_library_init();

   // init threading infrastructure
   init_qore_threads();

   init_lib_intern(environ);

   // initialize charset encoding support
   QEM.init(def_charset);

   // create default type values
   init_qore_types();

   // set up core operators
   oplist.init();

   builtinFunctions.init();

   // init module subsystem
   MM.init(show_module_errors);

   // init signals
   QSM.init(qore_library_options & QLO_DISABLE_SIGNAL_HANDLING);
}

// NOTE: we do not cleanup in reverse initialization order
// the threading subsystem is deleted before the modules are
// unloaded in case there are any module-specific thread
// cleanup functions to be run...
void qore_cleanup()
{
   // stop signal manager
   QSM.del();

   // now free memory
   delete_global_variables();

   // clear the list before modules are unloaded
   builtinFunctions.clear();

   // delete all loadable modules
   MM.cleanup();

   // delete default type values
   delete_qore_types();

   // delete threading infrastructure
   delete_qore_threads();

   // cleanup openssl library
   EVP_cleanup();
   ERR_free_strings();

   // cleanup libxml2 library
   xmlCleanupParser();
}
