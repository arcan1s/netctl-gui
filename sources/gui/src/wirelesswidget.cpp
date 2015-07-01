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

#include "wirelesswidget.h"
#include "ui_wirelesswidget.h"

#include <QFileDialog>
#include <QFile>
#include <QKeyEvent>


WirelessWidget::WirelessWidget(QWidget *parent, const QMap<QString, QString> settings)
    : QWidget(parent),
      ui(new Ui::WirelessWidget)
{
    ui->setupUi(this);
    rfkillDirectory = new QDir(settings[QString("RFKILL_DIR")]);
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
    ui->lineEdit_wpaConfig->setText(QString("/etc/wpa_supplicant/wpa_supplicant.conf"));
    ui->checkBox_key->setCheckState(Qt::Unchecked);
    ui->lineEdit_key->clear();
    ui->checkBox_hidden->setCheckState(Qt::Unchecked);
    ui->checkBox_adhoc->setCheckState(Qt::Unchecked);
    ui->spinBox_freq->setValue(2412);
    ui->listWidget_freq->setCurrentRow(-1);
    ui->listWidget_freq->clear();
    ui->spinBox_frequency->setValue(2000);
    ui->spinBox_priority->setValue(0);
    ui->lineEdit_country->clear();
    ui->lineEdit_wpaGroup->setText(QString("wheel"));
    ui->comboBox_drivers->setCurrentIndex(0);
    ui->listWidget_drivers->setCurrentRow(-1);
    ui->listWidget_drivers->clear();
    ui->comboBox_rfkill->clear();
    ui->comboBox_rfkill->addItem(QString("auto"));
    if (rfkillDirectory->exists()) {
        QStringList rfkillDevices = rfkillDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i=0; i<rfkillDevices.count(); i++) {
            QFile device(QString("%1/%2/name").arg(rfkillDirectory->absolutePath()).arg(rfkillDevices[i]));
            if (!device.open(QIODevice::ReadOnly)) continue;
            ui->comboBox_rfkill->addItem(QString(device.readLine()).trimmed());
            device.close();
        }
    }
    ui->comboBox_rfkill->setCurrentIndex(0);
    ui->spinBox_timeoutWpa->setValue(15);

    ui->pushButton_wirelessAdvanced->setChecked(false);
    showAdvanced();
}


void WirelessWidget::createActions()
{
    connect(ui->pushButton_wirelessAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_drivers, SIGNAL(clicked(bool)), this, SLOT(addDriver()));
    connect(ui->pushButton_freq, SIGNAL(clicked(bool)), this, SLOT(addFreq()));
    connect(ui->spinBox_freq, SIGNAL(editingFinished()), this, SLOT(addFreq()));
    connect(ui->pushButton_wpaConfigSection, SIGNAL(clicked(bool)), this, SLOT(addOption()));
    connect(ui->lineEdit_wpaConfigSection, SIGNAL(returnPressed()), this, SLOT(addOption()));
    connect(ui->pushButton_wpaConfig, SIGNAL(clicked(bool)), this, SLOT(selectWpaConfig()));
    connect(ui->comboBox_security, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeSecurity(QString)));
    connect(ui->checkBox_key, SIGNAL(stateChanged(int)), this, SLOT(setKeyEchoMode(int)));
}


void WirelessWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Delete) {
        if (ui->listWidget_wpaConfigSection->hasFocus() &&
            (ui->listWidget_wpaConfigSection->currentItem() != nullptr))
            delete ui->listWidget_wpaConfigSection->currentItem();
        else if (ui->listWidget_freq->hasFocus() &&
                 (ui->listWidget_freq->currentItem() != nullptr))
            delete ui->listWidget_freq->currentItem();
        else if (ui->listWidget_drivers->hasFocus() &&
                 (ui->listWidget_drivers->currentItem() != nullptr))
            delete ui->listWidget_drivers->currentItem();
    }
}


void WirelessWidget::addDriver()
{
    QString driver = ui->comboBox_drivers->currentText();
    if (ui->listWidget_drivers->findItems(driver, Qt::MatchExactly).count() == 0)
        ui->listWidget_drivers->addItem(driver);
}


void WirelessWidget::addFreq()
{
    QString freq = QString::number(ui->spinBox_freq->value());
    if (ui->listWidget_freq->findItems(freq, Qt::MatchExactly).count() == 0)
        ui->listWidget_freq->addItem(freq);
}


