Summary: Qore Programming Language
Name: qore
Version: 0.5.2
Release: 1
License: LGPL
Group: Development/Languages
URL: http://qore.sourceforge.net
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
qore is a modular, multithreaded, weakly-typed, object-oriented programming language suitable for embedding application logic, application scripting, interface development, and even complex multi-threaded, network-aware object-oriented application development. Qore features integrated XML capability, oracle, mysql, & TIBCO Active Enterprise modules, as well as built-in date arithmetic, method and member redirection for classes, private methods, synchronized (in the Java sense) functions and class methods, national character set support with implicit and explicit character set conversions, exception handling, regular expression support, powerful and easy-to-use data structures (arrays, hashes, etc), and much more. Qore is under active development and is also under heavy commercial use as the technology behind the OM/Qore Workflow and EAI System.

%package oracle
Summary: Oracle module for Qore
Group: Development/Languages

%description oracle
Oracle DBI driver module for the Qore Programming Language. The Oracle driver is character set aware, supports multithreading, transaction management, stored prodedure and function execution, etc.

%files oracle
/usr/lib/qore-0.5.2/auto/oracle.qmod

%package mysql
Summary: MySQL module for Qore
Group: Development/Languages

%description mysql
MySQL DBI driver module for the Qore Programming Language. The MySQL driver is character set aware and supports multithreading and transaction management. Currently stored procedure execution is not supported.

%files mysql
/usr/lib/qore-0.5.2/auto/mysql.qmod

%package tibae
Summary: TIBCO Active Enterprise integration module for Qore
Group: Development/Languages

%description tibae
This module provides the TibcoAdapter class, which enables qore scripts/programs to communicate with (or implement) TIBCO Active Enterprise adapters.

%files tibae
/usr/lib/qore-0.5.2/tibrv.qmod

%package tibrv
Summary: TIBCO Rendezvous integration module for Qore
Group: Development/Languages

%description tibrv
This module provides class enabling qore scripts/programs to communicate using TIBCO Rendezvous publish-subscribe messaging.

%files tibrv
/usr/lib/qore-0.5.2/tibrv.qmod

%prep
%setup -q
CXX=g++32 ./configure RPM_OPT_FLAGS="$RPM_OPT_FLAGS" --prefix=/usr --disable-debug --disable-static

%build
make -j2

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/lib/qore
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
/usr/lib/libqore.so.0.0.0
/usr/lib/libqore.so.0
/usr/lib/libqore.so
/usr/lib/libqore.la
/usr/lib/qore-0.5.2/ncurses.qmod
/usr/man/man1/qore.1.gz

%changelog
* Thu Dec 7 2005 David Nichols <david_nichols@users.sourceforge.net> - 0.4.0-1
- Initial rpm build.

