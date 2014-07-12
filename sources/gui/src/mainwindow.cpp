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

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>

#include <netctlgui/netctlinteract.h>
#include <netctlgui/netctlprofile.h>
#include <netctlgui/wpasupinteract.h>

#include "bridgewidget.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "netctlautowindow.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "settingswindow.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"


MainWindow::MainWindow(QWidget *parent, const bool defaultSettings, const bool debugCmd, const bool netctlAuto, const int tabNum)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      debug(debugCmd)
{
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "defaultSettings" << defaultSettings;
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "tabNum" << tabNum;
    if (debug) qDebug() << "[MainWindow]" << "[MainWindow]" << ":" << "debug" << debug;

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(tabNum-1);

    QString configPath = QDir::homePath() + QDir::separator() + QString(".config") +
            QDir::separator() + QString("netctl-gui.conf");
    settingsWin = new SettingsWindow(this, debug, configPath);
    if (defaultSettings)
        settingsWin->setDefault();
    configuration = settingsWin->getSettings();

    // gui
    netctlAutoWin = new NetctlAutoWindow(this, debug, configuration);
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
    // backend
    netctlCommand = new Netctl(debug, configuration);
    netctlProfile = new NetctlProfile(debug, configuration);
    wpaCommand = new WpaSup(debug, configuration);

    createActions();
    setIconsToButtons();
    updateTabs(ui->tabWidget->currentIndex());
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Ready"));

    if (netctlAuto)
        netctlAutoWin->showWindow();
}


MainWindow::~MainWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[~MainWindow]";

    delete netctlCommand;
    delete netctlProfile;
    delete wpaCommand;

    delete bridgeWid;
    delete ethernetWid;
    delete generalWid;
    delete ipWid;
    delete macvlanWid;
    delete mobileWid;
    delete pppoeWid;
    delete tunnelWid;
    delete tuntapWid;
    delete vlanWid;
    delete wirelessWid;

    delete netctlAutoWin;
    delete settingsWin;
    delete ui;
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
    if ((apps == QString("wpaact")) || (apps == QString("all"))) {
        commandLine.append(configuration[QString("WPAACTIOND_PATH")]);
    }
    QProcess command;
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Run cmd" << commandLine.join(QString(" "));
    command.start(commandLine.join(QString(" ")));
    command.waitForFinished(-1);
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Cmd returns" << command.exitCode();
    if (command.exitCode() != 0)
        return false;
    else
        return true;
}


bool MainWindow::checkState(const QString state, const QString item)
{
    if (debug) qDebug() << "[MainWindow]" << "[checkState]";
    if (debug) qDebug() << "[MainWindow]" << "[checkState]" << ":" << "Text" << item;
    if (debug) qDebug() << "[MainWindow]" << "[checkState]" << ":" << "State" << state;

    if (item.contains(state))
        return true;
    else
        return false;
}


// window signals
void MainWindow::createActions()
{
    if (debug) qDebug() << "[MainWindow]" << "[createActions]";

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabs(int)));
    connect(ui->actionNetctlAuto, SIGNAL(triggered(bool)), netctlAutoWin, SLOT(showWindow()));
    ui->actionNetctlAuto->setVisible(checkExternalApps(QString("all")));
    connect(ui->actionSettings, SIGNAL(triggered(bool)), settingsWin, SLOT(showWindow()));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

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


void MainWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[MainWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Return)
        if (ui->comboBox_profile->hasFocus())
            profileTabLoadProfile();
}


void MainWindow::setIconsToButtons()
{
    if (debug) qDebug() << "[MainWindow]" << "[setIconsToButtons]";

    // tab widget
    ui->tabWidget->setTabIcon(0, QIcon(":icon"));
    ui->tabWidget->setTabIcon(1, QIcon::fromTheme("document-new"));
    ui->tabWidget->setTabIcon(2, QIcon(":wifi"));

    // main tab
    ui->pushButton_mainRefresh->setIcon(QIcon::fromTheme("stock-refresh"));
    ui->pushButton_mainRestart->setIcon(QIcon::fromTheme("stock-refresh"));

    // profile tab
    ui->pushButton_profileClear->setIcon(QIcon::fromTheme("edit-clear"));
    ui->pushButton_profileSave->setIcon(QIcon::fromTheme("document-save"));


    // wifi tab
    ui->pushButton_wifiRefresh->setIcon(QIcon::fromTheme("stock-refresh"));
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
}


void MainWindow::updateMenu()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenu]";

    setMenuActionsShown(false);
    int tab = ui->tabWidget->currentIndex();
    if (debug) qDebug() << "[MainWindow]" << "[updateMenu]" << ":" << "Current tab" << tab;
    if (tab == 0)
        updateMenuMain();
    else if (tab == 1)
        updateMenuProfile();
    else if (tab == 2)
        updateMenuWifi();
}


