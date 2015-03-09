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

#include "controladaptor.h"

#include <QTextCodec>
#include <unistd.h>

#include "netctlhelper.h"
#include "version.h"


ControlAdaptor::ControlAdaptor(NetctlHelper *parent, const bool debugCmd, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent),
      debug(debugCmd),
      helper(parent)

{
    netctlCommand = new Netctl(debug, configuration);
    netctlProfile = new NetctlProfile(debug, configuration);
    wpaCommand = new WpaSup(debug, configuration);
}


ControlAdaptor::~ControlAdaptor()
{
    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
    if (wpaCommand != nullptr) delete wpaCommand;
}


// helper
bool ControlAdaptor::Active()
{
    return true;
}


QString ControlAdaptor::ApiDocs()
{
    return (QString("%1netctl-gui-dbus-api.html").arg(QString(DOCS_PATH)));
}


bool ControlAdaptor::Close()
{
    helper->quitHelper();
    return true;
}


QStringList ControlAdaptor::FindSettings()
{
    QMap<QString, QString> configuration;
    // apply settings from Netctl class
    QMap<QString, QString> librarySettings = netctlCommand->getRecommendedConfiguration();
    for (int i=0; i<librarySettings.keys().count(); i++)
        configuration[librarySettings.keys()[i]] = librarySettings[librarySettings.keys()[i]];
    // apply settings from NetctlProfile class
    librarySettings = netctlProfile->getRecommendedConfiguration();
    for (int i=0; i<librarySettings.keys().count(); i++)
        configuration[librarySettings.keys()[i]] = librarySettings[librarySettings.keys()[i]];
    // apply settings from WpaSup class
    librarySettings = wpaCommand->getRecommendedConfiguration();
    for (int i=0; i<librarySettings.keys().count(); i++)
        configuration[librarySettings.keys()[i]] = librarySettings[librarySettings.keys()[i]];

    QStringList settingsList;
    for (int i=0; i<configuration.keys().count(); i++)
        settingsList.append(QString("%1==%2").arg(configuration.keys()[i]).arg(configuration[configuration.keys()[i]]));

    return settingsList;
}


QString ControlAdaptor::LibraryDocs()
{
    return (QString("%1html/index.html").arg(QString(DOCS_PATH)));
}


QString ControlAdaptor::Pony()
{
    QString pony;
    QFile ponyFile(QString(":pinkiepie"));
    if (!ponyFile.open(QIODevice::ReadOnly)) return pony;
    pony = QTextCodec::codecForMib(106)->toUnicode(ponyFile.readAll());
    ponyFile.close();

    return pony;
}


QString ControlAdaptor::SecurityDocs()
{
    return (QString("%1netctl-gui-security-notes.html").arg(QString(DOCS_PATH)));
}


QStringList ControlAdaptor::Settings()
{
    return helper->printSettings();
}


QStringList ControlAdaptor::UIDs()
{
    QStringList uids;
    uids.append(QString::number(getuid()));
    uids.append(QString::number(geteuid()));

    return uids;
}


bool ControlAdaptor::Update()
{
    helper->updateConfiguration();
    return true;
}


// netctlCommand
bool ControlAdaptor::autoDisableAll()
{
    return netctlCommand->autoDisableAllProfiles();
}


bool ControlAdaptor::autoEnable(const QString profile)
{
    return netctlCommand->autoEnableProfile(profile);
}


bool ControlAdaptor::autoEnableAll()
{
    return netctlCommand->autoEnableAllProfiles();
}


bool ControlAdaptor::autoStart(const QString profile)
{
    return netctlCommand->autoStartProfile(profile);
}


bool ControlAdaptor::autoServiceEnable()
{
    return netctlCommand->autoEnableService();
}


bool ControlAdaptor::autoServiceRestart()
{
    return netctlCommand->autoRestartService();
}


bool ControlAdaptor::autoServiceStart()
{
    return netctlCommand->autoStartService();
}


