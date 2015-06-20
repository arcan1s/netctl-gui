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
    Q_INVOKABLE bool isDebugEnabled();
    Q_INVOKABLE QString parsePattern(const QString pattern);
    Q_INVOKABLE void runCmd(const QString cmd);
    Q_INVOKABLE void setDataBySource(const QString sourceName, const QVariantMap data);
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message);
    Q_INVOKABLE QString valueByKey(const QString key);
    // context menu
    Q_INVOKABLE void enableProfileSlot(const bool useHelper = true,
                                       const QString cmd = QString("/usr/bin/netctl"),
                                       const QString sudoCmd = QString(""));
    Q_INVOKABLE void restartProfileSlot(const bool useHelper = true,
                                        const QString cmd = QString("/usr/bin/netctl"),
                                        const QString sudoCmd = QString(""));
    Q_INVOKABLE void startProfileSlot(const bool useHelper = true,
                                      const QString cmd = QString("/usr/bin/netctl"),
                                      const QString sudoCmd = QString(""));
    Q_INVOKABLE void stopProfileSlot(const bool useHelper = true,
                                     const QString cmd = QString("/usr/bin/netctl"),
                                     const QString sudoCmd = QString(""));
    Q_INVOKABLE void stopAllProfilesSlot(const bool useHelper = true,
                                         const QString cmd = QString("/usr/bin/netctl"),
                                         const QString sudoCmd = QString(""));
    Q_INVOKABLE void switchToProfileSlot(const bool useHelper = true,
                                         const QString cmd = QString("/usr/bin/netctl-auto"));
    // dataengine
    Q_INVOKABLE QVariantMap readDataEngineConfiguration();
    Q_INVOKABLE void writeDataEngineConfiguration(const QVariantMap configuration);

signals:
    void needToBeUpdated();
    void needToNotify(const bool currentStatus);

private slots:
    void notifyAboutStatusChanging(const bool currentStatus);

private:
    bool debug = false;
    QList<QVariant> sendDBusRequest(const QString cmd, const QList<QVariant> args = QList<QVariant>());
    QMap<QString, QString> values;
};


#endif /* NETCTLADDS_H */
