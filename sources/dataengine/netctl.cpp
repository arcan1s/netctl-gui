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

#include <QDebug>
#include <QFile>
#include <QNetworkInterface>
#include <QProcessEnvironment>
#include <QSettings>
#include <QTextCodec>

#include <pdebug/pdebug.h>
#include <task/taskadds.h>
#include <version.h>

// KF5-KDE4 compability
#ifdef BUILD_KDE4
#include <KGlobal>
#include <KStandardDirs>
#else
#include <QStandardPaths>
#endif /* BUILD_KDE4 */


Netctl::Netctl(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)

    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));

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
    sources.append(QString("info"));
    sources.append(QString("interfaces"));
    sources.append(QString("intip4"));
    sources.append(QString("intip6"));
    sources.append(QString("netctlauto"));
    sources.append(QString("profiles"));
    sources.append(QString("status"));

    return sources;
}


QString Netctl::getCmdOutput(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return QTextCodec::codecForMib(106)->toUnicode(process.output).trimmed();
}


void Netctl::readConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QString fileName;
#ifdef BUILD_KDE4
    fileName = KGlobal::dirs()->findResource("config", "plasma-dataengine-netctl.conf");
#else
    fileName = QStandardPaths::locate(QStandardPaths::ConfigLocation, QString("plasma-dataengine-netctl.conf"));
#endif /* BUILD_KDE4 */
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QSettings settings(fileName, QSettings::IniFormat);

    settings.beginGroup(QString("Netctl commands"));
    configuration[QString("NETCTLCMD")] = settings.value(QString("NETCTLCMD"), QString("/usr/bin/netctl")).toString();
    configuration[QString("NETCTLAUTOCMD")] = settings.value(QString("NETCTLAUTOCMD"), QString("/usr/bin/netctl-auto")).toString();
    settings.endGroup();

    settings.beginGroup(QString("External IP"));
    configuration[QString("EXTIP4")] = settings.value(QString("EXTIP4"), QString("false")).toString();
    configuration[QString("EXTIP4CMD")] = settings.value(QString("EXTIP4CMD"), QString("curl ip4.telize.com")).toString();
    configuration[QString("EXTIP6")] = settings.value(QString("EXTIP6"), QString("false")).toString();
    configuration[QString("EXTIP6CMD")] = settings.value(QString("EXTIP6CMD"), QString("curl ip6.telize.com")).toString();
    settings.endGroup();
}


bool Netctl::sourceRequestEvent(const QString &name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source name" << name;

    return updateSourceEvent(name);
}


QString Netctl::getExtIp(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    // test network connection
    if (!isNetworkActive()) return QString("N\\A");

    return getCmdOutput(cmd);
}


QString Netctl::getInfo(const QStringList profiles, const QStringList statuses)
{
    if (debug) qDebug() << PDEBUG;
    if (profiles.count() != statuses.count()) return QString("N\\A");

    QStringList list;
    for (int i=0; i<profiles.count(); i++)
        list.append(QString("%1 (%2)").arg(profiles[i]).arg(statuses[i]));
    if (list.isEmpty()) list.append(QString("N\\A"));

    return list.join(QString(" | "));
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
        if (rawList[i].protocol() != protocol) continue;
        intIp = rawList[i].toString();
        break;
    }

    return intIp;
}


QStringList Netctl::getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto)
{
    if (debug) qDebug() << PDEBUG;

    netctlAutoStatus = true;
    QString output = getCmdOutput(QString("%1 list").arg(cmdNetctlAuto));
    if (output.isEmpty()) {
        output = getCmdOutput(QString("%1 list").arg(cmdNetctl));
        netctlAutoStatus = false;
    }

    // parse
    QStringList currentProfiles;
    QStringList profileList = output.split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<profileList.count(); i++) {
        if (profileList[i][0] != QChar('*')) continue;
        currentProfiles.append(profileList[i]);
    }
    // remove first symbols
    for (int i=0; i<profileList.count(); i++)
        profileList[i].remove(0, 2);
    for (int i=0; i<currentProfiles.count(); i++)
        currentProfiles[i].remove(0, 2);

    // return profiles
    currentProfile = currentProfiles;
    status = !currentProfiles.isEmpty();
    return profileList;
}


QStringList Netctl::getProfileStringStatus(const QString cmdNetctl)
{
    if (debug) qDebug() << PDEBUG;

    QStringList status;
    if (netctlAutoStatus) {
        status.append(QString("netctl-auto"));
        return status;
    }

    for (int i=0; i<currentProfile.count(); i++) {
        TaskResult process = runTask(QString("%1 is-enabled %2").arg(cmdNetctl).arg(currentProfile[i]));
        if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
        if (process.exitCode != 0) {
            if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
            status.append(QString("static"));
        } else
            status.append(QString("enabled"));
    }

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

    return (process.exitCode == 0);
}


bool Netctl::updateSourceEvent(const QString &source)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source name" << source;

    QString value = QString("N\\A");
    if (source == QString("active")) {
        value = status ? QString("true") : QString("false");
    } else if (source == QString("current")) {
        value = currentProfile.join(QChar('|'));
    } else if (source == QString("extip4")) {
        if (configuration[QString("EXTIP4")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIP4CMD")]);
    } else if (source == QString("extip6")) {
        if (configuration[QString("EXTIP6")] == QString("true"))
            value = getExtIp(configuration[QString("EXTIP6CMD")]);
    } else if (source == QString("info")) {
        value = getInfo(currentProfile, currentStatus);
    } else if (source == QString("interfaces")) {
        value = getInterfaceList().join(QChar(','));
    } else if (source == QString("intip4")) {
        value = getIntIp(QAbstractSocket::IPv4Protocol);
    } else if (source == QString("intip6")) {
        value = getIntIp(QAbstractSocket::IPv6Protocol);
    } else if (source == QString("netctlauto")) {
        value = netctlAutoStatus ? QString("true") : QString("false");
    } else if (source == QString("profiles")) {
        value = getProfileList(configuration[QString("NETCTLCMD")],
                               configuration[QString("NETCTLAUTOCMD")]).join(QChar(','));
    } else if (source == QString("status")) {
        currentStatus = getProfileStringStatus(configuration[QString("NETCTLCMD")]);
        value = currentStatus.join(QChar('|'));
    }
    setData(source, QString("value"), value);

    return true;
}


#ifdef BUILD_KDE4
K_EXPORT_PLASMA_DATAENGINE(netctl, Netctl)
#else
K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(netctl, Netctl, "plasma-dataengine-netctl.json")
#endif /* BUILD_KDE4 */

#include "netctl.moc"
