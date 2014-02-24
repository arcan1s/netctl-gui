/***************************************************************************
 *   This file is part of netctl-plasmoid                                  *
 *                                                                         *
 *   netctl-plasmoid is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-plasmoid is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-plasmoid. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "vlanwidget.h"
#include "ui_vlanwidget.h"


VlanWidget::VlanWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::VlanWidget)
{
    ui->setupUi(this);
    clear();
}


VlanWidget::~VlanWidget()
{
    delete ui;
}


void VlanWidget::clear()
{
    ui->spinBox_vlan->setValue(55);
}


void VlanWidget::setShown(bool state)
{
    if (state)
        VlanWidget::show();
    else
        VlanWidget::hide();
}


QHash<QString, QString> VlanWidget::getSettings()
{
    QHash<QString, QString> vlanSettings;

    if (isOk() == 0) {
        vlanSettings[QString("VLANID")] = QString(ui->spinBox_vlan->value());
        clear();
    }

    return vlanSettings;
}


int VlanWidget::isOk()
{
    // all fine
    return 0;
}


void VlanWidget::setSettings(QHash<QString, QString> settings)
{
    if (settings.contains(QString("VLANID")))
        ui->spinBox_vlan->setValue(settings[QString("VLANID")].toInt());
}
