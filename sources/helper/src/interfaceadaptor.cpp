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

#include "interfaceadaptor.h"

#include <QTextCodec>
#include <unistd.h>

#include <listmap/listmap.h>

#include "netctlhelper.h"
#include "version.h"


InterfaceAdaptor::InterfaceAdaptor(QObject *parent, const bool debugCmd, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent),
      debug(debugCmd)
{
    netctlInterface = new NetctlInterface(debug, configuration);
}


InterfaceAdaptor::~InterfaceAdaptor()
{
    if (netctlInterface != nullptr) delete netctlInterface;
}


// control slots
int InterfaceAdaptor::Create(const QString profile, const QStringList settingsList)
{
    return netctlInterface->createProfile(profile, listToMap(settingsList));
}


int InterfaceAdaptor::Enable(const QString profile)
{
    return netctlInterface->enableProfile(profile);
}


int InterfaceAdaptor::Essid(const QString essid, QStringList settingsList)
{
    return netctlInterface->connectToEssid(essid, listToMap(settingsList));
}


int InterfaceAdaptor::KnownEssid(const QString essid)
{
    return netctlInterface->connectToKnownEssid(essid);
}


int InterfaceAdaptor::Remove(const QString profile)
{
    return netctlInterface->removeProfile(profile);
}


int InterfaceAdaptor::Restart(const QString profile)
{
    return netctlInterface->restartProfile(profile);
}


int InterfaceAdaptor::Start(const QString profile)
{
    return netctlInterface->startProfile(profile);
}


int InterfaceAdaptor::StopAll()
{
    return netctlInterface->stopAllProfiles();
}


int InterfaceAdaptor::SwitchTo(const QString profile)
{
    return netctlInterface->switchToProfile(profile);
}


int InterfaceAdaptor::UnknownEssid(const QString essid, QStringList settingsList)
{
    return netctlInterface->connectToUnknownEssid(essid, listToMap(settingsList));
}


// information
QStringList InterfaceAdaptor::Information()
{
    netctlInformation information = netctlInterface->information();
    QStringList info;
    info.append(QString::number(information.netctlAuto));
    info.append(QString::number(information.netctlAutoEnabled));

    QList<netctlProfileInfo> profiles = information.netctlProfiles;
    profiles.append(information.netctlAutoProfiles);
    for (int i=0; i<profiles.count(); i++) {
        QStringList profileInfo;
        profileInfo.append(profiles[i].name);
        profileInfo.append(profiles[i].description);
        profileInfo.append(profiles[i].type);
        profileInfo.append(profiles[i].interface);
        profileInfo.append(profiles[i].essid);
        profileInfo.append(QString::number(profiles[i].active));
        profileInfo.append(QString::number(profiles[i].enabled));
        profileInfo.append(QString::number(profiles[i].netctlAuto));
        info.append(profileInfo.join(QChar('|')));
    }

    return info;
}


QStringList InterfaceAdaptor::Profile(const QString profile)
{
    QMap<QString, QString> settings = netctlInterface->profileSettings(profile);

    return mapToList(settings);
}


QStringList InterfaceAdaptor::Status()
{
    netctlCurrent current = netctlInterface->status();
    QStringList info;
    info.append(QString::number(current.netctlAuto));
    info.append(current.profiles.join(QChar('|')));
    info.append(current.current.join(QChar('|')));
    QStringList enables;
    for (int i=0; i<current.enables.count(); i++)
        enables.append(QString::number(current.enables[i]));
    info.append(enables.join(QChar('|')));

    return info;
}
