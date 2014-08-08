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
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QTranslator>
#include <QUrl>

#include <netctlgui/netctlgui.h>

#include "aboutwindow.h"
#include "bridgewidget.h"
#include "controladaptor.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "language.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "netctladaptor.h"
#include "netctlautowindow.h"
#include "netctlguiadaptor.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "settingswindow.h"
#include "trayicon.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "version.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"

#include <polkit-qt5-1/polkitqt1-authority.h>
#include <polkit-qt5-1/polkitqt1-subject.h>


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

    deleteObjects();
}


QString MainWindow::getInformation()
{
    if (debug) qDebug() << "[MainWindow]" << "[getInformation]";

    QString profile;
    QString status;
    if (netctlCommand->isNetctlAutoRunning()) {
        profile = netctlCommand->autoGetActiveProfile();
        status = QString("netctl-auto");
    }
    else {
        profile = netctlCommand->getActiveProfile();
        status = netctlCommand->getProfileStatus(profile);
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


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (debug) qDebug() << "[MainWindow]" << "[closeEvent]";

    if ((QSystemTrayIcon::isSystemTrayAvailable()) &&
            (configuration[QString("SYSTRAY")] == QString("true"))) {
        hide();
        event->ignore();
    }
    else
        closeMainWindow();
}


bool MainWindow::checkExternalApps(const QString apps = QString("all"))
{
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]";

    QStringList commandLine;
    commandLine.append("which");
    commandLine.append(configuration[QString("SUDO_PATH")]);
    if ((apps == QString("netctl")) || (apps == QString("all"))) {
        commandLine.append(configuration[QString("NETCTL_PATH")]);
        commandLine.append(configuration[QString("NETCTLAUTO_PATH")]);
    }
    if ((apps == QString("systemctl")) || (apps == QString("all"))) {
        commandLine.append(configuration[QString("SYSTEMCTL_PATH")]);
    }
    if ((apps == QString("wpasup")) || (apps == QString("all"))) {
        commandLine.append(configuration[QString("WPACLI_PATH")]);
        commandLine.append(configuration[QString("WPASUP_PATH")]);
    }
    QProcess command;
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Run cmd" << commandLine.join(QChar(' '));
    command.start(commandLine.join(QChar(' ')));
    command.waitForFinished(-1);
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() != 0)
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
    if (!bus.registerService(QString(DBUS_SERVICE)))
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register service";
    if (!bus.registerObject(QString(DBUS_OBJECT_PATH),
                            new NetctlGuiAdaptor(this),
                            QDBusConnection::ExportAllContents))
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register GUI object";
    if (!bus.registerObject(QString(DBUS_LIB_PATH),
                            new NetctlAdaptor(this, configuration),
                            QDBusConnection::ExportAllContents))
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register library object";
    if (!bus.registerObject(QString(DBUS_CONTROL_PATH),
                            new ControlAdaptor(this, configuration),
                            QDBusConnection::ExportAllContents))
        if (debug) qDebug() << "[MainWindow]" << "[createDBusSession]" << ":" << "Could not register control object";
}


void MainWindow::createObjects()
{
    if (debug) qDebug() << "[MainWindow]" << "[createObjects]";

    // backend
    createDBusSession();
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
    errorWin = new ErrorWindow(this, debug);
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

    QDBusConnection::sessionBus().unregisterService(QString(DBUS_SERVICE));
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
    if (debug) qDebug() << "[MainWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Return)
        if (ui->comboBox_profile->hasFocus())
            profileTabLoadProfile();
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


void MainWindow::reportABug()
{
    if (debug) qDebug() << "[MainWindow]" << "[reportABug]";

    if (QDesktopServices::openUrl(QUrl(QString(BUGTRACKER))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


// window slots
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


void MainWindow::closeMainWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[closeMainWindow]";

    qApp->quit();
}


void MainWindow::showAboutWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showAboutWindow]";

    aboutWin->show();
}


void MainWindow::showMainWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showMainWindow]";

    if (isHidden()) {
        updateTabs(ui->tabWidget->currentIndex());
        show();
    }
    else
        hide();
}


