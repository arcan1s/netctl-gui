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


int InterfaceAdaptor::autoEnable(const QString profile)
{
    return netctlInterface->autoEnableProfile(profile);
}


int InterfaceAdaptor::Create(const QString profile, const QStringList settingsList)
{
    QMap<QString, QString> settings;
    for (int i=0; i<settingsList.count(); i++) {
        if (!settingsList[i].contains(QString("=="))) continue;
        QString key = settingsList[i].split(QString("=="))[0];
        QString value = settingsList[i].split(QString("=="))[1];
        settings[key] = value;
    }

    return netctlInterface->createProfile(profile, settings);
}


int InterfaceAdaptor::Enable(const QString profile)
{
    return netctlInterface->enableProfile(profile);
}


int InterfaceAdaptor::Essid(const QString essid, QStringList settingsList)
{
    QMap<QString, QString> settings;
    for (int i=0; i<settingsList.count(); i++) {
        if (!settingsList[i].contains(QString("=="))) continue;
        QString key = settingsList[i].split(QString("=="))[0];
        QString value = settingsList[i].split(QString("=="))[1];
        settings[key] = value;
    }

    return netctlInterface->connectToEssid(essid, settings);
}


int InterfaceAdaptor::KnownEssid(const QString essid)
{
    return netctlInterface->connectToKnownEssid(essid);
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
    QMap<QString, QString> settings;
    for (int i=0; i<settingsList.count(); i++) {
        if (!settingsList[i].contains(QString("=="))) continue;
        QString key = settingsList[i].split(QString("=="))[0];
        QString value = settingsList[i].split(QString("=="))[1];
        settings[key] = value;
    }

    return netctlInterface->connectToUnknownEssid(essid, settings);
}
