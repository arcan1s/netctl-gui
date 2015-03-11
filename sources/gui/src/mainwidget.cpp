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

#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "mainwindow.h"
#include "netctlautowindow.h"


MainWidget::MainWidget(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QWidget(parent),
      debug(debugCmd),
      configuration(settings)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (configuration[QString("USE_HELPER")] == QString("true"));

    createObjects();
    createToolBars();
    createActions();
}


MainWidget::~MainWidget()
{
    if (debug) qDebug() << PDEBUG;

    deleteObjects();
}


void MainWidget::showNetctlAutoWindow()
{
    if (debug) qDebug() << PDEBUG;

    netctlAutoWin->showWindow();
}


void MainWidget::update()
{
    if (debug) qDebug() << PDEBUG;

    updateMainTab();
    updateMenuMain();
}


void MainWidget::updateMenuMain()
{
    if (debug) qDebug() << PDEBUG;

    bool selected = (ui->tableWidget_main->currentItem() != nullptr);
    toolBarActions[QString("mainStart")]->setVisible(selected);
    toolBarActions[QString("mainEnable")]->setVisible(selected);
    toolBarActions[QString("mainEdit")]->setVisible(selected);
    toolBarActions[QString("mainRemove")]->setVisible(selected);
    toolBarActions[QString("mainStopAll")]->setVisible(!mainTabGetActiveProfiles().isEmpty());

    if (selected && !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        // visible
        toolBarActions[QString("mainRestart")]->setVisible(true);
        toolBarActions[QString("mainSwitch")]->setVisible(true);
        // text
        toolBarActions[QString("mainStart")]->setText(QApplication::translate("MainWidget", "Stop"));
        toolBarActions[QString("mainStart")]->setIcon(QIcon::fromTheme("process-stop"));
    } else {
        // visible
        toolBarActions[QString("mainRestart")]->setVisible(false);
        toolBarActions[QString("mainSwitch")]->setVisible(false);
        // text
        toolBarActions[QString("mainStart")]->setText(QApplication::translate("MainWidget", "Start"));
        toolBarActions[QString("mainStart")]->setIcon(QIcon::fromTheme("system-run"));
    }
    if (selected && !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        toolBarActions[QString("mainEnable")]->setText(QApplication::translate("MainWidget", "Disable"));
        toolBarActions[QString("mainEnable")]->setIcon(QIcon::fromTheme("edit-remove"));
    } else {
        toolBarActions[QString("mainEnable")]->setText(QApplication::translate("MainWidget", "Enable"));
        toolBarActions[QString("mainEnable")]->setIcon(QIcon::fromTheme("list-add"));
    }
}


void MainWidget::updateMainTab()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }

    mainWindow->setDisabled(true);
    QList<netctlProfileInfo> profiles;
    bool netctlAutoStatus = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToLib(QString("isNetctlAutoActive"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return updateMainTab();
        }
        netctlAutoStatus = responce[0].toBool();
        profiles = parseOutputNetctl(sendRequestToLib(QString("netctlVerboseProfileList"), debug));
    } else {
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();
        profiles = netctlCommand->getProfileList();
    }
    ui->widget_netctlAuto->setHidden(!netctlAutoStatus);

    ui->tableWidget_main->setSortingEnabled(false);
    ui->tableWidget_main->selectRow(-1);
    ui->tableWidget_main->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_main->clear();
    ui->tableWidget_main->setRowCount(profiles.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("MainWidget", "Name"));
    headerList.append(QApplication::translate("MainWidget", "Description"));
    headerList.append(QApplication::translate("MainWidget", "Active"));
    headerList.append(QApplication::translate("MainWidget", "Enabled"));
    ui->tableWidget_main->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<profiles.count(); i++) {
        // font
        QFont font;
        font.setBold(profiles[i].active);
        font.setItalic(profiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2@%3\n").arg(QApplication::translate("MainWidget", "Type"))
                                         .arg(profiles[i].type).arg(profiles[i].interface);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWidget", "Active"))
                                      .arg(checkStatus(profiles[i].active));
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWidget", "Enabled"))
                                      .arg(checkStatus(profiles[i].enabled));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWidget", "Is wireless"))
                                    .arg(checkStatus(!profiles[i].essid.isEmpty()));
        // name
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(profiles[i].name));
        ui->tableWidget_main->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
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
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    mainWindow->showMessage(mainWindow->enableProfileSlot(profile));

    updateMainTab();
}


QStringList MainWidget::mainTabGetActiveProfiles()
{
    if (debug) qDebug() << PDEBUG;

    QStringList profiles;
    for (int i=0; i<ui->tableWidget_main->rowCount(); i++) {
        if (ui->tableWidget_main->item(i, 2)->text().isEmpty()) continue;
        profiles.append(ui->tableWidget_main->item(i, 0)->text());
    }

    return profiles;
}


void MainWidget::mainTabRemoveProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool status = false;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        QList<QVariant> responce = sendRequestToCtrlWithArgs(QString("Remove"), args, debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return mainTabRemoveProfile();
        }
        status = responce[0].toBool();
    } else
        status = netctlProfile->removeProfile(profile);
    mainWindow->showMessage(status);

    updateMainTab();
}


void MainWidget::mainTabRestartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    mainWindow->showMessage(mainWindow->restartProfileSlot(profile));

    updateMainTab();
}


