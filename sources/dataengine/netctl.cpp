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

#include <KGlobal>
#include <KStandardDirs>
#include <Plasma/DataContainer>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTextCodec>


Netctl::Netctl(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("NETCTLGUI_DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setMinimumPollingInterval(333);
    readConfiguration();
    setProcesses();
    setKeys();
}


QStringList Netctl::sources() const
{
    if (debug) qDebug() << "[DE]" << "[sources]";

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


void Netctl::initValues()
{
    if (debug) qDebug() << "[DE]" << "[initValues]";

    netctlAutoStatus = false;
    QStringList sourcesList = sources();
    for (int i=0; i<sourcesList.count(); i++)
        sourceRequestEvent(sourcesList[i]);
}


void Netctl::setKeys()
{
    if (debug) qDebug() << "[DE]" << "[setKeys]";

    QStringList sourcesList = sources();
    for (int i=0; i<sourcesList.count(); i++)
        setData(sourcesList[i], QString("value"), QString(""));

    initValues();
}


void Netctl::setProcesses()
{
    if (debug) qDebug() << "[DE]" << "[setProcesses]";

    processes[QString("currentProfile")] = new QProcess();
    connect(processes[QString("currentProfile")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setCurrentProfile(int, QProcess::ExitStatus)));
    processes[QString("extIp")] = new QProcess();
    connect(processes[QString("extIp")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setExtIp(int, QProcess::ExitStatus)));
    processes[QString("intIp")] = new QProcess();
    connect(processes[QString("intIp")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setIntIp(int, QProcess::ExitStatus)));
    processes[QString("profiles")] = new QProcess();
    connect(processes[QString("profiles")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setProfileList(int, QProcess::ExitStatus)));
    processes[QString("statusBool")] = new QProcess();
    connect(processes[QString("statusBool")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setProfileStatus(int, QProcess::ExitStatus)));
    processes[QString("statusString")] = new QProcess();
    connect(processes[QString("statusString")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setProfileStringStatus(int, QProcess::ExitStatus)));
}


void Netctl::readConfiguration()
{
    if (debug) qDebug() << "[DE]" << "[readConfiguration]";

    // default configuration
    QMap<QString, QString> rawConfig;
    rawConfig[QString("CMD")] = QString("/usr/bin/netctl");
    rawConfig[QString("EXTIP")] = QString("false");
    rawConfig[QString("EXTIPCMD")] = QString("wget -qO- http://ifconfig.me/ip");
    rawConfig[QString("IPCMD")] = QString("/usr/bin/ip");
    rawConfig[QString("NETDIR")] = QString("/sys/class/net/");
    rawConfig[QString("NETCTLAUTOCMD")] = QString("/usr/bin/netctl-auto");

    QString fileName = KGlobal::dirs()->findResource("config", "netctl.conf");
    if (debug) qDebug() << "[DE]" << "[readConfiguration]" << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::ReadOnly)) {
        configuration = updateConfiguration(rawConfig);
        return;
    }
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (configFile.atEnd()) break;
    }
    configFile.close();
    configuration = updateConfiguration(rawConfig);

    return;
}


QMap<QString, QString> Netctl::updateConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << "[DE]" << "[updateConfiguration]";

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("CMD")) &&
            (key != QString("EXTIPCMD")) &&
            (key != QString("IPCMD")) &&
            (key != QString("NETCTLAUTOCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << "[DE]" << "[updateConfiguration]" << ":" <<
            config.keys()[i] + QString("=") + config[config.keys()[i]];

    return config;
}


bool Netctl::sourceRequestEvent(const QString &name)
{
    if (debug) qDebug() << "[DE]" << "[sourceRequestEvent]";
    if (debug) qDebug() << "[DE]" << "[sourceRequestEvent]" << ":" << "Source name" << name;

    return updateSourceEvent(name);
}


QString Netctl::getCurrentProfile(const QString cmd)
{
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]";
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd" << cmd;

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
    if (debug) qDebug() << "[DE]" << "[getExtIp]";
    if (debug) qDebug() << "[DE]" << "[getExtIp]" << ":" << "Cmd" << cmd;

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
    if (debug) qDebug() << "[DE]" << "[getInterfaceList]";
    if (debug) qDebug() << "[DE]" << "[getInterfaceList]" << ":" << "Directory" << dir;

    QStringList interfaceList;
    if (QDir(dir).exists())
        interfaceList = QDir(dir).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    return interfaceList;
}


QString Netctl::getIntIp(const QString cmd, const QString dir)
{
    if (debug) qDebug() << "[DE]" << "[getIntIp]";
    if (debug) qDebug() << "[DE]" << "[getIntIp]" << ":" << "Cmd" << cmd;
    if (debug) qDebug() << "[DE]" << "[getIntIp]" << ":" << "Directory" << dir;

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
    if (debug) qDebug() << "[DE]" << "[getProfileList]";
    if (debug) qDebug() << "[DE]" << "[getProfileList]" << ":" << "Cmd" << cmd;

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
    if (debug) qDebug() << "[DE]" << "[getProfileStatus]";
    if (debug) qDebug() << "[DE]" << "[getProfileStatus]" << ":" << "Cmd" << cmd;

    bool status = false;
    QString cmdOutput = getCurrentProfile(cmd);
    if (!cmdOutput.isEmpty())
        status = true;

    return status;
}


QString Netctl::getProfileStringStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]";
    if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]" << ":" << "Cmd" << cmdNetctl;
    if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]" << ":" << "Cmd" << cmdNetctlAuto;

    QProcess command;
    QString status = QString("static");
    // check netctl-auto
    if (!getCurrentProfile(cmdNetctlAuto).isEmpty())
        status = QString("netctl-auto");
    else {
        // check netctl
        command.start(cmdNetctl + QString(" is-enabled ") + getCurrentProfile(cmdNetctl));
        command.waitForFinished(-1);
        if (command.exitCode() == 0)
            status = QString("enabled");
    }

    return status;
}


bool Netctl::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]";
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]" << ":" << "Source name" << source;

    QString key = QString("value");
    QString value = QString("");
    if (source == QString("currentProfile")) {
        value = getCurrentProfile(configuration[QString("NETCTLAUTOCMD")]);
        if (value.isEmpty())
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
        value = getProfileList(configuration[QString("NETCTLAUTOCMD")]).join(QChar(','));
        if (value.isEmpty())
            value = getProfileList(configuration[QString("CMD")]).join(QChar(','));
    }
    else if (source == QString("statusBool")) {
        if (getProfileStatus(configuration[QString("NETCTLAUTOCMD")]))
            value = QString("true");
        else if (getProfileStatus(configuration[QString("CMD")]))
            value = QString("true");
        else
            value = QString("false");
    }
    else if (source == QString("statusString")) {
        value = getProfileStringStatus(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    setData(source, key, value);

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
