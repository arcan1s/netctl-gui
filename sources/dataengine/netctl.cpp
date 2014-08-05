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
#include <QFile>
#include <QNetworkInterface>
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


Netctl::~Netctl()
{
    if (debug) qDebug() << "[DE]" << "[~Netctl]";

    QStringList processesKeys = processes.keys();
    for (int i=0; i<processesKeys.count(); i++) {
        processes[processesKeys[i]]->terminate();
        delete processes[processesKeys[i]];
    }
}


QStringList Netctl::sources() const
{
    if (debug) qDebug() << "[DE]" << "[sources]";

    QStringList sources;
    sources.append(QString("currentProfile"));
    sources.append(QString("extIp"));
    sources.append(QString("extIp6"));
    sources.append(QString("interfaces"));
    sources.append(QString("intIp"));
    sources.append(QString("intIp6"));
    sources.append(QString("netctlAuto"));
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
    processes[QString("extIp6")] = new QProcess();
    connect(processes[QString("extIp6")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setExtIp6(int, QProcess::ExitStatus)));
    processes[QString("netctlAuto")] = new QProcess();
    connect(processes[QString("netctlAuto")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setNetctlAutoStatus(int, QProcess::ExitStatus)));
    processes[QString("profiles")] = new QProcess();
    connect(processes[QString("profiles")], SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(setProfileList(int, QProcess::ExitStatus)));
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
    rawConfig[QString("EXTIPCMD")] = QString("curl ip4.telize.com");
    rawConfig[QString("EXTIP6")] = QString("false");
    rawConfig[QString("EXTIP6CMD")] = QString("curl ip6.telize.com");
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
            (key != QString("EXTIP6CMD")) &&
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


void Netctl::getCurrentProfile(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]";
    getNetctlAutoStatus(cmdNetctlAuto);

    if (processes[QString("currentProfile")]->state() != QProcess::NotRunning) return;
    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd" << cmd;

    processes[QString("currentProfile")]->start(cmd + QString(" list"));
}


void Netctl::setCurrentProfile(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setCurrentProfile]";
    if (debug) qDebug() << "[DE]" << "[setCurrentProfile]" << ":" << "Cmd returns" << exitCode;

    currentProfile = QString("");
    QString status = QString("false");
    QString cmdOutput = QTextCodec::codecForMib(106)
            ->toUnicode(processes[QString("currentProfile")]->readAllStandardOutput());
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        if (profileList[i][0] == QChar('*')) {
            currentProfile = profileList[i];
            break;
        }
    currentProfile.remove(0, 2);
    if (!currentProfile.isEmpty())
        status = QString("true");

    setData(QString("currentProfile"), QString("value"), currentProfile);
    setData(QString("statusBool"), QString("value"), status);
}


void Netctl::getExtIp(const QString cmd)
{
    if (debug) qDebug() << "[DE]" << "[getExtIp]";
    if (debug) qDebug() << "[DE]" << "[getExtIp]" << ":" << "Cmd" << cmd;

    if (processes[QString("extIp")]->state() != QProcess::NotRunning) return;

    processes[QString("extIp")]->start(cmd);
}


void Netctl::setExtIp(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setExtIp]";
    if (debug) qDebug() << "[DE]" << "[setExtIp]" << ":" << "Cmd returns" << exitCode;

    QString extIp = QString("");
    QString cmdOutput = QTextCodec::codecForMib(106)
            ->toUnicode(processes[QString("extIp")]->readAllStandardOutput())
            .trimmed();
    extIp = cmdOutput;

    setData(QString("extIp"), QString("value"), extIp);
}


void Netctl::getExtIp6(const QString cmd)
{
    if (debug) qDebug() << "[DE]" << "[getExtIp6]";
    if (debug) qDebug() << "[DE]" << "[getExtIp6]" << ":" << "Cmd" << cmd;

    if (processes[QString("extIp6")]->state() != QProcess::NotRunning) return;

    processes[QString("extIp6")]->start(cmd);
}


void Netctl::setExtIp6(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setExtIp6]";
    if (debug) qDebug() << "[DE]" << "[setExtIp6]" << ":" << "Cmd returns" << exitCode;

    QString extIp = QString("");
    QString cmdOutput = QTextCodec::codecForMib(106)
            ->toUnicode(processes[QString("extIp6")]->readAllStandardOutput())
            .trimmed();
    extIp = cmdOutput;

    setData(QString("extIp6"), QString("value"), extIp);
}


QStringList Netctl::getInterfaceList()
{
    if (debug) qDebug() << "[DE]" << "[getInterfaceList]";

    QList<QNetworkInterface> rawList = QNetworkInterface::allInterfaces();
    QStringList interfacesList;
    for (int i=0; i<rawList.count(); i++)
        interfacesList.append(rawList[i].name());

    return interfacesList;
}


