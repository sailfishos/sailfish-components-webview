import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Silica.private 1.0 as SilicaPrivate
import Sailfish.WebView 1.0

Item {
    property alias webView: webView

    property alias header: headerLoader.sourceComponent
    property Item headerItem: headerLoader.item

    property alias footer: footerLoader.sourceComponent
    property Item footerItem: footerLoader.item

    default property alias _data: flickable.data

    WebView {
        id: webView
        width: parent.width
        height: parent.height
        flickable: flickable
        bottomMargin: footerItem && footerItem.height || 0
        topMargin: headerItem && headerItem.height || 0
        clip: false

        y: Math.min(Math.max(0, -flickable.contentY), flickable.contentHeight-flickable.height - flickable.contentY)

        onScrollableOffsetChanged: if (!flickable.moving) flickable.contentY = scrollableOffset.y

        SilicaFlickable {
            id: flickable

            y: -webView.y // include pulley menu as it's made visible

            width: parent.width
            height: parent.height

            contentHeight: Math.max(webView.contentHeight+webView.bottomMargin+webView.topMargin, webView.height)
            contentWidth: width

            quickScrollEnabled: false

            onContentYChanged: if (flickable.moving) webView.scrollTo(webView.scrollableOffset.x, flickable.contentY)

            SilicaPrivate.Wallpaper {
                anchors.fill: headerLoader
            }
            Loader {
                id: headerLoader
                width: flickable.width
            }

            SilicaPrivate.Wallpaper {
                anchors.fill: footerLoader
                verticalOffset: flickable.contentHeight - flickable.contentY - height
            }
            Loader {
                id: footerLoader
                width: flickable.width
                y: Math.ceil(webView.y + flickable.contentHeight - height + (flickable.contentY - webView.scrollableOffset.y))
            }
        }
    }
}
