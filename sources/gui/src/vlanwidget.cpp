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


QMap<QString, QString> VlanWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    settings[QString("VLANID")] = QString::number(ui->spinBox_vlan->value());

    return settings;
}


int VlanWidget::isOk()
{
    // all fine
    return 0;
}


void VlanWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("VLANID")))
        ui->spinBox_vlan->setValue(settings[QString("VLANID")].toInt());
}
