import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "MilOS Secure Communication Hub"
    
    color: "#0a0a0a"
    
    TabView {
        anchors.fill: parent
        anchors.margins: 20
        
        Tab {
            title: "Email"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Encrypted Email"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Integrated with Encrypted Email Client"
                    font.pixelSize: 14
                    color: "#aaaaaa"
                }
            }
        }
        
        Tab {
            title: "Chat"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Encrypted Chat"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Integrated with Secure Messenger"
                    font.pixelSize: 14
                    color: "#aaaaaa"
                }
            }
        }
        
        Tab {
            title: "Voice/Video"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Voice & Video Communication"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Integrated with Secure Messenger Media Calls"
                    font.pixelSize: 14
                    color: "#aaaaaa"
                }
            }
        }
        
        Tab {
            title: "Groups"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Group Communication"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#ffffff"
                }
                
                Text {
                    text: "Unread Messages: " + communicationHub.unreadCount
                    font.pixelSize: 14
                    color: "#44ff44"
                }
            }
        }
    }
}

