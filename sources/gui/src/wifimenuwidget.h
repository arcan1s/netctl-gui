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

#ifndef WIFIMENUWIDGET_H
#define WIFIMENUWIDGET_H

#include <QApplication>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

#include <netctlgui/netctlgui.h>


class MainWindow;
class PasswdWidget;

namespace Ui {
class WiFiMenuWidget;
}

class WiFiMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WiFiMenuWidget(QWidget *parent = 0,
                            const QMap<QString,QString> settings = QMap<QString,QString>(),
                            const bool debugCmd = false);
    ~WiFiMenuWidget();

public slots:
    void update();
    // wifi tab slots
    void connectToUnknownEssid(const QString passwd);
    void setHiddenName(const QString name);

private slots:
    // update slots
    void updateMenuWifi();
    void updateWifiTab();
    // wifi tab slots
    void wifiTabContextualMenu(const QPoint &pos);
    void wifiTabSetEnabled(const bool state);
    void wifiTabStart();

private:
    // ui
    QMap<QString, QAction *> toolBarActions;
    QToolBar *actionToolBar = nullptr;
    MainWindow *mainWindow = nullptr;
    Ui::WiFiMenuWidget *ui = nullptr;
    PasswdWidget *passwdWid = nullptr;
    // backend
    Netctl *netctlCommand = nullptr;
    NetctlProfile *netctlProfile = nullptr;
    WpaSup *wpaCommand = nullptr;
    void createActions();
    void createObjects();
    void createToolBars();
    void deleteObjects();
    bool debug = false;
    bool hiddenNetwork;
    bool useHelper = true;
    // configuration
    QMap<QString, QString> configuration;
};


#endif /* WIFIMENUWIDGET_H */
