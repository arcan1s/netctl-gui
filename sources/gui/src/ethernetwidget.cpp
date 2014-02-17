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

#include "ethernetwidget.h"
#include "ui_ethernetwidget.h"


EthernetWidget::EthernetWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EthernetWidget)
{
    ui->setupUi(this);
    createActions();
    showAdvanced();
}


EthernetWidget::~EthernetWidget()
{
    delete ui;
}


void EthernetWidget::clear()
{
    ui->checkBox_skip->setCheckState(Qt::Unchecked);
    ui->checkBox_8021x->setCheckState(Qt::Unchecked);
    ui->lineEdit_wpaConfig->clear();
    ui->comboBox_driver->setCurrentIndex(0);
    ui->spinBox_timeoutCarrier->setValue(5);
    ui->spinBox_timeoutWpa->setValue(15);
}


void EthernetWidget::createActions()
{
    connect(ui->pushButton_ethernetAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->checkBox_8021x, SIGNAL(stateChanged(int)), this, SLOT(showWpa(int)));
}


void EthernetWidget::showAdvanced()
{
    if (ui->pushButton_ethernetAdvanced->text().indexOf(QString("Show")) > -1) {
        ui->widget_ethernetAdvanced->setShown(true);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Hide advanced"));
    }
    else {
        ui->widget_ethernetAdvanced->setHidden(true);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Show advanced"));
    }
    clear();
}


void EthernetWidget::showWpa(int state)
{
    if (state == 0)
        ui->widget_wpa->setHidden(true);
    else
        ui->widget_wpa->setShown(true);
}
