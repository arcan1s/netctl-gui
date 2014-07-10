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

#include "errorwindow.h"
#include "ui_errorwindow.h"

#include <QDebug>


ErrorWindow::ErrorWindow(QWidget *parent, const bool debugCmd, const int messageNumber)
    : QMainWindow(parent),
      debug(debugCmd),
      ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
    if (debug) qDebug() << "[ErrorWindow]" << "[ErrorWindow]" << ":" << "Initializate with error code" << messageNumber;
    setMessage(messageNumber);
}

ErrorWindow::~ErrorWindow()
{
    delete ui;
}

void ErrorWindow::setMessage(const int mess)
{
    QString message;
    switch(mess) {
    case 1:
        message = QApplication::translate("ErrorWindow", "Could not find components");
        break;
    case 2:
        message = QApplication::translate("ErrorWindow", "Does not support yet");
        break;
    case 3:
        message = QApplication::translate("ErrorWindow", "Profile name is not set");
        break;
    case 4:
        message = QApplication::translate("ErrorWindow", "Bind interfaces are not set");
        break;
    case 5:
        message = QApplication::translate("ErrorWindow", "Description is not set");
        break;
    case 6:
        message = QApplication::translate("ErrorWindow", "Ip settings are not set");
        break;
    case 7:
        message = QApplication::translate("ErrorWindow", "Configuration file does not exist");
        break;
    case 8:
        message = QApplication::translate("ErrorWindow", "Key is not set");
        break;
    case 9:
        message = QApplication::translate("ErrorWindow", "Wireless settings are not set");
        break;
    case 10:
        message = QApplication::translate("ErrorWindow", "Configuration file is not set");
        break;
    case 11:
        message = QApplication::translate("ErrorWindow", "ESSID is not set");
        break;
    case 12:
        message = QApplication::translate("ErrorWindow", "MAC address is incorrect");
        break;
    case 13:
        message = QApplication::translate("ErrorWindow", "Session ID is not set");
        break;
    case 14:
        message = QApplication::translate("ErrorWindow", "APN is not set");
        break;
    case 15:
        message = QApplication::translate("ErrorWindow", "APN is not set");
        break;
    case 16:
        message = QApplication::translate("ErrorWindow", "Empty user/group name");
        break;
    default:
        message = QApplication::translate("ErrorWindow", "Unknown error");
        break;
    }

    ui->label->setText(message);
}
