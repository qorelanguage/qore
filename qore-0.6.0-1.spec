Summary: Qore Programming Language
Name: qore
Version: 0.6.0
Release: 1.FC5
License: LGPL
Group: Development/Languages
URL: http://qore.sourceforge.net
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
qore is a modular, multithreaded, weakly-typed, object-oriented programming language suitable for embedding application logic, application scripting, interface development, and even complex multi-threaded, network-aware object-oriented application development. Qore features integrated XML capability, oracle, mysql, and TIBCO Rendezvous and Active Enterprise modules, as well as built-in date arithmetic, method and member redirection for classes, private methods, synchronized (in the Java sense) functions and class methods, national character set support with implicit and explicit character set conversions, exception handling, Perl5-compatible regular expression support, powerful and easy-to-use data structures (arrays, hashes, etc), and much more. Qore is under active development and is also under heavy commercial use as the technology behind the Qorus Integration Engine (formerly OM/Qore).

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
MySQL DBI driver module for the Qore Programming Language. The MySQL driver is character set aware and supports multithreading and transaction management. Currently stored procedure execution is not supported.

%files mysql-module
%ifarch i386
/usr/lib/qore-0.6.0/auto/mysql.qmod
%endif
%ifarch x86_64
/usr/lib64/qore-0.6.0/auto/mysql.qmod
%endif

%ifarch i386
%package tibae-module
Summary: TIBCO Active Enterprise integration module for Qore
Group: Development/Languages

%description tibae-module
This module provides the TibcoAdapter class, which enables qore scripts/programs to communicate with (or implement) TIBCO Active Enterprise adapters.

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
/usr/lib/libqore.so.0.0.0
/usr/lib/libqore.so.0
/usr/lib/libqore.so
/usr/lib/libqore.la
/usr/lib/qore-0.6.0/ncurses.qmod
%endif
%ifarch x86_64
/usr/lib64/libqore.so.0.0.0
/usr/lib64/libqore.so.0
/usr/lib64/libqore.so
/usr/lib64/libqore.la
/usr/lib64/qore-0.6.0/ncurses.qmod
%endif
/usr/man/man1/qore.1.gz

%changelog
* Thu Dec 7 2005 David Nichols <david_nichols@users.sourceforge.net> - 0.4.0-1
- Initial rpm build.

