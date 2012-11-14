#
# Copyright (c) 2008 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# needsrootforbuild
Url:            https://github.com/mbarringer/imagewriter
Name:           imagewriter
BuildRequires:  gcc-c++

%if 0%{?rhel_version} == 600 
     %define dist el6
     %define breq qt-devel
     %define backend hal-devel
     %define qmake /usr/bin/qmake-qt4  
     %define lrelease /usr/bin/lrelease-qt4
     %define definedbackend USEHAL
%endif

%if 0%{?fedora}
    %define breq qt4-devel
    %define backend udisks2
    %define qmake /usr/bin/qmake-qt4  
    %define lrelease /usr/bin/lrelease-qt4
    %define definedbackend USEUDISKS2
%endif

%if 0%{?mandriva_version}
    %define breq libqt4-devel
    %define backend hal-devel
    %define qmake /usr/lib/qt4/bin/qmake  
    %define lrelease /usr/lib/qt4/bin/lrelease
    %define definedbackend USEHAL
%endif  

%if 0%{?suse_version}
    %define breq libqt4-devel update-desktop-files
    %define qmake /usr/bin/qmake  
    %define lrelease /usr/bin/lrelease
%endif

%if 0%{?suse_version} <= 1130
    %define backend hal-devel
    %define definedbackend USEHAL
%endif

%if 0%{?suse_version} == 1140 || 0%{?suse_version} == 1210
    %define backend udisks
    %define definedbackend USEUDISKS
%endif

%if 0%{?suse_version} >= 1220
    %define backend udisks2
    %define definedbackend USEUDISKS2
%endif

Summary:        SUSE Imagewriter
Version:        1.10
Release:        0
Group:          Hardware/Other
License:        GPL-2.0
Source:         imagewriter-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  %{breq} %{backend}
%description
A graphical utility for writing raw disk images & hybrid isos to USB keys

%prep
%setup 

%build
%{qmake} PREFIX=$RPM_BUILD_ROOT/%{_prefix} DEFINES=%{definedbackend} imagewriter.pro
make buildroot=$RPM_BUILD_ROOT CFLAGS="$RPM_OPT_FLAGS -DKIOSKHACK"

%install
# build
# I don't know why 'make install' ignores the binary when built in OBS
install -d $RPM_BUILD_ROOT/usr/bin
install -m 755 -p imagewriter $RPM_BUILD_ROOT/%{_bindir}
make install
%if 0%{?suse_version}
    %suse_update_desktop_file imagewriter
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{_bindir}/imagewriter
%{_prefix}/share/applications/imagewriter.desktop
#%{_prefix}/share/icons/hicolor/*/apps/imagewriter.png
%dir %{_datadir}/icons/hicolor/*/apps/
%dir %{_datadir}/icons/hicolor/*/
%dir %{_datadir}/icons/hicolor/
%{_datadir}/icons/hicolor/*/apps/imagewriter.*
%doc COPYING

%changelog
