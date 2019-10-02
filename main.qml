import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 520
    height: 210
    title: qsTr("文本二进制互转工具V0.1  作者:大樹(842577951@qq.com)")

    property bool isTxt2Bin: true

    Rectangle {
        id: fileListShow
        anchors.left: parent.left
        anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.topMargin: 2

        width: parent.width/3*2
        height: parent.height - 2
        border.width: 1
        border.color: "#363636"
        radius: 1

        Text {
            width: parent.width - 5
            id: labels
            //text: "file:///C:/Users/Administrator/Desktop/kernel_data.bin.txt"
            wrapMode: Text.WrapAnywhere
            font.family: "宋体"
            font.pixelSize: 14
        }
    }

    Rectangle {
        id:controlArea
        anchors.left: fileListShow.right
        anchors.top: parent.top
        anchors.topMargin: 10
        width: parent.width/3 - 4
        height: parent.height - 2

        Button {
            id:openBtn
            height: 32
            width: parent.width - 20
            text:qsTr("选择文件")
            anchors.top: parent.top
            anchors.margins: 5
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                fds.open();
            }
        }

        Button {
            id:convertBtn
            height: 32
            width: parent.width - 20
            text:qsTr("开始转换")
            anchors.top: openBtn.bottom
            anchors.margins: 5
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                if(FILECONVERT.fileName == "") {
                    tipLabel.text = "未选择转换文件"
                }
                else {
                    if(isTxt2Bin)
                        FILECONVERT.doConvert(1);
                    else
                        FILECONVERT.doConvert(2);
                }
            }
        }

        Button {
            id:tipBtn
            height: 32
            width: parent.width - 20
            text:qsTr("说明")
            anchors.top: convertBtn.bottom
            anchors.margins: 5
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                msgDialog.open();
            }
        }

        MessageDialog {
            id:msgDialog
            width: 520
            height: 180
            //buttons: MessageDialog.Ok
            text: "文本二进制互转工具使用说明"
            informativeText: "\n二进制转文本，会在每个字节前面加\"0x\"头部，并使用空格分隔;\n文本转二进制，自动过滤\"0x\"、空格、换行符;"
        }

        FileDialog {
            id:fds
            title: "选择文件"
            folder: shortcuts.desktop
            selectExisting: true
            selectFolder: false
            selectMultiple: false
            nameFilters: ["txt文件 (*.txt)","bin文件 (*.bin)"]
            onAccepted: {
                labels.text = fds.fileUrl;                
                FILECONVERT.fileName = fds.fileUrl
                tipLabel.text = ""
                //根据文件名后缀，自动选择转换类型
                var suffix = FILECONVERT.fileName.substring(FILECONVERT.fileName.length - 4);
                if(suffix == ".txt")
                    isTxt2Bin = true;
                else
                    isTxt2Bin = false;
            }

            onRejected: {
                //if (labels.text == "")
                    //tipLabel.text = "未选中文件"
            }
        }

        Column {
            anchors.top: tipBtn.bottom
            anchors.topMargin: 2
            anchors.left: parent.left
            anchors.leftMargin: 10

            CheckBox {
                id: txt2binCB
                text: qsTr("文本转二进制")
                checked: true

                onClicked: {
                    isTxt2Bin = !isTxt2Bin
                }
            }
            CheckBox {
                id: bin2txtCB
                text: qsTr("二进制转文本")
                checked: false

                onClicked: {
                    isTxt2Bin = !isTxt2Bin
                }
            }
        }


    }

    onIsTxt2BinChanged: {
        txt2binCB.checked = isTxt2Bin
        bin2txtCB.checked = !isTxt2Bin
    }

    footer: Item {
        height: 20
        width: parent.width

        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 2
            color: "#EEEEEE"
            Label {
                id: tipLabel
                objectName: "tipLabelObj"
                anchors.left: parent.left
                anchors.leftMargin: 5
                font.family: "宋体"
                font.pixelSize: 14
                //anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }
}
