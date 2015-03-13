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

#include <pdebug/pdebug.h>

#include "dbusoperation.h"


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
