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

#include <QDebug>
#include <QProcessEnvironment>

#include <pdebug/pdebug.h>

#include "netctladds.h"
#include "version.h"


NetctlAdds::NetctlAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("NETCTLGUI_DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


NetctlAdds::~NetctlAdds()
{
    if (debug) qDebug() << PDEBUG;
}


QString NetctlAdds::getInfo(const QString current, const QString status)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Current profiles" << current;
    if (debug) qDebug() << PDEBUG << ":" << "Statuses" << status;

    QStringList profiles;
    for (int i=0; i<current.split(QChar('|')).count(); i++)
        profiles.append(current.split(QChar('|'))[i] +
                QString(" (") + status.split(QChar('|'))[i] + QString(")"));

    return profiles.join(QString(" | "));
}


QString NetctlAdds::parsePattern(const QString pattern, const QMap<QString, QVariant> dict)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Dictionary" << dict;

    QString parsed = pattern;
    for (int i=0; i<dict.keys().count(); i++)
        parsed.replace(QString("$") + dict.keys()[i], dict[dict.keys()[i]].toString());
    // fix newline
    parsed.replace(QString("\n"), QString("<br>"));

    return parsed;
}

