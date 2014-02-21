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


void TunnelWidget::createFilter()
{
    // using input mask because validators is not comfortable
    // ipv4
    ui->lineEdit_local->setInputMask(QString("999.999.999.999"));
    ui->lineEdit_remote->setInputMask(QString("999.999.999.999"));
}


QString TunnelWidget::getIp(QString rawIp)
{
    QStringList ip = rawIp.split(QString("."));

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

    return ip.join(QString("."));
}


QHash<QString, QString> TunnelWidget::getSettings()
{
    QHash<QString, QString> tunnelSettings;

    if (isOk() == 0) {
        tunnelSettings[QString("Mode")] = QString("'") + ui->comboBox_mode->currentText() + QString("'");
        if (!ui->lineEdit_local->text().split(QString(".")).join(QString("")).remove(QString(" ")).isEmpty())
            tunnelSettings[QString("Local")] = QString("'") + getIp(ui->lineEdit_local->text().remove(QString(" "))) + QString("'");
        if (!ui->lineEdit_remote->text().split(QString(".")).join(QString("")).remove(QString(" ")).isEmpty())
            tunnelSettings[QString("Remote")] = QString("'") + getIp(ui->lineEdit_remote->text().remove(QString(" "))) + QString("'");
        clear();
    }

    return tunnelSettings;
}


int TunnelWidget::isOk()
{
    // all fine
    return 0;
}
