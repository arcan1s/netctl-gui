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
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>

#include <pdebug/pdebug.h>

#include "bridgewidget.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "version.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"


void MainWindow::reportABug()
{
    if (debug) qDebug() << PDEBUG;

    if (QDesktopServices::openUrl(QUrl(QString(BUGTRACKER))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


// menu update slots
void MainWindow::setMenuActionsShown(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    // main
    toolBarActions[QString("mainRefresh")]->setVisible(state);
    toolBarActions[QString("mainStart")]->setVisible(state);
    toolBarActions[QString("mainSwitch")]->setVisible(state);
    toolBarActions[QString("mainRestart")]->setVisible(state);
    toolBarActions[QString("mainEnable")]->setVisible(state);
    toolBarActions[QString("mainStopAll")]->setVisible(state);
    toolBarActions[QString("mainEdit")]->setVisible(state);
    toolBarActions[QString("mainRemove")]->setVisible(state);
    // profile
    toolBarActions[QString("profileClear")]->setVisible(state);
    toolBarActions[QString("profileLoad")]->setVisible(state);
    toolBarActions[QString("profileSave")]->setVisible(state);
    toolBarActions[QString("profileRemove")]->setVisible(state);
    // wifi
    toolBarActions[QString("wifiRefresh")]->setVisible(state);
    toolBarActions[QString("wifiStart")]->setVisible(state);
}


void MainWindow::updateMenuMain()
{
    if (debug) qDebug() << PDEBUG;
    setMenuActionsShown(false);
    actionMenu->setDefaultAction(toolBarActions[QString("mainStart")]);

    toolBarActions[QString("mainRefresh")]->setVisible(true);
    if (ui->tableWidget_main->currentItem() == 0) return;
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        toolBarActions[QString("mainRestart")]->setVisible(true);
        toolBarActions[QString("mainStart")]->setText(QApplication::translate("MainWindow", "Stop"));
        toolBarActions[QString("mainStart")]->setIcon(QIcon::fromTheme("process-stop"));
    } else {
        toolBarActions[QString("mainStart")]->setText(QApplication::translate("MainWindow", "Start"));
        toolBarActions[QString("mainStart")]->setIcon(QIcon::fromTheme("system-run"));
    }
    if (!mainTabGetActiveProfiles().isEmpty()) {
        if (!mainTabGetActiveProfiles()
                .contains(ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text()))
            toolBarActions[QString("mainSwitch")]->setVisible(true);
        toolBarActions[QString("mainStopAll")]->setVisible(true);
    }
    toolBarActions[QString("mainStart")]->setVisible(true);
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        toolBarActions[QString("mainEnable")]->setText(QApplication::translate("MainWindow", "Disable"));
        toolBarActions[QString("mainEnable")]->setIcon(QIcon::fromTheme("edit-remove"));
    } else {
        toolBarActions[QString("mainEnable")]->setText(QApplication::translate("MainWindow", "Enable"));
        toolBarActions[QString("mainEnable")]->setIcon(QIcon::fromTheme("edit-add"));
    }
    toolBarActions[QString("mainEnable")]->setVisible(true);
    toolBarActions[QString("mainEdit")]->setVisible(true);
    toolBarActions[QString("mainRemove")]->setVisible(true);
}


void MainWindow::updateMenuProfile()
{
    if (debug) qDebug() << PDEBUG;
    setMenuActionsShown(false);
    actionMenu->setDefaultAction(toolBarActions[QString("profileSave")]);

    toolBarActions[QString("profileClear")]->setVisible(true);
    if (ui->comboBox_profile->currentText().isEmpty()) {
        toolBarActions[QString("profileLoad")]->setVisible(false);
        toolBarActions[QString("profileRemove")]->setVisible(false);
        toolBarActions[QString("profileSave")]->setVisible(false);
    } else {
        toolBarActions[QString("profileLoad")]->setVisible(true);
        toolBarActions[QString("profileRemove")]->setVisible(true);
        toolBarActions[QString("profileSave")]->setVisible(true);
    }
}


void MainWindow::updateMenuWifi()
{
    if (debug) qDebug() << PDEBUG;
    setMenuActionsShown(false);
    actionMenu->setDefaultAction(toolBarActions[QString("wifiStart")]);

    toolBarActions[QString("wifiRefresh")]->setVisible(true);
    if (ui->tableWidget_wifi->currentItem() == 0) return;
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty()) {
            toolBarActions[QString("wifiStart")]->setText(QApplication::translate("MainWindow", "Stop"));
            toolBarActions[QString("wifiStart")]->setIcon(QIcon::fromTheme("process-stop"));
        } else {
            toolBarActions[QString("wifiStart")]->setText(QApplication::translate("MainWindow", "Start"));
            toolBarActions[QString("wifiStart")]->setIcon(QIcon::fromTheme("system-run"));
        }
    } else {
        toolBarActions[QString("wifiStart")]->setText(QApplication::translate("MainWindow", "Start"));
        toolBarActions[QString("wifiStart")]->setIcon(QIcon::fromTheme("system-run"));
    }
    toolBarActions[QString("wifiStart")]->setVisible(true);
}


