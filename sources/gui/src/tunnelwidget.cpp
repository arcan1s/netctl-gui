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


void TunnelWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
}


void TunnelWidget::createFilter()
{
    ui->lineEdit_local->setValidator(IpRegExp::ipv4Validator());
    ui->lineEdit_remote->setValidator(IpRegExp::ipv4Validator());
}


QMap<QString, QString> TunnelWidget::getSettings()
{
    QMap<QString, QString> tunnelSettings;

    if (isOk() != 0)
        return tunnelSettings;

    tunnelSettings[QString("Mode")] = QString("'") + ui->comboBox_mode->currentText() + QString("'");
    if (!ui->lineEdit_local->text().remove(QChar('.')).remove(QChar(' ')).isEmpty())
        tunnelSettings[QString("Local")] = QString("'") + ui->lineEdit_local->text() + QString("'");
    if (!ui->lineEdit_remote->text().remove(QChar('.')).remove(QChar(' ')).isEmpty())
        tunnelSettings[QString("Remote")] = QString("'") + ui->lineEdit_remote->text() + QString("'");

    return tunnelSettings;
}


int TunnelWidget::isOk()
{
    // ip is not correct
    if ((!IpRegExp::checkString(ui->lineEdit_local->text(), IpRegExp::ip4Regex())) ||
        (!IpRegExp::checkString(ui->lineEdit_remote->text(), IpRegExp::ip4Regex())))
        return 1;
    // all fine
    return 0;
}


void TunnelWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();
    QMap<QString, QString> tunnelSettings = settings;

    if (tunnelSettings.contains(QString("Mode")))
        for (int i=0; i<ui->comboBox_mode->count(); i++)
            if (tunnelSettings[QString("Mode")] == ui->comboBox_mode->itemText(i))
                ui->comboBox_mode->setCurrentIndex(i);
    if (tunnelSettings.contains(QString("Local")))
        ui->lineEdit_local->setText(tunnelSettings[QString("Local")]);
    if (tunnelSettings.contains(QString("Remote")))
        ui->lineEdit_remote->setText(tunnelSettings[QString("Remote")]);
}
