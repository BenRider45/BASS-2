import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        Label {
            text: "Files"
            font.pixelSize: 16
            font.bold: true
            color: "#EEE"
        }

        // Page navigation
        RowLayout {
            Layout.fillWidth: true

            Button {
                text: "◀"
                enabled: fileListModel.currentPage > 0
                onClicked: fileListModel.currentPage = fileListModel.currentPage - 1
            }
            Label {
                text: "Page " + (fileListModel.currentPage + 1) + " / " + fileListModel.pageCount
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                color: "#DDD"
            }
            Button {
                text: "▶"
                enabled: fileListModel.currentPage < fileListModel.pageCount - 1
                onClicked: fileListModel.currentPage = fileListModel.currentPage + 1
            }
        }

        // File list
        ListView {
            id: fileList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: fileListModel
            clip: true
            currentIndex: fileListModel.currentIndex

            delegate: ItemDelegate {
                width: fileList.width
                height: 44
                highlighted: index === fileListModel.currentIndex

                contentItem: RowLayout {
                    spacing: 8
                    Label {
                        text: model.fileName || ""
                        color: highlighted ? "#FFF" : "#CCC"
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }
                    Label {
                        text: model.annotationCount !== undefined ? model.annotationCount.toString() : "0"
                        color: "#999"
                        font.pixelSize: 11
                    }
                }

                onClicked: fileListModel.selectFile(index)
            }

            ScrollBar.vertical: ScrollBar {}
        }

        Label {
            text: fileListModel.count + " files"
            color: "#999"
            font.pixelSize: 11
        }
    }
}
