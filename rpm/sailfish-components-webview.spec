Name: sailfish-components-webview-qt5
License: MPLv2
Version: 0.0.14
Release: 1
Source0: %{name}-%{version}.tar.bz2
Summary: Allows embedding Sailfish OS Browser WebView into applications
Group:   System/Libraries
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(qt5embedwidget)
BuildRequires:  pkgconfig(sailfishsilica)
BuildRequires:  qt5-qttools
BuildRequires:  qt5-qttools-linguist
Requires: sailfishsilica-qt5
Requires: sailfish-components-media-qt5
Requires: sailfish-components-pickers-qt5

%description
%{summary}.

%package ts-devel
Summary:   Translation source for sailfish-components-webview-qt5
Group:     System/Libraries
Requires:  %{name} = %{version}

%description ts-devel
%{summary}.

%package popups
Summary:   Popup and alert QML components used by sailfish-components-webview
Group:     System/Libraries
Requires:  %{name} = %{version}
Requires:  declarative-transferengine-qt5 >= 0.2.3

%description popups
%{summary}.

%package popups-ts-devel
Summary:   Translation source for sailfish-components-webview-popups-qt5
Group:     System/Libraries
Requires:  %{name} = %{version}

%description popups-ts-devel
%{summary}.

%package pickers
Summary:   Picker and selector QML components used by sailfish-components-webview
Group:     System/Libraries
Requires:  %{name} = %{version}

%description pickers
%{summary}.

%package pickers-ts-devel
Summary:   Translation source for sailfish-components-webview-pickers-qt5
Group:     System/Libraries
Requires:  %{name} = %{version}

%description pickers-ts-devel
%{summary}.

%package devel
Summary:    Sailfish OS WebEngine development files
Requires:   %{name} = %{version}-%{release}

%description devel
Development package which provides libsailfishwebengine

%package tests
Summary:    Sailfish OS WebView / WebEngine tests
BuildRequires:  pkgconfig(Qt5Test)
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtdeclarative-import-qttest
Requires:   nemo-test-tools

%description tests
Unit tests and functional tests of Sailfish OS WebView / WebEngine

%files
%defattr(-,root,root,-)
%{_libdir}/libsailfishwebengine.so.*
%{_datadir}/translations/sailfish_components_webview_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebEngine/libsailfishwebengineplugin.so
%{_libdir}/qt5/qml/Sailfish/WebEngine/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/libsailfishwebviewplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/*.qml

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_qt5.ts

%files popups
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_popups_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/libsailfishwebviewpopupsplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/*.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/*.js

%files pickers
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_pickers_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/libsailfishwebviewpickersplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/*.qml

%files popups-ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_popups_qt5.ts

%files pickers-ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_pickers_qt5.ts

%files devel
%defattr(-,root,root,-)
%{_libdir}/libsailfishwebengine.so
%{_libdir}/pkgconfig/sailfishwebengine.pc
%{_includedir}/libsailfishwebengine/*

%files tests
%defattr(-,root,root,-)
/opt/tests/sailfish-components-webview/*

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
make

%install
%qmake5_install

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%post popups
/sbin/ldconfig

%postun popups
/sbin/ldconfig

%post pickers
/sbin/ldconfig

%postun pickers
/sbin/ldconfig