void MainWindow::showNetctlAutoWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showNetctlAutoWindow]";

    netctlAutoWin->showWindow();
}


void MainWindow::showSettingsWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showSettingsWindow]";

    settingsWin->showWindow();
}


void MainWindow::setTab(int tab)
{
    if (debug) qDebug() << "[MainWindow]" << "[setTab]";
    if (debug) qDebug() << "[MainWindow]" << "[setTab]" << ":" << "Update tab" << tab;

    switch (tab) {
    case 0:
    case 1:
    case 2:
        break;
    default:
        tab = 0;
        break;
    }
    ui->tabWidget->setCurrentIndex(tab);

    updateTabs(tab);
}


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

    // update translation
    qApp->removeTranslator(translator);
    QString language = Language::defineLanguage(configPath,
                                                args[QString("options")].toString());
    translator->load(QString(":/translations/") + language);
    qApp->installTranslator(translator);
    // update settingsWin
    delete settingsWin;

    createObjects();
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


void MainWindow::updateMainTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMainTab]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [updateMainTab]"));

    ui->tabWidget->setDisabled(true);
    QList<netctlProfileInfo> profiles = netctlCommand->getProfileList();

    if (netctlCommand->isNetctlAutoRunning())
        ui->widget_netctlAuto->setHidden(false);
    else
        ui->widget_netctlAuto->setHidden(true);

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


void MainWindow::updateProfileTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateProfileTab]";

    ui->tabWidget->setDisabled(true);
    profileTabClear();
    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
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


void MainWindow::updateWifiTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateWifiTab]";
    wifiTabSetEnabled(checkExternalApps(QString("wpasup")));
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [updateWifiTab]"));

    ui->tabWidget->setDisabled(true);
    QList<netctlWifiInfo> scanResults = wpaCommand->scanWifi();

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


// main tab slots
void MainWindow::mainTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]";
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    // create menu
    QMenu menu(this);
    QAction *refreshTable = menu.addAction(QApplication::translate("MainWindow", "Refresh"));
    refreshTable->setIcon(QIcon::fromTheme("stock-refresh"));
    menu.addSeparator();
    QAction *startProfile = menu.addAction(QApplication::translate("MainWindow", "Start profile"));
    QAction *restartProfile = menu.addAction(QApplication::translate("MainWindow", "Restart profile"));
    restartProfile->setIcon(QIcon::fromTheme("stock-refresh"));
    QAction *enableProfile = menu.addAction(QApplication::translate("MainWindow", "Enable profile"));
    menu.addSeparator();
    QAction *editProfile = menu.addAction(QApplication::translate("MainWindow", "Edit profile"));
    editProfile->setIcon(QIcon::fromTheme("edit"));
    QAction *removeProfile = menu.addAction(QApplication::translate("MainWindow", "Remove profile"));
    removeProfile->setIcon(QIcon::fromTheme("edit-delete"));

    // set text
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        restartProfile->setVisible(true);
        startProfile->setText(QApplication::translate("MainWindow", "Stop profile"));
        startProfile->setIcon(QIcon::fromTheme("process-stop"));
    }
    else {
        restartProfile->setVisible(false);
        startProfile->setText(QApplication::translate("MainWindow", "Start profile"));
        startProfile->setIcon(QIcon::fromTheme("system-run"));
    }
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        enableProfile->setText(QApplication::translate("MainWindow", "Disable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-remove"));
    }
    else {
        enableProfile->setText(QApplication::translate("MainWindow", "Enable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-add"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Refresh table";
        updateMainTab();
    }
    else if (action == startProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Start profile";
        mainTabStartProfile();
    }
    else if (action == restartProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Restart profile";
        mainTabRestartProfile();
    }
    else if (action == enableProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Enable profile";
        mainTabEnableProfile();
    }
    else if (action == editProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Edit profile";
        mainTabEditProfile();
    }
    else if (action == removeProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Remove profile";
        mainTabRemoveProfile();
    }
}


void MainWindow::mainTabEditProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabEditProfile]";

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    ui->tabWidget->setCurrentIndex(1);
    ui->comboBox_profile->addItem(profile);
    ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count()-1);
}


