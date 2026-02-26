import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#1e1e1e"
    implicitHeight: 28

    property string leftText: projectManager.isInitialized
        ? projectManager.birdName + " — " + annotationModel.currentFile
        : "No project loaded"
    property string rightText: "Annotations: " + annotationModel.count
        + "  |  Page: " + spectrogramController.navPage
        + "/" + spectrogramController.navMax

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 12
        anchors.rightMargin: 12

        Label {
            text: root.leftText
            color: "#BBBBBB"
            font.pixelSize: 12
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        Label {
            text: root.rightText
            color: "#999999"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignRight
        }
    }

    // Top border
    Rectangle {
        anchors.top: parent.top
        width: parent.width
        height: 1
        color: "#444"
    }
}
