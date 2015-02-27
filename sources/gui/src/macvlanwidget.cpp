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


#include "macvlanwidget.h"
#include "ui_macvlanwidget.h"


MacvlanWidget::MacvlanWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MacvlanWidget)
{
    ui->setupUi(this);
    createFilter();
}


MacvlanWidget::~MacvlanWidget()
{
    delete ui;
}


void MacvlanWidget::clear()
{
    ui->comboBox_mode->setCurrentIndex(0);
    ui->lineEdit_mac->clear();
}


void MacvlanWidget::createFilter()
{
    // mac
    ui->lineEdit_mac->setInputMask(QString(">hh:hh:hh:hh:hh:hh"));
}


QMap<QString, QString> MacvlanWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    settings[QString("Mode")] = ui->comboBox_mode->currentText();
    if (!ui->lineEdit_mac->text().remove(QChar(':')).isEmpty())
        settings[QString("MACAddress")] = ui->lineEdit_mac->text();

    return settings;
}


int MacvlanWidget::isOk()
{
    // all fine
    return 0;
}


void MacvlanWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("Mode"))) {
        int index = ui->comboBox_mode->findText(settings[QString("Mode")]);
        ui->comboBox_mode->setCurrentIndex(index);
    }
    if (settings.contains(QString("MACAddress")))
        ui->lineEdit_mac->setText(settings[QString("MACAddress")]);
}
