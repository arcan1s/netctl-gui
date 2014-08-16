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
#include <QMessageBox>

#include "mainwindow.h"
#include "pdebug.h"


TrayIcon::TrayIcon(QObject *parent, const bool debugCmd)
    : QSystemTrayIcon(parent),
      debug(debugCmd)
{
    mainWindow = (MainWindow *)parent;

    init();
}


TrayIcon::~TrayIcon()
{
    if (debug) qDebug() << PDEBUG;

    setContextMenu(0);
    startProfileMenu->clear();
    switchToProfileMenu->clear();
    menuActions->clear();
    delete startProfileMenu;
    delete switchToProfileMenu;
    delete menuActions;
    for (int i=0; i<contextMenu.keys().count(); i++)
        delete contextMenu[contextMenu.keys()[i]];
}


int TrayIcon::showInformation()
{
    if (debug) qDebug() << PDEBUG;

    if (supportsMessages()) {
        QString title = QApplication::translate("TrayIcon", "netctl status");
        QStringList info = mainWindow->printInformation();
        QString message = QString("%1: %2\n").arg(QApplication::translate("TrayIcon", "Profile")).arg(info[0]);
        message += QString("%1: %2").arg(QApplication::translate("TrayIcon", "Status")).arg(info[1]);
        showMessage(title, message, QSystemTrayIcon::Information);
    }
    else
        return showInformationInWindow();
    return 0;
}


int TrayIcon::showInformationInWindow()
{
    if (debug) qDebug() << PDEBUG;

    QString title = QApplication::translate("TrayIcon", "netctl status");
    QStringList info = mainWindow->printInformation();
    QString message = QString("%1: %2\n").arg(QApplication::translate("TrayIcon", "Profile")).arg(info[0]);
    message += QString("%1: %2").arg(QApplication::translate("TrayIcon", "Status")).arg(info[1]);

    return QMessageBox::information(0, title, message);
}


void TrayIcon::updateMenu()
{
    if (debug) qDebug() << PDEBUG;

    QStringList info = mainWindow->printTrayInformation();
    bool netctlAutoStatus = info[0].toInt();
    QStringList profiles = info[1].split(QChar('|'));
    QString current = info[2];
    QString enabled = info[3];

    if (current.isEmpty()) {
        contextMenu[QString("title")]->setIcon(QIcon(QString(":network-offline-64x64")));
        contextMenu[QString("title")]->setText(QApplication::translate("TrayIcon", "(inactive)"));
    } else {
        contextMenu[QString("title")]->setIcon(QIcon(QString(":network-idle-64x64")));
        QStringList status;
        if (netctlAutoStatus)
            status.append(QApplication::translate("TrayIcon", "(netctl-auto)"));
        else {
            for (int i=0; i<enabled.split(QChar('|')).count(); i++)
                if (enabled.split(QChar('|'))[i] == QString("0"))
                    status.append(QApplication::translate("TrayIcon", "static"));
                else
                    status.append(QApplication::translate("TrayIcon", "enabled"));
        }
        contextMenu[QString("title")]->setText(current + QString(" ") +
                                               QString("(") + status.join(QChar('|')) + QString(")"));
    }

    if (netctlAutoStatus) {
        contextMenu[QString("start")]->setVisible(false);
        contextMenu[QString("stop")]->setVisible(false);
        contextMenu[QString("stopall")]->setVisible(false);
        contextMenu[QString("switch")]->setVisible(true);
        contextMenu[QString("restart")]->setVisible(false);
        contextMenu[QString("enable")]->setVisible(false);
        switchToProfileMenu->clear();
        for (int i=0; i<profiles.count(); i++) {
            QAction *profile = new QAction(profiles[i], this);
            switchToProfileMenu->addAction(profile);
        }
    } else {
        if (current.contains(QChar('|'))) {
            contextMenu[QString("start")]->setVisible(true);
            contextMenu[QString("stop")]->setVisible(false);
            contextMenu[QString("stopall")]->setVisible(true);
            contextMenu[QString("switch")]->setVisible(false);
            contextMenu[QString("restart")]->setVisible(false);
            contextMenu[QString("enable")]->setVisible(false);
        } else {
            contextMenu[QString("start")]->setVisible(true);
            contextMenu[QString("stop")]->setVisible(!current.isEmpty());
            contextMenu[QString("switch")]->setVisible(false);
            contextMenu[QString("stopall")]->setVisible(false);
            contextMenu[QString("restart")]->setVisible(!current.isEmpty());
            contextMenu[QString("enable")]->setVisible(!current.isEmpty());
        }
        if (!current.isEmpty()) {
            contextMenu[QString("start")]->setText(QApplication::translate("TrayIcon", "Start another profile"));
            contextMenu[QString("stop")]->setText(QApplication::translate("TrayIcon", "Stop %1").arg(current));
            contextMenu[QString("restart")]->setText(QApplication::translate("TrayIcon", "Restart %1").arg(current));
            if (enabled.split(QChar('|'))[0].toInt())
                contextMenu[QString("enable")]->setText(QApplication::translate("TrayIcon", "Disable %1").arg(current));
            else
                contextMenu[QString("enable")]->setText(QApplication::translate("TrayIcon", "Enable %1").arg(current));
        } else
            contextMenu[QString("start")]->setText(QApplication::translate("TrayIcon", "Start profile"));
        startProfileMenu->clear();
        for (int i=0; i<profiles.count(); i++) {
            QAction *profile = new QAction(profiles[i], this);
            startProfileMenu->addAction(profile);
        }
    }

    if (mainWindow->isHidden())
        contextMenu[QString("gui")]->setText(QApplication::translate("TrayIcon", "Show"));
    else
        contextMenu[QString("gui")]->setText(QApplication::translate("TrayIcon", "Hide"));
}


