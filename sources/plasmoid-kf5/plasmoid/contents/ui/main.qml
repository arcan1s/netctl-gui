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

import QtQuick 2.2
import QtQuick.Controls 1.3 as QtControls
import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents

import org.kde.plasma.private.netctl 1.0


Item {
    id: main

    // variables
    // internal
    property variant weight: {
        "light": Font.Light,
        "normal": Font.Normal,
        "demibold": Font.DemiBold,
        "bold": Font.Bold,
        "black": Font.Black
    }
    property variant align: {
        "left": Text.AlignLeft,
        "center": Text.AlignHCenter,
        "right": Text.AlignRight,
        "justify": Text.AlignJustify
    }
    // external
    property variant iconPath: {
        "true": plasmoid.configuration.activeIconPath,
        "false": plasmoid.configuration.inactiveIconPath
    }
    property variant info: {
        "current": "N\\A",
        "extip4": "127.0.0.1",
        "extip6": "::1",
        "intip4": "127.0.0.1",
        "intip6": "::1",
        "interfaces": "lo",
        "profiles": "",
        "status": "N\\A"
    }
    property int interval: plasmoid.configuration.autoUpdateInterval
    property string pattern: plasmoid.configuration.textPattern
    property bool status: false

    // init
    Plasmoid.icon: icon.source

    PlasmaCore.DataSource {
        id: mainData
        engine: "netctl"
        connectedSources: ["active", "current", "extip4", "extip6", "interfaces", "intip4", "intip6", "profiles", "status"]
        interval: main.interval

        onNewData: {
            if (data.value == "N\\A") return
            if (sourceName == "active") {
                status = data.value == "true"
                icon.source = iconPath[data.value]
            } else if (sourceName == "current") {
                info["current"] = data.value
                // text update
                info["info"] = NetctlAdds.getInfo(info["current"], info["status"])
                text.text = NetctlAdds.parsePattern(pattern, info)
                // update menus
                menuUpdate()
            } else if (sourceName == "extip4") {
                info["extip4"] = data.value
            } else if (sourceName == "extip6") {
                info["extip6"] = data.value
            } else if (sourceName == "interfaces") {
                info["interfaces"] = data.value
            } else if (sourceName == "intip4") {
                info["intip4"] = data.value
            } else if (sourceName == "intip6") {
                info["intip6"] = data.value
            } else if (sourceName == "profiles") {
                info["profiles"] = data.value
            } else if (sourceName == "status") {
                info["status"] = data.value
            }
        }
    }

    // ui
    Grid {
        id: mainGrid
        columns: 2

        Image {
            id: icon
            source: iconPath["false"]
        }
        Text {
            id: text
            color: plasmoid.configuration.fontColor
            font.family: plasmoid.configuration.fontFamily
            font.italic: plasmoid.configuration.fontStyle == "italic" ? true : false
            font.pointSize: plasmoid.configuration.fontSize
            font.weight: weight[plasmoid.configuration.fontWeight]
            horizontalAlignment: align[plasmoid.configuration.textAlign]
            textFormat: Text.RichText
            text: "N\\A"
        }
    }

    Component.onCompleted: {
        plasmoid.setAction("titleAction", "netctl-gui", plasmoid.icon)
        plasmoid.setAction("startProfile", i18n("Start profile"), "dialog-apply")
        plasmoid.setAction("stopProfile", i18n("Stop profile"), "dialog-close")
        plasmoid.setAction("stopAllProfiles", i18n("Stop all profiles"), "dialog-close")
        plasmoid.setAction("switchToProfile", i18n("Switch to profile"))
        plasmoid.setAction("restartProfile", i18n("Restart profile"), "stock-refresh")
        plasmoid.setAction("enableProfile", i18n("Enable profile"))
        // FIXME: icon from resources
        plasmoid.setAction("startWifi", i18n("Show WiFi menu"))
    }

    function menuUpdate() {
        var titleAction = plasmoid.action("titleAction")
        var startAction = plasmoid.action("startProfile")
        var stopAction = plasmoid.action("stopProfile")
        var stopAllAction = plasmoid.action("stopAllProfiles")
        var switchToAction = plasmoid.action("switchToProfile")
        var restartAction = plasmoid.action("restartProfile")
        var enableAction = plasmoid.action("enableProfile")
        var wifiAction = plasmoid.action("startWifi")

        titleAction.iconSource = plasmoid.icon
        titleAction.text = info["current"] + " " + info["status"]

        // FIXME: menu to actions
        if (info["status"] == "(netctl-auto)") {
            startAction.visible = false
            stopAction.visible = false
            stopAllAction.visible = false
            switchToAction.visible = true
            restartAction.visible = false
            enableAction.visible = false
            // MENU UPDATE
        } else {
            if (info["current"].indexOf("|") > -1) {
                startAction.visible = true
                stopAction.visible = false
                stopAllAction.visible = true
                switchToAction.visible = false
                restartAction.visible = false
                enableAction.visible = false
            } else {
                startAction.visible = true
                stopAction.visible = status
                stopAllAction.visible = false
                switchToAction.visible = false
                restartAction.visible = status
                enableAction.visible = status
            }
            if (status) {
                startAction.text = i18n("Start another profile")
                stopAction.text = i18n("Stop %1", info["current"])
                restartAction.text = i18n("Restart %1", info["current"])
                if (info["status"].indexOf("enabled") > -1)
                    enableAction.text = i18n("Disable %1", info["current"])
                else
                    enableAction.text = i18n("Enable %1", info["current"])
            } else
                startAction.text = i18n("Start profile")
            // MENU UPDATE
        }

        wifiAction.visible = plasmoid.configuration.useWifi
    }

    // actions
    function action_titleAction() {
        NetctlAdds.startApplication(plasmoid.configuration.guiPath)
    }

    function action_startProfile() {
//        NetctlAdds.startProfileSlot(profile, status, plasmoid.configuration.useHelper,
//                                    plasmoid.configuration.netctlPath,
//                                    plasmoid.configuration.sudoPath)
    }

    function action_stopProfile() {
        NetctlAdds.stopProfileSlot(info, plasmoid.configuration.useHelper,
                                   plasmoid.configuration.netctlPath,
                                   plasmoid.configuration.sudoPath)
    }

    function action_stopAllProfiles() {
        NetctlAdds.stopAllProfilesSlot(plasmoid.configuration.useHelper,
                                       plasmoid.configuration.netctlPath,
                                       plasmoid.configuration.sudoPath)
    }

    function action_switchToProfile() {
//        NetctlAdds.switchToProfileSlot(profile, plasmoid.configuration.useHelper,
//                                       plasmoid.configuration.netctlAutoPath)
    }

    function action_restartProfile() {
        NetctlAdds.restartProfileSlot(info, plasmoid.configuration.useHelper,
                                      plasmoid.configuration.netctlPath,
                                      plasmoid.configuration.sudoPath)
    }

    function action_enableProfile() {
        NetctlAdds.enableProfileSlot(info, plasmoid.configuration.useHelper,
                                     plasmoid.configuration.netctlPath,
                                     plasmoid.configuration.sudoPath)
    }

    function action_startWifi() {
        NetctlAdds.startApplication(plasmoid.configuration.wifiPath)
    }
}
