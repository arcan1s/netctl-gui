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

#ifndef NEWPROFILEWIDGET_H
#define NEWPROFILEWIDGET_H

#include <QMainWindow>

#include <netctlgui/netctlgui.h>


class BridgeWidget;
class EthernetWidget;
class GeneralWidget;
class IpWidget;
class MainWindow;
class MacvlanWidget;
class MobileWidget;
class PppoeWidget;
class TunnelWidget;
class TuntapWidget;
class VlanWidget;
class WirelessWidget;

namespace Ui {
class NewProfileWidget;
}

class NewProfileWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewProfileWidget(QWidget *parent = 0,
                              const QMap<QString,QString> settings = QMap<QString,QString>(),
                              const bool debugCmd = false);
    ~NewProfileWidget();
    Qt::ToolBarArea getToolBarArea();

public slots:
    void profileTabOpenProfileSlot(const QString profile);
    void update();
    void updateToolBarState(const Qt::ToolBarArea area = Qt::TopToolBarArea);

private slots:
    // update slots
    void updateMenuProfile();
    void updateProfileTab();
    // profile tab slots
    void profileTabChangeState(const QString current);
    void profileTabCreateProfile();
    void profileTabLoadProfile();
    void profileTabOpenInEditor();
    void profileTabRemoveProfile();

private:
    // ui
    MainWindow *mainWindow;
    Ui::NewProfileWidget *ui = nullptr;
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
    void createActions();
    bool debug = false;
    bool useHelper = true;
    // configuration
    QMap<QString, QString> configuration;
};


#endif /* NEWPROFILEWIDGET_H */
