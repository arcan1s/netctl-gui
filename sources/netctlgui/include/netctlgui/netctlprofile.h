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
     *                       PROFILE_DIR (path to directory which contains profiles),
     *                       SUDO_PATH (path to sudo command)
     */
    explicit NetctlProfile(const bool debugCmd = false,
                           const QMap<QString, QString> settings = QMap<QString, QString>());
    /**
      * @brief Netctl class destructor
      */
    ~NetctlProfile();
    /**
     * @brief function which copies temporary profile to PROFILE_DIR
     * @param oldPath        path to temprorary profile
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the function was completed without errors
     */
    bool copyProfile(const QString oldPath);
    /**
     * @brief function which creates temporary profile
     * @param profile        profile name
     * @param settings       profile configuration. All available keys will be printed to the profile
     * @return temporary profile name
     */
    QString createProfile(const QString profile, const QMap<QString, QString> settings);
    /**
     * @brief function which reads settings from profile
     * @param profile        profile name
     * @return settings from profile
     */
    QMap<QString, QString> getSettingsFromProfile(const QString profile);
    /**
     * @brief function which removes profile
     * @param profile        profile name
     * @return false if components are not found or command exit code is not equal to 0
     * @return true if the function was completed without errors
     */
    bool removeProfile(const QString profile);

private:
    /**
     * @brief show debug messages
     */
    bool debug;
    /**
     * @brief path to directory which contains profiles. Defaults is /etc/netctl
     */
    QDir *profileDirectory;
    /**
     * @brief path to sudo command. Defaults is /usr/bin/kdesu
     */
    QString sudoCommand;
};


#endif /* NETCTLPROFILE_H */
