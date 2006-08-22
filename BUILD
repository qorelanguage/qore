*************************************************
*** COMPILING and INSTALLING QORE FROM SOURCE ***
*************************************************

Build Requirements
------------------
*) flex 2.5.31 (or greater -- 2.5.4 or before will NOT work, sorry)
qore requires this nonstandard version of flex in order to build a reentrant parser.  I was not able to build a reentrant parser with earler versions of flex (including 2.5.4).  most distributions come with flex 2.5.4; this version will not work and the configure script will exit an error message if only this version is found.  You can download flex 2.5.31 at:
	 http://sourceforge.net/projects/lex

*) bison 1.85 (or better, 2.* versions are fine)
qore requires bison 1.85 or greater to be able to interface properly with the scanner produced by flex 2.5.31

*) POSIX threads
Support for building single-threaded versions of Qore has been removed - POSIX thread functions are now required to build qore.

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

NOTE that --enable-builtin-modules will only work with libtool 1.5.22 or better, which you have to copy to the root build directory by hand before you build the modules, otherwise libtool doesn't build the static libraries properly

"configure" Option Overview
---------------------------
--enable-64bit                 : to build a 64-bit binary (currently only supported on x86_64 architectures) - the default is to build a 32-bit binary even on 64-bit platforms
--disable-debug                : to disable debugging code - if you are not planning on debugging the qore language itself then it is highly advised to include this flag, as enabling debugging in qore slows down the language a great deal
--prefix=<dir>                 : default=/usr/local = qore in /usr/local/bin, libraries in /usr/local/lib, modules in /usr/local/lib/qore-<ver>/
--enable-builtin-modules       : will include code for all modules included in the source distrcibution directly in the shared library - note that this requires a very recent version of libtool otherwise it will fail - normally this option should not be used
--with-openssl-libs=<dir>      : directory for openssl library
--with-openssl-includes=<dir>  : directory for openssl include files
--with-pcre-libs=<dir>         : directory for pcre library
--with-pcre-includes=<dir>     : directory for pcre includes
--with-tibrv=<dir>             : directory for TIBCO Rendezvous installation ("tibrv" module)
--with-tibae=<dir>             : directory for TIBCO AE SDK ("tibae" module)
--with-tibae-tpcl=<dir>        : directory for TIBCO AE TPCL installation ("tibae" module)
--with-oracle=<dir>            : directory for Oracle installation ("oracle" module)
--with-mysql-includes=<dir>    : directory for MySQL includes ("mysql" module)
--with-mysql-libs=<dir>        : directory for MySQL libraries ("mysql" module)

recommended configure arguments: configure --disable-static --disable-debug --prefix=/usr   ( add --enable-64bit on x86_64 platforms for 64-bit builds)


========= to build optional modules ==========

*) "oracle": Oracle DBI module requires Oracle 9i or better
If you have Oracle 9i or higher you can build in Oracle integration.  Make sure your ORACLE_HOME is set before calling configure (otherwise use the --with-oracle configure option).  Header files and libraries must be available in the standard locations.  Oracle support is good.  See below for information on limitations of the Oracle driver.

*) "mysql": MySQL DBI module requires MySQL 3.3 or better
If you have MySQL 3.3+ or better you can build in MySQL support.  With MySQL 4.1+ you can get transaction support and the module will use the more efficient prepared statement interface.

*) "tibrv": TIBCO Rendezvous module requires TIBCO Rendezvous 7.x (6 may work)
Set the RV_ROOT environment variable to the Rendezvous directory before calling configure (or use the --with-tibrv configure option) to build the "tibrv" module for direct Rendezvous support.  Note that to build this module the libtibrvcpp library must be present; on some platforms you have to rebuild this yourself from the sources provided by TIBCO in order for it to link with the C++ compiler you are using - the sources are normally present in $RV_ROOT/src/librvcpp, normally you have to edit the Makefile provided there and then type "make" to rebuild.  I had to include "ranlib libtibrvcpp.a" on the libraries I rebuilt for OS X.  Secure daemon support is turned off by default in tibrvcpp, to enable secure daemon support edit $RV_ROOT/src/librvcpp/Makefile and uncomment the SD_MODULE line near the end of the file, rebuild, install the new library in $RV_ROOT/lib, and rerun qore's configure script

