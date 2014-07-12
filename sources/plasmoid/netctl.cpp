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

#include "netctl.h"
#include <ui_appearance.h>
#include <ui_widget.h>

#include <KConfigDialog>
#include <KFileDialog>
#include <KNotification>
#include <KUrl>
#include <plasma/theme.h>

#include <QGraphicsLinearLayout>
#include <QMenu>

#include <version.h>


Netctl::Netctl(QObject *parent, const QVariantList &args)
    : Plasma::Applet(parent, args)
{
    setBackgroundHints(DefaultBackground);
    setHasConfigurationInterface(true);
    // text format init
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


Netctl::~Netctl()
{
//    delete startProfileMenu;
//    delete switchToProfileMenu;
//    delete startProfile;
//    delete stopProfile;
//    delete restartProfile;
//    delete enableProfile;

//    delete iconWidget;

//    delete netctlEngine;
}


void Netctl::init()
{
    info[QString("name")] = QString("N\\A");
    info[QString("status")] = QString("N\\A");
    info[QString("intIp")] = QString("N\\A");
    info[QString("extIp")] = QString("N\\A");
    info[QString("interfaces")] = QString("lo");

    netctlEngine = dataEngine(QString("netctl"));
    createActions();
    // generate ui
    // main layout
    fullSpaceLayout = new QGraphicsLinearLayout();
    fullSpaceLayout->setContentsMargins(1, 1, 1, 1);
    setLayout(fullSpaceLayout);

    // frames
    // icon
    iconWidget = new Plasma::IconWidget(KIcon(""), QString(), this);
    iconWidget->setPreferredSize(30, 30);
    connect(iconWidget, SIGNAL(doubleClicked()), this, SLOT(showGui()));
    fullSpaceLayout->addItem(iconWidget);
    // text
    textLabel = new Plasma::Label();
    textLabel->setPreferredHeight(30);
    fullSpaceLayout->addItem(textLabel);

    // read variables
    configChanged();
}


void Netctl::updateInterface(bool setShown)
{
    if (setShown) {
        textLabel->show();
        fullSpaceLayout->updateGeometry();
        updateGeometry();
    }
    else {
        textLabel->hide();
        fullSpaceLayout->updateGeometry();
        updateGeometry();
    }
}


// context menu
void Netctl::enableProfileSlot()
{
    QProcess command;
    QString commandLine, enableStatus;
    if (info[QString("status")].contains(QString("enabled"))) {
        enableStatus = QString(" disable ");
        sendNotification(QString("Info"), i18n("Set profile %1 disabled", info[QString("name")]));
    }
    else {
        enableStatus = QString(" enable ");
        sendNotification(QString("Info"), i18n("Set profile %1 enabled", info[QString("name")]));
    }
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" ") + paths[QString("netctl")] +
                enableStatus + info[QString("name")];
    else
        commandLine = paths[QString("netctl")] + enableStatus + info[QString("name")];
    command.startDetached(commandLine);
}


void Netctl::startProfileSlot(QAction *profile)
{
    bool ready = true;
    QProcess command;
    QString commandLine;
    commandLine = QString("");
    sendNotification(QString("Info"), i18n("Start profile %1", profile->text().remove(QString("&"))));
    if (status) {
        commandLine = paths[QString("netctl")] + QString(" stop ") + info[QString("name")];
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ") + commandLine;
        command.start(commandLine);
        command.waitForFinished(-1);
        if (command.exitCode() != 0)
            ready = false;
    }
    if (ready) {
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ") + paths[QString("netctl")] +
                    QString(" start ") + profile->text().remove(QString("&"));
        else
            commandLine = paths[QString("netctl")] + QString(" start ") +
                    profile->text().remove(QString("&"));
        command.startDetached(commandLine);
    }
}


void Netctl::stopProfileSlot()
{
    QProcess command;
    QString commandLine;
    sendNotification(QString("Info"), i18n("Stop profile %1", info[QString("name")]));
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" ") + paths[QString("netctl")] +
                QString(" stop ") + info[QString("name")];
    else
        commandLine = paths[QString("netctl")] + QString(" stop ") + info[QString("name")];
    command.startDetached(commandLine);
}


