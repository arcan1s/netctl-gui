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
#include <QDirIterator>

#include <netctlgui/netctlgui.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "version.h"


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

    ifaceDirectory = new QDir(settings.value(QString("IFACE_DIR"), QString(IFACE_DIR)));
    mainInterface = settings.value(QString("PREFERED_IFACE"), QString(PREFERED_IFACE));
    netctlCommand = settings.value(QString("NETCTL_PATH"), QString(NETCTL_PATH));
    netctlAutoCommand = settings.value(QString("NETCTLAUTO_PATH"), QString(NETCTLAUTO_PATH));
    netctlAutoService = settings.value(QString("NETCTLAUTO_SERVICE"), QString(NETCTLAUTO_SERVICE));
    sudoCommand = settings.value(QString("SUDO_PATH"), QString(SUDO_PATH));
    systemctlCommand = settings.value(QString("SYSTEMCTL_PATH"), QString(SYSTEMCTL_PATH));
    useSuid = (settings.value(QString("FORCE_SUDO"), QString("true")) != QString("true"));

    if (useSuid) sudoCommand = QString("");
}


/**
 * @fn ~Netctl
 */
Netctl::~Netctl()
{
    if (debug) qDebug() << PDEBUG;

    if (netctlProfile != nullptr) delete netctlProfile;
    if (ifaceDirectory != nullptr) delete ifaceDirectory;
}


// general information
/**
 * @fn getActiveProfile
 */
QStringList Netctl::getActiveProfile() const
{
    if (debug) qDebug() << PDEBUG;

    return getActiveProfileTemplate(getProfileList());
}


/**
 * @fn autoGetActiveProfile
 */
QStringList Netctl::autoGetActiveProfile() const
{
    if (debug) qDebug() << PDEBUG;

    return getActiveProfileTemplate(getProfileListFromNetctlAuto());
}


/**
 * @fn getProfileList
 */
QList<netctlProfileInfo> Netctl::getProfileList() const
{
    if (debug) qDebug() << PDEBUG;

    return getProfileListTemplate(false);
}


/**
 * @fn getProfileListFromNetctlAuto
 */
QList<netctlProfileInfo> Netctl::getProfileListFromNetctlAuto() const
{
    if (debug) qDebug() << PDEBUG;

    return getProfileListTemplate(true);
}


/**
 * @fn getProfileStatus
 */
QString Netctl::getProfileStatus(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString status = isProfileActive(profile) ? QString("active") : QString("inactive");
    status += isProfileEnabled(profile) ?  QString(" (enabled)") : QString(" (static)");

    return status;
}


/**
 * @fn isProfileActive
 */
bool Netctl::isProfileActive(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(false, netctlCommand, QString("status"), profile).status();
}


/**
 * @fn isProfileEnabled
 */
bool Netctl::isProfileEnabled(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(false, netctlCommand, QString("is-enabled"), profile).output == QString("enabled\n");
}


/**
 * @fn autoIsProfileActive
 */
bool Netctl::autoIsProfileActive(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool status = false;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    foreach(netctlProfileInfo profileInfo, profiles) {
        if (profileInfo.name != profile) continue;
        status = profileInfo.active;
        break;
    }

    return status;
}


/**
 * @fn autoIsProfileEnabled
 */
bool Netctl::autoIsProfileEnabled(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool status = false;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    foreach(netctlProfileInfo profileInfo, profiles) {
        if (profileInfo.name != profile) continue;
        status = profileInfo.enabled;
        break;
    }

    return status;
}


/**
 * @fn isNetctlAutoEnabled
 */
bool Netctl::isNetctlAutoEnabled() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlAutoService.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find service";
        return false;
    }
    QStringList interfaces = getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not interface";
        return false;
    }

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces.first());

    return cmdCall(false, systemctlCommand, QString("is-enabled"), argument).status();
}


/**
 * @fn isNetctlAutoRunning
 */
bool Netctl::isNetctlAutoRunning() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlAutoService.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find service";
        return false;
    }
    QStringList interfaces = getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not interface";
        return false;
    }

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces.first());

    return cmdCall(false, systemctlCommand, QString("is-active"), argument).status();
}


/**
 * @fn getRecommendedConfiguration
 */
