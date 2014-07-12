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

#ifndef NETCTLPROFILE_H
#define NETCTLPROFILE_H

#include <QDir>
#include <QMap>
#include <QObject>


class NetctlProfile : public QObject
{
    Q_OBJECT

public:
    explicit NetctlProfile(const bool debugCmd = false,
                           const QMap<QString, QString> settings = QMap<QString, QString>());
    ~NetctlProfile();
    bool copyProfile(const QString oldPath);
    QString createProfile(const QString profile, const QMap<QString, QString> settings);
    QMap<QString, QString> getSettingsFromProfile(const QString profile);
    bool removeProfile(const QString profile);

private:
    bool debug;
    QDir *profileDirectory;
    QString sudoCommand;
};


#endif /* NETCTLPROFILE_H */
