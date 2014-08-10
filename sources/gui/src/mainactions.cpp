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

#include "aboutwindow.h"
#include "bridgewidget.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "language.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "netctlautowindow.h"
#include "netctlguiadaptor.h"
#include "passwdwidget.h"
#include "pppoewidget.h"
#include "settingswindow.h"
#include "taskadds.h"
#include "trayicon.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "version.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"


void MainWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[MainWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Return)
        if (ui->comboBox_profile->hasFocus())
            profileTabLoadProfile();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (debug) qDebug() << "[MainWindow]" << "[closeEvent]";

    if ((QSystemTrayIcon::isSystemTrayAvailable()) &&
            (configuration[QString("SYSTRAY")] == QString("true"))) {
        hide();
        event->ignore();
    }
    else
        closeMainWindow();
}


void MainWindow::closeMainWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[closeMainWindow]";

    qApp->quit();
}


void MainWindow::showAboutWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showAboutWindow]";

    aboutWin->show();
}


void MainWindow::showMainWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showMainWindow]";

    if (isHidden()) {
        updateTabs(ui->tabWidget->currentIndex());
        show();
    }
    else
        hide();
}


void MainWindow::showNetctlAutoWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showNetctlAutoWindow]";

    netctlAutoWin->showWindow();
}


void MainWindow::showSettingsWindow()
{
    if (debug) qDebug() << "[MainWindow]" << "[showSettingsWindow]";

    settingsWin->showWindow();
}


void MainWindow::showApi()
{
    if (debug) qDebug() << "[MainWindow]" << "[showApi]";

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("netctl-gui-dbus-api.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


void MainWindow::showLibrary()
{
    if (debug) qDebug() << "[MainWindow]" << "[showLibrary]";

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("html/index.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


void MainWindow::showSecurityNotes()
{
    if (debug) qDebug() << "[MainWindow]" << "[showSecurityNotes]";

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("netctl-gui-security-notes.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


bool MainWindow::forceStartHelper()
{
    if (debug) qDebug() << "[MainWindow]" << "[forceStartHelper]";
    if (!checkExternalApps(QString("helper"))) {
        errorWin->showWindow(1, QString("[MainWindow] : [forceStartHelper]"));
        return false;
    }

    QString cmd = configuration[QString("HELPER_PATH")] + QString(" -c ") + configPath;
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << "[MainWindow]" << "[checkExternalApps]" << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}


bool MainWindow::forceStopHelper()
{
    if (debug) qDebug() << "[MainWindow]" << "[forceStartHelper]";

    QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                               DBUS_HELPER_INTERFACE, QString("Close"),
                                               QList<QVariant>(), true, debug);

    return !responce.isEmpty();
}


bool MainWindow::startHelper()
{
    if (debug) qDebug() << "[MainWindow]" << "[startHelper]";

    if (isHelperActive())
        return forceStopHelper();
    else
        return forceStartHelper();
}


void MainWindow::setTab(int tab)
{
    if (debug) qDebug() << "[MainWindow]" << "[setTab]";
    if (debug) qDebug() << "[MainWindow]" << "[setTab]" << ":" << "Update tab" << tab;

    switch (tab) {
    case 0:
    case 1:
    case 2:
        break;
    default:
        tab = 0;
        break;
    }
    ui->tabWidget->setCurrentIndex(tab);

    updateTabs(tab);
}


void MainWindow::connectToUnknownEssid(const QString passwd)
{
    if (debug) qDebug() << "[MainWindow]" << "[connectToUnknownEssid]";

    if (passwdWid != 0)
        delete passwdWid;
    QStringList interfaces;
    if (useHelper)
        interfaces = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                     DBUS_HELPER_INTERFACE, QString("WirelessInterfaces"),
                                     QList<QVariant>(), true, debug)[0].toStringList();
    else
        interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty())
        return;

    QMap<QString, QString> settings;
    settings[QString("Description")] = QString("'Automatically generated profile by Netctl GUI'");
    settings[QString("Interface")] = interfaces[0];
    settings[QString("Connection")] = QString("wireless");
    QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 2)->text();
    if (security.contains(QString("WPA")))
        settings[QString("Security")] = QString("wpa");
    else if (security.contains(QString("WEP")))
        settings[QString("Security")] = QString("wep");
    else
        settings[QString("Security")] = QString("none");
    settings[QString("ESSID")] = QString("'") +
            ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() +
            QString("'");
    if (!passwd.isEmpty())
        settings[QString("Key")] = QString("'") + passwd + QString("'");
    settings[QString("IP")] = QString("dhcp");
    if (hiddenNetwork)
        settings[QString("Hidden")] = QString("yes");

    QString profile = QString("netctl-gui-") + settings[QString("ESSID")];
    profile.remove(QChar('"')).remove(QChar('\''));
    bool status = false;
    if (useHelper) {
        QStringList settingsList;
        for (int i=0; i<settings.keys().count(); i++)
            settingsList.append(settings.keys()[i] + QString("==") + settings[settings.keys()[i]]);
        QList<QVariant> args;
        args.append(profile);
        args.append(settingsList);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Create"),
                        args, true, debug);
        args.clear();
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Start"),
                        args, true, debug);
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                 DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                 args, true, debug)[0].toBool();
    }
    else {
        QString profileTempName = netctlProfile->createProfile(profile, settings);
        netctlProfile->copyProfile(profileTempName);
        netctlCommand->startProfile(profile);
        status = netctlCommand->isProfileActive(profile);
    }
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateWifiTab();
}


