import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: "Cluster Summary"
            font.pixelSize: 16
            font.bold: true
            color: "#EEE"
        }

        Label {
            text: "Syllable clusters will appear here"
            color: "#999"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        // Placeholder grid of syllable spectrograms
        GridView {
            id: clusterGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 80
            cellHeight: 80
            clip: true

            model: 0  // No data yet

            delegate: Rectangle {
                width: 72
                height: 72
                radius: 4
                color: "#3a3a3a"

                Label {
                    anchors.centerIn: parent
                    text: index.toString()
                    color: "#888"
                }
            }
        }

        Label {
            text: "No clusters loaded"
            color: "#666"
            font.pixelSize: 11
        }
    }
}
