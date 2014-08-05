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
/**
 * @file netctlinteract.cpp
 * Source code of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#include <QDebug>

#include "netctlgui.h"
#include "taskadds.h"


/**
 * @class Netctl
 */
/**
 * @fn Netctl
 */
Netctl::Netctl(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
    netctlProfile = new NetctlProfile(debug, settings);

    if (settings.contains(QString("IFACE_DIR")))
        ifaceDirectory = new QDir(settings[QString("IFACE_DIR")]);
    else
        ifaceDirectory = new QDir(QString("/sys/class/net/"));
    if (settings.contains(QString("PREFERED_IFACE")))
        mainInterface = settings[QString("PREFERED_IFACE")];
    else
        mainInterface = QString("");
    if (settings.contains(QString("NETCTL_PATH")))
        netctlCommand = settings[QString("NETCTL_PATH")];
    else
        netctlCommand = QString("/usr/bin/netctl");
    if (settings.contains(QString("NETCTLAUTO_PATH")))
        netctlAutoCommand = settings[QString("NETCTLAUTO_PATH")];
    else
        netctlAutoCommand = QString("/usr/bin/netctl-auto");
    if (settings.contains(QString("NETCTLAUTO_SERVICE")))
        netctlAutoService = settings[QString("NETCTLAUTO_SERVICE")];
    else
        netctlAutoService = QString("netctl-auto");
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    else
        sudoCommand = QString("/usr/bin/kdesu");
    if (settings.contains(QString("SYSTEMCTL_PATH")))
        systemctlCommand = settings[QString("SYSTEMCTL_PATH")];
    else
        systemctlCommand = QString("/usr/bin/systemctl");
}


/**
 * @fn ~Netctl
 */
Netctl::~Netctl()
{
    if (debug) qDebug() << "[Netctl]" << "[~Netctl]";

    if (netctlProfile != 0)
        delete netctlProfile;
    if (ifaceDirectory != 0)
        delete ifaceDirectory;
}


// functions
/**
 * @fn cmdCall
 */
bool Netctl::cmdCall(const bool sudo, const QString command, const QString commandLine, const QString argument)
{
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]";
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Command" << command;
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Command line" << commandLine;
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Argument" << argument;
    if (command == 0) {
        if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Could not find command";
        return false;
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Could not find sudo";
        return false;
    }

    QString cmd = QString("");
    if (sudo) cmd = sudoCommand + QString(" ");
    cmd += command + QString(" ") + commandLine;
    if (argument != 0) cmd += QString(" ") + argument;
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << "[Netctl]" << "[cmdCall]" << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}


/**
 * @fn getCmdOutput
 */
QString Netctl::getCmdOutput(const bool sudo, const QString command, const QString commandLine, const QString argument)
{
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]";
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Command" << command;
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Command line" << commandLine;
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Argument" << argument;
    if (command == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Could not find command";
        return QString();
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Could not find sudo";
        return QString();
    }

    QString cmd = QString("");
    if (sudo) cmd = sudoCommand + QString(" ");
    cmd += command + QString(" ") + commandLine;
    if (argument != 0) cmd += QString(" ") + argument;
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << "[Netctl]" << "[getCmdOutput]" << ":" << "Cmd returns" << process.exitCode;

    return process.output;
}


// general information
/**
 * @fn getProfileList
 */
QList<netctlProfileInfo> Netctl::getProfileList()
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileList]";

    QList<netctlProfileInfo> fullProfilesInfo;
    QStringList output = getCmdOutput(false, netctlCommand, QString("list"))
            .split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<output.count(); i++) {
        netctlProfileInfo profileInfo;
        profileInfo.name = output[i].mid(2, -1);
        profileInfo.description = getProfileDescription(profileInfo.name);
        profileInfo.status = getProfileStatus(profileInfo.name);
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileListFromNetctlAuto
 */
QList<netctlProfileInfo> Netctl::getProfileListFromNetctlAuto()
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileListFromNetctlAuto]";

    QList<netctlProfileInfo> fullProfilesInfo;
    QStringList output = getCmdOutput(false, netctlAutoCommand, QString("list"))
            .split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<output.count(); i++) {
        netctlProfileInfo profileInfo;
        profileInfo.name = output[i].mid(2, -1);
        profileInfo.description = getProfileDescription(profileInfo.name);
        profileInfo.status = output[i].left(1);
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileDescription
 */
QString Netctl::getProfileDescription(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]";
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]" << ":" << "Profile" << profile;
    if (netctlProfile == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]" << ":" << "Could not find library";
        return QString();
    }

    return netctlProfile->getValueFromProfile(profile, QString("Description"));
}


