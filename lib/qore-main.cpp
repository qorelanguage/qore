/*
  qore-main.cpp

  Qore Programming Language

  Copyright (C) 2003 - 2014 David Nichols

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
#include <qore/QoreHttpClientObject.h>

#include <qore/intern/QoreSignal.h>
#include <qore/intern/ModuleInfo.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#ifdef DARWIN
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ;
#endif

#include <vector>

int qore_trace = 0;
int debug = 0;
int qore_library_options = QLO_NONE;

qore_license_t qore_license;

const QoreStringMaker mpfrInfo("runtime: %s built with: %s (%d.%d.%d)", mpfr_get_version(), MPFR_VERSION_STRING, MPFR_VERSION_MAJOR,
      MPFR_VERSION_MINOR, MPFR_VERSION_PATCHLEVEL);

// static locks for openssl
typedef std::vector<QoreThreadLock*> mutex_vec_t;
static mutex_vec_t q_openssl_mutex_list;

static unsigned long q_openssl_id_function(void) {
#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__ 
   return GetCurrentThreadId();
#else
   return (unsigned long)pthread_self();
#endif
}

static void q_openssl_locking_function(int mode, int n, const char* file, int line) {
   if (mode & CRYPTO_LOCK)
      q_openssl_mutex_list[n]->lock();
   else
      q_openssl_mutex_list[n]->unlock();
}

void qore_init(qore_license_t license, const char *def_charset, bool show_module_errors, int n_qore_library_options) {
   qore_license = license;
   qore_library_options = n_qore_library_options;

   // initialize openssl library
   if (!qore_check_option(QLO_DISABLE_OPENSSL_INIT)) {
      OPENSSL_config(0);
      SSL_load_error_strings();
      OpenSSL_add_all_algorithms();
      SSL_library_init();
      ERR_load_crypto_strings();

      // create locks
      for (int i = 0; i < CRYPTO_num_locks(); ++i)
	 q_openssl_mutex_list.push_back(new QoreThreadLock());

      CRYPTO_set_id_callback(q_openssl_id_function);
      CRYPTO_set_locking_callback(q_openssl_locking_function);
   }

   qore_string_init();
   QoreHttpClientObject::static_init();

   // init threading infrastructure
   init_qore_threads();

   // initialize charset encoding support
   QEM.init(def_charset);

   // init character maps
   init_charmaps();

   init_lib_intern(environ);

   // create default type values
   init_qore_types();

   // set up core operators
   oplist.init();

   // init module subsystem
   QMM.init(show_module_errors);

#ifdef HAVE_SIGNAL_HANDLING
   // init signals
   QSM.init(qore_library_options & QLO_DISABLE_SIGNAL_HANDLING);
#endif

   // initialize static system namespaces
   staticSystemNamespace = new StaticSystemNamespace();

   // set up pseudo-methods
   pseudo_classes_init();

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__ 
   // do windows socket initialization
   WORD wsver = MAKEWORD(2, 2);
   WSADATA wsd;
   int err = WSAStartup(wsver, &wsd);
   if (err)
      printf("qore_init(): WSAStartup() failed with error: %d; sockets will not be available\n", err);
#endif
}

// NOTE: we do not cleanup in reverse initialization order
// the threading subsystem is deleted before the modules are
// unloaded in case there are any module-specific thread
// cleanup functions to be run...
void qore_cleanup() {
   // first delete all user modules
   QMM.delUser();

#if (defined _WIN32 || defined __WIN32__) && ! defined __CYGWIN__ 
   // do windows socket cleanup
   WSACleanup();
#endif

#ifdef HAVE_SIGNAL_HANDLING
   // stop signal manager
   QSM.del();
#endif

   // purge thread resources before deleting modules
   {
      ExceptionSink xsink;
      purge_thread_resources(&xsink);
   }

   // delete all loadable modules
   QMM.cleanup();

   // delete thread-local data
   delete_thread_local_data();

   // now free memory (like ARGV, QORE_ARGV, ENV, etc)
   delete_global_variables();

   // delete pseudo-methods
   pseudo_classes_del();

   // delete static system namespace after modules
   delete staticSystemNamespace;
#ifdef DEBUG
   staticSystemNamespace = 0;
#endif

   // delete default type values
   delete_qore_types();

   // delete threading infrastructure
   delete_qore_threads();

   // only perform openssl cleanup if not performed externally
   if (!qore_check_option(QLO_DISABLE_OPENSSL_CLEANUP)) {
      // cleanup openssl library
      ERR_free_strings();

      ENGINE_cleanup();
      EVP_cleanup();

      CONF_modules_finish();
      CONF_modules_free();
      CONF_modules_unload(1);

      CRYPTO_cleanup_all_ex_data();

      CRYPTO_set_id_callback(0);
      CRYPTO_set_locking_callback(0);

      // delete openssl locks
      for (mutex_vec_t::iterator i = q_openssl_mutex_list.begin(), e = q_openssl_mutex_list.end(); i != e; ++i)
	 delete *i;
   }
   printd(5, "qore_cleanup() exiting cleanly\n");
}
