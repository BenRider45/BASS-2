import QtQuick
import QtQuick.Controls

Item {
    id: root

    property int cursorX: width / 2
    property int cursorStepSize: 5
    property color cursorColor: "#FF4444"
    property color boundaryColor: "#44FF44"

    // Cursor line
    Rectangle {
        id: cursorLine
        x: root.cursorX
        y: 0
        width: 2
        height: parent.height
        color: root.cursorColor
        opacity: 0.9
    }

    // Annotation boundaries from model
    Repeater {
        model: annotationModel

        Item {
            // Left boundary
            Rectangle {
                x: model.startFrame ? (model.startFrame / 10) % root.width : 0
                y: 0
                width: 1
                height: root.height
                color: root.boundaryColor
                opacity: 0.7
            }

            // Right boundary
            Rectangle {
                x: model.endFrame ? (model.endFrame / 10) % root.width : 0
                y: 0
                width: 1
                height: root.height
                color: root.boundaryColor
                opacity: 0.7
            }

            // Label
            Text {
                x: model.startFrame ? (model.startFrame / 10) % root.width + 3 : 3
                y: 4
                text: model.label || ""
                color: "#FFFFFF"
                font.pixelSize: 11
                font.bold: true
            }
        }
    }
}