void TrayIcon::createActions()
{
    if (debug) qDebug() << PDEBUG;

    menuActions = new QMenu();

    contextMenu[QString("title")] = new QAction(QIcon(":icon"), QApplication::translate("TrayIcon", "Status"), this);
    menuActions->addAction(contextMenu[QString("title")]);

    menuActions->addSeparator();

    contextMenu[QString("start")] = new QAction(QIcon::fromTheme("system-run"), QApplication::translate("TrayIcon", "Start profile"), this);
    startProfileMenu = new QMenu();
    contextMenu[QString("start")]->setMenu(startProfileMenu);
    connect(startProfileMenu, SIGNAL(triggered(QAction *)), this, SLOT(startProfileSlot(QAction *)));
    menuActions->addAction(contextMenu[QString("start")]);

    contextMenu[QString("stop")] = new QAction(QIcon::fromTheme("process-stop"), QApplication::translate("TrayIcon", "Stop profile"), this);
    connect(contextMenu[QString("stop")], SIGNAL(triggered(bool)), this, SLOT(stopProfileSlot()));
    menuActions->addAction(contextMenu[QString("stop")]);

    contextMenu[QString("stopall")] = new QAction(QIcon::fromTheme("process-stop"), QApplication::translate("TrayIcon", "Stop all profiles"), this);
    connect(contextMenu[QString("stopall")], SIGNAL(triggered(bool)), this, SLOT(stopAllProfilesSlot()));
    menuActions->addAction(contextMenu[QString("stopall")]);

    contextMenu[QString("switch")] = new QAction(QIcon::fromTheme("system-run"), QApplication::translate("TrayIcon", "Switch to profile"), this);
    switchToProfileMenu = new QMenu();
    contextMenu[QString("switch")]->setMenu(switchToProfileMenu);
    connect(switchToProfileMenu, SIGNAL(triggered(QAction *)), this, SLOT(switchToProfileSlot(QAction *)));
    menuActions->addAction(contextMenu[QString("switch")]);

    contextMenu[QString("restart")] = new QAction(QIcon::fromTheme("stock-refresh"), QApplication::translate("TrayIcon", "Restart profile"), this);
    connect(contextMenu[QString("restart")], SIGNAL(triggered(bool)), this, SLOT(restartProfileSlot()));
    menuActions->addAction(contextMenu[QString("restart")]);

    contextMenu[QString("enable")] = new QAction(QApplication::translate("TrayIcon", "Enable profile"), this);
    connect(contextMenu[QString("enable")], SIGNAL(triggered(bool)), this, SLOT(enableProfileSlot()));
    menuActions->addAction(contextMenu[QString("enable")]);

    menuActions->addSeparator();

    contextMenu[QString("gui")] = new QAction(QApplication::translate("TrayIcon", "Show"), this);
    connect(contextMenu[QString("gui")], SIGNAL(triggered(bool)), mainWindow, SLOT(showMainWindow()));
    menuActions->addAction(contextMenu[QString("gui")]);

    contextMenu[QString("auto")] = new QAction(QApplication::translate("TrayIcon", "Show netctl-auto"), this);
    connect(contextMenu[QString("auto")], SIGNAL(triggered(bool)), mainWindow, SLOT(showNetctlAutoWindow()));
    menuActions->addAction(contextMenu[QString("auto")]);

    menuActions->addSeparator();

    contextMenu[QString("quit")] = new QAction(QApplication::translate("TrayIcon", "Quit"), this);
    connect(contextMenu[QString("quit")], SIGNAL(triggered(bool)), mainWindow, SLOT(closeMainWindow()));
    menuActions->addAction(contextMenu[QString("quit")]);
}


void TrayIcon::init()
{
    if (debug) qDebug() << PDEBUG;

    setIcon(QIcon(":icon"));
    setToolTip(QString("netctl-gui"));

    createActions();
    setContextMenu(menuActions);

    connect(this, SIGNAL(messageClicked()), this, SLOT(showInformationInWindow()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(itemActivated(QSystemTrayIcon::ActivationReason)));
}


void TrayIcon::itemActivated(const QSystemTrayIcon::ActivationReason reason)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Reason" << reason;

    switch (reason) {
    case QSystemTrayIcon::Trigger:
        showInformation();
        break;
    case QSystemTrayIcon::DoubleClick:
        mainWindow->showMainWindow();
        break;
    case QSystemTrayIcon::Context:
        updateMenu();
        break;
    default:
        break;
    }
}


bool TrayIcon::enableProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = mainWindow->printInformation()[0];

    return mainWindow->enableProfileSlot(profile);
}


bool TrayIcon::restartProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = mainWindow->printInformation()[0];

    return mainWindow->restartProfileSlot(profile);
}


bool TrayIcon::startProfileSlot(QAction *action)
{
    if (debug) qDebug() << PDEBUG;

    QString profile = action->text().remove(QChar('&'));

    return mainWindow->switchToProfileSlot(profile);
}


bool TrayIcon::stopProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = mainWindow->printInformation()[0];

    return mainWindow->startProfileSlot(profile);
}


bool TrayIcon::stopAllProfilesSlot()
{
    if (debug) qDebug() << PDEBUG;

    return mainWindow->stopAllProfilesSlot();
}


bool TrayIcon::switchToProfileSlot(QAction *action)
{
    if (debug) qDebug() << PDEBUG;

    QString profile = action->text().remove(QChar('&'));

    return mainWindow->switchToProfileSlot(profile);
}
