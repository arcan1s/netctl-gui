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

#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QDebug>
#include <QMenu>

#include <pdebug/pdebug.h>

#include "calls.h"
#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "mainwindow.h"


MainWidget::MainWidget(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QMainWindow(parent),
      debug(debugCmd),
      configuration(settings)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (configuration[QString("USE_HELPER")] == QString("true"));

    // windows
    ui = new Ui::MainWidget;
    ui->setupUi(this);
    ui->tableWidget_main->setColumnHidden(2, true);
    ui->tableWidget_main->setColumnHidden(3, true);
    updateToolBarState(static_cast<Qt::ToolBarArea>(configuration[QString("NETCTL_TOOLBAR")].toInt()));

    // append toolbar
    QMenu *actionMenu = new QMenu(this);
    actionMenu->addAction(ui->actionSwitch);
    actionMenu->addAction(ui->actionRestart);
    actionMenu->addAction(ui->actionEnable);
    ui->actionStart->setMenu(actionMenu);

    createActions();
}


MainWidget::~MainWidget()
{
    if (debug) qDebug() << PDEBUG;

    if (ui != nullptr) delete ui;
}


Qt::ToolBarArea MainWidget::getToolBarArea()
{
    if (debug) qDebug() << PDEBUG;

    return toolBarArea(ui->toolBar);
}


bool MainWidget::mainTabSelectProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    for (int i=0; i<ui->tableWidget_main->rowCount(); i++) {
        if (ui->tableWidget_main->item(i, 0)->text() != profile) continue;
        ui->tableWidget_main->setCurrentCell(i, 0);
    }

    return (ui->tableWidget_main->currentItem() != nullptr);
}


void MainWidget::update()
{
    if (debug) qDebug() << PDEBUG;

    updateMainTab();
    updateMenuMain();
}


void MainWidget::updateToolBarState(const Qt::ToolBarArea area)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Toolbar area" << area;

    removeToolBar(ui->toolBar);
    if (area != Qt::NoToolBarArea) {
        addToolBar(area, ui->toolBar);
        ui->toolBar->show();
    }
}


void MainWidget::updateMenuMain()
{
    if (debug) qDebug() << PDEBUG;

    bool selected = (ui->tableWidget_main->currentItem() != nullptr);
    ui->actionStart->setEnabled(selected);
    ui->actionEnable->setEnabled(selected);
    ui->actionEdit->setEnabled(selected);
    ui->actionRemove->setEnabled(selected);

    if (selected && !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        // visible
        ui->actionRestart->setEnabled(true);
        ui->actionSwitch->setEnabled(false);
        // text
        ui->actionStart->setText(QApplication::translate("MainWidget", "Stop"));
        ui->actionStart->setIcon(QIcon::fromTheme("process-stop"));
    } else {
        // visible
        ui->actionRestart->setEnabled(false);
        ui->actionSwitch->setEnabled(selected);
        // text
        ui->actionStart->setText(QApplication::translate("MainWidget", "Start"));
        ui->actionStart->setIcon(QIcon::fromTheme("system-run"));
    }
    if (selected && !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        ui->actionEnable->setText(QApplication::translate("MainWidget", "Disable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("edit-remove"));
    } else {
        ui->actionEnable->setText(QApplication::translate("MainWidget", "Enable"));
        ui->actionEnable->setIcon(QIcon::fromTheme("list-add"));
    }
}


void MainWidget::updateMainTab()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    mainWindow->setDisabled(true);
    netctlInformation info = generalInformation(mainWindow->netctlInterface,
                                                useHelper, debug);
    ui->label_netctlAuto->setHidden(!info.netctlAuto);

    ui->tableWidget_main->setSortingEnabled(false);
    ui->tableWidget_main->selectRow(-1);
    ui->tableWidget_main->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_main->clear();
    ui->tableWidget_main->setRowCount(info.netctlProfiles.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("MainWidget", "Name"));
    headerList.append(QApplication::translate("MainWidget", "Description"));
    headerList.append(QApplication::translate("MainWidget", "Active"));
    headerList.append(QApplication::translate("MainWidget", "Enabled"));
    ui->tableWidget_main->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<info.netctlProfiles.count(); i++) {
        // font
        QFont font;
        font.setBold(info.netctlProfiles[i].active);
        font.setItalic(info.netctlProfiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2@%3\n").arg(QApplication::translate("MainWidget", "Type"))
                                         .arg(info.netctlProfiles[i].type)
                                         .arg(info.netctlProfiles[i].interface);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWidget", "Active"))
                                      .arg(checkStatus(info.netctlProfiles[i].active));
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWidget", "Enabled"))
                                      .arg(checkStatus(info.netctlProfiles[i].enabled));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWidget", "Is wireless"))
                                    .arg(checkStatus(!info.netctlProfiles[i].essid.isEmpty()));
        // name
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(info.netctlProfiles[i].name));
        ui->tableWidget_main->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget_main->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget_main->item(i, 0)->setFont(font);
        // description
        ui->tableWidget_main->setItem(i, 1, new QTableWidgetItem(info.netctlProfiles[i].description));
        ui->tableWidget_main->item(i, 1)->setTextAlignment(Qt::AlignJustify | Qt::AlignVCenter);
        ui->tableWidget_main->item(i, 1)->setToolTip(toolTip);
        // active
        ui->tableWidget_main->setItem(i, 2, new QTableWidgetItem(checkStatus(info.netctlProfiles[i].active, true)));
        ui->tableWidget_main->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // enabled
        ui->tableWidget_main->setItem(i, 3, new QTableWidgetItem(checkStatus(info.netctlProfiles[i].enabled, true)));
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

    mainWindow->setDisabled(false);
    mainWindow->showMessage(true);
}


void MainWidget::mainTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    // create menu
    QMenu menu(this);
    QAction *refreshTable = menu.addAction(QApplication::translate("MainWidget", "Refresh"));
    refreshTable->setIcon(QIcon::fromTheme("view-refresh"));
    menu.addSeparator();
    QAction *startProfile = menu.addAction(QApplication::translate("MainWidget", "Start profile"));
    QAction *restartProfile = menu.addAction(QApplication::translate("MainWidget", "Restart profile"));
    restartProfile->setIcon(QIcon::fromTheme("view-refresh"));
    QAction *enableProfile = menu.addAction(QApplication::translate("MainWidget", "Enable profile"));
    menu.addSeparator();
    QAction *editProfile = menu.addAction(QApplication::translate("MainWidget", "Edit profile"));
    editProfile->setIcon(QIcon::fromTheme("document-edit"));
    QAction *removeProfile = menu.addAction(QApplication::translate("MainWidget", "Remove profile"));
    removeProfile->setIcon(QIcon::fromTheme("edit-delete"));

    // set text
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        restartProfile->setVisible(true);
        startProfile->setText(QApplication::translate("MainWidget", "Stop profile"));
        startProfile->setIcon(QIcon::fromTheme("process-stop"));
    } else {
        restartProfile->setVisible(false);
        startProfile->setText(QApplication::translate("MainWidget", "Start profile"));
        startProfile->setIcon(QIcon::fromTheme("system-run"));
    }
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        enableProfile->setText(QApplication::translate("MainWidget", "Disable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-remove"));
    } else {
        enableProfile->setText(QApplication::translate("MainWidget", "Enable profile"));
        enableProfile->setIcon(QIcon::fromTheme("list-add"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << PDEBUG << ":" << "Refresh table";
        updateMainTab();
    } else if (action == startProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Start profile";
        mainTabStartProfile();
    } else if (action == restartProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Restart profile";
        mainTabRestartProfile();
    } else if (action == enableProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Enable profile";
        mainTabEnableProfile();
    } else if (action == editProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Edit profile";
        mainTabEditProfile();
    } else if (action == removeProfile) {
        if (debug) qDebug() << PDEBUG << ":" << "Remove profile";
        mainTabRemoveProfile();
    }
}


void MainWidget::mainTabEditProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    mainWindow->setTab(1);
    mainWindow->openProfileSlot(profile);
}


