/***************************************************************************
 *   This file is part of netctl-plasmoid                                  *
 *                                                                         *
 *   netctl-plasmoid is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-plasmoid is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-plasmoid. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "errorwindow.h"
#include "ui_errorwindow.h"


ErrorWindow::ErrorWindow(QWidget *parent, int messageNumber)
    : QMainWindow(parent),
      ui(new Ui::ErrorWindow)
{
    ui->setupUi(this);
    setMessage(messageNumber);
}

ErrorWindow::~ErrorWindow()
{
    delete ui;
}

void ErrorWindow::setMessage(int mess)
{
    QString message;
    switch(mess) {
    case 0:
        message = QApplication::translate("ErrorWindow", "Unknown error");
        break;
    case 1:
        message = QApplication::translate("ErrorWindow", "Could not find components");
        break;
    case 2:
        message = QApplication::translate("ErrorWindow", "Doesn't support yet");
        break;
    default:
        message = QApplication::translate("ErrorWindow", "Unknown error");
        break;
    }

    ui->label->setText(message);
}