void MainWindow::updateToolBars()
{
    if (debug) qDebug() << PDEBUG;

    if (sender() == toolBarActions[QString("menu")]) {
        mainToolBar->setHidden(false);
        actionToolBar->setHidden(true);
        helpToolBar->setHidden(true);
    } else if (sender() == toolBarActions[QString("actions")]) {
        mainToolBar->setHidden(true);
        actionToolBar->setHidden(false);
        helpToolBar->setHidden(true);
        updateMenu();
    } else if (sender() == toolBarActions[QString("help")]) {
        mainToolBar->setHidden(true);
        actionToolBar->setHidden(true);
        helpToolBar->setHidden(false);

    }
}


// tab update slots
void MainWindow::updateMainTab()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));

    ui->tabWidget->setDisabled(true);
    QList<netctlProfileInfo> profiles;
    bool netctlAutoStatus = false;
    if (useHelper) {
        netctlAutoStatus = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                           DBUS_HELPER_INTERFACE, QString("isNetctlAutoActive"),
                                           QList<QVariant>(), true, debug)[0].toBool();
        profiles = parseOutputNetctl(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                     DBUS_HELPER_INTERFACE, QString("ProfileList"),
                                                     QList<QVariant>(), true, debug), debug);
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
    headerList.append(QApplication::translate("MainWindow", "Name"));
    headerList.append(QApplication::translate("MainWindow", "Description"));
    headerList.append(QApplication::translate("MainWindow", "Active"));
    headerList.append(QApplication::translate("MainWindow", "Enabled"));
    ui->tableWidget_main->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<profiles.count(); i++) {
        // font
        QFont font;
        font.setBold(profiles[i].active);
        font.setItalic(profiles[i].enabled);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Profile")).arg(profiles[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Active")).arg(checkStatus(profiles[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWindow", "Enabled")).arg(checkStatus(profiles[i].enabled));
        // name
        ui->tableWidget_main->setItem(i, 0, new QTableWidgetItem(profiles[i].name));
        ui->tableWidget_main->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
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

    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateProfileTab()
{
    if (debug) qDebug() << PDEBUG;

    ui->tabWidget->setDisabled(true);
    profileTabClear();
    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


void MainWindow::updateWifiTab()
{
    if (debug) qDebug() << PDEBUG;
    wifiTabSetEnabled(checkExternalApps(QString("wpasup")));
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString(PDEBUG));

    ui->tabWidget->setDisabled(true);
    QList<netctlWifiInfo> scanResults;
    if (useHelper)
        scanResults = parseOutputWifi(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                                      DBUS_HELPER_INTERFACE, QString("WiFi"),
                                                      QList<QVariant>(), true, debug), debug);
    else
        scanResults = wpaCommand->scanWifi();

    ui->tableWidget_wifi->setSortingEnabled(false);
    ui->tableWidget_wifi->selectRow(-1);
    ui->tableWidget_wifi->sortByColumn(0, Qt::AscendingOrder);
    ui->tableWidget_wifi->clear();
    ui->tableWidget_wifi->setRowCount(scanResults.count());

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("MainWindow", "Name"));
    headerList.append(QApplication::translate("MainWindow", "Signal"));
    headerList.append(QApplication::translate("MainWindow", "Security"));
    headerList.append(QApplication::translate("MainWindow", "Active"));
    headerList.append(QApplication::translate("MainWindow", "Exists"));
    ui->tableWidget_wifi->setHorizontalHeaderLabels(headerList);
    // create items
    for (int i=0; i<scanResults.count(); i++) {
        // font
        QFont font;
        font.setBold(scanResults[i].active);
        font.setItalic(scanResults[i].exists);
        // tooltip
        QString toolTip = QString("");
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "ESSID")).arg(scanResults[i].name);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("MainWindow", "Active")).arg(checkStatus(scanResults[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("MainWindow", "Exists")).arg(checkStatus(scanResults[i].exists));
        // name
        ui->tableWidget_wifi->setItem(i, 0, new QTableWidgetItem(scanResults[i].name));
        ui->tableWidget_wifi->item(i, 0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget_wifi->item(i, 0)->setFont(font);
        // signal
        ui->tableWidget_wifi->setItem(i, 1, new QTableWidgetItem(scanResults[i].signal));
        ui->tableWidget_wifi->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 1)->setToolTip(toolTip);
        // security
        ui->tableWidget_wifi->setItem(i, 2, new QTableWidgetItem(scanResults[i].security));
        ui->tableWidget_wifi->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 2)->setToolTip(toolTip);
        // active
        ui->tableWidget_wifi->setItem(i, 3, new QTableWidgetItem(checkStatus(scanResults[i].active, true)));
        ui->tableWidget_wifi->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // exists
        ui->tableWidget_wifi->setItem(i, 4, new QTableWidgetItem(checkStatus(scanResults[i].exists, true)));
        ui->tableWidget_wifi->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_wifi->setSortingEnabled(true);

    ui->tableWidget_wifi->resizeRowsToContents();
    ui->tableWidget_wifi->resizeColumnsToContents();
    ui->tableWidget_wifi->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_wifi->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tableWidget_wifi->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    ui->tabWidget->setEnabled(true);
    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Updated"));

    update();
}