void Netctl::switchToProfileSlot(QAction *profile)
{
    QProcess command;
    QString commandLine;
    commandLine = QString("");
    sendNotification(QString("Info"), i18n("Switch to profile %1", profile->text().remove(QString("&"))));
    commandLine = paths[QString("netctl-auto")] + QString(" switch-to ") +
            profile->text().remove(QString("&"));
    command.startDetached(commandLine);
}


void Netctl::restartProfileSlot()
{
    QProcess command;
    QString commandLine;
    sendNotification(QString("Info"), i18n("Restart profile %1", info[QString("name")]));
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" ") + paths[QString("netctl")] +
                QString(" restart ") + info[QString("name")];
    else
        commandLine = paths[QString("netctl")] + QString(" restart ") + info[QString("name")];
    command.startDetached(commandLine);
}


QList<QAction*> Netctl::contextualActions()
{
    if (status)
        contextMenu[QString("title")]->setIcon(QIcon(paths[QString("active")]));
    else
        contextMenu[QString("title")]->setIcon(QIcon(paths[QString("inactive")]));
    contextMenu[QString("title")]->setText(info[QString("name")] + QString(" ") + info[QString("status")]);

    if (info[QString("status")] == QString("(netctl-auto)")) {
        contextMenu[QString("start")]->setVisible(false);
        contextMenu[QString("stop")]->setVisible(false);
        contextMenu[QString("switch")]->setVisible(true);
        contextMenu[QString("restart")]->setVisible(false);
        contextMenu[QString("enable")]->setVisible(false);

        switchToProfileMenu->clear();
        for (int i=0; i<profileList.count(); i++) {
            QAction *profile = new QAction(profileList[i], this);
            switchToProfileMenu->addAction(profile);
        }
    }
    else {
        contextMenu[QString("start")]->setVisible(true);
        contextMenu[QString("stop")]->setVisible(true);
        contextMenu[QString("switch")]->setVisible(false);
        contextMenu[QString("restart")]->setVisible(true);
        contextMenu[QString("enable")]->setVisible(true);

        if (status) {
            contextMenu[QString("start")]->setText(i18n("Start another profile"));
            contextMenu[QString("stop")]->setVisible(true);
            contextMenu[QString("stop")]->setText(i18n("Stop %1", info[QString("name")]));
            contextMenu[QString("restart")]->setVisible(true);
            contextMenu[QString("restart")]->setText(i18n("Restart %1", info[QString("name")]));
            contextMenu[QString("enable")]->setVisible(true);
            if (info[QString("status")].contains(QString("enabled")))
                contextMenu[QString("enable")]->setText(i18n("Disable %1", info[QString("name")]));
            else
                contextMenu[QString("enable")]->setText(i18n("Enable %1", info[QString("name")]));
        }
        else {
            contextMenu[QString("start")]->setText(i18n("Start profile"));
            contextMenu[QString("stop")]->setVisible(false);
            contextMenu[QString("restart")]->setVisible(false);
            contextMenu[QString("enable")]->setVisible(false);
        }
        startProfileMenu->clear();
        for (int i=0; i<profileList.count(); i++) {
            QAction *profile = new QAction(profileList[i], this);
            startProfileMenu->addAction(profile);
        }
    }
    if (useWifi)
        contextMenu[QString("wifi")]->setVisible(true);
    else
        contextMenu[QString("wifi")]->setVisible(false);

    return menuActions;
}


