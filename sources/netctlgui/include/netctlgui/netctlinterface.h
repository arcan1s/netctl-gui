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
 * @file netctlinterface.h
 * Header of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#ifndef NETCTLINTERFACE_H
#define NETCTLINTERFACE_H

#include <QDir>
#include <QMap>
#include <QObject>


class Netctl;
class NetctlProfile;
class WpaSup;

/**
 * @enum InterfaceAnswer
 * @brief standard interface answer enumeration
 * @var InterfaceAnswer::False
 * false
 * @var InterfaceAnswer::True
 * true
 * @var InterfaceAnswer::Error
 * an error occurs
 */
enum InterfaceAnswer {
    False = 0,
    True,
    Error
};

/**
 * @brief The NetctlInterface class provides complex methods to get access to library
 */
class NetctlInterface : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief NetctlInterface class constructor
     * @param debugCmd        show debug messages
     * @param settings        default settings. See required keys in other modules
     */
    explicit NetctlInterface(const bool debugCmd = false,
                             const QMap<QString, QString> settings = QMap<QString, QString>());
    /**
     * @brief NetctlInterface class destructor
     */
    ~NetctlInterface();
    /**
     * @brief method which enables or disables selected profile and returns its status
     * @remark netctl-auto only
     * @param profile         profile name
     * @return InterfaceAnswer::False if profile is disabled
     * @return InterfaceAnswer::True if profile is enabled
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer autoEnableProfile(const QString profile);
    /**
     * @brief method which creates and copies profile
     * @remark netctl independ
     * @param profile         profile name
     * @param settings        profile settings
     * @return InterfaceAnswer::False if profile cannot be created
     * @return InterfaceAnswer::True if profile is created
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer createProfile(const QString profile, const QMap<QString, QString> settings);
    /**
     * @brief method which connects to ESSID
     * @remark netctl independ
     * @param essid           point ESSID
     * @param settings        profile settings (Security, ESSID, Key and Hidden are required)
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer connectToEssid(const QString essid, QMap<QString, QString> settings);
    /**
     * @brief method which connects to existent profile by ESSID
     * @remark netctl independ
     * @param essid           point ESSID
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer connectToKnownEssid(const QString essid);
    /**
     * @brief method which creates wireless profile and connects to it
     * @remark netctl independ
     * @param essid           point ESSID
     * @param settings        profile settings (Security, ESSID, Key and Hidden are required)
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer connectToUnknownEssid(const QString essid, QMap<QString, QString> settings);
    /**
     * @brief method which enables or disables selected profile and returns its status
     * @remark netctl only
     * @param profile         profile name
     * @return InterfaceAnswer::False if profile is disabled
     * @return InterfaceAnswer::True if profile is enabled
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer enableProfile(const QString profile);
    /**
     * @brief method which restarts selected profile and returns its status
     * @remark netctl only
     * @param profile         profile name
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer restartProfile(const QString profile);
    /**
     * @brief method which starts/stops or switchs to selected profile and returns its status
     * @remark netctl only
     * @param profile         profile name
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer startProfile(const QString profile);
    /**
     * @brief method which switchs to selected profile and returns its status
     * @remark both netctl and netctl-auto
     * @param profile         profile name
     * @return InterfaceAnswer::False if profile is inactive
     * @return InterfaceAnswer::True if profile is active
     * @return InterfaceAnswer::Error if an error occurs
     */
    InterfaceAnswer switchToProfile(const QString profile);

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
     * @brief WpaSup class
     */
    WpaSup *wpaCommand = nullptr;
    /**
     * @brief show debug messages
     */
    bool debug = false;
};


#endif /* NETCTLINTERFACE_H */
