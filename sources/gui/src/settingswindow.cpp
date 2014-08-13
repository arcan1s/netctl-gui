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

#include "language.h"
#include "mainwindow.h"
#include "pdebug.h"


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
    Q_UNUSED(previous)
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
    ((MainWindow *)parent())->updateConfiguration();
}


void SettingsWindow::saveSettings()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings = readSettings();
    QFile configFile(file);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&configFile);
    for (int i=0; i<settings.keys().count(); i++)
        out << settings.keys()[i] << QString("=") << settings[settings.keys()[i]] << endl;
    configFile.close();
}


void SettingsWindow::setTray()
{
    if (debug) qDebug() << PDEBUG;

    if (ui->checkBox_enableTray->checkState() == 0) {
        ui->checkBox_closeToTray->setDisabled(true);
        ui->checkBox_startToTray->setDisabled(true);
    } else if (ui->checkBox_enableTray->checkState() == 2) {
        ui->checkBox_closeToTray->setEnabled(true);
        ui->checkBox_startToTray->setEnabled(true);
    }
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

    QMap<QString, QString> settings;
    if (ui->checkBox_helperClose->checkState() == 2)
        settings[QString("CLOSE_HELPER")] = QString("true");
    else
        settings[QString("CLOSE_HELPER")] = QString("false");
    if (ui->checkBox_closeToTray->checkState() == 2)
        settings[QString("CLOSETOTRAY")] = QString("true");
    else
        settings[QString("CLOSETOTRAY")] = QString("false");
    settings[QString("CTRL_DIR")] = ui->lineEdit_wpaDir->text();
    settings[QString("CTRL_GROUP")] = ui->lineEdit_wpaGroup->text();
    if (ui->checkBox_forceSudo->checkState() == 2)
        settings[QString("FORCE_SUDO")] = QString("true");
    else
        settings[QString("FORCE_SUDO")] = QString("false");
    settings[QString("HELPER_PATH")] = ui->lineEdit_helperPath->text();
    settings[QString("HELPER_SERVICE")] = ui->lineEdit_helperService->text();
    settings[QString("IFACE_DIR")] = ui->lineEdit_interfacesDir->text();
    settings[QString("LANGUAGE")] = ui->comboBox_language->currentText();
    settings[QString("NETCTL_PATH")] = ui->lineEdit_netctlPath->text();
    settings[QString("NETCTLAUTO_PATH")] = ui->lineEdit_netctlAutoPath->text();
    settings[QString("NETCTLAUTO_SERVICE")] = ui->lineEdit_netctlAutoService->text();
    settings[QString("PID_FILE")] = ui->lineEdit_pid->text();
    settings[QString("PREFERED_IFACE")] = ui->lineEdit_interface->text();
    settings[QString("PROFILE_DIR")] = ui->lineEdit_profilePath->text();
    settings[QString("RFKILL_DIR")] = ui->lineEdit_rfkill->text();
    if (ui->checkBox_startToTray->checkState() == 2)
        settings[QString("STARTTOTRAY")] = QString("true");
    else
        settings[QString("STARTTOTRAY")] = QString("false");
    settings[QString("SUDO_PATH")] = ui->lineEdit_sudo->text();
    settings[QString("SYSTEMCTL_PATH")] = ui->lineEdit_systemctlPath->text();
    if (ui->checkBox_enableTray->checkState() == 2)
        settings[QString("SYSTRAY")] = QString("true");
    else
        settings[QString("SYSTRAY")] = QString("false");
    if (ui->checkBox_useHelper->checkState() == 2)
        settings[QString("USE_HELPER")] = QString("true");
    else
        settings[QString("USE_HELPER")] = QString("false");
    settings[QString("WPACLI_PATH")] = ui->lineEdit_wpaCliPath->text();
    settings[QString("WPASUP_PATH")] = ui->lineEdit_wpaSupPath->text();
    settings[QString("WPA_DRIVERS")] = ui->lineEdit_wpaSupDrivers->text();
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void SettingsWindow::setSettings(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;

    if (settings[QString("CLOSE_HELPER")] == QString("true"))
        ui->checkBox_helperClose->setCheckState(Qt::Checked);
    else
        ui->checkBox_helperClose->setCheckState(Qt::Unchecked);
    if (settings[QString("CLOSETOTRAY")] == QString("true"))
        ui->checkBox_closeToTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_closeToTray->setCheckState(Qt::Unchecked);
    ui->lineEdit_wpaDir->setText(settings[QString("CTRL_DIR")]);
    ui->lineEdit_wpaGroup->setText(settings[QString("CTRL_GROUP")]);
    if (settings[QString("FORCE_SUDO")] == QString("true"))
        ui->checkBox_forceSudo->setCheckState(Qt::Checked);
    else
        ui->checkBox_forceSudo->setCheckState(Qt::Unchecked);
    ui->lineEdit_helperPath->setText(settings[QString("HELPER_PATH")]);
    ui->lineEdit_helperService->setText(settings[QString("HELPER_SERVICE")]);
    ui->lineEdit_interfacesDir->setText(settings[QString("IFACE_DIR")]);
    ui->comboBox_language->setCurrentIndex(0);
    for (int i=0; i<ui->comboBox_language->count(); i++)
        if (ui->comboBox_language->itemText(i) == settings[QString("LANGUAGE")])
            ui->comboBox_language->setCurrentIndex(i);
    ui->lineEdit_netctlPath->setText(settings[QString("NETCTL_PATH")]);
    ui->lineEdit_netctlAutoPath->setText(settings[QString("NETCTLAUTO_PATH")]);
    ui->lineEdit_netctlAutoService->setText(settings[QString("NETCTLAUTO_SERVICE")]);
    ui->lineEdit_pid->setText(settings[QString("PID_FILE")]);
    ui->lineEdit_interface->setText(settings[QString("PREFERED_IFACE")]);
    ui->lineEdit_profilePath->setText(settings[QString("PROFILE_DIR")]);
    ui->lineEdit_rfkill->setText(settings[QString("RFKILL_DIR")]);
    if (settings[QString("STARTTOTRAY")] == QString("true"))
        ui->checkBox_startToTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_startToTray->setCheckState(Qt::Unchecked);
    ui->lineEdit_sudo->setText(settings[QString("SUDO_PATH")]);
    ui->lineEdit_systemctlPath->setText(settings[QString("SYSTEMCTL_PATH")]);
    if (settings[QString("SYSTRAY")] == QString("true"))
        ui->checkBox_enableTray->setCheckState(Qt::Checked);
    else
        ui->checkBox_enableTray->setCheckState(Qt::Unchecked);
    if (settings[QString("USE_HELPER")] == QString("true"))
        ui->checkBox_useHelper->setCheckState(Qt::Checked);
    else
        ui->checkBox_useHelper->setCheckState(Qt::Unchecked);
    ui->lineEdit_wpaCliPath->setText(settings[QString("WPACLI_PATH")]);
    ui->lineEdit_wpaSupPath->setText(settings[QString("WPASUP_PATH")]);
    ui->lineEdit_wpaSupDrivers->setText(settings[QString("WPA_DRIVERS")]);
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];
}


