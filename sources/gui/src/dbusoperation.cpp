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

#include "pdebug.h"


QList<netctlProfileInfo> parseOutputNetctl(const QList<QVariant> raw,
                                           const bool debug)
{
    if (debug) qDebug() << PDEBUG;

    QList<netctlProfileInfo> profileInfo;
    if (raw.size() == 0)
        return profileInfo;
    for (int i=0; i<raw[0].toStringList().count(); i++) {
        netctlProfileInfo profile;
        QStringList info = raw[0].toStringList()[i].split(QChar('|'));
        profile.name = info[0];
        profile.description = info[1];
        profile.active = info[2].toInt();
        profile.enabled = info[3].toInt();
        profileInfo.append(profile);
    }

    return profileInfo;
}


QList<netctlWifiInfo> parseOutputWifi(const QList<QVariant> raw,
                                      const bool debug)
{
    if (debug) qDebug() << PDEBUG;

    QList<netctlWifiInfo> wifiInfo;
    if (raw.size() == 0)
        return wifiInfo;
    for (int i=0; i<raw[0].toStringList().count(); i++) {
        netctlWifiInfo wifi;
        QStringList info = raw[0].toStringList()[i].split(QChar('|'));
        wifi.name = info[0];
        wifi.security = info[1];
        wifi.signal = info[2];
        wifi.active = info[3].toInt();
        wifi.exists = info[4].toInt();
        wifiInfo.append(wifi);
    }

    return wifiInfo;
}


QList<QVariant> sendDBusRequest(const QString service, const QString path,
                                const QString interface, const QString cmd,
                                const QList<QVariant> args, const bool system,
                                const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Service" << service;
    if (debug) qDebug() << PDEBUG << ":" << "Path" << path;
    if (debug) qDebug() << PDEBUG << ":" << "Interface" << interface;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;
    if (debug) qDebug() << PDEBUG << ":" << "is system bus" << system;

    QList<QVariant> arguments;
    QDBusMessage response;
    if (system) {
        QDBusConnection bus = QDBusConnection::systemBus();
        QDBusMessage request = QDBusMessage::createMethodCall(service, path, interface, cmd);
        if (!args.isEmpty())
            request.setArguments(args);
        response = bus.call(request, QDBus::BlockWithGui);
    } else {
        QDBusConnection bus = QDBusConnection::sessionBus();
        QDBusMessage request = QDBusMessage::createMethodCall(service, path, interface, cmd);
        if (!args.isEmpty())
            request.setArguments(args);
        response = bus.call(request, QDBus::BlockWithGui);
    }
    arguments = response.arguments();
    if (arguments.size() == 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();

    return arguments;
}
