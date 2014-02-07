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

#include "netctlinteract.h"
#include "wpasupinteract.h"
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
    sudoPath = QString("/usr/bin/sudo");
    wpaConfig.append(QString("/usr/bin/wpa_cli"));
    wpaConfig.append(QString("/usr/bin/wpa_supplicant"));
    ifaceDir = QString("/sys/class/net/");
    preferedInterface  = QString("");
    // additional settings
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui.pid"));
    wpaConfig.append(QString("nl80211,wext"));
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui"));
    wpaConfig.append(QString("users"));

    netctlCommand = new Netctl(this, netctlPath, profileDir, sudoPath);
    wpaCommand = new WpaSup(this, wpaConfig, sudoPath, ifaceDir, preferedInterface);

    createActions();
    updateTabs(ui->tabWidget->currentIndex());
}


MainWindow::~MainWindow()
{
    delete netctlCommand;
    delete wpaCommand;
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
    connect(ui->tableWidget_main, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(mainTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));

    // wifi page events
    connect(ui->pushButton_wifiRefresh, SIGNAL(clicked(bool)), this, SLOT(updateWifiTab()));
    connect(ui->tableWidget_wifi, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(wifiTabRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
}


// window slots
void MainWindow::updateTabs(const int tab)
{
    if (tab == 0)
        updateMainTab();
    else if (tab == 2)
        updateWifiTab();
}


void MainWindow::updateMainTab()
{
    if (!checkExternalApps(QString("netctl")))
        return;

    ui->tableWidget_main->setDisabled(true);
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
    ui->tableWidget_main->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateWifiTab()
{
    wifiTabSetEnabled(checkExternalApps(QString("wpasup")));
    if (!checkExternalApps(QString("wpasup")))
        return;

    QList<QStringList> scanResults = wpaCommand->scanWifi();

    ui->tableWidget_wifi->setDisabled(true);
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
    ui->tableWidget_wifi->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


// main tab slots
void MainWindow::mainTabEnableProfile()
{
    if (!checkExternalApps(QString("netctl")))
        return;

    ui->tableWidget_main->setDisabled(true);
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
    ui->tableWidget_main->setEnabled(true);
    updateMainTab();
}


void MainWindow::mainTabRestartProfile()
{
    if (!checkExternalApps(QString("netctl")))
        return;

    ui->tableWidget_main->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    netctlCommand->restartProfile(profile);
    if (netctlCommand->isProfileActive(profile))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    ui->tableWidget_main->setEnabled(true);
    updateMainTab();
}


void MainWindow::mainTabStartProfile()
{
    if (!checkExternalApps(QString("netctl")))
        return;

    ui->tableWidget_main->setDisabled(true);
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
    ui->tableWidget_main->setEnabled(true);
    updateMainTab();
}


void MainWindow::mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (current == 0)
        return;
    if (!checkExternalApps(QString("netctl")))
        return;

    QString profile = ui->tableWidget_main->item(current->row(), 0)->text();
    if (netctlCommand->isProfileActive(profile)) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
    }
    if (netctlCommand->isProfileEnabled(profile))
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
        ui->pushButton_wifiStart->setEnabled(true);
        ui->label_wifi->hide();
    }
    else {
        ui->tableWidget_wifi->hide();
        ui->pushButton_wifiRefresh->setDisabled(true);
        ui->pushButton_wifiStart->setDisabled(true);
        ui->label_wifi->show();
    }
}


void MainWindow::wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (current == 0)
        return;
    if (!checkExternalApps(QString("wpasup")))
        return;

    QString network = ui->tableWidget_wifi->item(current->row(), 0)->text();
    if (wpaCommand->isProfileExists(network)) {
        if (wpaCommand->isProfileActive(network))
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Stop"));
        else
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
    }
}
