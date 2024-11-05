import QtQuick 6.7
import QtQuick.Controls 6.7

ItemDelegate {
  //  property string targetPage
    property color normalColor
    property color hoverColor

    width: parent.width

//    onClicked: {
//        activated()
//    }

    hoverEnabled: true
    background: Rectangle {
        color: normalColor
    }

    onHoveredChanged: {
        if(hovered === true){
            background.color = hoverColor
        } else {
            background.color = normalColor
        }
    }
}
