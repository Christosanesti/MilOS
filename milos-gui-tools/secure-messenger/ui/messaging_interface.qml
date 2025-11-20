import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: messagingInterface
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        // Conversation list
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: "#2b2b2b"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10
                
                Text {
                    text: "Conversations"
                    font.pixelSize: 18
                    font.bold: true
                    color: "#ffffff"
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: conversationList
                        model: conversationManager ? conversationManager.getConversationsForUser("") : []
                        delegate: Rectangle {
                            width: conversationList.width
                            height: 60
                            color: mouseArea.containsMouse ? "#3b3b3b" : "#2b2b2b"
                            
                            MouseArea {
                                id: mouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: {
                                    // Select conversation
                                }
                            }
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                
                                Text {
                                    text: modelData.title || "Conversation"
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: "#ffffff"
                                }
                                
                                Text {
                                    text: modelData.lastMessageAt || ""
                                    font.pixelSize: 12
                                    color: "#aaaaaa"
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Message view
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0
            
            // Message list
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ListView {
                    id: messageList
                    model: []
                    delegate: Rectangle {
                        width: messageList.width
                        height: messageText.height + 20
                        color: "#1e1e1e"
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            
                            Text {
                                text: modelData.senderId || "Unknown"
                                font.pixelSize: 12
                                color: "#888888"
                            }
                            
                            Text {
                                id: messageText
                                text: modelData.content || ""
                                font.pixelSize: 14
                                color: "#ffffff"
                                wrapMode: Text.Wrap
                                Layout.fillWidth: true
                            }
                            
                            Text {
                                text: modelData.timestamp || ""
                                font.pixelSize: 10
                                color: "#666666"
                            }
                        }
                    }
                }
            }
            
            // Message input
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                color: "#2b2b2b"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    
                    TextArea {
                        id: messageInput
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        placeholderText: "Type a message..."
                        color: "#ffffff"
                        background: Rectangle {
                            color: "#1e1e1e"
                            border.color: "#444444"
                            border.width: 1
                            radius: 4
                        }
                    }
                    
                    Button {
                        text: "Send"
                        Layout.alignment: Qt.AlignRight
                        onClicked: {
                            if (messageInput.text.trim() !== "") {
                                // Send message
                                messageInput.text = "";
                            }
                        }
                    }
                }
            }
        }
    }
}

