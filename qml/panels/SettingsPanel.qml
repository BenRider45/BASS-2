import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: settingsTabBar
            Layout.fillWidth: true

            TabButton { text: "Annotation" }
            TabButton { text: "Features" }
            TabButton { text: "Training" }
            TabButton { text: "Decoder" }
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
                    Label { text: "Annotation Settings"; font.bold: true; color: "#EEE"; Layout.margins: 12 }
                    GroupBox {
                        title: "General"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            CheckBox { text: "Auto-save on file change"; checked: true }
                            CheckBox { text: "Show threshold line" }
                            CheckBox { text: "Snap to threshold crossing" }
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
                    Label { text: "Feature Settings"; font.bold: true; color: "#EEE"; Layout.margins: 12 }
                    GroupBox {
                        title: "Spectrogram"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            Label { text: "FFT Size"; color: "#DDD" }
                            ComboBox { model: ["256", "512", "1024", "2048"]; currentIndex: 2; Layout.fillWidth: true }
                            Label { text: "Hop Length"; color: "#DDD" }
                            SpinBox { from: 64; to: 1024; value: 512; stepSize: 64; Layout.fillWidth: true }
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
                    Label { text: "Training Settings"; font.bold: true; color: "#EEE"; Layout.margins: 12 }
                    GroupBox {
                        title: "Model"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            Label { text: "Epochs"; color: "#DDD" }
                            SpinBox { from: 1; to: 1000; value: 100; Layout.fillWidth: true }
                            Label { text: "Learning Rate"; color: "#DDD" }
                            TextField { text: "0.001"; Layout.fillWidth: true }
                            Button { text: "Start Training"; Layout.fillWidth: true; highlighted: true }
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
                    Label { text: "Decoder Settings"; font.bold: true; color: "#EEE"; Layout.margins: 12 }
                    GroupBox {
                        title: "Decoder"
                        Layout.fillWidth: true
                        Layout.margins: 8
                        ColumnLayout {
                            Label { text: "Min Syllable Duration (ms)"; color: "#DDD" }
                            SpinBox { from: 5; to: 500; value: 20; Layout.fillWidth: true }
                            Label { text: "Min Silent Gap (ms)"; color: "#DDD" }
                            SpinBox { from: 1; to: 200; value: 10; Layout.fillWidth: true }
                            Button { text: "Run Decoder"; Layout.fillWidth: true; highlighted: true }
                        }
                    }
                }
            }
        }
    }
}
