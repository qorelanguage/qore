%define with_ncurses 1
%define with_mysql   1
%define with_pgsql   1
%define with_mssql   1
%define with_oracle  0
%define with_sybase  0
%define with_tibae   0
%define with_tibrv   0
%define with_tuxedo  0

Summary: Qore Programming Language
Name: qore
Version: 0.6.2.6
Release: 1%{dist}
License: LGPL
Group: Development/Languages
URL: http://www.qoretechnologies.com/qore
Source: http://prdownloads.sourceforge.net/qore/qore-%{version}-src.tar.gz
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: /usr/bin/env
BuildRequires: gcc-c++
BuildRequires: flex >= 2.5.31
BuildRequires: openssl-devel
BuildRequires: pcre-devel
BuildRequires: libxml2-devel
%if 0%{?with_ncurses}
BuildRequires: ncurses-devel
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
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.


%if 0%{?suse_version}
%debug_package
%endif

%post
ldconfig %{_libdir}

%postun
ldconfig %{_libdir}

%if 0%{?with_ncurses}
%package ncurses-module
Summary: ncurses module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

%description ncurses-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

ncurses module for the Qore Programming Language.  The ncurses module allows 
Qore programs to implement complex character-based applications.  Note that the
use of the ncurses module with Qore threading is still experimental.


%files ncurses-module
%defattr(-,root,root,-)
%{_libdir}/qore-%{version}/ncurses.qmod
%endif

%if 0%{?with_oracle}
%package oracle-module
Summary: Oracle DBI module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

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
%{_libdir}/qore-%{version}/oracle.qmod
%endif

%if 0%{?with_mysql}
%package mysql-module
Summary: MySQL DBI module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}
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
%{_libdir}/qore-%{version}/mysql.qmod
%endif

%if 0%{?with_pgsql}
%package pgsql-module
Summary: PostgreSQL DBI module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}
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
%{_libdir}/qore-%{version}/pgsql.qmod
%endif

%if 0%{?with_sybase}
%package sybase-module
Summary: Sybase DBI module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

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
%{_libdir}/qore-%{version}/sybase.qmod
%endif

%if 0%{?with_mssql}
%package mssql-module
Summary: FreeTDS-based MS-SQL and Sybase DBI module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}
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
%{_libdir}/qore-%{version}/mssql.qmod
%endif

%if 0%{?with_tibae}
%ifarch i386 sparc
%package tibae-module
Summary: TIBCO Adapters integration module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

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
%{_libdir}/qore-%{version}/tibae.qmod
%endif
%endif

%if 0%{?with_tibrv}
%package tibrv-module
Summary: TIBCO Rendezvous integration module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

%description tibrv-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides functionality enabling qore scripts/programs to communicate
using TIBCO Rendezvous(R) publish-subscribe messaging (reliable and certified
protocols), join and monitor fault-tolerant groups, join distributed queues, 
etc.


%files tibrv-module
%defattr(-,root,root,-)
%{_libdir}/qore-%{version}/tibrv.qmod
%endif

%if 0%{?with_tuxedo}
%package tuxedo-module
Summary: BEA Tuxedo(R) client API integration module for Qore
Group: Development/Languages
Requires: %{name} = %{version}-%{release}

%description tuxedo-module
Qore is a modular, multithreaded, weakly-typed, object-oriented programming
language suitable for embedding application logic, application scripting,
interface development, and even complex multi-threaded, network-aware object-
oriented application development. Qore features integrated XML and JSON 
support (as well as HTTP, XML-RPC, and JSON-RPC client classes), database
integration, database-independent programming support, exception-handling and 
exception-safe programming support, TIBCO and Tuxedo modules, as well as built-
in date arithmetic, character encoding (including proper UTF-8) support, and
much more.

This module provides functionality enabling qore scripts/programs to communicate
using the BEA Tuxedo(R) client API.


%files tuxedo-module
%defattr(-,root,root,-)
%{_libdir}/qore-%{version}/tuxedo.qmod
%endif

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
mkdir -p $RPM_BUILD_ROOT/%{_libdir}/qore-%{version}
mkdir -p $RPM_BUILD_ROOT/%{_libdir}/qore-%{version}/auto
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
%{_libdir}/libqore.so.3.0.4
%{_libdir}/libqore.so.3
%{_libdir}/libqore.so
%{_libdir}/libqore.la
/usr/share/man/man1/qore.1.gz

%changelog
* Thu Feb 21 2008 David Nichols <david_nichols@users.sourceforge.net>
- updated version to 0.6.2.4 and library version to 3.0.4

* Sun Oct 28 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated version to 0.6.2.3 and library version to 3.0.3

* Sun Oct 21 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated version to 0.6.2.2 and library version to 3.0.2

* Sat Oct 13 2007 David Nichols <david_nichols@users.sourceforge.net>
- made ncurses a separate module as per the original opensuse rpm

* Tue Jul 17 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated version to 0.6.2.1
- updated library version to 3.0.1

* Sat Jul 14 2007 David Nichols <david_nichols@users.sourceforge.net>
- copied improvements from opensuse rpm and updated based on rpmlint output

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

