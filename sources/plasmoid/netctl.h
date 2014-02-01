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
 *   along with Foobar.  If not, see <http://www.gnu.org/licenses/>.       *
 ***************************************************************************/

#ifndef NETCTL_PLASMOID_H
#define NETCTL_PLASMOID_H

#include <Plasma/Applet>
#include <Plasma/Frame>
#include <Plasma/IconWidget>
#include <Plasma/Label>

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
    void showGui();
    void sendNotification(const QString eventId, const int num);
    // dataengine
    void dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data);
    // configuration interface
    void configAccepted();
    void configChanged();
    void setBigInterface();
    void setSudo();

private slots:
    // configuration interface
    void selectGuiExe();
    void selectNetctlExe();
    void selectActiveIcon();
    void selectInactiveIcon();
    // context menu
    void startProfileSlot(QAction *action);
    void stopProfileSlot();
    void restartProfileSlot();
    void enableProfileAutoloadSlot();

protected:
    void createConfigurationInterface(KConfigDialog *parent);
    QList<QAction*> contextualActions();

private:
    // ui
    QGraphicsLinearLayout *fullSpaceLayout;
    // icon
    Plasma::Frame *iconFrame;
    Plasma::IconWidget *iconWidget;
    // text
    Plasma::Frame *textFrame;
    Plasma::Label *textLabel;
    bool status;
    QString profileName;
    QString profileStatus;
    QString intIp;
    QString extIp;
    QString interfaces;
    QStringList profileList;
    QStringList formatLine;
    // context menu
    void createActions();
    QList<QAction*> menuActions;
    QAction *startProfile;
    QMenu *startProfileMenu;
    QAction *stopProfile;
    QAction *restartProfile;
    QAction *enableProfileAutoload;
    // data engine
    void connectToEngine();
    void disconnectFromEngine();
    // configuration interface
    Ui::ConfigWindow uiConfig;
    // configuration
    int autoUpdateInterval;
    QString guiPath;
    QString netctlPath;
    bool useSudo;
    QString sudoPath;
    bool showBigInterface;
    bool showNetDev;
    bool showExtIp;
    bool showIntIp;
    QString fontFamily;
    int fontSize;
    QString fontColor;
    int fontWeight;
    QString fontStyle;
    QString activeIconPath;
    QString inactiveIconPath;
};

K_EXPORT_PLASMA_APPLET(netctl, Netctl)

#endif /* NETCTL_PLASMOID_H */
