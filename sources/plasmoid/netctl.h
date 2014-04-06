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

#ifndef NETCTL_PLASMOID_H
#define NETCTL_PLASMOID_H

#include <Plasma/Applet>
#include <Plasma/DataEngine>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Label>
#include <QProcess>

#include <ui_configwindow.h>


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
    // dataengine
    void dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data);
    // configuration interface
    void configAccepted();
    void configChanged();
    void setBigInterface();
    void setSudo();

private slots:
    // ui
    void updateInterface(bool setHidden);
    // configuration interface
    void selectActiveIcon();
    void selectGuiExe();
    void selectInactiveIcon();
    void selectNetctlExe();
    // context menu
    void enableProfileAutoloadSlot();
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
    QAction *enableProfileAutoload;
    QAction *startProfile;
    QAction *stopProfile;
    QAction *restartProfile;
    // data engine
    Plasma::DataEngine *netctlEngine;
    void connectToEngine();
    void disconnectFromEngine();
    // notification
    //
    // configuration interface
    Ui::ConfigWindow uiConfig;
    // configuration
    int autoUpdateInterval;
    QMap<QString, bool> bigInterface;
    QStringList formatLine;
    QMap<QString, QString> paths;
    bool useSudo;
};

K_EXPORT_PLASMA_APPLET(netctl, Netctl)


#endif /* NETCTL_PLASMOID_H */
