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

#ifndef WPASUPINTERACT_H
#define WPASUPINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


class Netctl;

class WpaSup : public QObject
{
    Q_OBJECT

public:
    explicit WpaSup(const bool debugCmd = false,
                    const QMap<QString, QString> settings = QMap<QString, QString>());
    ~WpaSup();
    // general information
    QString existentProfile(const QString profile);
    QStringList getInterfaceList();
    bool isProfileActive(const QString profile);
    bool isProfileExists(const QString profile);

public slots:
    // functions
    QList<QStringList> scanWifi();
    bool startWpaSupplicant();
    bool stopWpaSupplicant();

private:
    Netctl *netctlCommand;
    bool debug;
    QString ctrlDir;
    QString ctrlGroup;
    QDir *ifaceDirectory;
    QString mainInterface;
    QString pidFile;
    QString sudoCommand;
    QString wpaCliPath;
    QString wpaDrivers;
    QString wpaSupPath;
    // functions
    bool wpaCliCall(const QString commandLine);
    QString getWpaCliOutput(const QString commandLine);
};


#endif /* WPASUPINTERACT_H */
