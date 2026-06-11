import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal xScaleChanged(double value)
    signal yScaleChanged(double value)
    signal x0Changed(double value)
    signal y0Changed(double value)
    signal windowLengthChanged(int value)
    signal hopLengthChanged(int value)
    signal windowFunctionChanged(string func)
    signal reRenderRequest
    property alias spectSettingsGroup: spectrogramSettingsGroup
    function getxScale() {
        return spectrogramSettingsGroup.xScale;
    }
    function setxScale(value) {
        //     spectrogramSettingsGroup.xScaleSlider = value;
    }
    function getyScale() {
        return spectrogramSettingsGroup.yScale;
    }

    function setyScale(value) {
        //      spectrogramSettingsGroup.yScaleSlider = value;
    }
    function getX0() {
        return spectrogramSettingsGroup.x0;
    }

    function setX0(value) {
        //     spectrogramSettingsGroup.x0Slider = value;
    }
    function getY0() {
        return spectrogramSettingsGroup.y0;
    }

    function setY0(value) {
        //      spectrogramSettingsGroup.y0Slider = value;
    }
    function getHopLength() {
        return spectrogramSettingsGroup.hopLength;
    }

    function setHopLength(value) {
        //     spectrogramSettingsGroup.hopLengthSlider = value;
    }

    function getWindowLength() {
        return spectrogramSettingsGroup.windowLength;
    }

    function setWindowLength(value) {
        //   spectrogramSettingsGroup.windowLength = value;
    }

    function setFileDeltaTPerSample(value) {
        spectSettingsGroup.deltaTPerSample = value;
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: settingsTabBar
            Layout.fillWidth: true

            // TabButton {
            //     text: "Annotation"
            // }
            TabButton {
                text: "Features"
            }
            // TabButton {
            // text: "Training"
            // }
            // TabButton {
            //     text: "Decoder"
            // }
        }

        StackLayout {
            currentIndex: settingsTabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Annotation settings
            // ScrollView {
            //     clip: true
            //     ColumnLayout {
            //         width: root.width
            //         spacing: 8
            //         Label {
            //             text: "Annotation Settings"
            //             font.bold: true
            //             color: "#EEE"
            //             Layout.margins: 12
            //         }
            //         GroupBox {
            //             title: "General"
            //             Layout.fillWidth: true
            //             Layout.margins: 8
            //             ColumnLayout {
            //                 CheckBox {
            //                     text: "Auto-save on file change"
            //                     checked: true
            //                 }
            //                 CheckBox {
            //                     text: "Show threshold line"
            //                 }
            //                 CheckBox {
            //                     text: "Snap to threshold crossing"
            //                 }
            //             }
            //         }
            //     }
            // }

            // Features settings
            ScrollView {

                clip: true
                ColumnLayout {
                    width: root.width
                    spacing: 8
                    Label {
                        text: "Spectrogram Settings"
                        font.bold: true
                        color: "#EEE"
                        Layout.margins: 12
                    }

                    GroupBox {
                        id: spectrogramSettingsGroup
                        title: "Spectrogram"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        property var isDirty: false
                        property var deltaTPerSample: 20
                        property alias xScale: xScaleSlider.value
                        property alias yScale: yScaleSlider.value
                        property alias x0: x0Slider.value
                        property alias y0: y0Slider.value
                        property alias hopLength: hopLengthSlider.value
                        property alias windowLength: windowLengthSpinBox.value
                        function revertViewConfigToDefault() {
                            const xScaleDefault = 1.0;
                            const yScaleDefault = 1.0;
                            const x0Default = 0.0;
                            const y0Default = 0.0;
                            xScaleSlider.value = xScaleDefault;
                            yScaleSlider.value = yScaleDefault;
                            x0Slider.value = x0Default;
                            y0Slider.value = y0Default;
                            root.xScaleChanged(xScaleDefault);
                            root.yScaleChanged(yScaleDefault);
                            root.x0Changed(x0Default);
                            root.y0Changed(y0Default);
                        }
                        function revertRenderConfigToDefault() {
                            const hopSizeDefault = 8.0;
                            const windowSizeExpdefault = 8.0;

                            if (hopLengthSlider.value != hopSizeDefault) {
                                hopLengthSlider.value = hopSizeDefault;
                                root.hopLengthChanged(hopSizeDefault);
                            }
                            if (windowLengthSpinBox.value != windowSizeExpdefault) {
                                windowLengthSpinBox.value = windowSizeExpdefault;

                                root.windowLengthChanged(Math.pow(2, windowSizeExpdefault));
                            }
                        }
                        ColumnLayout {
                            Label {
                                text: "Xscale: " + Math.round(spectrogramSettingsGroup.xScale * 100) / 100
                                color: "#DDD"
                            }
                            Slider {
                                id: xScaleSlider
                                from: 0.1

                                value: 1.0
                                to: 5

                                onMoved: () => {
                                    xScaleChanged(value);
                                }
                            }
                            Label {
                                text: "Yscale: " + Math.round(spectrogramSettingsGroup.yScale * 100) / 100

                                color: "#DDD"
                            }
                            Slider {
                                id: yScaleSlider
                                from: 0.1
                                value: 1.0
                                to: 5
                                onMoved: () => {
                                    yScaleChanged(value);
                                }
                            }

                            Label {
                                text: "y0: " + Math.round(spectrogramSettingsGroup.y0 * 100) / 100
                                color: "#DDD"
                            }
                            Slider {
                                id: y0Slider
                                from: -1000
                                value: 1.0
                                to: 1000

                                onMoved: () => {
                                    y0Changed(value / yScaleSlider.value);
                                }
                            }

                            Label {
                                text: "x0: " + Math.round(spectrogramSettingsGroup.x0 * 100) / 100
                                color: "#DDD"
                            }
                            Slider {
                                id: x0Slider
                                from: -1000
                                value: 1.0
                                to: 1000
                                onMoved: () => {
                                    x0Changed(value / xScaleSlider.value);
                                }
                            }

                            Label {
                                text: "Window Length (2^x) = " + Math.round(spectrogramSettingsGroup.deltaTPerSample * Math.pow(2, windowLengthSpinBox.value) * 100000) / 100 + " ms"
                                color: "#DDD"
                            }
                            SpinBox {
                                id: windowLengthSpinBox
                                from: 4
                                to: 10
                                value: 8
                                stepSize: 1
                                onValueModified: () => {
                                    if (hopLengthSlider.value >= Math.pow(2, value)) {
                                        hopLengthSlider.setValue(Math.pow(2, value) - 1);
                                    }
                                    windowLengthChanged(Math.pow(2, value));
                                    spectrogramSettingsGroup.isDirty = true;
                                }
                            }

                            Label {
                                text: "Hop Length: " + Math.round(hopLengthSlider.value * spectrogramSettingsGroup.deltaTPerSample * 100000) / 100 + " ms"
                                color: "#DDD"
                            }
                            SpinBox {
                                id: hopLengthSlider
                                from: 1
                                to: {
                                    Math.pow(2, windowLengthSpinBox.value) - 1;
                                }
                                value: 8
                                stepSize: 1
                                onValueModified: () => {
                                    root.hopLengthChanged(value);
                                    spectrogramSettingsGroup.isDirty = true;
                                }
                            }
                            ComboBox {
                                id: windowFunction
                                model: ["Hanning", "Kaiser"]
                                onAccepted: root.windowFunctionChanged(currentText)
                            }

                            Button {
                                text: "Revert View To Default "
                                onClicked: spectrogramSettingsGroup.revertViewConfigToDefault()
                            }

                            Button {
                                text: "Revert Spectrogram Config to Default"
                                onClicked: spectrogramSettingsGroup.revertRenderConfigToDefault()
                            }

                            Button {
                                text: "Re Render Spectrogram"
                                onClicked: () => {
                                    root.reRenderRequest();
                                    spectrogramSettingsGroup.isDirty = false;
                                }
                                highlighted: spectrogramSettingsGroup.isDirty
                            }
                        }
                    }
                }
            }

            // Training settings
            // ScrollView {
            //     clip: true
            //     ColumnLayout {
            //         width: root.width
            //         spacing: 8
            //         Label {
            //             text: "Training Settings"
            //             font.bold: true
            //             color: "#EEE"
            //             Layout.margins: 12
            //         }
            //         GroupBox {
            //             title: "Model"
            //             Layout.fillWidth: true
            //             Layout.margins: 8
            //             ColumnLayout {
            //                 Label {
            //                     text: "Epochs"
            //                     color: "#DDD"
            //                 }
            //                 SpinBox {
            //                     from: 1
            //                     to: 1000
            //                     value: 100
            //                     Layout.fillWidth: true
            //                 }
            //                 Label {
            //                     text: "Learning Rate"
            //                     color: "#DDD"
            //                 }
            //                 TextField {
            //                     text: "0.001"
            //                     Layout.fillWidth: true
            //                 }
            //                 Button {
            //                     text: "Start Training"
            //                     Layout.fillWidth: true
            //                     highlighted: true
            //                 }
            //             }
            //         }
            //     }
            // }

            // Decoder settings
            // ScrollView {
            //     clip: true
            //     ColumnLayout {
            //         width: root.width
            //         spacing: 8
            //         Label {
            //             text: "Decoder Settings"
            //             font.bold: true
            //             color: "#EEE"
            //             Layout.margins: 12
            //         }
            //         GroupBox {
            //             title: "Decoder"
            //             Layout.fillWidth: true
            //             Layout.margins: 8
            //             ColumnLayout {
            //                 Label {
            //                     text: "Min Syllable Duration (ms)"
            //                     color: "#DDD"
            //                 }
            //                 SpinBox {
            //                     from: 5
            //                     to: 500
            //                     value: 20
            //                     Layout.fillWidth: true
            //                 }
            //                 Label {
            //                     text: "Min Silent Gap (ms)"
            //                     color: "#DDD"
            //                 }
            //                 SpinBox {
            //                     from: 1
            //                     to: 200
            //                     value: 10
            //                     Layout.fillWidth: true
            //                 }
            //                 Button {
            //                     text: "Run Decoder"
            //                     Layout.fillWidth: true
            //                     highlighted: true
            //                 }
            //             }
            //         }
            //     }
            // }
        }
    }
}
