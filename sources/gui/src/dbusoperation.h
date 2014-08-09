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

#ifndef DBUSOPERATION_H
#define DBUSOPERATION_H

#include <QList>
#include <QVariant>

#include <netctlgui/netctlgui.h>

QList<netctlProfileInfo> parseOutputNetctl(const QList<QVariant> raw,
                                           const bool debug = false);
QList<netctlWifiInfo> parseOutputWifi(const QList<QVariant> raw,
                                      const bool debug = false);
QList<QVariant> sendDBusRequest(const QString service, const QString path,
                                const QString interface, const QString cmd,
                                const QList<QVariant> args = QList<QVariant>(),
                                const bool system = true, const bool debug = false);


#endif /* DBUSOPERATION_H */
