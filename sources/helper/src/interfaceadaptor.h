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

#ifndef INTERFACEADAPTOR_H
#define INTERFACEADAPTOR_H

#include <QDBusAbstractAdaptor>

#include <netctlgui/netctlgui.h>


class InterfaceAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.netctlgui.helper")

public:
    explicit InterfaceAdaptor(QObject *parent = 0,
                              const bool debugCmd = false,
                              const QMap<QString, QString> configuration = QMap<QString, QString>());
    ~InterfaceAdaptor();

public slots:
    // control slots
    int autoEnable(const QString profile);
    int Create(const QString profile, const QStringList settingsList);
    int Enable(const QString profile);
    int Essid(const QString essid, QStringList settingsList);
    int KnownEssid(const QString essid);
    int Remove(const QString profile);
    int Restart(const QString profile);
    int Start(const QString profile);
    int StopAll();
    int SwitchTo(const QString profile);
    int UnknownEssid(const QString essid, QStringList settingsList);
    // information
    QStringList Information();
    QStringList Profile(const QString profile);
    QStringList Status();

private:
    bool debug;
    NetctlInterface *netctlInterface = nullptr;
};


#endif /* INTERFACEADAPTOR_H */
