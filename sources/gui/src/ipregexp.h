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

#ifndef IPREGEXP_H
#define IPREGEXP_H

#include <QObject>
#include <QRegExp>
#include <QRegExpValidator>


class IpRegExp : public QObject {
    Q_OBJECT

public:
    explicit IpRegExp(QObject *parent = 0);
    ~IpRegExp();

    // ipv4
    static QRegExp ip4Regex();
    static QRegExp ip4CidrRegex();
    // ipv6
    static QRegExp ip6Regex();
    static QRegExp ip6CidrRegex();
    // validators
    static QRegExpValidator *ipv4Validator();
    static QRegExpValidator *ipv4CidrValidator();
    static QRegExpValidator *ipv6Validator();
    static QRegExpValidator *ipv6CidrValidator();
    // check
    static bool checkString(const QString input, const QRegExp regexp);
};


#endif /* IPREGEXP_H */
