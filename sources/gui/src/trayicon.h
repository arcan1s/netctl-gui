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

#ifndef TRAYICON_H
#define TRAYICON_H

#include <QAction>
#include <QObject>
#include <QSystemTrayIcon>


class MainWindow;

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = 0,
                      const bool debugCmd = false);
    ~TrayIcon();

public slots:
    void showInformation();

private slots:
    void itemActivated(const QSystemTrayIcon::ActivationReason reason);

private:
    bool debug;
    MainWindow *mainWindow;
    // contextual actions
    QMenu *menu;
    QAction *exit;
    QAction *showMainWindow;
    QAction *showNetctlAutoWindow;
    QAction *showStatus;
    void init();
};


#endif /* TRAYICON_H */
