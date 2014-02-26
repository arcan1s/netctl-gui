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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QProcess>

#include "bridgewidget.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "mobilewidget.h"
#include "netctlinteract.h"
#include "netctlprofile.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "vlanwidget.h"
#include "wpasupinteract.h"
#include "wirelesswidget.h"
#include <cstdio>


MainWindow::MainWindow(QWidget *parent, bool defaultSettings, int tabNum)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(tabNum-1);

    //  SettingsWindow *settingsWindow;
    //  settingsWindow = new SettingsWindow(this);
    // if (defaultSettings)
    //      стандартные настройки
    //  delete settingsWindow;

    // temporary block
    netctlPath = QString("/usr/bin/netctl");
    profileDir = QString("/etc/netctl");
    sudoPath = QString("/usr/bin/kdesu");
    wpaConfig.append(QString("/usr/bin/wpa_cli"));
    wpaConfig.append(QString("/usr/bin/wpa_supplicant"));
    preferedInterface  = QString("");
    //// additional settings
    ifaceDir = QString("/sys/class/net/");
    rfkillDir = QString("/sys/class/rfkill/");
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui.pid"));
    wpaConfig.append(QString("nl80211,wext"));
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui"));
    wpaConfig.append(QString("users"));

    // gui
    generalWid = new GeneralWidget(this, ifaceDir, profileDir);
    ui->scrollAreaWidgetContents->layout()->addWidget(generalWid);
    ipWid = new IpWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ipWid);
    bridgeWid = new BridgeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(bridgeWid);
    ethernetWid = new EthernetWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ethernetWid);
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
    wirelessWid = new WirelessWidget(this, rfkillDir);
    ui->scrollAreaWidgetContents->layout()->addWidget(wirelessWid);
    // backend
    netctlCommand = new Netctl(this, netctlPath, profileDir, sudoPath);
    netctlProfile = new NetctlProfile(this, profileDir, sudoPath);
    wpaCommand = new WpaSup(this, wpaConfig, sudoPath, ifaceDir, preferedInterface);

    createActions();
    updateTabs(ui->tabWidget->currentIndex());
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Ready"));
}


MainWindow::~MainWindow()
{
    delete netctlCommand;
    delete netctlProfile;
    delete wpaCommand;

    delete bridgeWid;
    delete ethernetWid;
    delete generalWid;
    delete ipWid;
    delete mobileWid;
    delete pppoeWid;
    delete tunnelWid;
    delete tuntapWid;
    delete vlanWid;
    delete wirelessWid;
    delete ui;
}


bool MainWindow::checkExternalApps(QString apps = QString("all"))
{
    QStringList commandLine;
    commandLine.append("which");
    commandLine.append(sudoPath);
    if ((apps == QString("netctl")) || (apps == QString("all"))) {
        commandLine.append(netctlPath);
    }
    if ((apps == QString("wpasup")) || (apps == QString("all"))) {
        commandLine.append(wpaConfig[0]);
        commandLine.append(wpaConfig[1]);
    }
    QProcess command;
    command.start(commandLine.join(QString(" ")));
    command.waitForFinished(-1);
    if (command.exitCode() != 0)
        return false;
    else
        return true;
}


bool MainWindow::checkState(QString state, QString item)
{
    if (item.indexOf(state) > -1)
        return true;
    else
        return false;
}