QMap<QString, QString> Netctl::getRecommendedConfiguration()
{
    QMap<QString, QString> settings;
    TaskResult process;
    QStringList recommended;
    // force sudo
    // find out helper exe
    settings[QString("FORCE_SUDO")] = QString("true");
    recommended.clear();
    recommended.append(QString("netctlgui-helper"));
    recommended.append(QString("netctlgui-helper-suid"));
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("FORCE_SUDO")] = QString("false");
            break;
        }
    }
    // interfaces
    // find out dircetory which contains wireless subdirectory
    // I don't think that this parametr may change =)
    settings[QString("IFACE_DIR")] = QString("/sys/class/net");
    settings[QString("PREFERED_IFACE")] = QString("");
    QDirIterator sysIterator(QDir("/sys"), QDirIterator::Subdirectories);
    while (sysIterator.hasNext()) {
        sysIterator.next();
        if (!sysIterator.fileInfo().isDir()) continue;
        QString name = sysIterator.filePath();
        if (name.contains(QString("wireless"))) {
            QString interfaceDir = QFileInfo(name).path();
            settings[QString("PREFERED_IFACE")] = QFileInfo(interfaceDir).fileName();
            break;
        }
    }
    // netctl path
    // find out netctl exe
    settings[QString("NETCTL_PATH")] = QString("");
    recommended.clear();
    recommended.append("netctl");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("NETCTL_PATH")] = process.output.trimmed();
            break;
        }
    }
    // netctl-auto path
    // find out netctl-auto exe
    settings[QString("NETCTLAUTO_PATH")] = QString("");
    recommended.clear();
    recommended.append("netctl-auto");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("NETCTLAUTO_PATH")] = process.output.trimmed();
            break;
        }
    }
    // netctl-auto service
    // usually it has the same name as netctl-auto
    settings[QString("NETCTLAUTO_SERVICE")] = QFileInfo(settings[QString("NETCTLAUTO_PATH")]).fileName();
    // profile path
    // find out netctl directory into /etc
    settings[QString("PROFILE_DIR")] = QString("");
    QDirIterator iterator(QDir("/etc"), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) continue;
        QString name = iterator.filePath();
        if (name.contains(QString("netctl"))) {
            settings[QString("PROFILE_DIR")] = name;
            break;
        }
    }
    // sudo path
    // find out sudo, kdesu, gksu exes
    settings[QString("SUDO_PATH")] = QString("");
    recommended.clear();
    recommended.append("sudo");
    recommended.append("kdesu");
    recommended.append("gksu");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("SUDO_PATH")] = process.output.trimmed();
            break;
        }
    }
    // systemctl path
    // find out systemctl exe
    settings[QString("SYSTEMCTL_PATH")] = QString("");
    recommended.clear();
    recommended.append("systemctl");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("SYSTEMCTL_PATH")] = process.output.trimmed();
            break;
        }
    }

    return settings;
}


/**
 * @fn getWirelessInterfaceList
 */
QStringList Netctl::getWirelessInterfaceList() const
{
    if (debug) qDebug() << PDEBUG;
    if (ifaceDirectory == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return QStringList();
    }

    QStringList interfaces;
    if (!mainInterface.isEmpty())
        interfaces.append(mainInterface);
    QStringList allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach(QString interface, allInterfaces) {
        QString dir = QString("%1/%2/wireless").arg(ifaceDirectory->path()).arg(interface);
        if (debug) qDebug() << PDEBUG << ":" << "Check directory" << dir;
        if (QDir(dir).exists())
            interfaces.append(interface);
    }

    return interfaces;
}


// functions
/**
 * @fn enableProfile
 */
bool Netctl::enableProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand,
                   isProfileEnabled(profile) ? QString("disable") : QString("enable"),
                   profile).status();
}


/**
 * @fn forceStartProfile
 */
bool Netctl::forceStartProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("start"), profile).status();
}


/**
 * @fn forceStopProfile
 */
bool Netctl::forceStopProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("stop"), profile).status();
}


/**
 * @fn reenableProfile
 */
bool Netctl::reenableProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("reenable"), profile).status();
}


/**
 * @fn restartProfile
 */
bool Netctl::restartProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("restart"), profile).status();
}


/**
 * @fn startProfile
 */
bool Netctl::startProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand,
                   isProfileActive(profile) ? QString("stop") : QString("start"),
                   profile).status();
}


/**
 * @fn stopAllProfiles
 */
bool Netctl::stopAllProfiles() const
{
    if (debug) qDebug() << PDEBUG;

    return cmdCall(true, netctlCommand, QString("stop-all")).status();
}


/**
 * @fn switchToProfile
 */
bool Netctl::switchToProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return ((isProfileActive(profile)) ||
            (cmdCall(true, netctlCommand, QString("switch-to"), profile)).status());
}


