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

#include "netctlinteract.h"

#include <QFile>
#include <QProcess>

#include "mainwindow.h"
#include "cstdio"


Netctl::Netctl(MainWindow *wid, QString netctlPath, QString profileDir, QString sudoPath)
    : parent(wid),
      netctlCommand(netctlPath),
      profileDirectory(new QDir(profileDir)),
      sudoCommand(sudoPath)
{

}


Netctl::~Netctl()
{
    delete profileDirectory;
}


// general information
QStringList Netctl::getProfileDescriptions(QStringList profileList)
{
    QStringList descriptions;

    for (int i=0; i<profileList.count(); i++) {
        QFile profile(profileDirectory->absolutePath() + QDir::separator() + profileList[i]);
        QString fileStr;
        if (profile.open(QIODevice::ReadOnly))
            while (true) {
                fileStr = QString(profile.readLine());
                if (profile.atEnd())
                    break;
                else if (fileStr[0] != '#')
                    if (fileStr.split(QString("="), QString::SkipEmptyParts).count() == 2)
                        if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("Description"))
                            descriptions.append(fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0]);
            }
        else
            descriptions.append(QString("<unknown>"));
    }

    for (int i=0; i<profileList.count(); i++) {
        descriptions[i].remove(QChar('\''));
        descriptions[i].remove(QChar('"'));
    }

    return descriptions;
}


QStringList Netctl::getProfileList()
{
    return profileDirectory->entryList(QDir::Files);
}


QStringList Netctl::getProfileStatuses(QStringList profileList)
{
    QStringList statuses;

    for (int i=0; i<profileList.count(); i++) {
        QString status = QString("");
        if (isProfileActive(profileList[i]))
            status = status + QString("active");
        else
            status = status + QString("inactive");
        if (isProfileEnabled(profileList[i]))
            status = status + QString(" (enabled)");
        else
            status = status + QString(" (static)");
        statuses.append(status);
    }

    return statuses;
}


bool Netctl::isProfileActive(QString profile)
{
    bool status = false;
    QProcess command;
    QString cmdOutput = QString("");

    command.start(netctlCommand + QString(" status ") + profile);
    command.waitForFinished(-1);
    cmdOutput = command.readAllStandardOutput();
    if (!cmdOutput.isEmpty())
        if (cmdOutput.indexOf(QString("Active: active")) > -1)
            status = true;

    return status;
}


bool Netctl::isProfileEnabled(QString profile)
{
    bool status = false;
    QProcess command;
    QString cmdOutput = QString("");

    command.start(netctlCommand + QString(" status ") + profile);
    command.waitForFinished(-1);
    cmdOutput = command.readAllStandardOutput();
    if (!cmdOutput.isEmpty()) {
        QStringList profileStatus = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
        for (int i=0; i<profileStatus.count(); i++)
            if (profileStatus[i].split(QString(" "), QString::SkipEmptyParts)[0] == QString("Loaded:"))
                if (profileStatus[i].indexOf(QString("enabled")) > -1)
                    status = true;
    }

    return status;
}


// functions
bool Netctl::enableProfile(QString profile)
{
    QProcess command;
    if (isProfileEnabled(profile))
        command.start(sudoCommand + QString(" ") + netctlCommand + QString(" disable ") + profile);
    else
        command.start(sudoCommand + QString(" ") + netctlCommand + QString(" enable ") + profile);
    command.waitForFinished(-1);
    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


bool Netctl::restartProfile(QString profile)
{
    QProcess command;
    if (isProfileActive(profile))
        command.start(sudoCommand + QString(" ") + netctlCommand + QString(" restart ") + profile);
    command.waitForFinished(-1);
    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


bool Netctl::startProfile(QString profile)
{
    QProcess command;
    if (isProfileActive(profile))
        command.start(sudoCommand + QString(" ") + netctlCommand + QString(" stop ") + profile);
    else
        command.start(sudoCommand + QString(" ") + netctlCommand + QString(" start ") + profile);
    command.waitForFinished(-1);
    if (command.exitCode() == 0)
        return true;
    else
        return false;
}
