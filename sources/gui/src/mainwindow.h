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
#include <QTreeWidgetItem>

#include <netctlgui/netctlgui.h>


class AboutWindow;
class MainWidget;
class NetctlAutoWindow;
class NewProfileWidget;
class SettingsWindow;
class TrayIcon;
class WiFiMenuWidget;

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
    Qt::ToolBarArea getToolBarArea();
    QStringList printInformation();
    QStringList printSettings();
    QStringList printTrayInformation();
    bool isHelperActive();
    bool isHelperServiceActive();
    // library interfaces
    Netctl *netctlCommand = nullptr;
    NetctlProfile *netctlProfile = nullptr;
    WpaSup *wpaCommand = nullptr;

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    // actions from trayicon
    void closeMainWindow();
    void openProfileSlot(const QString profile);
    void showAboutWindow();
    void showMainWindow();
    void showNetctlAutoWindow();
    void showSettingsWindow();
    // open docs
    void showApi();
    void showLibrary();
    void showSecurityNotes();
    // helper
    bool forceStartHelper();
    bool forceStopHelper();
    bool startHelper();
    // main
    void setDisabled(const bool disabled = true);
    void setTab(int tab);
    void showMessage(const bool status);
    void storeToolBars();
    void updateConfiguration(const QMap<QString, QVariant> args = QMap<QString, QVariant>());
    void updateTabs(const int tab);
    void updateToolBarState(const Qt::ToolBarArea area = Qt::TopToolBarArea);

signals:
    void needToBeConfigured();

private slots:
    void setMainTab();
    void setProfileTab();
    void setWifiTab();
    void reportABug();

private:
    // ui
    TrayIcon *trayIcon = nullptr;
    Ui::MainWindow *ui = nullptr;
    AboutWindow *aboutWin = nullptr;
    MainWidget *mainWidget = nullptr;
    NetctlAutoWindow *netctlAutoWin = nullptr;
    NewProfileWidget *newProfileWidget = nullptr;
    SettingsWindow *settingsWin = nullptr;
    WiFiMenuWidget *wifiMenuWidget = nullptr;
    // backend
    bool checkHelperStatus();
    void createActions();
    void createDBusSession();
    void createObjects();
    void deleteObjects();
    QString configPath;
    bool debug = false;
    bool useHelper = true;
    QTranslator *qtTranslator = nullptr;
    QTranslator *translator = nullptr;
    // configuration
    QMap<QString, QString> configuration;
    QMap<QString, QString> parseOptions(const QString options);
};


#endif /* MAINWINDOW_H */