/**
 * @fn autoDisableAllProfiles
 */
bool Netctl::autoDisableAllProfiles() const
{
    if (debug) qDebug() << PDEBUG;

    return cmdCall(false, netctlAutoCommand, QString("disable-all")).status();
}


/**
 * @fn autoEnableProfile
 */
bool Netctl::autoEnableProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(false, netctlAutoCommand,
                   autoIsProfileEnabled(profile) ? QString("disable") : QString("enable"),
                   profile).status();
}


/**
 * @fn autoEnableAllProfiles
 */
bool Netctl::autoEnableAllProfiles() const
{
    if (debug) qDebug() << PDEBUG;

    return cmdCall(false, netctlAutoCommand, QString("enable-all")).status();
}


/**
 * @fn autoStartProfile
 */
bool Netctl::autoStartProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return ((autoIsProfileActive(profile)) ||
            (cmdCall(false, netctlAutoCommand, QString("switch-to"), profile)).status());
}


/**
 * @fn autoEnableService
 */
bool Netctl::autoEnableService() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlAutoService.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find service";
        return false;
    }
    QStringList interfaces = getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not interface";
        return false;
    }

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces.first());

    return cmdCall(true, systemctlCommand,
                   isNetctlAutoEnabled() ? QString("disable") : QString("enable"),
                   argument).status();
}


/**
 * @fn autoRestartService
 */
bool Netctl::autoRestartService() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlAutoService.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find service";
        return false;
    }
    QStringList interfaces = getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not interface";
        return false;
    }

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces.first());

    return ((!isNetctlAutoRunning()) ||
            (cmdCall(true, systemctlCommand, QString("restart"), argument)).status());
}


/**
 * @fn autoStartService
 */
bool Netctl::autoStartService() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlAutoService.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find service";
        return false;
    }
    QStringList interfaces = getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not interface";
        return false;
    }

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces.first());

    return cmdCall(true, systemctlCommand,
                   isNetctlAutoRunning() ? QString("stop") : QString("start"),
                   argument).status();
}


// functions
/**
 * @fn cmdCall
 */
TaskResult Netctl::cmdCall(const bool sudo, const QString command,
                           const QString commandLine, const QString argument) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << command;
    if (debug) qDebug() << PDEBUG << ":" << "Command line" << commandLine;
    if (debug) qDebug() << PDEBUG << ":" << "Argument" << argument;

    QString cmd = QString("");
    if (sudo) cmd = QString("%1 ").arg(sudoCommand);
    cmd += QString("%1 %2").arg(command).arg(commandLine);
    if (!argument.isEmpty())  cmd += QString(" \"%1\"").arg(argument);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, (useSuid && sudo));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process;
};


/**
 * @fn getActiveProfileTemplate
 */
QStringList Netctl::getActiveProfileTemplate(const QList<netctlProfileInfo> data) const
{
    if (debug) qDebug() << PDEBUG;

    QStringList profiles;
    foreach(netctlProfileInfo profile, data) {
        if (!profile.active) continue;
        profiles.append(profile.name);
    }

    return profiles;
}


/**
 * @fn getProfileListTemplate
 */
QList<netctlProfileInfo> Netctl::getProfileListTemplate(const bool isAuto) const
{
    if (debug) qDebug() << PDEBUG;

    QList<netctlProfileInfo> fullProfilesInfo;
    QStringList output = QString(cmdCall(false, isAuto ? netctlAutoCommand : netctlCommand,
                                         QString("list")).output)
                                 .split(QChar('\n'), QString::SkipEmptyParts);
    foreach(QString profile, output) {
        netctlProfileInfo profileInfo;
        profileInfo.name = profile.mid(2, -1);
        profileInfo.active = (profile[0] == QChar('*'));
        profileInfo.enabled = isAuto ? (profile[0] != QChar('!')) : isProfileEnabled(profileInfo.name);
        // external
        QStringList keys;
        keys.append(QString("Connection"));
        keys.append(QString("Description"));
        keys.append(QString("ESSID"));
        keys.append(QString("Interface"));
        QStringList profileValues = netctlProfile->getValuesFromProfile(profileInfo.name,
                                                                        keys);
        profileInfo.description = profileValues[1];
        profileInfo.essid = profileValues[2];
        profileInfo.interface = profileValues[3];
        profileInfo.type = profileValues[0];
        profileInfo.netctlAuto = isAuto;
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}
