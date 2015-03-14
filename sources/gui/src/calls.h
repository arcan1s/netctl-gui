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

#ifndef CALLS_H
#define CALLS_H

#include <QString>

#include <netctlgui/netctlgui.h>


// control
InterfaceAnswer createProfileSlot(const QString profile, const QMap<QString,QString> settings,
                                  NetctlInterface *interface, const bool useHelper,
                                  const bool debug = false);
InterfaceAnswer enableProfileSlot(const QString profile, NetctlInterface *interface,
                                  const bool useHelper, const bool debug = false);
InterfaceAnswer removeProfileSlot(const QString profile, NetctlInterface *interface,
                                  const bool useHelper, const bool debug = false);
InterfaceAnswer restartProfileSlot(const QString profile, NetctlInterface *interface,
                                   const bool useHelper, const bool debug = false);
InterfaceAnswer startProfileSlot(const QString profile, NetctlInterface *interface,
                                 const bool useHelper, const bool debug = false);
InterfaceAnswer stopAllProfilesSlot(NetctlInterface *interface, const bool useHelper,
                                    const bool debug = false);
InterfaceAnswer switchToProfileSlot(const QString profile, NetctlInterface *interface,
                                    const bool useHelper, const bool debug = false);
// wireless connection
InterfaceAnswer connectToEssid(const QString essid, QMap<QString,QString> settings,
                               NetctlInterface *interface, const bool useHelper,
                               const bool debug = false);
// information
netctlInformation generalInformation(NetctlInterface *interface, const bool useHelper,
                                     const bool debug = false);
QMap<QString, QString> profileInformation(const QString profile, NetctlInterface *interface,
                                          const bool useHelper, const bool debug = false);
netctlCurrent trayInformation(NetctlInterface *interface, const bool useHelper,
                              const bool debug = false);


#endif /* CALLS_H */
