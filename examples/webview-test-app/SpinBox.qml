import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    id: root
    property alias label: textField.label
    property real minimumValue: -Infinity
    property real maximumValue: Infinity
    property real value: 0.0
    property real stepSize: 0.1

    implicitHeight: Theme.itemSizeMedium
    implicitWidth: 200

    onValueChanged: value = Math.min(Math.max(minimumValue, value), maximumValue)

    Row {
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter

        IconButton {
            id: buttonMinus
            width: height
            anchors.verticalCenter: textField.verticalCenter
            icon.source: "image://theme/icon-video-overlay-play"
            rotation: 180
            onClicked: root.value -= root.stepSize
        }

        TextField {
            id: textField

            inputMethodHints: Qt.ImhFormattedNumbersOnly
            placeholderText: label
            text: root.value.toFixed(2)
            horizontalAlignment: TextInput.AlignHCenter
            width: parent.width - 2*buttonMinus.width
            anchors.verticalCenter: parent.verticalCenter

            EnterKey.iconSource: "image://theme/icon-m-enter-next"
            EnterKey.onClicked: {
                focus = false;
                root.value = text;
            }

            onFocusChanged: {
                if (!focus) {
                    root.value = text;
                }
            }
        }

        IconButton {
            height: buttonMinus.height
            width: buttonMinus.height
            anchors.verticalCenter: textField.verticalCenter
            icon.source: "image://theme/icon-video-overlay-play"
            onClicked: root.value += root.stepSize
        }
    }
}