QMap<QString, QString> SettingsWindow::getDefault()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    settings[QString("CLOSE_HELPER")] = QString("false");
    settings[QString("CLOSETOTRAY")] = QString("true");
    settings[QString("CTRL_DIR")] = QString("/run/wpa_supplicant_netctl-gui");
    settings[QString("CTRL_GROUP")] = QString("users");
    settings[QString("FORCE_SUDO")] = QString("false");
    settings[QString("HELPER_PATH")] = QString("/usr/bin/netctlgui-helper");
    settings[QString("HELPER_SERVICE")] = QString("netctlgui-helper.service");
    settings[QString("IFACE_DIR")] = QString("/sys/class/net/");
    settings[QString("LANGUAGE")] = QString("en");
    settings[QString("NETCTL_PATH")] = QString("/usr/bin/netctl");
    settings[QString("NETCTLAUTO_PATH")] = QString("/usr/bin/netctl-auto");
    settings[QString("NETCTLAUTO_SERVICE")] = QString("netctl-auto");
    settings[QString("PID_FILE")] = QString("/run/wpa_supplicant_netctl-gui.pid");
    settings[QString("PREFERED_IFACE")] = QString("");
    settings[QString("PROFILE_DIR")] = QString("/etc/netctl/");
    settings[QString("RFKILL_DIR")] = QString("/sys/class/rfkill/");
    settings[QString("STARTTOTRAY")] = QString("false");
    settings[QString("SUDO_PATH")] = QString("/usr/bin/kdesu");
    settings[QString("SYSTEMCTL_PATH")] = QString("/usr/bin/systemctl");
    settings[QString("SYSTRAY")] = QString("true");
    settings[QString("USE_HELPER")] = QString("true");
    settings[QString("WPACLI_PATH")] = QString("/usr/bin/wpa_cli");
    settings[QString("WPASUP_PATH")] = QString("/usr/bin/wpa_supplicant");
    settings[QString("WPA_DRIVERS")] = QString("nl80211,wext");
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


QMap<QString, QString> SettingsWindow::getSettings()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings = getDefault();
    QFile configFile(file);
    QString fileStr;
    if (!configFile.open(QIODevice::ReadOnly))
        return settings;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('=')))
            settings[fileStr.split(QChar('='))[0]] = fileStr.split(QChar('='))[1];
        if (configFile.atEnd()) break;
    }
    configFile.close();
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void SettingsWindow::startHelper()
{
    if (debug) qDebug() << PDEBUG;

    ((MainWindow *)parent())->startHelper();
    updateHelper();
}


void SettingsWindow::updateHelper()
{
    if (debug) qDebug() << PDEBUG;

    if (((MainWindow *)parent())->isHelperServiceActive()) {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Active (systemd)"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Stop"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("process-stop"));
        ui->pushButton_status->setDisabled(true);
    } else if (((MainWindow *)parent())->isHelperActive()) {
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
