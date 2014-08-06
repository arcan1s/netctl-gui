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

#include "mainwindow.h"
#include "netctlguiadaptor.h"


NetctlGuiAdaptor::NetctlGuiAdaptor(MainWindow *parent, const bool debugCmd)
    : QDBusAbstractAdaptor(parent),
      debug(debugCmd),
      mainWindow(parent)
{
}


NetctlGuiAdaptor::~NetctlGuiAdaptor()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[~NetctlGuiAdaptor]";
}


bool NetctlGuiAdaptor::Close()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[Close]";

    mainWindow->closeMainWindow();
    return true;
}


QString NetctlGuiAdaptor::Information()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[Information]";

    return mainWindow->getInformation();
}


bool NetctlGuiAdaptor::RestoreWindow()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[RestoreWindow]";

    mainWindow->show();
    return true;
}


QStringList NetctlGuiAdaptor::Settings()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[Settings]";

    return mainWindow->getSettings();
}


bool NetctlGuiAdaptor::ShowAbout()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[ShowAbout]";

    mainWindow->showAboutWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowMain()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[ShowMain]";

    mainWindow->showMainWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowNetctlAuto()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[ShowNetctlAuto]";

    mainWindow->showNetctlAutoWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowSettings()
{
    if (debug) qDebug() << "[NetctlGuiAdaptor]" << "[ShowSettings]";

    mainWindow->showSettingsWindow();
    return true;
}
