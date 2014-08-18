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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

#include "aboutwindow.h"
#include "bridgewidget.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "netctlautowindow.h"
#include "netctlguiadaptor.h"
#include "passwdwidget.h"
#include "pdebug.h"
#include "pppoewidget.h"
#include "settingswindow.h"
#include "taskadds.h"
#include "trayicon.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "version.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"


MainWindow::MainWindow(QWidget *parent,
                       const QMap<QString, QVariant> args,
                       QTranslator *qtAppTranslator,
                       QTranslator *appTranslator)
    : QMainWindow(parent),
      configPath(args[QString("config")].toString()),
      debug(args[QString("debug")].toBool()),
      qtTranslator(qtAppTranslator),
      translator(appTranslator)
{
    setWindowIcon(QIcon(":icon"));
    if (debug) qDebug() << PDEBUG << ":" << "about" << args[QString("about")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "auto" << args[QString("auto")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "config" << configPath;
    if (debug) qDebug() << PDEBUG << ":" << "debug" << debug;
    if (debug) qDebug() << PDEBUG << ":" << "defaults" << args[QString("defaults")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "essid" << args[QString("essid")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "minimized" << args[QString("minimized")].toInt();
    if (debug) qDebug() << PDEBUG << ":" << "open" << args[QString("open")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "options" << args[QString("options")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "select" << args[QString("select")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "settings" << args[QString("settings")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "tab" << args[QString("tab")].toInt();

    if (args[QString("minimized")].toInt() == 1)
        isDaemon = true;
    else
        isDaemon = false;
    updateConfiguration(args);

    // main actions
    if (args[QString("essid")].toString() != QString("ESSID")) {
        for (int i=0; i<ui->tableWidget_wifi->rowCount(); i++)
            if (ui->tableWidget_wifi->item(i, 0)->text() == args[QString("essid")].toString())
                ui->tableWidget_wifi->setCurrentCell(i, 0);
        if (ui->tableWidget_wifi->currentItem() == 0)
            errorWin->showWindow(18, QString(PDEBUG));
    } else if (args[QString("open")].toString() != QString("PROFILE")) {
        ui->comboBox_profile->addItem(args[QString("open")].toString());
        ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count()-1);
    } else if (args[QString("select")].toString() != QString("PROFILE")) {
        for (int i=0; i<ui->tableWidget_main->rowCount(); i++)
            if (ui->tableWidget_main->item(i, 0)->text() == args[QString("select")].toString())
                ui->tableWidget_main->setCurrentCell(i, 0);
        if (ui->tableWidget_main->currentItem() == 0)
            errorWin->showWindow(17, QString(PDEBUG));
    }

    // show windows
    if (args[QString("about")].toBool())
        showAboutWindow();
    if (args[QString("auto")].toBool())
        showNetctlAutoWindow();
    if (args[QString("settings")].toBool())
        showSettingsWindow();

    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Ready"));
}


MainWindow::~MainWindow()
{
    if (debug) qDebug() << PDEBUG;

    if ((useHelper) && (configuration[QString("CLOSE_HELPER")] == QString("true")))
        forceStopHelper();
    deleteObjects();
}


QStringList MainWindow::printInformation()
{
    if (debug) qDebug() << PDEBUG;

    QString profile;
    QString status;
    if (useHelper) {
        QStringList request = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                              DBUS_HELPER_INTERFACE, QString("Information"),
                                              QList<QVariant>(), true, debug)[0].toStringList();
        profile = request[0];
        status = request[1];
    } else {
        if (netctlCommand->isNetctlAutoRunning()) {
            profile = netctlCommand->autoGetActiveProfile();
            status = QString("netctl-auto");
        } else {
            QStringList currentProfiles = netctlCommand->getActiveProfile();
            profile = currentProfiles.join(QChar('|'));
            QStringList statusList;
            for (int i=0; i<currentProfiles.count(); i++)
                statusList.append(netctlCommand->getProfileStatus(currentProfiles[i]));
            status = statusList.join(QChar('|'));
        }
    }
    QStringList output;
    output.append(profile);
    output.append(status);

    return output;
}


QStringList MainWindow::printSettings()
{
    if (debug) qDebug() << PDEBUG;

    QStringList settingsList;
    for (int i=0; i<configuration.keys().count(); i++)
        settingsList.append(configuration.keys()[i] + QString("==") +
                            configuration[configuration.keys()[i]]);

    return settingsList;
}


QStringList MainWindow::printTrayInformation()
{
    if (debug) qDebug() << PDEBUG;

    QStringList information;
    QString current;
    QString enabled;
    bool netctlAutoStatus = false;
    QList<netctlProfileInfo> profiles;
    if (useHelper) {
        current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                  DBUS_HELPER_INTERFACE, QString("ActiveProfile"),
                                  QList<QVariant>(), true, debug)[0].toString();
        netctlAutoStatus = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                           DBUS_HELPER_INTERFACE, QString("isNetctlAutoActive"),
                                           QList<QVariant>(), true, debug)[0].toBool();
        profiles = parseOutputNetctl(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                     DBUS_HELPER_INTERFACE, QString("ProfileList"),
                                                     QList<QVariant>(), true, debug), debug);
        if (netctlAutoStatus) {
            QList<QVariant> args;
            args.append(current);
            enabled = QString::number(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                      DBUS_HELPER_INTERFACE, QString("autoIsProfileEnabled"),
                                                      args, true, debug)[0].toBool());
        } else {
            QStringList currentProfiles = current.split(QChar('|'));
            QStringList enabledList;
            for (int i=0; i<currentProfiles.count(); i++) {
                QList<QVariant> args;
                args.append(currentProfiles[i]);
                enabledList.append(QString::number(
                                       sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                       DBUS_HELPER_INTERFACE, QString("isProfileEnabled"),
                                                       args, true, debug)[0].toBool()));
                enabled = enabledList.join(QChar('|'));
            }
        }
    } else {
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();
        if (netctlAutoStatus) {
            current = netctlCommand->autoGetActiveProfile();
            enabled = QString::number(netctlCommand->autoIsProfileEnabled(current));
            profiles = netctlCommand->getProfileListFromNetctlAuto();
        } else {
            QStringList currentProfiles = netctlCommand->getActiveProfile();
            current = currentProfiles.join(QChar('|'));
            QStringList enabledList;
            for (int i=0; i<currentProfiles.count(); i++)
                enabledList.append(QString::number(netctlCommand->isProfileEnabled(currentProfiles[i])));
            enabled = enabledList.join(QChar('|'));
            profiles = netctlCommand->getProfileList();
        }
    }

    information.append(QString::number(netctlAutoStatus));
    QStringList profileList;
    for (int i=0; i<profiles.count(); i++)
        profileList.append(profiles[i].name);
    information.append(profileList.join(QChar('|')));
    information.append(current);
    information.append(enabled);

    return information;
}


bool MainWindow::isHelperActive()
{
    if (debug) qDebug() << PDEBUG;

    QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                               DBUS_HELPER_INTERFACE, QString("Active"),
                                               QList<QVariant>(), true, debug);

    return (!responce.isEmpty());
}


