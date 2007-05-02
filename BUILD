*************************************************
*** COMPILING and INSTALLING QORE FROM SOURCE ***
*************************************************

Build Requirements
------------------
*) flex 2.5.31 (or greater -- 2.5.4 or before will NOT work, sorry, flex 2.5.33 recommended)
qore requires this nonstandard version of flex in order to build a reentrant parser.  I was not able to build a reentrant parser with earler versions of flex (including 2.5.4).  most distributions come with flex 2.5.4; this version will not work and the configure script will exit an error message if only this version is found.  You can download flex 2.5.33 at:
	 http://sourceforge.net/projects/flex

*) bison 1.85 (or better, 2.* versions are fine)
qore requires bison 1.85 or greater to be able to interface properly with the scanner produced by flex 2.5.3[1-3]*

*) POSIX threads
OS-level POSIX thread support is required to build qore.

*) libxml 2.4.22 or better
for the XML subsystem - note that this is no longer an optional component of Qore
	http://www.xmlsoft.org

*) PCRE 6 or higher (earlier versions will probably work as well)
for Perl-Compatible Regular Expressions, Qore now uses the pcre library for regular expression support instead of relying on POSIX regex functions.  tested with pcre 6.3 & 6.6
	http://www.pcre.org
if you have the PCRE headers and libraries in a location the configure script cannot find, then you can either use the --with-pcre-libs and --with-pcre-libraries options, or set the PCRE_DIR environment variable before running configure

*) openssl 0.9.8 or higher (earlier versions will probably work as well)
for ssl support in the Socket class
	http://www.openssl.org
if you have the open headers and libraries in a location the configure script cannot find, then you can either use the --with-openssl-libs and --with-openssl-libraries options, or set the OPENSSL_DIR environment variable before running configure

"configure" Option Overview
---------------------------
--enable-64bit                      : to build a 64-bit binary (support for x86_64, sparc, and pa-risc architectures) - the default is to build a 32-bit binary even on 64-bit platforms
--disable-static                    : to disable builing a static libqore.a library
--disable-debug                     : to disable debugging code - if you are not planning on debugging the qore language itself then it is highly advised to include this flag, as enabling debugging in qore slows down the language a great deal
--prefix=<dir>                      : default=/usr/local = qore in /usr/local/bin, libraries in /usr/local/lib, modules in /usr/local/lib/qore-<ver>/
--with-openssl-dir=<dir>            : directory of openssl installation
--with-libxml2-dir=<dir>            : directory of libxml2 installation
--with-pcre-dir=<dir>               : directory of pcre installation
--with-oracle=<dir>                 : directory of Oracle installation ("oracle" module)
--with-oracle-instant-client=<dir>  : directory of Oracle Instant Client installation ("oracle" module)
--with-mysql=<dir>                  : directory of MySQL installation ("mysql" module)
--with-pgsql=<dir>	            : directory of PostgreSQL installation ("pgsql" module)
--with-sybase=<dir>                 : directory of Sybase OCS installation ("sybase" module)
--with-tibrv=<dir>                  : directory of TIBCO Rendezvous installation ("tibrv" module)
--with-tibae=<dir>                  : directory of TIBCO AE SDK ("tibae" module)
--with-tibae-tpcl=<dir>             : directory of TIBCO AE TPCL installation ("tibae" module)
--with-tuxedo=<dir>                 : directory of Bea Tuxedo installation ("tuxedo" module)
--with-sybase=<dir>                 : directory of Sybase or Sybase OCS installation ("sybase" module)
--with-freetds=<dir>                : directory of FreeTDS installation ("mssql" module)

rarely used options
-------------------
--disable-single-compilation-unit   : to disable building all related files at once in each directory.  This is enabled by default because it normally makes for much quicker compiles and also allows the compiler to optimize based on the entire source at the same time.  However if you don't have enough (virtual) memory then you should turn it off, otherwise leave it on.
--enable-builtin-modules            : will include code for all modules included in the source distribution directly in the shared library - note that this requires a very recent version of libtool otherwise it will fail - normally this option should not be used (has not been tested recently, will be removed in a future release)

********************************
recommended configure arguments: configure --disable-static --disable-debug --prefix=/usr   ( add --enable-64bit on 64-bit platforms for 64-bit builds)
********************************

========= to build optional modules ==========

