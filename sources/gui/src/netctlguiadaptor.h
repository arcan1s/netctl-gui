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

#ifndef NETCTLGUIADAPTOR_H
#define NETCTLGUIADAPTOR_H

#include <QDBusAbstractAdaptor>
#include <QStringList>


class MainWindow;

class NetctlGuiAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.netctlgui.netctlgui")

public:
    explicit NetctlGuiAdaptor(MainWindow *parent = 0,
                              const bool debugCmd = false);
    ~NetctlGuiAdaptor();

public slots:
    bool Close();
    QString Information();
    bool RestoreWindow();
    QStringList Settings();
    bool ShowAbout();
    bool ShowMain();
    bool ShowNetctlAuto();
    bool ShowSettings();

private:
    bool debug;
    MainWindow *mainWindow;
};


#endif /* NETCTLGUIADAPTOR_H */