bool MainWindow::isHelperServiceActive()
{
    if (debug) qDebug() << PDEBUG;

    QString cmd = configuration[QString("SYSTEMCTL_PATH")] + QString(" is-active ") +
            configuration[QString("HELPER_SERVICE")];
    if (debug) qDebug() << PDEBUG << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    if (process.exitCode != 0)
        return false;
    else
        return true;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (debug) qDebug() << PDEBUG;

    if ((QSystemTrayIcon::isSystemTrayAvailable()) &&
            (configuration[QString("SYSTRAY")] == QString("true"))) {
        hide();
        event->ignore();
    } else
        closeMainWindow();
}


bool MainWindow::checkExternalApps(const QString apps = QString("all"))
{
    if (debug) qDebug() << PDEBUG;

    QStringList cmd;
    cmd.append("which");
    cmd.append(configuration[QString("SUDO_PATH")]);
    if ((apps == QString("helper")) || (apps == QString("all"))) {
        cmd.append(configuration[QString("HELPER_PATH")]);
    }
    if ((apps == QString("netctl")) || (apps == QString("all"))) {
        cmd.append(configuration[QString("NETCTL_PATH")]);
        cmd.append(configuration[QString("NETCTLAUTO_PATH")]);
    }
    if ((apps == QString("systemctl")) || (apps == QString("all"))) {
        cmd.append(configuration[QString("SYSTEMCTL_PATH")]);
    }
    if ((apps == QString("wpasup")) || (apps == QString("all"))) {
        cmd.append(configuration[QString("WPACLI_PATH")]);
        cmd.append(configuration[QString("WPASUP_PATH")]);
    }

    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd.join(QChar(' '));
    TaskResult process = runTask(cmd.join(QChar(' ')), false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    if (process.exitCode != 0)
        return false;
    else
        return true;
}


bool MainWindow::checkHelperStatus()
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper) useHelper = isHelperActive();
    if (useHelper)
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Update"),
                        QList<QVariant>(), true, debug);
    else
        configuration[QString("FORCE_SUDO")] = QString("true");
    if (isHelperServiceActive())
        configuration[QString("CLOSE_HELPER")] = QString("false");

    return useHelper;
}


