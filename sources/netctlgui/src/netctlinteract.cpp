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
#include <QFile>
#include <QProcess>

#include <netctlgui/netctlinteract.h>


/**
 * @class Netctl
 */
/**
 * @fn Netctl
 */
Netctl::Netctl(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
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
    if (settings.contains(QString("PROFILE_DIR")))
        profileDirectory = new QDir(settings[QString("PROFILE_DIR")]);
    else
        profileDirectory = new QDir(QString("/etc/netctl/"));
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

    if (ifaceDirectory != 0)
        delete ifaceDirectory;
    if (profileDirectory != 0)
        delete profileDirectory;
}


// functions
/**
 * @fn getNetctlOutput
 */
QString Netctl::getNetctlOutput(const bool sudo, const QString commandLine, const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]";
    if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]" << ":" << "Command" << commandLine;
    if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]" << ":" << "Profile" << profile;
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]" << ":" << "Could not find netctl";
        return QString();
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]" << ":" << "Could not find sudo";
        return QString();
    }

    QProcess command;
    QString commandText;
    if (sudo)
        commandText = sudoCommand + QString(" ") + netctlCommand + QString(" ") + commandLine +
                QString(" ") + profile;
    else
        commandText = netctlCommand + QString(" ") + commandLine + QString(" ") + profile;
    if (debug) qDebug() << "[Netctl]" << "[getNetctlOutput]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);

    return command.readAllStandardOutput();
}


/**
 * @fn getWifiInterface
 */
QString Netctl::getWifiInterface()
{
    if (debug) qDebug() << "[Netctl]" << "[getInterfaceList]";
    if (ifaceDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getInterfaceList]" << ":" << "Could not find directory";
        return QString();
    }

    QStringList interfaces;
    if (!mainInterface.isEmpty())
        interfaces.append(mainInterface);
    QStringList allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i=0; i<allInterfaces.count(); i++) {
        if (debug) qDebug() << "[Netctl]" << "[getInterfaceList]" << ":" << "Check directory"
                 << ifaceDirectory->path() + QDir::separator() + allInterfaces[i] + QDir::separator() + QString("wireless");
        if (QDir(ifaceDirectory->path() + QDir::separator() + allInterfaces[i] +
                 QDir::separator() + QString("wireless")).exists())
            interfaces.append(allInterfaces[i]);
    }

    return interfaces[0];
}


/**
 * @fn netctlCall
 */
bool Netctl::netctlCall(const bool sudo, const QString commandLine, const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[netctlCall]";
    if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Command" << commandLine;
    if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Profile" << profile;
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Could not find netctl";
        return false;
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Could not find sudo";
        return false;
    }

    QProcess command;
    QString commandText;
    if (sudo)
        commandText = sudoCommand + QString(" ") + netctlCommand + QString(" ") + commandLine +
                QString(" ") + profile;
    else
        commandText = netctlCommand + QString(" ") + commandLine + QString(" ") + profile;
    if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[Netctl]" << "[netctlCall]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


/**
 * @fn netctlAutoCall
 */
bool Netctl::netctlAutoCall(const bool sudo, const QString commandLine, const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]";
    if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Command" << commandLine;
    if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Profile" << profile;
    if (netctlAutoCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Could not find netctl-auto";
        return false;
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Could not find sudo";
        return false;
    }

    QProcess command;
    QString commandText;
    if (sudo)
        commandText = sudoCommand + QString(" ") + netctlAutoCommand + QString(" ") + commandLine;
    else
        commandText = netctlAutoCommand + QString(" ") + commandLine;
    if (profile != 0)
        commandText = commandText + QString(" ") + profile;
    if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[Netctl]" << "[netctlAutoCall]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


/**
 * @fn systemctlCall
 */
bool Netctl::systemctlCall(const bool sudo, const QString commandLine)
{
    if (debug) qDebug() << "[Netctl]" << "[systemctlCall]";
    if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Command" << commandLine;
    if (netctlAutoService == 0) {
        if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Could not find service";
        return false;
    }
    if ((sudo) && (sudoCommand == 0)) {
        if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Could not find sudo";
        return false;
    }
    if (systemctlCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Could not find systemctl";
        return false;
    }

    QProcess command;
    QString commandText;
    QString interface = getWifiInterface();
    if (sudo)
        commandText = sudoCommand + QString(" ") + systemctlCommand + QString(" ") + commandLine +
                QString(" ") + netctlAutoService + QString("@") + interface + QString(".service");
    else
        commandText = systemctlCommand + QString(" ") + commandLine + QString(" ") + netctlAutoService +
                QString("@") + interface + QString(".service");
    if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[Netctl]" << "[systemctlCall]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


// general information
/**
 * @fn getProfileList
 */
QList<QStringList> Netctl::getProfileList()
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileList]";
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getProfileList]" << ":" << "Could not find directory";
        return QList<QStringList>();
    }

    QList<QStringList> fullProfilesInfo;
    QStringList profiles = profileDirectory->entryList(QDir::Files);
    QStringList descriptions = getProfileDescriptions(profiles);
    QStringList statuses = getProfileStatuses(profiles);
    for (int i=0; i<profiles.count(); i++) {
        QStringList profileInfo;
        profileInfo.append(profiles[i]);
        profileInfo.append(descriptions[i]);
        profileInfo.append(statuses[i]);
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileListFromNetctlAuto
 */
QList<QStringList> Netctl::getProfileListFromNetctlAuto()
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileListFromNetctlAuto]";
    if (netctlAutoCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getProfileListFromNetctlAuto]" << ":" << "Could not find netctl-auto";
        return QList<QStringList>();
    }

    QProcess command;
    QString commandText = netctlAutoCommand + QString(" list");
    if (debug) qDebug() << "[Netctl]" << "[getProfileListFromNetctlAuto]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    QStringList output = QString(command.readAllStandardOutput()).split(QChar('\n'), QString::SkipEmptyParts);
    QList<QStringList> fullProfilesInfo;
    for (int i=0; i<output.count(); i++) {
        QStringList profileInfo;
        profileInfo.append(output[i].mid(2, -1));
        profileInfo.append(getProfileDescription(profileInfo[0]));
        profileInfo.append(output[i].left(1));
        fullProfilesInfo.append(profileInfo);
    }

    return fullProfilesInfo;
}


