Summary: Qore Programming Language
Name: qore
Version: 0.6.0
Release: 1%{?dist}
License: LGPL
Group: Development/Languages
URL: http://qore.sourceforge.net
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
qore is a modular, multithreaded, weakly-typed, object-oriented programming language suitable for embedding application logic, application scripting, interface development, and even complex multi-threaded, network-aware object-oriented application development. Qore features integrated XML capability, oracle, mysql, TIBCO Rendezvous and Adapters modules, as well as built-in date arithmetic, method and member redirection for classes, private methods, synchronized (in the Java sense) functions and class methods, national character set support with implicit and explicit character set conversions, exception handling, Perl5-compatible regular expression support, powerful and easy-to-use data structures (arrays, hashes, etc), and much more. Qore is under active development, very well-tested, and is also under commercial use as the technology behind the Qorus Integration Engine (formerly OM/Qore).

%package oracle-module
Summary: Oracle module for Qore
Group: Development/Languages

%description oracle-module
Oracle DBI driver module for the Qore Programming Language. The Oracle driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc.

%files oracle-module
%ifarch i386
/usr/lib/qore-0.6.0/auto/oracle.qmod
%endif
%ifarch x86_64
/usr/lib64/qore-0.6.0/auto/oracle.qmod
%endif

%package mysql-module
Summary: MySQL module for Qore
Group: Development/Languages

%description mysql-module
MySQL DBI driver module for the Qore Programming Language. The MySQL driver is character set aware and supports multithreading, transaction management, and stored procedure execution.

%files mysql-module
%ifarch i386
/usr/lib/qore-0.6.0/auto/mysql.qmod
%endif
%ifarch x86_64
/usr/lib64/qore-0.6.0/auto/mysql.qmod
%endif

%ifarch i386
%package tibae-module
Summary: TIBCO Adapters integration module for Qore
Group: Development/Languages

%description tibae-module
This module provides the TibcoAdapter class, which enables qore scripts/programs to communicate with (or implement) TIBCO Adapters.

%files tibae-module
/usr/lib/qore-0.6.0/tibae.qmod
%endif

%package tibrv-module
Summary: TIBCO Rendezvous integration module for Qore
Group: Development/Languages

%description tibrv-module
This module provides functionality enabling qore scripts/programs to communicate using TIBCO Rendezvous publish-subscribe messaging (reliable and certified protocols), join and monitor fault-tolerant groups, join distributed queues, etc.

%files tibrv-module
%ifarch i386
/usr/lib/qore-0.6.0/tibrv.qmod
%endif
%ifarch x86_64
/usr/lib64/qore-0.6.0/tibrv.qmod
%endif

%package tuxedo-module
Summary: BEA Tuxedo(R) client API integration module for Qore
Group: Development/Languages

%description tuxedo-module
This module provides functionality enabling qore scripts/programs to communicate using the BEA Tuxedo(R) client API.

%files tuxedo-module
%ifarch i386
/usr/lib/qore-0.6.0/tuxedo.qmod
%endif
%ifarch x86_64
/usr/lib64/qore-0.6.0/tuxedo.qmod
%endif

%prep
%setup -q
cxx=g++
%ifarch x86_64
c64=--enable-64bit
%endif
%ifarch i386
cxx=g++32
%endif

CXX=$cxx ./configure RPM_OPT_FLAGS="$RPM_OPT_FLAGS" --prefix=/usr --disable-debug --disable-static $c64

%build
make -j4

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
%ifarch i386
mkdir -p $RPM_BUILD_ROOT/usr/lib/qore-0.6.0
mkdir -p $RPM_BUILD_ROOT/usr/lib/qore-0.6.0/auto
%endif
%ifarch x86_64
mkdir -p $RPM_BUILD_ROOT/usr/lib64/qore-0.6.0
mkdir -p $RPM_BUILD_ROOT/usr/lib64/qore-0.6.0/auto
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
%ifarch i386
/usr/lib/libqore.so.1.0.0
/usr/lib/libqore.so.1
/usr/lib/libqore.so
/usr/lib/libqore.la
/usr/lib/qore-0.6.0/ncurses.qmod
%endif
%ifarch x86_64
/usr/lib64/libqore.so.1.0.0
/usr/lib64/libqore.so.1
/usr/lib64/libqore.so
/usr/lib64/libqore.la
/usr/lib64/qore-0.6.0/ncurses.qmod
%endif
/usr/man/man1/qore.1.gz

%changelog
* Tue Jan 30 2007 David Nichols <david_nichols@users.sourceforge.net>
- added tuxedo module

* Fri Jan 5 2007 David Nichols <david_nichols@users.sourceforge.net>
- updated libqore so version to 1.0.0

* Sat Nov 18 2006 David Nichols <david_nichols@users.sourceforge.net>
- updated descriptions
- changes to make spec file more release-agnostic (use of the dist tag in release)

* Thu Dec 7 2005 David Nichols <david_nichols@users.sourceforge.net>
- Initial rpm build.

