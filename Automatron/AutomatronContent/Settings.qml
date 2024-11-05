import QtQuick 6.6
import QtQuick.Controls 6.6
import QtQuick.Layouts
import Automatron

Rectangle {
    id: settings

    GroupBox {
        title: "Automatron/Settings"
        anchors.fill: parent

        GridLayout {
            id: grid
            columns: 2
            width: parent.width

            Label {
                text: qsTr("URL")
            }

            TextField {
                id: automatronUrl
                objectName: 'automatronUrl'
                placeholderText: qsTr("URL")
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("API Key")
            }

            TextField {
                id: apiKey
                objectName: 'apiKey'
                placeholderText: qsTr("API Key")
                Layout.fillWidth: true
            }
        }
    }
}
