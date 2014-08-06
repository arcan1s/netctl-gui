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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QTableWidgetItem>


class AboutWindow;
class BridgeWidget;
class ErrorWindow;
class EthernetWidget;
class GeneralWidget;
class IpWidget;
class MacvlanWidget;
class MobileWidget;
class Netctl;
class NetctlAutoWindow;
class NetctlProfile;
class PasswdWidget;
class PppoeWidget;
class SettingsWindow;
class TrayIcon;
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
                        const int startMinimized = false,
                        const bool showAbout = false,
                        const bool showNetctlAuto = false,
                        const bool showSettings = false,
                        const QString selectEssid = QString("ESSID"),
                        const QString openProfile = QString("PROFILE"),
                        const QString selectProfile = QString("PROFILE"),
                        const QString configPath = QString(""),
                        const bool debugCmd = false,
                        const bool defaultSettings = false,
                        const QString options = QString("OPTIONS"),
                        const int tabNum = 1);
    ~MainWindow();
    QString getInformation();
    bool isNetctlAutoWindowHidden();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void updateTabs(const int tab);
    void updateMenu();
    // wifi tab slots
    void connectToUnknownEssid(const QString passwd);
    void setHiddenName(const QString name);
    // actions from trayicon
    void closeMainWindow();
    void showNetctlAutoWindow();
    void showMainWindow();

private slots:
    void reportABug();
    // menu update slots
    void setMenuActionsShown(const bool state = true);
    void updateMenuMain();
    void updateMenuProfile();
    void updateMenuWifi();
    // tab update slots
    void updateMainTab();
    void updateProfileTab();
    void updateWifiTab();
    // main tab slots
    void mainTabContextualMenu(const QPoint &pos);
    void mainTabEditProfile();
    void mainTabRemoveProfile();
    void mainTabEnableProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);
    // profile tab slots
    void profileTabChangeState(const QString current);
    void profileTabClear();
    void profileTabCreateProfile();
    void profileTabLoadProfile();
    void profileTabRemoveProfile();
    // wifi tab slots
    void wifiTabContextualMenu(const QPoint &pos);
    void wifiTabSetEnabled(const bool state);
    void wifiTabStart();
    void wifiTabRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    // ui
    Ui::MainWindow *ui;
    AboutWindow *aboutWin;
    ErrorWindow *errorWin;
    NetctlAutoWindow *netctlAutoWin;
    PasswdWidget *passwdWid;
    SettingsWindow *settingsWin;
    BridgeWidget *bridgeWid;
    EthernetWidget *ethernetWid;
    GeneralWidget *generalWid;
    IpWidget *ipWid;
    MacvlanWidget *macvlanWid;
    MobileWidget *mobileWid;
    PppoeWidget *pppoeWid;
    TunnelWidget *tunnelWid;
    TuntapWidget *tuntapWid;
    VlanWidget *vlanWid;
    WirelessWidget *wirelessWid;
    TrayIcon *trayIcon;
    // backend
    Netctl *netctlCommand;
    NetctlProfile *netctlProfile;
    WpaSup *wpaCommand;
    bool checkExternalApps(const QString apps);
    QString checkStatus(const bool statusBool, const bool nullFalse = false);
    void createActions();
    void keyPressEvent(QKeyEvent *pressedKey);
    void setIconsToTabs();
    bool debug;
    bool hiddenNetwork;
    // configuration
    QMap<QString, QString> configuration;
    QMap<QString, QString> parseOptions(const QString options);
};


#endif /* MAINWINDOW_H */
