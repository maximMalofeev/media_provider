import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import MediaProvider 1.0

import QtQuick.Controls.Universal 2.12

Drawer {
    id: root
    width: 350
    height: 500
    font.pixelSize: 13

    Page{
        anchors.fill: parent
        header: Label{
            text: "VideoSettings"
            horizontalAlignment: Text.AlignHCenter
            ToolButton{
                anchors.right: parent.right
                anchors.rightMargin: 5
                height: parent.height
                text: "Reset"
                onClicked: {
                    backend.resetProvider()
                }
            }
        }

        states: [
            State{
                name: "ResourceError"
                extend: "HasResource"
                when: backend.resource.state === Resource.Invalid
                PropertyChanges {target: resourceError; visible: true; text: backend.resource.errorString}
            },
            State{
                name: "HasResource"
                extend: "HasProvider"
                when: backend.resource !== null
                PropertyChanges {target: busyIndicator; running: backend.resource.state === Resource.Initialising}
                PropertyChanges {target: currentResource; visible: true; text: backend.resource.resource}
                PropertyChanges {target: availableResources; visible: false}
                PropertyChanges {target: resolutions; visible: true; model: backend.resource.availableSizes}
                PropertyChanges {target: formats; visible: true; model: backend.resource.availableColorFormats}
                PropertyChanges {target: applySettings; visible: true}
            },
            State {
                name: "ProviderError"
                extend: "HasProvider"
                when: backend.provider.state === Provider.Invalid
                PropertyChanges {target: providerError; visible: true; text: backend.provider.errorString}
            },
            State {
                when: backend.provider !== null
                name: "HasProvider"
                PropertyChanges {target: availableProviders; visible: false}
                PropertyChanges {target: busyIndicator; running: backend.provider.state === Provider.Initialising}
                PropertyChanges {target: currentProvider; visible: true; text: backend.provider.provider}
                PropertyChanges {target: providerOrigin; visible: true; text: backend.provider.origin}
                PropertyChanges {target: availableResources; visible: true; model: backend.provider.availableResources}
            }
        ]

        BusyIndicator{
            id: busyIndicator
            anchors.centerIn: parent
            width: 70
            height: width
            running: false
        }

        GridLayout{
            anchors{
                left: parent.left
                top: parent.top
                topMargin: 5
                right: parent.right
            }
            columns: 2
            clip: true

            ListView{
                id: availableProviders
                Layout.fillWidth: true
                Layout.columnSpan: 2
                Layout.preferredHeight: contentHeight
                model: backend.availableProviders()
                clip: true
                spacing: 1
                header: Label{
                    width: parent.width
                    text: qsTr("Providers:")
                    horizontalAlignment: Qt.AlignHCenter
                }
                delegate: ItemDelegate {
                    width: parent.width
                    text: modelData
                    onClicked: {
                        backend.createProvider(modelData)
                        if(backend.provider){
                            backend.provider.initialise()
                        }
                    }
                }
                Frame {anchors.fill: parent; z: -1}
            }
            Label{
                text: qsTr("Provider:")
                visible: currentProvider.visible
            }
            Label{
                id: currentProvider
                elide: Text.ElideLeft
                visible: false
            }
            Label{
                Layout.alignment: Qt.AlignVCenter
                text: qsTr("Origin:")
                visible: providerOrigin.visible
            }
            TextField{
                Layout.fillWidth: true
                id: providerOrigin
                visible: false
                onAccepted: {
                    backend.provider.origin = text
                    backend.provider.initialise()
                    focus = false
                }
            }
            Label{
                text: qsTr("Error:")
                color: "red"
                visible: providerError.visible
            }
            Label{
                id: providerError
                elide: Text.ElideLeft
                color: "red"
                visible: false
            }

            ListView{
                id: availableResources
                Layout.fillWidth: true
                Layout.columnSpan: 2
                Layout.preferredHeight: contentHeight > 300 ? 300 : contentHeight
                width: parent.width
                clip: true
                visible: false
                ScrollBar.vertical: ScrollBar {topInset: 2; rightInset: 2; bottomInset: 2}

                header: Label{
                    width: parent.width
                    text: qsTr("Resources:")
                    horizontalAlignment: Qt.AlignHCenter
                }
                delegate: ItemDelegate {
                    width: parent.width
                    text: modelData
                    onClicked: {
                        backend.createResource(modelData)
                    }
                }
                Frame {anchors.fill: parent; z: -1}
            }

            Label{
                text: qsTr("Resource:")
                visible: currentResource.visible
            }
            Label{
                id: currentResource
                visible: false
            }
            Label{
                text: qsTr("Error:")
                visible: resourceError.visible
                color: "red"
            }
            Label{
                id: resourceError
                visible: false
                color: "red"
            }

            Label{
                text: qsTr("Resolution")
                visible: resolutions.visible
            }
            ComboBox{
                id: resolutions
                Layout.fillWidth: true
                visible: false
                editable: false
                Universal.theme: editable && activeFocus ? Universal.Light : undefined
                delegate: ItemDelegate{
                    width: parent.width
                    text: modelData.width + " x " + modelData.height
                    highlighted: parent.highlightedIndex === index
                    font.weight: parent.currentIndex === index ? Font.DemiBold : Font.Normal
                    hoverEnabled: parent.hoverEnabled
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
                visible: formats.visible
            }
            ComboBox{
                id: formats
                Layout.fillWidth: true
                visible: false
                delegate: ItemDelegate{
                    width: parent.width
                    text: modelData
                    highlighted: parent.highlightedIndex === index
                    font.weight: parent.currentIndex === index ? Font.DemiBold : Font.Normal
                    hoverEnabled: parent.hoverEnabled
                }
            }

            Button{
                id: applySettings
                Layout.alignment: Qt.AlignHCenter
                Layout.columnSpan: 2
                Layout.topMargin: 5
                text: qsTr("Apply")
                visible: false
                onClicked: {
                    root.close()
                    backend.applyResource()
                }
            }
        }
    }
}
