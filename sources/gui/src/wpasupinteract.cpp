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

#include <QProcess>

#include "mainwindow.h"
#include "sleepthread.h"
#include <cstdio>


WpaSup::WpaSup(MainWindow *wid, QStringList wpaConfig, QString sudoPath, QString ifaceDir, QString preferedInterface)
    : parent(wid),
      wpaConf(wpaConfig),
      sudoCommand(sudoPath),
      ifaceDirectory(new QDir(ifaceDir)),
      mainInterface(preferedInterface)
{
    // remove old files if they exist
    if (QFile(wpaConf[2]).exists() || QDir(wpaConf[4]).exists()) {
        QProcess command;
        command.start(sudoCommand + QString(" /usr/bin/rm -f ") + wpaConf[2] + QString(" ") + wpaConf[4]);
        command.waitForFinished(-1);
    }
}


WpaSup::~WpaSup()
{
    delete ifaceDirectory;
}


// general information
QStringList WpaSup::getInterfaceList()
{
    QStringList interfaces;

    if (mainInterface.isEmpty()) {
        QStringList allInterfaces;
        allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (int i=0; i<allInterfaces.count(); i++)
            if (QDir(ifaceDirectory->path() + QDir::separator() + allInterfaces[i] +
                     QDir::separator() + QString("wireless")).exists())
                interfaces.append(allInterfaces[i]);
    }
    else
        interfaces.append(mainInterface);

    return interfaces;
}


// functions
bool WpaSup::wpaCliCall(QString commandLine)
{
    QString interface = getInterfaceList()[0];
    QProcess command;
    command.start(wpaConf[0] + QString(" -i ") + interface + QString(" -p ") + wpaConf[4] +
            QString(" ") + commandLine);
    command.waitForFinished(-1);
    SleepThread::sleep(1);
    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


QString WpaSup::getWpaCliOutput(QString commandLine)
{
    QString interface = getInterfaceList()[0];
    QProcess command;
    command.start(wpaConf[0] + QString(" -i ") + interface + QString(" -p ") + wpaConf[4] +
            QString(" ") + commandLine);
    command.waitForFinished(-1);
    return command.readAllStandardOutput();
}


bool WpaSup::startWpaSupplicant()
{
    if (!QFile(wpaConf[2]).exists()) {
        QString interface = getInterfaceList()[0];
        QProcess command;
        command.start(sudoCommand + QString(" ") + wpaConf[1] + QString(" -B -P ") + wpaConf[2] +
                QString(" -i ") + interface + QString(" -D ") + wpaConf[3] +
                QString(" -C \"DIR=") + wpaConf[4] + QString(" GROUP=") + wpaConf[5]);
        command.waitForFinished(-1);
        SleepThread::sleep(1);
        if (command.exitCode() != 0)
            return false;
    }
    return true;
}


bool WpaSup::stopWpaSupplicant()
{
    return wpaCliCall(QString("terminate"));
}


QList<QStringList> WpaSup::scanWifi()
{
    QList<QStringList> scanResults;
    startWpaSupplicant();
    if (!wpaCliCall(QString("scan")))
        return scanResults;
    SleepThread::sleep(3);

    QStringList rawOutput = getWpaCliOutput(QString("scan_results")).split(QString("\n"));
    rawOutput.removeFirst();
    rawOutput.removeLast();
    for (int i=0; i<rawOutput.count()-1; i++)
        if (rawOutput[i].split(QString(" "), QString::SkipEmptyParts).count() > 4)
            for (int j=i+1; j<rawOutput.count(); j++)
                if (rawOutput[j].split(QString(" "), QString::SkipEmptyParts).count() > 4)
                    if (rawOutput[i].split(QString(" "), QString::SkipEmptyParts)[4] ==
                            rawOutput[j].split(QString(" "), QString::SkipEmptyParts)[4])
                        rawOutput.removeAt(j);

    for (int i=0; i<rawOutput.count(); i++) {
        QStringList wifiPoint;

        // point name
        if (rawOutput[i].split(QString("\t"), QString::SkipEmptyParts).count() > 4)
            wifiPoint.append(rawOutput[i].split(QString("\t"), QString::SkipEmptyParts)[4]);
        else
            wifiPoint.append(QString("<hidden>"));
        // point signal
        wifiPoint.append(rawOutput[i].split(QString("\t"), QString::SkipEmptyParts)[2]);
        // point security
        QString security = rawOutput[i].split(QString("\t"), QString::SkipEmptyParts)[3];
        if (security.indexOf(QString("WPA2")))
            security = QString("WPA2");
        else if (security.indexOf(QString("WPA")))
            security = QString("WPA");
        else if (security.indexOf(QString("WEP")))
            security = QString("WEP");
        else
            security = QString("none");
        wifiPoint.append(security);
        // point bssid
        wifiPoint.append(rawOutput[i].split(QString("\t"), QString::SkipEmptyParts)[0]);
        // profile existance

        scanResults.append(wifiPoint);
    }

    stopWpaSupplicant();
    return scanResults;
}
