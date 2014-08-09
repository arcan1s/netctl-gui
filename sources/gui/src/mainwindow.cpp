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
#include <QTranslator>

#include "aboutwindow.h"
#include "bridgewidget.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "language.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "netctlautowindow.h"
#include "netctlguiadaptor.h"
#include "passwdwidget.h"
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
                       QTranslator *appTranslator)
    : QMainWindow(parent),
      configPath(args[QString("config")].toString()),
      debug(args[QString("debug")].toBool()),
      translator(appTranslator)
{
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "about" << args[QString("about")].toBool();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "auto" << args[QString("auto")].toBool();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "config" << configPath;
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "debug" << debug;
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "defaults" << args[QString("defaults")].toBool();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "essid" << args[QString("essid")].toString();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "minimized" << args[QString("minimized")].toInt();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "open" << args[QString("open")].toString();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "options" << args[QString("options")].toString();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "select" << args[QString("select")].toString();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "settings" << args[QString("settings")].toBool();
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "tab" << args[QString("tab")].toInt();

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
            errorWin->showWindow(18, QString("[MainWindow] : [MainWindow]"));
    }
    else if (args[QString("open")].toString() != QString("PROFILE")) {
        ui->comboBox_profile->addItem(args[QString("open")].toString());
        ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count()-1);
    }
    else if (args[QString("select")].toString() != QString("PROFILE")) {
        for (int i=0; i<ui->tableWidget_main->rowCount(); i++)
            if (ui->tableWidget_main->item(i, 0)->text() == args[QString("select")].toString())
                ui->tableWidget_main->setCurrentCell(i, 0);
        if (ui->tableWidget_main->currentItem() == 0)
            errorWin->showWindow(17, QString("[MainWindow] : [MainWindow]"));
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
    if (debug) qDebug() << "[MainWindow]" << "[~MainWindow]";

    if ((useHelper) && (configuration[QString("CLOSE_HELPER")] == QString("true")))
        forceStopHelper();
    deleteObjects();
}


QString MainWindow::getInformation()
{
    if (debug) qDebug() << "[MainWindow]" << "[getInformation]";

    QString profile;
    QString status;
    if (useHelper) {
        QStringList request = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                              DBUS_HELPER_INTERFACE, QString("Information"),
                                              QList<QVariant>(), true, debug)[0].toStringList();
        profile = request[0];
        status = request[1];
    }
    else {
        if (netctlCommand->isNetctlAutoRunning()) {
            profile = netctlCommand->autoGetActiveProfile();
            status = QString("netctl-auto");
        }
        else {
            profile = netctlCommand->getActiveProfile();
            status = netctlCommand->getProfileStatus(profile);
        }
    }
    QString output = QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Profile")).arg(profile);
    output += QString("%1: %2").arg(QApplication::translate("MainWindow", "Status")).arg(status);

    return output;
}


QStringList MainWindow::getSettings()
{
    if (debug) qDebug() << "[MainWindow]" << "[getSettings]";

    QStringList settingsList;
    for (int i=0; i<configuration.keys().count(); i++)
        settingsList.append(configuration.keys()[i] + QString("=") +
                            configuration[configuration.keys()[i]]);

    return settingsList;
}


bool MainWindow::isHelperActive()
{
    if (debug) qDebug() << "[MainWindow]" << "[isHelperActive]";

    QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                               DBUS_HELPER_INTERFACE, QString("Active"),
                                               QList<QVariant>(), true, debug);

    return (!responce.isEmpty());
}


bool MainWindow::checkExternalApps(const QString apps = QString("all"))
{
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]";

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

    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Run cmd" << cmd.join(QChar(' '));
    TaskResult process = runTask(cmd.join(QChar(' ')), false);
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode != 0)
        return false;
    else
        return true;
}


