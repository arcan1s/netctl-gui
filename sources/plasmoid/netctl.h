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

#ifndef NETCTL_PLASMOID_H
#define NETCTL_PLASMOID_H

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <QProcess>

#include <ui_appearance.h>
#include <ui_widget.h>


class QGraphicsLinearLayout;

class Netctl : public Plasma::Applet
{
    Q_OBJECT

public:
    Netctl(QObject *parent, const QVariantList &args);
    ~Netctl();
    void init();

public slots:
    // events
    void sendNotification(const QString eventId, const QString message);
    void showGui();
    void showWifi();
    // dataengine
    void dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data);
    // configuration interface
    void configAccepted();
    void configChanged();
    void setBigInterface();
    void setSudo();
    void setWifi();

private slots:
    // ui
    void updateInterface(bool setHidden);
    // configuration interface
    void selectActiveIcon();
    void selectGuiExe();
    void selectInactiveIcon();
    void selectNetctlExe();
    void selectSudoExe();
    void selectWifiExe();
    // context menu
    void enableProfileSlot();
    void startProfileSlot(QAction *profile);
    void stopProfileSlot();
    void restartProfileSlot();

protected:
    void createConfigurationInterface(KConfigDialog *parent);
    QList<QAction*> contextualActions();

private:
    // ui
    QGraphicsLinearLayout *fullSpaceLayout;
    // icon
    Plasma::IconWidget *iconWidget;
    // text
    Plasma::Label *textLabel;
    bool status;
    QMap<QString, QString> info;
    QStringList profileList;
    // context menu
    void createActions();
    QList<QAction*> menuActions;
    QMenu *startProfileMenu;
    QMap<QString, QAction*> contextMenu;
    // data engine
    Plasma::DataEngine *netctlEngine;
    void connectToEngine();
    void disconnectFromEngine();
    // configuration interface
    Ui::AppearanceWindow uiAppConfig;
    Ui::ConfigWindow uiWidConfig;
    // configuration
    int autoUpdateInterval;
    QMap<QString, bool> bigInterface;
    QStringList formatLine;
    QMap<QString, QString> paths;
    bool useSudo, useWifi;
};

K_EXPORT_PLASMA_APPLET(netctl, Netctl)


#endif /* NETCTL_PLASMOID_H */
