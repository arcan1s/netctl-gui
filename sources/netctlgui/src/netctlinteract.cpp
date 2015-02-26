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
    if (settings.contains(QString("NETCTL_PATH")))
        netctlCommand = settings[QString("NETCTL_PATH")];
    if (settings.contains(QString("NETCTLAUTO_PATH")))
        netctlAutoCommand = settings[QString("NETCTLAUTO_PATH")];
    if (settings.contains(QString("NETCTLAUTO_SERVICE")))
        netctlAutoService = settings[QString("NETCTLAUTO_SERVICE")];
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    if (settings.contains(QString("SYSTEMCTL_PATH")))
        systemctlCommand = settings[QString("SYSTEMCTL_PATH")];
    if (settings.contains(QString("FORCE_SUDO")))
        useSuid = (settings[QString("FORCE_SUDO")] != QString("true"));

    if (useSuid)
        sudoCommand = QString("");
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


// functions
/**
 * @fn cmdCall
 */
bool Netctl::cmdCall(const bool sudo, const QString command, const QString commandLine, const QString argument)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << command;
    if (debug) qDebug() << PDEBUG << ":" << "Command line" << commandLine;
    if (debug) qDebug() << PDEBUG << ":" << "Argument" << argument;
    if (command.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find command";
        return false;
    }

    QString cmd = QString("");
    if (sudo)
        cmd = QString("%1 ").arg(sudoCommand);
    cmd += QString("%1 %2").arg(command).arg(commandLine);
    if (!argument.isEmpty())
        cmd += QString(" \"%1\"").arg(argument);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, (useSuid && sudo));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


/**
 * @fn getCmdOutput
 */
QString Netctl::getCmdOutput(const bool sudo, const QString command, const QString commandLine, const QString argument)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << command;
    if (debug) qDebug() << PDEBUG << ":" << "Command line" << commandLine;
    if (debug) qDebug() << PDEBUG << ":" << "Argument" << argument;
    if (command.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find command";
        return QString();
    }

    QString cmd = QString("");
    if (sudo)
        cmd = QString("%1 ").arg(sudoCommand);
    cmd += QString("%1 %2").arg(command).arg(commandLine);
    if (!argument.isEmpty())
        cmd += QString(" \"%1\"").arg(argument);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, (useSuid && sudo));
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.output;
}


// general information
/**
 * @fn getActiveProfile
 */
QStringList Netctl::getActiveProfile()
{
    if (debug) qDebug() << PDEBUG;

    QStringList profile;
    QList<netctlProfileInfo> fullProfilesInfo = getProfileList();
    for (int i=0; i<fullProfilesInfo.count(); i++)
        if (fullProfilesInfo[i].active)
            profile.append(fullProfilesInfo[i].name);

    return profile;
}


/**
 * @fn autoGetActiveProfile
 */
QString Netctl::autoGetActiveProfile()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = QString("");
    QList<netctlProfileInfo> fullProfilesInfo = getProfileListFromNetctlAuto();
    for (int i=0; i<fullProfilesInfo.count(); i++)
        if (fullProfilesInfo[i].active) {
            profile = fullProfilesInfo[i].name;
            break;
        }

    return profile;
}


/**
 * @fn getProfileList
 */
QList<netctlProfileInfo> Netctl::getProfileList()
{
    if (debug) qDebug() << PDEBUG;

    QList<netctlProfileInfo> fullProfilesInfo;
    QStringList output = getCmdOutput(false, netctlCommand, QString("list"))
            .split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<output.count(); i++) {
        netctlProfileInfo profileInfo;
        profileInfo.name = output[i].mid(2, -1);
        profileInfo.description = getProfileDescription(profileInfo.name);
        profileInfo.active = (output[i][0] == QChar('*'));
        profileInfo.enabled = isProfileEnabled(profileInfo.name);
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileListFromNetctlAuto
 */
QList<netctlProfileInfo> Netctl::getProfileListFromNetctlAuto()
{
    if (debug) qDebug() << PDEBUG;

    QList<netctlProfileInfo> fullProfilesInfo;
    QStringList output = getCmdOutput(false, netctlAutoCommand, QString("list"))
            .split(QChar('\n'), QString::SkipEmptyParts);
    for (int i=0; i<output.count(); i++) {
        netctlProfileInfo profileInfo;
        profileInfo.name = output[i].mid(2, -1);
        profileInfo.description = getProfileDescription(profileInfo.name);
        profileInfo.active = (output[i][0] == QChar('*'));
        profileInfo.enabled = (output[i][0] != QChar('!'));
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileDescription
 */
QString Netctl::getProfileDescription(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (netctlProfile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QString();
    }

    return netctlProfile->getValueFromProfile(profile, QString("Description"));
}


/**
 * @fn getProfileStatus
 */
QString Netctl::getProfileStatus(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString status;
    if (isProfileActive(profile))
        status = QString("active");
    else
        status = QString("inactive");
    if (isProfileEnabled(profile))
        status += QString(" (enabled)");
    else
        status += QString(" (static)");

    return status;
}


/**
 * @fn isProfileActive
 */
bool Netctl::isProfileActive(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString output = getCmdOutput(false, netctlCommand, QString("status"), profile);

    return (output.contains(QString("Active: active")));
}


/**
 * @fn isProfileEnabled
 */
bool Netctl::isProfileEnabled(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(false, netctlCommand, QString("is-enabled"), profile);
}


/**
 * @fn autoIsProfileActive
 */
bool Netctl::autoIsProfileActive(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool status = false;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i].name == profile) && (profiles[i].active)) {
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
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    bool status = false;
    QList<netctlProfileInfo> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i].name == profile) && (profiles[i].enabled)) {
            status = true;
            break;
        }

    return status;
}


