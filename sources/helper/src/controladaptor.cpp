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


#include "netctlhelper.h"
#include "controladaptor.h"


ControlAdaptor::ControlAdaptor(NetctlHelper *parent, const QMap<QString, QString> configuration)
    : QDBusAbstractAdaptor(parent),
      helper(parent)

{
    netctlCommand = new Netctl(false, configuration);
    netctlProfile = new NetctlProfile(false, configuration);
}


ControlAdaptor::~ControlAdaptor()
{
    delete netctlCommand;
    delete netctlProfile;
}


// helper
bool ControlAdaptor::Active()
{
    return true;
}


bool ControlAdaptor::Close()
{
    helper->quitHelper();
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
bool ControlAdaptor::Remove(const QString profile)
{
    return netctlProfile->removeProfile(profile);
}
