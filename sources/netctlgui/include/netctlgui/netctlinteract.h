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

#ifndef NETCTLINTERACT_H
#define NETCTLINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


class Netctl : public QObject
{
    Q_OBJECT

public:
    explicit Netctl(const bool debugCmd = false,
                    const QMap<QString, QString> settings = QMap<QString, QString>());
    ~Netctl();
    // general information
    QList<QStringList> getProfileList();
    QList<QStringList> getProfileListFromNetctlAuto();
    QString getProfileDescription(const QString profile);
    QStringList getProfileDescriptions(const QStringList profileList);
    QString getProfileStatus(const QString profile);
    QStringList getProfileStatuses(const QStringList profileList);
    QString getSsidFromProfile(const QString profile);
    bool isProfileActive(const QString profile);
    bool isProfileEnabled(const QString profile);
    bool autoIsProfileActive(const QString profile);
    bool autoIsProfileEnabled(const QString profile);
    bool isNetctlAutoEnabled();
    bool isNetctlAutoRunning();

public slots:
    // functions
    // netctl
    bool enableProfile(const QString profile);
    bool restartProfile(const QString profile);
    bool startProfile(const QString profile);
    // netctl-auto
    bool autoDisableAllProfiles();
    bool autoEnableProfile(const QString profile);
    bool autoEnableAllProfiles();
    bool autoStartProfile(const QString profile);
    // netctl-auto service
    bool autoEnableService();
    bool autoRestartService();
    bool autoStartService();

private:
    bool debug;
    QDir *ifaceDirectory;
    QString mainInterface;
    QString netctlCommand;
    QString netctlAutoCommand;
    QString netctlAutoService;
    QDir *profileDirectory;
    QString sudoCommand;
    QString systemctlCommand;
    // functions
    QString getNetctlOutput(const bool sudo, const QString commandLine, const QString profile);
    QString getWifiInterface();
    bool netctlCall(const bool sudo, const QString commandLine, const QString profile);
    bool netctlAutoCall(const bool sudo, const QString commandLine, const QString profile = 0);
    bool systemctlCall(const bool sudo, const QString commandLine);
};


#endif /* NETCTLINTERACT_H */