// main tab slots
void MainWindow::mainTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << PDEBUG;
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
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty()) {
        restartProfile->setVisible(true);
        startProfile->setText(QApplication::translate("MainWindow", "Stop profile"));
        startProfile->setIcon(QIcon::fromTheme("process-stop"));
    } else {
        restartProfile->setVisible(false);
        startProfile->setText(QApplication::translate("MainWindow", "Start profile"));
        startProfile->setIcon(QIcon::fromTheme("system-run"));
    }
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        enableProfile->setText(QApplication::translate("MainWindow", "Disable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-remove"));
    } else {
        enableProfile->setText(QApplication::translate("MainWindow", "Enable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-add"));
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


void MainWindow::mainTabEditProfile()
{
    if (debug) qDebug() << PDEBUG;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    ui->tabWidget->setCurrentIndex(1);
    ui->comboBox_profile->addItem(profile);
    ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count()-1);
}


void MainWindow::mainTabEnableProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));
    if (ui->tableWidget_main->currentItem() == 0) return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool previous = !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty();
    bool current = enableProfileSlot(profile);
    if (current != previous)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


QStringList MainWindow::mainTabGetActiveProfiles()
{
    if (debug) qDebug() << PDEBUG;

    QStringList profiles;
    for (int i=0; i<ui->tableWidget_main->rowCount(); i++)
        if (!ui->tableWidget_main->item(i, 2)->text().isEmpty())
            profiles.append(ui->tableWidget_main->item(i, 0)->text());

    return profiles;
}


void MainWindow::mainTabRemoveProfile()
{
    if (debug) qDebug() << PDEBUG;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool status;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                 DBUS_HELPER_INTERFACE, QString("Remove"),
                                 args, true, debug)[0].toBool();
    } else
        status = netctlProfile->removeProfile(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabRestartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));
    if (ui->tableWidget_main->currentItem() == 0) return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool status = restartProfileSlot(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabStartProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));
    if (ui->tableWidget_main->currentItem() == 0) return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool previous = !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty();
    bool current = startProfileSlot(profile);
    if (current != previous)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabStopAllProfiles()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));

    ui->tabWidget->setDisabled(true);
    bool status;
    if (useHelper)
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                 DBUS_HELPER_INTERFACE, QString("StopAll"),
                                 QList<QVariant>(), true, debug)[0].toBool();
    else
        status = netctlCommand->stopAllProfiles();
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabSwitchToProfile()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString(PDEBUG));
    if (ui->tableWidget_main->currentItem() == 0) return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool previous = !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty();
    bool current = switchToProfileSlot(profile);
    if (current != previous)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


