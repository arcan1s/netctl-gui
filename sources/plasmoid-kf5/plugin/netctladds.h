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


#ifndef NETCTLADDS_H
#define NETCTLADDS_H

#include <QMap>
#include <QObject>
#include <QVariant>


class NetctlAdds : public QObject
{
    Q_OBJECT

public:
    NetctlAdds(QObject *parent = 0);
    ~NetctlAdds();

    Q_INVOKABLE bool checkHelperStatus();
    Q_INVOKABLE QString getAboutText(const QString type = "header");
    Q_INVOKABLE QString getInfo(const QString current, const QString status);
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QString parsePattern(const QString pattern, const QMap<QString, QVariant> dict);
    Q_INVOKABLE void runCmd(const QString cmd);
    Q_INVOKABLE void sendNotification(const QString eventId, const QString message);
    // context menu
    Q_INVOKABLE void enableProfileSlot(const QMap<QString, QVariant> dict,
                                       const bool useHelper = true,
                                       const QString cmd = QString("/usr/bin/netctl"),
                                       const QString sudoCmd = QString(""));
    Q_INVOKABLE void restartProfileSlot(const QMap<QString, QVariant> dict,
                                        const bool useHelper = true,
                                        const QString cmd = QString("/usr/bin/netctl"),
                                        const QString sudoCmd = QString(""));
    Q_INVOKABLE void startProfileSlot(const QStringList profiles, const bool status,
                                      const bool useHelper = true,
                                      const QString cmd = QString("/usr/bin/netctl"),
                                      const QString sudoCmd = QString(""));
    Q_INVOKABLE void stopProfileSlot(const QMap<QString, QVariant> dict,
                                     const bool useHelper = true,
                                     const QString cmd = QString("/usr/bin/netctl"),
                                     const QString sudoCmd = QString(""));
    Q_INVOKABLE void stopAllProfilesSlot(const bool useHelper = true,
                                         const QString cmd = QString("/usr/bin/netctl"),
                                         const QString sudoCmd = QString(""));
    Q_INVOKABLE void switchToProfileSlot(const QStringList profiles,
                                         const bool useHelper = true,
                                         const QString cmd = QString("/usr/bin/netctl-auto"));
    // dataengine
    Q_INVOKABLE QMap<QString, QVariant> readDataEngineConfiguration();
    Q_INVOKABLE void writeDataEngineConfiguration(const QMap<QString, QVariant> configuration);

private:
    bool debug = false;
    QList<QVariant> sendDBusRequest(const QString cmd, const QList<QVariant> args = QList<QVariant>());
};


#endif /* NETCTLADDS_H */
