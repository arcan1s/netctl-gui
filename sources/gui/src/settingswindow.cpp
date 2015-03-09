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

#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>

#include <language/language.h>
#include <pdebug/pdebug.h>

#include "mainwindow.h"


SettingsWindow::SettingsWindow(QWidget *parent, const bool debugCmd, const QString configFile)
    : QMainWindow(parent),
      debug(debugCmd),
      file(configFile),
      ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    addLanguages();
    createActions();
}


SettingsWindow::~SettingsWindow()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


void SettingsWindow::createActions()
{
    if (debug) qDebug() << PDEBUG;

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(closeWindow()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(restoreSettings()));
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked(bool)), this, SLOT(setDefault()));
    connect(ui->checkBox_enableTray, SIGNAL(stateChanged(int)), this, SLOT(setTray()));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(changePage(QTreeWidgetItem *, QTreeWidgetItem *)));
    // buttons
    connect(ui->pushButton_helperPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_interfacesDir, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_netctlPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_netctlAutoPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_profilePath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_rfkill, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_status, SIGNAL(clicked(bool)), this, SLOT(startHelper()));
    connect(ui->pushButton_sudo, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_systemctlPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_wpaCliPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
    connect(ui->pushButton_wpaSupPath, SIGNAL(clicked(bool)), this, SLOT(selectAbstractSomething()));
}


// ESC press event
void SettingsWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << PDEBUG;

    if (pressedKey->key() == Qt::Key_Escape)
        close();
}


void SettingsWindow::addLanguages()
{
    if (debug) qDebug() << PDEBUG;

    ui->comboBox_language->clear();
    ui->comboBox_language->addItems(Language::getAvailableLanguages());
}


void SettingsWindow::changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << PDEBUG;

    for (int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
        if (current == ui->treeWidget->topLevelItem(i)) {
            ui->stackedWidget->setCurrentIndex(i);
            break;
        }
}


void SettingsWindow::closeWindow()
{
    if (debug) qDebug() << PDEBUG;

    saveSettings();
    close();
    dynamic_cast<MainWindow *>(parent())->updateConfiguration();
}


void SettingsWindow::saveSettings()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config = readSettings();
    QSettings settings(file, QSettings::IniFormat);

    settings.beginGroup(QString("Common"));
    settings.setValue(QString("LANGUAGE"), config[QString("LANGUAGE")]);
    settings.setValue(QString("SYSTRAY"), config[QString("SYSTRAY")]);
    settings.setValue(QString("CLOSETOTRAY"), config[QString("CLOSETOTRAY")]);
    settings.setValue(QString("STARTTOTRAY"), config[QString("STARTTOTRAY")]);
    settings.setValue(QString("SKIPCOMPONENTS"), config[QString("SKIPCOMPONENTS")]);
    settings.endGroup();

    settings.beginGroup(QString("Helper"));
    settings.setValue(QString("USE_HELPER"), config[QString("USE_HELPER")]);
    settings.setValue(QString("HELPER_GROUP"), config[QString("HELPER_GROUP")]);
    settings.setValue(QString("FORCE_SUDO"), config[QString("FORCE_SUDO")]);
    settings.setValue(QString("CLOSE_HELPER"), config[QString("CLOSE_HELPER")]);
    settings.setValue(QString("HELPER_PATH"), config[QString("HELPER_PATH")]);
    settings.setValue(QString("HELPER_SERVICE"), config[QString("HELPER_SERVICE")]);
    settings.endGroup();

    settings.beginGroup(QString("netctl"));
    settings.setValue(QString("SYSTEMCTL_PATH"), config[QString("SYSTEMCTL_PATH")]);
    settings.setValue(QString("NETCTL_PATH"), config[QString("NETCTL_PATH")]);
    settings.setValue(QString("NETCTLAUTO_PATH"), config[QString("NETCTLAUTO_PATH")]);
    settings.setValue(QString("NETCTLAUTO_PATH"), config[QString("NETCTLAUTO_PATH")]);
    settings.setValue(QString("PROFILE_DIR"), config[QString("PROFILE_DIR")]);
    settings.endGroup();

    settings.beginGroup(QString("sudo"));
    settings.setValue(QString("SUDO_PATH"), config[QString("SUDO_PATH")]);
    settings.endGroup();

    settings.beginGroup(QString("wpa_supplicant"));
    settings.setValue(QString("WPASUP_PATH"), config[QString("WPASUP_PATH")]);
    settings.setValue(QString("WPACLI_PATH"), config[QString("WPACLI_PATH")]);
    settings.setValue(QString("PID_FILE"), config[QString("PID_FILE")]);
    settings.setValue(QString("WPA_DRIVERS"), config[QString("WPA_DRIVERS")]);
    settings.setValue(QString("CTRL_DIR"), config[QString("CTRL_DIR")]);
    settings.setValue(QString("CTRL_GROUP"), config[QString("CTRL_GROUP")]);
    settings.endGroup();

    settings.beginGroup(QString("Other"));
    settings.setValue(QString("IFACE_DIR"), config[QString("IFACE_DIR")]);
    settings.setValue(QString("RFKILL_DIR"), config[QString("RFKILL_DIR")]);
    settings.setValue(QString("PREFERED_IFACE"), config[QString("PREFERED_IFACE")]);
    settings.endGroup();

    settings.sync();
}


