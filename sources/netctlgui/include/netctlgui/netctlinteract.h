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
 * @file netctlinteract.h
 * Header of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#ifndef NETCTLINTERACT_H
#define NETCTLINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


/**
 * @brief The Netctl class interacts with netctl
 */
class Netctl : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Netctl class constructor
     * @param debugCmd       show debug messages
     * @param settings       default settings. Needed keys are
     *                       IFACE_DIR (path to directory with interfaces),
     *                       PREFERED_IFACE (prefered interface for WiFi),
     *                       NETCTL_PATH (path to netctl command),
     *                       NETCTLAUTO_PATH (path to netctl-auto command),
     *                       NETCTLAUTO_SERVICE (netctl-auto service name),
     *                       PROFILE_DIR (path to directory which contains profiles),
     *                       SUDO_PATH (path to sudo command),
     *                       SYSTEMCTL_PATH (path to systemctl command)
     */
    explicit Netctl(const bool debugCmd = false,
                    const QMap<QString, QString> settings = QMap<QString, QString>());
    /**
     * @brief Netctl class destructor
     */
    ~Netctl();
    // general information
    /**
     * @brief method which returns profile informations from netctl
     * @return list of profiles. Available information is [NAME, DESCRIPTION, STATUS]:
     *         NAME is a profile name,
     *         DESCRIPTION is a profile description (see more details below),
     *         STATUS is a profile status (see more details below)
     */
    QList<QStringList> getProfileList();
    /**
     * @brief method which returns profile informations from netctl-auto
     * @return list of profiles. Available information is [NAME, DESCRIPTION, STATUS]:
     *         NAME is a profile name,
     *         DESCRIPTION is a profile description (see more details below),
     *         STATUS is a profile status (see more details below)
     */
    QList<QStringList> getProfileListFromNetctlAuto();
    /**
     * @brief method which gets description from profile
     * @param profile        profile name
     * @return profile description or "<unknown>"
     */
    QString getProfileDescription(const QString profile);
    /**
     * @brief method which gets descriptions from profile list
     * @param profileList    profile names
     * @return list of profile descriptions (if description is not available return "<unknown>")
     */
    QStringList getProfileDescriptions(const QStringList profileList);
    /**
     * @brief method which gets profile status
     * @param profile        profile name
     * @return profile status. It may be "active (enabled)", "active (static)",
     *         "inactive (enabled)", "inactive (static)"
     */
    QString getProfileStatus(const QString profile);
    /**
     * @brief method which gets statuses of profile list
     * @param profileList    profile names
     * @return list of profile statuses. It may be "active (enabled)", "active (static)",
     *         "inactive (enabled)", "inactive (static)"
     */
    QStringList getProfileStatuses(const QStringList profileList);
    /**
     * @brief method which gets ESSID from profile
     * @param profile        profile name
     * @return ESSID name or null string
     */
    QString getSsidFromProfile(const QString profile);
    /**
     * @brief method which checks if profile is active
     * @param profile        profile name
     * @return false if profile is inactive
     * @return true if profile is active
     */
    bool isProfileActive(const QString profile);
    /**
     * @brief method which checks if profile is enabled
     * @param profile        profile name
     * @return false if profile is disabled
     * @return true if profile is enabled
     */
    bool isProfileEnabled(const QString profile);
    /**
     * @brief method which checks if profile is active (netctl-auto)
     * @param profile        profile name
     * @return false if profile is inactive
     * @return true if profile is active
     */
    bool autoIsProfileActive(const QString profile);
    /**
     * @brief method which checks if profile is enabled (netctl-auto)
     * @param profile        profile name
     * @return false if profile is disabled
     * @return true if profile is enabled
     */
    bool autoIsProfileEnabled(const QString profile);
    /**
     * @brief method which checks netctl-auto autoload status
     * @return false if netctl-auto is disabled
     * @return true if netctl-auto is enabled
     */
    bool isNetctlAutoEnabled();
    /**
     * @brief method which checks netctl-auto status
     * @return false if netctl-auto is inactive
     * @return true if netctl-auto is active
     */
    bool isNetctlAutoRunning();

public slots:
    // functions
    // netctl
    /**
     * @brief method which sets profile disabled or enabled
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool enableProfile(const QString profile);
    /**
     * @brief method which restarts profile
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool restartProfile(const QString profile);
    /**
     * @brief method which starts or stops profile
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool startProfile(const QString profile);
    // netctl-auto
    /**
     * @brief method which sets all profiles disabled (netctl-auto)
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoDisableAllProfiles();
    /**
     * @brief method which sets profile disabled or enabled (netctl-auto)
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoEnableProfile(const QString profile);
    /**
     * @brief method which sets all profiles enabled (netctl-auto)
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoEnableAllProfiles();
    /**
     * @brief method which switchs to profile (netctl-auto)
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoStartProfile(const QString profile);
    // netctl-auto service
    /**
     * @brief method which sets netctl-auto service enabled or disabled
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoEnableService();
    /**
     * @brief method which restarted netctl-auto service
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoRestartService();
    /**
     * @brief method which starts or stops netctl-auto service
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool autoStartService();

private:
    /**
     * @brief show debug messages
     */
    bool debug;
    /**
     * @brief directory with interfaces. Default is "/sys/class/net/"
     */
    QDir *ifaceDirectory;
    /**
     * @brief prefered interface for WiFi. Default is ""
     */
    QString mainInterface;
    /**
     * @brief path to netctl command. Default is "/usr/bin/netctl"
     */
    QString netctlCommand;
    /**
     * @brief path to netctl-auto command. Default is "/usr/bin/netctl-auto"
     */
    QString netctlAutoCommand;
    /**
     * @brief netctl-auto service name. Default is "netctl-auto"
     */
    QString netctlAutoService;
    /**
     * @brief directory which contains profiles. Default is "/etc/netctl"
     */
    QDir *profileDirectory;
    /**
     * @brief path to sudo command. Default is "/usr/bin/kdesu"
     */
    QString sudoCommand;
    /**
     * @brief path to systemctl command. Default is "/usr/bin/systemctl"
     */
    QString systemctlCommand;
    // functions
    /**
     * @brief method which calls netctl and returns its output
     * @param sudo           set true if sudo is needed
     * @param commandLine    command which will be passed to netctl
     * @param profile        profile name
     * @return netctl output
     */
    QString getNetctlOutput(const bool sudo, const QString commandLine, const QString profile = 0);
    /**
     * @brief method which gets interface list from PREFERED_IFACE and IFACE_DIR
     * @return first element from interface list. If PREFERED_IFACE is not empty it will be first element
     */
    QString getWifiInterface();
    /**
     * @brief method which calls netctl
     * @param sudo           set true if sudo is needed
     * @param commandLine    command which will be passed to netctl
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool netctlCall(const bool sudo, const QString commandLine, const QString profile = 0);
    /**
     * @brief method which calls netctl-auto
     * @param sudo           set true if sudo is needed
     * @param commandLine    command which will be passed to netctl-auto
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool netctlAutoCall(const bool sudo, const QString commandLine, const QString profile = 0);
    /**
     * @brief method which calls systemctl associated with netctl-auto
     * @param sudo           set true if sudo is needed
     * @param commandLine    command which will be passed to systemctl
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool systemctlCall(const bool sudo, const QString commandLine);
};


#endif /* NETCTLINTERACT_H */