// profile tab slots
void MainWindow::profileTabChangeState(const QString current)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Current type" << current;

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
    } else if (current == QString("wireless")) {
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
    } else if ((current == QString("bond")) ||
               (current == QString("dummy")) ||
               (current == QString("openvswitch"))) {
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
    } else if (current == QString("bridge")) {
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
    } else if (current == QString("pppoe")) {
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
    } else if (current == QString("mobile_ppp")) {
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
    } else if (current == QString("tunnel")) {
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
    } else if (current == QString("tuntap")) {
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
    } else if (current == QString("vlan")) {
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
    } else if (current == QString("macvlan")) {
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
    if (debug) qDebug() << PDEBUG;

    ui->comboBox_profile->clear();
    QList<netctlProfileInfo> profiles;
    if (useHelper)
        profiles = parseOutputNetctl(sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                     DBUS_HELPER_INTERFACE, QString("ProfileList"),
                                                     QList<QVariant>(), true, debug), debug);
    else
        profiles = netctlCommand->getProfileList();
    for (int i=0; i<profiles.count(); i++)
        ui->comboBox_profile->addItem(profiles[i].name);
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
    if (debug) qDebug() << PDEBUG;

    // error checking
    if (ui->comboBox_profile->currentText().isEmpty())
        return errorWin->showWindow(3, QString(PDEBUG));
    if (generalWid->isOk() == 1)
        return errorWin->showWindow(4, QString(PDEBUG));
    else if (generalWid->isOk() == 2)
        return errorWin->showWindow(5, QString(PDEBUG));
    if ((generalWid->connectionType->currentText() == QString("ethernet")) ||
            (generalWid->connectionType->currentText() == QString("wireless")) ||
            (generalWid->connectionType->currentText() == QString("bond")) ||
            (generalWid->connectionType->currentText() == QString("dummy")) ||
            (generalWid->connectionType->currentText() == QString("bridge")) ||
            (generalWid->connectionType->currentText() == QString("tunnel")) ||
            (generalWid->connectionType->currentText() == QString("tuntap")) ||
            (generalWid->connectionType->currentText() == QString("vlan")) ||
            (generalWid->connectionType->currentText() == QString("macvlan")) ||
            (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        if (ipWid->isOk() == 1)
            return errorWin->showWindow(6, QString(PDEBUG));
        else if (ipWid->isOk() == 2)
            return errorWin->showWindow(6, QString(PDEBUG));
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString(PDEBUG));
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1)
            return errorWin->showWindow(8, QString(PDEBUG));
        else if (wirelessWid->isOk() == 2)
            return errorWin->showWindow(9, QString(PDEBUG));
        else if (wirelessWid->isOk() == 3)
            return errorWin->showWindow(10, QString(PDEBUG));
        else if (wirelessWid->isOk() == 4)
            return errorWin->showWindow(7, QString(PDEBUG));
        else if (wirelessWid->isOk() == 5)
            return errorWin->showWindow(11, QString(PDEBUG));
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1)
            return errorWin->showWindow(7, QString(PDEBUG));
        else if (pppoeWid->isOk() == 2)
            return errorWin->showWindow(12, QString(PDEBUG));
        else if (pppoeWid->isOk() == 3)
            return errorWin->showWindow(13, QString(PDEBUG));
        else if (pppoeWid->isOk() == 4)
            return errorWin->showWindow(12, QString(PDEBUG));
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1)
            return errorWin->showWindow(14, QString(PDEBUG));
        if (mobileWid->isOk() == 2)
            return errorWin->showWindow(7, QString(PDEBUG));
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1)
            return errorWin->showWindow(15, QString(PDEBUG));
        if (tuntapWid->isOk() == 2)
            return errorWin->showWindow(15, QString(PDEBUG));
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString(PDEBUG));
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString(PDEBUG));
    }

    ui->tabWidget->setDisabled(true);
    // read settings
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    QMap<QString, QString> settings;
    settings = generalWid->getSettings();
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = wirelessWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if ((generalWid->connectionType->currentText() == QString("bond")) ||
               (generalWid->connectionType->currentText() == QString("dummy")) ||
               (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings =  bridgeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        QMap<QString, QString> addSettings = pppoeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        QMap<QString, QString> addSettings = mobileWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tunnelWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tuntapWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("vlan")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = vlanWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("macvlan")) {
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
    bool status = false;
    if (useHelper) {
        QStringList settingsList;
        for (int i=0; i<settings.keys().count(); i++)
            settingsList.append(settings.keys()[i] + QString("==") + settings[settings.keys()[i]]);
        QList<QVariant> args;
        args.append(profile);
        args.append(settingsList);
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                 DBUS_HELPER_INTERFACE, QString("Create"),
                                 args, true, debug)[0].toBool();
    } else {
        QString profileTempName = netctlProfile->createProfile(profile, settings);
        status = netctlProfile->copyProfile(profileTempName);
    }
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateProfileTab();
}


