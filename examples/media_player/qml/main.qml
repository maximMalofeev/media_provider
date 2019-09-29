import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtMultimedia 5.13
import MediaProvider 1.0

ApplicationWindow {
    id: root
    minimumWidth: 680
    width: 680
    height: 570
    minimumHeight: 570
    flags: Qt.Window
    title: qsTr("Media Player")
    visible: true

    header: ToolBar{
        RowLayout{
            anchors.left: parent.left
            ToolButton{
                text: "Video"
                onClicked: {
                    videoSettingsDrawer.open()
                }
            }
        }
    }

    Drawer{
        id: videoSettingsDrawer
        width: 350
        height: parent.height

        BusyIndicator{
            id: videoSettingsBusyIndicator
            anchors.centerIn: parent
            width: 70
            height: width
            running: false
        }

        ColumnLayout{
            id: videoSettingsLayout
            spacing: 5
            anchors{
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 5
            }
            clip: true
            enabled: !videoSettingsBusyIndicator.running
            states: [
                State{
                    name: "HAS PROVIDER"
                    PropertyChanges { target: availableProviders; visible: false }
                    PropertyChanges { target: availableResources; visible: true }
                    PropertyChanges { target: currentProviderRow; visible: true }
                    PropertyChanges { target: currentProvider; text: backend.provider.provider }
                    PropertyChanges { target: providerOriginRow; visible: true }
                    PropertyChanges { target: providerOrigin; text: backend.provider.origin }
                    PropertyChanges { target: availableResourcesList; model: backend.provider.availableResources }
                    PropertyChanges { target: videoSettingsBusyIndicator; running: backend.provider.state === Provider.Initialising }
                    PropertyChanges { target: providerErrorState; when: backend.provider.state === Provider.Invalid }
                },
                State{
                    id: providerErrorState
                    name: "PROVIDER ERROR"
                    extend: "HAS PROVIDER"
                    PropertyChanges { target: providerErrorRow; visible: true }
                    PropertyChanges { target: providerError; text: backend.provider.errorString; color: "red" }
                    PropertyChanges { target: availableResourcesList; model: null }
                    PropertyChanges { target: applyVideoSettingsButton; text: "Ok"; onClicked: {backend.resetProvider(); videoSettingsLayout.state = ""} }
                },
                State{
                    name: "HAS RESOURCE"
                    extend: "HAS PROVIDER"
                    PropertyChanges { target: availableResources; visible: false }
                    PropertyChanges { target: resourceNameRow; visible: true }
                    PropertyChanges { target: resourceName; text: backend.resource.resource }
                    PropertyChanges { target: resourceParametersLayout; visible: true; }
                    PropertyChanges { target: resolutions; model: backend.resource.availableSizes; }
                    PropertyChanges { target: formats; model: backend.resource.availableColorFormats; }
                    PropertyChanges { target: applyVideoSettingsButton; visible: true; }
                    PropertyChanges { target: videoSettingsBusyIndicator; running: backend.resource.state === Resource.Initialising }
                    PropertyChanges { target: resourceErrorState; when: backend.resource.state === Resource.Invalid }
                },
                State{
                    id: resourceErrorState
                    name: "RESOURCE ERROR"
                    extend: "HAS RESOURCE"
                    PropertyChanges { target: resourceErrorRow; visible: true }
                    PropertyChanges { target: resourceError; text: backend.resource.errorString; color: "red" }
                    PropertyChanges { target: resourceParametersLayout; visible: false; }
                    PropertyChanges { target: applyVideoSettingsButton; text: "Ok"; onClicked: {videoSettingsLayout.state = "HAS PROVIDER"} }
                },
                State{
                    name: "APPLIED"
                    extend: "HAS RESOURCE"
                    PropertyChanges { target: applyVideoSettingsButton; visible: false; }
                    PropertyChanges { target: resourceParametersLayout; enabled: false; }
                    PropertyChanges {
                        target: streamStatus;
                        color: backend.resource.stream.state === Stream.Stopped ? "blue" :
                                                                                  backend.resource.stream.state === Stream.Playing ? "green" : "red";
                    }
                }

            ]

            Label{
                Layout.fillWidth: true
                Layout.bottomMargin: 5
                horizontalAlignment: Qt.AlignHCenter
                text: qsTr("Video Settings")
                ToolButton{
                    id: resetProviderButton
                    anchors.right: parent.right
                    height: parent.height
                    text: "Reset"
                    onClicked: {
                        videoSettingsLayout.state = ""
                        backend.resetProvider()
                    }
                }
            }

            Frame{
                id: availableProviders
                Layout.fillWidth: true
                Layout.preferredHeight: availableProvidersList.contentHeight + availableProvidersList.anchors.margins * 2
                padding: 0
                ListView{
                    id: availableProvidersList
                    anchors.fill: parent
                    anchors.margins: 4
                    model: backend.availableProviders()
                    clip: true
                    header: Label{
                        width: parent.width
                        text: qsTr("Providers:")
                        font.pixelSize: 15
                        horizontalAlignment: Qt.AlignHCenter
                    }
                    delegate: Button {
                        width: parent.width
                        text: modelData
                        onClicked: {
                            backend.createProvider(modelData)
                            videoSettingsLayout.state = "HAS PROVIDER"
                        }
                    }
                }
            }

            Row{
                id: currentProviderRow
                Layout.fillWidth: true
                visible: false
                spacing: 5
                Label{
                    text: qsTr("Provider:")
                }
                Label{
                    id: currentProvider
                    width: 300
                    elide: Text.ElideLeft
                }
            }
            Row{
                id: providerOriginRow
                Layout.fillWidth: true
                visible: false
                spacing: 5
                Label{
                    text: qsTr("Origin:")
                }
                Label{
                    id: providerOrigin
                    width: 300
                    elide: Text.ElideLeft
                }
            }
            Row{
                id: providerErrorRow
                Layout.fillWidth: true
                visible: false
                spacing: 5
                Label{
                    text: qsTr("Error:")
                    color: "red"
                }
                Label{
                    id: providerError
                    width: 300
                    elide: Text.ElideLeft
                }
            }

            Frame{
                id: availableResources
                Layout.fillWidth: true
                Layout.preferredHeight: availableResourcesList.contentPlusMargin < 300 ? availableResourcesList.contentPlusMargin : 300
                padding: 0
                visible: availableResourcesList.model ? true : false

                ScrollView{
                    anchors.fill: parent
                    ListView{
                        property int contentPlusMargin: contentHeight + anchors.margins * 2
                        id: availableResourcesList
                        width: parent.width
                        anchors.margins: 4
                        clip: true

                        header: Label{
                            width: parent.width
                            text: qsTr("Resources:")
                            font.pixelSize: 15
                            horizontalAlignment: Qt.AlignHCenter
                        }
                        delegate: Button {
                            width: parent.width
                            text: modelData
                            onClicked: {
                                backend.createResource(modelData)
                                videoSettingsLayout.state = "HAS RESOURCE"
                            }
                        }
                    }
                }
            }

            Row{
                id: resourceNameRow
                Layout.fillWidth: true
                visible: false
                spacing: 5
                Label{
                    text: qsTr("Resource:")
                }
                Label{
                    id: resourceName
                    elide: Text.ElideLeft
                    width: 270
                }
            }

            Row{
                id: resourceErrorRow
                Layout.fillWidth: true
                visible: false
                spacing: 5
                Label{
                    text: qsTr("Error:")
                    color: "red"
                }
                Label{
                    id: resourceError
                    elide: Text.ElideLeft
                    width: 270
                }
            }

            GridLayout{
                id: resourceParametersLayout
                Layout.fillWidth: true
                Layout.topMargin: 10
                columns: 2
                columnSpacing: 5
                rowSpacing: 5
                visible: false

                Label{
                    text: qsTr("Resolution")
                }
                ComboBox{
                    id: resolutions
                    Layout.fillWidth: true
                    //TODO add default background
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
                }
                Label{
                    text: qsTr("Format")
                }
                ComboBox{
                    id: formats
                    Layout.fillWidth: true
                }
            }

            Button{
                id: applyVideoSettingsButton
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 5
                text: qsTr("Apply")
                visible: false
                onClicked: {
                    videoSettingsDrawer.close()
                    videoSettingsLayout.state = "APPLIED"
                    backend.applyResource()
                }
            }
        }
    }

    VideoOutput{
        anchors.fill: parent
        source: backend
        fillMode: VideoOutput.PreserveAspectFit
    }

    footer: ToolBar{
        id: footer
        visible: videoSettingsLayout.state === "APPLIED"

        RowLayout{
            anchors.centerIn: parent
            Rectangle{
                id: streamStatus
                Layout.alignment: Qt.AlignVCenter
                radius: 20
                width: 20
                height: 20
            }
            ToolButton{
                text: "Start"
                onClicked: {
                    backend.resource.stream.start()
                }
            }
            ToolButton{
                text: "Stop"
                onClicked: {
                    backend.resource.stream.stop()
                }
            }
        }
    }
}