QString Netctl::getIntIp()
{
    if (debug) qDebug() << "[DE]" << "[getIntIp]";

    QString intIp = QString("127.0.0.1/8");
    QList<QHostAddress> rawList = QNetworkInterface::allAddresses();
    for (int i=0; i<rawList.count(); i++) {
        if(rawList[i] == QHostAddress(QHostAddress::LocalHost)) continue;
        if (rawList[i].protocol() != QAbstractSocket::IPv4Protocol) continue;
        intIp = rawList[i].toString();
        break;
    }

    return intIp;
}


QString Netctl::getIntIp6()
{
    if (debug) qDebug() << "[DE]" << "[getIntIp6]";

    QString intIp = QString("::1/128");
    QList<QHostAddress> rawList = QNetworkInterface::allAddresses();
    for (int i=0; i<rawList.count(); i++) {
        if(rawList[i] == QHostAddress(QHostAddress::LocalHost)) continue;
        if (rawList[i].protocol() != QAbstractSocket::IPv6Protocol) continue;
        intIp = rawList[i].toString();
        break;
    }

    return intIp;
}


void Netctl::getNetctlAutoStatus(const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getNetctlAutoStatus]";

    if (processes[QString("netctlAuto")]->state() != QProcess::NotRunning) return;

    processes[QString("netctlAuto")]->start(cmdNetctlAuto + QString(" list"));
}


void Netctl::setNetctlAutoStatus(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setNetctlAutoStatus]";
    if (debug) qDebug() << "[DE]" << "[setNetctlAutoStatus]" << ":" << "Cmd returns" << exitCode;

    QString status;
    QString cmdOutput = QTextCodec::codecForMib(106)
            ->toUnicode(processes[QString("netctlAuto")]->readAllStandardOutput());
    if (cmdOutput.isEmpty()) {
        netctlAutoStatus = false;
        status = QString("false");
    }
    else {
        netctlAutoStatus = true;
        status = QString("true");
    }

    setData(QString("netctlAuto"), QString("value"), status);
}


void Netctl::getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getProfileList]";
    getNetctlAutoStatus(cmdNetctlAuto);

    if (processes[QString("profiles")]->state() != QProcess::NotRunning) return;
    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd" << cmd;

    processes[QString("profiles")]->start(cmd + QString(" list"));
}


void Netctl::setProfileList(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setProfileList]";
    if (debug) qDebug() << "[DE]" << "[setProfileList]" << ":" << "Cmd returns" << exitCode;

    QString cmdOutput = QTextCodec::codecForMib(106)
            ->toUnicode(processes[QString("profiles")]->readAllStandardOutput());
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        profileList[i].remove(0, 2);

    setData(QString("profiles"), QString("value"), profileList.join(QChar(',')));
}


void Netctl::getProfileStringStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]";
    getNetctlAutoStatus(cmdNetctlAuto);

    if (netctlAutoStatus) {
        setData(QString("statusString"), QString("value"), QString("netctl-auto"));
        return;
    }
    if (processes[QString("statusString")]->state() != QProcess::NotRunning) return;

    processes[QString("statusString")]->start(cmdNetctl + QString(" is-enabled ") + currentProfile);
}


void Netctl::setProfileStringStatus(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    if (debug) qDebug() << "[DE]" << "[setProfileStringStatus]";
    if (debug) qDebug() << "[DE]" << "[setProfileStringStatus]" << ":" << "Cmd returns" << exitCode;

    QString status = QString("static");
    if (exitCode == 0)
        status = QString("enabled");

    setData(QString("statusString"), QString("value"), status);
}


bool Netctl::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]";
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]" << ":" << "Source name" << source;

    if (source == QString("currentProfile")) {
        getCurrentProfile(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("extIp")) {
        if (configuration[QString("EXTIP")] == QString("true"))
            getExtIp(configuration[QString("EXTIPCMD")]);
    }
    else if (source == QString("extIp6")) {
        if (configuration[QString("EXTIP6")] == QString("true"))
            getExtIp(configuration[QString("EXTIP6CMD")]);
    }
    else if (source == QString("interfaces")) {
        QString value = getInterfaceList().join(QChar(','));
        setData(source, QString("value"), value);
    }
    else if (source == QString("intIp")) {
        QString value = getIntIp();
        setData(source, QString("value"), value);
    }
    else if (source == QString("intIp6")) {
        QString value = getIntIp6();
        setData(source, QString("value"), value);
    }
    else if (source == QString("netctlAuto")) {
        getNetctlAutoStatus(configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("profiles")) {
        getProfileList(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("statusBool")) {
        getCurrentProfile(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("statusString")) {
        getProfileStringStatus(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
