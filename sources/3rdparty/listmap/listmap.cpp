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

#include "listmap.h"


QMap<QString, QString> listToMap(const QStringList list)
{
    QMap<QString, QString> map;
    for (int i=0; i<list.count(); i++) {
        QStringList str = list[i].split(QString("=="));
        if (str.count() != 2) continue;
        map[str[0]] = str[1];
    }

    return map;
}


QStringList mapToList(const QMap<QString, QString> map)
{
    QStringList list;
    for (int i=0; i<map.keys().count(); i++)
        list.append(QString("%1==%2").arg(map.keys()[i]).arg(map[map.keys()[i]]));

    return list;
}
