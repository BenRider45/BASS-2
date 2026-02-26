import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: root
    clip: true

    ColumnLayout {
        width: root.availableWidth
        spacing: 12

        // --- Navigation ---
        GroupBox {
            title: "Navigation"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Button {
                        text: "◀ Prev"
                        onClicked: spectrogramController.prevPage()
                        Layout.fillWidth: true
                    }
                    Label {
                        text: spectrogramController.navPage + " / " + spectrogramController.navMax
                        horizontalAlignment: Text.AlignHCenter
                        Layout.fillWidth: true
                        color: "#DDD"
                    }
                    Button {
                        text: "Next ▶"
                        onClicked: spectrogramController.nextPage()
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    Button {
                        text: "Prev File (p)"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Next File (n)"
                        Layout.fillWidth: true
                    }
                }
            }
        }

        // --- Annotation Tools ---
        GroupBox {
            title: "Annotation"
            Layout.fillWidth: true

            GridLayout {
                columns: 2
                anchors.fill: parent

                Button {
                    text: "Set Boundary (Enter)"
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }
                Button {
                    text: "Remove (r)"
                    Layout.fillWidth: true
                }
                Button {
                    text: "Relabel (l)"
                    Layout.fillWidth: true
                }
                Button {
                    text: "Next Ann. (v)"
                    Layout.fillWidth: true
                }
                Button {
                    text: "Prev Ann. (x)"
                    Layout.fillWidth: true
                }
                Button {
                    text: "Adjust (g)"
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }
            }
        }

        // --- Cursor ---
        GroupBox {
            title: "Cursor"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                RowLayout {
                    Button { text: "◀ (a)"; Layout.fillWidth: true }
                    Button { text: "▶ (d)"; Layout.fillWidth: true }
                }
                RowLayout {
                    Label { text: "Step Size:"; color: "#DDD" }
                    SpinBox {
                        from: 1; to: 50; value: 5
                        Layout.fillWidth: true
                    }
                }
                RowLayout {
                    Button { text: "Prev Cross (z)"; Layout.fillWidth: true }
                    Button { text: "Next Cross (c)"; Layout.fillWidth: true }
                }
            }
        }

        // --- Contrast / Color Limits ---
        GroupBox {
            title: "Display"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent

                Label { text: "Contrast Min (vmin)"; color: "#DDD" }
                Slider {
                    from: 0; to: 1; value: spectrogramController.vmin
                    Layout.fillWidth: true
                    onMoved: spectrogramController.setVmin(value)
                }

                Label { text: "Contrast Max (vmax)"; color: "#DDD" }
                Slider {
                    from: 0; to: 1; value: spectrogramController.vmax
                    Layout.fillWidth: true
                    onMoved: spectrogramController.setVmax(value)
                }

                Label { text: "Threshold"; color: "#DDD" }
                Slider {
                    from: 0; to: 1; value: spectrogramController.threshold
                    Layout.fillWidth: true
                    onMoved: spectrogramController.setThreshold(value)
                }
            }
        }

        // --- Save ---
        Button {
            text: "Save (Ctrl+S)"
            Layout.fillWidth: true
            highlighted: true
            onClicked: annotationModel.save()
        }

        Item { Layout.fillHeight: true }
    }
}