/**
 * @fn isNetctlAutoEnabled
 */
bool Netctl::isNetctlAutoEnabled()
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

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces[0]);

    return cmdCall(false, systemctlCommand, QString("is-enabled"), argument);
}


/**
 * @fn isNetctlAutoRunning
 */
bool Netctl::isNetctlAutoRunning()
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

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces[0]);

    return cmdCall(false, systemctlCommand, QString("is-active"), argument);
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
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
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
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
            settings[QString("NETCTL_PATH")] = process.output.trimmed();
            break;
        }
    }
    // netctl-auto path
    // find out netctl-auto exe
    settings[QString("NETCTLAUTO_PATH")] = QString("");
    recommended.clear();
    recommended.append("netctl-auto");
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
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
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
            settings[QString("SUDO_PATH")] = process.output.trimmed();
            break;
        }
    }
    // systemctl path
    // find out systemctl exe
    settings[QString("SYSTEMCTL_PATH")] = QString("");
    recommended.clear();
    recommended.append("systemctl");
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
            settings[QString("SYSTEMCTL_PATH")] = process.output.trimmed();
            break;
        }
    }

    return settings;
}


/**
 * @fn getWirelessInterfaceList
 */
QStringList Netctl::getWirelessInterfaceList()
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
    for (int i=0; i<allInterfaces.count(); i++) {
        QString dir = QString("%1%2%3%2wireless").arg(ifaceDirectory->path())
                                                 .arg(QDir::separator())
                                                 .arg(allInterfaces[i]);
        if (debug) qDebug() << PDEBUG << ":" << "Check directory" << dir;
        if (QDir(dir).exists())
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
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (isProfileEnabled(profile))
        return cmdCall(true, netctlCommand, QString("disable"), profile);
    else
        return cmdCall(true, netctlCommand, QString("enable"), profile);
}


/**
 * @fn forceStartProfile
 */
bool Netctl::forceStartProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("start"), profile);
}


/**
 * @fn forceStopProfile
 */
bool Netctl::forceStopProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("stop"), profile);
}


/**
 * @fn reenableProfile
 */
bool Netctl::reenableProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("reenable"), profile);
}


/**
 * @fn restartProfile
 */
bool Netctl::restartProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return cmdCall(true, netctlCommand, QString("restart"), profile);
}


/**
 * @fn startProfile
 */
bool Netctl::startProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    if (isProfileActive(profile))
        return cmdCall(true, netctlCommand, QString("stop"), profile);
    else
        return cmdCall(true, netctlCommand, QString("start"), profile);
}


/**
 * @fn stopAllProfiles
 */
bool Netctl::stopAllProfiles()
{
    if (debug) qDebug() << PDEBUG;

    return cmdCall(true, netctlCommand, QString("stop-all"));
}


/**
 * @fn switchToProfile
 */
bool Netctl::switchToProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return ((isProfileActive(profile)) ||
            (cmdCall(true, netctlCommand, QString("switch-to"), profile)));
}


/**
 * @fn autoDisableAllProfiles
 */
bool Netctl::autoDisableAllProfiles()
{
    if (debug) qDebug() << PDEBUG;

    return cmdCall(false, netctlAutoCommand, QString("disable-all"));
}


/**
 * @fn autoEnableProfile
 */
bool Netctl::autoEnableProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

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
    if (debug) qDebug() << PDEBUG;

    return cmdCall(false, netctlAutoCommand, QString("enable-all"));
}


/**
 * @fn autoStartProfile
 */
bool Netctl::autoStartProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    return ((autoIsProfileActive(profile)) ||
            (cmdCall(false, netctlAutoCommand, QString("switch-to"), profile)));
}


/**
 * @fn autoEnableService
 */
bool Netctl::autoEnableService()
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

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces[0]);

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

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces[0]);

    return ((!isNetctlAutoRunning()) ||
            (cmdCall(true, systemctlCommand, QString("restart"), argument)));
}


/**
 * @fn autoStartService
 */
bool Netctl::autoStartService()
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

    QString argument = QString("%1@%2.service").arg(netctlAutoService).arg(interfaces[0]);

    if (isNetctlAutoRunning())
        return cmdCall(true, systemctlCommand, QString("stop"), argument);
    else
        return cmdCall(true, systemctlCommand, QString("start"), argument);
}
