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
import QtQuick.Layouts 1.0 as QtLayouts

import org.kde.plasma.private.netctl 1.0


Item {
    id: aboutPage
    // backend
    NetctlAdds {
        id: netctlAdds;
    }
    width: childrenRect.width
    height: childrenRect.height
    implicitWidth: pageColumn.implicitWidth
    implicitHeight: pageColumn.implicitHeight

    property bool debug: netctlAdds.isDebugEnabled()

    Column {
        id: pageColumn
        anchors.fill: parent
        QtControls.TabView {
            height: parent.height
            width: parent.width
            QtControls.Tab {
                title: i18n("About")

                QtLayouts.ColumnLayout {
                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: netctlAdds.getAboutText("header")
                    }

                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        horizontalAlignment: Text.AlignJustify
                        text: netctlAdds.getAboutText("description")
                    }

                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                        textFormat: Text.RichText
                        text: netctlAdds.getAboutText("links")
                        onLinkActivated: Qt.openUrlExternally(link);
                    }

                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        font.capitalization: Font.SmallCaps
                        horizontalAlignment: Text.AlignHCenter
                        textFormat: Text.RichText
                        text: netctlAdds.getAboutText("copy")
                    }
                }
            }

            QtControls.Tab {
                title: i18n("Acknowledgment")

                QtLayouts.ColumnLayout {
                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        horizontalAlignment: Text.AlignJustify
                        text: netctlAdds.getAboutText("translators")
                    }

                    QtControls.Label {
                        QtLayouts.Layout.fillWidth: true
                        horizontalAlignment: Text.AlignJustify
                        textFormat: Text.RichText
                        text: netctlAdds.getAboutText("3rdparty")
                        onLinkActivated: Qt.openUrlExternally(link);
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (debug) console.log("[about::onCompleted]")
    }
}