void MainWindow::updateMainTab()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMainTab]";

    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }

    ui->tabWidget->setDisabled(true);
    QList<QStringList> profiles = netctlCommand->getProfileList();;

    ui->tableWidget_main->setSortingEnabled(false);
    ui->tableWidget_main->selectRow(-1);
    ui->tableWidget_main->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_main->clear();
    ui->tableWidget_main->setRowCount(profiles.count());

    // create header
    ui->tableWidget_main->setHorizontalHeaderLabels(QApplication::translate("MainWindow", "Name==Description==Status")
                                                    .split(QString("==")));
    // create items
    for (int i=0; i<profiles.count(); i++) {
        // name
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(profiles[i][0]));
        ui->tableWidget_main->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        // description
        ui->tableWidget_main->setItem(i, 1, new QTableWidgetItem(profiles[i][1]));
        ui->tableWidget_main->item(i, 1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        // status
        ui->tableWidget_main->setItem(i, 2, new QTableWidgetItem(profiles[i][2]));
        ui->tableWidget_main->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_main->setSortingEnabled(true);
    ui->tableWidget_main->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_main->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget_main->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
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
    QString item = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text();
    if (!checkState(QString("inactive"), item)) {
        ui->actionMainRestart->setVisible(true);
        ui->actionMainStart->setText(QApplication::translate("MainWindow", "Stop profile"));
        ui->actionMainStart->setIcon(QIcon::fromTheme("dialog-close"));
    }
    else {
        ui->actionMainRestart->setVisible(false);
        ui->actionMainStart->setText(QApplication::translate("MainWindow", "Start profile"));
        ui->actionMainStart->setIcon(QIcon::fromTheme("dialog-apply"));
    }
    ui->actionMainStart->setVisible(true);
    if (checkState(QString("enabled"), item)) {
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
    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }

    QList<QStringList> scanResults = wpaCommand->scanWifi();

    ui->tabWidget->setDisabled(true);
    ui->tableWidget_wifi->setSortingEnabled(false);
    ui->tableWidget_wifi->selectRow(-1);
    ui->tableWidget_wifi->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_wifi->clear();
    ui->tableWidget_wifi->setRowCount(scanResults.count());

    // create header
    ui->tableWidget_wifi->setHorizontalHeaderLabels(QApplication::translate("MainWindow", "Name==Status==Signal==Security")
                                                    .split(QString("==")));
    // create items
    for (int i=0; i<scanResults.count(); i++) {
        // name
        ui->tableWidget_wifi->setItem(i, 0, new QTableWidgetItem(scanResults[i][0]));
        ui->tableWidget_wifi->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        // status
        ui->tableWidget_wifi->setItem(i, 1, new QTableWidgetItem(scanResults[i][1]));
        ui->tableWidget_wifi->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // signal
        ui->tableWidget_wifi->setItem(i, 2, new QTableWidgetItem(scanResults[i][2]));
        ui->tableWidget_wifi->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // security
        ui->tableWidget_wifi->setItem(i, 3, new QTableWidgetItem(scanResults[i][3]));
        ui->tableWidget_wifi->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_wifi->setSortingEnabled(true);
    ui->tableWidget_wifi->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_wifi->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget_wifi->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
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
    QString item = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
    if (checkState(QString("exists"), item)) {
        if (!checkState(QString("inactive"), item)) {
            ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Stop WiFi"));
            ui->actionWifiStart->setIcon(QIcon::fromTheme("dialog-close"));
        }
        else {
            ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Start WiFi"));
            ui->actionWifiStart->setIcon(QIcon::fromTheme("dialog-apply"));
        }
    }
    else {
        ui->actionWifiStart->setText(QApplication::translate("MainWindow", "Start WiFi"));
        ui->actionWifiStart->setIcon(QIcon::fromTheme("dialog-apply"));
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
    QString item = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text();
    if (!checkState(QString("inactive"), item)) {
        restartProfile->setVisible(true);
        startProfile->setText(QApplication::translate("MainWindow", "Stop profile"));
        startProfile->setIcon(QIcon::fromTheme("dialog-close"));
    }
    else {
        restartProfile->setVisible(false);
        startProfile->setText(QApplication::translate("MainWindow", "Start profile"));
        startProfile->setIcon(QIcon::fromTheme("dialog-apply"));
    }
    if (checkState(QString("enabled"), item)) {
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
    ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->findText(profile));

    profileTabLoadProfile();
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

    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->enableProfile(profile);
    QString item = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text();
    if (checkState(QString("enabled"), item)) {
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

    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
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

    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->startProfile(profile);
    QString item = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text();
    if (checkState(QString("inactive"), item)) {
        if (netctlCommand->isProfileActive(profile))
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    }
    else {
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

    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
    if (current == 0) {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setDisabled(true);
        return;
    }

    ui->pushButton_mainStart->setEnabled(true);

    QString item = ui->tableWidget_main->item(current->row(), 2)->text();
    if (!checkState(QString("inactive"), item)) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("dialog-close"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("dialog-apply"));
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
    QList<QStringList> profiles = netctlCommand->getProfileList();
    for (int i=0; i<profiles.count(); i++)
        ui->comboBox_profile->addItem(profiles[i][0]);
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
    if (ui->comboBox_profile->currentText().isEmpty()) {
        errorWin = new ErrorWindow(this, debug, 3);
        errorWin->show();
        return;
    }
    if (generalWid->isOk() == 1) {
        errorWin = new ErrorWindow(this, debug, 4);
        errorWin->show();
        return;
    }
    else if (generalWid->isOk() == 2) {
        errorWin = new ErrorWindow(this, debug, 5);
        errorWin->show();
        return;
    }
    if ((generalWid->connectionType->currentText() == QString("ethernet")) ||
            (generalWid->connectionType->currentText() == QString("wireless")) ||
            (generalWid->connectionType->currentText() == QString("bond")) ||
            (generalWid->connectionType->currentText() == QString("dummy")) ||
            (generalWid->connectionType->currentText() == QString("bridge")) ||
            (generalWid->connectionType->currentText() == QString("tunnel")) ||
            (generalWid->connectionType->currentText() == QString("tuntap")) ||
            (generalWid->connectionType->currentText() == QString("vlan")) ||
            (generalWid->connectionType->currentText() == QString("macvlan"))) {
        if (ipWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 6);
            errorWin->show();
            return;
        }
        else if (ipWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, debug, 6);
            errorWin->show();
            return;
        }
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 8);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, debug, 9);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 3) {
            errorWin = new ErrorWindow(this, debug, 10);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 4) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 5) {
            errorWin = new ErrorWindow(this, debug, 11);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, debug, 12);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 3) {
            errorWin = new ErrorWindow(this, debug, 13);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 4) {
            errorWin = new ErrorWindow(this, debug, 12);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 14);
            errorWin->show();
            return;
        }
        if (mobileWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 15);
            errorWin->show();
            return;
        }
        if (tuntapWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, debug, 15);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        if (ethernetWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, debug, 7);
            errorWin->show();
            return;
        }
    }

    ui->tabWidget->setDisabled(true);
    // read settings
    QString profile = netctlProfile->getNameByString(ui->comboBox_profile->currentText());
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

    QString profile = netctlProfile->getNameByString(ui->comboBox_profile->currentText());
    QMap<QString, QString> settings = netctlProfile->getSettingsFromProfile(profile);

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
    QString profile = netctlProfile->getNameByString(ui->comboBox_profile->currentText());
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
    QString item = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
    if (checkState(QString("exists"), item)) {
        if (!checkState(QString("inactive"), item)) {
            startWifi->setText(QApplication::translate("MainWindow", "Stop WiFi"));
            startWifi->setIcon(QIcon::fromTheme("dialog-close"));
        }
        else {
            startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
            startWifi->setIcon(QIcon::fromTheme("dialog-apply"));
        }
    }
    else {
        startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
        startWifi->setIcon(QIcon::fromTheme("dialog-apply"));
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

    if (!passwd.isEmpty())
        delete passwdWid;

    QMap<QString, QString> settings;
    settings[QString("Description")] = QString("'Automatically generated profile by Netctl GUI'");
    settings[QString("Interface")] = wpaCommand->getInterfaceList()[0];
    settings[QString("Connection")] = QString("wireless");
    QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text();
    if (checkState(QString("WPA"), security))
        settings[QString("Security")] = QString("wpa");
    else if (checkState(QString("wep"), security))
        settings[QString("Security")] = QString("wep");
    else
        settings[QString("Security")] = QString("none");
    settings[QString("ESSID")] = QString("'") +
            ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() +
            QString("'");
    if (!passwd.isEmpty())
        settings[QString("Key")] = QString("'") + passwd + QString("'");
    settings[QString("IP")] = QString("dhcp");

    QString profile = QString("netctl-gui-") + settings[QString("ESSID")];
    profile.remove(QString("'"));
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

    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() == QString("<hidden>")) {
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

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text();
    QString item = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
    if (checkState(QString("exists"), item)) {
        QString profileName = wpaCommand->existentProfile(profile);
        netctlCommand->startProfile(profileName);
        item = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
        if (checkState(QString("inactive"), item)) {
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
        QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text();
        if (checkState(QString("none"), security))
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

    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, debug, 1);
        errorWin->show();
        return;
    }
    if (current == 0) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }
    if (ui->tableWidget_wifi->item(current->row(), 0)->text() == QString("<hidden>")) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }

    ui->pushButton_wifiStart->setEnabled(true);
    QString item = ui->tableWidget_wifi->item(current->row(), 1)->text();
    if (checkState(QString("exists"), item)) {
        if (!checkState(QString("inactive"), item)) {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Stop"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("dialog-close"));
        }
        else {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("dialog-apply"));
        }
    }
    else {
        ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("dialog-apply"));
    }
}
