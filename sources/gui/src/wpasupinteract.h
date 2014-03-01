/***************************************************************************
 *   This file is part of netctl-plasmoid                                  *
 *                                                                         *
 *   netctl-plasmoid is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-plasmoid is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-plasmoid. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#ifndef WPASUPINTERACT_H
#define WPASUPINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


class MainWindow;

class WpaSup : public QObject
{
    Q_OBJECT

public:
    explicit WpaSup(MainWindow *wid = 0,
                    const QMap<QString, QString> settings = QMap<QString, QString>());
    ~WpaSup();
    // general information
    QStringList getInterfaceList();
    // functions
    bool wpaCliCall(const QString commandLine);
    QString getWpaCliOutput(const QString commandLine);
    bool isProfileExists(const QString profile);
    QString existentProfile(const QString profile);
    bool isProfileActive(const QString profile);

public slots:
    // functions
    bool startWpaSupplicant();
    bool stopWpaSupplicant();
    QList<QStringList> scanWifi();

private:
    MainWindow *parent;
    QString ctrlDir;
    QString ctrlGroup;
    QDir *ifaceDirectory;
    QString mainInterface;
    QString pidFile;
    QString sudoCommand;
    QString wpaCliPath;
    QString wpaDrivers;
    QString wpaSupPath;
};


#endif /* WPASUPINTERACT_H */
