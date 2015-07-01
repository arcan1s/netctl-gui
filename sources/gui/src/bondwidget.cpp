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

#include "bondwidget.h"
#include "ui_bondwidget.h"


BondWidget::BondWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::BondWidget)
{
    ui->setupUi(this);
    clear();
}


BondWidget::~BondWidget()
{
    delete ui;
}


void BondWidget::clear()
{
    ui->lineEdit_mode->setText(QString("balance-rr"));
}


QMap<QString, QString> BondWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (ui->lineEdit_mode->text() != QString("balance-rr"))
        settings[QString("Mode")] = ui->lineEdit_mode->text();

    return settings;
}


int BondWidget::isOk()
{
    // all fine
    return 0;
}


void BondWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("Mode")))
        ui->lineEdit_mode->setText(settings[QString("Mode")]);
}
