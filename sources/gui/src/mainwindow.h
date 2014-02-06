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
#include <QMainWindow>
#include <QTableWidgetItem>


class Netctl;
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
    enum wpaConfigIndex {
        wpaCliPath = 0,
        wpaSupplicantPath = 1,
        wpaPidPath = 2,
        wpadSupDrivers = 3,
        wpaConfDir = 4,
        wpaConfGroup = 5
    };
    bool checkExternalApps();
    
private slots:
    void updateTabs(const int tab);
    void updateMainTab();
    void updateWifiTab();
    // main tab slots
    void mainTabEnableProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    Netctl *netctlCommand;
    WpaSup *wpaCommand;
    Ui::MainWindow *ui;
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
