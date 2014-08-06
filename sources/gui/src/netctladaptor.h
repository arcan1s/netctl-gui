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


class Netctl;
class NetctlProfile;
class WpaSup;

class NetctlAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.netctlgui.netctlgui")

public:
    explicit NetctlAdaptor(QObject *parent = 0,
                           const bool debugCmd = false,
                           const QMap<QString, QString> configuration = QMap<QString, QString>());
    ~NetctlAdaptor();

public slots:
    QString Information();

private:
    bool debug;
    Netctl *netctlCommand;
    NetctlProfile *netctlProfile;
    WpaSup *wpaCommand;
};


#endif /* NETCTLADAPTOR_H */