QString MainWindow::checkStatus(const bool statusBool, const bool nullFalse)
{
    if (debug) qDebug() << "[MainWindow]" << "[checkStatus]";
    if (debug) qDebug() << "[MainWindow]" << "[checkStatus]" << ":" << "Status" << statusBool;
    if (debug) qDebug() << "[MainWindow]" << "[checkStatus]" << ":" << "Return null false" << nullFalse;

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
    if (debug) qDebug() << "[MainWindow]" << "[createActions]";

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabs(int)));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAboutWindow()));
    connect(ui->actionNetctlAuto, SIGNAL(triggered(bool)), this, SLOT(showNetctlAutoWindow()));
    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(showSettingsWindow()));
    connect(ui->actionReport, SIGNAL(triggered(bool)), this, SLOT(reportABug()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(closeMainWindow()));

    // actions menu
    connect(ui->menuActions, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));
    connect(ui->actionMainEdit, SIGNAL(triggered(bool)), this, SLOT(mainTabEditProfile()));
    connect(ui->actionMainEnable, SIGNAL(triggered(bool)), this, SLOT(mainTabEnableProfile()));
    connect(ui->actionMainRefresh, SIGNAL(triggered(bool)), this, SLOT(updateMainTab()));
    connect(ui->actionMainRemove, SIGNAL(triggered(bool)), this, SLOT(mainTabRemoveProfile()));
    connect(ui->actionMainRestart, SIGNAL(triggered(bool)), this, SLOT(mainTabRestartProfile()));
    connect(ui->actionMainStart, SIGNAL(triggered(bool)), this, SLOT(mainTabStartProfile()));
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
    if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]";

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(DBUS_SERVICE)) {
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register service";
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << bus.lastError().message();
    }
    if (!bus.registerObject(DBUS_OBJECT_PATH,
                            new NetctlGuiAdaptor(this),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register GUI object";
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << bus.lastError().message();
    }
}


void MainWindow::createObjects()
{
    if (debug) qDebug() << "[MainWindow]" << "[createObjects]";

    // error messages
    errorWin = new ErrorWindow(this, debug);
    // backend
    createDBusSession();
    if (useHelper)
        if (!forceStartHelper()) {
            errorWin->showWindow(19, QString("[MainWindow] : [createObjects]"));
            useHelper = false;
        }

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
    if (debug) qDebug() << "[MainWindow]" << "[deleteObjects]";

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


void MainWindow::setIconsToTabs()
{
    if (debug) qDebug() << "[MainWindow]" << "[setIconsToTabs]";

    // tab widget
    ui->tabWidget->setTabIcon(0, QIcon(":icon"));
    ui->tabWidget->setTabIcon(1, QIcon::fromTheme("document-new"));
    ui->tabWidget->setTabIcon(2, QIcon(":wifi"));
}


QMap<QString, QString> MainWindow::parseOptions(const QString options)
{
    if (debug) qDebug() << "[MainWindow]" << "[parseOptions]";

    QMap<QString, QString> settings;
    for (int i=0; i<options.split(QChar(',')).count(); i++) {
        if (options.split(QChar(','))[i].split(QChar('=')).count() < 2)
            continue;
        settings[options.split(QChar(','))[i].split(QChar('='))[0]] =
                options.split(QChar(','))[i].split(QChar('='))[1];
    }
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << "[MainWindow]" << "[parseOptions]" << ":" <<
                    settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


// window slots
void MainWindow::updateConfiguration(const QMap<QString, QVariant> args)
{
    if (debug) qDebug() << "[MainWindow]" << "[updateConfiguration]";

    deleteObjects();

    settingsWin = new SettingsWindow(this, debug, configPath);
    if (args[QString("defauls")].toBool())
        settingsWin->setDefault();
    configuration = settingsWin->getSettings();
    QMap<QString, QString> optionsDict = parseOptions(args[QString("options")].toString());
    for (int i=0; i<optionsDict.keys().count(); i++)
        configuration[optionsDict.keys()[i]] = optionsDict[optionsDict.keys()[i]];
    if ((configuration[QString("USE_HELPER")] == QString("true")) &&
            (checkExternalApps(QString("helper"))))
        useHelper = true;
    else {
        useHelper = false;
        configuration[QString("USE_HELPER")] = QString("false");
    }

    // update translation
    qApp->removeTranslator(translator);
    QString language = Language::defineLanguage(configPath,
                                                args[QString("options")].toString());
    translator->load(QString(":/translations/") + language);
    qApp->installTranslator(translator);
    // update settingsWin
    delete settingsWin;

    createObjects();
    if (useHelper) useHelper = isHelperActive();
    setTab(args[QString("tab")].toInt() - 1);
    createActions();
    setIconsToTabs();

    // tray
    if (isDaemon)
        return;
    if ((QSystemTrayIcon::isSystemTrayAvailable()) &&
            (configuration[QString("SYSTRAY")] == QString("true")))
        trayIcon->setVisible(true);
    else
        trayIcon->setVisible(false);
    if (trayIcon->isVisible()) {
        if (configuration[QString("STARTTOTRAY")] == QString("true"))
            hide();
        else
            show();
        if (args[QString("minimized")].toInt() == 2)
            show();
        else if (args[QString("minimized")].toInt() == 3)
            hide();
    }
    else
        show();
}


void MainWindow::updateMenu()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenu]";

    int tab = ui->tabWidget->currentIndex();
    setMenuActionsShown(false);

    if (tab == 0)
        updateMenuMain();
    else if (tab == 1)
        updateMenuProfile();
    else if (tab == 2)
        updateMenuWifi();
}


