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

#include "netctladaptor.h"


NetctlAdaptor::NetctlAdaptor(QObject *parent, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent)
{
    netctlCommand = new Netctl(false, configuration);
    netctlProfile = new NetctlProfile(false, configuration);
    wpaCommand = new WpaSup(false, configuration);
}


NetctlAdaptor::~NetctlAdaptor()
{
    delete netctlCommand;
    delete netctlProfile;
    delete wpaCommand;
}


// netctlCommand
QString NetctlAdaptor::ActiveProfile()
{
    if (isNetctlAutoActive())
        return netctlCommand->autoGetActiveProfile();
    else
        return netctlCommand->getActiveProfile();
}


QString NetctlAdaptor::ActiveProfileStatus()
{
    if (isNetctlAutoActive())
        return QString("netctl-auto");
    else
        return netctlCommand->getProfileStatus(ActiveProfile());
}


bool NetctlAdaptor::autoIsProfileActive(const QString profile)
{
    return netctlCommand->autoIsProfileActive(profile);
}


bool NetctlAdaptor::autoIsProfileEnabled(const QString profile)
{
    return netctlCommand->autoIsProfileEnabled(profile);
}


QStringList NetctlAdaptor::Information()
{
    QStringList output;
    output.append(ActiveProfile());
    output.append(ActiveProfileStatus());

    return output;
}


bool NetctlAdaptor::isNetctlAutoActive()
{
    return netctlCommand->isNetctlAutoRunning();
}


bool NetctlAdaptor::isNetctlAutoEnabled()
{
    return netctlCommand->isNetctlAutoEnabled();
}


bool NetctlAdaptor::isProfileActive(const QString profile)
{
    return netctlCommand->isProfileActive(profile);
}


bool NetctlAdaptor::isProfileEnabled(const QString profile)
{
    return netctlCommand->isProfileEnabled(profile);
}


QStringList NetctlAdaptor::ProfileList()
{
    QList<netctlProfileInfo> profilesInfo;
    if (isNetctlAutoActive())
        profilesInfo = netctlCommand->getProfileListFromNetctlAuto();
    else
        profilesInfo = netctlCommand->getProfileList();
    QStringList info;
    for (int i=0; i<profilesInfo.count(); i++) {
        QStringList profileInfo;
        profileInfo.append(profilesInfo[i].name);
        profileInfo.append(profilesInfo[i].description);
        profileInfo.append(QString::number(profilesInfo[i].active));
        profileInfo.append(QString::number(profilesInfo[i].enabled));
        info.append(profileInfo.join(QChar('|')));
    }

    return info;
}


// netctlProfile
QStringList NetctlAdaptor::Profile(const QString profile)
{
    QMap<QString, QString> settings = netctlProfile->getSettingsFromProfile(profile);
    QStringList settingsList;
    for (int i=0; i<settings.keys().count(); i++)
        settingsList.append(settings.keys()[i] + QString("==") +
                            settings[settings.keys()[i]]);

    return settingsList;
}


QString NetctlAdaptor::ProfileValue(const QString profile, const QString key)
{
    return netctlProfile->getValueFromProfile(profile, key);
}


// wpaCommand
QString NetctlAdaptor::ProfileByEssid(const QString essid)
{
    return wpaCommand->existentProfile(essid);
}


QStringList NetctlAdaptor::WirelessInterfaces()
{
    return netctlCommand->getWirelessInterfaceList();
}