*) "tibae": TIBCO AE module requires TIBCO SDK 5.2.1 or better
If you have TIBCO Rendezvous and the AE SDK installed, and the supported C++ compiler, you can build in TIBCO AE integration.  Make sure that the RV_ROOT, SDK_ROOT, and TPCL_ROOT environment variables are pointing to your Rendezvous, SDK, and TPCL directories respectively before calling configure.  Otherwise you can use the --with-tibrv, --with-tibae, and with-tibae-tpcl configure options.  The TIBCO module will compile with SDK 4.* versions, but there are so many bugs in this version of the SDK (including some horrible dynamic memory leaks) that it doesn't make sense to use anything before 5.2.1...

*) "ncurses": ncurses module
note that this module is still experimental due to the fact that I'm not sure if it's possible to safely enable threading without putting a big lock around every curses call.  Right now Solaris curses is not properly detected by the configure script although if it were it the module can be built - I have to fix this in configure.ac

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
NOTE that pthread_create() on Darwin 8.7.1 (OS X 10.4.7) returns 0 (no error) on i386 at least, even when it appears that thread resources are exhausted and the new thread is never started.  This happens after 2560 threads are started, so normally this will not be an issue for most programs.

*) Solaris:
The g++ builds work fine (tested with g++ 4.0.1).  With CC (which I use to get the TIBCO AE SDK to link) I build a monolithic binary for easier deployment on our production site.  hash_map is detected and supported with CC 5.5 and stlport4 as well, however it is disabled if the "tibae" module is compiled in, because stlport4 clashes with the iostream library already linked in to the TIBCO AE SDK, therefore hash lookups will be slower on Solaris if the "tibae" module is used :-(

*) FreeBSD
I have heard that qore builds fine, but I have not actually seen it myself, nor do I have access to a FreeBSD platform for testing :-(

*) HP-UX
HP-UX builds are still not working 100% out of the box, at least not for me.  I suspect that libtool is the problem; I can get it to build with g++, but only using --disable-shared and then performing some link steps by hand (adding -L/usr/local/lib -liconv -L/usr/lib -lz  ... I think).  
Qore now uses strtoimax() as a replacement for strtoll() on HP-UX.  
Qore will not yet build with aCC without making some minor changes - aCC doesn't like to reuse local variables at the same lexical level (for example, in a for loop, like for (int i = 0; ,,,) {} - if you do this twice in a row the compiler will complain about variable i being declared twice even though it should be local to each loop - which means that the version of aCC I was using does not follow the C++ standard as far as I can tell) - the easy fix is to rename the additional occurrences of the variable in those functions...  I would be happy to hear from people who get qore working on HP platforms, and I would be even happier to get patches to support cleaner builds without the hand patching...

*) Windows
Windows is generally not supported, although I have built previous versions on Windows using a Cygwin environment, but the executable was so slow that it's not worth supporting.  Windows may be supported in the future if I get it to work without Cygwin (i.e. using native win32 apis)

CPU Support
-----------
*) i386, x86_64, and ppc: fast inline assembly atomic operations are supported for reference counting
*) all others (including sparc & pa-risc): I use a pthread mutex to ensure atomicity for reference counting.  I would be very happy to have atomic operation support for sparc and pa-risc (or other) CPUs for gcc (and CC on Solaris, aCC on HP-UX), but I haven't been able to do it myself yet...

Modules
-------
On platforms that support building shared libraries, modules are stored in a subdirectory named "qore-<ver>" of the library directoy.
Modules are installed with the extension *.qmod
