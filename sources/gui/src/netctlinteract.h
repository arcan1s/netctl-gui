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
#include <QWidget>


class MainWindow;

class Netctl : public QWidget
{
    Q_OBJECT

public:
    Netctl(MainWindow *wid, QString netctlPath, QString profileDir, QString sudoPath);
    ~Netctl();
    // general information
    QStringList getProfileDescriptions(QStringList profileList);
    QStringList getProfileList();
    QStringList getProfileStatuses(QStringList profileList);
    bool isProfileActive(QString profile);
    bool isProfileEnabled(QString profile);
    // functions
    bool enableProfile(QString profile);
    bool restartProfile(QString profile);
    bool startProfile(QString profile);

private:
    MainWindow *parent;
    QString netctlCommand;
    QDir *profileDirectory;
    QString sudoCommand;
};

#endif /* NETCTLINTERACT_H */