/**
 * @fn getProfileDescription
 */
QString Netctl::getProfileDescription(const QString profileName)
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]";
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]" << ":" << "Profile" << profileName;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]" << ":" << "Could not find directory";
        return QString();
    }

    QString description = QString("<unknown>");
    QString profileUrl = profileDirectory->absolutePath() + QDir::separator() + profileName;
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescription]" << ":" << "Check" << profileUrl;
    QFile profile(profileUrl);
    QString fileStr;
    if (!profile.open(QIODevice::ReadOnly))
        return description;
    while (true) {
        fileStr = QString(profile.readLine());
        if (fileStr[0] == QChar('#')) continue;
        if (fileStr.split(QChar('='), QString::SkipEmptyParts).count() == 2)
            if (fileStr.split(QChar('='), QString::SkipEmptyParts)[0] == QString("Description"))
                description = fileStr.split(QChar('='), QString::SkipEmptyParts)[1].trimmed();
        if (profile.atEnd())
            break;
    }
    description.remove(QChar('\'')).remove(QChar('"'));

    return description;
}


/**
 * @fn getProfileDescriptions
 */
QStringList Netctl::getProfileDescriptions(const QStringList profileList)
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescriptions]";
    if (debug) qDebug() << "[Netctl]" << "[getProfileDescriptions]" << ":" << "Profile list" << profileList;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getProfileDescriptions]" << ":" << "Could not find directory";
        return QStringList();
    }

    QStringList descriptions;
    for (int i=0; i<profileList.count(); i++) {
        QString description = QString("<unknown>");
        QString profileUrl = profileDirectory->absolutePath() + QDir::separator() + profileList[i];
        if (debug) qDebug() << "[Netctl]" << "[getProfileDescriptions]" << ":" << "Check" << profileUrl;
        QFile profile(profileUrl);
        QString fileStr;
        if (!profile.open(QIODevice::ReadOnly)) {
            descriptions.append(description);
            continue;
        }
        while (true) {
            fileStr = QString(profile.readLine());
            if (fileStr[0] == QChar('#')) continue;
            if (fileStr.split(QChar('='), QString::SkipEmptyParts).count() == 2)
                if (fileStr.split(QChar('='), QString::SkipEmptyParts)[0] == QString("Description"))
                    description = fileStr.split(QChar('='), QString::SkipEmptyParts)[1].trimmed();
            if (profile.atEnd())
                break;
        }
        description.remove(QChar('\'')).remove(QChar('"'));
        descriptions.append(description);
    }

    return descriptions;
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
 * @fn getProfileStatuses
 */
QStringList Netctl::getProfileStatuses(const QStringList profileList)
{
    if (debug) qDebug() << "[Netctl]" << "[getProfileStatuses]";
    if (debug) qDebug() << "[Netctl]" << "[getProfileStatuses]" << ":" << "Profile list" << profileList;

    QStringList statuses;
    for (int i=0; i<profileList.count(); i++) {
        QString status;
        if (isProfileActive(profileList[i]))
            status = QString("active");
        else
            status = QString("inactive");
        if (isProfileEnabled(profileList[i]))
            status = status + QString(" (enabled)");
        else
            status = status + QString(" (static)");
        statuses.append(status);
    }

    return statuses;
}


/**
 * @fn getSsidFromProfile
 */
QString Netctl::getSsidFromProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[getSsidFromProfile]";
    if (debug) qDebug() << "[Netctl]" << "[getSsidFromProfile]" << ":" << "Profile" << profile;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[Netctl]" << "[getSsidFromProfile]" << ":" << "Could not find directory";
        return QString();
    }

    QString ssidName;
    QString profileUrl = profileDirectory->absolutePath() + QDir::separator() + profile;
    if (debug) qDebug() << "[Netctl]" << "[getSsidFromProfile]" << ":" << "Check" << profileUrl;
    QFile profileFile(profileUrl);
    QString fileStr;
    if (!profileFile.open(QIODevice::ReadOnly))
        return ssidName;
    while (true) {
        fileStr = QString(profileFile.readLine());
        if (fileStr[0] == QChar('#')) continue;
        if (fileStr.split(QChar('='), QString::SkipEmptyParts).count() == 2)
            if (fileStr.split(QChar('='), QString::SkipEmptyParts)[0] == QString("ESSID"))
                ssidName = fileStr.split(QChar('='), QString::SkipEmptyParts)[1].trimmed();
        if (profileFile.atEnd())
            break;
    }
    profileFile.close();
    ssidName.remove(QChar('\'')).remove(QChar('"'));

    return ssidName;
}


