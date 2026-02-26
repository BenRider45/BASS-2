import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: root
    title: "Initialize Project"
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    width: 450
    anchors.centerIn: parent

    onAccepted: {
        projectManager.initProject(projectDirField.text, birdNameField.text)
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 12

        Label {
            text: "Welcome to BASS"
            font.pixelSize: 18
            font.bold: true
            color: "#EEE"
        }

        Label {
            text: "Set up your project directory and bird name to begin."
            color: "#BBB"
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        GroupBox {
            title: "Project"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 8

                Label { text: "Bird Name"; color: "#DDD" }
                TextField {
                    id: birdNameField
                    placeholderText: "e.g. bird_01"
                    Layout.fillWidth: true
                }

                Label { text: "Project Directory"; color: "#DDD" }
                RowLayout {
                    TextField {
                        id: projectDirField
                        placeholderText: "/path/to/project"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Browse..."
                        onClicked: folderDialog.open()
                    }
                }

                Label { text: "WAV Directory"; color: "#DDD" }
                RowLayout {
                    TextField {
                        id: wavDirField
                        placeholderText: "/path/to/wav/files"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Browse..."
                        onClicked: wavFolderDialog.open()
                    }
                }
            }
        }
    }

    FolderDialog {
        id: folderDialog
        title: "Select Project Directory"
        onAccepted: projectDirField.text = selectedFolder.toString().replace("file://", "")
    }

    FolderDialog {
        id: wavFolderDialog
        title: "Select WAV Directory"
        onAccepted: wavDirField.text = selectedFolder.toString().replace("file://", "")
    }
}
