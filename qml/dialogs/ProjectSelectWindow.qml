import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import BASS
Rectangle {
    id: projectSelectScreen
    width: Window.width/2
    height: Window.height/2
    radius: 20
    clip: true
    color: "dimgray"
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    property string selectedProjectName: "";
    property string selectedProjectPath: "";
    signal confirmedProject(string projName, string projPath);

    CreateProjectDialog {
        id: createProjectDialog
        margins: -1
        padding: 24
        onProjectCreated: (path, name ) => {
            console.log("project " + name + " created at " + "path");
            selectedProjectName = name;
            selectedProjectPath = path;
            projectLoadConfirmationDialog.open();
            }

    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        anchors.topMargin: 30
        anchors.bottomMargin: 20
        spacing:parent.Height/10
		    Text {
		        id: text1
		        height: 49
            opacity: 0.793
            Layout.fillWidth: true
		        text: qsTr("BASS")
		        font.pixelSize: 48
		        horizontalAlignment: Text.AlignHCenter
		        font.family: ".AppleSystemUIFont"
          }


        Row {
            id: row
            height: 50
            Layout.fillWidth: true
            Button {
                id: findProjectButton
                text: qsTr("Find Project")
                width: newProjectButton.Width
                anchors.left: parent
                
                Layout.leftMargin: 30
                highlighted: true
                onClicked: fileDialog.open()
            }

            Button {
                id: newProjectButton
                text: qsTr("Create New Project")
                highlighted: true
                anchors.right: parent
                Layout.rightMargin: 30
                onClicked: createProjectDialog.open()
            }
        }

        FileDialog {
            id: fileDialog
            title: "Select a Project"
            nameFilters: ["QML Projects (*.qmlproject)", "All files (*)"]

            onAccepted: {
                console.log("Project selected:", fileDialog.selectedFile)
                projectLoadConfirmationDialog.open();
            }
        }

        Rectangle {
            id: recentProjView
            Layout.fillHeight: true
            Layout.topMargin: 30
            opacity: 1
            visible: true
            color: "#91303030"
            radius: 12
            border.width: 10
//            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 100
            anchors.rightMargin: 100
 //           anchors.verticalCenterOffset: 60

            ListView {
                id: recentProjectsList
                anchors.fill: parent
                anchors.margins: 20
                layer.enabled: false
                clip: true // hides items outside the bounds
                model: recentProjectsModel

                delegate: Rectangle {
                    width: recentProjectsList.width
                    height: 60
                    color: mouseArea.containsMouse ? "#e8f0fe" : "transparent"

                    Column {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 16

                        Text {
                            text: model.name
                            font.pixelSize: 14
                            font.bold: true
                        }
                        Text {
                            text: model.path
                            font.pixelSize: 11
                            color: "#888"
                        }
                    }

                    Text {
                        text: model.lastOpened
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#aaa"
                        font.pixelSize: 11
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: () => {
                            selectedProjectName = model.name;
                            selectedProjectPath = model.path;
                            projectLoadConfirmationDialog.open();
                        }
                    }
                }

            }

            ProjectLoadConfirmationDialog {
                id: projectLoadConfirmationDialog
                onConfirmProjectLoad: () => {
                    console.log("Project Printed;")
                    confirmedProject(selectedProjectName, selectedProjectPath)
                }
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
            ListElement {
                name: "Blah Blah Blah"
                path: "/something/something/something"
                lastOpened: "1 week ago"
            }
        }
    }
}
