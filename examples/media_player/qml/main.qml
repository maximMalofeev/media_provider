import QtQuick 2.12
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.12
import QtMultimedia 5.13
import MediaProvider 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    SplitView{
        anchors.fill: parent
        anchors.margins: 4
        anchors.bottomMargin: footer.height

        Rectangle{
            SplitView.preferredWidth: 230
            SplitView.fillHeight: true
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
                                Rectangle{
                                    anchors.right: parent.right
                                    anchors.rightMargin: 4
                                    radius: 10
                                    color: backend.provider.state === Provider.Initialising ? "blue" : backend.provider.state === Provider.Initialised ? "green" : "red"
                                    width: parent.height - 2
                                    height: parent.height - 2
                                    ToolTip.text: backend.provider.errorString
                                    MouseArea{
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onHoveredChanged: {
                                           if(containsMouse && backend.provider.errorString.length) {
                                               parent.ToolTip.visible = true
                                           }
                                        }
                                    }
                                }
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
                            MouseArea{
                                id: resLabelMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                            }
                            Rectangle{
                                anchors.right: parent.right
                                anchors.rightMargin: 4
                                radius: 10
                                color: backend.resource.state === Resource.Initialising ? "blue" : backend.resource.state === Resource.Initialised ? "green" : "red"
                                width: parent.height - 2
                                height: parent.height - 2
                                ToolTip.text: backend.resource.errorString
                                MouseArea{
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onHoveredChanged: {
                                       if(containsMouse && backend.resource.errorString.length) {
                                           parent.ToolTip.visible = true
                                       }
                                    }
                                }
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
                                horizontalAlignment: Qt.AlignLeft
                                leftPadding: 10
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
            }
        }
        Frame{
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            VideoOutput{
                anchors.fill: parent
                anchors.bottomMargin: stream.height
                source: backend
                fillMode: VideoOutput.PreserveAspectFit
            }
            Frame{
                id: stream
                anchors{
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                padding: 2
                visible: backend.resource
                Row{
                    anchors.centerIn: parent
                    spacing: 2
                    Rectangle{
                        radius: 10
                        color: backend.resource.stream.state === Stream.Stopped ? "blue" : backend.resource.stream.state === Stream.Playing ? "green" : "red"
                        width: 20
                        height: 20
                        ToolTip.text: backend.resource.stream.errorString
                        MouseArea{
                            anchors.fill: parent
                            hoverEnabled: true
                            onHoveredChanged: {
                               if(containsMouse && backend.resource.stream.errorString.length) {
                                   parent.ToolTip.visible = true
                               }
                            }
                        }
                    }
                    Button{
                        id: startButton
                        text: "Start"
                        onClicked: {
                            backend.resource.stream.start()
                        }
                    }
                    Button{
                        id: stopButton
                        text: "Stop"
                        onClicked: {
                            backend.resource.stream.stop()
                        }
                    }
                }
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
