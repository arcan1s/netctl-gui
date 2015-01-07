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
import QtQuick.Controls 1.3 as QtControls
import QtQuick.Dialogs 1.1 as QtDialogs
import QtQuick.Layouts 1.0 as QtLayouts

import org.kde.plasma.private.netctl 1.0


Item {
    id: widgetPage
    width: childrenRect.width
    height: childrenRect.height

    property bool debug: NetctlAdds.isDebugEnabled()

    property alias cfg_autoUpdateInterval: autoUpdate.value
    property alias cfg_guiPath: guiPath.text
    property alias cfg_useHelper: useHelper.checked
    property alias cfg_helperPath: helperPath.text
    property alias cfg_netctlPath: netctlPath.text
    property alias cfg_netctlPathAuto: netctlAutoPath.text
    property alias cfg_useSudo: useSudo.checked
    property alias cfg_sudoPath: sudoPath.text
    property alias cfg_useWifi: useWifi.checked
    property alias cfg_wifiPath: wifiPath.text
    property alias cfg_textPattern: textPattern.text


    Column {
        id: pageColumn
        width: units.gridUnit * 25
        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Auto update interval, msec")
            }
            QtControls.SpinBox {
                id: autoUpdate
                height: parent.height
                width: parent.width * 2 / 3
                minimumValue: 1000
                maximumValue: 10000
                stepSize: 500
                value: plasmoid.configuration.autoUpdateInterval
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Path to GUI")
            }
            QtControls.TextField {
                id: guiPath
                height: parent.height
                width: parent.width * 2 / 3 - guiPathButton.width
                text: plasmoid.configuration.guiPath
            }
            QtControls.Button {
                id: guiPathButton
                text: i18n("Browse")
                onClicked: guiFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: guiFileDialog
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: guiPath.text = guiFileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.CheckBox {
                id: useHelper
                height: parent.height
                width: parent.width / 3
                text: i18n("Use helper")
            }
            QtControls.TextField {
                id: helperPath
                enabled: useHelper.checked
                height: parent.height
                width: parent.width * 2 / 3 - helperPathButton.width
                text: plasmoid.configuration.helperPath
            }
            QtControls.Button {
                id: helperPathButton
                enabled: useHelper.checked
                text: i18n("Browse")
                onClicked: helperFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: helperFileDialog
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: helperPath.text = helperFileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.Label {
                height: parent.height
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Path to netctl")
            }
            QtControls.TextField {
                id: netctlPath
                height: parent.height
                width: parent.width * 2 / 3 - netctlPathButton.width
                text: plasmoid.configuration.netctlPath
            }
            QtControls.Button {
                id: netctlPathButton
                text: i18n("Browse")
                onClicked: netctlFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: netctlFileDialog
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
                width: parent.width / 3
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: i18n("Path to netctl-auto")
            }
            QtControls.TextField {
                id: netctlAutoPath
                height: parent.height
                width: parent.width * 2 / 3 - netctlAutoPathButton.width
                text: plasmoid.configuration.netctlAutoPath
            }
            QtControls.Button {
                id: netctlAutoPathButton
                text: i18n("Browse")
                onClicked: netctlAutoFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: netctlAutoFileDialog
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
            QtControls.CheckBox {
                id: useSudo
                height: parent.height
                width: parent.width / 3
                text: i18n("Use sudo for netctl")
            }
            QtControls.TextField {
                id: sudoPath
                enabled: useSudo.checked
                height: parent.height
                width: parent.width * 2 / 3 - sudoPathButton.width
                text: plasmoid.configuration.sudoPath
            }
            QtControls.Button {
                id: sudoPathButton
                enabled: useSudo.checked
                text: i18n("Browse")
                onClicked: sudoFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: sudoFileDialog
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: sudoPath.text = sudoFileDialog.fileUrl
            }
        }

        Row {
            height: implicitHeight
            width: parent.width
            QtControls.CheckBox {
                id: useWifi
                height: parent.height
                width: parent.width / 3
                text: i18n("Show 'Start WiFi menu'")
            }
            QtControls.TextField {
                id: wifiPath
                enabled: useWifi.checked
                height: parent.height
                width: parent.width * 2 / 3 - wifiPathButton.width
                text: plasmoid.configuration.wifiPath
            }
            QtControls.Button {
                id: wifiPathButton
                enabled: useWifi.checked
                text: i18n("Browse")
                onClicked: wifiFileDialog.visible = true
            }

            QtDialogs.FileDialog {
                id: wifiFileDialog
                title: i18n("Select a path")
                folder: "/usr/bin"
                nameFilters: [ "All files (*)" ]
                selectExisting: true
                onAccepted: wifiPath.text = wifiFileDialog.fileUrl
            }
        }

        QtControls.TextArea {
            id: textPattern
            width: parent.width
            text: plasmoid.configuration.textPattern
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[about::onCompleted]")
    }
}
