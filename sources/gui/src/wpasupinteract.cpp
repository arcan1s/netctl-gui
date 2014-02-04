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

#include "wpasupinteract.h"

#include "mainwindow.h"
#include <cstdio>


WpaSup::WpaSup(MainWindow *wid, QString wpaCliPath, QString ifaceDir)
    : parent(wid),
      wpaCliCommand(wpaCliPath),
      ifaceDirectory(new QDir(ifaceDir))
{

}


WpaSup::~WpaSup()
{
    delete ifaceDirectory;
}


// general information
QStringList WpaSup::getInterfaceList()
{
    QStringList interfaces;
    QStringList allInterfaces;

    allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i=0; i<allInterfaces.count(); i++)
        if (QDir(ifaceDirectory->path() + QDir::separator() + allInterfaces[i] +
                 QDir::separator() + QString("wireless")).exists())
            interfaces.append(allInterfaces[i]);

    return interfaces;
}
