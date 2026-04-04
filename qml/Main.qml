import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Constants 1.0
import BASS

ApplicationWindow {
    id: window
    width: 1280
    height: 800
    visible: true
    title: projectManager.projectAttached ? "BASS — " + projectManager.projectMetadata.projectName : "BASS — Birdsong Annotations with Spike Sequences"

    Material.theme: Material.Dark
    Material.accent: Material.Teal

    property bool isLoading: false
    property string projectName: projectManager.projectAttached ? projectManager.projectMetadata.projectName : "noProject"
    property string currentProjectDir: projectManager.projectAttached ? projectManager.projectMetadata.projectDir : "no/project/dir"

    Connections {
        target: projectManager
        function onProjectLoading(projectDir) {
            console.log("Got to project loading");
            projectSelectWindow.close();
            // currentProjectDir = projectDir;
            isLoading = true;
        }

        function onProjectLoaded(projectData) {
            console.log("Saw onProjectLoaded Signal");
            //currentProjectData= projectData;

            // LoadingSplashScreen.close();
            isLoading = false;
        }

        function onCurrentProjectChanged(projectData) {
            console.log("New Project Data: ", projectData);
        }

        function onError(errormsg) {
            console.log("ERROR :( :", errormsg);
        }

        function onProjectAttachedChanged() {
            console.log("projectAttached Changed to ");
            console.log(projectManager.projectAttached);
            console.log("EOL");
        }
    }
    // ── Menu Bar ──
    menuBar: MenuBar {
        Menu {
            title: "&File"
            Action {
                text: "Open Project..."
                shortcut: "Ctrl+O"
                onTriggered: projectSelectWindow.open()
            }
            Action {
                text: "Save"
                shortcut: "Ctrl+S"
                onTriggered: annotationModel.save()
            }
            MenuSeparator {}
            Action {
                text: "Quit"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: "&View"
            Action {
                text: "Toggle Files Panel"
                shortcut: "Ctrl+1"
                onTriggered: leftDrawer.visible ? leftDrawer.close() : leftDrawer.open()
            }
            Action {
                text: "Toggle Control Panel"
                shortcut: "Ctrl+2"
                onTriggered: rightDrawer.visible ? rightDrawer.close() : rightDrawer.open()
            }
        }
        Menu {
            title: "&Decoder"
            Action {
                text: "Run Decoder"
            }
            Action {
                text: "Train Model"
            }
            MenuSeparator {}
            Action {
                text: "Decoder Settings..."
                onTriggered: {
                    rightDrawer.open();
                    rightTabBar.currentIndex = 2;
                }
            }
        }
        Menu {
            title: "&Help"
            Action {
                text: "About BASS"
                onTriggered: aboutDialog.open()
            }
        }
    }

    // ── Left Drawer (Files, Cluster Editor) ──
    Drawer {
        id: leftDrawer
        edge: Qt.LeftEdge
        modal: false
        interactive: true
        width: 300
        height: window.height - window.menuBar.height
        y: window.menuBar.height

        background: Rectangle {
            color: "#2b2b2b"
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            TabBar {
                id: leftTabBar
                Layout.fillWidth: true
                TabButton {
                    text: "Files"
                }
                TabButton {
                    text: "Clusters"
                }
            }

            StackLayout {
                currentIndex: leftTabBar.currentIndex
                Layout.fillWidth: true
                Layout.fillHeight: true

                FilesPanel {}
                ClusterSummaryPanel {}
            }
        }
    }

    // ── Right Drawer (Control Panel, Settings, Player) ──
    Drawer {
        id: rightDrawer
        edge: Qt.RightEdge
        modal: false
        interactive: true
        width: 320
        height: window.height - window.menuBar.height
        y: window.menuBar.height

        background: Rectangle {
            color: "#2b2b2b"
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            TabBar {
                id: rightTabBar
                Layout.fillWidth: true
                TabButton {
                    text: "Control"
                }
                TabButton {
                    text: "Player"
                }
                TabButton {
                    text: "Settings"
                }
            }

            StackLayout {
                currentIndex: rightTabBar.currentIndex
                Layout.fillWidth: true
                Layout.fillHeight: true

                ControlPanel {}
                PlayerPanel {}
                SettingsPanel {}
            }
        }
    }

    // ── Central Content ──
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        SpectrogramView {
            id: spectrogramView
            Layout.fillWidth: true
            Layout.fillHeight: true
            focus: true

            // Keyboard shortcuts
            Keys.onLeftPressed: spectrogramController.prevPage()
            Keys.onRightPressed: spectrogramController.nextPage()
            Keys.onUpPressed: spectrogramController.setThreshold(spectrogramController.threshold + 0.01)
            Keys.onDownPressed: spectrogramController.setThreshold(spectrogramController.threshold - 0.01)
            Keys.onReturnPressed: promptDialog.open()

            Keys.onPressed: function (event) {
                switch (event.key) {
                case Qt.Key_D:
                    spectrogramView.cursorX += spectrogramView.cursorStep;
                    event.accepted = true;
                    break;
                case Qt.Key_A:
                    spectrogramView.cursorX -= spectrogramView.cursorStep;
                    event.accepted = true;
                    break;
                case Qt.Key_C:
                    // Next threshold crossing — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_Z:
                    // Prev threshold crossing — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_V:
                    // Next annotation — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_X:
                    // Prev annotation — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_N:
                    // Next file — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_P:
                    // Prev file — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_R:
                    // Remove selected annotation — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_L:
                    promptDialog.open();
                    event.accepted = true;
                    break;
                case Qt.Key_G:
                    // Adjust to threshold — placeholder
                    event.accepted = true;
                    break;
                case Qt.Key_Plus:
                case Qt.Key_Equal:
                    spectrogramView.cursorStep = Math.min(50, spectrogramView.cursorStep + 1);
                    event.accepted = true;
                    break;
                case Qt.Key_Minus:
                    spectrogramView.cursorStep = Math.max(1, spectrogramView.cursorStep - 1);
                    event.accepted = true;
                    break;
                }
            }
        }

        InfoBar {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    // ── Dialogs ──
    ProjectInitDialog {
        id: projectInitDialog
    }

    ProjectSelectWindow {
        id: projectSelectWindow

        anchors.centerIn: parent
        onConfirmedProject: function (name, path, makingNewProject) {
            //projectName = name;
            console.log("Path: " + path);
            //projectDir = path;

            if (makingNewProject) {
                console.log("Making New Project");
                projectManager.initProject(path, name, "Tweety Bird");
            } else {
                console.log("Opening not new project");
                projectManager.loadProject(path);
            }
        }

        onOpeningRecentProject: function (UID) {
            console.log("Opening Recent Project with UID" + UID);
            projectManager.loadRecentProject(UID);
        }
    }

    LoadingSplashScreen {
        id: loadingScreen

        visible: isLoading
    }

    PromptDialog {
        id: promptDialog
        onLabelAccepted: function (label) {
            annotationModel.addFrame(spectrogramView.cursorX, spectrogramView.cursorX + 100, label);
        }
    }

    Dialog {
        id: aboutDialog
        title: "About BASS"
        modal: true
        standardButtons: Dialog.Ok
        width: 400
        anchors.centerIn: parent

        ColumnLayout {
            spacing: 12

            Label {
                text: "BASS"
                font.pixelSize: 24
                font.bold: true
                color: "#EEE"
            }
            Label {
                text: "Birdsong Annotations with Spike Sequences"
                color: "#CCC"
                font.pixelSize: 14
            }
            Label {
                text: "Jin Group"
                color: "#999"
                wrapMode: Text.Wrap
            }
        }
    }

    // Show project init on first launch
    Component.onCompleted: {
        //if (!projectManager.isInitialized) {
        projectSelectWindow.open();
        //}
    }
}