/**
 * @fn isProfileActive
 */
bool Netctl::isProfileActive(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[isProfileActive]";
    if (debug) qDebug() << "[Netctl]" << "[isProfileActive]" << ":" << "Profile" << profile;

    bool status = false;
    QString cmdOutput = getNetctlOutput(false, QString("status"), profile);
    if (cmdOutput.contains(QString("Active: active")))
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

    return netctlCall(false, QString("is-enabled"), profile);
}


/**
 * @fn autoIsProfileActive
 */
bool Netctl::autoIsProfileActive(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileActive]";
    if (debug) qDebug() << "[Netctl]" << "[autoIsProfileActive]" << ":" << "Profile" << profile;

    bool status = false;
    QList<QStringList> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i][0] == profile) && (profiles[i][2] == QString("*"))) {
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
    QList<QStringList> profiles = getProfileListFromNetctlAuto();
    for (int i=0; i<profiles.count(); i++)
        if ((profiles[i][0] == profile) && (profiles[i][2] == QString("!"))) {
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
    if (systemctlCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]" << ":" << "Could not find systemctl";
        return false;
    }

    QProcess command;
    QString interface = getWifiInterface();
    QString commandText = systemctlCommand + QString(" is-enabled ") + netctlAutoService + QString("@") +
            interface + QString(".service");
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoEnabled]" << ":" << "Cmd returns" << command.exitCode();
    QString output = command.readAllStandardOutput().trimmed();

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
    if (systemctlCommand == 0) {
        if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]" << ":" << "Could not find systemctl";
        return false;
    }

    QString interface = getWifiInterface();
    QProcess command;
    QString commandText = systemctlCommand + QString(" is-active ") + netctlAutoService + QString("@") +
            interface + QString(".service");
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    if (debug) qDebug() << "[Netctl]" << "[isNetctlAutoRunning]" << ":" << "Cmd returns" << command.exitCode();
    QString output = command.readAllStandardOutput().trimmed();

    if (output == QString("active"))
        return true;
    else
        return false;
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
        return netctlCall(true, QString("disable"), profile);
    else
        return netctlCall(true, QString("enable"), profile);
}


/**
 * @fn restartProfile
 */
bool Netctl::restartProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[restartProfile]";
    if (debug) qDebug() << "[Netctl]" << "[restartProfile]" << ":" << "Profile" << profile;

    return netctlCall(true, QString("restart"), profile);
}


/**
 * @fn startProfile
 */
bool Netctl::startProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[startProfile]";
    if (debug) qDebug() << "[Netctl]" << "[startProfile]" << ":" << "Profile" << profile;

    if (isProfileActive(profile))
        return netctlCall(true, QString("stop"), profile);
    else
        return netctlCall(true, QString("start"), profile);
}


/**
 * @fn autoDisableAllProfiles
 */
bool Netctl::autoDisableAllProfiles()
{
    if (debug) qDebug() << "[Netctl]" << "[autoDisableAllProfiles]";

    return netctlAutoCall(false, QString("disable-all"));
}


/**
 * @fn autoEnableProfile
 */
bool Netctl::autoEnableProfile(const QString profile)
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableProfile]";
    if (debug) qDebug() << "[Netctl]" << "[autoEnableProfile]" << ":" << "Profile" << profile;

    if (autoIsProfileEnabled(profile))
        return netctlAutoCall(false, QString("disable"), profile);
    else
        return netctlAutoCall(false, QString("enable"), profile);
}


/**
 * @fn autoEnableAllProfiles
 */
bool Netctl::autoEnableAllProfiles()
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableAllProfiles]";

    return netctlAutoCall(false, QString("enable-all"));
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
        return netctlAutoCall(false, QString("switch-to"), profile);
}


/**
 * @fn autoEnableService
 */
bool Netctl::autoEnableService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoEnableService]";

    if (isNetctlAutoEnabled())
        return systemctlCall(true, QString("disable"));
    else
        return systemctlCall(true, QString("enable"));
}


/**
 * @fn autoRestartService
 */
bool Netctl::autoRestartService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoRestartService]";

    if (isNetctlAutoRunning())
        return systemctlCall(true, QString("restart"));
    else
        return true;
}


/**
 * @fn autoStartService
 */
bool Netctl::autoStartService()
{
    if (debug) qDebug() << "[Netctl]" << "[autoStartService]";

    if (isNetctlAutoRunning())
        return systemctlCall(true, QString("stop"));
    else
        return systemctlCall(true, QString("start"));
}
