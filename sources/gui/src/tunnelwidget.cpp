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

#include "tunnelwidget.h"
#include "ui_tunnelwidget.h"

#include "ipregexp.h"


TunnelWidget::TunnelWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TunnelWidget)
{
    ui->setupUi(this);
    createFilter();
    clear();
}


TunnelWidget::~TunnelWidget()
{
    delete ui;
}


void TunnelWidget::clear()
{
    ui->comboBox_mode->setCurrentIndex(0);
    ui->lineEdit_local->clear();
    ui->lineEdit_remote->clear();
}


void TunnelWidget::createFilter()
{
    ui->lineEdit_local->setValidator(IpRegExp::ipv4Validator());
    ui->lineEdit_remote->setValidator(IpRegExp::ipv4Validator());
}


QMap<QString, QString> TunnelWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    settings[QString("Mode")] = QString("'%1'").arg(ui->comboBox_mode->currentText());
    if (!IpRegExp::checkString(ui->lineEdit_local->text(), IpRegExp::ip4Regex()))
        settings[QString("Local")] = QString("'%1'").arg(ui->lineEdit_local->text());
    settings[QString("Remote")] = QString("'%1'").arg(ui->lineEdit_remote->text());

    return settings;
}


int TunnelWidget::isOk()
{
    // ip is not correct
    if (!IpRegExp::checkString(ui->lineEdit_remote->text(), IpRegExp::ip4Regex()))
        return 1;
    // all fine
    return 0;
}


void TunnelWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("Mode"))) {
        int index = ui->comboBox_mode->findText(settings[QString("Mode")]);
        ui->comboBox_mode->setCurrentIndex(index);
    }
    if (settings.contains(QString("Local")))
        ui->lineEdit_local->setText(settings[QString("Local")]);
    if (settings.contains(QString("Remote")))
        ui->lineEdit_remote->setText(settings[QString("Remote")]);
}