void Netctl::createActions()
{
    menuActions.clear();

    contextMenu[QString("title")] = new QAction(QString("netctl-gui"), this);
    menuActions.append(contextMenu[QString("title")]);

    contextMenu[QString("start")] = new QAction(i18n("Start profile"), this);
    contextMenu[QString("start")]->setIcon(QIcon::fromTheme("dialog-apply"));
    startProfileMenu = new QMenu(NULL);
    contextMenu[QString("start")]->setMenu(startProfileMenu);
    connect(startProfileMenu, SIGNAL(triggered(QAction *)), this,
            SLOT(startProfileSlot(QAction *)));
    menuActions.append(contextMenu[QString("start")]);

    contextMenu[QString("stop")] = new QAction(i18n("Stop profile"), this);
    contextMenu[QString("stop")]->setIcon(QIcon::fromTheme("dialog-close"));
    connect(contextMenu[QString("stop")], SIGNAL(triggered(bool)), this, SLOT(stopProfileSlot()));
    menuActions.append(contextMenu[QString("stop")]);

    contextMenu[QString("switch")] = new QAction(i18n("Switch to profile"), this);
    contextMenu[QString("switch")]->setIcon(QIcon::fromTheme("dialog-apply"));
    switchToProfileMenu = new QMenu(NULL);
    contextMenu[QString("switch")]->setMenu(switchToProfileMenu);
    connect(switchToProfileMenu, SIGNAL(triggered(QAction *)), this,
            SLOT(switchToProfileSlot(QAction *)));
    menuActions.append(contextMenu[QString("switch")]);

    contextMenu[QString("restart")] = new QAction(i18n("Restart profile"), this);
    contextMenu[QString("restart")]->setIcon(QIcon::fromTheme("stock-refresh"));
    connect(contextMenu[QString("restart")], SIGNAL(triggered(bool)), this, SLOT(restartProfileSlot()));
    menuActions.append(contextMenu[QString("restart")]);

    contextMenu[QString("enable")] = new QAction(i18n("Enable profile"), this);
    connect(contextMenu[QString("enable")], SIGNAL(triggered(bool)), this, SLOT(enableProfileSlot()));
    menuActions.append(contextMenu[QString("enable")]);

    contextMenu[QString("wifi")] = new QAction(i18n("Show WiFi menu"), this);
    contextMenu[QString("wifi")]->setIcon(QIcon(":wifi"));
    connect(contextMenu[QString("wifi")], SIGNAL(triggered(bool)), this, SLOT(showWifi()));
    menuActions.append(contextMenu[QString("wifi")]);
}


// events
void Netctl::sendNotification(const QString eventId, const QString message)
{
    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_netctl"));
    notification->setTitle(QString("Netctl ::: ") + eventId);
    notification->setText(message);
    notification->sendEvent();
    delete notification;
}


void Netctl::showGui()
{
    sendNotification(QString("Info"), i18n("Start GUI"));
    QProcess command;
    command.startDetached(paths[QString("gui")]);
}


void Netctl::showWifi()
{
    sendNotification(QString("Info"), i18n("Start WiFi menu"));
    QProcess command;
    command.startDetached(paths[QString("wifi")]);
}


// data engine interaction
void Netctl::connectToEngine()
{
    netctlEngine->connectSource(QString("profiles"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusBool"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("currentProfile"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusString"), this, autoUpdateInterval);
    updateInterface(false);
    if (bigInterface[QString("main")]) {
        if (bigInterface[QString("extIp")])
            netctlEngine->connectSource(QString("extIp"), this, autoUpdateInterval);
        if (bigInterface[QString("intIp")])
            netctlEngine->connectSource(QString("intIp"), this, autoUpdateInterval);
        if (bigInterface[QString("netDev")])
            netctlEngine->connectSource(QString("interfaces"), this, autoUpdateInterval);
        updateInterface(true);
    }
}


