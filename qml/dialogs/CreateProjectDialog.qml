import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
Dialog{
    id: createProjectDialog
    title: "Create New Project"
    modal: true
    width: 400
    height: 300
    signal projectCreated(string name, string path);
    // Folder browser dialog
        FolderDialog {
            id: folderDialog
            title: "Select Project Location"

            onAccepted: {
                // Strip the file:/// prefix and populate the text field
                projectPathField.text = folderDialog.selectedFolder.toString().replace("file:///", "/")
            }
        }

    Column {
        spacing: 12
        anchors.fill: parent

        Text { text: "Project Name" }
        TextField {
            id: projectNameField
            width: parent.width
            placeholderText: "Enter project name..."
        }

        // Row combines the text field and browse button
                Row {
                    width: parent.width
                    spacing: 8

                    TextField {
                        id: projectPathField
                        width: parent.width - browseButton.width - 8
                        placeholderText: "Select a folder..."
                        readOnly: true    // user picks via dialog, not typing
                    }

                    Button {
                        id: browseButton
                        text: "Browse"
                        onClicked: folderDialog.open()
                    }
                }

    }

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: {
        projectCreated(projectNameField.text, projectPathField.text)
    }
}
