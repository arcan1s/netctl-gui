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

#include "netctlautowindow.h"
#include "ui_netctlautowindow.h"

#include <QDebug>

#include <netctlgui/netctlgui.h>


NetctlAutoWindow::NetctlAutoWindow(QWidget *parent, const bool debugCmd, const QMap<QString, QString> settings)
    : QMainWindow(parent),
      ui(new Ui::NetctlAutoWindow),
      debug(debugCmd)
{
    ui->setupUi(this);
    netctlCommand = new Netctl(debug, settings);

    createActions();
    ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Ready"));
}


NetctlAutoWindow::~NetctlAutoWindow()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[~NetctlAutoWindow]";

    delete ui;
    delete netctlCommand;
}


void NetctlAutoWindow::createActions()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[createActions]";

    // menu actions
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionDisableAll, SIGNAL(triggered(bool)), this, SLOT(netctlAutoDisableAllProfiles()));
    connect(ui->actionEnable, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableProfile()));
    connect(ui->actionEnableAll, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableAllProfiles()));\
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), this, SLOT(netctlAutoUpdateTable()));
    connect(ui->actionSwitch, SIGNAL(triggered(bool)), this, SLOT(netctlAutoStartProfile()));
    // service
    connect(ui->actionEnableService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableService()));
    connect(ui->actionRestartService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoRestartService()));
    connect(ui->actionStartService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoStartService()));

    // table actions
    connect(ui->tableWidget, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(netctlAutoStartProfile()));
    connect(ui->tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(netctlAutoRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(netctlAutoContextualMenu(QPoint)));

    // buttons
    connect(ui->pushButton_enable, SIGNAL(clicked(bool)), this, SLOT(netctlAutoEnableProfile()));
    connect(ui->pushButton_refresh, SIGNAL(clicked(bool)), this, SLOT(netctlAutoUpdateTable()));
    connect(ui->pushButton_switch, SIGNAL(clicked(bool)), this, SLOT(netctlAutoStartProfile()));
}


void NetctlAutoWindow::showWindow()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[showWindow]";

    netctlAutoUpdateTable();

    show();
}


void NetctlAutoWindow::netctlAutoUpdateTable()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoUpdateTable]";

    ui->tableWidget->setDisabled(true);
    QList<QStringList> profiles = netctlCommand->getProfileListFromNetctlAuto();

    // actions
    if (netctlCommand->isNetctlAutoEnabled())
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Disable service"));
    else
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Enable service"));
    ui->actionEnableService->setVisible(true);
    if (netctlCommand->isNetctlAutoRunning()) {
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is running"));
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Stop service"));
        ui->actionRestartService->setVisible(true);
    }
    else {
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Start service"));
        ui->actionRestartService->setVisible(false);
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is not running"));
        netctlAutoRefreshButtons(0, 0);
        return;
    }

    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->selectRow(-1);
    ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(profiles.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("NetctlAutoWindow", "Name"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Description"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Active"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Disabled"));
    ui->tableWidget->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<profiles.count(); i++) {
        // name
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(profiles[i][0]));
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (profiles[i][2] == QString("*")) {
            QFont font;
            font.setBold(true);
            ui->tableWidget->item(i, 0)->setFont(font);
        }
        else if (profiles[i][2] == QString("!")) {
            QFont font;
            font.setItalic(true);
            ui->tableWidget->item(i, 0)->setFont(font);
        }
        // description
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(profiles[i][1]));
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (profiles[i][2] == QString("*")) {
            // active
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QApplication::translate("NetctlAutoWindow", "yes")));
            ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString("")));
        if (profiles[i][2] == QString("!")) {
            // disabled
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QApplication::translate("NetctlAutoWindow", "yes")));
            ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString("")));
    }

    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#endif
    ui->tableWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Updated"));

    netctlAutoRefreshButtons(0, 0);
    update();
}


