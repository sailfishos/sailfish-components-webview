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

%files
%defattr(-,root,root,-)
%{_datadir}/translations/sailfish_components_webview_qt5_eng_en.qm
%{_libdir}/qt5/qml/Sailfish/WebView/libsailfishwebviewplugin.so
%{_libdir}/qt5/qml/Sailfish/WebView/qmldir
%{_libdir}/qt5/qml/Sailfish/WebView/WebView.qml
%{_libdir}/qt5/qml/Sailfish/WebView/UserPrompt.qml
%{_libdir}/qt5/qml/Sailfish/WebView/AlertDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/ConfirmDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MultiSelectDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/SingleSelectPage.qml
%{_libdir}/qt5/qml/Sailfish/WebView/PromptDialog.qml
%{_libdir}/qt5/qml/Sailfish/WebView/PromptLabel.qml
%{_libdir}/qt5/qml/Sailfish/WebView/PickerCreator.qml
%{_libdir}/qt5/qml/Sailfish/WebView/ContentPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/ImagePicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MusicPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/VideoPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MultiContentPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MultiImagePicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MultiMusicPicker.qml
%{_libdir}/qt5/qml/Sailfish/WebView/MultiVideoPicker.qml

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_webview_qt5.ts

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
