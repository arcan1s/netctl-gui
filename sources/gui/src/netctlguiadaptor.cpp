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

#include "netctlguiadaptor.h"

#include "mainwindow.h"


NetctlGuiAdaptor::NetctlGuiAdaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent),
      mainWindow(parent)
{
}


NetctlGuiAdaptor::~NetctlGuiAdaptor()
{
}


void NetctlGuiAdaptor::ApiDocs()
{
    return mainWindow->showApi();
}


bool NetctlGuiAdaptor::Close()
{
    mainWindow->closeMainWindow();
    return true;
}


QString NetctlGuiAdaptor::Information()
{
    return mainWindow->printInformation();
}


void NetctlGuiAdaptor::LibraryDocs()
{
    return mainWindow->showLibrary();
}


bool NetctlGuiAdaptor::Restore()
{
    mainWindow->show();
    return true;
}


void NetctlGuiAdaptor::SecurityDocs()
{
    return mainWindow->showSecurityNotes();
}


QStringList NetctlGuiAdaptor::Settings()
{
    return mainWindow->printSettings();
}


bool NetctlGuiAdaptor::ShowAbout()
{
    mainWindow->showAboutWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowMain()
{
    mainWindow->showMainWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowNetctlAuto()
{
    mainWindow->showNetctlAutoWindow();
    return true;
}


bool NetctlGuiAdaptor::ShowSettings()
{
    mainWindow->showSettingsWindow();
    return true;
}
