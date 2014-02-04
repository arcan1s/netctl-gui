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

#include "netctl.h"
#include "ui_configwindow.h"
#include <version.h>

#include <KConfigDialog>
#include <KFileDialog>
#include <KNotification>
#include <KUrl>
#include <plasma/theme.h>

#include <QGraphicsLinearLayout>
#include <QMenu>
#include <cstdio>


Netctl::Netctl(QObject *parent, const QVariantList &args) :
    Plasma::Applet(parent, args)
{
    setBackgroundHints(DefaultBackground);
    setHasConfigurationInterface(true);
    // text format init
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


Netctl::~Netctl()
{
    delete startProfileMenu;
    delete startProfile;
    delete stopProfile;
    delete restartProfile;
    delete enableProfileAutoload;

    delete iconWidget;
    delete iconFrame;
    delete textFrame;

    delete netctlEngine;
}


void Netctl::init()
{
    netctlEngine = dataEngine(QString("netctl"));
    createActions();
    // generate ui
    // main layout
    fullSpaceLayout = new QGraphicsLinearLayout();
    setLayout(fullSpaceLayout);

    // frames
    // icon
    iconFrame = new Plasma::Frame();
    QGraphicsLinearLayout *iconLayout = new QGraphicsLinearLayout();
    iconWidget = new Plasma::IconWidget(KIcon(""), QString(), this);
    connect(iconWidget, SIGNAL(doubleClicked()), this, SLOT(showGui()));
    iconFrame->setLayout(iconLayout);
    iconLayout->addItem(iconWidget);
    fullSpaceLayout->addItem(iconFrame);
    // text
    textFrame = new Plasma::Frame();
    QGraphicsLinearLayout *textLayout = new QGraphicsLinearLayout();
    textLabel = new Plasma::Label();
    textLayout->addItem(textLabel);
    textFrame->setLayout(textLayout);
    fullSpaceLayout->addItem(textFrame);
    textFrame->hide();
    // stretch
    fullSpaceLayout->addStretch(1);

    // read variables
    configChanged();
}


// context menu
void Netctl::startProfileSlot(QAction *profile)
{
    QProcess command;
    QString commandLine;
    commandLine = QString("");
    if (status)
        commandLine = netctlPath + QString(" stop ") + profileName +  QString(" && ");
    if (useSudo)
        commandLine = sudoPath + QString(" \"") + commandLine + netctlPath + QString(" start ") + profile->text().mid(1) + QString("\"");
    else
        commandLine = commandLine + netctlPath + QString(" start ") + profile->text().mid(1);
    command.startDetached(commandLine);
}


void Netctl::stopProfileSlot()
{
    QProcess command;
    QString commandLine;
    if (useSudo)
        commandLine = sudoPath + QString(" \"") + netctlPath + QString(" stop ") + profileName + QString("\"");
    else
        commandLine = netctlPath + QString(" stop ") + profileName;
    command.startDetached(commandLine);
}


void Netctl::restartProfileSlot()
{
    QProcess command;
    QString commandLine;
    if (useSudo)
        commandLine = sudoPath + QString(" \"") + netctlPath + QString(" restart ") + profileName + QString("\"");
    else
        commandLine = netctlPath + QString(" restart ") + profileName;
    command.startDetached(commandLine);
}


void Netctl::enableProfileAutoloadSlot()
{
    QProcess command;
    QString commandLine, enableStatus;
    if (profileStatus == QString("(enable"))
        enableStatus = QString("disable");
    else
        enableStatus = QString("enable");
    if (useSudo)
        commandLine = sudoPath + QString(" \"") + netctlPath + enableStatus + profileName + QString("\"");
    else
        commandLine = netctlPath + enableStatus + profileName;
    command.startDetached(commandLine);
}


void Netctl::createActions()
{
    menuActions.clear();

    startProfile = new QAction(QString("Start profile"), this);
    startProfileMenu = new QMenu(NULL);
    startProfile->setMenu(startProfileMenu);
    connect(startProfileMenu, SIGNAL(triggered(QAction *)), this, SLOT(startProfileSlot(QAction *)));
    menuActions.append(startProfile);

    stopProfile = new QAction(QString("Stop profile"), this);
    connect(stopProfile, SIGNAL(triggered(bool)), this, SLOT(stopProfileSlot()));
    menuActions.append(stopProfile);

    restartProfile = new QAction(QString("Restart profile"), this);
    connect(restartProfile, SIGNAL(triggered(bool)), this, SLOT(restartProfileSlot()));
    menuActions.append(restartProfile);

    enableProfileAutoload = new QAction(QString("Enable profile"), this);
    connect(enableProfileAutoload, SIGNAL(triggered(bool)), this, SLOT(enableProfileAutoloadSlot()));
    menuActions.append(enableProfileAutoload);
}


QList<QAction*> Netctl::contextualActions()
{
    if (status) {
        startProfile->setText(QString("Start another profile"));
        stopProfile->setVisible(true);
        stopProfile->setText(QString("Stop ") + profileName);
        restartProfile->setVisible(true);
        restartProfile->setText(QString("Restart ") + profileName);
        enableProfileAutoload->setVisible(true);
        if (profileStatus == QString("(enable"))
            enableProfileAutoload->setText(QString("Disable ") + profileName);
        else
            enableProfileAutoload->setText(QString("Enable ") + profileName);
    }
    else {
        startProfile->setText(QString("Start profile"));
        stopProfile->setVisible(false);
        restartProfile->setVisible(false);
        enableProfileAutoload->setVisible(false);
    }

    startProfileMenu->clear();
    for (int i=0; i<profileList.count(); i++) {
        QAction *profile = new QAction(profileList[i], this);
        startProfileMenu->addAction(profile);
    }

    return menuActions;
}


// events
void Netctl::showGui()
{
    QProcess command;
    command.startDetached(guiPath);
}


void Netctl::sendNotification(const QString eventId, const QString message)
{
    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_netctl"));
    notification->setTitle(eventId);
    notification->setText(message);
    notification->sendEvent();
    delete notification;
}


// data engine interaction
void Netctl::connectToEngine()
{
    netctlEngine->connectSource(QString("profiles"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusBool"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("currentProfile"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusString"), this, autoUpdateInterval);
    if (showBigInterface) {
        if (showExtIp)
            netctlEngine->connectSource(QString("extIp"), this, autoUpdateInterval);
        if (showIntIp)
            netctlEngine->connectSource(QString("intIp"), this, autoUpdateInterval);
        if (showNetDev)
            netctlEngine->connectSource(QString("interfaces"), this, autoUpdateInterval);
        textFrame->show();
    }
}


void Netctl::disconnectFromEngine()
{
    netctlEngine->disconnectSource(QString("profiles"), this);
    netctlEngine->disconnectSource(QString("statusBool"), this);
    netctlEngine->disconnectSource(QString("currentProfile"), this);
    netctlEngine->disconnectSource(QString("statusString"), this);
    if (showBigInterface) {
        if (showExtIp)
            netctlEngine->disconnectSource(QString("extIp"), this);
        if (showIntIp)
            netctlEngine->disconnectSource(QString("intIp"), this);
        if (showNetDev)
            netctlEngine->disconnectSource(QString("interfaces"), this);
        textFrame->hide();
    }
    update();
}


void Netctl::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (data.keys().count() == 0)
        return;
    QString value = data[QString("value")].toString();;
    if (value.isEmpty())
        value = QString("N\\A");

    if (sourceName == QString("currentProfile")) {
        profileName = value;

        // update text
        QStringList text;
        text.append(profileName + QString(" ") + profileStatus);
        if (showIntIp)
            text.append(intIp);
        if (showExtIp)
            text.append(extIp);
        if (showNetDev)
            text.append(interfaces);
        textLabel->setText(formatLine[0] + text.join(QString("<br>")) + formatLine[1]);
    }
    else if (sourceName == QString("extIp")) {
        extIp = value;
    }
    else if (sourceName == QString("intIp")) {
        intIp = value;
    }
    else if (sourceName == QString("interfaces")) {
        interfaces = value;
    }
    else if (sourceName == QString("profiles")) {
        profileList = value.split(QString(","));
    }
    else if (sourceName == QString("statusBool")) {
        if (value == QString("true")) {
            status = true;
            iconWidget->setIcon(activeIconPath);
        }
        else {
            iconWidget->setIcon(inactiveIconPath);
            status = false;
        }
    }
    else if (sourceName == QString("statusString")) {
        profileStatus = QString("(") + value + QString(")");
    }

    update();
}