void WirelessWidget::addOption()
{
    if (!ui->lineEdit_wpaConfigSection->text().isEmpty())
        ui->listWidget_wpaConfigSection->addItem(ui->lineEdit_wpaConfigSection->text());
}


void WirelessWidget::changeSecurity(const QString currentText)
{
    ui->widget_essid->setHidden((currentText != QString("none")) &&
                                (currentText != QString("wep")) &&
                                (currentText != QString("wpa")));
    ui->widget_key->setHidden((currentText != QString("wep")) && (currentText != QString("wpa")));
    ui->widget_wpaConfig->setHidden(currentText != QString("wpa-config"));
    ui->widget_wpaConfigSection->setHidden(currentText != QString("wpa-configsection"));
}


void WirelessWidget::selectWpaConfig()
{
    QString filename = QFileDialog::getOpenFileName(this,
                QApplication::translate("EthernetWidget", "Select wpa configuration file"),
                QDir::currentPath(),
                QApplication::translate("EthernetWidget", "Configuration files (*.conf)"));
    if (!filename.isEmpty())
        ui->lineEdit_wpaConfig->setText(filename);
}


void WirelessWidget::setKeyEchoMode(const int mode)
{
    if (mode == 0)
        ui->lineEdit_key->setEchoMode(QLineEdit::Password);
    else
        ui->lineEdit_key->setEchoMode(QLineEdit::Normal);
}


void WirelessWidget::showAdvanced()
{
    if (ui->pushButton_wirelessAdvanced->isChecked()) {
        ui->widget_wirelessAdvanced->setHidden(false);
        ui->pushButton_wirelessAdvanced->setArrowType(Qt::UpArrow);
    } else {
        ui->widget_wirelessAdvanced->setHidden(true);
        ui->pushButton_wirelessAdvanced->setArrowType(Qt::DownArrow);
    }
}


QMap<QString, QString> WirelessWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    settings[QString("Security")] = ui->comboBox_security->currentText();
    settings[QString("ESSID")] = QString("'%1'").arg(ui->lineEdit_essid->text());
    if (ui->comboBox_security->currentText() == QString("wep")) {
        if (ui->lineEdit_key->text().indexOf(QString("\\\"")) == 0)
            settings[QString("Key")] = ui->lineEdit_key->text();
        else
            settings[QString("Key")] = QString("'%1'").arg(ui->lineEdit_key->text());
    } else if (ui->comboBox_security->currentText() == QString("wpa"))
        settings[QString("Key")] = QString("'%1'").arg(ui->lineEdit_key->text());
    if (ui->comboBox_security->currentText() == QString("wpa-configsection")) {
        QStringList section;
        for (int i=0; i<ui->listWidget_wpaConfigSection->count(); i++)
            section.append(QString("'%1'").arg(ui->listWidget_wpaConfigSection->item(i)->text()));
        settings[QString("WPAConfigSection")] = QString("\n%1\n").arg(section.join(QChar('\n')));
    }
    if (ui->comboBox_security->currentText() == QString("wpa-config"))
        settings[QString("WPAConfigFile")] = QString("'%1'").arg(ui->lineEdit_wpaConfig->text());
    if (ui->checkBox_hidden->checkState() == Qt::Checked)
        settings[QString("Hidden")] = QString("yes");
    if (ui->checkBox_adhoc->checkState() == Qt::Checked)
        settings[QString("AdHoc")] = QString("yes");
    if (ui->listWidget_freq->count() != 0) {
        QStringList freqs;
        for (int i=0; i<ui->listWidget_freq->count(); i++)
            freqs.append(ui->listWidget_freq->item(i)->text());
        settings[QString("ScanFrequencies")] = freqs.join(QChar(' '));
    }
    if (ui->spinBox_frequency->value() != 2000)
        settings[QString("Frequency")] = QString::number(ui->spinBox_frequency->value());
    if (ui->spinBox_priority->value() != 0)
        settings[QString("Priority")] = QString::number(ui->spinBox_priority->value());
    if (!ui->lineEdit_country->text().isEmpty())
        settings[QString("Country")] = QString("'%1'").arg(ui->lineEdit_country->text());
    if (ui->lineEdit_wpaGroup->text() != QString("wheel"))
        settings[QString("WPAGroup")] = QString("'%1'").arg(ui->lineEdit_wpaGroup->text());
    if (ui->listWidget_drivers->count() != 0) {
        QStringList drivers;
        for (int i=0; i<ui->listWidget_drivers->count(); i++)
            drivers.append(ui->listWidget_drivers->item(i)->text());
        settings[QString("WPADriver")] = QString("'%1'").arg(drivers.join(QChar(',')));
    }
    if (ui->comboBox_rfkill->currentText() != QString("auto"))
        settings[QString("RFKill")] = ui->comboBox_rfkill->currentText();
    if (ui->spinBox_timeoutWpa->value() != 15)
        settings[QString("TimeoutWPA")] = QString::number(ui->spinBox_timeoutWpa->value());

    return settings;
}


