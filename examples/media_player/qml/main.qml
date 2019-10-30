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
                text: "\u2630"
                onClicked: {
                    videoSettings.open()
                }
            }
        }
    }

    VideoSettings{
        id: videoSettings
        height: parent.height
    }

    VideoOutput{
        anchors.fill: parent
        source: backend
        fillMode: VideoOutput.PreserveAspectFit
    }

    footer: ToolBar{
        id: footer
        visible: backend.resource !== null &&  backend.resource.state === Resource.Initialised

        RowLayout{
            anchors.centerIn: parent
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
