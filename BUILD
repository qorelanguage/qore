*************************************************
*** COMPILING and INSTALLING QORE FROM SOURCE ***
*************************************************

see README-SVN to lean how to build qore from svn sources

Build Requirements
------------------
*) flex 2.5.31 (or greater -- 2.5.4 or before will NOT work, sorry.  flex 2.5.35 or greater is recommended)
qore requires this very new version of flex in order to build a reentrant parser.  I was not able to build a reentrant parser with earler versions of flex (including 2.5.4).  many older linux distributions ship with flex 2.5.4; this version will not work and the configure script will exit an error message if only this version is found.  You can download flex 2.5.35 at:
	 http://sourceforge.net/projects/flex
To use a flex in an alternative location, set the environment variable LEX before running configure (i.e.: LEX=/usr/local/bin/flex configure ...)

*) bison 1.85 (or better, 2.* versions are fine)
qore requires bison 1.85 or greater to be able to interface properly with the scanner produced by flex 2.5.3*

*) POSIX threads
OS-level POSIX thread support is required to build qore.

*) libxml2 2.6.0 or better
for the XML subsystem - note that this is no longer an optional component of Qore
	http://www.xmlsoft.org
to find libxml2 headers and libraries in a non-standard location, use the --with-libxml2-dir configure option or set the LIBXML2_DIR environment variable

*) PCRE 6 or higher (earlier versions will probably work as well)
for Perl-Compatible Regular Expressions, Qore now uses the pcre library for regular expression support instead of relying on POSIX regex functions.  tested with pcre 6.3 & 6.6
	http://www.pcre.org
if you have the PCRE headers and libraries in a location the configure script cannot find, then you can either use the --with-pcre-libs and --with-pcre-libraries options, or set the PCRE_DIR environment variable before running configure

*) openssl 0.9.7 or higher (0.9.8 or better recommended)
	http://www.openssl.org
if you have the open headers and libraries in a location the configure script cannot find, then you can either use the --with-openssl-libs and --with-openssl-libraries options, or set the OPENSSL_DIR environment variable before running configure

*) bzlib 1.0.4 or higher (earlier versions may work as well)

*) zlib 1.1.3 or higher (some earlier versions will work as well)

"configure" Option Overview
---------------------------
--enable-64bit                      : to build a 64-bit binary (support for x86_64, sparc, and pa-risc architectures)
--disable-static                    : to disable builing a static libqore.a library
--disable-debug                     : to disable debugging code - if you are not planning on debugging the qore language itself then it is highly advised to include this flag, as enabling debugging in qore slows down the language a great deal
--prefix=<dir>                      : default=/usr/local = qore in /usr/local/bin, libraries in /usr/local/lib, modules in /usr/local/lib/qore-module-api-<ver>/
--with-openssl-dir=<dir>            : directory of openssl installation
--with-libxml2-dir=<dir>            : directory of libxml2 installation
--with-pcre-dir=<dir>               : directory of pcre installation
--with-zlib-dir=<dir>               : directory of zlib installation

rarely used options
-------------------
--disable-single-compilation-unit   : to disable building all related files at once in each directory.  This is enabled by default because it normally makes for much quicker compiles and also allows the compiler to optimize based on the entire source at the same time.  However if you don't have enough memory (at least 1G RAM) then you should turn it off, otherwise leave it on.

********************************
recommended configure arguments: configure --disable-static --disable-debug --prefix=/usr   ( add --enable-64bit on 64-bit platforms for 64-bit builds)
********************************

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
Various distributions have been tested: FC3-8, Gentoo, Ubuntu, ARCH, etc

*) Darwin - OS/X
there are no particular issues on newer version of OS/X (10.5+), just make sure you have the prerequisite libraries and header files available - this applies to PCRE on newer versions of OS/X.  On older versions (10.4 and earlier), you'll also need to ensure that you have at least libtool 1.5.10 when building from svn, get it from fink or macports as you like.
NOTE that pthread_create() on Darwin 8.7.1 (OS X 10.4.7) returns 0 (no error) on i386 at least, even when it appears that thread resources are exhausted and the new thread is never started.  This happens after 2560 threads are started, so normally this will not be an issue for most programs.  To make sure that this doesn't happen, when qore is compiled on Darwin MAX_QORE_THREADS is automatically set to 2560 (otherwise the default is normally 4096)

*) Solaris:
The g++ static and shared builds work fine (tested with g++ 4.0.1, 4.1.1, CC).  Note that the sunpro (CC) compiler is required to link with the TIBCO AE SDK.  stlport is no longer supported, so hash_map support is not available on Solaris with CC.  map is used instead.
Note that on Solaris x86 when making a 64-bit build I had to use libtool 1.5.22, libtool 1.5.11 did not recognize that -xarch=generic64 should be passed to the linker and the linker for some reason did not recognize that it should produce a 64-bit output file
Also note that qore requires a relatively new version of the SunPro compiler (CC), Sun Studio 11 works fine, whereas SunPro 5.5 does not.

*) FreeBSD
I have heard that qore builds fine, but I have not actually seen it myself, nor do I have access to a FreeBSD platform for testing :-(

*) HP-UX
HP-UX builds are finally working with g++ (tested 4.1.1), however the configure script include a hack to libtool to get the modules to link dynamic libraries with static libaries and to prohibit -ldl from being automatically included in the link lines.  I am using HP-UX 11.23 (v2) on PA-RISC.
With aCC, PA-RISC 2.0 32-bit binaries are produced in 32-bit mode, with --enable-64-bit, PA-RISC 2.0 64-bit binaries are produced
With g++, PA-RISC 1.1 32-bit binaries are produced in 32-bit mode, with --enable-64-bit, PA-RISC 2.0 64-bit binaries are produced
Qore now uses strtoimax() as a replacement for strtoll() on HP-UX.
Currently there is no fast atomic reference count support on PA-RISC platforms.
Note that only PA-RISC builds have been tested; itanium builds are untested.

*) Windows
Windows is generally not supported, although I have built previous versions on Windows using a Cygwin environment, but the executable was so slow that it's not worth supporting.
Windows may be supported in the future if I get it to work without Cygwin (i.e. using native win32 apis)
There have been numerous requests for this, so any (clean) patches would be appreciated!

CPU Support
-----------
*) gcc with i386, x86_64, ppc, and sparc: fast inline assembly atomic operations are supported for reference counting, as well as a SMP cache invalidation optimization for temporary objects (temporary object do not require a cache invalidation)
*) all others (including sparc & pa-risc): I use a pthread mutex to ensure atomicity for reference counting.  I would be very happy to have atomic operation support for sparc and pa-risc (or other) CPUs for gcc (and CC on Solaris, aCC on HP-UX), but I haven't been able to do it myself yet...
The cache invalidation optimization is not safe on platforms without an atomic reference counting implementation :-(

Modules
-------
On platforms that support building shared libraries, modules are stored in a subdirectory named "qore-module-api-<ver>" of the library directoy.
Modules are installed with the extension *.qmod
Note that modules are (as of version 0.7.0 of qore) delivered separately from the qore library, see the file README-MODULES for more information.
