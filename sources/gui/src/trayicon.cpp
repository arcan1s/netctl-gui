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

#include "trayicon.h"

#include <QDebug>
#include <QMenu>

#include <pdebug/pdebug.h>

#include "calls.h"
#include "mainwindow.h"


TrayIcon::TrayIcon(QObject *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QSystemTrayIcon(parent),
      debug(debugCmd)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (settings[QString("USE_HELPER")] == QString("true"));

    init();
}


TrayIcon::~TrayIcon()
{
    if (debug) qDebug() << PDEBUG;

    setContextMenu(0);
    startProfileMenu->clear();
    switchToProfileMenu->clear();
    menuActions->clear();
    delete menuActions;
}


void TrayIcon::updateMenu()
{
    if (debug) qDebug() << PDEBUG;

    netctlCurrent info = mainWindow->printTrayInformation();

    if (info.current.isEmpty()) {
        contextMenu[QString("title")]->setIcon(QIcon(QString(":network-offline-64x64")));
        contextMenu[QString("title")]->setText(QApplication::translate("TrayIcon", "(inactive)"));
    } else {
        contextMenu[QString("title")]->setIcon(QIcon(QString(":network-idle-64x64")));
        QStringList profiles;
        if (info.netctlAuto)
            profiles.append(QString("%1 (%2)").arg(info.current[0])
                                              .arg(QApplication::translate("TrayIcon", "netctl-auto")));
        else
            for (int i=0; i<info.current.count(); i++) {
                QString status;
                if (info.enables[i])
                    status = QApplication::translate("TrayIcon", "enabled");
                else
                    status = QApplication::translate("TrayIcon", "static");
                profiles.append(QString("%1 (%2)").arg(info.current[i]).arg(status));
            }
        contextMenu[QString("title")]->setText(profiles.join(QString(" | ")));
    }

    startProfileMenu->menuAction()->setVisible(!info.netctlAuto);
    contextMenu[QString("stop")]->setVisible(!info.netctlAuto && (info.current.count() == 1));
    contextMenu[QString("stopall")]->setVisible(!info.netctlAuto && (info.current.count() > 1));
    switchToProfileMenu->menuAction()->setVisible(info.netctlAuto);
    contextMenu[QString("restart")]->setVisible(!info.netctlAuto && (info.current.count() == 1));
    contextMenu[QString("enable")]->setVisible(!info.netctlAuto && (info.current.count() == 1));

    if (info.netctlAuto) {
        switchToProfileMenu->clear();
        for (int i=0; i<info.profiles.count(); i++)
            switchToProfileMenu->addAction(info.profiles[i]);
    } else {
        if (info.current.count() == 1) {
            contextMenu[QString("stop")]->setText(QApplication::translate("TrayIcon", "Stop %1").arg(info.current[0]));
            contextMenu[QString("restart")]->setText(QApplication::translate("TrayIcon", "Restart %1").arg(info.current[0]));
            if (info.enables[0])
                contextMenu[QString("enable")]->setText(QApplication::translate("TrayIcon", "Disable %1").arg(info.current[0]));
            else
                contextMenu[QString("enable")]->setText(QApplication::translate("TrayIcon", "Enable %1").arg(info.current[0]));
        }
        startProfileMenu->clear();
        for (int i=0; i<info.profiles.count(); i++)
            startProfileMenu->addAction(info.profiles[i]);
    }

//     setContextMenu(menuActions);
}


void TrayIcon::createActions()
{
    if (debug) qDebug() << PDEBUG;

    menuActions = new QMenu();

    contextMenu[QString("title")] = menuActions->addAction(QIcon(":icon"),
                                                           QApplication::translate("TrayIcon", "Status"),
                                                           mainWindow,
                                                           SLOT(showMainWindow()));
    menuActions->addSeparator();

    startProfileMenu = menuActions->addMenu(QIcon::fromTheme("system-run"),
                                            QApplication::translate("TrayIcon", "Start profile"));
    connect(startProfileMenu, SIGNAL(triggered(QAction *)), this, SLOT(startProfileTraySlot(QAction *)));

    contextMenu[QString("stop")] = menuActions->addAction(QIcon::fromTheme("process-stop"),
                                                          QApplication::translate("TrayIcon", "Stop profile"),
                                                          this,
                                                          SLOT(startProfileTraySlot()));
    contextMenu[QString("stopall")] = menuActions->addAction(QIcon::fromTheme("process-stop"),
                                                             QApplication::translate("TrayIcon", "Stop all profiles"),
                                                             this,
                                                             SLOT(stopAllProfilesTraySlot()));

    switchToProfileMenu = menuActions->addMenu(QIcon::fromTheme("system-run"),
                                               QApplication::translate("TrayIcon", "Switch to profile"));
    connect(switchToProfileMenu, SIGNAL(triggered(QAction *)), this, SLOT(switchToProfileTraySlot(QAction *)));

    contextMenu[QString("restart")] = menuActions->addAction(QIcon::fromTheme("view-refresh"),
                                                             QApplication::translate("TrayIcon", "Restart profile"),
                                                             this,
                                                             SLOT(restartProfileTraySlot()));
    contextMenu[QString("enable")] = menuActions->addAction(QIcon::fromTheme("list-add"),
                                                            QApplication::translate("TrayIcon", "Enable profile"),
                                                            this,
                                                            SLOT(enableProfileTraySlot()));
    menuActions->addSeparator();

    contextMenu[QString("quit")] = menuActions->addAction(QIcon::fromTheme("application-exit"),
                                                          QApplication::translate("TrayIcon", "Quit"),
                                                          mainWindow,
                                                          SLOT(closeMainWindow()));
}


void TrayIcon::init()
{
    if (debug) qDebug() << PDEBUG;

    setIcon(QIcon(":icon"));
    setToolTip(QString("netctl-gui"));

    createActions();
    setContextMenu(menuActions);

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(itemActivated(QSystemTrayIcon::ActivationReason)));
}


void TrayIcon::itemActivated(const QSystemTrayIcon::ActivationReason reason)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Reason" << reason;

    switch (reason) {
    case QSystemTrayIcon::Trigger:
        mainWindow->showMainWindow();
        break;
    case QSystemTrayIcon::Context:
        updateMenu();
        break;
    default:
        break;
    }
}


void TrayIcon::enableProfileTraySlot()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = mainWindow->printInformation()[0];

    enableProfileSlot(profile, mainWindow->netctlInterface, useHelper, debug);
}


void TrayIcon::restartProfileTraySlot()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = mainWindow->printInformation()[0];

    restartProfileSlot(profile, mainWindow->netctlInterface, useHelper, debug);
}


void TrayIcon::startProfileTraySlot(QAction *action)
{
    if (debug) qDebug() << PDEBUG;

    QString profile = action == nullptr ? mainWindow->printInformation()[0] : action->text().remove(QChar('&'));

    startProfileSlot(profile, mainWindow->netctlInterface, useHelper, debug);
}


void TrayIcon::stopAllProfilesTraySlot()
{
    if (debug) qDebug() << PDEBUG;

    stopAllProfilesSlot(mainWindow->netctlInterface, useHelper, debug);
}


void TrayIcon::switchToProfileTraySlot(QAction *action)
{
    if (debug) qDebug() << PDEBUG;
    if (action == nullptr) return;

    QString profile = action->text().remove(QChar('&'));

    switchToProfileSlot(profile, mainWindow->netctlInterface, useHelper, debug);
}
