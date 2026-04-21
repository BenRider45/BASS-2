import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal xScaleChanged(int value)
    signal yScaleChanged(int value)
    signal x0Changed(int value)
    signal y0Changed(int value)
    signal hopLengthChanged(int value)
    function getxScale() {
        return settingsGroup.xScale;
    }

    function getyScale() {
        return settingsGroup.yScale;
    }

    function getX0() {
        return settingsGroup.x0;
    }
    function getY0() {
        return settingsGroup.y0;
    }
    function getHopLength() {
        return settingsGroup.hopLength;
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: settingsTabBar
            Layout.fillWidth: true

            TabButton {
                text: "Annotation"
            }
            TabButton {
                text: "Features"
            }
            TabButton {
                text: "Training"
            }
            TabButton {
                text: "Decoder"
            }
        }

        StackLayout {
            currentIndex: settingsTabBar.currentIndex
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Annotation settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: root.width
                    spacing: 8
                    Label {
                        text: "Annotation Settings"
                        font.bold: true
                        color: "#EEE"
                        Layout.margins: 12
                    }
                    GroupBox {
                        title: "General"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            CheckBox {
                                text: "Auto-save on file change"
                                checked: true
                            }
                            CheckBox {
                                text: "Show threshold line"
                            }
                            CheckBox {
                                text: "Snap to threshold crossing"
                            }
                        }
                    }
                }
            }

            // Features settings
            ScrollView {

                clip: true
                ColumnLayout {
                    width: root.width
                    spacing: 8
                    Label {
                        text: "Feature Settings"
                        font.bold: true
                        color: "#EEE"
                        Layout.margins: 12
                    }

                    GroupBox {
                        id: settingsGroup
                        title: "Spectrogram"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        property alias xScale: xScaleSlider.value
                        property alias yScale: yScaleSlider.value
                        property alias x0: x0Slider.value
                        property alias y0: y0Slider.value
                        property alias hopLength: hopLengthSlider.value
                        ColumnLayout {
                            Label {
                                text: "Xscale"
                                color: "#DDD"
                            }
                            Slider {
                                id: xScaleSlider
                                from: 0.1
                                value: 1
                                to: 10
                                Layout.fillWidth: true
                                onMoved: () => {
                                    xscaleChanged(value);
                                }
                            }
                            Label {
                                text: "Yscale"
                                color: "#DDD"
                            }
                            Slider {
                                id: yScaleSlider
                                from: 0.1
                                value: 1
                                to: 10
                                Layout.fillWidth: true
                                onMoved: () => {
                                    yscaleChanged(value);
                                }
                            }
                            RowLayout {
                                Label {
                                    text: "y0"
                                    color: "#DDD"
                                }
                                Slider {
                                    id: y0Slider
                                    from: 0.1
                                    value: 1
                                    to: 10
                                    Layout.fillWidth: true
                                    onMoved: () => {
                                        y0Changed(value);
                                    }
                                }

                                Label {
                                    text: "x0"
                                    color: "#DDD"
                                }
                                Slider {
                                    id: x0Slider
                                    from: 0.1
                                    value: 1
                                    to: 10
                                    onMoved: () => {
                                        x0Changed(value);
                                    }
                                }
                            }

                            Label {
                                text: "Hop Length"
                                color: "#DDD"
                            }
                            SpinBox {
                                id: hopLengthSlider
                                from: 64
                                to: 1024
                                value: 512
                                stepSize: 64
                                Layout.fillWidth: true
                                onValueModified: () => {
                                    hopLengthChanged(value);
                                }
                            }
                        }
                    }
                }
            }

            // Training settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: root.width
                    spacing: 8
                    Label {
                        text: "Training Settings"
                        font.bold: true
                        color: "#EEE"
                        Layout.margins: 12
                    }
                    GroupBox {
                        title: "Model"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            Label {
                                text: "Epochs"
                                color: "#DDD"
                            }
                            SpinBox {
                                from: 1
                                to: 1000
                                value: 100
                                Layout.fillWidth: true
                            }
                            Label {
                                text: "Learning Rate"
                                color: "#DDD"
                            }
                            TextField {
                                text: "0.001"
                                Layout.fillWidth: true
                            }
                            Button {
                                text: "Start Training"
                                Layout.fillWidth: true
                                highlighted: true
                            }
                        }
                    }
                }
            }

            // Decoder settings
            ScrollView {
                clip: true
                ColumnLayout {
                    width: root.width
                    spacing: 8
                    Label {
                        text: "Decoder Settings"
                        font.bold: true
                        color: "#EEE"
                        Layout.margins: 12
                    }
                    GroupBox {
                        title: "Decoder"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            Label {
                                text: "Min Syllable Duration (ms)"
                                color: "#DDD"
                            }
                            SpinBox {
                                from: 5
                                to: 500
                                value: 20
                                Layout.fillWidth: true
                            }
                            Label {
                                text: "Min Silent Gap (ms)"
                                color: "#DDD"
                            }
                            SpinBox {
                                from: 1
                                to: 200
                                value: 10
                                Layout.fillWidth: true
                            }
                            Button {
                                text: "Run Decoder"
                                Layout.fillWidth: true
                                highlighted: true
                            }
                        }
                    }
                }
            }
        }
    }
}
