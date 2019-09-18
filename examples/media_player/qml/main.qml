import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import QtMultimedia 5.13
import MediaProvider 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    RowLayout{
        anchors.fill: parent
        anchors.margins: 4
        anchors.bottomMargin: footer.height

        Rectangle{
            Layout.preferredWidth: 230
            Layout.fillHeight: true
            ColumnLayout{
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                Frame{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    padding: 0
                    ScrollView{
                        anchors.fill: parent
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        ListView{
                            anchors.fill: parent
                            anchors.margins: 4
                            model: backend.providers
                            clip: true
                            header: Label{
                                width: parent.width
                                text: "Providers:"
                                font.pixelSize: 15
                                horizontalAlignment: Qt.AlignHCenter
                            }
                            delegate: Button {
                                width: parent.width
                                text: modelData
                                onClicked: {
                                    backend.createProvider(modelData)
                                }
                            }
                        }
                    }
                }

                Frame{
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    visible: backend.provider ? true : false
                    padding: 0
                    ScrollView{
                        anchors.fill: parent
                        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                        ListView{
                            id: availableResources
                            property Provider provider: backend.provider
                            anchors.fill: parent
                            anchors.margins: 4
                            model: provider ? provider.availableResources : null
                            clip: true
                            header: Label{
                                width: parent.width
                                text: backend.provider ? backend.provider.origin : ""
                                font.pixelSize: 15
                                horizontalAlignment: Qt.AlignHCenter
                            }
                            delegate: Button {
                                width: parent.width
                                text: modelData
                                onClicked: {
                                    backend.createResource(modelData)
                                }
                            }
                        }
                    }
                }

                Frame{
                    id: res
                    Layout.fillWidth: true
                    Layout.preferredHeight: 130
                    visible: backend.resource ? true : false
                    padding: 2
                    GridLayout{
                        id: resGrid
                        anchors.fill: parent
                        anchors.margins: 2
                        columns: 2
                        Label{
                            text: "Resource"
                            font.pixelSize: 14
                        }
                        Label{
                            Layout.fillWidth: true
                            text: backend.resource ? backend.resource.resource : ""
                            font.pixelSize: 14
                            horizontalAlignment: Qt.AlignLeft
                            elide: Text.ElideRight
                            ToolTip.text: backend.resource ? backend.resource.resource : ""
                            ToolTip.visible: resLabelMouseArea.containsMouse
                            MouseArea{
                                id: resLabelMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                            }
                        }
                        Label{
                            text: "Resolution"
                            font.pixelSize: 14
                        }
                        ComboBox{
                            id: resolutions
                            Layout.fillWidth: true
                            model: backend.resource ? backend.resource.availableSizes : null
                            delegate: ItemDelegate{
                                width: parent.width
                                text: modelData.width + " x " + modelData.height
                                highlighted: parent.highlightedIndex === index
                            }
                            contentItem: Label{
                                verticalAlignment: Qt.AlignVCenter
                                horizontalAlignment: Qt.AlignHCenter
                                text: resolutions.currentIndex !== -1
                                      ? resolutions.model[resolutions.currentIndex].width + " x " + resolutions.model[resolutions.currentIndex].height
                                      : ""
                            }
                            onActivated: {
                                console.log("activated", index)
                                backend.resource.size = model[index]
                            }
                        }
                        Label{
                            text: "Format"
                            font.pixelSize: 14
                        }
                        ComboBox{
                            Layout.fillWidth: true
                            model: backend.resource ? backend.resource.availableColorFormats : null
                            onActivated: {
                                console.log("activated", index)
                                backend.resource.colorFormat = model[index]
                            }
                        }
                    }
                }
                Frame{
                    id: stream
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    padding: 2
                    visible: backend.resource && backend.resource.state === Resource.Initialised
                    Button{
                        id: startButton
                        anchors{
                            top: parent.top
                            left: parent.left
                            right: parent.right
                            margins: 2
                        }
                        text: "Start"
                        onClicked: {
                            backend.resource.stream.start()
                        }
                    }
                    Button{
                        id: stopButton
                        anchors{
                            top: startButton.bottom
                            left: parent.left
                            right: parent.right
                            margins: 2
                        }
                        text: "Stop"
                        onClicked: {
                            backend.resource.stream.stop()
                        }
                    }
                }
            }
        }
        Frame{
            Layout.fillWidth: true
            Layout.fillHeight: true
            VideoOutput{
                anchors.fill: parent
                source: backend
                fillMode: VideoOutput.PreserveAspectFit
            }
        }
    }

    Rectangle{
        id: footer
        anchors{
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 40
        color: "green"
    }
}
