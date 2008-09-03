%define module_api 0.4
%define module_dir %{_libdir}/qore-module-api-%{module_api}

%define with_mysql      1
%define with_pgsql      1
%define with_mssql      1
%define with_oracle     1
%define with_sybase     1
%define with_tibae      0

%if 0%{?sles_version}

%if 0%{?sles_version} == 10
%define dist .sle10
%endif

%if 0%{?sles_version} == 9
%define dist .sle9
%endif

%else
%if 0%{?suse_version}

%if 0%{?suse_version} == 1100
%define dist .opensuse11
%endif

%if 0%{?suse_version} == 1030
%define dist .opensuse10.3
%endif

%if 0%{?suse_version} == 1020
%define dist .opensuse10.2
%endif

%if 0%{?suse_version} == 1010
%define dist .suse10.1
%endif

%if 0%{?suse_version} == 1000
%define dist .suse10
%endif

%if 0%{?suse_version} == 930
%define dist .suse9.3
%endif

%endif

%endif

Summary: Qore Programming Language
Name: qore
Version: 0.7.0
Release: 1%{dist}
License: LGPL or GPL
Group: Development/Languages
URL: http://www.qoretechnologies.com/qore
Source: http://prdownloads.sourceforge.net/qore/qore-%{version}.tar.gz
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: /usr/bin/env
BuildRequires: gcc-c++
BuildRequires: flex >= 2.5.31
BuildRequires: openssl-devel
BuildRequires: pcre-devel
BuildRequires: libxml2-devel
BuildRequires: zlib-devel
%if 0%{?suse_version}
BuildRequires: libbz2-devel
%else
BuildRequires: bzip2-devel
%endif
%if 0%{?with_mysql}
BuildRequires: mysql-devel
%endif
%if 0%{?with_pgsql}
BuildRequires: postgresql-devel
%endif
%if 0%{?with_mssql}
BuildRequires: freetds-devel
%endif


%description
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, as well as built-in date arithmetic,
character encoding (including proper UTF-8) support, and much more.


%if 0%{?suse_version}
%debug_package
%endif

%post
ldconfig %{_libdir}

%postun
ldconfig %{_libdir}

%if 0%{?with_oracle}
%package oracle-module
Summary: Oracle DBI module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}

%description oracle-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

Oracle DBI driver module for the Qore Programming Language. The Oracle driver is
character set aware, supports multithreading, transaction management, stored
prodedure and function execution, etc.


%files oracle-module
%defattr(-,root,root,-)
%{module_dir}/oracle.qmod
%endif

%if 0%{?with_mysql}
%package mysql-module
Summary: MySQL DBI module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}
Requires: mysql-libs

%description mysql-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

MySQL DBI driver module for the Qore Programming Language. The MySQL driver is
character set aware and supports multithreading, transaction management, and
stored procedure execution.


%files mysql-module
%defattr(-,root,root,-)
%{module_dir}/mysql.qmod
%endif

%if 0%{?with_pgsql}
%package pgsql-module
Summary: PostgreSQL DBI module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}
Requires: postgresql-libs

%description pgsql-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

PostgreSQL DBI driver module for the Qore Programming Language. The PostgreSQL
driver is character set aware, supports multithreading, transaction management,
stored prodedure and function execution, etc.


%files pgsql-module
%defattr(-,root,root,-)
%{module_dir}/pgsql.qmod
%endif

%if 0%{?with_sybase}
%package sybase-module
Summary: Sybase DBI module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}

%description sybase-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

Sybase DBI driver module for the Qore Programming Language. The Sybase driver is
character set aware, supports multithreading, transaction management, stored
prodedure and function execution, etc.


%files sybase-module
%defattr(-,root,root,-)
%{module_dir}/sybase.qmod
%endif

%if 0%{?with_mssql}
%package mssql-module
Summary: FreeTDS-based MS-SQL and Sybase DBI module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}
Requires: freetds

%description mssql-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

FreeTDS-based MS-SQL Server and Sybase DBI driver module for the Qore
Programming Language. This driver is character set aware, supports
multithreading, transaction management, stored prodedure and function
execution, etc, and can be used to connect to Sybase and Microsoft SQL Server
databases.


%files mssql-module
%defattr(-,root,root,-)
%{module_dir}/mssql.qmod
%endif

%if 0%{?with_tibae}
%ifarch i386 sparc
%package tibae-module
Summary: TIBCO Adapters integration module for Qore
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}

%description tibae-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides the TibcoAdapter class, which enables qore scripts/programs
to communicate with (or implement) TIBCO Adapters.


%files tibae-module
%defattr(-,root,root,-)
%{module_dir}/tibae.qmod
%endif
%endif

%package libs
Summary: The libraries for qore runtime and qore clients
Group: Development/Languages
Provides: qore-module-api-%{module_api}

%description libs
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides the qore library required for all clients using qore
functionality.

%files libs
%defattr(-,root,root,-)
%{_libdir}/libqore.so.4.0.0
%{_libdir}/libqore.so.4
%{_libdir}/libqore.la
%doc COPYING.LGPL COPYING.GPL README README-LICENSE RELEASE-NOTES CHANGELOG AUTHORS WHATISQORE

%package doc
Summary: API documentation, programming language reference, and Qore example programs
Group: Development/Languages

%description doc
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides API documentation, programming language reference, and
example programs


%files doc
%defattr(-,root,root,-)
%doc docs/library docs/qore-style.css docs/img docs/qore.html examples/ test/ 

%package devel
Summary: The header files needed to compile programs using the qore library
Group: Development/Languages
Requires: %{name}-libs = %{version}-%{release}

%description devel
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides header files needed to compile client programs using the
Qore library.

%files devel
%defattr(-,root,root,-)
%{_libdir}/libqore.so
%{_prefix}/include/*


%prep
%setup -q
%ifarch x86_64 ppc64 x390x
c64=--enable-64bit
%endif
# need to configure with /usr as prefix as this will be used to derive the module directory
./configure RPM_OPT_FLAGS="$RPM_OPT_FLAGS" --prefix=/usr --disable-debug --disable-static $c64

%build
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/%{module_dir}
mkdir -p $RPM_BUILD_ROOT/%{module_dir}/auto
mkdir -p $RPM_BUILD_ROOT/usr/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/examples
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/test
make install prefix=$RPM_BUILD_ROOT/usr

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/qore
/usr/share/man/man1/qore.1.gz

%changelog
* Tue Sep 2 2008 David Nichols <david_nichols@users.sourceforge.net>
- fixed dist tag for suse distributions
- updated for new module directory, added qore-module-api-* capability

* Thu Jun 12 2008 David Nichols <david_nichols@users.sourceforge.net>
- added new modules

* Tue Oct 22 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated spec file with corrections from suse open build service

* Tue Jul 17 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated library version to 3.1.0

* Sat Jul 14 2007 David Nichols <david_nichols@users.sourceforge.net>
- copied improvements from opensuse rpm and updated based on rpmlint output
- updated version to 0.7.0

* Thu Jun 14 2007 David Nichols <david_nichols@users.sourceforge.net>
- fixed spec file to support more architectures

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
- Initial rpm build