void Netctl::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (data.keys().count() == 0)
        return;
    QString value = data[QString("value")].toString();
    if (value.isEmpty())
        value = QString("N\\A");

    if (sourceName == QString("currentProfile")) {
        info[QString("name")] = value;

        // update text
        QStringList text;
        text.append(info[QString("name")] + QString(" ") + info[QString("status")]);
        if (bigInterface[QString("intIp")])
            text.append(info[QString("intIp")]);
        if (bigInterface[QString("extIp")])
            text.append(info[QString("extIp")]);
        if (bigInterface[QString("netDev")])
            text.append(info[QString("interfaces")]);
        if (bigInterface[QString("main")])
            textLabel->setText(formatLine[0] + text.join(QString("<br>")) + formatLine[1]);
    }
    else if (sourceName == QString("extIp")) {
        info[QString("extIp")] = value;
    }
    else if (sourceName == QString("intIp")) {
        info[QString("intIp")] = value;
    }
    else if (sourceName == QString("interfaces")) {
        info[QString("interfaces")] = value;
    }
    else if (sourceName == QString("profiles")) {
        profileList = value.split(QString(","));
    }
    else if (sourceName == QString("statusBool")) {
        if (value == QString("true")) {
            if (! status)
                sendNotification(QString("Info"), i18n("Network is up"));
            status = true;
            iconWidget->setIcon(paths[QString("active")]);
        }
        else {
            if (status)
                sendNotification(QString("Info"), i18n("Network is down"));
            status = false;
            iconWidget->setIcon(paths[QString("inactive")]);
        }
    }
    else if (sourceName == QString("statusString")) {
        info[QString("status")] = QString("(") + value + QString(")");
    }

    update();
}


void Netctl::disconnectFromEngine()
{
    netctlEngine->disconnectSource(QString("profiles"), this);
    netctlEngine->disconnectSource(QString("statusBool"), this);
    netctlEngine->disconnectSource(QString("currentProfile"), this);
    netctlEngine->disconnectSource(QString("statusString"), this);
    if (bigInterface[QString("main")]) {
        if (bigInterface[QString("extIp")])
            netctlEngine->disconnectSource(QString("extIp"), this);
        if (bigInterface[QString("intIp")])
            netctlEngine->disconnectSource(QString("intIp"), this);
        if (bigInterface[QString("netDev")])
            netctlEngine->disconnectSource(QString("interfaces"), this);
    }
    updateInterface(false);
}


//  configuration interface
void Netctl::selectActiveIcon()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiAppConfig.lineEdit_activeIcon->setText(url.path());
}


void Netctl::selectGuiExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiWidConfig.lineEdit_gui->setText(url.path());
}


void Netctl::selectInactiveIcon()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiAppConfig.lineEdit_inactiveIcon->setText(url.path());
}


void Netctl::selectNetctlExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiWidConfig.lineEdit_netctl->setText(url.path());
}


void Netctl::selectNetctlAutoExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiWidConfig.lineEdit_netctlAuto->setText(url.path());
}


void Netctl::selectSudoExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiWidConfig.lineEdit_sudo->setText(url.path());
}


void Netctl::selectWifiExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiWidConfig.lineEdit_wifi->setText(url.path());
}


