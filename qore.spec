%define module_api 0.5
%define module_dir %{_libdir}/qore-modules

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
Version: 0.7.2
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
BuildRequires: bison
BuildRequires: openssl-devel
BuildRequires: pcre-devel
BuildRequires: libxml2-devel
BuildRequires: zlib-devel
%if 0%{?suse_version}
BuildRequires: libbz2-devel
%else
BuildRequires: bzip2-devel
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

%package -n libqore5
Summary: The libraries for qore runtime and qore clients
Group: Development/Languages
Provides: qore-module-api-%{module_api}

%description -n libqore5
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

%files -n libqore5
%defattr(-,root,root,-)
%{_libdir}/libqore.so.5.1.0
%{_libdir}/libqore.so.5
%doc COPYING.LGPL COPYING.GPL README README-LICENSE README-MODULES RELEASE-NOTES CHANGELOG AUTHORS WHATISQORE

%post -n libqore5
ldconfig %{_libdir}

%postun -n libqore5
ldconfig %{_libdir}

%package doc
Summary: API documentation, programming language reference, and Qore example programs
Group: Development/Languages
BuildArchitectures: noarch

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
%if 0%{?suse_version}
Requires: libqore5 = %{version}-%{release}
%else
Requires: %{name}-libs = %{version}-%{release}
%endif

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
%{_libdir}/libqore.la
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
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/%{module_dir}
mkdir -p $RPM_BUILD_ROOT/%{module_dir}/auto
mkdir -p $RPM_BUILD_ROOT/usr/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/examples
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/qore/test
make install prefix=$RPM_BUILD_ROOT/usr
rm $RPM_BUILD_ROOT/%{_libdir}/libqore.la

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
/usr/bin/qore
/usr/share/man/man1/qore.1.gz

%changelog
* Web Nov 26 2008 David Nichols <david_nichols@users.sourceforge.net>
- made libqore* the default name for lib package, removed la file

* Sun Nov 23 2008 David Nichols <david_nichols@users.sourceforge.net>
- updated to 0.7.2

* Tue Oct 7 2008 David Nichols <david_nichols@users.sourceforge.net>
- released 0.7.0

* Thu Sep 4 2008 David Nichols <david_nichols@users.sourceforge.net>
- removed all modules as they are now independent projects

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

