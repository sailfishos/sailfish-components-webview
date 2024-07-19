Name:    sailfish-components-webview-qt5
Summary: Allows embedding Sailfish WebView into applications
Version: 1.4.2
Release: 1
License: MPLv2.0
Url:     https://github.com/sailfishos/sailfish-components-webview
Source0: %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(qt5embedwidget) >= 1.52.20
BuildRequires:  pkgconfig(sailfishsilica)
BuildRequires:  qt5-qttools
BuildRequires:  qt5-qttools-linguist
Requires: sailfishsilica-qt5 >= 1.1.123
Requires: sailfish-components-media-qt5
Requires: sailfish-components-pickers-qt5
Requires: embedlite-components-qt5 >= 1.21.2
Requires: libqofono-qt5-declarative >= 0.115

%description
%{summary}.

%package ts-devel
Summary:   Translation source for sailfish-components-webview-qt5
Requires:  %{name} = %{version}

%description ts-devel
%{summary}.

%package popups
Summary:   Popup and alert QML components used by sailfish-components-webview
Requires:  %{name} = %{version}
Requires:  declarative-transferengine-qt5 >= 0.3.1
Requires:  nemo-qml-plugin-systemsettings

%description popups
%{summary}.

%package pickers
Summary:   Picker and selector QML components used by sailfish-components-webview
Requires:  %{name} = %{version}

%description pickers
%{summary}.

%package devel
Summary:    Sailfish WebEngine development files
Requires:   %{name} = %{version}-%{release}

%description devel
Development package for libsailfishwebengine.

%package tests
Summary:    Sailfish WebView tests
BuildRequires:  pkgconfig(Qt5Test)
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtdeclarative-import-qttest
Requires:   nemo-test-tools

%description tests
Unit tests and functional tests of Sailfish WebView.

%package examples
Summary:    Sailfish WebView examples
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-popups = %{version}-%{release}

%description examples
Examples using Sailfish WebView.

%package doc
Summary:    Documentation for Sailfish WebView
BuildRequires:  sailfish-qdoc-template
BuildRequires:  qt5-qttools-qthelp-devel
BuildRequires:  qt5-tools

%description doc
%{summary}.

%prep
%autosetup -n %{name}-%{version}

%build
%qmake5 VERSION=%{version}
%make_build

%install
%qmake5_install

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%license LICENSE.txt
%{_libdir}/libsailfishwebengine.so.*
%{_datadir}/translations/sailfish_components_webview_qt5_eng_en.qm
%{_datadir}/translations/sailfish_components_webview_controls_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebEngine/libsailfishwebengineplugin.so
%{_libdir}/qt5/qml/Sailfish/WebEngine/qmldir
%{_libdir}/qt5/qml/Sailfish/WebEngine/plugins.qmltypes
%{_libdir}/qt5/qml/Sailfish/WebView/libsailfishwebviewplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/plugins.qmltypes
%{_libdir}/qt5/qml/Sailfish/WebView/*.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Controls/libsailfishwebviewcontrolsplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Controls/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Controls/plugins.qmltypes
%{_libdir}/qt5/qml/Sailfish/WebView/Controls/*.qml

%files ts-devel
%{_datadir}/translations/source/sailfish_components_webview_qt5.ts
%{_datadir}/translations/source/sailfish_components_webview_controls_qt5.ts
%{_datadir}/translations/source/sailfish_components_webview_popups_qt5.ts
%{_datadir}/translations/source/sailfish_components_webview_pickers_qt5.ts

%files popups
%{_datadir}/translations/sailfish_components_webview_popups_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/libsailfishwebviewpopupsplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/plugins.qmltypes
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/*.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/*.js

%files pickers
%{_datadir}/translations/sailfish_components_webview_pickers_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/libsailfishwebviewpickersplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/plugins.qmltypes
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/*.qml

%files devel
%{_libdir}/libsailfishwebengine.so
%{_libdir}/pkgconfig/sailfishwebengine.pc
%{_includedir}/libsailfishwebengine/*

%files tests
/opt/tests/sailfish-components-webview/*

%files examples
%{_bindir}/custompopups
%{_bindir}/webview-test-app

%files doc
%dir %{_datadir}/doc/sailfish-components-webview
%{_datadir}/doc/sailfish-components-webview/*
