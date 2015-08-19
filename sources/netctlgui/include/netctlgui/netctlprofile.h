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
 * @file netctlprofile.h
 * Header of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#ifndef NETCTLPROFILE_H
#define NETCTLPROFILE_H

#include <QDir>
#include <QMap>
#include <QObject>


/**
 * @brief The NetctlProfile class interacts with netctl profiles
 */
class NetctlProfile : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief NetctlProfile class constructor
     * @param debugCmd       show debug messages
     * @param settings       default settings. Needed keys are
     *                       FORCE_SUDO (force to use sudo),
     *                       PROFILE_DIR (path to directory which contains profiles),
     *                       SUDO_PATH (path to sudo command)
     */
    explicit NetctlProfile(const bool debugCmd = false,
                           const QMap<QString, QString> settings = QMap<QString, QString>());
    /**
      * @brief Netctl class destructor
      */
    virtual ~NetctlProfile();
    /**
     * @brief method which copies temporary profile to PROFILE_DIR
     * @param oldPath        path to temprorary profile
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool copyProfile(const QString oldPath) const;
    /**
     * @brief method which creates temporary profile
     * @param profile        profile name
     * @param settings       profile configuration. All available keys will be printed to the profile
     * @return temporary profile name
     */
    QString createProfile(const QString profile, const QMap<QString, QString> settings) const;
    /**
     * @brief method which check system configuration and return recommended values to keys
     * @return recommended parametrs
     */
    static QMap<QString, QString> getRecommendedConfiguration();
    /**
     * @brief method which reads settings from profile
     * @param profile        profile name
     * @return settings from profile
     */
    QMap<QString, QString> getSettingsFromProfile(const QString profile) const;
    /**
     * @brief method which return value from profile by key
     * @param profile        profile name
     * @param key            required key
     * @return value by key
     */
    QString getValueFromProfile(const QString profile, const QString key) const;
    /**
     * @brief method which return values from profile by keys
     * @param profile        profile name
     * @param keys           required keys
     * @return values by keys
     */
    QStringList getValuesFromProfile(const QString profile, const QStringList keys) const;
    /**
     * @brief method which removes profile
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the method was completed without errors
     */
    bool removeProfile(const QString profile) const;

private:
    /**
     * @brief show debug messages. Default is false
     */
    bool debug = false;
    /**
     * @brief use RootProcess instead of QProcess. Default is true
     */
    bool useSuid;
    /**
     * @brief directory which contains profiles. Default is "/etc/netctl"
     */
    QDir *profileDirectory = nullptr;
    /**
     * @brief path to sudo command. Default is "kdesu"
     */
    QString sudoCommand;
};


#endif /* NETCTLPROFILE_H */
