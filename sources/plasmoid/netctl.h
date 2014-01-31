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
    // ui
    int showGui();
    int sendNotification(QString eventId, int num);
    // for dataengine
    void dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data);
    // for configuration interface
    void configAccepted();
    void configChanged();
    int setBigInterface();

private slots:
    void selectGuiExe();
    void selectNetctlExe();
    void selectActiveIcon();
    void selectInactiveIcon();

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
    QString profileName;
    QString profileStatus;
    QString intIp;
    QString extIp;
    QString interfaces;
    QStringList formatLine;
    // context menu
    void createActions();
    QAction *startProfile;
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
