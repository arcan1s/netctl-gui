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
            if (data.isEmpty) return
            if (sourceName == "active") {
                main.status = data.value == "true" ? true : false
                icon.source = iconPath[data.value]
            } else if (sourceName == "current") {
                info["current"] = data.value
                // text update
                info["info"] = NetctlAdds.getInfo(info["current"], info["status"])
                text.text = NetctlAdds.parsePattern(pattern, info)
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
}
