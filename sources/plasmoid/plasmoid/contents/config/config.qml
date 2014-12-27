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

import org.kde.plasma.configuration 2.0
import org.kde.plasma.plasmoid 2.0


ConfigModel {
    ConfigCategory {
         name: i18n("Netctl plasmoid")
         icon: "/usr/share/pixmaps/netctl-gui-widget.png"
         source: "widget.qml"
    }

    ConfigCategory {
         name: i18n("Appearance")
         icon: "preferences-desktop-theme"
         source: "appearance.qml"
    }
}