void SettingsWindow::setTray()
{
    if (debug) qDebug() << PDEBUG;

    ui->checkBox_closeToTray->setDisabled(ui->checkBox_enableTray->checkState() == 0);
    ui->checkBox_startToTray->setDisabled(ui->checkBox_enableTray->checkState() == 0);
}


void SettingsWindow::restoreSettings()
{
    if (debug) qDebug() << PDEBUG;

    setSettings(getSettings());
}


void SettingsWindow::setDefault()
{
    if (debug) qDebug() << PDEBUG;

    setSettings(getDefault());
    if (sender() != ui->buttonBox->button(QDialogButtonBox::Reset))
        saveSettings();
}


void SettingsWindow::selectAbstractSomething()
{
    if (debug) qDebug() << PDEBUG;

    bool isDir = false;
    QString path = QString("/usr/bin");
    QString text = QApplication::translate("SettingsWindow", "Select helper command");
    QLineEdit *lineEdit = ui->lineEdit_helperPath;
    if (sender() == ui->pushButton_helperPath) {
        text = QApplication::translate("SettingsWindow", "Select helper command");
        lineEdit = ui->lineEdit_helperPath;
    } else if (sender() == ui->pushButton_interfacesDir) {
        isDir = true;
        text = QApplication::translate("SettingsWindow", "Select path to directory with interfaces");
        path = QString("/sys");
        lineEdit = ui->lineEdit_interfacesDir;
    } else if (sender() == ui->pushButton_netctlPath) {
        text = QApplication::translate("SettingsWindow", "Select netctl command");
        lineEdit = ui->lineEdit_netctlPath;
    } else if (sender() == ui->pushButton_netctlAutoPath) {
        text = QApplication::translate("SettingsWindow", "Select netctl-auto command");
        lineEdit = ui->lineEdit_netctlAutoPath;
    } else if (sender() == ui->pushButton_profilePath) {
        isDir = true;
        text = QApplication::translate("SettingsWindow", "Select path to profile directory");
        path = QString("/etc");
        lineEdit = ui->lineEdit_profilePath;
    } else if (sender() == ui->pushButton_rfkill) {
        isDir = true;
        text = QApplication::translate("SettingsWindow", "Select path to directory with rfkill devices");
        path = QString("/sys");
        lineEdit = ui->lineEdit_rfkill;
    } else if (sender() == ui->pushButton_sudo) {
        text = QApplication::translate("SettingsWindow", "Select sudo command");
        lineEdit = ui->lineEdit_sudo;
    } else if (sender() == ui->pushButton_systemctlPath) {
        text = QApplication::translate("SettingsWindow", "Select systemctl command");
        lineEdit = ui->lineEdit_systemctlPath;
    } else if (sender() == ui->pushButton_wpaCliPath) {
        text = QApplication::translate("SettingsWindow", "Select wpa_cli command");
        lineEdit = ui->lineEdit_wpaCliPath;
    } else if (sender() == ui->pushButton_wpaSupPath) {
        text = QApplication::translate("SettingsWindow", "Select wpa_supplicant command");
        lineEdit = ui->lineEdit_wpaSupPath;
    }

    QString filename;
    if (isDir)
        filename = QFileDialog::getExistingDirectory(this, text, path);
    else
        filename = QFileDialog::getOpenFileName(this, text, path,
                                                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        lineEdit->setText(filename);
}


void SettingsWindow::showWindow()
{
    if (debug) qDebug() << PDEBUG;

    setSettings(getSettings());
    setTray();
    updateHelper();

    show();
}


QMap<QString, QString> SettingsWindow::readSettings()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;

    if (ui->checkBox_helperClose->checkState() == 2)
        config[QString("CLOSE_HELPER")] = QString("true");
    else
        config[QString("CLOSE_HELPER")] = QString("false");
    if (ui->checkBox_closeToTray->checkState() == 2)
        config[QString("CLOSETOTRAY")] = QString("true");
    else
        config[QString("CLOSETOTRAY")] = QString("false");
    config[QString("CTRL_DIR")] = ui->lineEdit_wpaDir->text();
    config[QString("CTRL_GROUP")] = ui->lineEdit_wpaGroup->text();
    if (ui->checkBox_forceSudo->checkState() == 2)
        config[QString("FORCE_SUDO")] = QString("true");
    else
        config[QString("FORCE_SUDO")] = QString("false");
    config[QString("HELPER_PATH")] = ui->lineEdit_helperPath->text();
    config[QString("HELPER_SERVICE")] = ui->lineEdit_helperService->text();
    config[QString("IFACE_DIR")] = ui->lineEdit_interfacesDir->text();
    config[QString("LANGUAGE")] = ui->comboBox_language->currentText();
    config[QString("NETCTL_PATH")] = ui->lineEdit_netctlPath->text();
    config[QString("NETCTLAUTO_PATH")] = ui->lineEdit_netctlAutoPath->text();
    config[QString("NETCTLAUTO_SERVICE")] = ui->lineEdit_netctlAutoService->text();
    config[QString("PID_FILE")] = ui->lineEdit_pid->text();
    config[QString("PREFERED_IFACE")] = ui->lineEdit_interface->text();
    config[QString("PROFILE_DIR")] = ui->lineEdit_profilePath->text();
    config[QString("RFKILL_DIR")] = ui->lineEdit_rfkill->text();
    if (ui->checkBox_components->checkState() == 2)
        config[QString("SKIPCOMPONENTS")] = QString("true");
    else
        config[QString("SKIPCOMPONENTS")] = QString("false");
    if (ui->checkBox_startToTray->checkState() == 2)
        config[QString("STARTTOTRAY")] = QString("true");
    else
        config[QString("STARTTOTRAY")] = QString("false");
    config[QString("SUDO_PATH")] = ui->lineEdit_sudo->text();
    config[QString("SYSTEMCTL_PATH")] = ui->lineEdit_systemctlPath->text();
    if (ui->checkBox_enableTray->checkState() == 2)
        config[QString("SYSTRAY")] = QString("true");
    else
        config[QString("SYSTRAY")] = QString("false");
    if (ui->checkBox_useHelper->checkState() == 2)
        config[QString("USE_HELPER")] = QString("true");
    else
        config[QString("USE_HELPER")] = QString("false");
    config[QString("WPACLI_PATH")] = ui->lineEdit_wpaCliPath->text();
    config[QString("WPASUP_PATH")] = ui->lineEdit_wpaSupPath->text();
    config[QString("WPA_DRIVERS")] = ui->lineEdit_wpaSupDrivers->text();

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << QString("%1=%2").arg(config.keys()[i]).arg(config[config.keys()[i]]);

    return config;
}