// window signals
void MainWindow::createActions()
{
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabs(int)));
    connect(ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    // main page events
    connect(ui->pushButton_mainRefresh, SIGNAL(clicked(bool)), this, SLOT(updateMainTab()));
    connect(ui->pushButton_mainEnable, SIGNAL(clicked(bool)), this, SLOT(mainTabEnableProfile()));
    connect(ui->pushButton_mainRestart, SIGNAL(clicked(bool)), this, SLOT(mainTabRestartProfile()));
    connect(ui->pushButton_mainStart, SIGNAL(clicked(bool)), this, SLOT(mainTabStartProfile()));
    connect(ui->tableWidget_main, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(mainTabStartProfile()));
    connect(ui->tableWidget_main, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(mainTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));

    // profile tab events
    connect(ui->lineEdit_profile, SIGNAL(returnPressed()), this, SLOT(profileTabLoadProfile()));
    connect(ui->pushButton_profile, SIGNAL(clicked(bool)), this, SLOT(profileTabBrowseProfile()));
    connect(ui->pushButton_profileClear, SIGNAL(clicked(bool)), this, SLOT(profileTabClear()));
    connect(ui->pushButton_profileSave, SIGNAL(clicked(bool)), this, SLOT(profileTabCreateProfile()));
    connect(ui->pushButton_profileLoad, SIGNAL(clicked(bool)), this, SLOT(profileTabLoadProfile()));
    connect(generalWid->connectionType, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileTabChangeState(QString)));

    // wifi page events
    connect(ui->pushButton_wifiRefresh, SIGNAL(clicked(bool)), this, SLOT(updateWifiTab()));
    connect(ui->pushButton_wifiStart, SIGNAL(clicked(bool)), this, SLOT(wifiTabStart()));
    connect(ui->tableWidget_wifi, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(wifiTabStart()));
    connect(ui->tableWidget_wifi, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(wifiTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
}


// window slots
void MainWindow::updateTabs(const int tab)
{
    if (tab == 0)
        updateMainTab();
    else if (tab == 1)
        updateProfileTab();
    else if (tab == 2)
        updateWifiTab();
}


void MainWindow::updateMainTab()
{
    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, 1);
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
    ui->tableWidget_main->setHorizontalHeaderLabels(QString("Name Description Status").split(QString(" ")));
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
    ui->tableWidget_main->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateProfileTab()
{
    ui->tabWidget->setDisabled(true);
    profileTabClear();
    ui->tabWidget->setEnabled(true);
}


void MainWindow::updateWifiTab()
{
    wifiTabSetEnabled(checkExternalApps(QString("wpasup")));
    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, 1);
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
    ui->tableWidget_wifi->setHorizontalHeaderLabels(QString("Name Status Signal Security").split(QString(" ")));
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
    ui->tableWidget_wifi->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


// main tab slots
void MainWindow::mainTabEnableProfile()
{
    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->enableProfile(profile);
    if (ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().indexOf(QString("enabled")) > -1) {
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
    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, 1);
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
    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->startProfile(profile);
    if (ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().indexOf(QString("inactive")) == -1) {
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
    if (!checkExternalApps(QString("netctl"))) {
        errorWin = new ErrorWindow(this, 1);
        errorWin->show();
        return;
    }
    if (current == 0) {
        ui->pushButton_mainEnable->setDisabled(true);
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setDisabled(true);
        return;
    }

    ui->pushButton_mainEnable->setEnabled(true);
    ui->pushButton_mainStart->setEnabled(true);

    QString item = ui->tableWidget_main->item(current->row(), 2)->text();
    if (!checkState(QString("inactive"), item)) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
    }
    if (checkState(QString("enabled"), item))
        ui->pushButton_mainEnable->setText(QApplication::translate("MainWindow", "Disable"));
    else
        ui->pushButton_mainEnable->setText(QApplication::translate("MainWindow", "Enable"));
}


// profile tab slots
void MainWindow::profileTabBrowseProfile()
{
    QString filename = QFileDialog::getSaveFileName(
                this,
                QApplication::translate("MainWindow", "Save profile as..."),
                QString("/etc/netctl/"),
                QApplication::translate("MainWindow", "Profile (*)"));
    if (!filename.isEmpty())
        ui->lineEdit_profile->setText(filename);
}


void MainWindow::profileTabChangeState(QString current)
{
    if (current == QString("ethernet")) {
        generalWid->setShown(true);
        ipWid->setShown(true);
        bridgeWid->setShown(false);
        ethernetWid->setShown(true);
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
        mobileWid->setShown(false);
        pppoeWid->setShown(false);
        tunnelWid->setShown(false);
        tuntapWid->setShown(false);
        vlanWid->setShown(true);
        wirelessWid->setShown(false);
    }
}


void MainWindow::profileTabClear()
{
    ui->lineEdit_profile->clear();

    generalWid->clear();
    ipWid->clear();
    bridgeWid->clear();
    ethernetWid->clear();
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
    // error checking
    if (ui->lineEdit_profile->text().isEmpty()) {
        errorWin = new ErrorWindow(this, 3);
        errorWin->show();
        return;
    }
    if (generalWid->isOk() == 1) {
        errorWin = new ErrorWindow(this, 4);
        errorWin->show();
        return;
    }
    else if (generalWid->isOk() == 2) {
        errorWin = new ErrorWindow(this, 5);
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
            (generalWid->connectionType->currentText() == QString("vlan"))) {
        if (ipWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 6);
            errorWin->show();
            return;
        }
        else if (ipWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, 6);
            errorWin->show();
            return;
        }
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 7);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 8);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, 9);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 3) {
            errorWin = new ErrorWindow(this, 10);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 4) {
            errorWin = new ErrorWindow(this, 7);
            errorWin->show();
            return;
        }
        else if (wirelessWid->isOk() == 5) {
            errorWin = new ErrorWindow(this, 11);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 7);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, 12);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 3) {
            errorWin = new ErrorWindow(this, 13);
            errorWin->show();
            return;
        }
        else if (pppoeWid->isOk() == 4) {
            errorWin = new ErrorWindow(this, 12);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 14);
            errorWin->show();
            return;
        }
        if (mobileWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, 7);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 15);
            errorWin->show();
            return;
        }
        if (tuntapWid->isOk() == 2) {
            errorWin = new ErrorWindow(this, 15);
            errorWin->show();
            return;
        }
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1) {
            errorWin = new ErrorWindow(this, 7);
            errorWin->show();
            return;
        }
    }

    ui->tabWidget->setDisabled(true);
    // read settings
    QString profile = ui->lineEdit_profile->text();
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
    QString profile = ui->lineEdit_profile->text();
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
}


// wifi tab slots
void MainWindow::wifiTabSetEnabled(bool state)
{
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


void MainWindow::connectToUnknownEssid(QString passwd)
{
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
    QString profileTempName = netctlProfile->createProfile(profile, settings);
    netctlProfile->copyProfile(profileTempName);
    netctlCommand->startProfile(profile);
    if (netctlCommand->isProfileActive(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    updateWifiTab();
}


void MainWindow::wifiTabStart()
{
    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, 1);
        errorWin->show();
        return;
    }
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() == QString("<hidden>")) {
        ui->pushButton_wifiStart->setDisabled(true);
        errorWin = new ErrorWindow(this, 2);
        errorWin->show();
        return;
    }

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text();
    QString item = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
    if (checkState(QString("exists"), item)) {
        QString profileName = wpaCommand->existentProfile(profile);
        netctlCommand->startProfile(profileName);
        if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text().indexOf(QString("inactive")) == -1) {
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
    if (!checkExternalApps(QString("wpasup"))) {
        errorWin = new ErrorWindow(this, 1);
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
        if (!checkState(QString("inactive"), item))
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Stop"));
        else
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
    }
    else
        ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
}
