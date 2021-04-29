import QtQml 2.0

QtObject {
    property var contextMenu: ({"type": "item", "component": "ContextMenu.qml" })
    property var alertPopup: ({"type": "dialog", "component": "AlertDialog.qml" })
    property var confirmPopup: ({"type": "dialog", "component": "ConfirmDialog.qml" })
    property var promptPopup: ({"type": "dialog", "component": "PromptDialog.qml" })
    property var authPopup: ({"type": "dialog", "component": "AuthDialog.qml" })
    property var passwordManagerPopup: ({"type": "dialog", "component": "PasswordManagerDialog.qml" })
    property var locationPermissionPopup: ({"type": "dialog", "component": "LocationDialog.qml" })
}
