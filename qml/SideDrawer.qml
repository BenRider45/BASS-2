import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Drawer {
    id: root

    property int drawerEdge: Qt.RightEdge
    property alias tabBar: tabBarLoader.sourceComponent
    property alias content: contentStack.children

    edge: drawerEdge
    modal: false
    interactive: true
    width: 320
    height: parent ? parent.height : 600

    background: Rectangle {
        color: "#2b2b2b"
        Rectangle {
            width: root.drawerEdge === Qt.LeftEdge ? 1 : 0
            height: parent.height
            anchors.right: parent.right
            color: "#555"
        }
        Rectangle {
            width: root.drawerEdge === Qt.RightEdge ? 1 : 0
            height: parent.height
            anchors.left: parent.left
            color: "#555"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Loader {
            id: tabBarLoader
            Layout.fillWidth: true
        }

        Item {
            id: contentStack
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