int WirelessWidget::isOk()
{
    // empty key
    if ((ui->comboBox_security->currentText() == QString("wep")) ||
        (ui->comboBox_security->currentText() == QString("wpa")))
        if (ui->lineEdit_key->text().isEmpty()) return 1;
    // empty settings
    if (ui->comboBox_security->currentText() == QString("wpa-configsection"))
        if (ui->listWidget_wpaConfigSection->count() == 0) return 2;
    // file is not set
    if (ui->comboBox_security->currentText() == QString("wpa-config"))
        if (ui->lineEdit_wpaConfig->text().isEmpty()) return 3;
    // file wpa_supplicant doesn't exists
    if (ui->comboBox_security->currentText() == QString("wpa-config"))
        if (!QFile(ui->lineEdit_wpaConfig->text()).exists()) return 4;
    // essid is not set
    if ((ui->comboBox_security->currentText() == QString("none")) ||
        (ui->comboBox_security->currentText() == QString("wep")) ||
        (ui->comboBox_security->currentText() == QString("wpa")))
        if (ui->lineEdit_essid->text().isEmpty()) return 5;
    // all fine
    return 0;
}


void WirelessWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("Security"))) {
        int index = ui->comboBox_security->findText(settings[QString("Security")]);
        ui->comboBox_security->setCurrentIndex(index);
    }
    if (settings.contains(QString("ESSID")))
        ui->lineEdit_essid->setText(settings[QString("ESSID")]);
    if (settings.contains(QString("Key"))) {
        QString key = settings[QString("Key")];
        // workaround for wireless-wep example
        if (settings.contains(QString("Security")))
            if ((settings[QString("Security")] == QString("wep")) &&
                (key[0] == QChar('"')))
              key = QChar('\\') + key;
        ui->lineEdit_key->setText(key);
    }
    if (settings.contains(QString("WPAConfigSection")))
        ui->listWidget_wpaConfigSection->addItems(settings[QString("WPAConfigSection")].split(QChar('\n')));
    if (settings.contains(QString("WPAConfigFile")))
        ui->lineEdit_wpaConfig->setText(settings[QString("WPAConfigFile")]);
    if (settings.contains(QString("Hidden")))
        if (settings[QString("Hidden")] == QString("yes"))
            ui->checkBox_hidden->setCheckState(Qt::Checked);
    if (settings.contains(QString("AdHoc")))
        if (settings[QString("AdHoc")] == QString("yes"))
            ui->checkBox_adhoc->setCheckState(Qt::Checked);
    if (settings.contains(QString("ScanFrequencies")))
        ui->listWidget_freq->addItems(settings[QString("ScanFrequencies")].split(QChar('\n')));
    if (settings.contains(QString("Frequency")))
        ui->spinBox_frequency->setValue(settings[QString("Frequency")].toInt());
    if (settings.contains(QString("Priority")))
        ui->spinBox_priority->setValue(settings[QString("Priority")].toInt());
    if (settings.contains(QString("Country")))
        ui->lineEdit_country->setText(settings[QString("Country")]);
    if (settings.contains(QString("WPAGroup")))
        ui->lineEdit_wpaGroup->setText(settings[QString("WPAGroup")]);
    if (settings.contains(QString("WPADriver")))
        ui->listWidget_drivers->addItems(settings[QString("WPADriver")].split(QChar(',')));
    if (settings.contains(QString("RFKill"))) {
        int index = ui->comboBox_rfkill->findText(settings[QString("RFKill")]);
        ui->comboBox_rfkill->setCurrentIndex(index);
    }
    if (settings.contains(QString("TimeoutWPA")))
        ui->spinBox_timeoutWpa->setValue(settings[QString("TimeoutWPA")].toInt());

    changeSecurity(ui->comboBox_security->currentText());
}
