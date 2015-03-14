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

#include <pdebug/pdebug.h>

#include "dbusoperation.h"
#include "mainwindow.h"


NetctlAutoWindow::NetctlAutoWindow(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QMainWindow(parent),
      ui(new Ui::NetctlAutoWindow),
      debug(debugCmd)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (settings[QString("USE_HELPER")] == QString("true"));
    ui->setupUi(this);
    ui->tableWidget->setColumnHidden(2, true);
    ui->tableWidget->setColumnHidden(3, true);
    updateToolBarState(static_cast<Qt::ToolBarArea>(settings[QString("NETCTLAUTO_TOOLBAR")].toInt()));

    createActions();
    ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Ready"));
}


NetctlAutoWindow::~NetctlAutoWindow()
{
    if (debug) qDebug() << PDEBUG;

    delete ui;
}


Qt::ToolBarArea NetctlAutoWindow::getToolBarArea()
{
    if (debug) qDebug() << PDEBUG;

    return toolBarArea(ui->toolBar);
}


QString NetctlAutoWindow::checkStatus(const bool statusBool, const bool nullFalse)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Status" << statusBool;
    if (debug) qDebug() << PDEBUG << ":" << "Return null false" << nullFalse;

    if (statusBool) return QApplication::translate("NetctlAutoWindow", "yes");
    if (!nullFalse) return QApplication::translate("NetctlAutoWindow", "no");

    return QString("");
}


void NetctlAutoWindow::createActions()
{
    if (debug) qDebug() << PDEBUG;

    // menu actions
    connect(ui->actionClose, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(ui->actionDisableAll, SIGNAL(triggered(bool)), this, SLOT(netctlAutoDisableAllProfiles()));
    connect(ui->actionEnable, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableProfile()));
    connect(ui->actionEnableAll, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableAllProfiles()));
    connect(ui->actionSwitch, SIGNAL(triggered(bool)), this, SLOT(netctlAutoStartProfile()));
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), this, SLOT(netctlAutoUpdateTable()));
    // service
    connect(ui->actionEnableService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoEnableService()));
    connect(ui->actionRestartService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoRestartService()));
    connect(ui->actionStartService, SIGNAL(triggered(bool)), this, SLOT(netctlAutoStartService()));

    // table actions
    connect(ui->tableWidget, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(netctlAutoStartProfile()));
    connect(ui->tableWidget, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(netctlAutoRefreshButtons(QTableWidgetItem *, QTableWidgetItem *)));
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(netctlAutoContextualMenu(QPoint)));
}


void NetctlAutoWindow::showWindow()
{
    if (debug) qDebug() << PDEBUG;

    netctlAutoUpdateTable();

    show();
}


void NetctlAutoWindow::updateToolBarState(const Qt::ToolBarArea area)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Toolbar area" << area;

    removeToolBar(ui->toolBar);
    if (area != Qt::NoToolBarArea) {
        addToolBar(area, ui->toolBar);
        ui->toolBar->show();
    }
}


