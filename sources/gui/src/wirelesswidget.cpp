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
    changeSecurity(ui->comboBox_security->currentText());
    ui->lineEdit_essid->clear();
    ui->lineEdit_wpaConfigSection->clear();
    ui->listWidget_wpaConfigSection->setCurrentRow(-1);
    ui->listWidget_wpaConfigSection->clear();
    ui->lineEdit_wpaConfig->setText(QString("/etc/wpa_supplicant.conf"));
    ui->lineEdit_key->clear();
    ui->checkBox_hidden->setCheckState(Qt::Unchecked);
    ui->checkBox_adhoc->setCheckState(Qt::Unchecked);
    ui->spinBox_freq->setValue(2412);
    ui->listWidget_freq->setCurrentRow(-1);
    ui->listWidget_freq->clear();
    ui->spinBox_priority->setValue(0);
    ui->lineEdit_country->clear();
    ui->lineEdit_wpaGroup->setText(QString("wheel"));
    ui->comboBox_drivers->setCurrentIndex(0);
    ui->listWidget_drivers->setCurrentRow(-1);
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


void WirelessWidget::setShown(bool state)
{
    if (state)
        show();
    else
        hide();
}


void WirelessWidget::createActions()
{
    connect(ui->pushButton_wirelessAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_drivers, SIGNAL(clicked(bool)), this, SLOT(addDriver()));
    connect(ui->pushButton_freq, SIGNAL(clicked(bool)), this, SLOT(addFreq()));
    connect(ui->pushButton_wpaConfigSection, SIGNAL(clicked(bool)), this, SLOT(addOption()));
    connect(ui->pushButton_wpaConfig, SIGNAL(clicked(bool)), this, SLOT(selectWpaConfig()));
    connect(ui->comboBox_security, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeSecurity(QString)));
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


