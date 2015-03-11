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

#ifndef NETCTLADAPTOR_H
#define NETCTLADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QStringList>

#include <netctlgui/netctlgui.h>


class NetctlAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.netctlgui.helper")

public:
    explicit NetctlAdaptor(QObject *parent = 0,
                           const bool debugCmd = false,
                           const QMap<QString, QString> configuration = QMap<QString, QString>());
    ~NetctlAdaptor();

public slots:
    // netctlCommand
    QString ActiveProfile();
    QString ActiveProfileStatus();
    bool autoIsProfileActive(const QString profile);
    bool autoIsProfileEnabled(const QString profile);
    QStringList Information();
    bool isNetctlAutoActive();
    bool isNetctlAutoEnabled();
    bool isProfileActive(const QString profile);
    bool isProfileEnabled(const QString profile);
    QString netctlActiveProfile();
    QStringList netctlProfileList();
    QStringList netctlVerboseProfileList();
    Q_DECL_DEPRECATED QStringList ProfileList();
    QStringList VerboseProfileList();
    // netctlProfile
    QStringList Profile(const QString profile);
    QString ProfileValue(const QString profile, const QString key);
    QStringList ProfileValues(const QString profile, const QStringList keys);
    // wpaCommand
    QString ProfileByEssid(const QString essid);
    QStringList WirelessInterfaces();

private:
    bool debug;
    Netctl *netctlCommand = nullptr;
    NetctlProfile *netctlProfile = nullptr;
    WpaSup *wpaCommand = nullptr;
};


#endif /* NETCTLADAPTOR_H */
