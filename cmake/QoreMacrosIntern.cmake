#
# Written by Niclas Rosenvik <youremailsarecrap@gmail.com>
# Based on the tests found in qores configure.ac
#
#

include(CheckCSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckCXXSymbolExists)
include(CheckFunctionExists)
include(CheckIncludeFileCXX)


macro(qore_check_headers_cxx)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs)

    cmake_parse_arguments(_HEADERS "${options}" "${onevalueArgs}" "${multiValueArgs}" ${ARGN})

    foreach (include_file ${_HEADERS_UNPARSED_ARGUMENTS})
        string(REPLACE "." "_" include_file_ ${include_file})
	string(REPLACE "/" "_" _include_file_ ${include_file_})
	string(TOUPPER ${_include_file_} upper_include_file_)
	string(CONCAT have_var "HAVE_" ${upper_include_file_})
	check_include_file_cxx(${include_file} ${have_var})
#	string(CONCAT file_output ${file_output} "\n#cmakedefine " ${have_var})
    endforeach()
#    file(WRITE ${CMAKE_BINARY_DIR}/header_defines.txt ${file_output})
#    unset(file_output)
endmacro()



macro(qore_check_funcs)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs)

    cmake_parse_arguments(_FUNCS "${options}" "${onevalueArgs}" "${multiValueArgs}" ${ARGN})

    foreach (FUNCTION_TO_CHECK ${_FUNCS_UNPARSED_ARGUMENTS})
        string(TOUPPER ${FUNCTION_TO_CHECK} UPPER_FUNCTION_TO_CHECK)
	string(CONCAT HAVE_VAR "HAVE_" ${UPPER_FUNCTION_TO_CHECK})
	check_function_exists(${FUNCTION_TO_CHECK} ${HAVE_VAR})
#	string(CONCAT file_output ${file_output} "\n#cmakedefine " ${HAVE_VAR})
    endforeach()
#    file(WRITE ${CMAKE_BINARY_DIR}/funcs_defines.txt ${file_output})
#    unset(file_output)
endmacro()

macro(qore_openssl_checks)

set(CMAKE_REQUIRED_INCLUDES ${OPENSSL_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${OPENSSL_LIBRARIES})

check_cxx_symbol_exists(EVP_sha512 openssl/evp.h HAVE_OPENSSL_SHA512)

# check for openssl const correctness (later versions have it)
check_cxx_source_compiles("
#include <openssl/x509.h>
int main(void) {
const unsigned char *p;
d2i_X509(0, &p, 0l);
return 0;
}" 
HAVE_OPENSSL_CONST)

# check for const required with SSL_CTX_new
check_cxx_source_compiles("
#include <openssl/ssl.h>
int main(void) {
const SSL_METHOD *meth;
SSL_CTX_new(meth);
return 0;
}" 
NEED_SSL_CTX_NEW_CONST)

# check for return value with HMAC_Update and friends
check_cxx_source_compiles("
#include <openssl/hmac.h>
int main(void) {
int rc = HMAC_Update(0, 0, 0);
return 0;
}" 
HAVE_OPENSSL_HMAC_RV)

unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)

endmacro()



macro(qore_mpfr_checks)

set(CMAKE_REQUIRED_INCLUDES ${MPFR_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${MPFR_LIBRARIES})

check_cxx_symbol_exists(mpfr_divby0_p mpfr.h HAVE_MPFR_DIVBY0)
check_cxx_symbol_exists(mpfr_regular_p mpfr.h HAVE_MPFR_REGULAR)
check_cxx_symbol_exists(mpfr_sprintf mpfr.h HAVE_MPFR_SPRINTF)
check_cxx_symbol_exists(mpfr_buildopt_tls_p mpfr.h HAVE_MPFR_BUILDOPT_TLS_P)

check_cxx_source_compiles("
#include <mpfr.h>
int main(void){
mpfr_exp_t test;
return 0;
}"
HAVE_MPFR_EXP_T)

check_cxx_source_compiles("
#include <mpfr.h>
int main(void){
mpfr_rnd_t test = MPFR_RNDN;
return 0;
}"
HAVE_RNDN)


unset(CMAKE_REQUIRED_INCLUDES)
unset(CMAKE_REQUIRED_LIBRARIES)

endmacro()



macro(qore_find_pthreads)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Found POSIX Threads: TRUE")
else(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Found POSIX Threads: FALSE")
    message(FATAL_ERROR "POSIX threads do not seem to be supported on this platform, aborting")
endif()
endmacro()



macro(qore_gethost_checks)

check_cxx_symbol_exists(gethostbyaddr_r netdb.h HAVE_GETHOSTBYADDR_R)
check_cxx_symbol_exists(gethostbyname_r netdb.h HAVE_GETHOSTBYNAME_R)

if(HAVE_GETHOSTBYADDR_R)

# Check definition of gethostbyaddr_r (glibc2 defines this with 8 arguments)
check_c_source_compiles("
#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int main(void) {
int skr;
struct hostent *foo = gethostbyaddr_r((const char *) 0,0, 0, (struct hostent *) 0, (char *) NULL,  0, &skr);
return (foo == 0);
}"
HAVE_SOLARIS_STYLE_GETHOST)

endif(HAVE_GETHOSTBYADDR_R)



if(HAVE_GETHOSTBYNAME_R)

# Check definition of gethostbyname_r (glibc2.0.100 is different from Solaris)
check_c_source_compiles("
#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int main(void){
int skr;
skr = gethostbyname_r((const char *) 0,(struct hostent*) 0, (char*) 0, 0, (struct hostent **) 0, &skr);
return 0;
}"
HAVE_GETHOSTBYNAME_R_GLIBC2_STYLE)

# Check 3rd argument of gethostbyname_r
check_c_source_compiles("
#undef inline
#if !defined(SCO) && !defined(__osf__) && !defined(_REENTRANT)
#define _REENTRANT
#endif
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int main(void){
int skr;
skr = gethostbyname_r((const char *) 0, (struct hostent*) 0, (struct hostent_data*) 0);
return 0;
}"
HAVE_GETHOSTBYNAME_R_RETURN_INT)

endif(HAVE_GETHOSTBYNAME_R)

endmacro()



macro(qore_other_checks)

# check if the compiler supports variadic local arrays
check_cxx_source_compiles("
int main(void){
int x = 2; int y[x];
return 0;
}"
HAVE_LOCAL_VARIADIC_ARRAYS)

# see if struct flock is declared
check_c_source_compiles("
#include <fcntl.h>
int main(void){
struct flock fl;
return 0;
}"
HAVE_STRUCT_FLOCK)

check_cxx_compiler_flag(-fvisibility=hidden HAVE_GCC_VISIBILITY)

check_cxx_symbol_exists(strtoimax inttypes.h HAVE_STRTOIMAX)

endmacro()

macro(create_git_revision)
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/include/qore/intern/git-revision.h)
   find_package(Git)
   if(GIT_FOUND)
       execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		       OUTPUT_VARIABLE GIT_REV OUTPUT_STRIP_TRAILING_WHITESPACE)
       string(CONCAT GIT_REV_FILE_OUTPUT "#define BUILD \"" ${GIT_REV} "\"")
       file(WRITE ${CMAKE_BINARY_DIR}/include/qore/intern/git-revision.h ${GIT_REV_FILE_OUTPUT})
   else()
       message(FATAL_ERROR "Git is needed to generate git-revision.h")
   endif()
   
endif()

endmacro()
