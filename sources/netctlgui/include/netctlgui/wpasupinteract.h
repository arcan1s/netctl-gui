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
 * @file wpasupinteract.h
 * Header of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#ifndef WPASUPINTERACT_H
#define WPASUPINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


class Netctl;
class NetctlProfile;

/**
 * @struct netctlWifiInfo
 * @brief WiFi information structure
 * @var netctlWifiInfo::name
 * ESSID
 * @var netctlWifiInfo::security
 * may be "WPA2", "WEP", "WEP", "none"
 * @var netctlWifiInfo::signal
 * Wifi point signal
 * @var netctlWifiInfo::active
 * whether associated profile is active
 * @var netctlWifiInfo::exists
 * whether associated profile exists
 */
typedef struct
{
    QString name;
    QString security;
    QString signal;
    bool active;
    bool exists;
} netctlWifiInfo;

/**
 * @brief The WpaSup class interacts with wpa_supplicant
 */
class WpaSup : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief WpaSup class constructor
     * @param debugCmd       show debug messages
     * @param settings       default settings. Needed keys are
     *                       CTRL_DIR (path to ctrl_directory),
     *                       CTRL_GROUP (group which is owner of CTRL_DIR),
     *                       FORCE_SUDO (force to use sudo),
     *                       PID_FILE (wpa_supplicant PID file),
     *                       SUDO_PATH (path to sudo command),
     *                       WPACLI_PATH (path to wpa_cli command),
     *                       WPA_DRIVERS (wpa_supplicant drivers comma separated),
     *                       WPASUP_PATH (path to wpa_supplicant command)
     */
    explicit WpaSup(const bool debugCmd = false,
                    const QMap<QString, QString> settings = QMap<QString, QString>());
    /**
     * @brief WpaSup class destructor
     */
    ~WpaSup();
    // general information
    /**
     * @brief method which gets profile name by ESSID
     * @param essid          ESSID name
     * @return profile name
     */
    QString existentProfile(const QString essid);
    /**
     * @brief method which check system configuration and return recommended values to keys
     * @return recommended parametrs
     */
    static QMap<QString, QString> getRecommendedConfiguration();
    /**
     * @brief method which checks profile status by ESSID
     * @param essid          ESSID name
     * @return false if profile is inactive
     * @return true if profile is active
     */
    bool isProfileActive(const QString essid);
    /**
     * @brief method which checks profile existence by ESSID
     * @param essid          ESSID name
     * @return false if profile does not exist
     * @return true if profile exists
     */
    bool isProfileExists(const QString essid);

public slots:
    // functions
    /**
     * @brief method which scans WiFi networks
     * @return list of essids
     */
    QList<netctlWifiInfo> scanWifi();
    /**
     * @brief method which calls wpa_supplicant
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool startWpaSupplicant();
    /**
     * @brief method which send TERMINATE signal to wpa_supplicant
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool stopWpaSupplicant();

private:
    /**
     * @brief Netctl class
     */
    Netctl *netctlCommand = nullptr;
    /**
     * @brief NetctlProfile class
     */
    NetctlProfile *netctlProfile = nullptr;
    /**
     * @brief show debug messages
     */
    bool debug = false;
    /**
     * @brief use RootProcess instead of QProcess. Default is true
     */
    bool useSuid = true;
    /**
     * @brief path to ctrl_directory. Defaults is "/run/wpa_supplicant_netctl-gui"
     */
    QString ctrlDir = QString("/run/wpa_supplicant_netctl-gui");
    /**
     * @brief group which is owner of CTRL_DIR. Default is "users"
     */
    QString ctrlGroup = QString("users");
    /**
     * @brief wpa_supplicant PID file. Default is "/run/wpa_supplicant_netctl-gui.pid"
     */
    QString pidFile = QString("/run/wpa_supplicant_netctl-gui.pid");
    /**
     * @brief path to sudo command. Default is "/usr/bin/kdesu"
     */
    QString sudoCommand = QString("/usr/bin/kdesu");
    /**
     * @brief path to wpa_cli command. Default is "/usr/bin/wpa_cli"
     */
    QString wpaCliPath = QString("/usr/bin/wpa_cli");
    /**
     * @brief wpa_supplicant drivers comma separated. Default is "nl80211,wext"
     */
    QString wpaDrivers = QString("nl80211,wext");
    /**
     * @brief path to wpa_supplicant command. Default is "/usr/bin/wpa_supplicant"
     */
    QString wpaSupPath = QString("/usr/bin/wpa_supplicant");
    // functions
    /**
     * @brief method which calls wpa_cli and returns its output
     * @param commandLine    command which will be passed to wpa_cli
     * @return wpa_cli output
     */
    QString getWpaCliOutput(const QString commandLine);
    /**
     * @brief method which will be called to sleep thread
     * @param sec            time interval, seconds
     */
    bool waitForProcess(const int sec);
    /**
     * @brief method which calls wpa_cli
     * @param commandLine    command which will be passed to wpa_cli
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool wpaCliCall(const QString commandLine);
};


#endif /* WPASUPINTERACT_H */
