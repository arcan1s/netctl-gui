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
#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextCodec>


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


void Netctl::readConfiguration()
{
    // default configuration
    QMap<QString, QString> rawConfig;
    rawConfig[QString("CMD")] = QString("/usr/bin/netctl");
    rawConfig[QString("EXTIP")] = QString("false");
    rawConfig[QString("EXTIPCMD")] = QString("wget -qO- http://ifconfig.me/ip");
    rawConfig[QString("IPCMD")] = QString("/usr/bin/ip");
    rawConfig[QString("NETDIR")] = QString("/sys/class/net/");

    QString fileName = KGlobal::dirs()->findResource("config", "netctl.conf");
    QFile confFile(fileName);
    bool ok = confFile.open(QIODevice::ReadOnly);
    if (!ok) {
        configuration = updateConfiguration(rawConfig);
        return;
    }
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(confFile.readLine()).trimmed();
        if (fileStr[0] == QChar('#')) continue;
        if (fileStr[0] == QChar(';')) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (confFile.atEnd())
            break;
    }
    confFile.close();
    configuration = updateConfiguration(rawConfig);
    return;
}


QMap<QString, QString> Netctl::updateConfiguration(const QMap<QString, QString> rawConfig)
{
    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("CMD")) &&
            (key != QString("EXTIPCMD")) &&
            (key != QString("IPCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }
    return config;
}


bool Netctl::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}


QString Netctl::getCurrentProfile(const QString cmd)
{
    QProcess command;
    QString profile = QString("");
    command.start(cmd + QString(" list"));
    command.waitForFinished(-1);
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        if (profileList[i][0] == QChar('*')) {
            profile = profileList[i];
            break;
        }
    profile.remove(0, 1);
    return profile;
}


QString Netctl::getExtIp(const QString cmd)
{
    QProcess command;
    QString extIp = QString("");
    command.start(cmd);
    command.waitForFinished(-1);
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    extIp = cmdOutput.trimmed();
    return extIp;
}


QStringList Netctl::getInterfaceList(const QString dir)
{
    QStringList interfaceList;
    if (QDir(dir).exists())
        interfaceList = QDir(dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return interfaceList;
}


QString Netctl::getIntIp(const QString cmd, const QString dir)
{
    QProcess command;
    QString intIp = QString("127.0.0.1/8");
    QStringList interfaceList = getInterfaceList(dir);
    for (int i=0; i<interfaceList.count(); i++)
        if (interfaceList[i] != QString("lo")) {
            command.start(cmd + QString(" addr show ") + interfaceList[i]);
            command.waitForFinished(-1);
            QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
            QStringList deviceInfo = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
            for (int j=0; j<deviceInfo.count(); j++)
                if (deviceInfo[j].split(QChar(' '), QString::SkipEmptyParts)[0] == QString("inet"))
                    intIp = deviceInfo[j].split(QChar(' '), QString::SkipEmptyParts)[1];
        }
    return intIp;
}


QStringList Netctl::getProfileList(const QString cmd)
{
    QProcess command;
    command.start(cmd + QString(" list"));
    command.waitForFinished(-1);
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        profileList[i].remove(0, 1);
    return profileList;
}


bool Netctl::getProfileStatus(const QString cmd)
{
    bool status = false;
    QString cmdOutput = getCurrentProfile(cmd);
    if (!cmdOutput.isEmpty())
        status = true;
    return status;
}


QString Netctl::getProfileStringStatus(const QString cmd)
{
    QProcess command;
    QString status = QString("static");
    QString profile = getCurrentProfile(cmd);
    command.start(cmd + QString(" status ") + profile);
    command.waitForFinished(-1);
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(command.readAllStandardOutput());
    QStringList profileStatus = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileStatus.count(); i++)
        if (profileStatus[i].split(QChar(' '), QString::SkipEmptyParts)[0] == QString("Loaded:")) {
            if (profileStatus[i].contains(QString("enabled")))
                status = QString("enabled");
            break;
        }
    return status;
}


bool Netctl::updateSourceEvent(const QString &source)
{
    QString key = QString("value");
    QString value = QString("");
    if (source == QString("currentProfile")) {
        value = getCurrentProfile(configuration[QString("CMD")]);
    }
    else if (source == QString("extIp")) {
        if (configuration[QString("EXTIP")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIPCMD")]);
    }
    else if (source == QString("interfaces")) {
        value = getInterfaceList(configuration[QString("NETDIR")]).join(QChar(','));
    }
    else if (source == QString("intIp")) {
        value = getIntIp(configuration[QString("IPCMD")], configuration[QString("NETDIR")]);
    }
    else if (source == QString("profiles")) {
        value = getProfileList(configuration[QString("CMD")]).join(QChar(','));
    }
    else if (source == QString("statusBool")) {
        if (getProfileStatus(configuration[QString("CMD")]))
            value = QString("true");
        else
            value = QString("false");
    }
    else if (source == QString("statusString")) {
        value = getProfileStringStatus(configuration[QString("CMD")]);
    }
    setData(source, key, value);
    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
