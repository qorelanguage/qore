Summary: Qore Programming Language
Name: qore
Version: 0.6.2
Release: 1%{dist}
License: LGPL
Group: Development/Languages
URL: http://qore.sourceforge.net
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
qore is a modular, multithreaded, weakly-typed, object-oriented programming language suitable for embedding application logic, application scripting, interface development, and even complex multi-threaded, network-aware object-oriented application development. Qore features integrated XML capability, oracle, mysql, TIBCO Rendezvous and Adapters modules, as well as built-in date arithmetic, method and member redirection for classes, private methods, synchronized (in the Java sense) functions and class methods, national character set support with implicit and explicit character set conversions, exception handling, Perl5-compatible regular expression support, powerful and easy-to-use data structures (arrays, hashes, etc), and much more. Qore is under active development, very well-tested, and is also under commercial use as the technology behind the Qorus Integration Engine (formerly OM/Qore).

%package oracle-module
Summary: Oracle DBI module for Qore
Group: Development/Languages

%description oracle-module
Oracle DBI driver module for the Qore Programming Language. The Oracle driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc.

%files oracle-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/oracle.qmod
%else
/usr/lib64/qore-0.6.2/oracle.qmod
%endif

%package mysql-module
Summary: MySQL DBI module for Qore
Group: Development/Languages

%description mysql-module
MySQL DBI driver module for the Qore Programming Language. The MySQL driver is character set aware and supports multithreading, transaction management, and stored procedure execution.

%files mysql-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/mysql.qmod
%else
/usr/lib64/qore-0.6.2/mysql.qmod
%endif

%package pgsql-module
Summary: PostgreSQL DBI module for Qore
Group: Development/Languages

%description pgsql-module
PostgreSQL DBI driver module for the Qore Programming Language. The PostgreSQL driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc.

%files pgsql-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/pgsql.qmod
%else
/usr/lib64/qore-0.6.2/pgsql.qmod
%endif

%package sybase-module
Summary: Sybase DBI module for Qore
Group: Development/Languages

%description sybase-module
Sybase DBI driver module for the Qore Programming Language. The Sybase driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc.

%files sybase-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/sybase.qmod
%else
/usr/lib64/qore-0.6.2/sybase.qmod
%endif

%package mssql-module
Summary: FreeTDS-based MS-SQL and Sybase DBI module for Qore
Group: Development/Languages

%description mssql-module
FreeTDS-based MS-SQL Server and Sybase DBI driver module for the Qore Programming Language. This driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc, and can be used to connect to Sybase and Microsoft SQL Server databases.

%files mssql-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/mssql.qmod
%else
/usr/lib64/qore-0.6.2/mssql.qmod
%endif

#%ifarch i386 sparc
#%package tibae-module
#Summary: TIBCO Adapters integration module for Qore
#Group: Development/Languages
#
#%description tibae-module
#This module provides the TibcoAdapter class, which enables qore scripts/programs to communicate with (or implement) TIBCO Adapters.
#
#%files tibae-module
#/usr/lib/qore-0.6.2/tibae.qmod
#%endif

%package tibrv-module
Summary: TIBCO Rendezvous integration module for Qore
Group: Development/Languages

%description tibrv-module
This module provides functionality enabling qore scripts/programs to communicate using TIBCO Rendezvous publish-subscribe messaging (reliable and certified protocols), join and monitor fault-tolerant groups, join distributed queues, etc.

%files tibrv-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/tibrv.qmod
%else
/usr/lib64/qore-0.6.2/tibrv.qmod
%endif

%package tuxedo-module
Summary: BEA Tuxedo(R) client API integration module for Qore
Group: Development/Languages

%description tuxedo-module
This module provides functionality enabling qore scripts/programs to communicate using the BEA Tuxedo(R) client API.

%files tuxedo-module
%ifnarch x86_64
/usr/lib/qore-0.6.2/tuxedo.qmod
%else
/usr/lib64/qore-0.6.2/tuxedo.qmod
%endif

%prep
%setup -q
cxx=g++
%ifarch x86_64
c64=--enable-64bit
%endif

CXX=$cxx ./configure RPM_OPT_FLAGS="$RPM_OPT_FLAGS" --prefix=/usr --disable-debug --disable-static $c64

%build
make -j4

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
%ifarch x86_64
mkdir -p $RPM_BUILD_ROOT/usr/lib64/qore-0.6.2
mkdir -p $RPM_BUILD_ROOT/usr/lib64/qore-0.6.2/auto
%else
mkdir -p $RPM_BUILD_ROOT/usr/lib/qore-0.6.2
mkdir -p $RPM_BUILD_ROOT/usr/lib/qore-0.6.2/auto
%endif
mkdir -p $RPM_BUILD_ROOT/usr/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/examples
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/test
make install prefix=$RPM_BUILD_ROOT/usr

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc COPYING README RELEASE-NOTES CHANGELOG AUTHORS WHATISQORE docs/roadmap.html docs/qore-style.css docs/img docs/qore.html examples/ test/

/usr/bin/qore
%ifnarch x86_64
/usr/lib/libqore.so.3.0.0
/usr/lib/libqore.so.3
/usr/lib/libqore.so
/usr/lib/libqore.la
/usr/lib/qore-0.6.2/ncurses.qmod
%else
/usr/lib64/libqore.so.3.0.0
/usr/lib64/libqore.so.3
/usr/lib64/libqore.so
/usr/lib64/libqore.la
/usr/lib64/qore-0.6.2/ncurses.qmod
%endif
/usr/share/man/man1/qore.1.gz

%changelog
* Thu Jun 14 2007 David Nichols <david_nichols@users.sourceforge.net>
- fixed spec file to support omre architectures

* Wed Jun 13 2007 David Nichols <david_nichols@users.sourceforge.net>
- removed tibae module from spec file due to compiler requiremenets (g++-32)
- added pgsql module

* Tue Feb 20 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated to libqore.so.3.0.0

* Tue Feb 11 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated to 0.6.2 and libqore 1.1

* Tue Jan 30 2007 David Nichols <david_nichols@users.sourceforge.net>
- added tuxedo module

* Fri Jan 5 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated libqore so version to 1.0.0

* Sat Nov 18 2006 David Nichols <david_nichols@users.sourceforge.net>
- updated descriptions
- changes to make spec file more release-agnostic (use of the dist tag in release)

* Thu Dec 7 2005 David Nichols <david_nichols@users.sourceforge.net>
- Initial rpm build.

