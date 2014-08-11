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

#include "netctlhelper.h"
#include "version.h"


ControlAdaptor::ControlAdaptor(NetctlHelper *parent, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent),
      helper(parent)

{
    netctlCommand = new Netctl(false, configuration);
    netctlProfile = new NetctlProfile(false, configuration);
    wpaCommand = new WpaSup(false, configuration);
}


ControlAdaptor::~ControlAdaptor()
{
    delete netctlCommand;
    delete netctlProfile;
    delete wpaCommand;
}


// helper
bool ControlAdaptor::Active()
{
    return true;
}


QString ControlAdaptor::ApiDocs()
{
    return (QString(DOCS_PATH) + QString("netctl-gui-dbus-api.html"));
}


bool ControlAdaptor::Close()
{
    helper->quitHelper();
    return true;
}


QString ControlAdaptor::LibraryDocs()
{
    return (QString(DOCS_PATH) + QString("html/index.html"));
}


QString ControlAdaptor::Pony()
{
    QString pony;
    QFile ponyFile(QString(":pinkiepie"));
    if (!ponyFile.open(QIODevice::ReadOnly))
        return pony;
    pony = QTextCodec::codecForMib(106)->toUnicode(ponyFile.readAll());
    ponyFile.close();

    return pony;
}


QString ControlAdaptor::SecurityDocs()
{
    return (QString(DOCS_PATH) + QString("netctl-gui-security-notes.html"));
}


QStringList ControlAdaptor::Settings()
{
    return helper->printSettings();
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


bool ControlAdaptor::Restart(const QString profile)
{
    return netctlCommand->restartProfile(profile);
}


bool ControlAdaptor::Start(const QString profile)
{
    return netctlCommand->startProfile(profile);
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
QStringList ControlAdaptor::WiFi()
{
    QList<netctlWifiInfo> wifiPoints = wpaCommand->scanWifi();
    QStringList info;
    for (int i=0; i<wifiPoints.count(); i++) {
        QStringList point;
        point.append(wifiPoints[i].name);
        point.append(wifiPoints[i].security);
        point.append(wifiPoints[i].signal);
        point.append(QString::number(wifiPoints[i].active));
        point.append(QString::number(wifiPoints[i].exists));
        info.append(point.join(QChar('|')));
    }

    return info;
}
