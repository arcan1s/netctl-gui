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
    WpaSup(MainWindow *wid, QStringList wpaConfig, QString sudoPath, QString ifaceDir, QString preferedInterface);
    ~WpaSup();
    // general information
    QStringList getInterfaceList();
    // functions
    bool wpaCliCall(QString commandLine);
    QString getWpaCliOutput(QString commandLine);
    bool isProfileExists(QString profile);
    bool isProfileActive(QString profile);

public slots:
    // functions
    bool startWpaSupplicant();
    bool stopWpaSupplicant();
    QList<QStringList> scanWifi();

private:
    MainWindow *parent;
    QStringList wpaConf;
    QString sudoCommand;
    QDir *ifaceDirectory;
    QString mainInterface;
};


#endif /* WPASUPINTERACT_H */