void NetctlAutoWindow::netctlAutoContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << PDEBUG;

    if (ui->tableWidget->currentItem() == nullptr) return;
    // create menu
    QMenu menu(this);
    QAction *startProfile = menu.addAction(QApplication::translate("NetctlAutoWindow", "Switch to profile"));
    startProfile->setIcon(QIcon::fromTheme("system-run"));
    QAction *enableProfile = menu.addAction(QApplication::translate("NetctlAutoWindow", "Enable profile"));
    menu.addSeparator();
    QAction *enableAllProfiles = menu.addAction(QApplication::translate("NetctlAutoWindow", "Enable all profiles"));
    enableAllProfiles->setIcon(QIcon::fromTheme("list-add"));
    QAction *disableAllProfiles = menu.addAction(QApplication::translate("NetctlAutoWindow", "Disable all profiles"));
    disableAllProfiles->setIcon(QIcon::fromTheme("edit-delete"));

    // set text
    startProfile->setVisible(ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 2)->text().isEmpty());
    if (!ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 3)->text().isEmpty()) {
        enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
        enableProfile->setIcon(QIcon::fromTheme("list-add"));
    } else {
        enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
        enableProfile->setIcon(QIcon::fromTheme("edit-delete"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget->viewport()->mapToGlobal(pos));
    if (action == startProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Switch to profile";
        netctlAutoStartProfile();
    } else if (action == enableProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Enable profile";
        netctlAutoEnableProfile();
    } else if (action == enableAllProfiles) {
        if (debug) qDebug() << PDEBUG << ":" << "Enable all profiles";
        netctlAutoEnableAllProfiles();
    } else if (action == disableAllProfiles) {
        if (debug) qDebug() << PDEBUG << ":" << "Disable all profiles";
        netctlAutoDisableAllProfiles();
    }
}


void NetctlAutoWindow::netctlAutoUpdateTable()
{
    if (debug) qDebug() << PDEBUG;

    ui->tableWidget->setDisabled(true);
    // actions
    bool enabled = false;
    bool running = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToLib(QString("isNetctlAutoActive"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoUpdateTable();
        }
        enabled = responce[0].toBool();
        responce = sendRequestToLib(QString("isNetctlAutoActive"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoUpdateTable();
        }
        running = responce[0].toBool();
    } else {
        enabled = mainWindow->netctlCommand->isNetctlAutoEnabled();
        running = mainWindow->netctlCommand->isNetctlAutoRunning();
    }
    ui->actionDisableAll->setEnabled(running);
    ui->actionEnableAll->setEnabled(running);
    ui->actionRestartService->setEnabled(running);
    if (enabled)
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Disable service"));
    else
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Enable service"));
    if (running) {
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is running"));
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Stop service"));
    } else {
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is not running"));
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Start service"));
        netctlAutoRefreshButtons(nullptr, nullptr);
        return;
    }
    QList<netctlProfileInfo> profiles;
    if (useHelper)
        profiles = parseOutputNetctl(sendRequestToLib(QString("VerboseProfileList"), debug));
    else
        profiles = mainWindow->netctlCommand->getProfileListFromNetctlAuto();

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
        // font
        QFont font;
        font.setBold(profiles[i].active);
        font.setItalic(profiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("NetctlAutoWindow", "Profile")).arg(profiles[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("NetctlAutoWindow", "Active")).arg(checkStatus(profiles[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("NetctlAutoWindow", "Disabled")).arg(checkStatus(!profiles[i].enabled));
        // name
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(profiles[i].name));
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tableWidget->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget->item(i, 0)->setFont(font);
        // description
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(profiles[i].description));
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignJustify | Qt::AlignVCenter);
        ui->tableWidget->item(i, 1)->setToolTip(toolTip);
        // active
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(checkStatus(profiles[i].active, true)));
        ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // enabled
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(checkStatus(!profiles[i].enabled, true)));
        ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget->setSortingEnabled(true);

    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->tableWidget->setCurrentCell(-1, -1);
    ui->tableWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Updated"));

    netctlAutoRefreshButtons(nullptr, nullptr);
    update();
}


void NetctlAutoWindow::netctlAutoDisableAllProfiles()
{
    if (debug) qDebug() << PDEBUG;

    ui->tableWidget->setDisabled(true);
    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("autoDisableAll"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoDisableAllProfiles();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoDisableAllProfiles();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableProfile()
{
    if (debug) qDebug() << PDEBUG;

    if (ui->tableWidget->currentItem() == nullptr) return;
    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    bool status = false;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        QList<QVariant> responce = sendRequestToCtrlWithArgs(QString("autoEnable"), args, debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoEnableProfile();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoEnableProfile(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableAllProfiles()
{
    if (debug) qDebug() << PDEBUG;

    ui->tableWidget->setDisabled(true);
    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("autoEnableAll"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoEnableAllProfiles();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoEnableAllProfiles();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartProfile()
{
    if (debug) qDebug() << PDEBUG;

    if (ui->tableWidget->currentItem() == nullptr) return;
    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    bool status = false;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        QList<QVariant> responce = sendRequestToCtrlWithArgs(QString("autoStart"), args, debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoStartProfile();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoStartProfile(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableService()
{
    if (debug) qDebug() << PDEBUG;

    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("autoServiceEnable"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoEnableService();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoEnableService();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoRestartService()
{
    if (debug) qDebug() << PDEBUG;

    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("autoServiceRestart"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoRestartService();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoRestartService();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartService()
{
    if (debug) qDebug() << PDEBUG;

    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("autoServiceStart"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return netctlAutoStartService();
        }
        status = responce[0].toBool();
    } else
        status = mainWindow->netctlCommand->autoStartService();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << PDEBUG;

    bool selected = (current != nullptr);
    ui->actionEnable->setEnabled(selected);
    ui->actionSwitch->setEnabled(selected && ui->tableWidget->item(current->row(), 2)->text().isEmpty());
    if (selected && !ui->tableWidget->item(current->row(), 3)->text().isEmpty()) {
        ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("list-add"));
    } else {
        ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("edit-delete"));
    }
}
