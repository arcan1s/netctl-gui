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
#include <QMainWindow>
#include <QTableWidgetItem>


class BridgeWidget;
class ErrorWindow;
class EthernetWidget;
class GeneralWidget;
class IpWidget;
class MobileWidget;
class Netctl;
class NetctlProfile;
class PasswdWidget;
class PppoeWidget;
class TunnelWidget;
class TuntapWidget;
class VlanWidget;
class WirelessWidget;
class WpaSup;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0,
                        bool defaultSettings = false,
                        int tabNum = 0);
    ~MainWindow();
    Netctl *netctlCommand;
    NetctlProfile *netctlProfile;
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
    void updateProfileTab();
    void updateWifiTab();
    // main tab slots
    void mainTabEnableProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);
    // profile tab slots
    void profileTabBrowseProfile();
    void profileTabChangeState(QString current);
    void profileTabClear();
    void profileTabCreateProfile();
    void profileTabLoadProfile();
    // wifi tab slots
    void wifiTabSetEnabled(bool state);
    void wifiTabStart();
    void wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    // ui
    Ui::MainWindow *ui;
    BridgeWidget *bridgeWid;
    EthernetWidget *ethernetWid;
    GeneralWidget *generalWid;
    IpWidget *ipWid;
    MobileWidget *mobileWid;
    PppoeWidget *pppoeWid;
    TunnelWidget *tunnelWid;
    TuntapWidget *tuntapWid;
    VlanWidget *vlanWid;
    WirelessWidget *wirelessWid;
    // backend
    ErrorWindow *errorWin;
    PasswdWidget *passwdWid;
    bool checkExternalApps(QString apps);
    bool checkState(QString state, QString item);
    void createActions();
    // configuration
    QString ifaceDir;
    QString netctlPath;
    QString preferedInterface;
    QString profileDir;
    QString rfkillDir;
    QString sudoPath;
    QStringList wpaConfig;
};


#endif /* MAINWINDOW_H */