*) "oracle": Oracle DBI module requires Oracle 9i or better
Oracle DB installation: If you have Oracle 9i or higher you can build in Oracle integration.  Make sure your ORACLE_HOME is set before calling configure (otherwise use the --with-oracle configure option).  Header files and libraries must be available in the standard locations.  
Oracle Instant Client installation: Make sure the ORACLE_INSTANT_CLIENT environment variable is set before you run configure.  Note that on HPUX I have not found a working instant client for 32-bit PA-RISC, for some reason libnnz10 would not link
Also note that on HPUX and Solaris I had to manually link the libclntsh.sl1.10.* (libclintsh.so.10.*) to libclintsh.sl (libclintsh.so) in instant client installations in order to link with instant client installations.
Oracle support in qore is good, very well tested.

*) "mysql": MySQL DBI module requires MySQL 3.3 or better
If you have MySQL 3.3+ or better you can build in MySQL support.  With MySQL 4.1+ you can get transaction support and the module will use the more efficient prepared statement interface.
If your mysql installation is in a non-standard location, set the MYSQL_DIR environment variable to the location of the installation before running configure.
Note that you have to use g++ 4.0.* on Darwin to link with newer versions of the MySQL libraries
MySQL support in qore is good and well tested.

*) "pgsql": PostgreSQL DBI module requires PostgreSQL 7+ client libraries and headers
If your PostgreSQL libraries are in a non-standard location you can use the --with-pgsql configure option or set the PGSQL_DIR environment variable.
PostgreSQL support in qore is good and well tested.

*) "sybase": Sybase DBI module requires Sybase OCS 15+ client libraries and headers
Use --with-sybase or set the SYBASE and SYBASE_OCS environment variables to build the "sybase" module.  note that the sybase module has not been tested with x86_64 builds yet

*) "mssql": FreeTDS-based Sybase and Microsoft SQL Server driver, requires FreeTDS headers and libraries
User --with-freetds or set the FREETDS environment variable to your FreeTDS installation to build the "mssql" module.  Note that the "mssql" driver is built from the same source as the "sybase" driver.

*) "tibrv": TIBCO Rendezvous module requires TIBCO Rendezvous 7.x (6 may work)
Set the RV_ROOT environment variable to the Rendezvous directory before calling configure (or use the --with-tibrv configure option) to build the "tibrv" module for direct Rendezvous support.  Note that to build this module the libtibrvcpp library must be present; on some platforms you have to rebuild this yourself from the sources provided by TIBCO in order for it to link with the C++ compiler you are using - the sources are normally present in $RV_ROOT/src/librvcpp, normally you have to edit the Makefile provided there and then type "make" to rebuild.  I had to include "ranlib libtibrvcpp.a" on the libraries I rebuilt for OS X.  Secure daemon support is turned off by default in tibrvcpp, to enable secure daemon support edit $RV_ROOT/src/librvcpp/Makefile and uncomment the SD_MODULE line near the end of the file, rebuild, install the new library in $RV_ROOT/lib, and rerun qore's configure script

*) "tibae": TIBCO AE module requires TIBCO SDK 5.2.1 or better
If you have TIBCO Rendezvous and the AE SDK installed, and the supported C++ compiler, you can build in TIBCO AE integration.  Make sure that the RV_ROOT, SDK_ROOT, and TPCL_ROOT environment variables are pointing to your Rendezvous, SDK, and TPCL directories respectively before calling configure.  Otherwise you can use the --with-tibrv, --with-tibae, and with-tibae-tpcl configure options.  The TIBCO module will compile with SDK 4.* versions, but there are so many bugs in this version of the SDK (including some horrible dynamic memory leaks) that it doesn't make sense to use anything before 5.2.1...
Note that this module is not supported on HP-UX PA-RISC platforms due to the fact that the compiler requirements for aCC for the SDK are incompatible with compiling qore.

*) "tuxedo": BEA Tuxedo support requores Tuxedo 8 or better

*) "ncurses": ncurses module
note that this module is still experimental due to the fact that I'm not sure if it's possible to safely enable threading without putting a big lock around every curses call.
if your ncurses is in a non-standard location, set the NCURSES_DIR environment variable before running configure.  Also can be built with Solaris curses.

To build qore, run the following commands:

   ./configure [options]  (for non-debugging builds I recommend: configure --disable-static --disable-debug --prefix=/usr)
   make

