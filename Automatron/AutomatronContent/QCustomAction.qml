import QtQuick 6.6
import QtQuick.Controls 6.6
import QtQuick.Layouts
import Automatron

Rectangle {
    id: customActionSettings

    property variant settingsData

    onSettingsDataChanged: function(){
        for(let i = 0;  i < grid.children.length; ++i){
            if(grid.children[i].objectName !== null && grid.children[i].objectName.length > 0){
                if(settingsData.hasOwnProperty(grid.children[i].objectName)){

                }
            }
        }
    }

    GroupBox {
        title: "QCustomAction/Settings"
        anchors.fill: parent

        GridLayout {
            id: grid
            columns: 2
            width: parent.width

            Label {
                text: qsTr("Title")
            }

            TextField {
                id: title
                objectName: 'title'
                placeholderText: qsTr("Title")
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Button Text")
            }

            TextField {
                id: buttonText
                objectName: 'buttonText'
                placeholderText: qsTr("Button Text")
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Interval")
            }

            SpinBox {
                id: interval
                objectName: 'interval'
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }

            Label {
                text: qsTr("Progress")
            }

            Switch {
                id: progress
                objectName: 'progress'
                text: qsTr("")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }

            Label {
                text: qsTr("Verbose")
            }

            Switch {
                id: verbose
                objectName: 'verbose'
                text: qsTr("")
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
        }
    }
}
