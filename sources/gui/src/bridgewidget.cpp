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


QHash<QString, QString> BridgeWidget::getSettings()
{
    QHash<QString, QString> bridgeSettings;

    if (isOk() == 0) {
        if (ui->checkBox_skip->checkState() == Qt::Checked)
            bridgeSettings[QString("SkipForwardingDelay")] = QString("yes");
        clear();
    }

    return bridgeSettings;
}


int BridgeWidget::isOk()
{
    // all fine
    return 0;
}


void BridgeWidget::setSettings(QHash<QString, QString> settings)
{
    if (settings.contains(QString("SkipForwardingDelay")))
        if (settings[QString("SkipForwardingDelay")] == QString("yes"))
            ui->checkBox_skip->setCheckState(Qt::Checked);
}
