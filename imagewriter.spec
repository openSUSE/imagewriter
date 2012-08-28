#
# Copyright (c) 2008 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# needsrootforbuild
Url:            http://kiwi.berlios.de
Name:           imagewriter
BuildRequires:  hal-devel
BuildRequires:  gcc-c++

%if 0%{?fedora_version}  
    %define breq qt4-devel  
    %define qmake /usr/bin/qmake-qt4  
    %define lrelease /usr/bin/lrelease-qt4  
%endif    
%if 0%{?mandriva_version}  
    %define breq libqt4-devel
    %define qmake /usr/lib/qt4/bin/qmake  
    %define lrelease /usr/lib/qt4/bin/lrelease  
%endif  
%if 0%{?suse_version}  
    %define breq libqt4-devel  
    %define qmake /usr/bin/qmake  
    %define lrelease /usr/bin/lrelease  
%endif

Summary:        SUSE Studio Imagewriter
Version:        1.8
Release:        0
Group:          Hardware/Other
License:        GPL v2
Source:         imagewriter-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  %{breq}
%description
Graphical image writer application

%prep
%setup 

%build
%{qmake} PREFIX=%{_prefix} -makefile imagewriter.pro
make buildroot=$RPM_BUILD_ROOT CFLAGS="$RPM_OPT_FLAGS"

%install
# build
#install -d $RPM_BUILD_ROOT/usr/bin
#install -m 755 -p imagewriter $RPM_BUILD_ROOT/usr/bin
make install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/imagewriter
%{_prefix}/share/applications/imagewriter.desktop
%{_prefix}/share/icons/hicolor/*/apps/imagewriter.png 
%doc COPYING

%changelog
