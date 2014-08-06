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


#include <QDebug>

#include <netctlgui/netctlgui.h>
#include "netctladaptor.h"


NetctlAdaptor::NetctlAdaptor(QObject *parent, const bool debugCmd, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent),
      debug(debugCmd)
{
    netctlCommand = new Netctl(debug, configuration);
    netctlProfile = new NetctlProfile(debug, configuration);
    wpaCommand = new WpaSup(debug, configuration);
}


NetctlAdaptor::~NetctlAdaptor()
{
    if (debug) qDebug() << "[NetctlAdaptor]" << "[~NetctlAdaptor]";

    delete netctlCommand;
    delete netctlProfile;
    delete wpaCommand;
}


QString NetctlAdaptor::Information()
{
    if (debug) qDebug() << "[NetctlAdaptor]" << "[Information]";
}