void MainWindow::updateTabs(const int tab)
{
    if (debug) qDebug() << "[MainWindow]" << "[updateTabs]";
    if (debug) qDebug() << "[MainWindow]" << "[updateTabs]" << ":" << "Update tab" << tab;

    if (tab == 0)
        updateMainTab();
    else if (tab == 1)
        updateProfileTab();
    else if (tab == 2)
        updateWifiTab();
    updateMenu();
}


void MainWindow::setMenuActionsShown(const bool state)
{
    if (debug) qDebug() << "[MainWindow]" << "[setMenuActionsShown]";
    if (debug) qDebug() << "[MainWindow]" << "[setMenuActionsShown]" << ":" << "State" << state;

    // main
    ui->actionMainEdit->setVisible(state);
    ui->actionMainEnable->setVisible(state);
    ui->actionMainRefresh->setVisible(state);
    ui->actionMainRemove->setVisible(state);
    ui->actionMainRestart->setVisible(state);
    ui->actionMainStart->setVisible(state);
    // profile
    ui->actionProfileClear->setVisible(state);
    ui->actionProfileLoad->setVisible(state);
    ui->actionProfileRemove->setVisible(state);
    ui->actionProfileSave->setVisible(state);
    // wifi
    ui->actionWifiRefresh->setVisible(state);
    ui->actionWifiStart->setVisible(state);
}


void MainWindow::updateMenuMain()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenuMain]";

    ui->actionMainRefresh->setVisible(true);
    if (ui->tableWidget_main->currentItem() == 0)
        return;
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        ui->actionMainRestart->setVisible(true);
        ui->actionMainStart->setText(QApplication::translate("MainWindow", "Stop profile"));
        ui->actionMainStart->setIcon(QIcon::fromTheme("process-stop"));
    }
    else {
        ui->actionMainRestart->setVisible(false);
        ui->actionMainStart->setText(QApplication::translate("MainWindow", "Start profile"));
        ui->actionMainStart->setIcon(QIcon::fromTheme("system-run"));
    }
    ui->actionMainStart->setVisible(true);
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        ui->actionMainEnable->setText(QApplication::translate("MainWindow", "Disable profile"));
        ui->actionMainEnable->setIcon(QIcon::fromTheme("edit-remove"));
    }
    else {
        ui->actionMainEnable->setText(QApplication::translate("MainWindow", "Enable profile"));
        ui->actionMainEnable->setIcon(QIcon::fromTheme("edit-add"));
    }
    ui->actionMainEnable->setVisible(true);
    ui->actionMainEdit->setVisible(true);
    ui->actionMainRemove->setVisible(true);
}


void MainWindow::updateMenuProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenuProfile]";

    ui->actionProfileClear->setVisible(true);
    if (ui->comboBox_profile->currentText().isEmpty()) {
        ui->actionProfileLoad->setVisible(false);
        ui->actionProfileRemove->setVisible(false);
        ui->actionProfileSave->setVisible(false);
    }
    else {
        ui->actionProfileLoad->setVisible(true);
        ui->actionProfileRemove->setVisible(true);
        ui->actionProfileSave->setVisible(true);
    }
}


void MainWindow::updateMenuWifi()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenuWifi]";

    ui->actionWifiRefresh->setVisible(true);
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty()) {
            ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Stop WiFi"));
            ui->actionWifiStart->setIcon(QIcon::fromTheme("process-stop"));
        }
        else {
            ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Start WiFi"));
            ui->actionWifiStart->setIcon(QIcon::fromTheme("system-run"));
        }
    }
    else {
        ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Start WiFi"));
        ui->actionWifiStart->setIcon(QIcon::fromTheme("system-run"));
    }
    ui->actionWifiStart->setVisible(true);
}


