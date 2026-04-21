import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal xScaleChanged(double value)
    signal yScaleChanged(double value)
    signal x0Changed(double  value)
    signal y0Changed(double value)
    signal windowLengthChanged(int value)
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

      function getWindowLength() {
        return settingsGroup.windowLength;
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
                        property alias windowLength: windowLengthSpinBox.value
                        ColumnLayout {
                            Label {
                                text: "Xscale"
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
                                text: "Yscale"
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
                                    text: "y0"
                                    color: "#DDD"
                                }
                                Slider {
                                    id: y0Slider
                                    from: -1000
                                    value: 1.0
                                    to: 1000

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
                                    from: -1000
                                    value: 1.0
                                    to: 1000
                                    onMoved: () => {
                                        x0Changed(value);
                                    }
                                }
                          

                            Label {
                                text: "Window Length (2^x)"
                                color: "#DDD"
                              }
                            SpinBox {
                                id: windowLengthSpinBox
                                from: 4
                                to: 10
                                value: 8
                                stepSize: 1
                                onValueModified: () => {
                                  if(hopLengthSlider.value >= Math.pow(2,value)){
                                    hopLengthSlider.setValue(Math.pow(2,value)-1);

                                  }
                                  windowLengthChanged(Math.pow(2, value));

                                }
                              }

                            Label {
                              text: "Hop Length: " + hopLengthSlider.value
                              color: "#DDD"
                            }
                            SpinBox{
                              id: hopLengthSlider
                              from: windowLengthSpinBox.value <= 4 ? 1 : 8
                              to: {Math.pow(2, windowLengthSpinBox.value) - 1}
                              value: 8
                              stepSize: 1
                              onValueModified: () => {hopLengthChanged(value);}

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