/**
 * @fn getProfileStatus
 */
QString Netctl::getProfileStatus(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileStatus]";
    if (debug) qDebug() << "[Netctl]" << "[getProfileStatus]" << ":" << "Profile" << profile;

    QString status;
    if (isProfileActive(profile))
        status = QString("active");
    else
        status = QString("inactive");
    if (isProfileEnabled(profile))
        status = status + QString(" (enabled)");
    else
        status = status + QString(" (static)");

    return status;
}


/**
 * @fn isProfileActive
 */
bool Netctl::isProfileActive(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[isProfileActive]";
    if (debug) qDebug() << "[Netctl]" << "[isProfileActive]" << ":" << "Profile" << profile;

    bool status = false;
    QString output = getCmdOutput(false, netctlCommand, QString("status"), profile);
    if (output.contains(QString("Active: active")))
        status = true;

    return status;
}


/**
 * @fn isProfileEnabled
 */
bool Netctl::isProfileEnabled(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[isProfileEnabled]";
    if (debug) qDebug() << "[Netctl]" << "[isProfileEnabled]" << ":" << "Profile" << profile;

    return cmdCall(false, netctlCommand, QString("is-enabled"), profile);
}


/**
 * @fn autoIsProfileActive
 */
bool Netctl::autoIsProfileActive(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileActive]";
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileActive]" << ":" << "Profile" << profile;

    bool status = false;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i].name == profile) && (profiles[i].status == QString("*"))) {
            status = true;
            break;
        }

    return status;
}


/**
 * @fn autoIsProfileEnabled
 */
bool Netctl::autoIsProfileEnabled(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileEnabled]";
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileEnabled]" << ":" << "Profile" << profile;

    bool status = true;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i].name == profile) && (profiles[i].status == QString("!"))) {
            status = false;
            break;
        }

    return status;
}


/**
 * @fn isNetctlAutoEnabled
 */
bool Netctl::isNetctlAutoEnabled()
{
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]";
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]" << ":" << "Could not find service";
        return false;
    }
    if (getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]" << ":" << "Could not interface";
        return false;
    }

    QString interface = getWirelessInterfaceList()[0];
    QString argument = netctlAutoService + QString("@") + interface + QString(".service");
    QString output = getCmdOutput(false, systemctlCommand, QString("is-enabled"), argument).trimmed();

    if (output == QString("enabled"))
        return true;
    else
        return false;
}


/**
 * @fn isNetctlAutoRunning
 */
bool Netctl::isNetctlAutoRunning()
{
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]";
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]" << ":" << "Could not find service";
        return false;
    }
    if (getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]" << ":" << "Could not interface";
        return false;
    }

    QString interface = getWirelessInterfaceList()[0];
    QString argument = netctlAutoService + QString("@") + interface + QString(".service");
    QString output = getCmdOutput(false, systemctlCommand, QString("is-active"), argument).trimmed();

    if (output == QString("active"))
        return true;
    else
        return false;
}


/**
 * @fn getWirelessInterfaceList
 */
QStringList Netctl::getWirelessInterfaceList()
{
    if (debug) qDebug() << "[Netctl]" << "[getWirelessInterfaceList]";
    if (ifaceDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getWirelessInterfaceList]" << ":" << "Could not find directory";
        return QStringList();
    }

    QStringList interfaces;
    if (!mainInterface.isEmpty())
        interfaces.append(mainInterface);
    QStringList allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i=0; i<allInterfaces.count(); i++) {
        if (debug) qDebug() << "[Netctl]" << "[getWirelessInterfaceList]" << ":" << "Check directory"
                 << ifaceDirectory->path() + QDir::separator() + allInterfaces[i] + QDir::separator() + QString("wireless");
        if (QDir(ifaceDirectory->path() + QDir::separator() + allInterfaces[i] +
                 QDir::separator() + QString("wireless")).exists())
            interfaces.append(allInterfaces[i]);
    }

    return interfaces;
}


