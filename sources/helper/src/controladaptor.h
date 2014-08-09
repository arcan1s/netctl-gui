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

#ifndef CONTROLADAPTOR_H
#define CONTROLADAPTOR_H

#include <QDBusAbstractAdaptor>

#include <netctlgui/netctlgui.h>


class NetctlHelper;

class ControlAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.netctlgui.helper")

public:
    explicit ControlAdaptor(NetctlHelper *parent = 0,
                            const QMap<QString, QString> configuration = QMap<QString, QString>());
    ~ControlAdaptor();

public slots:
    // helper
    bool Active();
    bool Close();
    // netctlCommand
    bool autoDisableAll();
    bool autoEnable(const QString profile);
    bool autoEnableAll();
    bool autoStart(const QString profile);
    bool autoServiceEnable();
    bool autoServiceRestart();
    bool autoServiceStart();
    bool Enable(const QString profile);
    bool Restart(const QString profile);
    bool Start(const QString profile);
    bool SwitchTo(const QString profile);
    // netctlProfile
    bool Create(const QString profile, const QStringList settingsList);
    bool Remove(const QString profile);
    // wpaCommand
    QStringList WiFi();

private:
    NetctlHelper *helper;
    Netctl *netctlCommand;
    NetctlProfile *netctlProfile;
    WpaSup *wpaCommand;
};


#endif /* CONTROLADAPTOR_H */
