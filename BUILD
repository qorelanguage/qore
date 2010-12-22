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

*) PCRE 6 or higher (earlier versions will probably work as well)
for Perl-Compatible Regular Expressions, Qore now uses the pcre library for regular expression support instead of relying on POSIX regex functions.  tested with pcre 6.3 & 6.6
	http://www.pcre.org
if you have the PCRE headers and libraries in a location the configure script cannot find, then you can either use the --with-pcre-libs and --with-pcre-libraries options, or set the PCRE_DIR environment variable before running configure

*) openssl 0.9.7 or higher (0.9.8 or better recommended)
	http://www.openssl.org
if you have the open headers and libraries in a location the configure script cannot find, then you can either use the --with-openssl-libs and --with-openssl-libraries options, or set the OPENSSL_DIR environment variable before running configure

*) bzlib 1.0.4 or higher (earlier versions may work as well)

*) zlib 1.1.3 or higher (some earlier versions will work as well)

(note that libxml2 is no longer a build requirement as all xml functionality has been moved to the xml module)

"configure" Option Overview
---------------------------
--enable-64bit                      : to build a 64-bit binary (support for x86_64, sparc, and pa-risc architectures)
--disable-static                    : to disable builing a static libqore.a library
--disable-debug                     : to disable debugging code - if you are not planning on debugging the qore language itself then it is highly advised to include this flag, as enabling debugging in qore slows down the language a great deal
--prefix=<dir>                      : default=/usr/local = qore in /usr/local/bin, libraries in /usr/local/lib, modules in /usr/local/lib/qore-module-api-<ver>/
--with-openssl-dir=<dir>            : directory of openssl installation
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
there are no particular issues on Linux, this is one of the main development platforms.
Various distributions have been tested: FC3-8, Gentoo, Ubuntu, ARCH, etc

*) Darwin - OS/X
One of the main development platforms for Qore.  There are no particular issues on newer version of OS/X (10.5+), just make sure you have the prerequisite libraries and header files available - this applies to PCRE on newer versions of OS/X.  On older versions (10.4 and earlier), you'll also need to ensure that you have at least libtool 1.5.10 when building from svn, get it from fink or macports as you like.
NOTE that pthread_create() on Darwin 8.7.1 (OS X 10.4.7) returns 0 (no error) on i386 at least, even when it appears that thread resources are exhausted and the new thread is never started.  This happens after 2560 threads are started, so normally this will not be an issue for most programs.  To make sure that this doesn't happen, when qore is compiled on Darwin MAX_QORE_THREADS is automatically set to 2560 (otherwise the default is normally 4096)

*) Solaris:
One of the main development platforms for Qore.  g++ and CC static and shared builds work fine (tested with many versions of g++ and CC).
Note that on Solaris x86 when making a 64-bit build I had to use libtool 1.5.22, libtool 1.5.11 did not recognize that -xarch=generic64 should be passed to the linker and the linker for some reason did not recognize that it should produce a 64-bit output file
Also note that qore requires a relatively new version of the SunPro compiler (CC), Sun Studio 11 and 12 work fine, whereas SunPro 5.5 does not.

*) FreeBSD
Is working; freebsd port is available here: http://www.freebsd.org/cgi/cvsweb.cgi/ports/lang/qore/

*) HP-UX
HP-UX builds are finally working with g++ (tested 4.1.1 and 4.2.0), however the configure script include a hack to libtool to get the modules to link dynamic libraries with static libaries and to prohibit -ldl from being automatically included in the link lines.  I am using HP-UX 11.23 (v2) on PA-RISC.
With aCC, PA-RISC 2.0 32-bit binaries are produced in 32-bit mode, with --enable-64-bit, PA-RISC 2.0 64-bit binaries are produced
With g++, PA-RISC 1.1 32-bit binaries are produced in 32-bit mode, with --enable-64-bit, PA-RISC 2.0 64-bit binaries are produced
Qore now uses strtoimax() as a replacement for strtoll() on HP-UX.
Currently there is no fast atomic reference count support on PA-RISC platforms.
when compiling on Itanium 64-bit binaries are produced by default
Note that as of qore 0.8.0, building with aCC A.03.90 (for HP-UX PA-RISC 11.23) failed; unfortunately I don't have access to a newer OS and compiler combination to try

*) Windows
As of 0.8.0, qore builds on Windows with cygwin, but only as a monolithic binary.  No modules are working yet.
Windows may be supported in the future if I get it to work without Cygwin (i.e. using native win32 apis)
There have been numerous requests for this, so any patches would be appreciated!

CPU Support
-----------
*) gcc with i386, x86_64, ppc, sparc32, itanium, aCC with itanium, CC (SunPro or Sun Studio CC) with i386, x86_64, sparc32: fast inline assembly atomic operations are supported for reference counting, as well as a SMP cache invalidation optimization for temporary objects (temporary object do not require a cache invalidation);
*) CPU stack guard is working on all above combinations and with g++ and aCC with PA-RISC as well
*) all others: I use a pthread mutex to ensure atomicity for reference counting.
The cache invalidation optimization is not safe on platforms without an atomic reference counting implementation, therefore is not implemented for these platforms

Modules
-------
On platforms that support building shared libraries, modules are stored in a subdirectory named "qore-module-api-<ver>" of the library directoy.
Modules are installed with the extension *.qmod
Note that modules are (as of version 0.7.0 of qore) delivered separately from the qore library, see the file README-MODULES for more information.