//  configuration interface
void Netctl::selectGuiExe()
{
    KUrl guiUrl = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!guiUrl.isEmpty())
        uiConfig.lineEdit_gui->setText(guiUrl.path());
}


void Netctl::selectNetctlExe()
{
    KUrl netctlUrl = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!netctlUrl.isEmpty())
        uiConfig.lineEdit_netctl->setText(netctlUrl.path());
}


void Netctl::selectActiveIcon()
{
    KUrl activeIconUrl = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!activeIconUrl.isEmpty())
        uiConfig.lineEdit_activeIcon->setText(activeIconUrl.path());
}


void Netctl::selectInactiveIcon()
{
    KUrl inactiveIconUrl = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!inactiveIconUrl.isEmpty())
        uiConfig.lineEdit_inactiveIcon->setText(inactiveIconUrl.path());
}


void Netctl::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *configwin = new QWidget;
    uiConfig.setupUi(configwin);
    QString text = QString(NAME) + " - " + QString(VERSION) + "\n" + "(c) " + QString(DATE) + " " + QString(AUTHOR);
    uiConfig.label_info->setText(text);

    uiConfig.spinBox_autoUpdate->setValue(autoUpdateInterval);
    uiConfig.lineEdit_gui->setText(guiPath);
    uiConfig.lineEdit_netctl->setText(netctlPath);
    if (useSudo)
        uiConfig.checkBox_sudo->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_sudo->setCheckState(Qt::Unchecked);
    uiConfig.lineEdit_sudo->setText(sudoPath);
    if (showBigInterface)
        uiConfig.checkBox_showBigInterface->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showBigInterface->setCheckState(Qt::Unchecked);
    if (showNetDev)
        uiConfig.checkBox_showNetDev->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showNetDev->setCheckState(Qt::Unchecked);
    if (showExtIp)
        uiConfig.checkBox_showExtIp->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showExtIp->setCheckState(Qt::Unchecked);
    if (showIntIp)
        uiConfig.checkBox_showIntIp->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showIntIp->setCheckState(Qt::Unchecked);
    setBigInterface();

    QFont font = QFont(fontFamily, 12, 400, FALSE);
    uiConfig.fontComboBox_font->setCurrentFont(font);
    uiConfig.spinBox_fontSize->setValue(fontSize);
    uiConfig.kcolorcombo_fontColor->setColor(fontColor);
    uiConfig.spinBox_fontWeight->setValue(fontWeight);
    if (fontStyle == "normal")
        uiConfig.comboBox_fontStyle->setCurrentIndex(0);
    else if (fontStyle == "italic")
        uiConfig.comboBox_fontStyle->setCurrentIndex(1);
    uiConfig.lineEdit_activeIcon->setText(activeIconPath);
    uiConfig.lineEdit_inactiveIcon->setText(inactiveIconPath);

    parent->addPage(configwin, i18n("Netctl plasmoid"), Applet::icon());

    connect(uiConfig.checkBox_showBigInterface, SIGNAL(stateChanged(int)), this, SLOT(setBigInterface()));
    connect(uiConfig.checkBox_sudo, SIGNAL(stateChanged(int)), this, SLOT(setSudo()));

    connect(uiConfig.pushButton_gui, SIGNAL(clicked()), this, SLOT(selectGuiExe()));
    connect(uiConfig.pushButton_netctl, SIGNAL(clicked()), this, SLOT(selectNetctlExe()));
    connect(uiConfig.pushButton_activeIcon, SIGNAL(clicked()), this, SLOT(selectActiveIcon()));
    connect(uiConfig.pushButton_inactiveIcon, SIGNAL(clicked()), this, SLOT(selectInactiveIcon()));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void Netctl::configAccepted()
{
    disconnectFromEngine();
    KConfigGroup cg = config();

    cg.writeEntry("autoUpdateInterval", uiConfig.spinBox_autoUpdate->value());
    cg.writeEntry("guiPath", uiConfig.lineEdit_gui->text());
    cg.writeEntry("netctlPath", uiConfig.lineEdit_netctl->text());
    if (uiConfig.checkBox_sudo->checkState() == 0)
        cg.writeEntry("useSudo", false);
    else
        cg.writeEntry("useSudo", true);
    cg.writeEntry("sudoPath", uiConfig.lineEdit_sudo->text());
    if (uiConfig.checkBox_showBigInterface->checkState() == 0)
        cg.writeEntry("showBigInterface", false);
    else
        cg.writeEntry("showBigInterface", true);
    if (uiConfig.checkBox_showNetDev->checkState() == 0)
        cg.writeEntry("showNetDev", false);
    else
        cg.writeEntry("showNetDev", true);
    if (uiConfig.checkBox_showExtIp->checkState() == 0)
        cg.writeEntry("showExtIp", false);
    else
        cg.writeEntry("showExtIp", true);
    if (uiConfig.checkBox_showIntIp->checkState() == 0)
        cg.writeEntry("showIntIp", false);
    else
        cg.writeEntry("showIntIp", true);

    cg.writeEntry("fontFamily", uiConfig.fontComboBox_font->currentFont().family());
    cg.writeEntry("fontSize", uiConfig.spinBox_fontSize->value());
    cg.writeEntry("fontColor", uiConfig.kcolorcombo_fontColor->color().name());
    cg.writeEntry("fontWeight", uiConfig.spinBox_fontWeight->value());
    cg.writeEntry("fontStyle", uiConfig.comboBox_fontStyle->currentText());
    cg.writeEntry("activeIconPath", uiConfig.lineEdit_activeIcon->text());
    cg.writeEntry("inactiveIconPath", uiConfig.lineEdit_inactiveIcon->text());
}


