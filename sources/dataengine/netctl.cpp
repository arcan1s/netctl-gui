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

#include <KGlobal>
#include <KStandardDirs>
#include <Plasma/DataContainer>

#include <QDebug>
#include <QFile>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QTextCodec>

#include "netctl.h"
#include "taskadds.h"


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
    setKeys();
}


Netctl::~Netctl()
{
    if (debug) qDebug() << "[DE]" << "[~Netctl]";
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


void Netctl::setKeys()
{
    if (debug) qDebug() << "[DE]" << "[setKeys]";

    QStringList sourcesList = sources();
    for (int i=0; i<sourcesList.count(); i++)
        setData(sourcesList[i], QString("value"), QString(""));
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


QString Netctl::getCurrentProfile(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]";
    getNetctlAutoStatus(cmdNetctlAuto);

    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd" << cmd;
    TaskResult process = runTask(cmd + QString(" list"));
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd returns" << process.exitCode;

    currentProfile = QString("");
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        if (profileList[i][0] == QChar('*')) {
            currentProfile = profileList[i];
            break;
        }
    currentProfile.remove(0, 2);

    return currentProfile;
}


QString Netctl::getExtIp(const QString cmd)
{
    if (debug) qDebug() << "[DE]" << "[getExtIp]";
    if (debug) qDebug() << "[DE]" << "[getExtIp]" << ":" << "Cmd" << cmd;

    TaskResult process = runTask(cmd);
    if (debug) qDebug() << "[DE]" << "[getExtIp]" << ":" << "Cmd returns" << process.exitCode;

    QString extIp = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();

    return extIp;
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


QString Netctl::getIntIp(const QAbstractSocket::NetworkLayerProtocol protocol)
{
    if (debug) qDebug() << "[DE]" << "[getIntIp]";

    QString intIp = QString("");
    if (protocol == QAbstractSocket::IPv4Protocol)
        intIp = QString("127.0.0.1/8");
    else if (protocol == QAbstractSocket::IPv6Protocol)
        intIp = QString("::1/128");
    QList<QHostAddress> rawList = QNetworkInterface::allAddresses();
    for (int i=0; i<rawList.count(); i++) {
        if(rawList[i] == QHostAddress(QHostAddress::LocalHost)) continue;
        if (rawList[i].protocol() == protocol) {
            intIp = rawList[i].toString();
            break;
        }
    }

    return intIp;
}


QString Netctl::getNetctlAutoStatus(const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getNetctlAutoStatus]";

    TaskResult process = runTask(cmdNetctlAuto + QString(" list"));
    if (debug) qDebug() << "[DE]" << "[getNetctlAutoStatus]" << ":" << "Cmd returns" << process.exitCode;

    QString status;
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    if (cmdOutput.isEmpty()) {
        netctlAutoStatus = false;
        status = QString("false");
    }
    else {
        netctlAutoStatus = true;
        status = QString("true");
    }

    return status;
}


QStringList Netctl::getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getProfileList]";
    getNetctlAutoStatus(cmdNetctlAuto);

    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd" << cmd;
    TaskResult process = runTask(cmd + QString(" list"));
    if (debug) qDebug() << "[DE]" << "[getCurrentProfile]" << ":" << "Cmd returns" << process.exitCode;

    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        profileList[i].remove(0, 2);

    return profileList;
}


QString Netctl::getProfileStringStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]";
    getNetctlAutoStatus(cmdNetctlAuto);

    QString status = QString("static");
    if (netctlAutoStatus)
        status = QString("netctl-auto");
    else {
        TaskResult process = runTask(cmdNetctl + QString(" is-enabled ") + getCurrentProfile(cmdNetctl, cmdNetctlAuto));
        if (debug) qDebug() << "[DE]" << "[getProfileStringStatus]" << ":" << "Cmd returns" << process.exitCode;

        if (process.exitCode == 0)
            status = QString("enabled");
    }

    return status;
}


QString Netctl::getStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << "[DE]" << "[getStatus]";
    getNetctlAutoStatus(cmdNetctlAuto);

    QString status = QString("false");
    QString currentProfile = getCurrentProfile(cmdNetctl, cmdNetctlAuto);
    if (!currentProfile.isEmpty())
        status = QString("true");

    return status;
}


bool Netctl::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]";
    if (debug) qDebug() << "[DE]" << "[updateSourceEvent]" << ":" << "Source name" << source;

    QString value = QString("N\\A");
    if (source == QString("currentProfile")) {
        value = getCurrentProfile(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("extIp")) {
        if (configuration[QString("EXTIP")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIPCMD")]);
    }
    else if (source == QString("extIp6")) {
        if (configuration[QString("EXTIP6")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIP6CMD")]);
    }
    else if (source == QString("interfaces")) {
        value = getInterfaceList().join(QChar(','));
    }
    else if (source == QString("intIp")) {
        value = getIntIp(QAbstractSocket::IPv4Protocol);
    }
    else if (source == QString("intIp6")) {
        value = getIntIp(QAbstractSocket::IPv6Protocol);
    }
    else if (source == QString("netctlAuto")) {
        value = getNetctlAutoStatus(configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("profiles")) {
        value = getProfileList(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")])
                .join(QChar(','));
    }
    else if (source == QString("statusBool")) {
        value = getStatus(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    else if (source == QString("statusString")) {
        value = getProfileStringStatus(configuration[QString("CMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    }
    setData(source, QString("value"), value);

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
