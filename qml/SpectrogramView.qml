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
    function crementSpectrogramProviderConfig(increment, step, configOption) {
        if (increment) {
            console.log("incrementing config");
            hi.increaseSpectrogramProviderConfig(step, configOption);
        } else {
            hi.decreaseSpectrogramProviderConfig(step, configOption);
        }
    }
    function getSpectrogramValue(configOption) {
        return hi.getSpectrogramConfigValue(configOption);
    }
    function reRender() {
        hi.reRenderRequest();
    }
    function getDeltaTPerSample() {
        return spectrogram.current_File_Delta_T_Per_Sample;
    }
    signal spectrogramProviderConfigChanged(double value, var configOption)
    signal currentFileDeltaTPerSampleChanged(double value)
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
            property alias spectrogram: spectrogram
            Layout.fillHeight: true
            Layout.minimumHeight: 256
            function renderNewSpectrogramData(index) {
                console.log("Got here");
                spectrogram.loadNewSpectrogramData(index);
            }
            function reRenderRequest() {
                spectrogram.recomputeSpectrogram();
            }
            function modifySpectrogramProviderConfig(value, configOption) {
                spectrogram.modify_CONFIG_value(value, configOption);
                spectrogramProviderConfigChanged(value, configOption);
            }

            function increaseSpectrogramProviderConfig(magnitude, configOption) {
                spectrogram.crement_CONFIG_value(configOption, magnitude, true);

                spectrogramProviderConfigChanged(getSpectrogramConfigValue(configOption) + magnitude, configOption);
            }

            function decreaseSpectrogramProviderConfig(magnitude, configOption) {
                spectrogram.crement_CONFIG_value(configOption, magnitude, false);

                spectrogramProviderConfigChanged(getSpectrogramConfigValue(configOption) - magnitude, configOption);
            }

            function getSpectrogramConfigValue(configOption) {
                switch (configOption) {
                case SpectrogramProvider.CONFIG_TYPE.XSCALE:
                    return spectrogram.CONFIG_xscale;
                case SpectrogramProvider.CONFIG_TYPE.YSCALE:
                    return spectrogram.CONFIG_yscale;
                case SpectrogramProvider.CONFIG_TYPE.X0:
                    return spectrogram.CONFIG_x0;
                case SpectrogramProvider.CONFIG_TYPE.Y0:
                    return spectrogram.CONFIG_y0;
                case SpectrogramProvider.CONFIG_TYPE.HOP_SIZE:
                    return spectrogram.CONFIG_hop_size;
                case SpectrogramProvider.CONFIG_TYPE.WINDOW_LENGTH:
                    return spectrogram.CONFIG_window_length;
                }
            }

            SpectrogramProvider {
                id: spectrogram
                onCurrentFileDeltaTPerSampleChanged: function (value) {
                    root.currentFileDeltaTPerSampleChanged(value);
                }
                anchors.fill: parent
                Layout.fillHeight: true
                audioFilesModel: projectManager.audioFiles

                //    Component.onCompleted: loadNewSpectrogramData(0)
            }
            // AnnotationOverlay {
            //     id: overlay
            //     anchors.fill: parent
            //     cursorX: root.cursorX
            //     x_0: hi.getSpectrogramConfigValue(SpectrogramProvider.CONFIG_TYPE.X0)
            //     x_scale: hi.getSpectrogramConfigValue(SpectrogramProvider.CONFIG_TYPE.XSCALE)
            // }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: function (mouse) {
                    console.log("onClicked in SpecrView\n");
                    root.cursorX = mouse.x;
                    root.forceActiveFocus();
                }
                cursorShape: Qt.PointingHandCursor
                onWheel: wheel => {
                    if (wheel.modifiers & Qt.ShiftModifier) {

                        // if(wheel.angleDelta.y > 0){
                        //     hi.increaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_CHANGE_TYPE.Y0);
                        //   }
                        //   else if (wheel.angleDelta.y < 0){
                        //     hi.decreaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_CHANGE_TYPE.Y0);
                        //   }

                        if (wheel.angleDelta.x > 0) {
                            hi.increaseSpectrogramProviderConfig(.1, SpectrogramProvider.CONFIG_TYPE.XScale);
                        } else if (wheel.angleDelta.x < 0) {
                            if (spectrogram.CONFIG_xscale <= 0) {
                                hi.increaseSpectrogramProviderConfig(.1, SpectrogramProvider.CONFIG_TYPE.XScale);
                            } else {
                                hi.decreaseSpectrogramProviderConfig(.1, SpectrogramProvider.CONFIG_TYPE.XScale);
                            }
                        }
                        // hi.modifySpectrogramProviderConfig(1 * wheel.angleDelta.y, SpectrogramProvider.CONFIG_TYPE.X0);
                    } else {
                        if (wheel.angleDelta.y < 0) {
                            //   hi.increaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_TYPE.Y0);
                        } else if (wheel.angleDelta.y > 0) {
                            //   hi.decreaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_TYPE.Y0);
                        }

                        if (wheel.angleDelta.x < 0) {
                            hi.increaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_TYPE.X0);
                        } else if (wheel.angleDelta.x > 0) {
                            hi.decreaseSpectrogramProviderConfig(5, SpectrogramProvider.CONFIG_TYPE.X0);
                        }
                        // hi.modifySpectrogramProviderConfig(1 * wheel.angleDelta.y, SpectrogramProvider.CONFIG_TYPE.X0);
                    }
                }
                drag.target: spectrogram
                drag.minimumX: 0
                drag.maximumX: 1000
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
