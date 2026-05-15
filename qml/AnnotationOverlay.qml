import QtQuick
import QtQuick.Controls

Item {
    id: root

    property int cursorX: width / 2
    property int cursorStepSize: 5
    property color cursorColor: "#FF4444"
    property color boundaryColor: "#44FF44"
    property double x_0: 0.0
    property double x_scale: 1.0

    // Cursor line
    Rectangle {
        id: cursorLine
        x: root.cursorX
        y: 0
        width: 2
        height: parent.height
        color: root.cursorColor
        opacity: .9
    }

    // Annotation boundaries from model
    Repeater {
        model: annotationModel

        Item {
            // Left boundary
            Rectangle {
                x: ((model.startFrame % root.width) - root.x_0) * root.x_scale
                y: 0
                width: 10
                height: root.height
                color: root.boundaryColor
                opacity: 1
            }

            // Right boundary
            Rectangle {
                x: ((model.endFrame % root.width) - root.x_0) * root.x_scale
                y: 0
                width: 10
                height: root.height
                color: root.boundaryColor
                opacity: 1
                visible: !model.isPartial
            }

            // Label
            Text {
                x: ((((model.endFrame + model.startFrame) / 2) % root.width) - root.x_0) * root.x_scale
                y: 4
                text: model.label
                color: "#FFFFFF"
                font.pixelSize: 11
                font.bold: true
            }
        }
    }
}
