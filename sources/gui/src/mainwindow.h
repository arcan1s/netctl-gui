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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QItemSelection>
#include <QTableWidgetItem>
#include <QMainWindow>


class ErrorWindow;
class Netctl;
class PasswdWidget;
class WpaSup;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Netctl *netctlCommand;
    WpaSup *wpaCommand;
    enum wpaConfigIndex {
        wpaCliPath = 0,
        wpaSupplicantPath = 1,
        wpaPidPath = 2,
        wpadSupDrivers = 3,
        wpaConfDir = 4,
        wpaConfGroup = 5
    };

public slots:
    void updateTabs(const int tab);
    // wifi tab slots
    void connectToUnknownEssid(QString passwd);
    
private slots:
    void updateMainTab();
    void updateWifiTab();
    // main tab slots
    void mainTabEnableProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);
    // wifi tab slots
    void wifiTabSetEnabled(bool state);
    void wifiTabStart();
    void wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    Ui::MainWindow *ui;
    ErrorWindow *errorwin;
    PasswdWidget *passwdwid;
    bool checkExternalApps(QString apps);
    bool checkState(QString state, QString item);
    void createActions();
    // configuration
    QString netctlPath;
    QString profileDir;
    QString sudoPath;
    QStringList wpaConfig;
    QString ifaceDir;
    QString preferedInterface;
};


#endif /* MAINWINDOW_H */
