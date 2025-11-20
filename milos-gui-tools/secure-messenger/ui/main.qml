import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: "MilOS Secure Messenger"

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"

        TabBar {
            id: tabBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 40

            TabButton {
                text: "Key Management"
            }

            TabButton {
                text: "Messaging"
            }
        }

        StackLayout {
            anchors.top: tabBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            currentIndex: tabBar.currentIndex

            KeyManagement {
                anchors.fill: parent
            }

            MessagingInterface {
                anchors.fill: parent
            }
        }
    }
}

