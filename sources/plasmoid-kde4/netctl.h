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

#include <Plasma/DataEngine>
#include <Plasma/PopupApplet>
#include <QProcess>

#include <ui_about.h>
#include <ui_appearance.h>
#include <ui_dataengine.h>
#include <ui_widget.h>


class Netctl;

class IconLabel : public QLabel
{
    Q_OBJECT

public:
    IconLabel(Netctl *wid, const bool debugCmd);
    ~IconLabel();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    bool debug;
    Netctl *widget;
};

class Netctl : public Plasma::PopupApplet
{
    Q_OBJECT

public:
    Netctl(QObject *parent, const QVariantList &args);
    ~Netctl();
    void init();
    QString parsePattern(const QString rawLine);
    QMap<QString, QString> readDataEngineConfiguration();
    void writeDataEngineConfiguration(const QMap<QString, QString> configuration);

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
    void setDataEngineExternalIp4();
    void setDataEngineExternalIp6();
    void setHelper();
    void setSudo();
    void setWifi();

private slots:
    // ui
    void updateIcon();
    // configuration interface
    void selectAbstractSomething();
    // context menu
    void enableProfileSlot();
    void restartProfileSlot();
    void startProfileSlot(QAction *profile);
    void stopProfileSlot();
    void stopAllProfilesSlot();
    void switchToProfileSlot(QAction *profile);
    // helper
    void checkHelperStatus();
    void startHelper();

protected:
    void createConfigurationInterface(KConfigDialog *parent);
    QList<QAction*> contextualActions();

private:
    // ui
    QWidget *graphicsWidget = nullptr;
    QHBoxLayout *layout = nullptr;
    IconLabel *iconLabel = nullptr;
    QLabel *textLabel = nullptr;
    // information
    bool status = false;
    QMap<QString, QString> info;
    QStringList profileList;
    // context menu
    void createActions();
    QList<QAction*> menuActions;
    QMenu *startProfileMenu = nullptr;
    QMenu *switchToProfileMenu = nullptr;
    QMap<QString, QAction*> contextMenu;
    // data engine
    Plasma::DataEngine *netctlEngine = nullptr;
    void connectToEngine();
    void disconnectFromEngine();
    QList<QVariant> sendDBusRequest(const QString cmd, const QList<QVariant> args = QList<QVariant>());
    // configuration interface
    Ui::AppearanceWindow uiAppConfig;
    Ui::DataEngineWindow uiDEConfig;
    Ui::ConfigWindow uiWidConfig;
    Ui::About uiAboutConfig;
    // configuration
    int autoUpdateInterval = 1000;
    bool debug = false;
    QString textPattern = QString();
    QStringList formatLine;
    QMap<QString, QString> paths;
    bool useHelper = true;
    bool useSudo = false;
    bool useWifi = false;
};

K_EXPORT_PLASMA_APPLET(netctl, Netctl)


#endif /* NETCTL_PLASMOID_H */
