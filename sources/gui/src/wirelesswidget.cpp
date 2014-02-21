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

#include "wirelesswidget.h"
#include "ui_wirelesswidget.h"

#include <QFileDialog>
#include <QFile>
#include <QKeyEvent>


WirelessWidget::WirelessWidget(QWidget *parent, QString rfkillDir)
    : QWidget(parent),
      rfkillDirectory(new QDir(rfkillDir)),
      ui(new Ui::WirelessWidget)
{
    ui->setupUi(this);
    createActions();
    clear();
}


WirelessWidget::~WirelessWidget()
{
    delete rfkillDirectory;
    delete ui;
}


void WirelessWidget::clear()
{
    ui->comboBox_security->setCurrentIndex(0);
    changeSecurity(ui->comboBox_security->currentIndex());
    ui->lineEdit_essid->clear();
    ui->lineEdit_wpaConfigSection->clear();
    ui->listWidget_wpaConfigSection->clear();
    ui->lineEdit_wpaConfig->setText(QString("/etc/wpa_supplicant.conf"));
    ui->lineEdit_key->clear();
    ui->checkBox_hidden->setCheckState(Qt::Unchecked);
    ui->checkBox_adhoc->setCheckState(Qt::Unchecked);
    ui->spinBox_freq->setValue(2412);
    ui->listWidget_freq->clear();
    ui->spinBox_priority->setValue(0);
    ui->lineEdit_country->clear();
    ui->lineEdit_wpaGroup->clear();
    ui->comboBox_drivers->setCurrentIndex(0);
    ui->listWidget_drivers->clear();
    ui->comboBox_rfkill->addItem(QString("auto"));
    if (rfkillDirectory->exists()) {
        QStringList rfkillDevices = rfkillDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i=0; i<rfkillDevices.count(); i++) {
            QFile device(rfkillDirectory->absolutePath() + QDir::separator() + rfkillDevices[i] +
                         QDir::separator() + QString("name"));
            QString deviceName;
            if (device.open(QIODevice::ReadOnly))
                deviceName = QString(device.readLine()).remove(QString("\n"));
            ui->comboBox_rfkill->addItem(deviceName);
        }
    }
    ui->comboBox_rfkill->setCurrentIndex(0);
    ui->spinBox_timeoutWpa->setValue(15);
    ui->checkBox_exclude->setCheckState(Qt::Unchecked);

    ui->pushButton_wirelessAdvanced->setChecked(false);
    showAdvanced();
}


void WirelessWidget::createActions()
{
    connect(ui->pushButton_wirelessAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_drivers, SIGNAL(clicked(bool)), this, SLOT(addDriver()));
    connect(ui->pushButton_freq, SIGNAL(clicked(bool)), this, SLOT(addFreq()));
    connect(ui->pushButton_wpaConfigSection, SIGNAL(clicked(bool)), this, SLOT(addOption()));
    connect(ui->pushButton_wpaConfig, SIGNAL(clicked(bool)), this, SLOT(selectWpaConfig()));
    connect(ui->comboBox_security, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSecurity(int)));
}


void WirelessWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Delete) {
        if (ui->listWidget_wpaConfigSection->hasFocus() &&
                (ui->listWidget_wpaConfigSection->currentItem() != 0))
            delete ui->listWidget_wpaConfigSection->currentItem();
        else if (ui->listWidget_freq->hasFocus() &&
                 (ui->listWidget_freq->currentItem() != 0))
             delete ui->listWidget_freq->currentItem();
        else if (ui->listWidget_drivers->hasFocus() &&
                 (ui->listWidget_drivers->currentItem() != 0))
             delete ui->listWidget_drivers->currentItem();
    }
}


void WirelessWidget::addDriver()
{
    QString driver = ui->comboBox_drivers->currentText();
    bool exists = false;
    for (int i=0; i<ui->listWidget_drivers->count(); i++)
        if (ui->listWidget_drivers->item(i)->text() == driver)
            exists = true;
    if (!exists)
        ui->listWidget_drivers->addItem(driver);
}


void WirelessWidget::addFreq()
{
    QString freq = QString(ui->spinBox_freq->value());
    bool exists = false;
    for (int i=0; i<ui->listWidget_freq->count(); i++)
        if (ui->listWidget_freq->item(i)->text() == freq)
            exists = true;
    if (!exists)
        ui->listWidget_freq->addItem(freq);
}


void WirelessWidget::addOption()
{
    if (!ui->lineEdit_wpaConfigSection->text().isEmpty())
        ui->listWidget_wpaConfigSection->addItem(ui->lineEdit_wpaConfigSection->text());
}


void WirelessWidget::changeSecurity(int index)
{
    if (index == 0) {
        ui->widget_wpaConfigSection->setHidden(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setHidden(true);
    }
    else if ((index == 1) || (index == 2)) {
        ui->widget_wpaConfigSection->setHidden(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setShown(true);
    }
    else if (index == 3) {
        ui->widget_wpaConfigSection->setShown(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setHidden(true);
    }
    else if (index == 4) {
        ui->widget_wpaConfigSection->setHidden(true);
        ui->widget_wpaConfig->setShown(true);
        ui->widget_key->setHidden(true);
    }
}


void WirelessWidget::selectWpaConfig()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("EthernetWidget", "Select wpa configuration file"),
                QDir::currentPath(),
                QApplication::translate("EthernetWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_wpaConfig->setText(filename);
}


void WirelessWidget::showAdvanced()
{
    if (ui->pushButton_wirelessAdvanced->isChecked()) {
        ui->widget_wirelessAdvanced->setShown(true);
        ui->pushButton_wirelessAdvanced->setText(QApplication::translate("WirelessWidget", "Hide advanced"));
    }
    else {
        ui->widget_wirelessAdvanced->setHidden(true);
        ui->pushButton_wirelessAdvanced->setText(QApplication::translate("WirelessWidget", "Show advanced"));
    }
}


QHash<QString, QString> WirelessWidget::getSettings()
{
    QHash<QString, QString> wirelessSettings;

    if (isOk() == 0) {
//        wirelessSettings[QString("VLANID")] = QString(ui->spinBox_vlan->value());
        clear();
    }

    return wirelessSettings;
}


int WirelessWidget::isOk()
{
    // all fine
    return 0;
}
