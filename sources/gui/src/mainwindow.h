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
#include <QShortcut>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QToolButton>

#include <netctlgui/netctlgui.h>


class AboutWindow;
class BridgeWidget;
class EthernetWidget;
class GeneralWidget;
class IpWidget;
class MacvlanWidget;
class MobileWidget;
class NetctlAutoWindow;
class PasswdWidget;
class PppoeWidget;
class SettingsWindow;
class TrayIcon;
class TunnelWidget;
class TuntapWidget;
class VlanWidget;
class WirelessWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0,
                        const QMap<QString, QVariant> args = QMap<QString, QVariant>(),
                        QTranslator *qtAppTranslator = 0,
                        QTranslator *appTranslator = 0);
    ~MainWindow();
    QStringList printInformation();
    QStringList printSettings();
    QStringList printTrayInformation();
    bool isHelperActive();
    bool isHelperServiceActive();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    // actions from trayicon
    void closeMainWindow();
    void showAboutWindow();
    void showMainWindow();
    void showNetctlAutoWindow();
    void showSettingsWindow();
    // trayicon control slots
    bool enableProfileSlot(const QString profile);
    bool startProfileSlot(const QString profile);
    bool stopAllProfilesSlot();
    bool switchToProfileSlot(const QString profile);
    bool restartProfileSlot(const QString profile);
    // open docs
    void showApi();
    void showLibrary();
    void showSecurityNotes();
    // helper
    bool forceStartHelper();
    bool forceStopHelper();
    bool startHelper();
    // main
    void setTab(int tab);
    void updateConfiguration(const QMap<QString, QVariant> args = QMap<QString, QVariant>());
    void updateMenu();
    void updateTabs(const int tab);
    // wifi tab slots
    void connectToUnknownEssid(const QString passwd);
    void setHiddenName(const QString name);

signals:
    void needToBeConfigured();

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
    void mainTabEnableProfile();
    QStringList mainTabGetActiveProfiles();
    void mainTabRemoveProfile();
    void mainTabRestartProfile();
    void mainTabStartProfile();
    void mainTabStopAllProfiles();
    void mainTabSwitchToProfile();
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

private:
    // ui
    TrayIcon *trayIcon = nullptr;
    QMap<QString, QAction *> toolBarActions;
    QToolButton *actionMenu = nullptr;
    QToolBar *actionToolBar = nullptr;
    Ui::MainWindow *ui = nullptr;
    AboutWindow *aboutWin = nullptr;
    NetctlAutoWindow *netctlAutoWin = nullptr;
    PasswdWidget *passwdWid = nullptr;
    SettingsWindow *settingsWin = nullptr;
    BridgeWidget *bridgeWid = nullptr;
    EthernetWidget *ethernetWid = nullptr;
    GeneralWidget *generalWid = nullptr;
    IpWidget *ipWid = nullptr;
    MacvlanWidget *macvlanWid = nullptr;
    MobileWidget *mobileWid = nullptr;
    PppoeWidget *pppoeWid = nullptr;
    TunnelWidget *tunnelWid = nullptr;
    TuntapWidget *tuntapWid = nullptr;
    VlanWidget *vlanWid = nullptr;
    WirelessWidget *wirelessWid = nullptr;
    // backend
    Netctl *netctlCommand = nullptr;
    NetctlProfile *netctlProfile = nullptr;
    WpaSup *wpaCommand = nullptr;
    bool checkExternalApps(const QString apps);
    bool checkHelperStatus();
    QString checkStatus(const bool statusBool, const bool nullFalse = false);
    void createActions();
    void createDBusSession();
    void createObjects();
    void createToolBars();
    void deleteObjects();
    void keyPressEvent(QKeyEvent *pressedKey);
    QString configPath;
    bool debug = false;
    bool hiddenNetwork;
    bool useHelper = true;
    QTranslator *qtTranslator = nullptr;
    QTranslator *translator = nullptr;
    // configuration
    QMap<QString, QString> configuration;
    QMap<QString, QString> parseOptions(const QString options);
};


#endif /* MAINWINDOW_H */
