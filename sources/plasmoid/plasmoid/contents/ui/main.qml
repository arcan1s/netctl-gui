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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.plasmoid 2.0


Item {
    id: main

    // variables
    Text {
        id: info
        property string current: "N\\A"
        property string extip4: "127.0.0.1"
        property string extip6: "::1"
        property string intip4: "127.0.0.1"
        property string intip6: "::1"
        property var interfaces: ["lo"]
        property var profiles: []
        property string status: "N\\A"
    }
    Text {
        id: iconPath
        property string active: "/usr/share/icons/hicolor/64x64/apps/netctl-idle.png"
        property string inactive: "/usr/share/icons/hicolor/64x64/apps/netctl-offline.png"
    }
    property int interval: 1000
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
                if (data.value == "true") {
                    main.status = true
                    icon.source = iconPath.active
                } else {
                    main.status = false
                    icon.source = iconPath.inactive
                }
            } else if (sourceName == "current") {
                info.current= data.value
                // text update
            } else if (sourceName == "extip4") {
                info.extip4 = data.value
            } else if (sourceName == "extip6") {
                info.extip6 = data.value
            } else if (sourceName == "interfaces") {
                info.interfaces = data.value
            } else if (sourceName == "intip4") {
                info.intip4 = data.value
            } else if (sourceName == "intip6") {
                info.intip6 = data.value
            } else if (sourceName == "profiles") {
                info.profiles = data.value
            } else if (sourceName == "status") {
                info.status = data.value
            }
        }
    }

    Grid {
        id: mainGrid
        columns: 2

        Image {
            id: icon
            source: iconPath.inactive
        }
        Text {
            id: text
            text: "N\\A"
        }
    }
}
