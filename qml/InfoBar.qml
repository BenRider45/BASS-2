import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Constants 1.0

Rectangle {
    id: root
    color: "#1e1e1e"
    implicitHeight: 28
    property color boundaryColor: "#44FF44"
    property string leftText: projectManager.projectAttached ? projectManager.currentProjectBirdName + " — " + projectManager.currentProjectDir : "No project loaded"
    property string currentFileName: "No file Selected"
    //    property string rightText: "Annotations: " + annotationModel.count + "  |  Page: " + spectrogramController.navPage + "/" + spectrogramController.navMax
    // RowLayout {
    //
    //     Label {
    //         text: root.leftText
    //         color: "#BBBBBB"
    //         font.pixelSize: 12
    //         elide: Text.ElideRight
    //         Layout.fillWidth: true
    //     }

    // Label {
    //     text: root.rightText
    //     color: "#999999"
    //     font.pixelSize: 12
    //     horizontalAlignment: Text.AlignRight
    // }
    ColumnLayout {
        anchors.fill: parent
        RowLayout {
            spacing: 20
            Layout.margins: 20
            Layout.fillWidth: true
            implicitHeight: 50

            Label {
                Layout.fillWidth: true

                text: "Syllable:"
                color: "#FFFFFF"
            }
            Rectangle {
                width: 100
                color: "steelblue"
            }
            Label {
                Layout.fillWidth: true
                text: "File:"
                color: "#FFFFFF"
            }
        }
        TableView {
          id: tableView
            columnSpacing: 2
            rowSpacing: 2
            clip: true
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true
            Layout.fillHeight: true 
            Layout.margins: 20
            model: projectManager.annotationsModel
            delegate: Rectangle {
              implicitHeight: 50
              implicitWidth:tableView.width 

              radius: 10 
              RowLayout{
                anchors.fill: parent
                
                Text {
                  Layout.fillWidth: true
                  Layout.fillHeight:true
                  leftPadding: 20
                  text: model.label
                  verticalAlignment: Text.AlignVCenter 
               }


                Text{
                  Layout.fillWidth: true
                  text: model.fileName
                  rightPadding: 20
                  verticalAlignment: Qt.AlignVCenter 
                  horizontalAlignment: Text.AlignRight
                  }

                }

            
            }
        }

        // ListView {
        //     id: annotationList
        //     model: projectManager.annotationsModel
        //     clip: true
        //     currentIndex: 0
        //     Layout.fillWidth: true
        //     Layout.fillHeight: true
        //     delegate: ItemDelegate {
        //         width: annotationList.width
        //         height: 60
        //         contentItem: Rectangle {
        //             color: model.fileName == root.currentFileName ? "#28B5F5" : "#9DDCF9"
        //             RowLayout {
        //
        //                 Layout.margins: 40
        //                 anchors.fill: parent
        //                 Label {
        //                   text: model.label
        //                     Layout.fillWidth:true
        //                     // margin: 15
        //                     color: "#000000"
        //                   }
        //
        //                 Label{
        //                   text: model.fileName
        //
        //                     Layout.fillWidth:true
        //                     color: "#000000"
        //                 }
        //
        //
        //             }
        //         }
        //     }
        //
        //     ScrollBar.vertical: ScrollBar {}
        // }
    }
    // Top border
}