QString MainWindow::checkStatus(const bool statusBool, const bool nullFalse)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Status" << statusBool;
    if (debug) qDebug() << PDEBUG << ":" << "Return null false" << nullFalse;

    if (statusBool)
        return QApplication::translate("MainWindow", "yes");
    if (nullFalse)
        return QString("");
    else
        return QApplication::translate("MainWindow", "no");
}


// window signals
void MainWindow::createActions()
{
    if (debug) qDebug() << PDEBUG;

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabs(int)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAboutWindow()));
    connect(ui->actionApi, SIGNAL(triggered(bool)), this, SLOT(showApi()));
    connect(ui->actionLibrary, SIGNAL(triggered(bool)), this, SLOT(showLibrary()));
    connect(ui->actionNetctlAuto, SIGNAL(triggered(bool)), this, SLOT(showNetctlAutoWindow()));
    connect(ui->actionReport, SIGNAL(triggered(bool)), this, SLOT(reportABug()));
    connect(ui->actionSecurity, SIGNAL(triggered(bool)), this, SLOT(showSecurityNotes()));
    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(showSettingsWindow()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(closeMainWindow()));

    // actions menu
    connect(ui->menuActions, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));
    connect(ui->actionMainEdit, SIGNAL(triggered(bool)), this, SLOT(mainTabEditProfile()));
    connect(ui->actionMainEnable, SIGNAL(triggered(bool)), this, SLOT(mainTabEnableProfile()));
    connect(ui->actionMainRefresh, SIGNAL(triggered(bool)), this, SLOT(updateMainTab()));
    connect(ui->actionMainRemove, SIGNAL(triggered(bool)), this, SLOT(mainTabRemoveProfile()));
    connect(ui->actionMainRestart, SIGNAL(triggered(bool)), this, SLOT(mainTabRestartProfile()));
    connect(ui->actionMainStart, SIGNAL(triggered(bool)), this, SLOT(mainTabStartProfile()));
    connect(ui->actionMainStopAll, SIGNAL(triggered(bool)), this, SLOT(mainTabStopAllProfiles()));
    connect(ui->actionMainSwitch, SIGNAL(triggered(bool)), this, SLOT(mainTabSwitchToProfile()));
    connect(ui->actionProfileClear, SIGNAL(triggered(bool)), this, SLOT(profileTabClear()));
    connect(ui->actionProfileLoad, SIGNAL(triggered(bool)), this, SLOT(profileTabLoadProfile()));
    connect(ui->actionProfileRemove, SIGNAL(triggered(bool)), this, SLOT(profileTabRemoveProfile()));
    connect(ui->actionProfileSave, SIGNAL(triggered(bool)), this, SLOT(profileTabCreateProfile()));
    connect(ui->actionWifiRefresh, SIGNAL(triggered(bool)), this, SLOT(updateWifiTab()));
    connect(ui->actionWifiStart, SIGNAL(triggered(bool)), this, SLOT(wifiTabStart()));

    // main tab events
    connect(ui->pushButton_mainRefresh, SIGNAL(clicked(bool)), this, SLOT(updateMainTab()));
    connect(ui->pushButton_mainRestart, SIGNAL(clicked(bool)), this, SLOT(mainTabRestartProfile()));
    connect(ui->pushButton_mainStart, SIGNAL(clicked(bool)), this, SLOT(mainTabStartProfile()));
    connect(ui->pushButton_netctlAuto, SIGNAL(clicked(bool)), this, SLOT(showNetctlAutoWindow()));
    connect(ui->tableWidget_main, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(mainTabStartProfile()));
    connect(ui->tableWidget_main, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(mainTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->tableWidget_main, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(mainTabContextualMenu(QPoint)));

    // profile tab events
    connect(ui->comboBox_profile, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileTabLoadProfile()));
    connect(ui->pushButton_profileClear, SIGNAL(clicked(bool)), this, SLOT(profileTabClear()));
    connect(ui->pushButton_profileSave, SIGNAL(clicked(bool)), this, SLOT(profileTabCreateProfile()));
    connect(generalWid->connectionType, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileTabChangeState(QString)));

    // wifi tab events
    connect(ui->pushButton_wifiRefresh, SIGNAL(clicked(bool)), this, SLOT(updateWifiTab()));
    connect(ui->pushButton_wifiStart, SIGNAL(clicked(bool)), this, SLOT(wifiTabStart()));
    connect(ui->tableWidget_wifi, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(wifiTabStart()));
    connect(ui->tableWidget_wifi, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(wifiTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->tableWidget_wifi, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(wifiTabContextualMenu(QPoint)));
}