void MainWindow::mainTabRemoveProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRemoveProfile]";

    ui->tabWidget->setDisabled(true);
    // call netctlprofile
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    if (netctlProfile->removeProfile(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabEnableProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabEnableProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabEnableProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->enableProfile(profile);
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        if (netctlCommand->isProfileEnabled(profile))
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    }
    else {
        if (netctlCommand->isProfileEnabled(profile))
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    }

    updateMainTab();
}


void MainWindow::mainTabRestartProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRestartProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabRestartProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->restartProfile(profile);
    if (netctlCommand->isProfileActive(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabStartProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabStartProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabStartProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        netctlCommand->startProfile(profile);
        if (netctlCommand->isProfileActive(profile))
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    }
    else {
        if (netctlCommand->getActiveProfile().isEmpty())
            netctlCommand->startProfile(profile);
        else
            netctlCommand->switchToProfile(profile);
        if (netctlCommand->isProfileActive(profile))
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    }

    updateMainTab();
}


void MainWindow::mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRefreshButtons]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabRefreshButtons]"));
    if (current == 0) {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setDisabled(true);
        return;
    }

    ui->pushButton_mainStart->setEnabled(true);

    if (!ui->tableWidget_main->item(current->row(), 2)->text().isEmpty()) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("process-stop"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("system-run"));
    }
}


// profile tab slots
void MainWindow::profileTabChangeState(const QString current)
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabChangeState]";
    if (debug) qDebug() << "[MainWindow]" << "[profileTabChangeState]" << ":" << "Current type" << current;

    if (current == QString("ethernet")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(true);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("wireless")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(true);
    }
    else if ((current == QString("bond")) ||
             (current == QString("dummy"))) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("bridge")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(true);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("pppoe")) {
        generalWid->setShown(true);
        ipWid->setShown(false);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(true);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("mobile_ppp")) {
        generalWid->setShown(true);
        ipWid->setShown(false);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(true);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("tunnel")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(true);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("tuntap")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(false);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(true);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
    else if (current == QString("vlan")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(true);
        macvlanWid->setShown(false);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(true);
        wirelessWid->setShown(false);
    }
    else if (current == QString("macvlan")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(true);
        macvlanWid->setShown(true);
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(false);
        wirelessWid->setShown(false);
    }
}


void MainWindow::profileTabClear()
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabClear]";

    ui->comboBox_profile->clear();
    QList<netctlProfileInfo> profiles = netctlCommand->getProfileList();
    for (int i=0; i<profiles.count(); i++)
        ui->comboBox_profile->addItem(profiles[i].name);
    ui->comboBox_profile->setCurrentIndex(-1);

    generalWid->clear();
    ipWid->clear();
    bridgeWid->clear();
    ethernetWid->clear();
    macvlanWid->clear();
    mobileWid->clear();
    pppoeWid->clear();
    tunnelWid->clear();
    tuntapWid->clear();
    vlanWid->clear();
    wirelessWid->clear();

    profileTabChangeState(generalWid->connectionType->currentText());
}