void Netctl::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);
    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QString text = QString(NAME) + " - " + QString(VERSION) + "\n" + "(c) " +
            QString(DATE) + " " + QString(AUTHOR);
    uiWidConfig.label_info->setText(text);

    uiWidConfig.spinBox_autoUpdate->setValue(autoUpdateInterval);
    uiWidConfig.lineEdit_gui->setText(paths[QString("gui")]);
    uiWidConfig.lineEdit_netctl->setText(paths[QString("netctl")]);
    uiWidConfig.lineEdit_netctlAuto->setText(paths[QString("netctlAuto")]);
    if (useSudo)
        uiWidConfig.checkBox_sudo->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_sudo->setCheckState(Qt::Unchecked);
    uiWidConfig.lineEdit_sudo->setText(paths[QString("sudo")]);
    if (useWifi)
        uiWidConfig.checkBox_wifi->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_wifi->setCheckState(Qt::Unchecked);
    uiWidConfig.lineEdit_wifi->setText(paths[QString("wifi")]);
    if (bigInterface[QString("main")])
        uiWidConfig.checkBox_showBigInterface->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_showBigInterface->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("netDev")])
        uiWidConfig.checkBox_showNetDev->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_showNetDev->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("extIp")])
        uiWidConfig.checkBox_showExtIp->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_showExtIp->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("intIp")])
        uiWidConfig.checkBox_showIntIp->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_showIntIp->setCheckState(Qt::Unchecked);
    setBigInterface();

    KConfigGroup cg = config();
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    QFont font = QFont(fontFamily, 12, 400, FALSE);
    uiAppConfig.fontComboBox_font->setCurrentFont(font);
    uiAppConfig.spinBox_fontSize->setValue(fontSize);
    uiAppConfig.kcolorcombo_fontColor->setColor(fontColor);
    uiAppConfig.spinBox_fontWeight->setValue(fontWeight);
    if (fontStyle == "normal")
        uiAppConfig.comboBox_fontStyle->setCurrentIndex(0);
    else if (fontStyle == "italic")
        uiAppConfig.comboBox_fontStyle->setCurrentIndex(1);
    uiAppConfig.lineEdit_activeIcon->setText(paths[QString("active")]);
    uiAppConfig.lineEdit_inactiveIcon->setText(paths[QString("inactive")]);

    parent->addPage(configWidget, i18n("Netctl plasmoid"), Applet::icon());
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));

    connect(uiWidConfig.checkBox_showBigInterface, SIGNAL(stateChanged(int)), this,
            SLOT(setBigInterface()));
    connect(uiWidConfig.checkBox_sudo, SIGNAL(stateChanged(int)), this, SLOT(setSudo()));
    connect(uiWidConfig.checkBox_wifi, SIGNAL(stateChanged(int)), this, SLOT(setWifi()));

    connect(uiWidConfig.pushButton_gui, SIGNAL(clicked()), this, SLOT(selectGuiExe()));
    connect(uiWidConfig.pushButton_netctl, SIGNAL(clicked()), this, SLOT(selectNetctlExe()));
    connect(uiWidConfig.pushButton_netctlAuto, SIGNAL(clicked()), this, SLOT(selectNetctlAutoExe()));
    connect(uiWidConfig.pushButton_sudo, SIGNAL(clicked()), this, SLOT(selectSudoExe()));
    connect(uiWidConfig.pushButton_wifi, SIGNAL(clicked()), this, SLOT(selecWifiExe()));
    connect(uiAppConfig.pushButton_activeIcon, SIGNAL(clicked()), this, SLOT(selectActiveIcon()));
    connect(uiAppConfig.pushButton_inactiveIcon, SIGNAL(clicked()), this, SLOT(selectInactiveIcon()));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void Netctl::configAccepted()
{
    disconnectFromEngine();
    KConfigGroup cg = config();

    cg.writeEntry("autoUpdateInterval", uiWidConfig.spinBox_autoUpdate->value());
    cg.writeEntry("guiPath", uiWidConfig.lineEdit_gui->text());
    cg.writeEntry("netctlPath", uiWidConfig.lineEdit_netctl->text());
    cg.writeEntry("netctlAutoPath", uiWidConfig.lineEdit_netctlAuto->text());
    if (uiWidConfig.checkBox_sudo->checkState() == 0)
        cg.writeEntry("useSudo", false);
    else
        cg.writeEntry("useSudo", true);
    cg.writeEntry("sudoPath", uiWidConfig.lineEdit_sudo->text());
    if (uiWidConfig.checkBox_wifi->checkState() == 0)
        cg.writeEntry("useWifi", false);
    else
        cg.writeEntry("useWifi", true);
    cg.writeEntry("wifiPath", uiWidConfig.lineEdit_wifi->text());
    if (uiWidConfig.checkBox_showBigInterface->checkState() == 0)
        cg.writeEntry("showBigInterface", false);
    else
        cg.writeEntry("showBigInterface", true);
    if (uiWidConfig.checkBox_showNetDev->checkState() == 0)
        cg.writeEntry("showNetDev", false);
    else
        cg.writeEntry("showNetDev", true);
    if (uiWidConfig.checkBox_showExtIp->checkState() == 0)
        cg.writeEntry("showExtIp", false);
    else
        cg.writeEntry("showExtIp", true);
    if (uiWidConfig.checkBox_showIntIp->checkState() == 0)
        cg.writeEntry("showIntIp", false);
    else
        cg.writeEntry("showIntIp", true);

    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox_font->currentFont().family());
    cg.writeEntry("fontSize", uiAppConfig.spinBox_fontSize->value());
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo_fontColor->color().name());
    cg.writeEntry("fontWeight", uiAppConfig.spinBox_fontWeight->value());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_fontStyle->currentText());
    cg.writeEntry("activeIconPath", uiAppConfig.lineEdit_activeIcon->text());
    cg.writeEntry("inactiveIconPath", uiAppConfig.lineEdit_inactiveIcon->text());
}