void MainWindow::createDBusSession()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(DBUS_SERVICE)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register service";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
    }
    if (!bus.registerObject(DBUS_OBJECT_PATH,
                            new NetctlGuiAdaptor(this),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register GUI object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
    }
}


void MainWindow::createObjects()
{
    if (debug) qDebug() << PDEBUG;

    // error messages
    errorWin = new ErrorWindow(this, debug);
    // backend
    createDBusSession();
    if (useHelper)
        if (!forceStartHelper()) {
            errorWin->showWindow(19, QString(PDEBUG));
            useHelper = false;
        }
    checkHelperStatus();

    netctlCommand = new Netctl(debug, configuration);
    netctlProfile = new NetctlProfile(debug, configuration);
    wpaCommand = new WpaSup(debug, configuration);
    // frontend
    trayIcon = new TrayIcon(this, debug);
    // windows
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    ui->tableWidget_main->setColumnHidden(2, true);
    ui->tableWidget_main->setColumnHidden(3, true);
    ui->tableWidget_wifi->setColumnHidden(3, true);
    ui->tableWidget_wifi->setColumnHidden(4, true);
    aboutWin = new AboutWindow(this, debug);
    netctlAutoWin = new NetctlAutoWindow(this, debug, configuration);
    settingsWin = new SettingsWindow(this, debug, configPath);
    // profile widgets
    generalWid = new GeneralWidget(this, configuration);
    ui->scrollAreaWidgetContents->layout()->addWidget(generalWid);
    ipWid = new IpWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ipWid);
    bridgeWid = new BridgeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(bridgeWid);
    ethernetWid = new EthernetWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ethernetWid);
    macvlanWid = new MacvlanWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(macvlanWid);
    mobileWid = new MobileWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(mobileWid);
    pppoeWid = new PppoeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(pppoeWid);
    tunnelWid = new TunnelWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(tunnelWid);
    tuntapWid = new TuntapWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(tuntapWid);
    vlanWid = new VlanWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(vlanWid);
    wirelessWid = new WirelessWidget(this, configuration);
    ui->scrollAreaWidgetContents->layout()->addWidget(wirelessWid);
}


void MainWindow::deleteObjects()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection::sessionBus().unregisterObject(DBUS_OBJECT_PATH);
    QDBusConnection::sessionBus().unregisterService(DBUS_SERVICE);
    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
    if (wpaCommand != nullptr) delete wpaCommand;

    if (bridgeWid != nullptr) delete bridgeWid;
    if (ethernetWid != nullptr) delete ethernetWid;
    if (generalWid != nullptr) delete generalWid;
    if (ipWid != nullptr) delete ipWid;
    if (macvlanWid != nullptr) delete macvlanWid;
    if (mobileWid != nullptr) delete mobileWid;
    if (pppoeWid != nullptr) delete pppoeWid;
    if (tunnelWid != nullptr) delete tunnelWid;
    if (tuntapWid != nullptr) delete tuntapWid;
    if (vlanWid != nullptr) delete vlanWid;
    if (wirelessWid != nullptr) delete wirelessWid;

    if (aboutWin != nullptr) delete aboutWin;
    if (errorWin != nullptr) delete errorWin;
    if (netctlAutoWin != nullptr) delete netctlAutoWin;
    if (settingsWin != nullptr) delete settingsWin;
    if (trayIcon != nullptr) delete trayIcon;
    if (ui != nullptr) delete ui;
}


void MainWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << PDEBUG;

    if (pressedKey->key() == Qt::Key_Return)
        if (ui->comboBox_profile->hasFocus())
            profileTabLoadProfile();
}


QMap<QString, QString> MainWindow::parseOptions(const QString options)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    for (int i=0; i<options.split(QChar(',')).count(); i++) {
        if (options.split(QChar(','))[i].split(QChar('=')).count() < 2) continue;
        settings[options.split(QChar(','))[i].split(QChar('='))[0]] =
                options.split(QChar(','))[i].split(QChar('='))[1];
    }
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}
