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


ErrorWindow::ErrorWindow(QWidget *parent, const bool debugCmd)
    : debug(debugCmd)
{
}


ErrorWindow::~ErrorWindow()
{
    if (debug) qDebug() << "[ErrorWindow]" << "[~ErrorWindow]";
}


QStringList ErrorWindow::getMessage(const int mess)
{
    if (debug) qDebug() << "[ErrorWindow]" << "[getMessage]";
    if (debug) qDebug() << "[ErrorWindow]" << "[getMessage]" << ":" << "Message" << mess;

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
        message = QApplication::translate("ErrorWindow", "APN is not set");
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
    if (debug) qDebug() << "[ErrorWindow]" << "[getIcon]";
    if (debug) qDebug() << "[ErrorWindow]" << "[getIcon]" << ":" << "Message" << mess;

    QMessageBox::Icon icon;
    switch(mess) {
    case 1:
        icon = QMessageBox::Critical;
        break;
    case 2:
        icon = QMessageBox::Critical;
        break;
    case 3:
        icon = QMessageBox::Critical;
        break;
    case 4:
        icon = QMessageBox::Critical;
        break;
    case 5:
        icon = QMessageBox::Critical;
        break;
    case 6:
        icon = QMessageBox::Critical;
        break;
    case 7:
        icon = QMessageBox::Critical;
        break;
    case 8:
        icon = QMessageBox::Critical;
        break;
    case 9:
        icon = QMessageBox::Critical;
        break;
    case 10:
        icon = QMessageBox::Critical;
        break;
    case 11:
        icon = QMessageBox::Critical;
        break;
    case 12:
        icon = QMessageBox::Critical;
        break;
    case 13:
        icon = QMessageBox::Critical;
        break;
    case 14:
        icon = QMessageBox::Critical;
        break;
    case 15:
        icon = QMessageBox::Critical;
        break;
    case 16:
        icon = QMessageBox::Critical;
        break;
    case 17:
        icon = QMessageBox::Critical;
        break;
    case 18:
        icon = QMessageBox::Critical;
        break;
    default:
        icon = QMessageBox::NoIcon;
        break;
    }

    return icon;
}


void ErrorWindow::showWindow(const int mess, const QString sender)
{
    if (debug) qDebug() << "[ErrorWindow]" << "[showWindow]";
    if (debug) qDebug() << "[ErrorWindow]" << "[showWindow]" << ":" << "Message" << mess;
    if (debug) qDebug() << "[ErrorWindow]" << "[showWindow]" << ":" << "Sender" << sender;

    QStringList message = getMessage(mess);
    QMessageBox messageBox;
    messageBox.setText(message[0]);
    messageBox.setInformativeText(message[1]);
    if (debug)
        messageBox.setDetailedText(QApplication::translate("ErrorWindow", "Sender : %1").
                                   arg(sender));
    messageBox.setIcon(getIcon(mess));
    messageBox.setStandardButtons(QMessageBox::Ok);
    messageBox.setDefaultButton(QMessageBox::Ok);

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = (QGridLayout *)messageBox.layout();
    layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    messageBox.exec();
    return;
}
