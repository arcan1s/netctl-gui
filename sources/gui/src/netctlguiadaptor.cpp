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

#include <QTextCodec>
#include <unistd.h>

#include "mainwindow.h"


NetctlGuiAdaptor::NetctlGuiAdaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent),
      mainWindow(parent)
{
}


NetctlGuiAdaptor::~NetctlGuiAdaptor()
{
}


bool NetctlGuiAdaptor::Active()
{
    return true;
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


void NetctlGuiAdaptor::LibraryDocs()
{
    return mainWindow->showLibrary();
}


QString NetctlGuiAdaptor::Pony()
{
    QString pony;
    QFile ponyFile(QString(":pinkiepie"));
    if (!ponyFile.open(QIODevice::ReadOnly)) return pony;
    pony = QTextCodec::codecForMib(106)->toUnicode(ponyFile.readAll());
    ponyFile.close();

    return pony;
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


QStringList NetctlGuiAdaptor::UIDs()
{
    QStringList uids;
    uids.append(QString::number(getuid()));
    uids.append(QString::number(geteuid()));

    return uids;
}
