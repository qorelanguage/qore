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

========= to build optional modules ==========

*) Oracle 9i or better for the Oracle DBI module
If you have Oracle 9i or higher you can build in Oracle integration.  Make sure your ORACLE_HOME is set before calling configure (otherwise use the --with-oracle configure option).  Header files and libraries must be available in the standard locations.  Oracle support is good.  See below for information on limitations of the Oracle driver.

*) MySQL 3.3 or better for the MySQL DBI module
If you have MySQL 3.3+ or better you can build in MySQL support.  With MySQL 4.1+ you can get transaction support and qore will use the more efficient prepared statement interface.

*) TIBCO SDK 5.2.1 or better for the TIBCO module
If you have TIBCO Rendezvous and the AE SDK installed, and the supported C++ compiler, you can build in TIBCO AE integration.  Make sure that the RV_ROOT and SDK_ROOT environment variables are pointing to your Rendezvous and SDK directories respectively.  Otherwise you can use the --with-tibco-rv and --with-tibco-sdk configure options.  The TIBCO module will compile with SDK 4.* versions, but there are so many bugs in this version of the SDK (including some horrible dynamic memory leaks) that it doesn't make sense to use anything before 5.2.1...

To build qore, run the following commands:

   ./configure [options]  (for non-debugging builds I recommend: configure --disable-static --disable-debug)
   make

Installing Qore
---------------
To install qore once it's been built, run the following commands:

   make install

by default the program will be installed in /usr/local/bin and libraries in /usr/local/lib, with language modules in /usr/local/lib/qore.  This can be changed with the --prefix option to configure.  The name of the binary is "qore"

OS-Specific Issues
------------------
*) Linux:
there are no particular issues on Linux, this is the main development platform.  
Various distributions have been tested: FC3, FC4, FC5, Gentoo, Ubuntu, ARCH, etc

*) Darwin - OS/X
I use fink to provide libtool 1.5.10 (libtool14 package), which works for me to build shared libraries and shared modules on Darwin.  I was not able to make a build with any other version of libtool on Darwin (particularly not the version of libtool that comes with OS/X - i.e. /usr/bin/glibtool on OS/X 10.4.2 is version 1.5 and did not work for me).  Also this version seems to have a bug: it created modules with a suffix of *.so instead of *.dylib.  I have built in a workaround for this in the configure.ac script, under darwin, when builind modules, the module suffix is set to .so...
Older builds worked fine with 10.3.8, currently the new version with shared libraries & modules has been tested only on 10.4.[234] with g++ 4.0.0 (with some fink components)

*) Solaris:
The g++ builds work fine (tested with g++ 4.0.1).  With CC (which I use to get the TIBCO SDK to link) I build a monolithic binary for easier deployment on our production site.  hash_map is detected and supported with CC 5.5 and stlport4 as well, however it is disabled if the TIBCO module is compiled in, because stlport4 clashes with the iostream library already linked in to the TIBCO SDK, therefore hash lookups will be slower on Solaris if the TIBCO module is used :-(

*) FreeBSD
I have heard that qore builds fine, but I have not actually seen it myself, nor do I have access to a FreeBSD platform for testing :-(

*) HP-UX
HP-UX builds are still not working 100% out of the box, at least not for me.  I suspect that libtool is the problem; I can get it to build with g++, but only using --disable-shared and then performing some link steps by hand (adding -L/usr/local/lib -liconv -L/usr/lib -lz  ... I think).  
Qore now uses strtoimax() as a replacement for strtoll() on HP-UX.  
Qore will not yet build with aCC without making some minor changes - aCC doesn't like to reuse local variables in a for loop at the same lexical level (which means that the version of aCC I was using does not follow the C++ standard as far as I can tell) - the easy fix is to rename the additional occurrences of the variable in those functions...  I would be happy to hear from people who get qore working on HP platforms, and I would be even happier to get patches to support cleaner builds without the hand patching...

CPU Support
-----------
*) i386 & powerpc: fast inline assembly atomic operations are supported for reference counting
*) all others (including sparc & pa-risc): I use a pthread mutex to ensure atomicity for reference counting.  I would be very happy to have atomic operation support for sparc (or other) CPUs for gcc (and CC on Solaris), but I haven't been able to do it yet...

Modules
-------
On platforms that support building shared libraries, modules are stored in a subdirectory named "qore" of the library directoy.
Modules are installed with the extension *.qmod
