import QtQuick
import QtQuick.Controls

Rectangle {
    id: splashScreen
    width: 400
    height: 300
    opacity: 0.951
    color: "#4d4d4d"
    radius: 50
    signal projectLoaded()
    Column{
        id: column
        anchors.fill: parent
        Text{
            text: "Loading..."
            anchors.top: parent.top
            anchors.topMargin: 50
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }


    }


    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 20
    }

    Timer {
        id: loadTimer
        interval: 2000        // 2 seconds, adjust to taste
        repeat: false
        running: splashScreen.visible
        onTriggered: {
            projectLoaded()
        }
    }

}
