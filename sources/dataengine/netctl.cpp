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

#include "netctl.h"

#include <Plasma/DataContainer>
#include <QDir>
#include <QFile>
#include <QProcess>


Netctl::Netctl(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    setMinimumPollingInterval(333);
    readConfiguration();
}


QStringList Netctl::sources() const
{
    QStringList sources;

    sources.append(QString("currentProfile"));
    sources.append(QString("extIp"));
    sources.append(QString("interfaces"));
    sources.append(QString("intIp"));
    sources.append(QString("profiles"));
    sources.append(QString("statusBool"));
    sources.append(QString("statusString"));

    return sources;
}


bool Netctl::readConfiguration()
{
    // default configuration
    configuration[QString("CMD")] = QString("/usr/bin/netctl");
    configuration[QString("EXTIP")] = QString("false");
    configuration[QString("EXTIPCMD")] = QString("wget -qO- http://ifconfig.me/ip");
    configuration[QString("IPCMD")] = QString("/usr/bin/ip");
    configuration[QString("NETDIR")] = QString("/sys/class/net/");

    QString fileStr;
    // FIXME: define configuration file
    QFile confFile(QString(getenv("HOME")) + QString("/.kde4/share/config/netctl.conf"));
    bool exists = confFile.open(QIODevice::ReadOnly);
    if (!exists) {
        confFile.setFileName("/usr/share/config/netctl.conf");
        exists = confFile.open(QIODevice::ReadOnly);
        if (!exists)
            return false;
    }

    while (true) {
        fileStr = QString(confFile.readLine());
        if (fileStr[0] != '#') {
            if (fileStr.contains(QString("=")))
                configuration[fileStr.split(QString("="))[0]] = fileStr.split(QString("="))[1]
                      .remove(QString(" "))
                      .trimmed();
        }
        if (confFile.atEnd())
            break;
    }

    confFile.close();
    return true;
}


bool Netctl::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}


bool Netctl::updateSourceEvent(const QString &source)
{
    QProcess command;
    QString cmdOutput = QString("");
    QString value = QString("");

    if (source == QString("currentProfile")) {
        command.start(configuration[QString("CMD")] + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        if (!cmdOutput.isEmpty()) {
            QStringList profileList = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profileList.count(); i++)
                if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 2) {
                    value = profileList[i].split(QString(" "), QString::SkipEmptyParts)[1];
                    break;
                }
        }
        setData(source, QString("value"), value);
    }
    else if (source == QString("extIp")) {
        if (configuration[QString("EXTIP")] == QString("true")) {
            command.start(configuration[QString("EXTIPCMD")]);
            command.waitForFinished(-1);
            cmdOutput = command.readAllStandardOutput();
            if (!cmdOutput.isEmpty())
                value = cmdOutput.split(QString("\n"), QString::SkipEmptyParts)[0];
        }
        setData(source, QString("value"), value);
    }
    else if (source == QString("interfaces")) {
        if (QDir(configuration[QString("NETDIR")]).exists())
            value = QDir(configuration[QString("NETDIR")]).entryList(QDir::Dirs | QDir::NoDotAndDotDot).join(QString(","));
        setData(source, QString("value"), value);
    }
    else if (source == QString("intIp")) {
        if (QDir(configuration[QString("NETDIR")]).exists()) {
            value = QString("127.0.0.1/8");
            QStringList netDevices = QDir(configuration[QString("NETDIR")]).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (int i=0; i<netDevices.count(); i++)
                if (netDevices[i] != QString("lo")) {
                    cmdOutput = QString("");
                    command.start(configuration[QString("IPCMD")] + QString(" addr show ") + netDevices[i]);
                    command.waitForFinished(-1);
                    cmdOutput = command.readAllStandardOutput();
                    if (!cmdOutput.isEmpty()) {
                        QStringList deviceInfo = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
                        for (int j=0; j<deviceInfo.count(); j++)
                            if (deviceInfo[j].split(QString(" "), QString::SkipEmptyParts)[0] == QString("inet"))
                                value = deviceInfo[j].split(QString(" "), QString::SkipEmptyParts)[1];
                    }
                }
        }
        setData(source, QString("value"), value);
    }
    else if (source == QString("profiles")) {
        command.start(configuration[QString("CMD")] + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        QStringList list;
        if (!cmdOutput.isEmpty()) {
            QStringList profileList = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profileList.count(); i++)
                if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 1)
                    list.append(profileList[i].split(QString(" "), QString::SkipEmptyParts)[0]);
                else if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 2)
                    list.append(profileList[i].split(QString(" "), QString::SkipEmptyParts)[1]);
        }
        value = list.join(QString(","));
        setData(source, QString("value"), value);
    }
    else if (source == QString("statusBool")) {
        command.start(configuration[QString("CMD")] + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        value = QString("false");
        if (!cmdOutput.isEmpty()) {
            QStringList profileList = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profileList.count(); i++)
                if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 2) {
                    value = QString("true");
                    break;
                }
        }
        setData(source, QString("value"), value);
    }
    else if (source == QString("statusString")) {
        command.start(configuration[QString("CMD")] + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        QString currentProfile;
        if (!cmdOutput.isEmpty()) {
            QStringList profileList = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profileList.count(); i++)
                if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 2) {
                    currentProfile = profileList[i].split(QString(" "), QString::SkipEmptyParts)[1];
                    break;
                }
        }
        command.start(configuration[QString("CMD")] + QString(" status ") + currentProfile);
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        if (!cmdOutput.isEmpty()) {
            QStringList profile = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profile.count(); i++)
                if (profile[i].split(QString(" "), QString::SkipEmptyParts)[0] == QString("Loaded:")) {
                    if (profile[i].contains(QString("enabled")))
                        value = QString("enabled");
                    else if (profile[i].contains(QString("static")))
                        value = QString("static");
                    break;
                }
        }
        setData(source, QString("value"), value);
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
