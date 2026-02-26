import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Label {
            text: "Playback"
            font.pixelSize: 16
            font.bold: true
            color: "#EEE"
        }

        // Status
        Rectangle {
            Layout.fillWidth: true
            height: 40
            radius: 6
            color: audioPlayer.isPlaying ? "#2d5a2d" : "#3a3a3a"

            Label {
                anchors.centerIn: parent
                text: audioPlayer.isPlaying ? "Playing..." : "Stopped"
                color: audioPlayer.isPlaying ? "#88FF88" : "#999"
                font.pixelSize: 14
            }
        }

        // Controls
        GridLayout {
            columns: 2
            Layout.fillWidth: true
            columnSpacing: 8
            rowSpacing: 8

            Button {
                text: "Stop"
                Layout.fillWidth: true
                onClicked: audioPlayer.stop()
                enabled: audioPlayer.isPlaying
            }
            Button {
                text: "Play All"
                Layout.fillWidth: true
                onClicked: audioPlayer.playAll()
            }
            Button {
                text: "Play Selected"
                Layout.fillWidth: true
                onClicked: audioPlayer.playSelected(0, 0)
            }
            Button {
                text: "Play from Cursor"
                Layout.fillWidth: true
                onClicked: audioPlayer.playFromCursor(0)
            }
        }

        // Position display
        GroupBox {
            title: "Position"
            Layout.fillWidth: true

            Label {
                text: "Frame: " + audioPlayer.position
                color: "#DDD"
            }
        }

        Item { Layout.fillHeight: true }
    }
}
