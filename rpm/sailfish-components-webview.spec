Name: sailfish-components-webview-qt5
License: Mozilla License
Version: 0.0.1
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

%files
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/libsailfishwebviewplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/WebView.qml

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_qt5.ts

%files popups
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_popups_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/libsailfishwebviewpopupsplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/UserPrompt.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/AlertDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/ConfirmDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/PromptDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Popups/PromptLabel.qml

%files pickers
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_pickers_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/libsailfishwebviewpickersplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MultiSelectDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/SingleSelectPage.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/PickerCreator.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/ContentPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/ImagePicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MusicPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/VideoPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MultiContentPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MultiImagePicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MultiMusicPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/Pickers/MultiVideoPicker.qml

%files popups-ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_popups_qt5.ts

%files pickers-ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_pickers_qt5.ts

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
