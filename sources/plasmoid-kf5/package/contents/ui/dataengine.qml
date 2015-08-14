/***************************************************************************
 *   This file is part of netctl-gui                                       *
 *                                                                         *
 *   netctl-gui is free software: you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-gui is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-gui. If not, see http://www.gnu.org/licenses/       *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 1.0 as QtControls
import QtQuick.Dialogs 1.1 as QtDialogs
import QtQuick.Layouts 1.0 as QtLayouts
import QtQuick.Controls.Styles 1.3 as QtStyles

import org.kde.plasma.private.netctl 1.0


Item {
    id: dataenginePage
    // backend
    NetctlAdds {
        id: netctlAdds;
    }
    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: netctlAdds.isDebugEnabled()

    property variant cfg_dataengine: netctlAdds.readDataEngineConfiguration()

    Column {
        id: pageColumn
        anchors.fill: parent
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Path to netctl")
            }
            QtControls.TextField {
                id: netctlPath
                height: parent.height
                width: parent.width * 3 / 5 - netctlPathButton.width
                text: cfg_dataengine["NETCTLCMD"]
            }
            QtControls.Button {
                id: netctlPathButton
                text: i18n("Browse")
                onClicked: netctlFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: netctlFileDialog
                modality: Qt.NonModal
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: netctlPath.text = netctlFileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Path to netctl-auto")
            }
            QtControls.TextField {
                id: netctlAutoPath
                height: parent.height
                width: parent.width * 3 / 5 - netctlAutoPathButton.width
                text: cfg_dataengine["NETCTLAUTOCMD"]
            }
            QtControls.Button {
                id: netctlAutoPathButton
                text: i18n("Browse")
                onClicked: netctlAutoFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: netctlAutoFileDialog
                modality: Qt.NonModal
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: netctlAutoPath.text = netctlAutoFileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5 - extIp4.width
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Check external IPv4")
            }
            QtControls.CheckBox {
                id: extIp4
                height: parent.height
                width: implicitWidth
                checked: cfg_dataengine["EXTIP4"] == "true"
                style: QtStyles.CheckBoxStyle {
                    indicator: Rectangle {
                        implicitWidth: 16
                        implicitHeight: 16
                        radius: 3
                        border.width: 1
                        border.color: control.activeFocus ? "darkblue" : "gray"
                        Rectangle {
                            visible: control.checked
                            radius: 1
                            anchors.fill: parent
                            anchors.margins: 4
                            color: "#555555"
                            border.color: "#333333"
                        }
                    }
                }
            }
            QtControls.TextField {
                id: extIp4Path
                enabled: extIp4.checked
                height: parent.height
                width: parent.width * 3 / 5 - extIp4PathButton.width
                text: cfg_dataengine["EXTIP4CMD"]
            }
            QtControls.Button {
                id: extIp4PathButton
                enabled: extIp4.checked
                text: i18n("Browse")
                onClicked: extIp4FileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: extIp4FileDialog
                modality: Qt.NonModal
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: extIp4Path.text = extIp4FileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width * 2 / 5 - extIp6.width
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Check external IPv6")
            }
            QtControls.CheckBox {
                id: extIp6
                height: parent.height
                width: implicitWidth
                checked: cfg_dataengine["EXTIP6"] == "true"
                style: QtStyles.CheckBoxStyle {
                    indicator: Rectangle {
                        implicitWidth: 16
                        implicitHeight: 16
                        radius: 3
                        border.width: 1
                        border.color: control.activeFocus ? "darkblue" : "gray"
                        Rectangle {
                            visible: control.checked
                            radius: 1
                            anchors.fill: parent
                            anchors.margins: 4
                            color: "#555555"
                            border.color: "#333333"
                        }
                    }
                }
            }
            QtControls.TextField {
                id: extIp6Path
                enabled: extIp6.checked
                height: parent.height
                width: parent.width * 3 / 5 - extIp6PathButton.width
                text: cfg_dataengine["EXTIP6CMD"]
            }
            QtControls.Button {
                id: extIp6PathButton
                enabled: extIp6.checked
                text: i18n("Browse")
                onClicked: extIp6FileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: extIp6FileDialog
                modality: Qt.NonModal
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: extIp6Path.text = extIp6FileDialog.fileUrl
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[about::onCompleted]")
    }

    Component.onDestruction: {
        cfg_dataengine["EXTIP4"] = extIp4.checked ? "true" : "false"
        cfg_dataengine["EXTIP6"] = extIp6.checked ? "true" : "false"
        netctlAdds.writeDataEngineConfiguration(cfg_dataengine)
    }
}
