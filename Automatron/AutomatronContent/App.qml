// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick 6.6
import QtQuick.Controls 6.6
import QtQuick.Layouts
import "Requests.js" as Requests
import "Utils.js" as Utils
import Automatron

ApplicationWindow {
    width: Constants.width
    height: Constants.height
    id: window
    visible: true
    title: "Automatron"

    /*QCustomAction {
        id: customActionSettings
        color: window.color
        width:window.width
    }*/

    header: ToolBar {
        contentHeight: toolButton.implicitHeight
        RowLayout{
            anchors.fill: parent
            anchors.right: parent.right
            anchors.left: parent.left

            ToolButton {
                id: toolButton
                text: "\u2630"
                font:  Constants.largeFont
                onClicked: {
                    if(navDrawer.opened === true){
                        navDrawer.close()

                    } else {
                        navDrawer.open()

                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                id: btnUsuario
                    //icon.source: "qrc:/icons/icons/ic_bullet_menu.svg"
                text: "\u22ee"
                font:  Constants.hugeFont
            }
        }
    }

    Drawer {
        y: header.height
        width: window.width * 0.6
        height: window.height - header.height
        id: navDrawer

        ColumnLayout {
            spacing:0
            width: parent.width

            DrawerDelegate{
                text: qsTr("Settings")
                Layout.fillWidth: true
                normalColor: navDrawer.background.color
                hoverColor: navDrawer.palette.mid

                onClicked: {
                    stackView.push("Settings.qml")
                    navDrawer.close()
                    row.visible = true
                }
            }

            DrawerDelegate{
                text: qsTr("Quit")
                Layout.fillWidth: true
                normalColor: navDrawer.background.color
                hoverColor: navDrawer.palette.mid

                onClicked: {
                    Qt.quit()
                }
            }
        }

        onOpened: {
            toolButton.text = "\u2717"
        }

        onClosed: {
            toolButton.text = "\u2630"
        }
    }

    Instances {
        id: instances
        color: window.color
        width: window.width
        height: window.height-row.height-header.height
    }

    StackView {
        id: stackView
        initialItem: instances
        width: window.width
        height: window.height-row.height-header.height
    }

    Connections {
        target: instances
        function onActivated(data){
            var component = Qt.createComponent(data[0]+".qml")
            var name = data[0]
            if(component.status === Component.Null || component.status === Component.Error){
                var d = Qt.createQmlObject(`
                    import QtQuick.Controls
                    import QtQuick.Dialogs
                    import Qt.labs.platform
                    MessageDialog {
                        id: dialog
                        title: "Error loading component"
                        text: "Failed to load component"
                        informativeText: "Component: '${name}' not found"
                        buttons: MessageDialog.Ok
                        modality: Qt.WindowModal
                    }`, window)
                d.open()
                return
            }

            var o = component.createObject(stackView, {
                color: window.color,
                width: window.width,
                height: window.height-row.height,
                visible: false
            })

            var oname = data[1]

            Requests.sendGetRequest(`http://127.0.0.1:1234/${oname}/settings`, o, function(response, context) {
                let object = JSON.parse(response.content)
                o.settingsData = object
            })


            stackView.push(o)
        }

    }

    Row {
        id: row
        visible: false
        width: parent.width
        anchors.bottom: parent.bottom
        spacing: 4
        layoutDirection: Qt.RightToLeft
        padding: Constants.font.pixelSize/2

        Button {
            id: cancelButton
            text: qsTr("Cancel")
            width: Constants.font.pixelSize*8
        }

        Button {
            id: applyButton
            text: qsTr("Apply")
            width: Constants.font.pixelSize*8
        }

        Button {
            id: okButton
            text: qsTr("Ok")
            width: Constants.font.pixelSize*8
        }
    }
}