void MainWindow::profileTabLoadProfile()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    if (profile.isEmpty())
        return;
    QMap<QString, QString> settings;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        QStringList settingsList = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("Profile"),
                                                   args, true, debug)[0].toStringList();
        for (int i=0; i<settingsList.count(); i++) {
            if (!settingsList[i].contains(QString("=="))) continue;
            QString key = settingsList[i].split(QString("=="))[0];
            QString value = settingsList[i].split(QString("=="))[1];
            settings[key] = value;
        }
    } else
        settings = netctlProfile->getSettingsFromProfile(profile);



    if (settings.isEmpty())
        return errorWin->showWindow(17, QString(PDEBUG));

    generalWid->setSettings(settings);
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        ipWid->setSettings(settings);
        wirelessWid->setSettings(settings);
    } else if ((generalWid->connectionType->currentText() == QString("bond")) ||
               (generalWid->connectionType->currentText() == QString("dummy")) ||
               (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        ipWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
        ipWid->setSettings(settings);
        bridgeWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        pppoeWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        mobileWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        ipWid->setSettings(settings);
        tunnelWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        ipWid->setSettings(settings);
        tuntapWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("vlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        vlanWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        macvlanWid->setSettings(settings);
    }
}


void MainWindow::profileTabRemoveProfile()
{
    if (debug) qDebug() << PDEBUG;

    ui->tabWidget->setDisabled(true);
    // call netctlprofile
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    bool status = false;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                 DBUS_HELPER_INTERFACE, QString("Remove"),
                                 args, true, debug)[0].toBool();
    } else
        status = netctlProfile->removeProfile(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateProfileTab();
}


// wifi tab slots
void MainWindow::wifiTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_wifi->currentItem() == 0) return;

    // create menu
    QMenu menu(this);
    QAction *refreshTable = menu.addAction(QApplication::translate("MainWindow", "Refresh"));
    refreshTable->setIcon(QIcon::fromTheme("stock-refresh"));
    menu.addSeparator();
    QAction *startWifi = menu.addAction(QApplication::translate("MainWindow", "Start WiFi"));

    // set text
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty()) {
            startWifi->setText(QApplication::translate("MainWindow", "Stop WiFi"));
            startWifi->setIcon(QIcon::fromTheme("process-stop"));
        } else {
            startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
            startWifi->setIcon(QIcon::fromTheme("system-run"));
        }
    } else {
        startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
        startWifi->setIcon(QIcon::fromTheme("system-run"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << PDEBUG << ":" << "Refresh WiFi";
        updateWifiTab();
    } else if (action == startWifi) {
        if (debug) qDebug() << PDEBUG << ":" << "Start WiFi";
        wifiTabStart();
    }
}


void MainWindow::wifiTabSetEnabled(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    if (state) {
        ui->tableWidget_wifi->show();
        ui->label_wifi->hide();
    } else {
        ui->tableWidget_wifi->hide();
        ui->label_wifi->show();
    }
}


void MainWindow::wifiTabStart()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString(PDEBUG));
    if (ui->tableWidget_wifi->currentItem() == 0) return;

    ui->tabWidget->setDisabled(true);
    // name is hidden
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() == QString("<hidden>")) {
        hiddenNetwork = true;
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

    // name isn't hidden
    hiddenNetwork = false;
    QString profile = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text();
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 4)->text().isEmpty()) {
        bool previous = !ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 3)->text().isEmpty();
        bool current;
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            QString profileName = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                  DBUS_HELPER_INTERFACE, QString("ProfileByEssid"),
                                                  args, true, debug)[0].toString();
            args.clear();
            args.append(profileName);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("Start"),
                            args, true, debug);
            current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                      DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                      args, true, debug)[0].toBool();
        } else {
            QString profileName = wpaCommand->existentProfile(profile);
            netctlCommand->startProfile(profileName);
            current = netctlCommand->isProfileActive(profileName);
        }
        if (current != previous)
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    } else {
        QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
        if (security.contains(QString("none")))
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
