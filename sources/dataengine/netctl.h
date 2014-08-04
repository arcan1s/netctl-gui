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
#include <QProcess>


class Netctl : public Plasma::DataEngine
{
    Q_OBJECT

public:
    Netctl(QObject *parent, const QVariantList &args);
    ~Netctl();
    void getCurrentProfile(const QString cmdNetctl, const QString cmdNetctlAuto);
    void getExtIp(const QString cmd);
    void getExtIp6(const QString cmd);
    QStringList getInterfaceList();
    QString getIntIp();
    QString getIntIp6();
    void getNetctlAutoStatus(const QString cmdNetctlAuto);
    void getProfileList(const QString cmdNetctl, const QString cmdNetctlAuto);
    void getProfileStringStatus(const QString cmdNetctl, const QString cmdNetctlAuto);

protected:
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private slots:
    void setCurrentProfile(int exitCode, QProcess::ExitStatus exitStatus);
    void setExtIp(int exitCode, QProcess::ExitStatus exitStatus);
    void setExtIp6(int exitCode, QProcess::ExitStatus exitStatus);
    void setNetctlAutoStatus(int exitCode, QProcess::ExitStatus exitStatus);
    void setProfileList(int exitCode, QProcess::ExitStatus exitStatus);
    void setProfileStringStatus(int exitCode, QProcess::ExitStatus exitStatus);

private:
    bool netctlAutoStatus;
    QString currentProfile;
    // processes
    QMap<QString, QProcess *> processes;
    // configuration
    bool debug;
    QMap<QString, QString> configuration;
    void initValues();
    void setKeys();
    void setProcesses();
    void readConfiguration();
    QMap<QString, QString> updateConfiguration(const QMap<QString, QString> rawConfig);
};


#endif /* NETCTL_DE_H */
