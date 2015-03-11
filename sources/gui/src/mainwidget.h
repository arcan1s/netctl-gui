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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include <netctlgui/netctlgui.h>


class MainWindow;
class NetctlAutoWindow;

namespace Ui {
class MainWidget;
}

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0,
                        const QMap<QString,QString> settings = QMap<QString,QString>(),
                        const bool debugCmd = false);
    ~MainWidget();

public slots:
    bool mainTabSelectProfileSlot(const QString profile);
    void showNetctlAutoWindow();
    void update();

private slots:
    // update slots
    void updateMenuMain();
    void updateMainTab();
    // main tab slots
    void mainTabContextualMenu(const QPoint &pos);
    void mainTabEditProfile();
    void mainTabEnableProfile();
    void mainTabRemoveProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabStopAllProfiles();
    void mainTabSwitchToProfile();

private:
    // ui
    MainWindow *mainWindow = nullptr;
    Ui::MainWidget *ui = nullptr;
    NetctlAutoWindow *netctlAutoWin = nullptr;
    // backend
    Netctl *netctlCommand = nullptr;
    NetctlProfile *netctlProfile = nullptr;
    void createActions();
    void createObjects();
    void deleteObjects();
    QString configPath;
    bool debug = false;
    bool useHelper = true;
    // configuration
    QMap<QString, QString> configuration;
};


#endif /* MAINWIDGET_H */