void MainWidget::mainTabStartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    mainWindow->showMessage(mainWindow->startProfileSlot(profile));

    updateMainTab();
}


void MainWidget::mainTabStopAllProfiles()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }

    mainWindow->setDisabled(true);
    bool status = false;
    if (useHelper) {
        QList<QVariant> responce = sendRequestToCtrl(QString("StolAll"), debug);
        if (responce.isEmpty()) {
            if (debug) qDebug() << PDEBUG << ":" << "Could not interact with helper, disable it";
            useHelper = false;
            return mainTabStopAllProfiles();
        }
        status = responce[0].toBool();
    } else
        status = netctlCommand->stopAllProfiles();
    mainWindow->showMessage(status);

    updateMainTab();
}


void MainWidget::mainTabSwitchToProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(mainWindow->needToBeConfigured());
        return;
    }
    if (ui->tableWidget_main->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    mainWindow->showMessage(mainWindow->switchToProfileSlot(profile));

    updateMainTab();
}


void MainWidget::createActions()
{
    if (debug) qDebug() << PDEBUG;

    // main tab events
    connect(ui->pushButton_netctlAuto, SIGNAL(clicked(bool)), this, SLOT(showNetctlAutoWindow()));
    connect(ui->tableWidget_main, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(mainTabStartProfile()));
    connect(ui->tableWidget_main, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
            this, SLOT(updateMenuMain()));
    connect(ui->tableWidget_main, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(mainTabContextualMenu(QPoint)));
}


void MainWidget::createObjects()
{
    if (debug) qDebug() << PDEBUG;

    // backend
    netctlCommand = new Netctl(debug, configuration);
    netctlProfile = new NetctlProfile(debug, configuration);
    // windows
    ui = new Ui::MainWidget;
    ui->setupUi(this);
    ui->tableWidget_main->setColumnHidden(2, true);
    ui->tableWidget_main->setColumnHidden(3, true);
    netctlAutoWin = new NetctlAutoWindow(this, debug, configuration);
}


void MainWidget::createToolBars()
{
    if (debug) qDebug() << PDEBUG;

    actionToolBar = new QToolBar(this);
    actionToolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    toolBarActions[QString("netctlAuto")] = actionToolBar->addAction(QApplication::translate("MainWidget", "netctl-auto"),
                                                                     this, SLOT(showNetctlAutoWindow()));
    toolBarActions[QString("mainRefresh")] = actionToolBar->addAction(QIcon::fromTheme(QString("view-refresh")),
                                                                      QApplication::translate("MainWidget", "Refresh"),
                                                                      this, SLOT(updateMainTab()));

    actionMenu = new QToolButton(this);
    actionMenu->setPopupMode(QToolButton::DelayedPopup);
    actionMenu->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    QMenu *menu = new QMenu(actionMenu);
    toolBarActions[QString("mainStart")] = menu->addAction(QIcon::fromTheme(QString("system-run")),
                                                           QApplication::translate("MainWidget", "Start"),
                                                           this, SLOT(mainTabStartProfile()));
    toolBarActions[QString("mainSwitch")] = menu->addAction(QIcon::fromTheme(QString("system-run")),
                                                            QApplication::translate("MainWidget", "Switch"),
                                                            this, SLOT(mainTabSwitchToProfile()));
    toolBarActions[QString("mainRestart")] = menu->addAction(QIcon::fromTheme(QString("view-refresh")),
                                                             QApplication::translate("MainWidget", "Restart"),
                                                             this, SLOT(mainTabRestartProfile()));
    toolBarActions[QString("mainEnable")] = menu->addAction(QIcon::fromTheme(QString("list-add")),
                                                            QApplication::translate("MainWidget", "Enable"),
                                                            this, SLOT(mainTabEnableProfile()));
    toolBarActions[QString("mainStopAll")] = menu->addAction(QIcon::fromTheme(QString("process-stop")),
                                                             QApplication::translate("MainWidget", "Stop all"),
                                                             this, SLOT(mainTabStopAllProfiles()));
    actionMenu->setMenu(menu);
    actionToolBar->addWidget(actionMenu);

    toolBarActions[QString("mainEdit")] = actionToolBar->addAction(QIcon::fromTheme(QString("document-edit")),
                                                                   QApplication::translate("MainWidget", "Edit"),
                                                                   this, SLOT(mainTabEditProfile()));
    toolBarActions[QString("mainRemove")] = actionToolBar->addAction(QIcon::fromTheme(QString("edit-delete")),
                                                                     QApplication::translate("MainWidget", "Remove"),
                                                                     this, SLOT(mainTabRemoveProfile()));

    actionMenu->setDefaultAction(toolBarActions[QString("mainStart")]);
    ui->verticalLayout->insertWidget(0, actionToolBar);
}


void MainWidget::deleteObjects()
{
    if (debug) qDebug() << PDEBUG;

    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;

    if (netctlAutoWin != nullptr) delete netctlAutoWin;
    if (actionMenu != nullptr) {
        actionMenu->menu()->clear();
        delete actionMenu;
    }
    if (actionToolBar != nullptr) {
        actionToolBar->clear();
        delete actionToolBar;
    }
    if (ui != nullptr) delete ui;
}
