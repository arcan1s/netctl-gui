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

#include <QMimeData>
#include <QProcess>

#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "netctlinteract.h"
#include "netctlprofile.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "wpasupinteract.h"
#include "wirelesswidget.h"
#include <cstdio>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Ready"));

    //  SettingsWindow *settingsWindow;
    //  settingsWindow = new SettingsWindow(this);
    //  delete settingsWindow;

    // temporary block
    netctlPath = QString("/usr/bin/netctl");
    profileDir = QString("/etc/netctl");
    sudoPath = QString("/usr/bin/kdesu");
    wpaConfig.append(QString("/usr/bin/wpa_cli"));
    wpaConfig.append(QString("/usr/bin/wpa_supplicant"));
    ifaceDir = QString("/sys/class/net/");
    preferedInterface  = QString("");
    //// additional settings
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui.pid"));
    wpaConfig.append(QString("nl80211,wext"));
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui"));
    wpaConfig.append(QString("users"));

    // gui
    generalWid = new GeneralWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(generalWid);
    ipWid = new IpWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ipWid);
    ethernetWid = new EthernetWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ethernetWid);
    wirelessWid = new WirelessWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(wirelessWid);
    pppoeWid = new PppoeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(pppoeWid);
    // backend
    netctlCommand = new Netctl(this, netctlPath, profileDir, sudoPath);
    netctlProfile = new NetctlProfile(this, profileDir, sudoPath);
    wpaCommand = new WpaSup(this, wpaConfig, sudoPath, ifaceDir, preferedInterface);

    createActions();
    updateTabs(ui->tabWidget->currentIndex());
}


MainWindow::~MainWindow()
{
    delete ethernetWid;
    delete generalWid;
    delete ipWid;
    delete netctlCommand;
    delete netctlProfile;
    delete pppoeWid;
    delete ui;
    delete wpaCommand;
    delete wirelessWid;
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
    QStringList profileInfo;
    profileInfo.append(QString("Automatically generated profile by Netctl GUI"));
    profileInfo.append(wpaCommand->getInterfaceList()[0]);
    profileInfo.append(QString("wireless"));
    QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text();
    if (checkState(QString("WPA"), security))
        profileInfo.append(QString("wpa"));
    else if (checkState(QString("wep"), security))
        profileInfo.append(QString("wep"));
    else
        profileInfo.append(QString("none"));
    profileInfo.append(ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text());
    profileInfo.append(passwd);
    profileInfo.append(QString("dhcp"));



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
