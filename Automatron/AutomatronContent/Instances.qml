import QtQuick 6.6
import QtQuick.Controls 6.6
import QtQuick.Layouts
import Automatron
import Qt.labs.qmlmodels
import "Requests.js" as Requests
import "Utils.js" as Utils

Rectangle {
    id: instances
    width: Constants.width
    height: Constants.height

    FontMetrics {
        id: fontMetric
    }

    signal activated(variant pluginInfo)

    Connections {
        target: instancesView
        function onActivated(row, column) {
            let plugin = instancesView.index(row, 0).data("display")
            let name = instancesView.index(row, 1).data("display")
            let path = instancesView.index(row, 4).data("display")
            activated([plugin, name, path])
        }
    }

    GroupBox {
        anchors.fill: parent
        title: "Instances"

        Rectangle {
            anchors.fill: parent
            color: Application.styleHints.appearance === Qt.Light ? palette.mid : palette.midlight

            /*
            HorizontalHeaderView {
                id: horizontalHeader
                anchors.left: instancesView.left
                anchors.top: parent.top
                syncView: instancesView
                clip: true
            }
            */

            TableView {
                id: instancesView
                /*
                anchors.left: verticalHeader.right
                anchors.top: horizontalHeader.bottom
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width:parent.width
                */

                anchors.leftMargin: 1
                anchors.rightMargin: 1
                anchors.topMargin: 1
                anchors.bottomMargin: 1

                selectionMode: TableView.SingleSelection
                selectionBehavior: TableView.SelectRows
                keyNavigationEnabled: true
                selectionModel: ItemSelectionModel {}

                anchors.fill: parent
                clip: true
                columnSpacing: 1
                rowSpacing: 1
                alternatingRows: true

                signal activated(int row, int column)

                model: TableModel{
                    id: instancesModel

                    TableModelColumn { display: "name" }
                    TableModelColumn { display: "objectName" }
                    TableModelColumn { display: "type" }
                    TableModelColumn { display: "version" }
                    TableModelColumn { display: "settingsPath" }
                    TableModelColumn { display: "uuid" }
                    TableModelColumn { display: "author" }
                    TableModelColumn { display: "depends" }
                    TableModelColumn { display: "multipleInstances" }
                }

                columnWidthProvider: function (column) {

                    let ret = 50
                    for(let i = 0; i < model.rowCount; ++i){
                        let tmp = fontMetric.boundingRect(model.index(i, column).data("display")).width+2*fontMetric.averageCharacterWidth
                        if(tmp > ret){
                            ret = tmp
                        }
                    }

                    return ret;
                }


                delegate: Rectangle {
                    //required property bool containsDrag
                    required property bool current
                    required property bool selected

                    color: selected ? palette.highlight : (model.row % 2 === 0 ? palette.alternateBase : palette.base)

                    implicitWidth: 200
                    implicitHeight: Constants.font.pixelSize*2

                    Text {
                        text: display
                        anchors.centerIn: parent
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: function(){
                            let tableView = parent.parent.parent
                            tableView.activated(model.row, model.column)
                        }
                    }
                }


            }

            Rectangle {
                id: fakeOverlay
                anchors.fill: parent
                color: 'grey'

                BusyIndicator {
                    id: wheel
                    running: true
                    padding:0
                    anchors.centerIn: parent
                }

                Label {
                    id: statusLabel
                    anchors.centerIn: parent
                    visible: false
                }
            }



            Component.onCompleted: function() {
                fakeOverlay.visible = true

                Requests.sendGetRequest("http://127.0.0.1:1234/", instancesModel, function(response, context) {
                    if (response.readyState === 4 && response.status === 200) {
                        let objects = JSON.parse(response.content)
                        for (const [key, value] of Object.entries(objects)) {
                          if(Utils.isObject(value)){
                            context.appendRow(value)
                          }
                        }
                        fakeOverlay.visible = false
                    } else {
                        wheel.visible = false
                        statusLabel.visible = true
                        statusLabel.text = qsTr(`Error: failed to fetch remote resource`)
                    }
                })
            }
        }
    }
}