void Netctl::configChanged()
{
    KConfigGroup cg = config();

    autoUpdateInterval = cg.readEntry("autoUpdateInterval", 1000);
    paths[QString("gui")] = cg.readEntry("guiPath", "/usr/bin/netctl-gui");
    paths[QString("netctl")] = cg.readEntry("netctlPath", "/usr/bin/netctl");
    paths[QString("netctlAuto")] = cg.readEntry("netctlAutoPath", "/usr/bin/netctl-auto");
    paths[QString("sudo")] = cg.readEntry("sudoPath", "/usr/bin/kdesu");
    paths[QString("wifi")] = cg.readEntry("wifiPath", "/usr/bin/netctl-gui -t 3");
    useSudo = cg.readEntry("useSudo", true);
    useWifi = cg.readEntry("useWifi", true);
    bigInterface[QString("main")] = cg.readEntry("showBigInterface", true);
    bigInterface[QString("extIp")] = cg.readEntry("showExtIp", false);
    bigInterface[QString("netDev")] = cg.readEntry("showNetDev", true);
    bigInterface[QString("intIp")] = cg.readEntry("showIntIp", true);

    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    paths[QString("active")] = cg.readEntry("activeIconPath",
                                            "/usr/share/icons/hicolor/64x64/apps/netctl-idle.png");
    paths[QString("inactive")] = cg.readEntry("inactiveIconPath",
                                              "/usr/share/icons/hicolor/64x64/apps/netctl-offline.png");

    formatLine[0] = ("<p align=\"center\"><span style=\" font-family:'" + fontFamily + \
                     "'; font-style:" + fontStyle + \
                     "; font-size:" + QString::number(fontSize) + \
                     "pt; font-weight:" + QString::number(fontWeight) + \
                     "; color:" + fontColor + \
                     ";\">");
    formatLine[1] = ("</span></p>");
    connectToEngine();
}


void Netctl::setBigInterface()
{
    if (uiWidConfig.checkBox_showBigInterface->checkState() == 0) {
        uiWidConfig.checkBox_showNetDev->setDisabled(true);
        uiWidConfig.checkBox_showExtIp->setDisabled(true);
        uiWidConfig.checkBox_showIntIp->setDisabled(true);
    }
    else if (uiWidConfig.checkBox_showBigInterface->checkState() == 2) {
        uiWidConfig.checkBox_showNetDev->setEnabled(true);
        uiWidConfig.checkBox_showExtIp->setEnabled(true);
        uiWidConfig.checkBox_showIntIp->setEnabled(true);
    }
}


void Netctl::setSudo()
{
    if (uiWidConfig.checkBox_sudo->checkState() == 0)
        uiWidConfig.lineEdit_sudo->setDisabled(true);
    else if (uiWidConfig.checkBox_sudo->checkState() == 2)
        uiWidConfig.lineEdit_sudo->setEnabled(true);
}


void Netctl::setWifi()
{
    if (uiWidConfig.checkBox_wifi->checkState() == 0)
        uiWidConfig.lineEdit_wifi->setDisabled(true);
    else if (uiWidConfig.checkBox_wifi->checkState() == 2)
        uiWidConfig.checkBox_wifi->setEnabled(true);
}


#include "netctl.moc"
