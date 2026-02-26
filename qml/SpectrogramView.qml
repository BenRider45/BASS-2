import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int cursorX: width / 2
    property int cursorStep: 5

    Layout.fillWidth: true
    implicitHeight: 340

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Main spectrogram image + overlay
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 256

            Image {
                id: spectrogramImage
                anchors.fill: parent
                source: "image://spectrogram/current"
                cache: false
                fillMode: Image.Stretch

                function reload() {
                    var src = source;
                    source = "";
                    source = src;
                }
            }

            AnnotationOverlay {
                id: overlay
                anchors.fill: parent
                cursorX: root.cursorX
            }

            MouseArea {
                anchors.fill: parent
                onClicked: function(mouse) {
                    root.cursorX = mouse.x;
                    root.forceActiveFocus();
                }
            }
        }

        // Trace view below spectrogram
        Image {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            source: "image://spectrogram/trace"
            cache: false
            fillMode: Image.Stretch
        }
    }

    // Navigation label
    Label {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 8
        text: "Page " + spectrogramController.navPage + " / " + spectrogramController.navMax
        color: "#AAAAAA"
        font.pixelSize: 12
        background: Rectangle { color: "#80000000"; radius: 3 }
        padding: 4
    }
}
