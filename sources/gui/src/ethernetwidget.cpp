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

#include <QDir>
#include <QFileDialog>
#include <QFile>


EthernetWidget::EthernetWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EthernetWidget)
{
    ui->setupUi(this);
    createActions();
    clear();
}


EthernetWidget::~EthernetWidget()
{
    delete ui;
}


void EthernetWidget::clear()
{
    ui->checkBox_skip->setCheckState(Qt::Unchecked);
    ui->checkBox_8021x->setCheckState(Qt::Unchecked);
    showWpa(ui->checkBox_8021x->checkState());
    ui->lineEdit_wpaConfig->setText(QString("/etc/wpa_supplicant.conf"));
    ui->comboBox_driver->setCurrentIndex(0);
    ui->spinBox_timeoutCarrier->setValue(5);
    ui->spinBox_timeoutWpa->setValue(15);

    ui->pushButton_ethernetAdvanced->setChecked(false);
    showAdvanced();
}


void EthernetWidget::setShown(bool state)
{
    if (state)
        EthernetWidget::show();
    else
        EthernetWidget::hide();
}


void EthernetWidget::createActions()
{
    connect(ui->pushButton_ethernetAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_wpaConfig, SIGNAL(clicked(bool)), this, SLOT(selectWpaConfig()));
    connect(ui->checkBox_8021x, SIGNAL(stateChanged(int)), this, SLOT(showWpa(int)));
}


void EthernetWidget::selectWpaConfig()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("EthernetWidget", "Select wpa configuration file"),
                QDir::currentPath(),
                QApplication::translate("EthernetWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_wpaConfig->setText(filename);
}


void EthernetWidget::showAdvanced()
{
    if (ui->pushButton_ethernetAdvanced->isChecked()) {
        ui->widget_ethernetAdvanced->setShown(true);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Hide advanced"));
    }
    else {
        ui->widget_ethernetAdvanced->setHidden(true);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Show advanced"));
    }
}


void EthernetWidget::showWpa(int state)
{
    if (state == 0)
        ui->widget_wpa->setHidden(true);
    else
        ui->widget_wpa->setShown(true);
}


QHash<QString, QString> EthernetWidget::getSettings()
{
    QHash<QString, QString> ethernetSettings;

    if (isOk() == 0) {
        if (ui->checkBox_skip->checkState() == Qt::Checked)
            ethernetSettings[QString("SkipNoCarrier")] = QString("yes");
        if (ui->checkBox_8021x->checkState() == Qt::Checked) {
            ethernetSettings[QString("Auth8021X")] = QString("yes");
            ethernetSettings[QString("WPAConfigFile")] = QString("'") + ui->lineEdit_wpaConfig->text() + QString("'");
            ethernetSettings[QString("WPADriver")] = ui->comboBox_driver->currentText();
        }
        if (ui->spinBox_timeoutCarrier->value() != 5)
            ethernetSettings[QString("TimeoutCarrier")] = QString(ui->spinBox_timeoutCarrier->value());
        if (ui->spinBox_timeoutWpa->value() != 15)
            ethernetSettings[QString("TimeoutWPA")] = QString(ui->spinBox_timeoutWpa->value());
    }

    return ethernetSettings;
}


int EthernetWidget::isOk()
{
    // file wpa_supplicant doesn't exists
    if (!ui->lineEdit_wpaConfig->text().isEmpty())
        if (!QFile(ui->lineEdit_wpaConfig->text()).exists())
            return 1;
    // all fine
    return 0;
}


void EthernetWidget::setSettings(QHash<QString, QString> settings)
{
    if (settings.contains(QString("SkipNoCarrier")))
        if (settings[QString("SkipNoCarrier")].remove(QString("'")) == QString("yes"))
            ui->checkBox_skip->setCheckState(Qt::Checked);
    if (settings.contains(QString("Auth8021X")))
        if (settings[QString("Auth8021X")].remove(QString("'")) == QString("yes"))
            ui->checkBox_8021x->setCheckState(Qt::Checked);
    if (settings.contains(QString("WPAConfigFile")))
        ui->lineEdit_wpaConfig->setText(settings[QString("WPAConfigFile")].remove(QString("'")));
    if (settings.contains(QString("WPADriver")))
        for (int i=0; i<ui->comboBox_driver->count(); i++)
            if (settings[QString("WPADriver")].remove(QString("'")) == ui->comboBox_driver->itemText(i))
                ui->comboBox_driver->setCurrentIndex(i);
    if (settings.contains(QString("TimeoutCarrier")))
        ui->spinBox_timeoutCarrier->setValue(settings[QString("TimeoutCarrier")].toInt());
    if (settings.contains(QString("TimeoutWPA")))
        ui->spinBox_timeoutWpa->setValue(settings[QString("TimeoutWPA")].toInt());

    showWpa(ui->checkBox_8021x->checkState());
}