void MainWindow::profileTabCreateProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabCreateProfile]";

    // error checking
    if (ui->comboBox_profile->currentText().isEmpty())
        return errorWin->showWindow(3, QString("[MainWindow] : [profileTabCreateProfile]"));
    if (generalWid->isOk() == 1)
        return errorWin->showWindow(4, QString("[MainWindow] : [profileTabCreateProfile]"));
    else if (generalWid->isOk() == 2)
        return errorWin->showWindow(5, QString("[MainWindow] : [profileTabCreateProfile]"));
    if ((generalWid->connectionType->currentText() == QString("ethernet")) ||
            (generalWid->connectionType->currentText() == QString("wireless")) ||
            (generalWid->connectionType->currentText() == QString("bond")) ||
            (generalWid->connectionType->currentText() == QString("dummy")) ||
            (generalWid->connectionType->currentText() == QString("bridge")) ||
            (generalWid->connectionType->currentText() == QString("tunnel")) ||
            (generalWid->connectionType->currentText() == QString("tuntap")) ||
            (generalWid->connectionType->currentText() == QString("vlan")) ||
            (generalWid->connectionType->currentText() == QString("macvlan"))) {
        if (ipWid->isOk() == 1)
            return errorWin->showWindow(6, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (ipWid->isOk() == 2)
            return errorWin->showWindow(6, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1)
            return errorWin->showWindow(8, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 2)
            return errorWin->showWindow(9, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 3)
            return errorWin->showWindow(10, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 4)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 5)
            return errorWin->showWindow(11, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 2)
            return errorWin->showWindow(12, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 3)
            return errorWin->showWindow(13, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 4)
            return errorWin->showWindow(12, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1)
            return errorWin->showWindow(14, QString("[MainWindow] : [profileTabCreateProfile]"));
        if (mobileWid->isOk() == 2)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1)
            return errorWin->showWindow(15, QString("[MainWindow] : [profileTabCreateProfile]"));
        if (tuntapWid->isOk() == 2)
            return errorWin->showWindow(15, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }

    ui->tabWidget->setDisabled(true);
    // read settings
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    QMap<QString, QString> settings;
    settings = generalWid->getSettings();
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = wirelessWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if ((generalWid->connectionType->currentText() == QString("bond")) ||
             (generalWid->connectionType->currentText() == QString("dummy"))) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings =  bridgeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        QMap<QString, QString> addSettings = pppoeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        QMap<QString, QString> addSettings = mobileWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tunnelWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tuntapWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = vlanWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = macvlanWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }

    // call netctlprofile
    QString profileTempName = netctlProfile->createProfile(profile, settings);
    if (netctlProfile->copyProfile(profileTempName))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateProfileTab();
}


void MainWindow::profileTabLoadProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabLoadProfile]";

    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    if (profile.isEmpty())
        return;
    QMap<QString, QString> settings = netctlProfile->getSettingsFromProfile(profile);
    if (settings.isEmpty())
        return errorWin->showWindow(17, QString("[MainWindow] : [profileTabLoadProfile]"));

    generalWid->setSettings(settings);
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        ipWid->setSettings(settings);
        wirelessWid->setSettings(settings);
    }
    else if ((generalWid->connectionType->currentText() == QString("bond")) ||
             (generalWid->connectionType->currentText() == QString("dummy"))) {
        ipWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
        ipWid->setSettings(settings);
        bridgeWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        pppoeWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        mobileWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        ipWid->setSettings(settings);
        tunnelWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        ipWid->setSettings(settings);
        tuntapWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        vlanWid->setSettings(settings);
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        macvlanWid->setSettings(settings);
    }
}


void MainWindow::profileTabRemoveProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabRemoveProfile]";

    ui->tabWidget->setDisabled(true);
    // call netctlprofile
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    if (netctlProfile->removeProfile(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateProfileTab();
}


// wifi tab slots
void MainWindow::wifiTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]";
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;

    // create menu
    QMenu menu(this);
    QAction *refreshTable = menu.addAction(QApplication::translate("MainWindow", "Refresh"));
    refreshTable->setIcon(QIcon::fromTheme("stock-refresh"));
    menu.addSeparator();
    QAction *startWifi = menu.addAction(QApplication::translate("MainWindow", "Start WiFi"));

    // set text
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty()) {
            startWifi->setText(QApplication::translate("MainWindow", "Stop WiFi"));
            startWifi->setIcon(QIcon::fromTheme("process-stop"));
        }
        else {
            startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
            startWifi->setIcon(QIcon::fromTheme("system-run"));
        }
    }
    else {
        startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
        startWifi->setIcon(QIcon::fromTheme("system-run"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]" << "Refresh WiFi";
        updateWifiTab();
    }
    else if (action == startWifi) {
        if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]" << "Start WiFi";
        wifiTabStart();
    }
}


void MainWindow::wifiTabSetEnabled(const bool state)
{
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabSetEnables]";
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabSetEnables]" << ":" << "State" << state;

    if (state) {
        ui->tableWidget_wifi->show();
        ui->pushButton_wifiRefresh->setEnabled(true);
        ui->label_wifi->hide();
    }
    else {
        ui->tableWidget_wifi->hide();
        ui->pushButton_wifiRefresh->setDisabled(true);
        ui->pushButton_wifiStart->setDisabled(true);
        ui->label_wifi->show();
    }
}


