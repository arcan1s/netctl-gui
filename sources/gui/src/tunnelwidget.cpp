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
    // using input mask because validators is not comfortable
    // ipv4
    ui->lineEdit_local->setInputMask(QString("999.999.999.999"));
    ui->lineEdit_remote->setInputMask(QString("999.999.999.999"));
}


QString TunnelWidget::getIp(const QString rawIp)
{
    QStringList ip = rawIp.split(QChar('.'));

    // fix empty fields
    if (ip[0].isEmpty())
        ip[0] = QString("127");
    if (ip[1].isEmpty())
        ip[1] = QString("0");
    if (ip[2].isEmpty())
        ip[2] = QString("0");
    if (ip[3].isEmpty())
        ip[3] = QString("1");
    // fix numbers
    for (int i=0; i<4; i++)
        if (ip[i].toInt() > 255)
            ip[i] = QString("255");

    return ip.join(QChar('.'));
}


QMap<QString, QString> TunnelWidget::getSettings()
{
    QMap<QString, QString> tunnelSettings;

    if (isOk() != 0)
        return tunnelSettings;

    tunnelSettings[QString("Mode")] = QString("'") + ui->comboBox_mode->currentText() + QString("'");
    if (!ui->lineEdit_local->text().remove(QChar('.')).remove(QChar(' ')).isEmpty())
        tunnelSettings[QString("Local")] = QString("'") + getIp(ui->lineEdit_local->text().remove(QChar(' '))) + QString("'");
    if (!ui->lineEdit_remote->text().remove(QChar('.')).remove(QChar(' ')).isEmpty())
        tunnelSettings[QString("Remote")] = QString("'") + getIp(ui->lineEdit_remote->text().remove(QChar(' '))) + QString("'");

    return tunnelSettings;
}


int TunnelWidget::isOk()
{
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
