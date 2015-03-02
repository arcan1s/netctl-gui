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
#include <QLibraryInfo>
#include <QTranslator>
#include <QUrl>

#include <language/language.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "aboutwindow.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "netctlautowindow.h"
#include "passwdwidget.h"
#include "settingswindow.h"
#include "trayicon.h"
#include "version.h"


void MainWindow::closeMainWindow()
{
    if (debug) qDebug() << PDEBUG;

    qApp->quit();
}


void MainWindow::showAboutWindow()
{
    if (debug) qDebug() << PDEBUG;

    aboutWin->show();
}


void MainWindow::showMainWindow()
{
    if (debug) qDebug() << PDEBUG;

    if (isHidden()) {
        updateTabs(ui->tabWidget->currentIndex());
        show();
    } else
        hide();
}


void MainWindow::showNetctlAutoWindow()
{
    if (debug) qDebug() << PDEBUG;

    netctlAutoWin->showWindow();
}


void MainWindow::showSettingsWindow()
{
    if (debug) qDebug() << PDEBUG;

    settingsWin->showWindow();
}


bool MainWindow::enableProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Enable"),
                        args, true, debug);
        QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("isProfileEnabled"),
                                                   args, true, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileEnabled(profile);
        else
            current = responce[0].toBool();
    } else {
        netctlCommand->enableProfile(profile);
        current = netctlCommand->isProfileEnabled(profile);
    }

    return current;
}


bool MainWindow::restartProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Restart"),
                        args, true, debug)[0].toBool();
        QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                                   args, true, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileActive(profile);
        else
            current = responce[0].toBool();
    } else {
        netctlCommand->restartProfile(profile);
        current = netctlCommand->isProfileActive(profile);
    }

    return current;
}


bool MainWindow::startProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool current;
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("ActiveProfile"),
                                                   QList<QVariant>(), true, debug);
        QStringList currentProfile;
        if (!responce.isEmpty()) currentProfile = responce[0].toString().split(QChar('|'));
        if ((currentProfile.isEmpty()) || (currentProfile.contains(profile)))
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("Start"),
                            args, true, debug);
        else
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("SwitchTo"),
                            args, true, debug);
        responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                   DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                   args, true, debug);
        if (responce.isEmpty())
            current = netctlCommand->isProfileActive(profile);
        else
            current = responce[0].toBool();
    } else {
        QStringList currentProfile = netctlCommand->getActiveProfile();
        if ((currentProfile.isEmpty()) || (currentProfile.contains(profile)))
            netctlCommand->startProfile(profile);
        else
            netctlCommand->switchToProfile(profile);
        current = netctlCommand->isProfileActive(profile);
    }

    return current;
}


bool MainWindow::stopAllProfilesSlot()
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper)
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("StopAll"),
                        QList<QVariant>(), true, debug);
    else
        netctlCommand->stopAllProfiles();

    return true;
}


bool MainWindow::switchToProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool netctlAutoStatus = false;
    if (useHelper) {
        QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("isNetctlAutoActive"),
                                                   QList<QVariant>(), true, debug);
        if (!responce.isEmpty()) netctlAutoStatus = responce[0].toBool();
    } else
        netctlAutoStatus = netctlCommand->isNetctlAutoRunning();

    bool current;
    if (netctlAutoStatus) {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("autoStart"),
                            args, true, debug);
            QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                       DBUS_HELPER_INTERFACE, QString("autoIsProfileActive"),
                                                       args, true, debug);
            if (responce.isEmpty())
                current = netctlCommand->autoIsProfileActive(profile);
            else
                current = responce[0].toBool();
        } else {
            netctlCommand->autoStartProfile(profile);
            current = netctlCommand->autoIsProfileActive(profile);
        }
    } else {
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("SwitchTo"),
                            args, true, debug);
            QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                       DBUS_HELPER_INTERFACE, QString("isProfileActive"),
                                                       args, true, debug);
            if (responce.isEmpty())
                current = netctlCommand->isProfileActive(profile);
            else
                current = responce[0].toBool();
        } else {
            netctlCommand->switchToProfile(profile);
            current = netctlCommand->isProfileActive(profile);
        }
    }

    return current;
}


