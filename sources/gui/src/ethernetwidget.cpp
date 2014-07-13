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
    ui->lineEdit_wpaConfig->clear();
    ui->comboBox_driver->setCurrentIndex(0);
    ui->spinBox_timeoutCarrier->setValue(5);
    ui->spinBox_timeoutWpa->setValue(15);

    ui->pushButton_ethernetAdvanced->setChecked(false);
    showAdvanced();
}


void EthernetWidget::setShown(const bool state)
{
    if (state)
        show();
    else
        hide();
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
        ui->widget_ethernetAdvanced->setHidden(false);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Hide advanced"));
    }
    else {
        ui->widget_ethernetAdvanced->setHidden(true);
        ui->pushButton_ethernetAdvanced->setText(QApplication::translate("EthernetWidget", "Show advanced"));
    }
}


void EthernetWidget::showWpa(const int state)
{
    if (state == 0)
        ui->widget_wpa->setHidden(true);
    else
        ui->widget_wpa->setHidden(false);
}


QMap<QString, QString> EthernetWidget::getSettings()
{
    QMap<QString, QString> ethernetSettings;

    if (isOk() != 0)
        return ethernetSettings;

    if (ui->checkBox_skip->checkState() == Qt::Checked)
        ethernetSettings[QString("SkipNoCarrier")] = QString("yes");
    if (ui->checkBox_8021x->checkState() == Qt::Checked) {
        ethernetSettings[QString("Auth8021X")] = QString("yes");
        ethernetSettings[QString("WPAConfigFile")] = QString("'") + ui->lineEdit_wpaConfig->text() + QString("'");
        ethernetSettings[QString("WPADriver")] = ui->comboBox_driver->currentText();
    }
    if (ui->spinBox_timeoutCarrier->value() != 5)
        ethernetSettings[QString("TimeoutCarrier")] = QString::number(ui->spinBox_timeoutCarrier->value());
    if (ui->spinBox_timeoutWpa->value() != 15)
        ethernetSettings[QString("TimeoutWPA")] = QString::number(ui->spinBox_timeoutWpa->value());

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


void EthernetWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();
    QMap<QString, QString> ethernetSettings = settings;

    if (ethernetSettings.contains(QString("SkipNoCarrier")))
        if (ethernetSettings[QString("SkipNoCarrier")] == QString("yes"))
            ui->checkBox_skip->setCheckState(Qt::Checked);
    if (ethernetSettings.contains(QString("Auth8021X")))
        if (ethernetSettings[QString("Auth8021X")] == QString("yes"))
            ui->checkBox_8021x->setCheckState(Qt::Checked);
    if (ethernetSettings.contains(QString("WPAConfigFile")))
        ui->lineEdit_wpaConfig->setText(ethernetSettings[QString("WPAConfigFile")]);
    if (ethernetSettings.contains(QString("WPADriver")))
        for (int i=0; i<ui->comboBox_driver->count(); i++)
            if (ethernetSettings[QString("WPADriver")] == ui->comboBox_driver->itemText(i))
                ui->comboBox_driver->setCurrentIndex(i);
    if (ethernetSettings.contains(QString("TimeoutCarrier")))
        ui->spinBox_timeoutCarrier->setValue(ethernetSettings[QString("TimeoutCarrier")].toInt());
    if (ethernetSettings.contains(QString("TimeoutWPA")))
        ui->spinBox_timeoutWpa->setValue(ethernetSettings[QString("TimeoutWPA")].toInt());

    showWpa(ui->checkBox_8021x->checkState());
}
