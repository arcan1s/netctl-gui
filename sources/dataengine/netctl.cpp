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
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.       *
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
    checkExtIP = QString("false");
    cmd = QString("/usr/bin/netctl");
    extIpCmd = QString("wget -qO- http://ifconfig.me/ip");
    ipCmd = QString("/usr/bin/ip");
    netDir = QString("/sys/class/net/");

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
        if (confFile.atEnd())
            break;
        else if (fileStr[0] != '#') {
            if (fileStr.split(QString("="), QString::SkipEmptyParts).count() == 2) {
                if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("EXTIP"))
                    checkExtIP = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
                else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("CMD"))
                    cmd = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
                else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("EXTIPCMD"))
                    extIpCmd = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
                else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("IPCMD"))
                    ipCmd = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
                else if (fileStr.split(QString("="), QString::SkipEmptyParts)[0] == QString("NETDIR"))
                    netDir = fileStr.split(QString("="), QString::SkipEmptyParts)[1].split(QString("\n"), QString::SkipEmptyParts)[0];
            }
        }
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
        command.start(cmd + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        if (cmdOutput != QString("")) {
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
        if (checkExtIP == QString("true")) {
            command.start(extIpCmd);
            command.waitForFinished(-1);
            cmdOutput = command.readAllStandardOutput();
            if (cmdOutput != QString(""))
                value = cmdOutput.split(QString("\n"), QString::SkipEmptyParts)[0];
        }
        setData(source, QString("value"), value);
    }
    else if (source == QString("interfaces")) {
        if (QDir(netDir).exists())
            value = QDir(netDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot).join(QString(","));
        setData(source, QString("value"), value);
    }
    else if (source == QString("intIp")) {
        if (QDir(netDir).exists()) {
            QStringList netDevices = QDir(netDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            for (int i=0; i<netDevices.count(); i++) {
                cmdOutput = QString("");
                command.start(ipCmd + QString(" addr show ") + netDevices[i]);
                command.waitForFinished(-1);
                cmdOutput = command.readAllStandardOutput();
                if (cmdOutput != QString("")) {
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
        command.start(cmd + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        QStringList list;
        if (cmdOutput != QString("")) {
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
        command.start(cmd + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        value = QString("false");
        if (cmdOutput != QString("")) {
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
        command.start(cmd + QString(" list"));
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        QString currentProfile;
        if (cmdOutput != QString("")) {
            QStringList profileList = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profileList.count(); i++)
                if (profileList[i].split(QString(" "), QString::SkipEmptyParts).count() == 2) {
                    currentProfile = profileList[i].split(QString(" "), QString::SkipEmptyParts)[1];
                    break;
                }
        }
        command.start(cmd + QString(" status ") + currentProfile);
        command.waitForFinished(-1);
        cmdOutput = command.readAllStandardOutput();
        value = QString("static");
        if (cmdOutput != QString("")) {
            QStringList profile = cmdOutput.split(QString("\n"), QString::SkipEmptyParts);
            for (int i=0; i<profile.count(); i++)
                if (profile[i].split(QString(" "), QString::SkipEmptyParts)[0] == QString("Loaded:"))
                    if (profile[i].indexOf(QString("enabled")) > -1) {
                        value = QString("enabled");
                        break;
                    }
        }
        setData(source, QString("value"), value);
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
