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

#include "netctlautowindow.h"
#include "ui_netctlautowindow.h"

#include <QDebug>

#include <netctlgui/netctlinteract.h>


NetctlAutoWindow::NetctlAutoWindow(QWidget *parent, const bool debugCmd, const QMap<QString, QString> settings)
    : QMainWindow(parent),
      ui(new Ui::NetctlAutoWindow),
      debug(debugCmd)
{
    ui->setupUi(this);
    netctlCommand = new Netctl(debug, settings);
}


NetctlAutoWindow::~NetctlAutoWindow()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[~NetctlAutoWindow]";

    delete ui;
}


void NetctlAutoWindow::createActions()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[createActions]";

//    connect(ui->comboBox_language, SIGNAL(currentIndexChanged(int)), ui->label_info, SLOT(show()));
//    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(close()));
//    connect(ui->buttonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked(bool)), this, SLOT(setDefault()));
//    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
//    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(close()));
//    // buttons
//    connect(ui->pushButton_interfaceDir, SIGNAL(clicked(bool)), SLOT(selectIfaceDir()));
//    connect(ui->pushButton_netctlPath, SIGNAL(clicked(bool)), SLOT(selectNetctlPath()));
//    connect(ui->pushButton_profilePath, SIGNAL(clicked(bool)), SLOT(selectProfileDir()));
//    connect(ui->pushButton_rfkill, SIGNAL(clicked(bool)), SLOT(selectRfkillDir()));
//    connect(ui->pushButton_sudo, SIGNAL(clicked(bool)), SLOT(selectSudoPath()));
//    connect(ui->pushButton_wpaCliPath, SIGNAL(clicked(bool)), SLOT(selectWpaCliPath()));
//    connect(ui->pushButton_wpaSupPath, SIGNAL(clicked(bool)), SLOT(selectWpaSupPath()));
}


// ESC press event
void NetctlAutoWindow::keyPressEvent(QKeyEvent *pressedKey)
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[keyPressEvent]";

    if (pressedKey->key() == Qt::Key_Escape)
        close();
}


void NetctlAutoWindow::showWindow()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[showWindow]";

    clear();
    appendActiveProfiles();
    appendAvailableProfiles();
    show();
}


void NetctlAutoWindow::appendActiveProfiles()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[appendActiveProfiles]";


}


void NetctlAutoWindow::appendAvailableProfiles()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[appendAvailableProfiles]";

    QList<QStringList> profiles = netctlCommand->getProfileList();
    for (int i=0; i<profiles.count(); i++)
        ui->listWidget_available->addItem(profiles[i][0]);
}


void NetctlAutoWindow::clear()
{
    if (debug) qDebug() << "[NetctlAutoWindow]" << "[clear]";

    ui->listWidget_available->setCurrentRow(-1);
    ui->listWidget_available->clear();
    ui->listWidget_active->clear();
    ui->listWidget_active->setCurrentRow(-1);
}