void MainWidget::mainTabEnableProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    InterfaceAnswer answer = enableProfileSlot(profile, mainWindow->netctlInterface,
                                               useHelper, debug);
    // TODO check if status has been changed?
    mainWindow->showMessage(answer != InterfaceAnswer::Error);

    updateMainTab();
}


void MainWidget::mainTabRemoveProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("sudo"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("sudo"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    InterfaceAnswer answer = removeProfileSlot(profile, mainWindow->netctlInterface,
                                               useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateMainTab();
}


void MainWidget::mainTabRestartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    InterfaceAnswer answer = restartProfileSlot(profile, mainWindow->netctlInterface,
                                               useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateMainTab();
}


void MainWidget::mainTabStartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    InterfaceAnswer answer = startProfileSlot(profile, mainWindow->netctlInterface,
                                              useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateMainTab();
}


void MainWidget::mainTabStopAllProfiles()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    mainWindow->setDisabled(true);
    InterfaceAnswer answer = stopAllProfilesSlot(mainWindow->netctlInterface,
                                                 useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateMainTab();
}


void MainWidget::mainTabSwitchToProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("netctl"), configuration).join(QChar('\n')), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    InterfaceAnswer answer = switchToProfileSlot(profile, mainWindow->netctlInterface,
                                                 useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateMainTab();
}


void MainWidget::createActions()
{
    if (debug) qDebug() << PDEBUG;

    // menu actions
    connect(ui->actionEnable, SIGNAL(triggered(bool)), this, SLOT(mainTabEnableProfile()));
    connect(ui->actionEdit, SIGNAL(triggered(bool)), this, SLOT(mainTabEditProfile()));
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), this, SLOT(updateMainTab()));
    connect(ui->actionRemove, SIGNAL(triggered(bool)), this, SLOT(mainTabRemoveProfile()));
    connect(ui->actionRestart, SIGNAL(triggered(bool)), this, SLOT(mainTabRestartProfile()));
    connect(ui->actionStart, SIGNAL(triggered(bool)), this, SLOT(mainTabStartProfile()));
    connect(ui->actionStop_all, SIGNAL(triggered(bool)), this, SLOT(mainTabStopAllProfiles()));
    connect(ui->actionSwitch, SIGNAL(triggered(bool)), this, SLOT(mainTabSwitchToProfile()));
    // main tab events
    connect(ui->tableWidget_main, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(mainTabStartProfile()));
    connect(ui->tableWidget_main, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
            this, SLOT(updateMenuMain()));
    connect(ui->tableWidget_main, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(mainTabContextualMenu(QPoint)));
}
