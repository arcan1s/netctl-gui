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
#include <QTimer>
#include <QTranslator>
#include <QUrl>

#include "aboutwindow.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "language.h"
#include "netctlautowindow.h"
#include "passwdwidget.h"
#include "settingswindow.h"
#include "taskadds.h"
#include "trayicon.h"
#include "version.h"


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


bool MainWindow::enableProfileSlot(const QString profile)
{
    if (debug) qDebug() << "[MainWindow]" << "[enableProfileSlot]";
    if (debug) qDebug() << "[MainWindow]" << "[enableProfileSlot]" << ":" << "Profile" << profile;

    bool current;
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

    return current;
}


bool MainWindow::restartProfileSlot(const QString profile)
{
    if (debug) qDebug() << "[MainWindow]" << "[restartProfileSlot]";
    if (debug) qDebug() << "[MainWindow]" << "[restartProfileSlot]" << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Restart"),
                        args, true, debug)[0].toBool();
        current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                  DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                  args, true, debug)[0].toBool();
    }
    else {
        netctlCommand->restartProfile(profile);
        current = netctlCommand->isProfileActive(profile);
    }

    return current;
}


bool MainWindow::startProfileSlot(const QString profile)
{
    if (debug) qDebug() << "[MainWindow]" << "[startProfileSlot]";
    if (debug) qDebug() << "[MainWindow]" << "[startProfileSlot]" << ":" << "Profile" << profile;

    bool current;
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

    return current;
}


bool MainWindow::switchToProfileSlot(const QString profile)
{
    if (debug) qDebug() << "[MainWindow]" << "[switchToProfileSlot]";
    if (debug) qDebug() << "[MainWindow]" << "[switchToProfileSlot]" << ":" << "Profile" << profile;

    bool netctlAutoStatus = false;
    if (useHelper)
        netctlAutoStatus = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                           DBUS_HELPER_INTERFACE, QString("isNetctlAutoActive"),
                                           QList<QVariant>(), true, debug)[0].toBool();
    else
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();

    bool current;
    if (netctlAutoStatus) {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("autoStart"),
                            args, true, debug);
            current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                      DBUS_HELPER_INTERFACE, QString("autoIsProfileActive"),
                                      args, true, debug)[0].toBool();
        }
        else {
            netctlCommand->autoStartProfile(profile);
            current = netctlCommand->autoIsProfileActive(profile);
        }
    }
    else {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("SwitchTo"),
                            args, true, debug);
            current = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                      DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                      args, true, debug)[0].toBool();
        }
        else {
            netctlCommand->switchToProfile(profile);
            current = netctlCommand->isProfileActive(profile);
        }
    }

    return current;
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


void MainWindow::updateConfiguration(const QMap<QString, QVariant> args)
{
    if (debug) qDebug() << "[MainWindow]" << "[updateConfiguration]";

    deleteObjects();

    settingsWin = new SettingsWindow(this, debug, configPath);
    if (args[QString("defauls")].toBool())
        settingsWin->setDefault();
    configuration = settingsWin->getSettings();
    QMap<QString, QString> optionsDict = parseOptions(args[QString("options")].toString());
    for (int i=0; i<optionsDict.keys().count(); i++)
        configuration[optionsDict.keys()[i]] = optionsDict[optionsDict.keys()[i]];
    if ((configuration[QString("USE_HELPER")] == QString("true")) &&
            (checkExternalApps(QString("helper"))))
        useHelper = true;
    else {
        useHelper = false;
        configuration[QString("USE_HELPER")] = QString("false");
    }
    // some helper fixs
    // because interface will be created with a delay
    QTimer::singleShot(1000, this, SLOT(checkHelperStatus()));

    // update translation
    qApp->removeTranslator(translator);
    QString language = Language::defineLanguage(configPath,
                                                args[QString("options")].toString());
    translator->load(QString(":/translations/") + language);
    qApp->installTranslator(translator);
    // update settingsWin
    delete settingsWin;

    createObjects();
    // update ui
    setTab(args[QString("tab")].toInt() - 1);
    createActions();

    // tray
    if (isDaemon)
        return;
    if ((QSystemTrayIcon::isSystemTrayAvailable()) &&
            (configuration[QString("SYSTRAY")] == QString("true")))
        trayIcon->setVisible(true);
    else
        trayIcon->setVisible(false);
    if (trayIcon->isVisible()) {
        if (configuration[QString("STARTTOTRAY")] == QString("true"))
            hide();
        else
            show();
        if (args[QString("minimized")].toInt() == 2)
            show();
        else if (args[QString("minimized")].toInt() == 3)
            hide();
    }
    else
        show();
}


void MainWindow::updateMenu()
{
    if (debug) qDebug() << "[MainWindow]" << "[updateMenu]";

    int tab = ui->tabWidget->currentIndex();
    setMenuActionsShown(false);

    if (tab == 0)
        updateMenuMain();
    else if (tab == 1)
        updateMenuProfile();
    else if (tab == 2)
        updateMenuWifi();
}


void MainWindow::updateTabs(const int tab)
{
    if (debug) qDebug() << "[MainWindow]" << "[updateTabs]";
    if (debug) qDebug() << "[MainWindow]" << "[updateTabs]" << ":" << "Update tab" << tab;

    if (tab == 0)
        updateMainTab();
    else if (tab == 1)
        updateProfileTab();
    else if (tab == 2)
        updateWifiTab();
    updateMenu();
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
