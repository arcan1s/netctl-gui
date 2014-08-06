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


#include <QDebug>
#include <QMenu>
#include <QMessageBox>

#include "mainwindow.h"
#include "trayicon.h"


TrayIcon::TrayIcon(QObject *parent, const bool debugCmd)
    : QSystemTrayIcon(parent),
      debug(debugCmd)
{
    mainWindow = (MainWindow *)parent;

    init();
}


TrayIcon::~TrayIcon()
{
    if (debug) qDebug() << "[TrayIcon]" << "[~TrayIcon]";

    setContextMenu(0);
    delete exit;
    delete showMainWindow;
    delete showNetctlAutoWindow;
    delete showStatus;
}


int TrayIcon::showInformation()
{
    if (debug) qDebug() << "[TrayIcon]" << "[showInformation]";

    if (supportsMessages()) {
        QString title = QApplication::translate("TrayIcon", "netctl status");
        QString message = mainWindow->getInformation();
        showMessage(title, message, QSystemTrayIcon::Information);
    }
    else
        return showInformationInWindow();
    return 0;
}


int TrayIcon::showInformationInWindow()
{
    if (debug) qDebug() << "[TrayIcon]" << "[showInformationInWindow]";

    QString title = QApplication::translate("TrayIcon", "netctl status");
    QString message = mainWindow->getInformation();

    return QMessageBox::information(0, title, message);
}


void TrayIcon::init()
{
    if (debug) qDebug() << "[TrayIcon]" << "[init]";

    setIcon(QIcon(":icon"));
    setToolTip(QString("netctl-gui"));

    exit = new QAction(QIcon::fromTheme("exit"), QApplication::translate("TrayIcon", "Quit"), this);
    connect(exit, SIGNAL(triggered(bool)), mainWindow, SLOT(closeMainWindow()));
    showMainWindow = new QAction(QApplication::translate("TrayIcon", "Show"), this);
    connect(showMainWindow, SIGNAL(triggered(bool)), mainWindow, SLOT(showMainWindow()));
    showNetctlAutoWindow = new QAction(QApplication::translate("TrayIcon", "Show netctl-auto"), this);
    connect(showNetctlAutoWindow, SIGNAL(triggered(bool)),mainWindow, SLOT(showNetctlAutoWindow()));
    showStatus = new QAction(QIcon(":icon"), QApplication::translate("TrayIcon", "Status"), this);
    connect(showStatus, SIGNAL(triggered(bool)), this, SLOT(showInformation()));

    QMenu *menu = new QMenu();
    menu->addAction(showStatus);
    menu->addSeparator();
    menu->addAction(showMainWindow);
    menu->addAction(showNetctlAutoWindow);
    menu->addSeparator();
    menu->addAction(exit);
    setContextMenu(menu);

    connect(this, SIGNAL(messageClicked()), this, SLOT(showInformationInWindow()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(itemActivated(QSystemTrayIcon::ActivationReason)));
}


void TrayIcon::itemActivated(const QSystemTrayIcon::ActivationReason reason)
{
    if (debug) qDebug() << "[TrayIcon]" << "[itemActivated]";
    if (debug) qDebug() << "[TrayIcon]" << "[itemActivated]" << ":" << "Reason" << reason;

    switch (reason) {
    case QSystemTrayIcon::Trigger:
        showInformation();
        break;
    case QSystemTrayIcon::DoubleClick:
        mainWindow->showMainWindow();
        break;
    case QSystemTrayIcon::Context:
        if (mainWindow->isHidden())
            showMainWindow->setText(QApplication::translate("TrayIcon", "Show"));
        else
            showMainWindow->setText(QApplication::translate("TrayIcon", "Hide"));
        break;
    default:
        break;
    }
}