void NetctlAutoWindow::netctlAutoContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoContextualMenu]";

    if (ui->tableWidget->currentItem() == 0)
        return;
    // create menu
    QMenu menu(this);
    QAction *startProfile = menu.addAction(QApplication::translate("NetctlAutoWindow", "Switch to profile"));
    startProfile->setIcon(QIcon::fromTheme("dialog-apply"));
    QAction *enableProfile = menu.addAction(QApplication::translate("NetctlAutoWindow", "Enable profile"));
    menu.addSeparator();
    QAction *enableAllProfiles = menu.addAction(QApplication::translate("NetctlAutoWindow", "Enable all profiles"));
    enableAllProfiles->setIcon(QIcon::fromTheme("edit-add"));
    QAction *disableAllProfiles = menu.addAction(QApplication::translate("NetctlAutoWindow", "Disable all profiles"));
    disableAllProfiles->setIcon(QIcon::fromTheme("edit-delete"));

    // set text
    if (ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 2)->text() == QString("yes")) {
        enableProfile->setVisible(false);
        startProfile->setVisible(false);
    }
    else {
        enableProfile->setVisible(true);
        startProfile->setVisible(true);
        if (ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 3)->text() == QString("yes")) {
            enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
            enableProfile->setIcon(QIcon::fromTheme("edit-add"));
        }
        else {
            enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
            enableProfile->setIcon(QIcon::fromTheme("edit-delete"));
        }
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget->viewport()->mapToGlobal(pos));
    if (action == startProfile) {
        if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoContextualMenu]" << "Switch to profile";
        netctlAutoStartProfile();
    }
    else if (action == enableProfile) {
        if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoContextualMenu]" << "Enable profile";
        netctlAutoEnableProfile();
    }
    else if (action == enableAllProfiles) {
        if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoContextualMenu]" << "Enable all profiles";
        netctlAutoEnableAllProfiles();
    }
    else if (action == disableAllProfiles) {
        if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoContextualMenu]" << "Disable all profiles";
        netctlAutoDisableAllProfiles();
    }
}


void NetctlAutoWindow::netctlAutoDisableAllProfiles()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoDisableAllProfiles]";

    ui->tableWidget->setDisabled(true);
    if (netctlCommand->autoDisableAllProfiles())
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableProfile()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoEnableProfile]";

    if (ui->tableWidget->currentItem() == 0)
        return;
    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    if (netctlCommand->autoEnableProfile(profile))
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableAllProfiles()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoEnableAllProfiles]";

    ui->tableWidget->setDisabled(true);
    if (netctlCommand->autoEnableAllProfiles())
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartProfile()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoStartProfile]";

    if (ui->tableWidget->currentItem() == 0)
        return;
    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    if (netctlCommand->autoStartProfile(profile))
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableService()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoEnableService]";

    if (netctlCommand->autoEnableService())
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoRestartService()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoRestartService]";

    if (netctlCommand->autoRestartService())
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartService()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoStartService]";

    if (netctlCommand->autoStartService())
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[netctlAutoRefreshButtons]";

    if (current == 0) {
        // buttons
        ui->pushButton_enable->setDisabled(true);
        ui->pushButton_switch->setDisabled(true);
        // menu
        ui->actionEnable->setVisible(false);
        ui->actionSwitch->setVisible(false);
        return;
    }
    if (ui->tableWidget->item(current->row(), 2)->text() == QString("yes")) {
        // buttons
        ui->pushButton_enable->setDisabled(true);
        ui->pushButton_switch->setDisabled(true);
        // menu
        ui->actionEnable->setVisible(false);
        ui->actionSwitch->setVisible(false);
    }
    else {
        // buttons
        ui->pushButton_enable->setEnabled(true);
        ui->pushButton_switch->setEnabled(true);
        // menu
        ui->actionEnable->setVisible(true);
        ui->actionSwitch->setVisible(true);
        if (ui->tableWidget->item(current->row(), 3)->text() == QString("yes")) {
            // buttons
            ui->pushButton_enable->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
            ui->pushButton_enable->setIcon(QIcon::fromTheme("edit-add"));
            // menu
            ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Enable profile"));
            ui->actionEnable->setIcon(QIcon::fromTheme("edit-add"));
        }
        else {
            // buttons
            ui->pushButton_enable->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
            ui->pushButton_enable->setIcon(QIcon::fromTheme("edit-delete"));
            // menu
            ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Disable profile"));
            ui->actionEnable->setIcon(QIcon::fromTheme("edit-delete"));
        }
    }
}