void MainWindow::setHiddenName(const QString name)
{
    if (debug) qDebug() << "[MainWindow]" << "[setHiddenName]";
    if (debug) qDebug() << "[MainWindow]" << "[setHiddenName]" << ":" << "Set name" << name;

    ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->setText(name);

    wifiTabStart();
}


void MainWindow::reportABug()
{
    if (debug) qDebug() << "[MainWindow]" << "[reportABug]";

    if (QDesktopServices::openUrl(QUrl(QString(BUGTRACKER))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


// main tab slots
void MainWindow::mainTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]";
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
    }
    else {
        restartProfile->setVisible(false);
        startProfile->setText(QApplication::translate("MainWindow", "Start profile"));
        startProfile->setIcon(QIcon::fromTheme("system-run"));
    }
    if (!ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty()) {
        enableProfile->setText(QApplication::translate("MainWindow", "Disable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-remove"));
    }
    else {
        enableProfile->setText(QApplication::translate("MainWindow", "Enable profile"));
        enableProfile->setIcon(QIcon::fromTheme("edit-add"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Refresh table";
        updateMainTab();
    }
    else if (action == startProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Start profile";
        mainTabStartProfile();
    }
    else if (action == restartProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Restart profile";
        mainTabRestartProfile();
    }
    else if (action == enableProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Enable profile";
        mainTabEnableProfile();
    }
    else if (action == editProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Edit profile";
        mainTabEditProfile();
    }
    else if (action == removeProfile) {
        if (debug) qDebug() << "[MainWindow]" << "[mainTabContextualMenu]" << "Remove profile";
        mainTabRemoveProfile();
    }
}


void MainWindow::mainTabEditProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabEditProfile]";

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    ui->tabWidget->setCurrentIndex(1);
    ui->comboBox_profile->addItem(profile);
    ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count()-1);
}


