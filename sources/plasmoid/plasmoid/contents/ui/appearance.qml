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
import QtQuick.Controls.Styles 1.3 as QtStyles
import QtQuick.Dialogs 1.1 as QtDialogs
import QtQuick.Layouts 1.0 as QtLayouts


Item {
    id: appearancePage
    width: childrenRect.width
    height: childrenRect.height

    property variant weight: {
        25: 0,
        50: 1,
        63: 3,
        75: 4,
        87: 5
    }

    property string cfg_textAlign: textAlign.currentText
    property alias cfg_fontFamily: selectFont.text
    property alias cfg_fontSize: fontSize.value
    property string cfg_fontWeight: fontWeight.currentText
    property string cfg_fontStyle: fontStyle.currentText
    property alias cfg_fontColor: selectColor.text
    property alias cfg_activeIconPath: activeIcon.text
    property alias cfg_inactiveIconPath: inactiveIcon.text


    QtLayouts.ColumnLayout {
        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Text align")
            }
            QtControls.ComboBox {
                id: textAlign
                textRole: "label"
                model: [
                    {
                        'label': i18n("center"),
                        'name': "center"
                    },
                    {
                        'label': i18n("right"),
                        'name': "right"
                    },
                    {
                        'label': i18n("left"),
                        'name': "left"
                    },
                    {
                        'label': i18n("justify"),
                        'name': "justify"
                    }
                ]
                onCurrentIndexChanged: cfg_textAlign = model[currentIndex]["name"]
                Component.onCompleted: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.textAlign) {
                            textAlign.currentIndex = i;
                        }
                    }
                }
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Font")
            }
            QtControls.Button {
                id: selectFont
                text: plasmoid.configuration.fontFamily
                onClicked: fontDialog.visible = true
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Font size")
            }
            QtControls.SpinBox {
                id: fontSize
                minimumValue: 8
                maximumValue: 24
                stepSize: 1
                value: plasmoid.configuration.fontSize
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Font weight")
            }
            QtControls.ComboBox {
                id: fontWeight
                textRole: "label"
                model: [
                    {
                        'label': i18n("light"),
                        'name': "light"
                    },
                    {
                        'label': i18n("normal"),
                        'name': "normal"
                    },
                    {
                        'label': i18n("demi bold"),
                        'name': "demibold"
                    },
                    {
                        'label': i18n("bold"),
                        'name': "bold"
                    },
                    {
                        'label': i18n("black"),
                        'name': "black"
                    }
                ]
                onCurrentIndexChanged: cfg_fontWeight = model[currentIndex]["name"]
                Component.onCompleted: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.fontWeight) {
                            fontWeight.currentIndex = i;
                        }
                    }
                }
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Font style")
            }
            QtControls.ComboBox {
                id: fontStyle
                textRole: "label"
                model: [
                    {
                        'label': i18n("normal"),
                        'name': "normal"
                    },
                    {
                        'label': i18n("italic"),
                        'name': "italic"
                    }
                ]
                onCurrentIndexChanged: cfg_fontStyle = model[currentIndex]["name"]
                Component.onCompleted: {
                    for (var i = 0; i < model.length; i++) {
                        if (model[i]["name"] == plasmoid.configuration.fontStyle) {
                            fontStyle.currentIndex = i;
                        }
                    }
                }
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Font color")
            }
            QtControls.Button {
                id: selectColor
                style: QtStyles.ButtonStyle {
                    background: Rectangle {
                        color: plasmoid.configuration.fontColor
                    }
                }
                text: plasmoid.configuration.fontColor
                onClicked: colorDialog.visible = true
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Active icon")
            }
            QtControls.TextField {
                id: activeIcon
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.activeIconPath
            }
            QtControls.Button {
                text: i18n("Browse")
                onClicked: {
                    // FIXME: more clean directory definition
                    var list = inactiveIcon.text.split('/')
                    list.pop()
                    activeFileDialog.folder = "/" + list.join('/')
                    activeFileDialog.visible = true
                }
            }

            QtDialogs.FileDialog {
                id: activeFileDialog
                title: i18n("Select a path")
                nameFilters: [ "Image files (*.jpeg *.jpg *.png)", "All files (*)" ]
                selectExisting: true
                onAccepted: activeIcon.text = activeFileDialog.fileUrl
            }
        }

        QtLayouts.RowLayout {
            QtControls.Label {
                text: i18n("Inactive icon")
            }
            QtControls.TextField {
                id: inactiveIcon
                QtLayouts.Layout.fillWidth: true
                text: plasmoid.configuration.inactiveIconPath
            }
            QtControls.Button {
                text: i18n("Browse")
                onClicked: {
                    // FIXME: more clean directory definition
                    var list = inactiveIcon.text.split('/')
                    list.pop()
                    inactiveFileDialog.folder = "/" + list.join('/')
                    inactiveFileDialog.visible = true
                }
            }

            QtDialogs.FileDialog {
                id: inactiveFileDialog
                title: i18n("Select a path")
                nameFilters: [ "Image files (*.jpeg *.jpg *.png)", "All files (*)" ]
                selectExisting: true
                onAccepted: inactiveIcon.text = inactiveFileDialog.fileUrl
            }
        }
    }

    QtDialogs.ColorDialog {
        id: colorDialog
        title: i18n("Select a color")
        color: selectColor.text
        onAccepted: {
            selectColor.text = colorDialog.color
        }
    }

    QtDialogs.FontDialog {
        id: fontDialog
        title: i18n("Select a font")
        font: Qt.font({ family: selectFont.text, pointSize: fontSize.value, weight: Font.Normal })
        onAccepted: {
            selectFont.text = fontDialog.font.family
            fontSize.value = fontDialog.font.pointSize
            fontStyle.currentIndex = fontDialog.font.italic ? 1 : 0
            fontWeight.currentIndex = weight[fontDialog.font.weight]
        }
    }
}
