import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    title: "Enter Label"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 300
    anchors.centerIn: parent

    property alias labelText: labelField.text
    signal labelAccepted(string label)

    onAccepted: {
        labelAccepted(labelField.text);
        labelField.text = "";
    }

    onOpened: {
        labelField.forceActiveFocus();
        labelField.selectAll();
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: "Syllable label:"
            color: "#DDD"
        }

        TextField {
            id: labelField
            placeholderText: "Enter label..."
            Layout.fillWidth: true

            Keys.onReturnPressed: root.accept()
        }
    }
}
