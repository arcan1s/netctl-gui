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

#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

#include "mainwindow.h"


SettingsWindow::SettingsWindow(MainWindow *wid, const QString configFile)
    : QMainWindow(wid),
      parent(wid),
      file(configFile),
      ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    addLanguages();
    createActions();
}


SettingsWindow::~SettingsWindow()
{
    delete ui;
}


void SettingsWindow::createActions()
{
    connect(ui->comboBox_language, SIGNAL(currentIndexChanged(int)), ui->label_info, SLOT(show()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(setDefault()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(close()));
    // buttons
    connect(ui->pushButton_interfaceDir, SIGNAL(clicked(bool)), SLOT(selectIfaceDir()));
    connect(ui->pushButton_netctlPath, SIGNAL(clicked(bool)), SLOT(selectNetctlPath()));
    connect(ui->pushButton_profilePath, SIGNAL(clicked(bool)), SLOT(selectProfileDir()));
    connect(ui->pushButton_rfkill, SIGNAL(clicked(bool)), SLOT(selectRfkillDir()));
    connect(ui->pushButton_sudo, SIGNAL(clicked(bool)), SLOT(selectSudoPath()));
    connect(ui->pushButton_wpaCliPath, SIGNAL(clicked(bool)), SLOT(selectWpaCliPath()));
    connect(ui->pushButton_wpaSupPath, SIGNAL(clicked(bool)), SLOT(selectWpaSupPath()));
}


// ESC press event
void SettingsWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Escape)
        close();
}


void SettingsWindow::addLanguages()
{
    ui->comboBox_language->addItem(QString("english"));
    ui->comboBox_language->addItem(QString("russian"));
}


void SettingsWindow::saveSettings()
{
    QMap<QString, QString> settings = readSettings();
    QFile configFile(file);

    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&configFile);
    for (int i=0; i<settings.keys().count(); i++)
        out << settings.keys()[i] << QString("=") << settings[settings.keys()[i]] << QString("\n");
    configFile.close();
}


void SettingsWindow::setDefault()
{
    setSettings(getDefault());
    saveSettings();
}


void SettingsWindow::selectIfaceDir()
{
    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to directory with interfaces"),
                QString("/sys/"));
    if (!directory.isEmpty())
        ui->lineEdit_interfacesDir->setText(directory);
}


void SettingsWindow::selectNetctlPath()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select netctl command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_netctlPath->setText(filename);
}


void SettingsWindow::selectProfileDir()
{
    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to profile directory"),
                QString("/etc/"));
    if (!directory.isEmpty())
        ui->lineEdit_profilePath->setText(directory);
}


void SettingsWindow::selectRfkillDir()
{
    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to directory with rfkill devices"),
                QString("/sys/"));
    if (!directory.isEmpty())
        ui->lineEdit_rfkill->setText(directory);
}


void SettingsWindow::selectSudoPath()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select sudo command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_sudo->setText(filename);
}


void SettingsWindow::selectWpaCliPath()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select wpa_cli command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_wpaCliPath->setText(filename);
}


void SettingsWindow::selectWpaSupPath()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select wpa_supplicant command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_wpaSupPath->setText(filename);
}


void SettingsWindow::showWindow()
{
    setSettings(getSettings());
    ui->label_info->hide();
    show();
}