void MainWindow::mainTabEnableProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabEnableProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabEnableProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    bool previous = !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 3)->text().isEmpty();
    bool current;
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Enable"),
                        args, true, debug);
        current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                  DBUS_HELPER_INTERFACE, QString("isProfileEnabled"),
                                  args, true, debug)[0].toBool();
    }
    else {
        netctlCommand->enableProfile(profile);
        current = netctlCommand->isProfileEnabled(profile);
    }
    if (current != previous)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabRemoveProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRemoveProfile]";

    ui->tabWidget->setDisabled(true);
    // call netctlprofile
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool status;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                 DBUS_HELPER_INTERFACE, QString("Remove"),
                                 args, true, debug)[0].toBool();
    }
    else
        status = netctlProfile->removeProfile(profile);
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabRestartProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRestartProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabRestartProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    bool status = false;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Restart"),
                        args, true, debug)[0].toBool();
        status = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                 DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                 args, true, debug)[0].toBool();
    }
    else {
        netctlCommand->restartProfile(profile);
        status = netctlCommand->isProfileActive(profile);
    }
    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabStartProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[mainTabStartProfile]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabStartProfile]"));
    if (ui->tableWidget_main->currentItem() == 0)
        return;

    ui->tabWidget->setDisabled(true);
    bool previous = !ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 2)->text().isEmpty();
    bool current;
    QString profile = ui->tableWidget_main->item(ui->tableWidget_main->currentItem()->row(), 0)->text();
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Start"),
                        args, true, debug);
        current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                  DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                  args, true, debug)[0].toBool();
    }
    else {
        netctlCommand->startProfile(profile);
        current = netctlCommand->isProfileActive(profile);
    }
    if (current != previous)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));

    updateMainTab();
}


void MainWindow::mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << "[MainWindow]" << "[mainTabRefreshButtons]";
    if (!checkExternalApps(QString("netctl")))
        return errorWin->showWindow(1, QString("[MainWindow] : [mainTabRefreshButtons]"));
    if (current == 0) {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setDisabled(true);
        return;
    }

    ui->pushButton_mainStart->setEnabled(true);

    if (!ui->tableWidget_main->item(current->row(), 2)->text().isEmpty()) {
        ui->pushButton_mainRestart->setEnabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Stop"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("process-stop"));
    }
    else {
        ui->pushButton_mainRestart->setDisabled(true);
        ui->pushButton_mainStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_mainStart->setIcon(QIcon::fromTheme("system-run"));
    }
}


// profile tab slots
void MainWindow::profileTabChangeState(const QString current)
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabChangeState]";
    if (debug) qDebug() << "[MainWindow]" << "[profileTabChangeState]" << ":" << "Current type" << current;

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
    }
    else if (current == QString("wireless")) {
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
    }
    else if ((current == QString("bond")) ||
             (current == QString("dummy"))) {
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
    }
    else if (current == QString("bridge")) {
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
    }
    else if (current == QString("pppoe")) {
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
    }
    else if (current == QString("mobile_ppp")) {
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
    }
    else if (current == QString("tunnel")) {
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
    }
    else if (current == QString("tuntap")) {
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
    }
    else if (current == QString("vlan")) {
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
    }
    else if (current == QString("macvlan")) {
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
    if (debug) qDebug() << "[MainWindow]" << "[profileTabClear]";

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
    if (debug) qDebug() << "[MainWindow]" << "[profileTabCreateProfile]";

    // error checking
    if (ui->comboBox_profile->currentText().isEmpty())
        return errorWin->showWindow(3, QString("[MainWindow] : [profileTabCreateProfile]"));
    if (generalWid->isOk() == 1)
        return errorWin->showWindow(4, QString("[MainWindow] : [profileTabCreateProfile]"));
    else if (generalWid->isOk() == 2)
        return errorWin->showWindow(5, QString("[MainWindow] : [profileTabCreateProfile]"));
    if ((generalWid->connectionType->currentText() == QString("ethernet")) ||
            (generalWid->connectionType->currentText() == QString("wireless")) ||
            (generalWid->connectionType->currentText() == QString("bond")) ||
            (generalWid->connectionType->currentText() == QString("dummy")) ||
            (generalWid->connectionType->currentText() == QString("bridge")) ||
            (generalWid->connectionType->currentText() == QString("tunnel")) ||
            (generalWid->connectionType->currentText() == QString("tuntap")) ||
            (generalWid->connectionType->currentText() == QString("vlan")) ||
            (generalWid->connectionType->currentText() == QString("macvlan"))) {
        if (ipWid->isOk() == 1)
            return errorWin->showWindow(6, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (ipWid->isOk() == 2)
            return errorWin->showWindow(6, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1)
            return errorWin->showWindow(8, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 2)
            return errorWin->showWindow(9, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 3)
            return errorWin->showWindow(10, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 4)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (wirelessWid->isOk() == 5)
            return errorWin->showWindow(11, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("bridge")) {
    }
    else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 2)
            return errorWin->showWindow(12, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 3)
            return errorWin->showWindow(13, QString("[MainWindow] : [profileTabCreateProfile]"));
        else if (pppoeWid->isOk() == 4)
            return errorWin->showWindow(12, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1)
            return errorWin->showWindow(14, QString("[MainWindow] : [profileTabCreateProfile]"));
        if (mobileWid->isOk() == 2)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1)
            return errorWin->showWindow(15, QString("[MainWindow] : [profileTabCreateProfile]"));
        if (tuntapWid->isOk() == 2)
            return errorWin->showWindow(15, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        if (ethernetWid->isOk() == 1)
            return errorWin->showWindow(7, QString("[MainWindow] : [profileTabCreateProfile]"));
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
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
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
    }
    else {
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
    if (debug) qDebug() << "[MainWindow]" << "[profileTabLoadProfile]";

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
    }
    else
        settings = netctlProfile->getSettingsFromProfile(profile);



    if (settings.isEmpty())
        return errorWin->showWindow(17, QString("[MainWindow] : [profileTabLoadProfile]"));

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
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        macvlanWid->setSettings(settings);
    }
}


