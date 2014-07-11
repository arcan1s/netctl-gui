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


void MacvlanWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
}


void MacvlanWidget::createFilter()
{
    // mac
    ui->lineEdit_mac->setInputMask(QString(">HH:HH:HH:HH:HH:HH"));
}


QMap<QString, QString> MacvlanWidget::getSettings()
{
    QMap<QString, QString> macvlanSettings;

    if (isOk() != 0)
        return macvlanSettings;

    macvlanSettings[QString("Mode")] = ui->comboBox_mode->currentText();
    if (!ui->lineEdit_mac->text().split(QChar(':')).join(QString("")).remove(QString(" ")).isEmpty())
        macvlanSettings[QString("MACAddress")] = ui->lineEdit_mac->text();

    return macvlanSettings;
}


int MacvlanWidget::isOk()
{
    // all fine
    return 0;
}


void MacvlanWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();
    QMap<QString, QString> macvlanSettings = settings;

    if (macvlanSettings.contains(QString("Mode")))
        for (int i=0; i<ui->comboBox_mode->count(); i++)
            if (macvlanSettings[QString("Mode")].remove(QString("'")) == ui->comboBox_mode->itemText(i))
                ui->comboBox_mode->setCurrentIndex(i);
    if (macvlanSettings.contains(QString("MACAddress")))
        ui->lineEdit_mac->setText(macvlanSettings[QString("MACAddress")]);
}
