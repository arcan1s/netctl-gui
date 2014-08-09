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
    if (debug) qDebug() << "[SettingsWindow]" << "[~SettingsWindow]";

    delete ui;
}


void SettingsWindow::createActions()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[createActions]";

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(setDefault()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(closeWindow()));
    connect(ui->checkBox_enableTray, SIGNAL(stateChanged(int)), this, SLOT(setTray()));
    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
            this, SLOT(changePage(QTreeWidgetItem *, QTreeWidgetItem *)));
    // buttons
    connect(ui->pushButton_helperPath, SIGNAL(clicked(bool)), this, SLOT(selectHelperPath()));
    connect(ui->pushButton_interfaceDir, SIGNAL(clicked(bool)), this, SLOT(selectIfaceDir()));
    connect(ui->pushButton_netctlPath, SIGNAL(clicked(bool)), this, SLOT(selectNetctlPath()));
    connect(ui->pushButton_netctlAutoPath, SIGNAL(clicked(bool)), this, SLOT(selectNetctlAutoPath()));
    connect(ui->pushButton_profilePath, SIGNAL(clicked(bool)), this, SLOT(selectProfileDir()));
    connect(ui->pushButton_rfkill, SIGNAL(clicked(bool)), this, SLOT(selectRfkillDir()));
    connect(ui->pushButton_status, SIGNAL(clicked(bool)), this, SLOT(startHelper()));
    connect(ui->pushButton_sudo, SIGNAL(clicked(bool)), this, SLOT(selectSudoPath()));
    connect(ui->pushButton_systemctlPath, SIGNAL(clicked(bool)), this, SLOT(selectSystemctlPath()));
    connect(ui->pushButton_wpaCliPath, SIGNAL(clicked(bool)), this, SLOT(selectWpaCliPath()));
    connect(ui->pushButton_wpaSupPath, SIGNAL(clicked(bool)), this, SLOT(selectWpaSupPath()));
}


// ESC press event
void SettingsWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[SettingsWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Escape)
        close();
}


void SettingsWindow::addLanguages()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[addLanguages]";

    ui->comboBox_language->clear();
    ui->comboBox_language->addItems(Language::getAvailableLanguages());
}


void SettingsWindow::changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (debug) qDebug() << "[SettingsWindow]" << "[changePage]";

    for (int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
        if (current == ui->treeWidget->topLevelItem(i)) {
            ui->stackedWidget->setCurrentIndex(i);
            break;
        }
}


void SettingsWindow::closeWindow()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[closeWindow]";

    saveSettings();
    close();
    ((MainWindow *)parent())->updateConfiguration();
}


void SettingsWindow::saveSettings()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[saveSettings]";

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
    if (debug) qDebug() << "[SettingsWindow]" << "[setTray]";

    if (ui->checkBox_enableTray->checkState() == 0) {
        ui->checkBox_closeToTray->setDisabled(true);
        ui->checkBox_startToTray->setDisabled(true);
    }
    else if (ui->checkBox_enableTray->checkState() == 2) {
        ui->checkBox_closeToTray->setEnabled(true);
        ui->checkBox_startToTray->setEnabled(true);
    }
}


void SettingsWindow::setDefault()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[setDefault]";

    setSettings(getDefault());
    saveSettings();
}


void SettingsWindow::selectHelperPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectHelperPath]";

    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select helper command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_helperPath->setText(filename);
}


void SettingsWindow::selectIfaceDir()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectIfaceDir]";

    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to directory with interfaces"),
                QString("/sys/"));
    if (!directory.isEmpty())
        ui->lineEdit_interfacesDir->setText(directory);
}


void SettingsWindow::selectNetctlPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectNetctlPath]";

    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select netctl command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_netctlPath->setText(filename);
}


void SettingsWindow::selectNetctlAutoPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectNetctlAutoPath]";

    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select netctl-auto command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_netctlAutoPath->setText(filename);
}


void SettingsWindow::selectProfileDir()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectProfileDir]";

    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to profile directory"),
                QString("/etc/"));
    if (!directory.isEmpty())
        ui->lineEdit_profilePath->setText(directory);
}


void SettingsWindow::selectRfkillDir()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectRfkillDir]";

    QString directory = QFileDialog::getExistingDirectory(
                this,
                QApplication::translate("SettingsWindow", "Select path to directory with rfkill devices"),
                QString("/sys/"));
    if (!directory.isEmpty())
        ui->lineEdit_rfkill->setText(directory);
}


void SettingsWindow::selectSudoPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectSudoPath]";

    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select sudo command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_sudo->setText(filename);
}


void SettingsWindow::selectSystemctlPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectSystemctlPath]";

    QString filename = QFileDialog::getOpenFileName(
                this,
                QApplication::translate("SettingsWindow", "Select systemctl command"),
                QString("/usr/bin/"),
                QApplication::translate("SettingsWindow", "All files (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_systemctlPath->setText(filename);
}


void SettingsWindow::selectWpaCliPath()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[selectWpaCliPath]";

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
    if (debug) qDebug() << "[SettingsWindow]" << "[selectWpaSupPath]";

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
    if (debug) qDebug() << "[SettingsWindow]" << "[showWindow]";

    setSettings(getSettings());
    setTray();
    updateHelper();

    show();
}


QMap<QString, QString> SettingsWindow::readSettings()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[readSettings]";

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
        if (debug) qDebug() << "[SettingsWindow]" << "[readSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void SettingsWindow::setSettings(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << "[SettingsWindow]" << "[setSettings]";

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
    ui->lineEdit_helperPath->setText(settings[QString("HELPER_SERVICE")]);
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
        if (debug) qDebug() << "[SettingsWindow]" << "[setSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];
}


QMap<QString, QString> SettingsWindow::getDefault()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[getDefault]";

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
        if (debug) qDebug() << "[SettingsWindow]" << "[getDefault]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


QMap<QString, QString> SettingsWindow::getSettings()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[getSettings]";

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
        if (debug) qDebug() << "[SettingsWindow]" << "[getSettings]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void SettingsWindow::startHelper()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[startHelper]";

    ((MainWindow *)parent())->startHelper();
    updateHelper();
}


void SettingsWindow::updateHelper()
{
    if (debug) qDebug() << "[SettingsWindow]" << "[updateHelper]";

    if (((MainWindow *)parent())->isHelperServiceActive()) {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Active (systemd)"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Stop"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("process-stop"));
        ui->pushButton_status->setDisabled(true);
    }
    else if (((MainWindow *)parent())->isHelperActive()) {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Active"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Stop"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("process-stop"));
        ui->pushButton_status->setEnabled(true);
    }
    else {
        ui->label_status->setText(QApplication::translate("SettingsWindow", "Inactive"));
        ui->pushButton_status->setText(QApplication::translate("SettingsWindow", "Start"));
        ui->pushButton_status->setIcon(QIcon::fromTheme("system-run"));
        ui->pushButton_status->setEnabled(true);
    }
}