// functions
/**
 * @fn enableProfile
 */
bool Netctl::enableProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[enableProfile]";
    if (debug) qDebug() << "[Netctl]" << "[enableProfile]" << ":" << "Profile" << profile;

    if (isProfileEnabled(profile))
        return cmdCall(true, netctlCommand, QString("disable"), profile);
    else
        return cmdCall(true, netctlCommand, QString("enable"), profile);
}


/**
 * @fn restartProfile
 */
bool Netctl::restartProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[restartProfile]";
    if (debug) qDebug() << "[Netctl]" << "[restartProfile]" << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("restart"), profile);
}


/**
 * @fn startProfile
 */
bool Netctl::startProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[startProfile]";
    if (debug) qDebug() << "[Netctl]" << "[startProfile]" << ":" << "Profile" << profile;

    if (isProfileActive(profile))
        return cmdCall(true, netctlCommand, QString("stop"), profile);
    else
        return cmdCall(true, netctlCommand, QString("start"), profile);
}


/**
 * @fn autoDisableAllProfiles
 */
bool Netctl::autoDisableAllProfiles()
{
    if (debug) qDebug() << "[Netctl]" << "[autoDisableAllProfiles]";

    return cmdCall(false, netctlAutoCommand, QString("disable-all"));
}


/**
 * @fn autoEnableProfile
 */
bool Netctl::autoEnableProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableProfile]";
    if (debug) qDebug() << "[Netctl]" << "[autoEnableProfile]" << ":" << "Profile" << profile;

    if (autoIsProfileEnabled(profile))
        return cmdCall(false, netctlAutoCommand, QString("disable"), profile);
    else
        return cmdCall(false, netctlAutoCommand, QString("enable"), profile);
}


/**
 * @fn autoEnableAllProfiles
 */
bool Netctl::autoEnableAllProfiles()
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableAllProfiles]";

    return cmdCall(false, netctlAutoCommand, QString("enable-all"));
}


/**
 * @fn autoStartProfile
 */
bool Netctl::autoStartProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoStartProfile]";
    if (debug) qDebug() << "[Netctl]" << "[autoStartProfile]" << ":" << "Profile" << profile;

    if (autoIsProfileActive(profile))
        return true;
    else
        return cmdCall(false, netctlAutoCommand, QString("switch-to"), profile);
}


/**
 * @fn autoEnableService
 */
bool Netctl::autoEnableService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableService]";
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[autoEnableService]" << ":" << "Could not find service";
        return false;
    }
    if (getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[Netctl]" << "[autoEnableService]" << ":" << "Could not interface";
        return false;
    }

    QString interface = getWirelessInterfaceList()[0];
    QString argument = netctlAutoService + QString("@") + interface + QString(".service");

    if (isNetctlAutoEnabled())
        return cmdCall(true, systemctlCommand, QString("disable"), argument);
    else
        return cmdCall(true, systemctlCommand, QString("enable"), argument);
}


/**
 * @fn autoRestartService
 */
bool Netctl::autoRestartService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoRestartService]";
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[autoRestartService]" << ":" << "Could not find service";
        return false;
    }
    if (getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[Netctl]" << "[autoRestartService]" << ":" << "Could not interface";
        return false;
    }

    QString interface = getWirelessInterfaceList()[0];
    QString argument = netctlAutoService + QString("@") + interface + QString(".service");

    if (isNetctlAutoRunning())
        return cmdCall(true, systemctlCommand, QString("restart"), argument);
    else
        return true;
}


/**
 * @fn autoStartService
 */
bool Netctl::autoStartService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoStartService]";
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[autoStartService]" << ":" << "Could not find service";
        return false;
    }
    if (getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[Netctl]" << "[autoStartService]" << ":" << "Could not interface";
        return false;
    }

    QString interface = getWirelessInterfaceList()[0];
    QString argument = netctlAutoService + QString("@") + interface + QString(".service");

    if (isNetctlAutoRunning())
        return cmdCall(true, systemctlCommand, QString("stop"), argument);
    else
        return cmdCall(true, systemctlCommand, QString("start"), argument);
}
