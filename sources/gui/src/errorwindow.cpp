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

#include <QApplication>
#include <QGridLayout>
#include <QSpacerItem>
#include <QDebug>

#include <pdebug/pdebug.h>


ErrorWindow::ErrorWindow(QWidget *parent, const bool debugCmd)
    : debug(debugCmd)
{
}


ErrorWindow::~ErrorWindow()
{
    if (debug) qDebug() << PDEBUG;
}


QStringList ErrorWindow::getMessage(const int mess)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << mess;

    QString message, title;
    switch(mess) {
    case 1:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Could not find components");
        break;
    case 2:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Does not support yet");
        break;
    case 3:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Profile name is not set");
        break;
    case 4:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Bind interfaces are not set");
        break;
    case 5:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Description is not set");
        break;
    case 6:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Ip settings are not set");
        break;
    case 7:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Configuration file does not exist");
        break;
    case 8:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Key is not set");
        break;
    case 9:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Wireless settings are not set");
        break;
    case 10:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Configuration file is not set");
        break;
    case 11:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "ESSID is not set");
        break;
    case 12:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "MAC address is incorrect");
        break;
    case 13:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Session ID is not set");
        break;
    case 14:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "User is not in network group, helper will not be started");
        break;
    case 15:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "APN is not set");
        break;
    case 16:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Empty user/group name");
        break;
    case 17:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Could not find profile");
        break;
    case 18:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Could not find ESSID");
        break;
    case 19:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Could not run helper");
        break;
    case 20:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "IP address does not match the standard");
        break;
    default:
        title = QApplication::translate("ErrorWindow", "Error!");
        message = QApplication::translate("ErrorWindow", "Unknown error");
        break;
    }

    QStringList fullMessage;
    fullMessage.append(title);
    fullMessage.append(message);
    return fullMessage;
}


QMessageBox::Icon ErrorWindow::getIcon(const int mess)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << mess;

    QMessageBox::Icon icon;
    switch(mess) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
        icon = QMessageBox::Critical;
        break;
    default:
        icon = QMessageBox::NoIcon;
        break;
    }

    return icon;
}


void ErrorWindow::showWindow(const int mess, const QString sender, const bool debugCmd)
{
    if (debugCmd) qDebug() << PDEBUG;
    if (debugCmd) qDebug() << PDEBUG << ":" << "Message" << mess;
    if (debugCmd) qDebug() << PDEBUG << ":" << "Sender" << sender;

    ErrorWindow *errorWin = new ErrorWindow(0, debugCmd);
    QStringList message = errorWin->getMessage(mess);
    QMessageBox messageBox;
    messageBox.setText(message[0]);
    messageBox.setInformativeText(message[1]);
    if (debugCmd)
        messageBox.setDetailedText(QApplication::translate("ErrorWindow", "Sender : %1").
                                   arg(sender));
    messageBox.setIcon(errorWin->getIcon(mess));
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.setDefaultButton(QMessageBox::Ok);

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)messageBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    messageBox.exec();
    delete errorWin;
}
