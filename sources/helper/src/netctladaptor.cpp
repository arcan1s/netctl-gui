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
    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
    if (wpaCommand != nullptr) delete wpaCommand;
}


// netctlCommand
QString NetctlAdaptor::ActiveProfile()
{
    if (isNetctlAutoActive())
        return netctlCommand->autoGetActiveProfile();
    else
        return netctlCommand->getActiveProfile().join(QChar('|'));
}


QString NetctlAdaptor::ActiveProfileStatus()
{
    if (isNetctlAutoActive())
        return QString("netctl-auto");
    else {
        QStringList status;
        QStringList profiles = ActiveProfile().split(QChar('|'));
        for (int i=0; i<profiles.count(); i++)
            status.append(netctlCommand->getProfileStatus(profiles[i]));
        return status.join(QChar('|'));
    }
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


QString NetctlAdaptor::netctlActiveProfile()
{
    return netctlCommand->getActiveProfile().join(QChar('|'));
}


QStringList NetctlAdaptor::netctlProfileList()
{
    QList<netctlProfileInfo> profilesInfo = netctlCommand->getProfileList();
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


QStringList NetctlAdaptor::netctlVerboseProfileList()
{
    QList<netctlProfileInfo> profilesInfo = netctlCommand->getProfileList();
    QStringList info;
    for (int i=0; i<profilesInfo.count(); i++) {
        QStringList profileInfo;
        profileInfo.append(profilesInfo[i].name);
        profileInfo.append(profilesInfo[i].description);
        profileInfo.append(profilesInfo[i].type);
        profileInfo.append(profilesInfo[i].interface);
        profileInfo.append(profilesInfo[i].essid);
        profileInfo.append(QString::number(profilesInfo[i].active));
        profileInfo.append(QString::number(profilesInfo[i].enabled));
        info.append(profileInfo.join(QChar('|')));
    }

    return info;
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


QStringList NetctlAdaptor::VerboseProfileList()
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
        profileInfo.append(profilesInfo[i].type);
        profileInfo.append(profilesInfo[i].interface);
        profileInfo.append(profilesInfo[i].essid);
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
        settingsList.append(QString("%1==%2").arg(settings.keys()[i]).arg(settings[settings.keys()[i]]));

    return settingsList;
}


QString NetctlAdaptor::ProfileValue(const QString profile, const QString key)
{
    return netctlProfile->getValueFromProfile(profile, key);
}


QStringList NetctlAdaptor::ProfileValues(const QString profile, const QStringList keys)
{
    return netctlProfile->getValuesFromProfile(profile, keys);
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
