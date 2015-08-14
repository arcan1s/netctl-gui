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

#ifndef NETCTL_DE_H
#define NETCTL_DE_H

#include <Plasma/DataEngine>
#include <QAbstractSocket>


class Netctl : public Plasma::DataEngine
{
    Q_OBJECT

public:
    explicit Netctl(QObject *parent, const QVariantList &args);
    virtual ~Netctl();
    QString getExtIp(const QString cmd) const;
    QString getInfo(const QStringList profiles, const QStringList statuses) const;
    QStringList getInterfaceList() const;
    QString getIntIp(const QAbstractSocket::NetworkLayerProtocol protocol) const;
    QStringList getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto);
    QStringList getProfileStringStatus(const QString cmdNetctl) const;

public slots:
    void initSources();

protected:
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private:
    bool isNetctlCheckRunning = false;
    bool netctlAutoStatus = false;
    bool status = false;
    QStringList currentProfile;
    QStringList currentStatus;
    // configuration
    bool debug;
    QVariantMap configuration;
    QString getCmdOutput(const QString cmd) const;
    void readConfiguration();
};


#endif /* NETCTL_DE_H */