void SettingsWindow::setSettings(const QMap<QString, QString> config)
{
    if (debug) qDebug() << PDEBUG;

    if (config[QString("CLOSE_HELPER")] == QString("true"))
        ui->checkBox_helperClose->setCheckState(Qt::Checked);
    else
        ui->checkBox_helperClose->setCheckState(Qt::Unchecked);
    if (config[QString("CLOSETOTRAY")] == QString("true"))
        ui->checkBox_closeToTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_closeToTray->setCheckState(Qt::Unchecked);
    ui->lineEdit_wpaDir->setText(config[QString("CTRL_DIR")]);
    ui->lineEdit_wpaGroup->setText(config[QString("CTRL_GROUP")]);
    if (config[QString("FORCE_SUDO")] == QString("true"))
        ui->checkBox_forceSudo->setCheckState(Qt::Checked);
    else
        ui->checkBox_forceSudo->setCheckState(Qt::Unchecked);
    ui->lineEdit_helperPath->setText(config[QString("HELPER_PATH")]);
    ui->lineEdit_helperService->setText(config[QString("HELPER_SERVICE")]);
    ui->lineEdit_interfacesDir->setText(config[QString("IFACE_DIR")]);
    int index = ui->comboBox_language->findText(config[QString("LANGUAGE")]);
    ui->comboBox_language->setCurrentIndex(index);
    ui->lineEdit_netctlPath->setText(config[QString("NETCTL_PATH")]);
    ui->lineEdit_netctlAutoPath->setText(config[QString("NETCTLAUTO_PATH")]);
    ui->lineEdit_netctlAutoService->setText(config[QString("NETCTLAUTO_SERVICE")]);
    ui->lineEdit_pid->setText(config[QString("PID_FILE")]);
    ui->lineEdit_interface->setText(config[QString("PREFERED_IFACE")]);
    ui->lineEdit_profilePath->setText(config[QString("PROFILE_DIR")]);
    ui->lineEdit_rfkill->setText(config[QString("RFKILL_DIR")]);
    if (config[QString("SKIPCOMPONENTS")] == QString("true"))
        ui->checkBox_components->setCheckState(Qt::Checked);
    else
        ui->checkBox_components->setCheckState(Qt::Unchecked);
    if (config[QString("STARTTOTRAY")] == QString("true"))
        ui->checkBox_startToTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_startToTray->setCheckState(Qt::Unchecked);
    ui->lineEdit_sudo->setText(config[QString("SUDO_PATH")]);
    ui->lineEdit_systemctlPath->setText(config[QString("SYSTEMCTL_PATH")]);
    if (config[QString("SYSTRAY")] == QString("true"))
        ui->checkBox_enableTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_enableTray->setCheckState(Qt::Unchecked);
    if (config[QString("USE_HELPER")] == QString("true"))
        ui->checkBox_useHelper->setCheckState(Qt::Checked);
    else
        ui->checkBox_useHelper->setCheckState(Qt::Unchecked);
    ui->lineEdit_wpaCliPath->setText(config[QString("WPACLI_PATH")]);
    ui->lineEdit_wpaSupPath->setText(config[QString("WPASUP_PATH")]);
    ui->lineEdit_wpaSupDrivers->setText(config[QString("WPA_DRIVERS")]);

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << QString("%1=%2").arg(config.keys()[i]).arg(config[config.keys()[i]]);
}