void MainWindow::showApi()
{
    if (debug) qDebug() << PDEBUG;

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("netctl-gui-dbus-api.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


void MainWindow::showLibrary()
{
    if (debug) qDebug() << PDEBUG;

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("html/index.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


void MainWindow::showSecurityNotes()
{
    if (debug) qDebug() << PDEBUG;

    if (QDesktopServices::openUrl(QUrl(QString(DOCS_PATH) + QString("netctl-gui-security-notes.html"))))
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


bool MainWindow::forceStartHelper()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("helper"))) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        emit(needToBeConfigured());
        return false;
    }

    QString cmd = QString("%1 -c %2").arg(configuration[QString("HELPER_PATH")]).arg(configPath);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


bool MainWindow::forceStopHelper()
{
    if (debug) qDebug() << PDEBUG;

    QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                               DBUS_HELPER_INTERFACE, QString("Close"),
                                               QList<QVariant>(), true, debug);

    return !responce.isEmpty();
}


bool MainWindow::startHelper()
{
    if (debug) qDebug() << PDEBUG;

    if (isHelperActive())
        return forceStopHelper();
    else
        return forceStartHelper();
}


void MainWindow::setTab(int tab)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Update tab" << tab;

    if (tab > 2) tab = 0;
    ui->tabWidget->setCurrentIndex(tab);

    updateTabs(tab);
}


void MainWindow::updateConfiguration(const QMap<QString, QVariant> args)
{
    if (debug) qDebug() << PDEBUG;

    deleteObjects();

    settingsWin = new SettingsWindow(this, debug, configPath);
    if (args[QString("default")].toBool())
        settingsWin->setDefault();
    configuration = settingsWin->getSettings();
    delete settingsWin;
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

    // update translation
    qApp->removeTranslator(translator);
    QString language = Language::defineLanguage(configPath, args[QString("options")].toString());
    qtTranslator->load(QString("qt_%1").arg(language), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);
    translator->load(QString(":/translations/") + language);
    qApp->installTranslator(translator);

    createObjects();
    createActions();
    createToolBars();

    // tray
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
        if (args[QString("minimized")].toInt() == 1)
            show();
        else if (args[QString("minimized")].toInt() == 2)
            hide();
    }
    else
        show();

    // update ui
    setTab(args[QString("tab")].toInt() - 1);
}


void MainWindow::updateMenu()
{
    if (debug) qDebug() << PDEBUG;

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
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Update tab" << tab;

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
    if (debug) qDebug() << PDEBUG;

    if (passwdWid != nullptr) delete passwdWid;
    QStringList interfaces;
    if (useHelper) {
        QList<QVariant> responce = sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_LIB_PATH,
                                                   DBUS_HELPER_INTERFACE, QString("WirelessInterfaces"),
                                                   QList<QVariant>(), true, debug);
        if (responce.isEmpty())
            interfaces = netctlCommand->getWirelessInterfaceList();
        else
            interfaces = responce[0].toStringList();
    } else
        interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) return;

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
    settings[QString("ESSID")] = QString("'%1'").arg(ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text());
    if (!passwd.isEmpty())
        settings[QString("Key")] = QString("'%1'").arg(passwd);
    settings[QString("IP")] = QString("dhcp");
    if (hiddenNetwork)
        settings[QString("Hidden")] = QString("yes");

    QString profile = QString("netctl-gui-") + settings[QString("ESSID")];
    profile.remove(QChar('"')).remove(QChar('\''));
    if (useHelper) {
        QStringList settingsList;
        for (int i=0; i<settings.keys().count(); i++)
            settingsList.append(QString("%1==%2").arg(settings.keys()[i]).arg(settings[settings.keys()[i]]));
        QList<QVariant> args;
        args.append(profile);
        args.append(settingsList);
        sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                        DBUS_HELPER_INTERFACE, QString("Create"),
                        args, true, debug);
    } else {
        QString profileTempName = netctlProfile->createProfile(profile, settings);
        netctlProfile->copyProfile(profileTempName);
    }
    QString message;
    if (startProfileSlot(profile)) {
        message = QApplication::translate("MainWindow", "Connection is successfully.");
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    } else {
        message = QApplication::translate("MainWindow", "Connection failed.");
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
    }
    message += QString("\n");
    message += QApplication::translate("MainWindow", "Do you want to save profile %1?").arg(profile);
    int select = QMessageBox::question(this, QApplication::translate("MainWindow", "WiFi menu"),
                                       message, QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save);
    switch (select) {
    case QMessageBox::Save:
        break;
    case QMessageBox::Discard:
    default:
        if (useHelper) {
            QList<QVariant> args;
            args.append(profile);
            sendDBusRequest(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                            DBUS_HELPER_INTERFACE, QString("Remove"),
                            args, true, debug);
        } else
            netctlProfile->removeProfile(profile);
        break;
    }

    updateWifiTab();
}


void MainWindow::setHiddenName(const QString name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Set name" << name;

    ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->setText(name);

    wifiTabStart();
}
