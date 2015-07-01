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
    ui->spinBox_priority->setValue(1);
    ui->spinBox_timeoutCarrier->setValue(5);
    ui->spinBox_timeoutWpa->setValue(15);

    ui->pushButton_ethernetAdvanced->setChecked(false);
    showAdvanced();
}


void EthernetWidget::createActions()
{
    connect(ui->pushButton_ethernetAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_wpaConfig, SIGNAL(clicked(bool)), this, SLOT(selectWpaConfig()));
    connect(ui->checkBox_8021x, SIGNAL(stateChanged(int)), this, SLOT(showWpa(int)));
}


void EthernetWidget::selectWpaConfig()
{
    QString filename = QFileDialog::getOpenFileName(this,
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
        ui->pushButton_ethernetAdvanced->setArrowType(Qt::UpArrow);
    } else {
        ui->widget_ethernetAdvanced->setHidden(true);
        ui->pushButton_ethernetAdvanced->setArrowType(Qt::DownArrow);
    }
}


void EthernetWidget::showWpa(const int state)
{
    ui->widget_wpa->setHidden(state == 0);
}


QMap<QString, QString> EthernetWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    if (ui->checkBox_skip->checkState() == Qt::Checked)
        settings[QString("SkipNoCarrier")] = QString("yes");
    if (ui->checkBox_8021x->checkState() == Qt::Checked) {
        settings[QString("Auth8021X")] = QString("yes");
        settings[QString("WPAConfigFile")] = QString("'%1'").arg(ui->lineEdit_wpaConfig->text());
        settings[QString("WPADriver")] = ui->comboBox_driver->currentText();
    }
    if (ui->spinBox_priority->value() != 1)
        settings[QString("Priority")] = QString::number(ui->spinBox_priority->value());
    if (ui->spinBox_timeoutCarrier->value() != 5)
        settings[QString("TimeoutCarrier")] = QString::number(ui->spinBox_timeoutCarrier->value());
    if (ui->spinBox_timeoutWpa->value() != 15)
        settings[QString("TimeoutWPA")] = QString::number(ui->spinBox_timeoutWpa->value());

    return settings;
}


int EthernetWidget::isOk()
{
    // file wpa_supplicant doesn't exists
    if (!ui->lineEdit_wpaConfig->text().isEmpty())
        if (!QFile(ui->lineEdit_wpaConfig->text()).exists()) return 1;
    // all fine
    return 0;
}


void EthernetWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("SkipNoCarrier")))
        if (settings[QString("SkipNoCarrier")] == QString("yes"))
            ui->checkBox_skip->setCheckState(Qt::Checked);
    if (settings.contains(QString("Auth8021X")))
        if (settings[QString("Auth8021X")] == QString("yes"))
            ui->checkBox_8021x->setCheckState(Qt::Checked);
    if (settings.contains(QString("WPAConfigFile")))
        ui->lineEdit_wpaConfig->setText(settings[QString("WPAConfigFile")]);
    if (settings.contains(QString("WPADriver"))) {
        int index = ui->comboBox_driver->findText(settings[QString("WPADriver")]);
        ui->comboBox_driver->setCurrentIndex(index);
    }
    if (settings.contains(QString("Priority")))
        ui->spinBox_priority->setValue(settings[QString("Priority")].toInt());
    if (settings.contains(QString("TimeoutCarrier")))
        ui->spinBox_timeoutCarrier->setValue(settings[QString("TimeoutCarrier")].toInt());
    if (settings.contains(QString("TimeoutWPA")))
        ui->spinBox_timeoutWpa->setValue(settings[QString("TimeoutWPA")].toInt());

    showWpa(ui->checkBox_8021x->checkState());
}