void WirelessWidget::changeSecurity(QString currentText)
{
    if (currentText == QString("none")) {
        ui->widget_essid->setShown(true);
        ui->widget_wpaConfigSection->setHidden(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setHidden(true);
    }
    else if ((currentText == QString("wep")) ||
             (currentText == QString("wpa"))) {
        ui->widget_essid->setShown(true);
        ui->widget_wpaConfigSection->setHidden(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setShown(true);
    }
    else if (currentText == QString("wpa-configsection")) {
        ui->widget_essid->setHidden(true);
        ui->widget_wpaConfigSection->setShown(true);
        ui->widget_wpaConfig->setHidden(true);
        ui->widget_key->setHidden(true);
    }
    else if (currentText == QString("wpa-config")) {
        ui->widget_essid->setHidden(true);
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
        wirelessSettings[QString("Security")] = ui->comboBox_security->currentText();
        wirelessSettings[QString("ESSID")] = QString("'") + ui->lineEdit_essid->text() + QString("'");
        if ((ui->comboBox_security->currentText() == QString("wep")) ||
                (ui->comboBox_security->currentText() == QString("wpa")))
            wirelessSettings[QString("Key")] = QString("'") + ui->lineEdit_key->text() + QString("'");
        if (ui->comboBox_security->currentText() == QString("wpa-configsection")) {
            QStringList section;
            for (int i=0; i<ui->listWidget_wpaConfigSection->count(); i++)
                section.append(QString("'") + ui->listWidget_wpaConfigSection->item(i)->text() + QString("'"));
            wirelessSettings[QString("WPAConfigSection")] = QString("\n") + section.join(QString("\n")) + QString("\n");
        }
        if (ui->comboBox_security->currentText() == QString("wpa-config"))
            wirelessSettings[QString("WPAConfigFile")] = QString("'") + ui->lineEdit_wpaConfig->text() + QString("'");
        if (ui->checkBox_hidden->checkState() == Qt::Checked)
            wirelessSettings[QString("Hidden")] = QString("yes");
        if (ui->checkBox_adhoc->checkState() == Qt::Checked)
            wirelessSettings[QString("AdHoc")] = QString("yes");
        if (ui->listWidget_freq->count() != 0) {
            QStringList freqs;
            for (int i=0; i<ui->listWidget_freq->count(); i++)
                freqs.append(ui->listWidget_freq->item(i)->text());
            wirelessSettings[QString("ScanFrequencies")] = freqs.join(QString(" "));
        }
        if (ui->spinBox_priority->value() != 0)
            wirelessSettings[QString("Priority")] = QString(ui->spinBox_priority->value());
        if (!ui->lineEdit_country->text().isEmpty())
            wirelessSettings[QString("Country")] = QString("'") + ui->lineEdit_country->text() + QString("'");
        if (ui->lineEdit_wpaGroup->text() != QString("wheel"))
            wirelessSettings[QString("WPAGroup")] = QString("'") + ui->lineEdit_wpaGroup->text() + QString("'");
        if (ui->listWidget_drivers->count() != 0) {
            QStringList drivers;
            for (int i=0; i<ui->listWidget_drivers->count(); i++)
                drivers.append(ui->listWidget_drivers->item(i)->text());
            wirelessSettings[QString("WPADriver")] = QString("'") + drivers.join(QString(",")) + QString("'");
        }
        if (ui->comboBox_rfkill->currentText() != QString("auto"))
            wirelessSettings[QString("RFKill")] = ui->comboBox_rfkill->currentText();
        if (ui->spinBox_timeoutWpa->value() != 15)
            wirelessSettings[QString("TimeoutWPA")] = QString(ui->spinBox_timeoutWpa->value());
        if (ui->checkBox_exclude->checkState() == Qt::Checked)
            wirelessSettings[QString("ExcludeAuto")] = QString("yes");
    }

    return wirelessSettings;
}


int WirelessWidget::isOk()
{
    // empty key
    if ((ui->comboBox_security->currentText() == QString("wep")) ||
            (ui->comboBox_security->currentText() == QString("wpa")))
        if (ui->lineEdit_key->text().isEmpty())
            return 1;
    // empty settings
    if (ui->comboBox_security->currentText() == QString("wpa-configsection"))
        if (ui->listWidget_wpaConfigSection->count() == 0)
            return 2;
    // file is not set
    if (ui->comboBox_security->currentText() == QString("wpa-config"))
        if (ui->lineEdit_wpaConfig->text().isEmpty())
            return 3;
    // file wpa_supplicant doesn't exists
    if (ui->comboBox_security->currentText() == QString("wpa-config"))
        if (!ui->lineEdit_wpaConfig->text().isEmpty())
            if (!QFile(ui->lineEdit_wpaConfig->text()).exists())
                return 4;
    // essid is not set
    if ((ui->comboBox_security->currentText() == QString("none")) ||
            (ui->comboBox_security->currentText() == QString("wep")) ||
            (ui->comboBox_security->currentText() == QString("wpa")))
        if (ui->lineEdit_essid->text().isEmpty())
            return 5;
    // all fine
    return 0;
}


void WirelessWidget::setSettings(QHash<QString, QString> settings)
{
    if (settings.contains(QString("Security")))
        for (int i=0; i<ui->comboBox_security->count(); i++)
            if (settings[QString("Security")].remove(QString("'")) == ui->comboBox_security->itemText(i))
                ui->comboBox_security->setCurrentIndex(i);
    if (settings.contains(QString("ESSID")))
        ui->lineEdit_essid->setText(settings[QString("ESSID")].remove(QString("'")));
    if (settings.contains(QString("Key")))
        ui->lineEdit_key->setText(settings[QString("Key")].remove(QString("'")));
    if (settings.contains(QString("WPAConfigSection")))
        ui->listWidget_wpaConfigSection->addItems(settings[QString("WPAConfigSection")].split(QString("\n")));
    if (settings.contains(QString("WPAConfigFile")))
        ui->lineEdit_wpaConfig->setText(settings[QString("WPAConfigFile")].remove(QString("'")));
    if (settings.contains(QString("Hidden")))
        if (settings[QString("Hidden")].remove(QString("'")) == QString("yes"))
            ui->checkBox_hidden->setCheckState(Qt::Checked);
    if (settings.contains(QString("AdHoc")))
        if (settings[QString("AdHoc")].remove(QString("'")) == QString("yes"))
            ui->checkBox_adhoc->setCheckState(Qt::Checked);
    if (settings.contains(QString("ScanFrequencies")))
        ui->listWidget_freq->addItems(settings[QString("ScanFrequencies")].split(QString("\n")));
    if (settings.contains(QString("Priority")))
        ui->spinBox_priority->setValue(settings[QString("Priority")].toInt());
    if (settings.contains(QString("Country")))
        ui->lineEdit_country->setText(settings[QString("Country")].remove(QString("'")));
    if (settings.contains(QString("WPAGroup")))
        ui->lineEdit_wpaGroup->setText(settings[QString("WPAGroup")].remove(QString("'")));
    if (settings.contains(QString("WPADriver")))
        ui->listWidget_drivers->addItems(settings[QString("WPADriver")].split(QString(",")));
    if (settings.contains(QString("RFKill")))
        for (int i=0; i<ui->comboBox_rfkill->count(); i++)
            if (settings[QString("RFKill")].remove(QString("'")) == ui->comboBox_rfkill->itemText(i))
                ui->comboBox_rfkill->setCurrentIndex(i);
    if (settings.contains(QString("TimeoutWPA")))
        ui->spinBox_timeoutWpa->setValue(settings[QString("TimeoutWPA")].toInt());
    if (settings.contains(QString("ExcludeAuto")))
        if (settings[QString("ExcludeAuto")].remove(QString("'")) == QString("yes"))
            ui->checkBox_exclude->setCheckState(Qt::Checked);

    changeSecurity(ui->comboBox_security->currentText());
}
