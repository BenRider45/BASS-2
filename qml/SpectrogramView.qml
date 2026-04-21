import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import BASSGraphics 1.0
import BASS

Item {
    id: root
    function renderNewFile(index) {
        hi.renderNewSpectrogramData(index);
    }
    function adjustSpectrogramProviderConfig(value, configOption) {
      console.log("Got to external mod config function\n");
      hi.modifySpectrogramProviderConfig(value, configOption);
    }
    property int cursorX: width / 2
    property int cursorStep: 5

    Layout.fillWidth: true
    implicitHeight: 340

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Main spectrogram image + overlay
        Item {
            id: hi
            Layout.fillWidth: true

            Layout.fillHeight: true
            Layout.minimumHeight: 256
            function renderNewSpectrogramData(index) {
                console.log("Got here");
                spectrogram.loadNewSpectrogramData(index);
            }

            function modifySpectrogramProviderConfig(value, configOption) {
              console.log("Got to internal mod spec provider config function\n");
              spectrogram.modify_CONFIG_value(value, configOption);
            }

            SpectrogramProvider {
                id: spectrogram

                anchors.fill: parent
                Layout.fillHeight: true
                audioFilesModel: projectManager.audioFiles

                // Component.onCompleted: loadNewSpectrogramData(0)
            }
            AnnotationOverlay {
                id: overlay
                anchors.fill: parent
                cursorX: root.cursorX
            }

            MouseArea {
                anchors.fill: parent
                onClicked: function (mouse) {
                    root.cursorX = mouse.x;
                    root.forceActiveFocus();
                }

                onWheel: wheel => {
                    console.log("Sroll!!!!!");
                }
            }
        }
    }

    // Navigation label
    // Label {
    //     anchors.top: parent.top
    //     anchors.right: parent.right
    //     anchors.margins: 8
    //     text: "Page " + spectrogramController.navPage + " / " + spectrogramController.navMax
    //     color: "#AAAAAA"
    //     font.pixelSize: 12
    //     background: Rectangle { color: "#80000000"; radius: 3 }
    //     padding: 4
    // }
}