void MainWindow::connectToUnknownEssid(const QString passwd)
{
    if (debug) qDebug() << "[MainWindow]" << "[connectToUnknownEssid]";

    if (passwdWid != 0)
        delete passwdWid;
    if (netctlCommand->getWirelessInterfaceList().isEmpty())
        return;

    QMap<QString, QString> settings;
    settings[QString("Description")] = QString("'Automatically generated profile by Netctl GUI'");
    settings[QString("Interface")] = netctlCommand->getWirelessInterfaceList()[0];
    settings[QString("Connection")] = QString("wireless");
    QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 2)->text();
    if (security.contains(QString("WPA")))
        settings[QString("Security")] = QString("wpa");
    else if (security.contains(QString("WEP")))
        settings[QString("Security")] = QString("wep");
    else
        settings[QString("Security")] = QString("none");
    settings[QString("ESSID")] = QString("'") +
            ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() +
            QString("'");
    if (!passwd.isEmpty())
        settings[QString("Key")] = QString("'") + passwd + QString("'");
    settings[QString("IP")] = QString("dhcp");
    if (hiddenNetwork)
        settings[QString("Hidden")] = QString("yes");

    QString profile = QString("netctl-gui-") + settings[QString("ESSID")];
    profile.remove(QChar('"')).remove(QChar('\''));
    QString profileTempName = netctlProfile->createProfile(profile, settings);
    if (netctlProfile->copyProfile(profileTempName))
        netctlCommand->startProfile(profile);
    if (netctlCommand->isProfileActive(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateWifiTab();
}


void MainWindow::setHiddenName(const QString name)
{
    if (debug) qDebug() << "[MainWindow]" << "[setHiddenName]";
    if (debug) qDebug() << "[MainWindow]" << "[setHiddenName]" << ":" << "Set name" << name;

    ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->setText(name);

    wifiTabStart();
}


void MainWindow::wifiTabStart()
{
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabStart]";
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [wifiTabStart]"));
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;

    // name is hidden
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() == QString("<hidden>")) {
        hiddenNetwork = true;
        passwdWid = new PasswdWidget(this);
        passwdWid->setPassword(false);
        int widgetWidth = 270;
        int widgetHeight = 86;
        int x = (width() - widgetWidth) / 2;
        int y = (height() - widgetHeight) / 2;
        passwdWid->setGeometry(x, y, widgetWidth, widgetHeight);
        passwdWid->show();
        passwdWid->setFocusToLineEdit();
        return;
    }

    // name isn't hidden
    ui->tabWidget->setDisabled(true);
    hiddenNetwork = false;
    QString profile = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text();
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        QString profileName = wpaCommand->existentProfile(profile);
        netctlCommand->startProfile(profileName);
        if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty()) {
            if (netctlCommand->isProfileActive(profileName))
                ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
            else
                ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
        }
        else {
            if (netctlCommand->isProfileActive(profileName))
                ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
            else
                ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
        }
    }
    else {
        QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
        if (security.contains(QString("none")))
            return connectToUnknownEssid(QString(""));
        else {
            passwdWid = new PasswdWidget(this);
            passwdWid->setPassword(true);
            int widgetWidth = 270;
            int widgetHeight = 86;
            int x = (width() - widgetWidth) / 2;
            int y = (height() - widgetHeight) / 2;
            passwdWid->setGeometry(x, y, widgetWidth, widgetHeight);
            passwdWid->show();
            passwdWid->setFocusToLineEdit();
            return;
        }
    }

    updateWifiTab();
}


void MainWindow::wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabRefreshButtons]";
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [wifiTabRefreshButtons]"));
    if (current == 0) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }
    if (ui->tableWidget_wifi->item(current->row(), 0)->text() == QString("<hidden>")) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }

    ui->pushButton_wifiStart->setEnabled(true);
    if (!ui->tableWidget_wifi->item(current->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(current->row(), 3)->text().isEmpty()) {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Stop"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("process-stop"));
        }
        else {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("system-run"));
        }
    }
    else {
        ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("system-run"));
    }
}
