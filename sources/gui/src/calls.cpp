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

#include "calls.h"

#include <QDebug>

#include <listmap/listmap.h>
#include <pdebug/pdebug.h>

#include "dbusoperation.h"


InterfaceAnswer createProfileSlot(const QString profile, QMap<QString,QString> settings,
                                  NetctlInterface* interface, const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->createProfile(profile, settings);
    try {
        QList<QVariant> args;
        args.append(profile);
        args.append(mapToList(settings));
        int responce = sendRequestToInterfaceWithArgs(QString("Create"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer enableProfileSlot(const QString profile, NetctlInterface *interface,
                                  const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->enableProfile(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        int responce = sendRequestToInterfaceWithArgs(QString("Enable"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer removeProfileSlot(const QString profile, NetctlInterface *interface,
                                  const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->removeProfile(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        int responce = sendRequestToInterfaceWithArgs(QString("Remove"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer restartProfileSlot(const QString profile, NetctlInterface *interface,
                                   const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->restartProfile(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        int responce = sendRequestToInterfaceWithArgs(QString("Restart"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer startProfileSlot(const QString profile, NetctlInterface *interface,
                                 const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->startProfile(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        int responce = sendRequestToInterfaceWithArgs(QString("Start"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer stopAllProfilesSlot(NetctlInterface *interface, const bool useHelper,
                                    const bool debug)
{
    if (debug) qDebug() << PDEBUG;

    if (!useHelper) return interface->stopAllProfiles();
    try {
        int responce = sendRequestToInterface(QString("StopAll"), debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer switchToProfileSlot(const QString profile, NetctlInterface *interface,
                                    const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (!useHelper) return interface->switchToProfile(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        int responce = sendRequestToInterfaceWithArgs(QString("SwitchTo"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


InterfaceAnswer connectToEssid(const QString essid, QMap<QString,QString> settings,
                               NetctlInterface* interface, const bool useHelper, const bool debug)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;


    if (!useHelper) return interface->connectToEssid(essid, settings);
    try {
        QList<QVariant> args;
        args.append(essid);
        args.append(mapToList(settings));
        int responce = sendRequestToInterfaceWithArgs(QString("Essid"), args, debug)[0].toInt();
        return static_cast<InterfaceAnswer>(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return InterfaceAnswer::Error;
    }
}


netctlInformation generalInformation(NetctlInterface *interface, const bool useHelper,
                                     const bool debug)
{
    if (debug) qDebug() << PDEBUG;

    if (!useHelper) return interface->information();
    try {
        QList<QVariant> responce = sendRequestToInterface(QString("Information"), debug);
        // main
        netctlInformation info;
        info.netctlAuto = responce[0].toStringList().takeFirst().toInt();
        info.netctlAutoEnabled = responce[0].toStringList().takeFirst().toInt();
        QList<netctlProfileInfo> profiles = parseOutputNetctl(responce);
        for (int i=0; i<profiles.count(); i++) {
            if (profiles[i].netctlAuto)
                info.netctlAutoProfiles.append(profiles[i]);
            else
                info.netctlProfiles.append(profiles[i]);
        }
        return info;
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return netctlInformation();
    }
}


QMap<QString, QString> profileInformation(const QString profile, NetctlInterface *interface,
                                          const bool useHelper, const bool debug)
{
    if (!useHelper) return interface->profileSettings(profile);
    try {
        QList<QVariant> args;
        args.append(profile);
        QStringList responce = sendRequestToInterfaceWithArgs(QString("Profile"), args, debug)[0].toStringList();
        return listToMap(responce);
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return QMap<QString, QString>();
    }
}


netctlCurrent trayInformation(NetctlInterface *interface, const bool useHelper,
                              const bool debug)
{
    if (debug) qDebug() << PDEBUG;

    if (!useHelper) return interface->status();
    try {
        QStringList responce = sendRequestToInterface(QString("Status"), debug)[0].toStringList();
        netctlCurrent status;
        status.netctlAuto = responce[0].toInt();
        status.profiles = responce[1].split(QChar('|'));
        status.current = responce[2].split(QChar('|'));
        QStringList enables = responce[3].split(QChar('|'));
        for (int i=0; i<enables.count(); i++)
            status.enables.append(enables[i].toInt());
        return status;
    } catch (...) {
        if (debug) qDebug() << PDEBUG << ":" << "An exception recevied";
        return netctlCurrent();
    }
}
