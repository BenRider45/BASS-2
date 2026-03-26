import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick.Window
import Constants 1.0
import BASS

Dialog {
    id: projectSelectDialog
    title: "Select or Create Project"
    modal: true
    width: Math.min(800, parent.width * 0.8)
    height: Math.min(600, parent.height * 0.8)

    property string selectedProjectName: ""
    property string selectedProjectPath: ""
    property bool useMockProjectList: true
    property bool makingNewProject: false

    signal confirmedProject(string projName, string projPath, bool newProject)
    signal openingRecentProject(string UID)

    contentItem: ColumnLayout {
        spacing: 20

        Text {
            text: "BASS"
            font.pixelSize: 36
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            opacity: 0.9
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: "Find Project"
                highlighted: true
                Layout.fillWidth: true
                onClicked: fileDialog.open()
            }

            Button {
                text: "Create New Project"
                highlighted: true
                Layout.fillWidth: true
                onClicked: createProjectDialog.open()
            }
        }
        Text {
            text: "Recent Projects"
            font.pixelSize: 14
            font.bold: true
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            radius: 8
            border.color: "#444"
            border.width: 1

            ListView {
                id: recentProjectsList
                anchors.fill: parent
                anchors.margins: 8
                clip: true
                model: projectManager.recentProjects
                spacing: 4

                delegate: Rectangle {
                    width: recentProjectsList.width
                    height: 60
                    color: mouseArea.containsMouse ? "#2e2e2e" : "transparent"
                    radius: 4
                    border.color: mouseArea.containsMouse ? "#666" : "transparent"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text {
                                text: modelData[SharedConstants.PROJECT_DATA][SharedConstants.PROJECT_NAME]
                                font.pixelSize: 13
                                font.bold: true
                                color: "#fff"
                            }
                            Text {
                                text: modelData[SharedConstants.PROJECT_DATA][SharedConstants.PROJECT_DIR]
                                font.pixelSize: 10
                                color: "#999"
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }

                        Text {
                            text: modelData[SharedConstants.PROJECT_DATA][SharedConstants.PROJECT_LAST_ACCESSED]
                            font.pixelSize: 10
                            color: "#777"
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            makingNewProject = false;
                            openingRecentProject(modelData[SharedConstants.PROJECT_ID]);
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select a Project"
        nameFilters: ["twty Project (*.twty)", "All files (*)"]

        onAccepted: {
            console.log("Project selected:", fileDialog.selectedFile);
            projectLoadConfirmationDialog.open();
        }
    }

    CreateProjectDialog {
        id: createProjectDialog
        anchors.centerIn: parent
        onProjectCreated: (name, path) => {
            console.log("project " + name + " created at " + path);
            selectedProjectName = name;
            selectedProjectPath = path;
            makingNewProject = true;
            projectLoadConfirmationDialog.open();
        }
    }

    ProjectLoadConfirmationDialog {
        id: projectLoadConfirmationDialog
        anchors.centerIn: parent
        onConfirmProjectLoad: {
            console.log("Project loaded:", selectedProjectName);
            confirmedProject(selectedProjectName, selectedProjectPath, makingNewProject);
            projectSelectDialog.close();
        }
    }

    ListModel {
        id: recentProjectsModel

        ListElement {
            name: "My App"
            path: "/home/user/projects/myapp"
            lastOpened: "2 days ago"
        }
        ListElement {
            name: "Dashboard"
            path: "/home/user/projects/dash"
            lastOpened: "1 week ago"
        }
        ListElement {
            name: "Blah Blah Blah"
            path: "/something/something/something"
            lastOpened: "1 week ago"
        }
        ListElement {
            name: "Blah Blah Blah"
            path: "/something/something/something"
            lastOpened: "1 week ago"
        }
        ListElement {
            name: "Blah Blah Blah"
            path: "/something/something/something"
            lastOpened: "1 week ago"
        }
    }
}
