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

#include "dbusoperation.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

#include <pdebug/pdebug.h>

#include "version.h"


QList<netctlProfileInfo> parseOutputNetctl(const QList<QVariant> raw)
{
    QList<netctlProfileInfo> profileInfo;
    if (raw.isEmpty()) return profileInfo;
    QStringList list = raw[0].toStringList();
    for (int i=0; i<list.count(); i++) {
        QStringList info = list[i].split(QChar('|'));
        if (info.count() != 5) continue;
        netctlProfileInfo profile;
        profile.name = info[0];
        profile.description = info[1];
        profile.essid = info[2];
        profile.active = info[3].toInt();
        profile.enabled = info[4].toInt();
        profileInfo.append(profile);
    }

    return profileInfo;
}


QList<netctlWifiInfo> parseOutputWifi(const QList<QVariant> raw)
{
    QList<netctlWifiInfo> wifiInfo;
    if (raw.isEmpty()) return wifiInfo;
    QStringList list = raw[0].toStringList();
    for (int i=0; i<list.count(); i++) {
        QStringList info = list[i].split(QChar('|'));
        if (info.count() != 8) continue;
        netctlWifiInfo wifi;
        wifi.name = info[0];
        wifi.security = info[1];
        wifi.type = static_cast<PointType>(info[2].toInt());
        for (int j=0; j<info[3].split(QChar(',')).count(); j++)
            wifi.frequencies.append(info[3].split(QChar(','))[j].toInt());
        wifi.macs = info[4].split(QChar(','));
        wifi.signal = info[5].toInt();
        wifi.active = info[6].toInt();
        wifi.exists = info[7].toInt();
        wifiInfo.append(wifi);
    }

    return wifiInfo;
}


QList<QVariant> sendRequestToHelper(const QString path, const QString cmd,
                                    const QList<QVariant> args, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;

    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, path,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty()) request.setArguments(args);
    QDBusMessage response = bus.call(request, QDBus::BlockWithGui);
    QList<QVariant> arguments = response.arguments();
    if (arguments.isEmpty())
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();

    return arguments;
}


QList<QVariant> sendRequestToCtrl(const QString cmd, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;

    return sendRequestToCtrlWithArgs(cmd, QList<QVariant>(), debug);
}


QList<QVariant> sendRequestToCtrlWithArgs(const QString cmd, const QList<QVariant> args,
                                          const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;

    return sendRequestToHelper(DBUS_CTRL_PATH, cmd, args, debug);
}


QList<QVariant> sendRequestToLib(const QString cmd, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;

    return sendRequestToLibWithArgs(cmd, QList<QVariant>(), debug);
}


QList<QVariant> sendRequestToLibWithArgs(const QString cmd, const QList<QVariant> args,
                                         const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;

    return sendRequestToHelper(DBUS_LIB_PATH, cmd, args, debug);
}
