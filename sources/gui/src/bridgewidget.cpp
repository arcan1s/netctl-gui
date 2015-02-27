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

#include "bridgewidget.h"
#include "ui_bridgewidget.h"


BridgeWidget::BridgeWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::BridgeWidget)
{
    ui->setupUi(this);
    clear();
}


BridgeWidget::~BridgeWidget()
{
    delete ui;
}


void BridgeWidget::clear()
{
    ui->checkBox_skip->setCheckState(Qt::Unchecked);
}


QMap<QString, QString> BridgeWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (ui->checkBox_skip->checkState() == Qt::Checked)
        settings[QString("SkipForwardingDelay")] = QString("yes");

    return settings;
}


int BridgeWidget::isOk()
{
    // all fine
    return 0;
}


void BridgeWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("SkipForwardingDelay")))
        if (settings[QString("SkipForwardingDelay")] == QString("yes"))
            ui->checkBox_skip->setCheckState(Qt::Checked);
}