void MainWindow::updateMainTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMainTab]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [updateMainTab]"));

    ui->tabWidget->setDisabled(true);
    QList<netctlProfileInfo> profiles;
    bool netctlAutoStatus = false;
    if (useHelper) {
        netctlAutoStatus = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                           DBUS_HELPER_INTERFACE, QString("isNetctlAutoActive"),
                                           QList<QVariant>(), true, debug)[0].toBool();
        profiles = parseOutputNetctl(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                     DBUS_HELPER_INTERFACE, QString("ProfileList"),
                                                     QList<QVariant>(), true, debug), debug);
    }
    else {
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();
        profiles = netctlCommand->getProfileList();
    }
    ui->widget_netctlAuto->setHidden(!netctlAutoStatus);

    ui->tableWidget_main->setSortingEnabled(false);
    ui->tableWidget_main->selectRow(-1);
    ui->tableWidget_main->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_main->clear();
    ui->tableWidget_main->setRowCount(profiles.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("MainWindow", "Name"));
    headerList.append(QApplication::translate("MainWindow", "Description"));
    headerList.append(QApplication::translate("MainWindow", "Active"));
    headerList.append(QApplication::translate("MainWindow", "Enabled"));
    ui->tableWidget_main->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<profiles.count(); i++) {
        // font
        QFont font;
        font.setBold(profiles[i].active);
        font.setItalic(profiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Profile")).arg(profiles[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Active")).arg(checkStatus(profiles[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWindow", "Enabled")).arg(checkStatus(profiles[i].enabled));
        // name
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(profiles[i].name));
        ui->tableWidget_main->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tableWidget_main->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget_main->item(i, 0)->setFont(font);
        // description
        ui->tableWidget_main->setItem(i, 1, new QTableWidgetItem(profiles[i].description));
        ui->tableWidget_main->item(i, 1)->setTextAlignment(Qt::AlignJustify | Qt::AlignVCenter);
        ui->tableWidget_main->item(i, 1)->setToolTip(toolTip);
        // active
        ui->tableWidget_main->setItem(i, 2, new QTableWidgetItem(checkStatus(profiles[i].active, true)));
        ui->tableWidget_main->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // enabled
        ui->tableWidget_main->setItem(i, 3, new QTableWidgetItem(checkStatus(profiles[i].enabled, true)));
        ui->tableWidget_main->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_main->setSortingEnabled(true);

    ui->tableWidget_main->resizeRowsToContents();
    ui->tableWidget_main->resizeColumnsToContents();
    ui->tableWidget_main->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_main->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tableWidget_main->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateProfileTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateProfileTab]";

    ui->tabWidget->setDisabled(true);
    profileTabClear();
    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateWifiTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateWifiTab]";
    wifiTabSetEnabled(checkExternalApps(QString("wpasup")));
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [updateWifiTab]"));

    ui->tabWidget->setDisabled(true);
    QList<netctlWifiInfo> scanResults;
    if (useHelper)
        scanResults = parseOutputWifi(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                                      DBUS_HELPER_INTERFACE, QString("WiFi"),
                                                      QList<QVariant>(), true, debug), debug);
    else
        scanResults = wpaCommand->scanWifi();

    ui->tableWidget_wifi->setSortingEnabled(false);
    ui->tableWidget_wifi->selectRow(-1);
    ui->tableWidget_wifi->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_wifi->clear();
    ui->tableWidget_wifi->setRowCount(scanResults.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("MainWindow", "Name"));
    headerList.append(QApplication::translate("MainWindow", "Signal"));
    headerList.append(QApplication::translate("MainWindow", "Security"));
    headerList.append(QApplication::translate("MainWindow", "Active"));
    headerList.append(QApplication::translate("MainWindow", "Exists"));
    ui->tableWidget_wifi->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<scanResults.count(); i++) {
        // font
        QFont font;
        font.setBold(scanResults[i].active);
        font.setItalic(scanResults[i].exists);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "ESSID")).arg(scanResults[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Active")).arg(checkStatus(scanResults[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWindow", "Exists")).arg(checkStatus(scanResults[i].exists));
        // name
        ui->tableWidget_wifi->setItem(i, 0, new QTableWidgetItem(scanResults[i].name));
        ui->tableWidget_wifi->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget_wifi->item(i, 0)->setFont(font);
        // signal
        ui->tableWidget_wifi->setItem(i, 1, new QTableWidgetItem(scanResults[i].signal));
        ui->tableWidget_wifi->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 1)->setToolTip(toolTip);
        // security
        ui->tableWidget_wifi->setItem(i, 2, new QTableWidgetItem(scanResults[i].security));
        ui->tableWidget_wifi->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 2)->setToolTip(toolTip);
        // active
        ui->tableWidget_wifi->setItem(i, 3, new QTableWidgetItem(checkStatus(scanResults[i].active, true)));
        ui->tableWidget_wifi->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // exists
        ui->tableWidget_wifi->setItem(i, 4, new QTableWidgetItem(checkStatus(scanResults[i].exists, true)));
        ui->tableWidget_wifi->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_wifi->setSortingEnabled(true);

    ui->tableWidget_wifi->resizeRowsToContents();
    ui->tableWidget_wifi->resizeColumnsToContents();
    ui->tableWidget_wifi->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_wifi->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tableWidget_wifi->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}
