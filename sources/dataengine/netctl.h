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

#ifndef NETCTL_DE_H
#define NETCTL_DE_H

#include <Plasma/DataEngine>


class Netctl : public Plasma::DataEngine
{
    Q_OBJECT

public:
    Netctl(QObject *parent, const QVariantList &args);

protected:
    bool readConfiguration();
    bool sourceRequestEvent(const QString &name);
    bool updateSourceEvent(const QString &source);
    QStringList sources() const;

private:
    // configuration
    QMap<QString, QString> configuration;
};


#endif /* NETCTL_DE_H */
