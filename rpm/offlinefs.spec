#
# spec file for package offlinefs
#
# Copyright (c) 2010, 2011 Ohm University


Name:           offlinefs
BuildRequires:  automake libtool autoconf
BuildRequires:  fuse-devel pkgconfig gcc-c++ libattr-devel
# %if not suse something
# BuildRequires: dbus-devel
# %endif
# %if suse something
# BuildRequires: dbus-1-devel
# %endif
BuildRequires:  libconfuse-devel dbus-1-devel
Requires:       attr
Version:        0.3
Release:        1.1
Group:          System/Filesystems
License:        GPL
Url:            http://sourceforge.net/projects/offlinefs/files/offlinefs/%{version}/offlinefs-%{version}.tar.bz2/download
Summary:        Offline (Ohm) File System
Source:         %{name}-%{version}.tar.bz2
# PATCH-FIX-UPSTREAM offlinefs-0.3-NONVOID-FIX.patch Fixes returning random data.
#Patch:          offlinefs-0.3-NONVOID-FIX.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description

OFS (Offline File System or Ohm File System) is an offline-layer to extend
every filesystem with offline capabilities. This means the user can still work
on his network documents, even when disconnected from the server. When network
connection is restored, the offline file system automatically synchronizes the
files.

Authors:
--------
Tobias Jaehnel
Carsten Kolassa
Frank Gsellmann
Frank Jurak
Peter Trommler

%package        devel
Summary:        C and C++ Development files for Offline File System
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version} 

%description    devel
OFS (Offline File System or Ohm File System) is an offline-layer to extend
every filesystem with offline capabilities. This means the user can still work
on his network documents, even when disconnected from the server. When network
connection is restored, the offline file system automatically synchronizes the
files.

Authors:
--------
Tobias Jaehnel
Carsten Kolassa
Frank Gsellmann
Frank Jurak
Peter Trommler



%prep
%setup -q
#%patch -p1
#%patch -P 1 -p1
#%patch -P 2 -p1

%build
ACLOCAL="aclocal -I m4" aclocal
#aclocal
libtoolize --force
autoheader
automake
autoconf
%configure --docdir=%{_docdir}/%{name}
#make %{?jobs:-j%jobs}
make

%install
make DESTDIR=%{buildroot} install
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)
%doc %{_docdir}/%{name}
%{_libdir}/*.so.*
%{_mandir}/man8/mount.ofs.8.gz
/usr/sbin/ofs
/sbin/mount.ofs

%files devel
%defattr(-,root,root)
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la


%changelog
* Fri Apr 8   2011 peter.trommler@ohm-hochschule.de
- upgraded to 0.2.3 from upstream
* Mon Mar 21  2011 peter.trommler@ohm-hochschule.de
- upgraded to 0.2.2 from upstream
* Mon Feb 28  2011 peter.trommler@ohm-hochschule.de
- upgraded to 0.2.1 from upstream
* Wed Feb 16  2011 peter.trommler@ohm-hochschule.de
- upgraded to svn 261 from upstream
* Tue Jan 11  2011 peter.trommler@ohm-hochschule.de
- created package