QMap<QString, QString> SettingsWindow::readSettings()
{
    QMap<QString, QString> settings;

    settings[QString("CTRL_DIR")] = ui->lineEdit_wpaDir->text();
    settings[QString("CTRL_GROUP")] = ui->lineEdit_wpaGroup->text();
    settings[QString("IFACE_DIR")] = ui->lineEdit_interfacesDir->text();
    settings[QString("LANGUAGE")] = ui->comboBox_language->currentText();
    settings[QString("NETCTL_PATH")] = ui->lineEdit_netctlPath->text();
    settings[QString("PID_FILE")] = ui->lineEdit_pid->text();
    settings[QString("PREFERED_IFACE")] = ui->lineEdit_interface->text();
    settings[QString("PROFILE_DIR")] = ui->lineEdit_profilePath->text();
    settings[QString("RFKILL_DIR")] = ui->lineEdit_rfkill->text();
    settings[QString("SUDO_PATH")] = ui->lineEdit_sudo->text();
    settings[QString("WPACLI_PATH")] = ui->lineEdit_wpaCliPath->text();
    settings[QString("WPASUP_PATH")] = ui->lineEdit_wpaSupPath->text();
    settings[QString("WPA_DRIVERS")] = ui->lineEdit_wpaSupDrivers->text();

    for (int i=0; i<settings.keys().count(); i++)
        qDebug() << "[SettingsWindow]" << "[readSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void SettingsWindow::setSettings(const QMap<QString, QString> settings)
{
    ui->lineEdit_wpaDir->setText(settings[QString("CTRL_DIR")]);
    ui->lineEdit_wpaGroup->setText(settings[QString("CTRL_GROUP")]);
    ui->lineEdit_interfacesDir->setText(settings[QString("IFACE_DIR")]);
    ui->comboBox_language->setCurrentIndex(0);
    for (int i=0; i<ui->comboBox_language->count(); i++)
        if (ui->comboBox_language->itemText(i) == settings[QString("LANGUAGE")])
            ui->comboBox_language->setCurrentIndex(i);
    ui->lineEdit_netctlPath->setText(settings[QString("NETCTL_PATH")]);
    ui->lineEdit_pid->setText(settings[QString("PID_FILE")]);
    ui->lineEdit_interface->setText(settings[QString("PREFERED_IFACE")]);
    ui->lineEdit_profilePath->setText(settings[QString("PROFILE_DIR")]);
    ui->lineEdit_rfkill->setText(settings[QString("RFKILL_DIR")]);
    ui->lineEdit_sudo->setText(settings[QString("SUDO_PATH")]);
    ui->lineEdit_wpaCliPath->setText(settings[QString("WPACLI_PATH")]);
    ui->lineEdit_wpaSupPath->setText(settings[QString("WPASUP_PATH")]);
    ui->lineEdit_wpaSupDrivers->setText(settings[QString("WPA_DRIVERS")]);

    for (int i=0; i<settings.keys().count(); i++)
        qDebug() << "[SettingsWindow]" << "[setSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];
}


QMap<QString, QString> SettingsWindow::getDefault()
{
    QMap<QString, QString> settings;

    settings[QString("CTRL_DIR")] = QString("/run/wpa_supplicant_netctl-gui");
    settings[QString("CTRL_GROUP")] = QString("users");
    settings[QString("IFACE_DIR")] = QString("/sys/class/net/");
    settings[QString("LANGUAGE")] = QString("english");
    settings[QString("NETCTL_PATH")] = QString("/usr/bin/netctl-gui-netctl");
    settings[QString("PID_FILE")] = QString("/run/wpa_supplicant_netctl-gui.pid");
    settings[QString("PREFERED_IFACE")] = QString("");
    settings[QString("PROFILE_DIR")] = QString("/etc/netctl/");
    settings[QString("RFKILL_DIR")] = QString("/sys/class/rfkill/");
    settings[QString("SUDO_PATH")] = QString("/usr/bin/kdesu");
    settings[QString("WPACLI_PATH")] = QString("/usr/bin/wpa_cli");
    settings[QString("WPASUP_PATH")] = QString("/usr/bin/netctl-gui-wpa_supplicant");
    settings[QString("WPA_DRIVERS")] = QString("nl80211,wext");

    for (int i=0; i<settings.keys().count(); i++)
        qDebug() << "[SettingsWindow]" << "[getDefault]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


QMap<QString, QString> SettingsWindow::getSettings()
{
    QMap<QString, QString> settings;
    QFile configFile(file);
    QString fileStr;

    if (!configFile.open(QIODevice::ReadOnly))
        return getDefault();
    while (true) {
        fileStr = QString(configFile.readLine());
        if (fileStr[0] != '#') {
            if (fileStr.contains(QString("=")))
                settings[fileStr.split(QString("="))[0]] = fileStr.split(QString("="))[1]
                        .remove(QString(" "))
                        .trimmed();
        }
        if (configFile.atEnd())
            break;
    }

    configFile.close();

    for (int i=0; i<settings.keys().count(); i++)
        qDebug() << "[SettingsWindow]" << "[getSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}
