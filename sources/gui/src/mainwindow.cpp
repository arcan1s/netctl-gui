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

#include "netctlinteract.h"
#include "wpasupinteract.h"
#include <cstdio>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget_main->setSortingEnabled(true);
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
    preferedInterface  = QString("wifi0");
    // additional settings
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui.pid"));
    wpaConfig.append(QString("nl80211,wext"));
    wpaConfig.append(QString("/run/wpa_supplicant_netctl-gui"));

    netctlCommand = new Netctl(this, netctlPath, profileDir, sudoPath);
    wpaCommand = new WpaSup(this, wpaConfig, sudoPath, ifaceDir, preferedInterface);

    createActions();
    updateMainTab();
}

MainWindow::~MainWindow()
{
    delete netctlCommand;
    delete wpaCommand;
    delete ui;
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
    QStringList profiles = netctlCommand->getProfileList();
    QStringList descriptions = netctlCommand->getProfileDescriptions(profiles);
    QStringList statuses = netctlCommand->getProfileStatuses(profiles);


    ui->tableWidget_main->setRowCount(profiles.count());
    ui->tableWidget_main->sortByColumn(0, Qt::AscendingOrder);

    for (int i=0; i<profiles.count(); i++) {
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(profiles[i]));
        ui->tableWidget_main->setItem(i, 1, new QTableWidgetItem(descriptions[i]));
        ui->tableWidget_main->setItem(i, 2, new QTableWidgetItem(statuses[i]));
    }

    ui->tableWidget_main->resizeColumnsToContents();
    ui->tableWidget_main->resizeRowsToContents();
    ui->tableWidget_main->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_main->setCurrentCell(0, 0);
    update();
}


void MainWindow::updateWifiTab()
{
    QList<QStringList> scanResults = wpaCommand->scanWifi();
    for (int i=0; i<scanResults.count(); i++)
        printf("%s\n", scanResults[i][0].toUtf8().data());
}


// main tab slots
void MainWindow::mainTabEnableProfile()
{
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

    QString profile = ui->tableWidget_main->item(current->row(), 0)->text();
    bool isActive = netctlCommand->isProfileActive(profile);
    bool isEnable = netctlCommand->isProfileEnabled(profile);

    if (isActive) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
    }
    if (isEnable)
        ui->pushButton_mainEnable->setText(QApplication::translate("MainWindow", "Disable"));
    else
        ui->pushButton_mainEnable->setText(QApplication::translate("MainWindow", "Enable"));
}
