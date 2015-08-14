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

    Q_INVOKABLE bool checkHelperStatus() const;
    Q_INVOKABLE QString getAboutText(const QString type = QString("header")) const;
    Q_INVOKABLE bool isDebugEnabled() const;
    Q_INVOKABLE void runCmd(const QString cmd) const;
    Q_INVOKABLE void setDataBySource(const QString sourceName, const QVariantMap data);
    Q_INVOKABLE void setPattern(const QString _pattern);
    Q_INVOKABLE static void sendNotification(const QString eventId, const QString message);
    // context menu
    Q_INVOKABLE void enableProfileSlot(const bool useHelper = true,
                                       const QString cmd = QString("netctl"),
                                       const QString sudoCmd = QString("")) const;
    Q_INVOKABLE void restartProfileSlot(const bool useHelper = true,
                                        const QString cmd = QString("netctl"),
                                        const QString sudoCmd = QString("")) const;
    Q_INVOKABLE void startProfileSlot(const bool useHelper = true,
                                      const QString cmd = QString("netctl"),
                                      const QString sudoCmd = QString("")) const;
    Q_INVOKABLE void stopProfileSlot(const bool useHelper = true,
                                     const QString cmd = QString("netctl"),
                                     const QString sudoCmd = QString("")) const;
    Q_INVOKABLE void stopAllProfilesSlot(const bool useHelper = true,
                                         const QString cmd = QString("netctl"),
                                         const QString sudoCmd = QString("")) const;
    Q_INVOKABLE void switchToProfileSlot(const bool useHelper = true,
                                         const QString cmd = QString("netctl-auto")) const;
    // dataengine
    Q_INVOKABLE QVariantMap readDataEngineConfiguration() const;
    Q_INVOKABLE void writeDataEngineConfiguration(const QVariantMap configuration) const;

signals:
    void needIconToBeUpdated(const QString newIcon) const;
    void needMenuUpdate(const QString current, const QString status, const bool isActive) const;
    void needTextToBeUpdated(const QString newText, const QString newToolTip) const;
    void needToBeUpdated() const;
    void needToNotify(const bool currentStatus) const;

private slots:
    void notifyAboutStatusChanging(const bool currentStatus) const;
    void parsePattern() const;

private:
    bool debug = false;
    QVariantList sendDBusRequest(const QString cmd,
                                 const QVariantList args = QVariantList()) const;
    QString pattern;
    QHash<QString, QString> values;
};


#endif /* NETCTLADDS_H */
