/****************************************************************************
**
** Copyright (c) 2021 Open Mobile Platform LLC
**
****************************************************************************/

import QtQuick 2.6
import Sailfish.Silica 1.0
import Sailfish.WebView 1.0

ApplicationWindow {
    id: root

    initialPage: Component {
        Page {
            id: page

            anchors.fill: parent

            function startTest() {
                var props = {
                    "mode": mode.currentIndex,
                    "webPageAddress": webPageAddress.text,
                    "creationDelay": creationDelay.value,
                    "destructionDelay": destructionDelay.value,
                    "reloadInterval": reloadInterval.value,
                    "viewportInfo": viewportInfoSwitch.checked
                }

                pageStack.push(webViewPageComponent, props)
            }

            Column {
                id: contentColumn

                width: parent.width
                spacing: Theme.paddingMedium
                bottomPadding: Theme.paddingLarge

                PageHeader {
                    title: "WebView test app"
                }

                TextField {
                    id: webPageAddress

                    text: "https://en.wikipedia.org/"
                    placeholderText: "Web page address"
                    label: placeholderText
                    inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly

                    EnterKey.onClicked: {
                        if (!text) {
                            return
                        }

                        page.startTest()
                    }

                    onFocusChanged: {
                        if (focus) {
                            webPageAddress.selectAll()
                        }
                    }
                }

                ComboBox {
                    id: mode

                    label: "Mode"
                    menu: ContextMenu {
                        MenuItem { text: "Open webpage" }
                        MenuItem { text: "Webview recreation" }
                        MenuItem { text: "Webpage reload" }
                        MenuItem { text: "Webpage scrollable rect" }
                    }
                }

                SpinBox {
                    id: creationDelay

                    width: parent.width
                    visible: mode.currentIndex === 1
                    minimumValue: 0.1
                    maximumValue: 600.0
                    value: 1.0
                    label: "Creation delay [s]"
                    stepSize: 0.1
                }

                SpinBox {
                    id: destructionDelay

                    width: parent.width
                    visible: mode.currentIndex === 1
                    minimumValue: 0.1
                    maximumValue: 600.0
                    value: 5.0
                    label: "Destruction delay[s]"
                    stepSize: 0.1
                }

                SpinBox {
                    id: reloadInterval

                    width: parent.width
                    visible: mode.currentIndex === 2
                    minimumValue: 0.0
                    maximumValue: 600.0
                    value: 5.0
                    label: "Reload interval [s]"
                    stepSize: 0.1
                }

                TextSwitch {
                    id: viewportInfoSwitch
                    text: "Viewport info"
                }

                Item {
                    width: parent.width
                    height: Theme.paddingLarge
                }

                Button {
                    text: mode.currentIndex === 0 ? "Open webpage" : "Start"
                    anchors.horizontalCenter: parent.horizontalCenter

                    onClicked: {
                        page.startTest()
                    }
                }
            }
        }
    }

    Component {
        id: webViewPageComponent

        Page {
            id: webViewPage

            property int mode
            property string webPageAddress
            property real creationDelay
            property real destructionDelay
            property real reloadInterval
            property bool viewportInfo

            property bool webViewLoading: false
            property int webViewLoadProgress: 0

            anchors.fill: parent

            showNavigationIndicator: false
            backNavigation: false

            Component.onCompleted: {
                webViewLoader.active = true
            }

            Timer {
                id: webViewRecreationTimer

                interval: (webViewLoader.active ?
                               destructionDelay :
                               creationDelay) * 1000.0

                running: pageStack.currentPage === webViewPage && mode === 1
                repeat: true

                onTriggered: {
                    webViewLoader.active = !webViewLoader.active
                    loadStatusShowTimer.start()
                }
            }

            Component {
                id: webViewComponent

                WebView {
                    id: webView

                    anchors.fill: parent
                    active: true

                    Timer {
                        id: webViewReloadTimer

                        running: pageStack.currentPage === webViewPage && (mode === 2 || mode === 3)
                        interval: reloadInterval * 1000.0
                        repeat: true

                        onTriggered: {
                            console.log("webview reload")
                            webView.reload()
                        }
                    }

                    onLoadingChanged: {
                        webViewPage.webViewLoading = loading
                        webViewPage.webViewLoadProgress = 0
                        loadStatusShowTimer.start()
                    }

                    onLoadProgressChanged: {
                        webViewPage.webViewLoadProgress = loadProgress
                    }
                }
            }

            Loader {
                id: webViewLoader

                onActiveChanged: {
                    if (active) {
                        console.log("webview creation")
                    } else {
                        console.log("webview destruction")
                    }
                }

                anchors.fill: parent
                active: false
                sourceComponent: webViewComponent

                onItemChanged: {
                    if (item) {
                        console.log("page loading: " + webPageAddress)
                        item.load(webPageAddress)
                    }
                }
            }

            Rectangle {
                visible: viewportInfo
                color: "grey"
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: 22
                }

                height: contentColumn.height + Theme.paddingMedium * 2
                radius: 5

                Column {
                    id: contentColumn
                    y: Theme.paddingMedium
                    x: Theme.paddingMedium
                    width: parent.width - Theme.paddingMedium * 2

                    Label {
                        text: "Scrollable offset x: " + webViewLoader.item.scrollableOffset.x.toFixed(2) + ", y: " + webViewLoader.item.scrollableOffset.y.toFixed(2)
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "Scrollable width: " + webViewLoader.item.scrollableSize.width.toFixed(2) + ", height: " + webViewLoader.item.scrollableSize.height.toFixed(2)
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "Content width: " + webViewLoader.item.contentWidth.toFixed(2) + ", height: " + webViewLoader.item.contentHeight.toFixed(2)
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "Content rect x: " + webViewLoader.item.contentRect.x.toFixed(2) + ", y: " + webViewLoader.item.contentRect.y.toFixed(2)
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "Content rect width: " + webViewLoader.item.contentRect.width.toFixed(2) + ", height: " + webViewLoader.item.contentRect.height.toFixed(2)
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "atXBeginning: " + webViewLoader.item.atXBeginning
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "atYBeginning: " + webViewLoader.item.atYBeginning
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "atXEnd: " + webViewLoader.item.atXEnd
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "atYEnd: " + webViewLoader.item.atYEnd
                        font.pixelSize: Theme.fontSizeSmall
                    }

                    Label {
                        text: "resolution: " + webViewLoader.item.resolution
                        font.pixelSize: Theme.fontSizeSmall
                    }
                }
            }

            Rectangle {
                color: "grey"
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    margins: 22
                }
                width: 180
                height: 55
                radius: 5

                opacity: (webViewPage.webViewLoading || loadStatusShowTimer.running) ? 0.5 : 0.0
                Behavior on opacity { FadeAnimator {} }

                Timer {
                    id: loadStatusShowTimer
                }

                Label {
                    anchors.centerIn: parent
                    text: !webViewLoader.active ? "destroyed" : (webViewPage.webViewLoading ? webViewPage.webViewLoadProgress + "%" : "loaded")
                }
            }

            IconButton {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                opacity: 0.5
                icon.color: "gray"
                icon.highlightColor: "red"
                icon.highlighted: down
                icon.source: "image://theme/icon-m-close"

                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }
}
