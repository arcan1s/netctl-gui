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

#include "netctlprofile.h"

#include <QFile>

#include "mainwindow.h"


NetctlProfile::NetctlProfile(MainWindow *wid, QString profileDir, QString sudoPath)
    : parent(wid),
      profileDirectory(new QDir(profileDir)),
      sudoCommand(sudoPath)
{

}


NetctlProfile::~NetctlProfile()
{
    delete profileDirectory;
}


QHash<QString, QString> NetctlProfile::getSettingsFromProfile(QString profile)
{
    QHash<QString, QString> settings;
    QFile profileFile;
    QString fileStr;

    if (profile[0] == QDir::separator())
        profileFile.setFileName(profile);
    else
        profileFile.setFileName(profileDirectory->absolutePath() + QDir::separator() + profile);
    if (!profileFile.open(QIODevice::ReadOnly))
        return settings;
    while (true) {
        fileStr = QString(profileFile.readLine());
        if (profileFile.atEnd())
            break;
        else if (fileStr[0] != '#') {
            if (fileStr.split(QString("="), QString::SkipEmptyParts).count() == 2)
                settings[fileStr.split(QString("="))[0]] = fileStr.split(QString("="))[1]
                        .remove(QString("("))
                        .remove(QString(")"))
                        .remove(QString("\n"));
        }
    }

    profileFile.close();
    return settings;
}