void MainWindow::profileTabRemoveProfile()
{
    if (debug) qDebug() << "[MainWindow]" << "[profileTabRemoveProfile]";

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
    }
    else
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
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]";
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;

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
        }
        else {
            startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
            startWifi->setIcon(QIcon::fromTheme("system-run"));
        }
    }
    else {
        startWifi->setText(QApplication::translate("MainWindow", "Start WiFi"));
        startWifi->setIcon(QIcon::fromTheme("system-run"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_main->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]" << "Refresh WiFi";
        updateWifiTab();
    }
    else if (action == startWifi) {
        if (debug) qDebug() << "[MainWindow]" << "[wifiTabContextualMenu]" << "Start WiFi";
        wifiTabStart();
    }
}


void MainWindow::wifiTabSetEnabled(const bool state)
{
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabSetEnables]";
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabSetEnables]" << ":" << "State" << state;

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


void MainWindow::wifiTabStart()
{
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabStart]";
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [wifiTabStart]"));
    if (ui->tableWidget_wifi->currentItem() == 0)
        return;

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
        }
        else {
            QString profileName = wpaCommand->existentProfile(profile);
            netctlCommand->startProfile(profileName);
            current = netctlCommand->isProfileActive(profileName);
        }
        if (current != previous)
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
        else
            ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    }
    else {
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


void MainWindow::wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (debug) qDebug() << "[MainWindow]" << "[wifiTabRefreshButtons]";
    if (!checkExternalApps(QString("wpasup")))
        return errorWin->showWindow(1, QString("[MainWindow] : [wifiTabRefreshButtons]"));
    if (current == 0) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }
    if (ui->tableWidget_wifi->item(current->row(), 0)->text() == QString("<hidden>")) {
        ui->pushButton_wifiStart->setDisabled(true);
        return;
    }

    ui->pushButton_wifiStart->setEnabled(true);
    if (!ui->tableWidget_wifi->item(current->row(), 4)->text().isEmpty()) {
        if (!ui->tableWidget_wifi->item(current->row(), 3)->text().isEmpty()) {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Stop"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("process-stop"));
        }
        else {
            ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
            ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("system-run"));
        }
    }
    else {
        ui->pushButton_wifiStart->setText(QApplication::translate("MainWindow", "Start"));
        ui->pushButton_wifiStart->setIcon(QIcon::fromTheme("system-run"));
    }
}