void Netctl::configChanged()
{
    KConfigGroup cg = config();

    autoUpdateInterval = cg.readEntry("autoUpdateInterval", 1000);
    guiPath = cg.readEntry("guiPath", "/usr/bin/netctl-gui");
    netctlPath = cg.readEntry("netctlPath", "/usr/bin/netctl");
    useSudo = cg.readEntry("useSudo", true);
    sudoPath = cg.readEntry("sudoPath", "/usr/bin/kdesu -c");
    showBigInterface = cg.readEntry("showBigInterface", true);
    showNetDev = cg.readEntry("showNetDev", true);
    showExtIp = cg.readEntry("showExtIp", true);
    showIntIp = cg.readEntry("showIntIp", true);

    fontFamily = cg.readEntry("fontFamily", "Terminus");
    fontSize = cg.readEntry("fontSize", 10);
    fontColor = cg.readEntry("fontColor", "#000000");
    fontWeight = cg.readEntry("fontWeight", 400);
    fontStyle = cg.readEntry("fontStyle", "normal");
    activeIconPath = cg.readEntry("activeIconPath", "/usr/share/icons/hicolor/64x64/apps/network-idle.png");
    inactiveIconPath = cg.readEntry("inactiveIconPath", "/usr/share/icons/hicolor/64x64/apps/network-offline.png");

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
    if (uiConfig.checkBox_showBigInterface->checkState() == 0) {
        uiConfig.checkBox_showNetDev->setDisabled(true);
        uiConfig.checkBox_showExtIp->setDisabled(true);
        uiConfig.checkBox_showIntIp->setDisabled(true);
    }
    else if (uiConfig.checkBox_showBigInterface->checkState() == 2) {
        uiConfig.checkBox_showNetDev->setEnabled(true);
        uiConfig.checkBox_showExtIp->setEnabled(true);
        uiConfig.checkBox_showIntIp->setEnabled(true);
    }
}


void Netctl::setSudo()
{
    if (uiConfig.checkBox_sudo->checkState() == 0)
        uiConfig.lineEdit_sudo->setDisabled(true);
    else if (uiConfig.checkBox_showBigInterface->checkState() == 2)
        uiConfig.lineEdit_sudo->setEnabled(true);
}


#include "netctl.moc"
