import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int cursorX: width / 2
    property int cursorStepSize: 5
    property color cursorColor: "#FF4444"
    property color boundaryColor: "#44FF44"
    property double x_0: 0.0
    property double x_scale: 1.0
    property bool deBUG: true
    // Cursor line
    Rectangle {
        id: cursorLine
        x: root.cursorX
        y: 0
        width: 1
        height: parent.height
        color: root.cursorColor
        opacity: .9
    }
    ColumnLayout {
        Text {
            x: 10

            color: "#FFFFFF"
            text: "x_0: " + x_0 + " , x_scale: " + x_scale + " , cursor pos: " + cursorX
        }
        Text {
            x: 10

            color: "#FFFFFF"
            text: "Map Value: (x+ x_0 /x_scale): " + (cursorX + x_0) / x_scale
        }
        visible: deBUG
    }
    // Annotation boundaries from model
    Repeater {
        model: annotationModel

        Item {
            // Left boundary
            Rectangle {
                x: model.startFrame * x_scale - x_0 * x_scale
                y: 0
                width: 10
                height: root.height
                color: root.boundaryColor
                opacity: 1
            }

            // Right boundary
            Rectangle {
                //x: ((model.endFrame % root.width) - root.x_0) * root.x_scale
                x: model.endFrame * x_scale - x_0 * x_scale
                y: 0
                width: 10
                height: root.height
                color: root.boundaryColor
                opacity: 1
                visible: !model.isPartial
            }

            // Label
            Text {
                x: (((model.endFrame + model.startFrame) / 2) * x_scale - x_0 * x_scale)
                y: 4
                text: model.label
                color: "#FFFFFF"
                font.pixelSize: 11
                font.bold: true
            }
        }
    }
}
