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

#include <pdebug/pdebug.h>
#include <task/taskadds.h>


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
    initSources();
}


Netctl::~Netctl()
{
    if (debug) qDebug() << PDEBUG;
}


QStringList Netctl::sources() const
{
    if (debug) qDebug() << PDEBUG;

    QStringList sources;
    sources.append(QString("active"));
    sources.append(QString("current"));
    sources.append(QString("extip4"));
    sources.append(QString("extip6"));
    sources.append(QString("interfaces"));
    sources.append(QString("intip4"));
    sources.append(QString("intip6"));
    sources.append(QString("netctlauto"));
    sources.append(QString("profiles"));
    sources.append(QString("status"));

    return sources;
}


void Netctl::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    // default configuration
    QMap<QString, QString> rawConfig;
    rawConfig[QString("EXTIP4")] = QString("false");
    rawConfig[QString("EXTIP4CMD")] = QString("curl ip4.telize.com");
    rawConfig[QString("EXTIP6")] = QString("false");
    rawConfig[QString("EXTIP6CMD")] = QString("curl ip6.telize.com");
    rawConfig[QString("NETCTLCMD")] = QString("/usr/bin/netctl");
    rawConfig[QString("NETCTLAUTOCMD")] = QString("/usr/bin/netctl-auto");

    QString fileName = KGlobal::dirs()->findResource("config", "netctl.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
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
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("EXTIP4CMD")) &&
            (key != QString("EXTIP6CMD")) &&
            (key != QString("NETCTLCMD")) &&
            (key != QString("NETCTLAUTOCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << config.keys()[i] + QString("=") + config[config.keys()[i]];

    return config;
}


bool Netctl::sourceRequestEvent(const QString &name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source name" << name;

    return updateSourceEvent(name);
}


QStringList Netctl::getCurrentProfile(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;
    getNetctlAutoStatus(cmdNetctlAuto);

    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;
    TaskResult process = runTask(cmd + QString(" list"));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QStringList currentProfile;
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        if (profileList[i][0] == QChar('*'))
            currentProfile.append(profileList[i]);
    for (int i=0; i<currentProfile.count(); i++)
        currentProfile[i].remove(0, 2);

    return currentProfile;
}


QString Netctl::getExtIp(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    // test network connection
    if (!isNetworkActive()) return QString("N\\A");
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QString extIp = QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();

    return extIp;
}


QStringList Netctl::getInterfaceList()
{
    if (debug) qDebug() << PDEBUG;

    QList<QNetworkInterface> rawList = QNetworkInterface::allInterfaces();
    QStringList interfacesList;
    for (int i=0; i<rawList.count(); i++)
        interfacesList.append(rawList[i].name());

    return interfacesList;
}


QString Netctl::getIntIp(const QAbstractSocket::NetworkLayerProtocol protocol)
{
    if (debug) qDebug() << PDEBUG;

    QString intIp = QString("");
    if (protocol == QAbstractSocket::IPv4Protocol)
        intIp = QString("127.0.0.1/8");
    else if (protocol == QAbstractSocket::IPv6Protocol)
        intIp = QString("::1/128");
    QList<QHostAddress> rawList = QNetworkInterface::allAddresses();
    for (int i=0; i<rawList.count(); i++) {
        if(rawList[i] == QHostAddress(QHostAddress::LocalHost)) continue;
        if(rawList[i] == QHostAddress(QHostAddress::LocalHostIPv6)) continue;
        if (rawList[i].protocol() == protocol) {
            intIp = rawList[i].toString();
            break;
        }
    }

    return intIp;
}


QString Netctl::getNetctlAutoStatus(const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;

    TaskResult process = runTask(cmdNetctlAuto + QString(" list"));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QString status;
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    if (cmdOutput.isEmpty()) {
        netctlAutoStatus = false;
        status = QString("false");
    } else {
        netctlAutoStatus = true;
        status = QString("true");
    }

    return status;
}


QStringList Netctl::getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;
    getNetctlAutoStatus(cmdNetctlAuto);

    QString cmd;
    if (netctlAutoStatus)
        cmd = cmdNetctlAuto;
    else
        cmd = cmdNetctl;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;
    TaskResult process = runTask(cmd + QString(" list"));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QString cmdOutput = QTextCodec::codecForMib(106)->toUnicode(process.output);
    QStringList profileList = cmdOutput.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++)
        profileList[i].remove(0, 2);

    return profileList;
}


QStringList Netctl::getProfileStringStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;
    getNetctlAutoStatus(cmdNetctlAuto);

    QStringList status;
    if (netctlAutoStatus)
        status.append(QString("netctl-auto"));
    else {
        QStringList profiles = getCurrentProfile(cmdNetctl, cmdNetctlAuto);
        for (int i=0; i<profiles.count(); i++) {
            TaskResult process = runTask(cmdNetctl + QString(" is-enabled ") + profiles[i]);
            if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
            if (process.exitCode != 0)
                if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
            if (process.exitCode == 0)
                status.append(QString("enabled"));
            else
                status.append(QString("static"));
        }
    }

    return status;
}


QString Netctl::getStatus(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;
    getNetctlAutoStatus(cmdNetctlAuto);

    QString status = QString("false");
    QStringList currentProfile = getCurrentProfile(cmdNetctl, cmdNetctlAuto);
    if (!currentProfile.isEmpty())
        status = QString("true");

    return status;
}


void Netctl::initSources()
{
    if (debug) qDebug() << PDEBUG;

    QStringList sourcesList = sources();
    for (int i=0; i<sourcesList.count(); i++)
        setData(sourcesList[i], QString("value"), QString("N\\A"));
}


bool Netctl::isNetworkActive()
{
    if (debug) qDebug() << PDEBUG;

    QString cmd = QString("ping -c 1 google.com");
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}


bool Netctl::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source name" << source;

    QString value = QString("N\\A");
    if (source == QString("active")) {
        value = getStatus(configuration[QString("NETCTLCMD")],
                configuration[QString("NETCTLAUTOCMD")]);
    } else if (source == QString("current")) {
        value = getCurrentProfile(configuration[QString("NETCTLCMD")],
                configuration[QString("NETCTLAUTOCMD")])
                .join(QChar('|'));
    } else if (source == QString("extip4")) {
        if (configuration[QString("EXTIP4")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIP4CMD")]);
    } else if (source == QString("extip6")) {
        if (configuration[QString("EXTIP6")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIP6CMD")]);
    } else if (source == QString("interfaces")) {
        value = getInterfaceList().join(QChar(','));
    } else if (source == QString("intip4")) {
        value = getIntIp(QAbstractSocket::IPv4Protocol);
    } else if (source == QString("intip6")) {
        value = getIntIp(QAbstractSocket::IPv6Protocol);
    } else if (source == QString("netctlauto")) {
        value = getNetctlAutoStatus(configuration[QString("NETCTLAUTOCMD")]);
    } else if (source == QString("profiles")) {
        value = getProfileList(configuration[QString("NETCTLCMD")],
                configuration[QString("NETCTLAUTOCMD")])
                .join(QChar(','));
    } else if (source == QString("status")) {
        value = getProfileStringStatus(configuration[QString("NETCTLCMD")],
                configuration[QString("NETCTLAUTOCMD")])
                .join(QChar('|'));
    }
    setData(source, QString("value"), value);

    return true;
}


K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)

#include "netctl.moc"