QMap<QString, QString> SettingsWindow::getDefault()
{
    if (debug) qDebug() << PDEBUG;

    return getSettings(QString("/dev/null"));
}


QMap<QString, QString> SettingsWindow::getSettings(QString fileName)
{
    if (debug) qDebug() << PDEBUG;
    if (fileName.isEmpty()) fileName = file;

    QMap<QString, QString> config;
    QSettings settings(fileName, QSettings::IniFormat);

    config[QString("LANGUAGE")] = Language::defineLanguage(fileName, QString());
    settings.beginGroup(QString("Common"));
    config[QString("LANGUAGE")] = settings.value(QString("LANGUAGE"), QString("en")).toString();
    config[QString("SYSTRAY")] = settings.value(QString("SYSTRAY"), QString("true")).toString();
    config[QString("CLOSETOTRAY")] = settings.value(QString("CLOSETOTRAY"), QString("true")).toString();
    config[QString("STARTTOTRAY")] = settings.value(QString("STARTTOTRAY"), QString("false")).toString();
    config[QString("SKIPCOMPONENTS")] = settings.value(QString("SKIPCOMPONENTS"), QString("false")).toString();
    settings.endGroup();

    settings.beginGroup(QString("Helper"));
    config[QString("USE_HELPER")] = settings.value(QString("USE_HELPER"), QString("true")).toString();
    config[QString("HELPER_GROUP")] = settings.value(QString("HELPER_GROUP"), QString("network")).toString();
    config[QString("FORCE_SUDO")] = settings.value(QString("FORCE_SUDO"), QString("false")).toString();
    config[QString("CLOSE_HELPER")] = settings.value(QString("CLOSE_HELPER"), QString("false")).toString();
    config[QString("HELPER_PATH")] = settings.value(QString("HELPER_PATH"), QString("/usr/bin/netctlgui-helper")).toString();
    config[QString("HELPER_SERVICE")] = settings.value(QString("HELPER_SERVICE"), QString("netctlgui-helper.service")).toString();
    settings.endGroup();

    settings.beginGroup(QString("netctl"));
    config[QString("SYSTEMCTL_PATH")] = settings.value(QString("SYSTEMCTL_PATH"), QString("/usr/bin/systemctl")).toString();
    config[QString("NETCTL_PATH")] = settings.value(QString("NETCTL_PATH"), QString("/usr/bin/netctl")).toString();
    config[QString("NETCTLAUTO_PATH")] = settings.value(QString("NETCTLAUTO_PATH"), QString("/usr/bin/netctl-auto")).toString();
    config[QString("NETCTLAUTO_SERVICE")] = settings.value(QString("NETCTLAUTO_SERVICE"), QString("netctl-auto")).toString();
    config[QString("PROFILE_DIR")] = settings.value(QString("PROFILE_DIR"), QString("/etc/netctl")).toString();
    settings.endGroup();

    settings.beginGroup(QString("sudo"));
    config[QString("SUDO_PATH")] = settings.value(QString("SUDO_PATH"), QString("/usr/bin/kdesu")).toString();
    settings.endGroup();

    settings.beginGroup(QString("wpa_supplicant"));
    config[QString("WPASUP_PATH")] = settings.value(QString("WPASUP_PATH"), QString("/usr/bin/wpa_supplicant")).toString();
    config[QString("WPACLI_PATH")] = settings.value(QString("WPACLI_PATH"), QString("/usr/bin/wpa_cli")).toString();
    config[QString("PID_FILE")] = settings.value(QString("PID_FILE"), QString("/run/wpa_supplicant_netctl-gui.pid")).toString();
    config[QString("WPA_DRIVERS")] = settings.value(QString("WPA_DRIVERS"), QString("nl80211,wext")).toString();
    config[QString("CTRL_DIR")] = settings.value(QString("CTRL_DIR"), QString("/run/wpa_supplicant_netctl-gui")).toString();
    config[QString("CTRL_GROUP")] = settings.value(QString("CTRL_GROUP"), QString("users")).toString();
    settings.endGroup();

    settings.beginGroup(QString("Other"));
    config[QString("IFACE_DIR")] = settings.value(QString("IFACE_DIR"), QString("/sys/class/net/")).toString();
    config[QString("RFKILL_DIR")] = settings.value(QString("RFKILL_DIR"), QString("/sys/class/rfkill/")).toString();
    config[QString("PREFERED_IFACE")] = settings.value(QString("PREFERED_IFACE"), QString("")).toString();
    settings.endGroup();

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << QString("%1=%2").arg(config.keys()[i]).arg(config[config.keys()[i]]);

    return config;
}


void SettingsWindow::startHelper()
{
    if (debug) qDebug() << PDEBUG;

    dynamic_cast<MainWindow *>(parent())->startHelper();
    updateHelper();
}


void SettingsWindow::updateHelper()
{
    if (debug) qDebug() << PDEBUG;

    if (dynamic_cast<MainWindow *>(parent())->isHelperServiceActive()) {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Active (systemd)"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Stop"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("process-stop"));
        ui->pushButton_status->setDisabled(true);
    } else if (dynamic_cast<MainWindow *>(parent())->isHelperActive()) {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Active"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Stop"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("process-stop"));
        ui->pushButton_status->setEnabled(true);
    } else {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Inactive"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Start"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("system-run"));
        ui->pushButton_status->setEnabled(true);
    }
}