bool ControlAdaptor::Enable(const QString profile)
{
    return netctlCommand->enableProfile(profile);
}


bool ControlAdaptor::forceStart(const QString profile)
{
    return netctlCommand->forceStartProfile(profile);
}


bool ControlAdaptor::forceStop(const QString profile)
{
    return netctlCommand->forceStopProfile(profile);
}


bool ControlAdaptor::Reenable(const QString profile)
{
    return netctlCommand->reenableProfile(profile);
}


bool ControlAdaptor::Restart(const QString profile)
{
    return netctlCommand->restartProfile(profile);
}


bool ControlAdaptor::Start(const QString profile)
{
    return netctlCommand->startProfile(profile);
}


bool ControlAdaptor::StopAll()
{
    return netctlCommand->stopAllProfiles();
}


bool ControlAdaptor::SwitchTo(const QString profile)
{
    return netctlCommand->switchToProfile(profile);
}


// netctlProfile
bool ControlAdaptor::Create(const QString profile, const QStringList settingsList)
{
    QMap<QString, QString> settings;
    for (int i=0; i<settingsList.count(); i++) {
        if (!settingsList[i].contains(QString("=="))) continue;
        QString key = settingsList[i].split(QString("=="))[0];
        QString value = settingsList[i].split(QString("=="))[1];
        settings[key] = value;
    }
    QString temporaryProfile = netctlProfile->createProfile(profile, settings);

    return netctlProfile->copyProfile(temporaryProfile);
}


bool ControlAdaptor::Remove(const QString profile)
{
    return netctlProfile->removeProfile(profile);
}


// wpaCommand
QString ControlAdaptor::CurrentWiFi()
{
    netctlWifiInfo wifiPoint = wpaCommand->current();
    QStringList point;
    point.append(wifiPoint.name);
    point.append(wifiPoint.security);
    point.append(QString::number(wifiPoint.type));
    QStringList freqList;
    for (int j=0; j<wifiPoint.frequencies.count(); j++)
        freqList.append(QString::number(wifiPoint.frequencies[j]));
    point.append(freqList.join(QChar(',')));
    point.append(wifiPoint.macs.join(QChar(',')));
    point.append(QString::number(wifiPoint.signal));
    point.append(QString::number(wifiPoint.active));
    point.append(QString::number(wifiPoint.exists));

    return point.join(QChar('|'));
}


QStringList ControlAdaptor::VerboseWiFi()
{
    QList<netctlWifiInfo> wifiPoints = wpaCommand->scanWifi();
    QStringList info;
    for (int i=0; i<wifiPoints.count(); i++) {
        QStringList point;
        point.append(wifiPoints[i].name);
        point.append(wifiPoints[i].security);
        point.append(QString::number(wifiPoints[i].type));
        QStringList freqList;
        for (int j=0; j<wifiPoints[i].frequencies.count(); j++)
            freqList.append(QString::number(wifiPoints[i].frequencies[j]));
        point.append(freqList.join(QChar(',')));
        point.append(wifiPoints[i].macs.join(QChar(',')));
        point.append(QString::number(wifiPoints[i].signal));
        point.append(QString::number(wifiPoints[i].active));
        point.append(QString::number(wifiPoints[i].exists));
        info.append(point.join(QChar('|')));
    }

    return info;
}


QStringList ControlAdaptor::WiFi()
{
    QList<netctlWifiInfo> wifiPoints = wpaCommand->scanWifi();
    QStringList info;
    for (int i=0; i<wifiPoints.count(); i++) {
        QStringList point;
        point.append(wifiPoints[i].name);
        point.append(wifiPoints[i].security);
        point.append(QString::number(wifiPoints[i].signal));
        point.append(QString::number(wifiPoints[i].active));
        point.append(QString::number(wifiPoints[i].exists));
        info.append(point.join(QChar('|')));
    }

    return info;
}
