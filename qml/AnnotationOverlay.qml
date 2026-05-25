import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Bass

Item {
    id: root

    property int cursorX: width / 2
    property int cursorStepSize: 5
    property color cursorColor: "#FF4444"
    property color boundaryColor: "#44FF44"
    property double x_0: 0.0
    property double x_scale: 1.0
    property bool deBUG: true
    property string currentFileName: ""

    function howManySelected() {
        var output = 0;
        console.log("how many selected");
        var annotationStatus = annotationSelectionStatus();
        console.log("annotatationStatus: ", annotationStatus);

        for (var i = 0; i < annotationStatus.length; i++) {
            if (annotationStatus[i]) {
                console.log("Selected");
                output = output + 1;
                console.log("Now output: ", output);
            } else {
                console.log("not selected :(");
                console.log("Now output: ", output);
            }
        }
        return output;
    }

    function deleteSelected() {
        var annotationStatus = annotationSelectionStatus();
        for (var i = 0; i < annotationStatus.length; i++) {
            if (annotationStatus[i]) {
                console.log(projectManager.annotationsModel.removeFrame(i));
                console.log("removed annotation at index ", i);
            }
        }
    }
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
    RowLayout {
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
        }
        GridLayout {
            x: 10
            y: 10
            Repeater {
                model: repeater.model
                Layout.fillWidth: true
                Layout.fillHeight: true
                delegate: Text {
                    color: repeater.itemAt(model.index).modelData.selected ? Color.Green : Color.Red

                    //size: 20
                    text: model.index
                }
            }
        }

        visible: root.deBUG
    }

    function annotationSelectionStatus() {
        var output = [];
        for (var i = 0; i < repeater.count; i++) {
            output[i] = repeater.itemAt(i).selected;
        }
        return output;
    }

    Repeater {
        id: repeater
        model: projectManager.annotationsModel

        delegate: Item {
            property bool selected: model.fileName == root.currentFileName & (root.cursorX >= (model.startFrame * x_scale - x_0 * x_scale) & root.cursorX <= (model.endFrame * x_scale - x_0 * x_scale))
            // Left boundary

            Rectangle {
                x: model.startFrame * x_scale - x_0 * x_scale
                y: 0
                width: 10
                height: root.height
                color: selected ? "#FFFFFF" : root.boundaryColor
                opacity: 1
                ColumnLayout {
                    Repeater {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: 15
                        delegate: Rectangle {
                            height: 3
                            width: parent.width
                            color: "white"
                        }
                    }
                }
                visible: model.fileName == root.currentFileName
            }

            // Right boundary
            Rectangle {
                //x: ((model.endFrame % root.width) - root.x_0) * root.x_scale
                x: model.endFrame * x_scale - x_0 * x_scale
                y: 0
                width: 10
                height: root.height
                color: selected ? "#FFFFFF" : root.boundaryColor
                opacity: 1
                ColumnLayout {
                    Repeater {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: 15
                        delegate: Rectangle {
                            height: 30
                            width: parent.width
                            color: "orange"
                        }
                    }
                }
                visible: !model.isPartial & model.fileName == root.currentFileName
            }

            // Label
            Text {
                x: (((model.endFrame + model.startFrame) / 2) * x_scale - x_0 * x_scale)
                y: 4
                text: model.label
                color: "#FFFFFF"
                font.pixelSize: 11
                font.bold: true
                visible: model.fileName == root.currentFileName
            }
        }
    }
}
