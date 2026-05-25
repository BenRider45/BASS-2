import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import Constants 1.0
import BASSGraphics 1.0
import BASS

ApplicationWindow {
    id: window
    width: 1280
    height: 800
    visible: true
    title: projectManager.projectAttached ? "BASS — " + projectManager.currentProjectName : "BASS — Birdsong Annotations with Spike Sequences"

    Material.theme: Material.Dark
    Material.accent: Material.Teal

    property bool isLoading: false
    property string projectName: projectManager.projectAttached ? projectManager.currentProjectName : "No Project"
    property string currentProjectDir: projectManager.projectAttached ? projectManager.currentProjectDir.absolutePath : "No project"
    property bool deBUG: false
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
                onTriggered: projectManager.saveCurrentProject()
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
            Action {
                text: "Toggle Debug Mode "
                shortcut: "Ctrl+D"
                onTriggered: () => {
                    window.deBUG = !window.deBUG;
                }
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

                FilesPanel {

                    onImportFiles: () => {
                        fileDialog.open();
                    }
                    onFileSelected: index => {
                        spectrogramView.renderNewFile(index);
                        //console.log("Opening file with name: ", spectrogramView.getCurrentFileName());
                        overlay.currentFileName = spectrogramView.getCurrentFileName();
                    }
                }
                ClusterSummaryPanel {}
            }
        }
    }

    FolderDialog {
        id: fileDialog
        title: "Select a directory of wav files to import"

        onAccepted: {
            console.log("Importing Wav Files from directory:", fileDialog.selectedFolder);
            projectManager.importWavFiles(fileDialog.selectedFolder.toString().replace("file://", ""));
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
                //TODO This design is super clunky, the spectrogramView and SettingsPanel are so coupled together all of these function calls are redundant
                //The Settings Panel needs refactored such that any component who needs a section in the setting panel OWN the front end to that settings panel.
                //Perhaps an abstract settings Panel Module which allows for dynamic addition of settings? (list model!!!)
                SettingsPanel {
                    id: settingsPanel
                    onXScaleChanged: function (value) {
                        console.log("XScale Changed to ", value);
                        spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.XSCALE);
                    }

                    onYScaleChanged: function (value) {
                        console.log("YScale Changed to ", value);
                    //   spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.YSCALE);
                    }

                    onX0Changed: function (value) {
                        console.log("X0 Changed to ", value);

                        spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.X0);
                    }
                    onY0Changed: function (value) {
                        console.log("Y0 Changed to ", value);

                    //  spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.Y0);
                    }
                    onHopLengthChanged: function (value) {
                        console.log("Hop Length Changed to ", value);

                        spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.HOP_SIZE);
                    }

                    onWindowLengthChanged: function (value) {
                        console.log("Window length changed to ", value);
                        spectrogramView.adjustSpectrogramProviderConfig(value, SpectrogramProvider.CONFIG_TYPE.WINDOW_LENGTH);
                    }
                    function setConfigValue(value, configOption) {
                        switch (configOption) {
                        case SpectrogramProvider.CONFIG_TYPE.XSCALE:
                            settingsPanel.setxScale(value);
                            break;
                        case SpectrogramProvider.CONFIG_TYPE.YSCALE:
                            settingsPanel.setyScale(value);
                            break;
                        case SpectrogramProvider.CONFIG_TYPE.X0:
                            settingsPanel.setX0(value);
                            break;
                        case SpectrogramProvider.CONFIG_TYPE.Y0:
                            settingsPanel.setY0(value);
                            break;
                        case SpectrogramProvider.CONFIG_TYPE.HOP_SIZE:
                            settingsPanel.setHopLength(value);
                            break;
                        case SpectrogramProvider.CONFIG_TYPE.WINDOW_LENGTH:
                            settingsPanel.setWindowLength(value);
                            break;
                        }
                    }
                    onReRenderRequest: () => {
                        spectrogramView.reRender();
                    }
                }
            }
        }
    }

    // ── Central Content ──
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Vertical
            // Item {
            // property alias spectrogramView: spectrogramView
            Item {
                id: spectrogramAnnotationScaffold
                SplitView.fillWidth: true
                SplitView.minimumHeight: 256
                property alias spectrogramView: spectrogramView
                property alias overlay: overlay
                property int annotationMode: 0
                property int partialAnnotationIndex: 0
                property int partialAnnotationBeginX: 0
                focus: true

                Keys.onLeftPressed: spectrogramView.crementSpectrogramProviderConfig(false, 5, SpectrogramProvider.CONFIG_TYPE.X0)
                Keys.onRightPressed: spectrogramView.crementSpectrogramProviderConfig(true, 5, SpectrogramProvider.CONFIG_TYPE.X0)
                //Keys.onUpPressed: spectrogramView.crementSpectrogramProviderConfig(true, 5, SpectrogramProvider.CONFIG_TYPE.Y0)
                //Keys.onDownPressed: spectrogramView.crementSpectrogramProviderConfig(false, 5, SpectrogramProvider.CONFIG_TYPE.Y0)
                Keys.onReturnPressed: function () {
                    console.log("Current Annotation Mode: ", annotationMode);
                    switch (annotationMode) {
                    case 0:
                        console.log("Beginning annotation with cursor at: ", overlay.cursorX);
                        partialAnnotationIndex = projectManager.annotationsModel.beginFrame((overlay.cursorX / overlay.x_scale) + overlay.x_0, overlay.currentFileName);

                        partialAnnotationBeginX = overlay.cursorX;
                        break;
                    case 1:
                        if (overlay.cursorX > partialAnnotationBeginX) {
                            console.log("Valid End Frame");
                            var index = (overlay.cursorX / overlay.x_scale) + overlay.x_0;
                            projectManager.annotationsModel.completeFrame(partialAnnotationIndex, index);
                            console.log("Index: ", overlay.cursorX);
                            promptDialog.modifyingIndex = partialAnnotationIndex;
                            console.log("promptDialog.modifyingIndex: ", promptDialog.modifyingIndex);
                            promptDialog.open();
                            break;
                        } else {
                            annotationMode -= 1;
                        }
                        break;
                    }

                    annotationMode = (annotationMode + 1) % 2;
                }
                Keys.onPressed: function (event) {
                    switch (event.key) {
                    case Qt.Key_D:
                        overlay.cursorX += spectrogramView.cursorStep;
                        event.accepted = true;
                        break;
                    case Qt.Key_A:
                        overlay.cursorX -= spectrogramView.cursorStep;
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
                        event.accepted = true;
                    case Qt.Key_Equal:
                        spectrogramView.cursorStep = Math.min(50, spectrogramView.cursorStep + 1);
                        break;
                    case Qt.Key_Minus:
                        spectrogramView.cursorStep = Math.max(1, spectrogramView.cursorStep - 1);
                        event.accepted = true;
                        break;
                    case Qt.Key_Backspace:
                        console.log("back");
                        console.log(overlay.howManySelected());
                        overlay.deleteSelected();

                        break;
                    }
                }
                SpectrogramView {
                    id: spectrogramView
                    focus: true
                    anchors.fill: parent
                    onSpectrogramProviderConfigChanged: function (value, configOption) {
                        settingsPanel.setConfigValue(value, configOption);
                    }
                    onCurrentFileDeltaTPerSampleChanged: function (value) {
                        settingsPanel.setFileDeltaTPerSample(value);
                    }
                    // Keyboard shortcuts

                }
                AnnotationOverlay {
                    id: overlay
                    cursorX: width / 2
                    anchors.fill: parent
                    SplitView.fillWidth: true
                    SplitView.minimumHeight: 256
                    x_0: spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.X0)
                    x_scale: spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.XSCALE)
                    deBUG: window.deBUG
                    currentFileName: ""
                }
                MouseArea {
                    id: spectrogramAnnotationScaffoldMouseArea
                    anchors.fill: parent
                    onClicked: function (mouse) {
                        overlay.cursorX = mouse.x;
                        console.log("curorX : ", overlay.cursorX);
                        overlay.forceActiveFocus();
                    }
                }
            }
            InfoBar {
                //SplitView.Layout.fillWidth: true

                SplitView.Layout.fillHeight: true
            }
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
        property int modifyingIndex: 0
        onLabelAccepted: function (label) {
            console.log("Modifying index: ", modifyingIndex);
            projectManager.annotationsModel.editLabel(modifyingIndex, label);
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
        syncSpectrogramSettingsPanelToSpectrogramProvider();

        projectSelectWindow.open();
        //}
    }

    function syncSpectrogramSettingsPanelToSpectrogramProvider() {
        settingsPanel.setxScale(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.XSCALE));
        settingsPanel.setyScale(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.YSCALE));
        settingsPanel.setX0(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.X0));

        settingsPanel.setY0(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.Y0));

        settingsPanel.setHopLength(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.HOP_SIZE));

        settingsPanel.setWindowLength(spectrogramAnnotationScaffold.spectrogramView.getSpectrogramValue(SpectrogramProvider.CONFIG_TYPE.WINDOW_LENGTH));
    }
}
