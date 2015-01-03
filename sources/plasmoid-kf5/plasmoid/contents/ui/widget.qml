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


Item {
    id: widgetPage
    width: childrenRect.width
    height: childrenRect.height

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


    QtLayouts.ColumnLayout {
        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Auto update interval, msec")
            }
            QtControls.SpinBox {
                id: autoUpdate
                minimumValue: 1000
                maximumValue: 10000
                stepSize: 500
                value: plasmoid.configuration.autoUpdateInterval
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Path to GUI")
            }
            QtControls.TextField {
                id: guiPath
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.guiPath
            }
            QtControls.Button {
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

        QtLayouts.RowLayout {
            QtControls.CheckBox {
                id: useHelper
                text: i18n("Use helper")
            }
            QtControls.TextField {
                id: helperPath
                enabled: useHelper.checked
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.helperPath
            }
            QtControls.Button {
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

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Path to netctl")
            }
            QtControls.TextField {
                id: netctlPath
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.netctlPath
            }
            QtControls.Button {
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

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Path to netctl-auto")
            }
            QtControls.TextField {
                id: netctlAutoPath
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.netctlAutoPath
            }
            QtControls.Button {
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

        QtLayouts.RowLayout {
            QtControls.CheckBox {
                id: useSudo
                text: i18n("Use sudo for netctl")
            }
            QtControls.TextField {
                id: sudoPath
                enabled: useSudo.checked
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.sudoPath
            }
            QtControls.Button {
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

        QtLayouts.RowLayout {
            QtControls.CheckBox {
                id: useWifi
                text: i18n("Show 'Start WiFi menu'")
            }
            QtControls.TextField {
                id: wifiPath
                enabled: useWifi.checked
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.wifiPath
            }
            QtControls.Button {
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
            QtLayouts.Layout.fillWidth: true
            text: plasmoid.configuration.textPattern
        }
    }
}