Installing Qore
---------------
To install qore once it's been built, run the following commands:

   make install

by default the program will be installed in /usr/local/bin and libraries in /usr/local/lib, with language modules in /usr/local/lib/qore-<ver>.  This can be changed with the --prefix option to configure.  The name of the binary is "qore"

OS-Specific Issues
------------------
*) Linux:
there are no particular issues on Linux, this is the main development platform.
Various distributions have been tested: FC3-5, Gentoo, Ubuntu, ARCH, etc

*) Darwin - OS/X
I use fink to provide libtool 1.5.10 (libtool14 package), which works for me to build shared libraries and shared modules on Darwin.  I was not able to make a build with any other version of libtool on Darwin (particularly not the version of libtool that comes with OS/X - i.e. /usr/bin/glibtool on OS/X 10.4.2 is version 1.5 and did not work for me).  Also this version seems to have a bug: it created modules with a suffix of *.so instead of *.dylib.  I have built in a workaround for this in the configure.ac script, under darwin, when builind modules, the module suffix is set to .so...
Older builds worked fine with 10.3.8, currently the new version with shared libraries & modules has been tested only on 10.4.[2-7] with g++ 4.0.[01] (with some fink/port components) on i386 and ppc
NOTE that pthread_create() on Darwin 8.7.1 (OS X 10.4.7) returns 0 (no error) on i386 at least, even when it appears that thread resources are exhausted and the new thread is never started.  This happens after 2560 threads are started, so normally this will not be an issue for most programs.  To make sure that this doesn't happen, when qore is compiled on Darwin MAX_QORE_THREADS is automatically set to 2560 (otherwise the default is normally 4096)

*) Solaris:
The g++ static and shared builds work fine (tested with g++ 4.0.1, 4.1.1, CC).  Note that the sunpro (CC) compiler is required to link with the TIBCO AE SDK.  stlport is no longer supported, so hash_map support is not available on Solaris with CC.  map is used instead.
Note that on Solaris x86 when making a 64-bit build I had to use libtool 1.5.22, libtool 1.5.11 did not recognize that -xarch=generic64 should be passed to the linker and the linker for some reason did not recognize that it should produce a 64-bit output file
Also note that qore requires a relatively new version of the SunPro compiler (CC), Sun Studio 11 works fine, whereas SunPro 5.5 does not.

*) FreeBSD
I have heard that qore builds fine, but I have not actually seen it myself, nor do I have access to a FreeBSD platform for testing :-(

*) HP-UX
HP-UX builds are finally working with g++ (tested 4.1.1 and aCC), however the configure script include a hack to libtool to get the modules to link dynamic libraries with static libaries and to prohibit -ldl from being automatically included in the link lines.  I am using HP-UX 11.23 (v2) on PA-RISC.
PA-RISC 2.0 32-bit binaries are produced in 32-bit mode, with --enable-64-bit, PA-RISC 2.0 64-bit binaries are produced
Qore now uses strtoimax() as a replacement for strtoll() on HP-UX.
The TIBCO Adapters module (tibae) is not supported on PA-RISC because the compiler requirements are incompatible with compiling qore.
Currently there is no fast atomic reference count support on PA-RISC platforms.
Note that only PA-RISC builds have been tested; itanium builds are untested.

*) Windows
Windows is generally not supported, although I have built previous versions on Windows using a Cygwin environment, but the executable was so slow that it's not worth supporting.  Windows may be supported in the future if I get it to work without Cygwin (i.e. using native win32 apis)
There have been numerous requests for this, so any (clean) patches would be appreciated!

CPU Support
-----------
*) i386, x86_64, and ppc: fast inline assembly atomic operations are supported for reference counting, as well as a SMP cache invalidation optimization for temporary objects (temporary object do not require a cache invalidation)
*) all others (including sparc & pa-risc): I use a pthread mutex to ensure atomicity for reference counting.  I would be very happy to have atomic operation support for sparc and pa-risc (or other) CPUs for gcc (and CC on Solaris, aCC on HP-UX), but I haven't been able to do it myself yet...
The cache invalidation optimization is not safe on platforms without an atomic reference counting implementation :-(

Modules
-------
On platforms that support building shared libraries, modules are stored in a subdirectory named "qore-<ver>" of the library directoy.
Modules are installed with the extension *.qmod
