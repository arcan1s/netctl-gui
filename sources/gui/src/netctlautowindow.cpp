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

#include "calls.h"
#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "mainwindow.h"


NetctlAutoWindow::NetctlAutoWindow(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QMainWindow(parent),
      debug(debugCmd),
      configuration(settings)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (configuration[QString("USE_HELPER")] == QString("true"));

    // ui
    ui = new Ui::NetctlAutoWindow;
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


void NetctlAutoWindow::showMessage(const bool status)
{
    if (debug) qDebug() << PDEBUG;

    if (status)
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("NetctlAutoWindow", "Error"));
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
        enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
        enableProfile->setIcon(QIcon::fromTheme("list-add"));
    } else {
        enableProfile->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
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
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    ui->tableWidget->setDisabled(true);
    netctlInformation info = generalInformation(mainWindow->netctlInterface,
                                                useHelper, debug);

    // actions
    ui->actionDisableAll->setEnabled(info.netctlAuto);
    ui->actionEnableAll->setEnabled(info.netctlAuto);
    ui->actionRestartService->setEnabled(info.netctlAuto);
    if (info.netctlAutoEnabled)
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Disable service"));
    else
        ui->actionEnableService->setText(QApplication::translate("NetctlAutoWindow", "Enable service"));
    if (info.netctlAuto) {
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is running"));
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Stop service"));
    } else {
        ui->label_info->setText(QApplication::translate("NetctlAutoWindow", "netctl-auto is not running"));
        ui->actionStartService->setText(QApplication::translate("NetctlAutoWindow", "Start service"));
        netctlAutoRefreshButtons(nullptr, nullptr);
        return;
    }

    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->selectRow(-1);
    ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(info.netctlAutoProfiles.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("NetctlAutoWindow", "Name"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Description"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Active"));
    headerList.append(QApplication::translate("NetctlAutoWindow", "Enabled"));
    ui->tableWidget->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<info.netctlAutoProfiles.count(); i++) {
        // font
        QFont font;
        font.setBold(info.netctlAutoProfiles[i].active);
        font.setItalic(info.netctlAutoProfiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("NetctlAutoWindow", "Profile"))
                                      .arg(info.netctlAutoProfiles[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("NetctlAutoWindow", "Active"))
                                      .arg(checkStatus(info.netctlAutoProfiles[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("NetctlAutoWindow", "Disabled"))
                                     .arg(checkStatus(!info.netctlAutoProfiles[i].enabled));
        // name
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(info.netctlAutoProfiles[i].name));
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tableWidget->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget->item(i, 0)->setFont(font);
        // description
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(info.netctlAutoProfiles[i].description));
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignJustify | Qt::AlignVCenter);
        ui->tableWidget->item(i, 1)->setToolTip(toolTip);
        // active
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(checkStatus(info.netctlAutoProfiles[i].active, true)));
        ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // enabled
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(checkStatus(info.netctlAutoProfiles[i].enabled, true)));
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
    ui->tableWidget->setDisabled(false);
    showMessage(true);

    netctlAutoRefreshButtons(nullptr, nullptr);
    update();
}


void NetctlAutoWindow::netctlAutoDisableAllProfiles()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    ui->tableWidget->setDisabled(true);
    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoDisableAllProfiles();
    else try {
        responce = sendRequestToCtrl(QString("autoDisableAll"), debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget->currentItem() == nullptr) return;

    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoEnableProfile(profile);
    else try {
        QList<QVariant> args;
        args.append(profile);
        responce = sendRequestToCtrlWithArgs(QString("autoEnable"), args, debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableAllProfiles()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    ui->tableWidget->setDisabled(true);
    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoEnableAllProfiles();
    else try {
        responce = sendRequestToCtrl(QString("autoEnableAll"), debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget->currentItem() == nullptr) return;

    ui->tableWidget->setDisabled(true);
    QString profile = ui->tableWidget->item(ui->tableWidget->currentItem()->row(), 0)->text();
    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoStartProfile(profile);
    else try {
        QList<QVariant> args;
        args.append(profile);
        responce = sendRequestToCtrlWithArgs(QString("autoStart"), args, debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoEnableService()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoEnableService();
    else try {
        responce = sendRequestToCtrl(QString("autoServiceEnable"), debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoRestartService()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("systemctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("systemctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoRestartService();
    else try {
        responce = sendRequestToCtrl(QString("autoServiceRestart"), debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

    netctlAutoUpdateTable();
}


void NetctlAutoWindow::netctlAutoStartService()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("systemctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("systemctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    bool responce = false;
    if (!useHelper)
        responce = mainWindow->netctlCommand->autoStartService();
    else try {
        responce = sendRequestToCtrl(QString("autoServiceStart"), debug)[0].toBool();
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
    }
    showMessage(responce);

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
        ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Disable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("edit-delete"));
    } else {
        ui->actionEnable->setText(QApplication::translate("NetctlAutoWindow", "Enable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("list-add"));
    }
}
