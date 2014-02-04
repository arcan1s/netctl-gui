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
#include <QWidget>


class MainWindow;

class WpaSup : public QWidget
{
    Q_OBJECT

public:
    WpaSup(MainWindow *wid, QString wpaCliPath, QString ifaceDir);
    ~WpaSup();
    // general information
    QStringList getInterfaceList();

private:
    MainWindow *parent;
    QString wpaCliCommand;
    QDir *ifaceDirectory;
};


#endif /* WPASUPINTERACT_H */
