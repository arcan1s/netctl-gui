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

#ifndef NETCTLHELPER_H
#define NETCTLHELPER_H

#include <QMap>
#include <QObject>
#include <QVariant>


class NetctlHelper : public QObject
{
    Q_OBJECT

public:
    explicit NetctlHelper(QObject *parent = 0,
                          QMap<QString, QVariant> args = QMap<QString, QVariant>());
    ~NetctlHelper();
    QStringList printSettings();

public slots:
    void updateConfiguration();
    void quitHelper();

private:
    QString configPath;
    QMap<QString, QString> configuration;
    bool debug;
    void createInterface();
    void deleteInterface();
    QMap<QString, QString> getDefault();
    QMap<QString, QString> getSettings();
};


#endif /* NETCTLHELPER_H */
