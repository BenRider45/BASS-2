import QtQuick
import QtQuick.Controls

Dialog {
    id: projectLoadConfirmationDialog
    title: "Confirm loading project"
    width: 400
    height: 300
    modal: true

    signal confirmProjectLoad()


    Text {
        text: "Are you sure you want to continue?"
    }


    standardButtons: Dialog.Ok | Dialog.Cancel


    onAccepted: () => {
                    console.log("User clicked OK")
                    confirmProjectLoad();

                }
    onRejected: console.log("User clicked Cancel")




}
