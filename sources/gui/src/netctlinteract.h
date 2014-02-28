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

#ifndef NETCTLINTERACT_H
#define NETCTLINTERACT_H

#include <QDir>
#include <QMap>
#include <QObject>


class MainWindow;

class Netctl : public QObject
{
    Q_OBJECT

public:
    explicit Netctl(MainWindow *wid = 0,
                    QMap<QString, QString> settings = QMap<QString, QString>());
    ~Netctl();
    // general information
    QList<QStringList> getProfileList();
    QStringList getProfileDescriptions(const QStringList profileList);
    QStringList getProfileStatuses(const QStringList profileList);
    QString getSsidFromProfile(const QString profile);
    bool isProfileActive(const QString profile);
    bool isProfileEnabled(const QString profile);

public slots:
    // functions
    bool enableProfile(const QString profile);
    bool restartProfile(const QString profile);
    bool startProfile(const QString profile);

private:
    MainWindow *parent;
    QString netctlCommand;
    QDir *profileDirectory;
    QString sudoCommand;
};


#endif /* NETCTLINTERACT_H */
