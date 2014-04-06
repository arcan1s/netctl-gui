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
#include "ui_configwindow.h"

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
//    delete startProfile;
//    delete stopProfile;
//    delete restartProfile;
//    delete enableProfileAutoload;

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
void Netctl::enableProfileAutoloadSlot()
{
    QProcess command;
    QString commandLine, enableStatus;
    if (info[QString("status")].contains(QString("enabled")))
        enableStatus = QString(" disable ");
    else
        enableStatus = QString(" enable ");
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" \"") + paths[QString("netctl")] +
                enableStatus + info[QString("name")] + QString("\"");
    else
        commandLine = paths[QString("netctl")] + enableStatus + info[QString("name")];
    command.startDetached(commandLine);
}


void Netctl::startProfileSlot(QAction *profile)
{
    QProcess command;
    QString commandLine;
    commandLine = QString("");
    if (status)
        commandLine = paths[QString("netctl")] + QString(" stop ") +
                info[QString("name")] +  QString(" && ");
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" \"") + commandLine +
                paths[QString("netctl")] + QString(" start ") +
                profile->text().remove(QString("&")) + QString("\"");
    else
        commandLine = commandLine + paths[QString("netctl")] + QString(" start ") +
                profile->text().remove(QString("&"));
    command.startDetached(commandLine);
}


void Netctl::stopProfileSlot()
{
    QProcess command;
    QString commandLine;
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" \"") + paths[QString("netctl")] +
                QString(" stop ") + info[QString("name")] + QString("\"");
    else
        commandLine = paths[QString("netctl")] + QString(" stop ") + info[QString("name")];
    command.startDetached(commandLine);
}


void Netctl::restartProfileSlot()
{
    QProcess command;
    QString commandLine;
    if (useSudo)
        commandLine = paths[QString("sudo")] + QString(" \"") + paths[QString("netctl")] +
                QString(" restart ") + info[QString("name")] + QString("\"");
    else
        commandLine = paths[QString("netctl")] + QString(" restart ") + info[QString("name")];
    command.startDetached(commandLine);
}


QList<QAction*> Netctl::contextualActions()
{
    if (status) {
        startProfile->setText(i18n("Start another profile"));
        stopProfile->setVisible(true);
        stopProfile->setText(i18n("Stop ") + info[QString("name")]);
        restartProfile->setVisible(true);
        restartProfile->setText(i18n("Restart ") + info[QString("name")]);
        enableProfileAutoload->setVisible(true);
        if (info[QString("status")].contains(QString("enabled")))
            enableProfileAutoload->setText(i18n("Disable ") + info[QString("name")]);
        else
            enableProfileAutoload->setText(i18n("Enable ") + info[QString("name")]);
    }
    else {
        startProfile->setText(i18n("Start profile"));
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


void Netctl::createActions()
{
    menuActions.clear();

    startProfile = new QAction(i18n("Start profile"), this);
    startProfileMenu = new QMenu(NULL);
    startProfile->setMenu(startProfileMenu);
    connect(startProfileMenu, SIGNAL(triggered(QAction *)), this,
            SLOT(startProfileSlot(QAction *)));
    menuActions.append(startProfile);

    stopProfile = new QAction(i18n("Stop profile"), this);
    connect(stopProfile, SIGNAL(triggered(bool)), this, SLOT(stopProfileSlot()));
    menuActions.append(stopProfile);

    restartProfile = new QAction(i18n("Restart profile"), this);
    connect(restartProfile, SIGNAL(triggered(bool)), this, SLOT(restartProfileSlot()));
    menuActions.append(restartProfile);

    enableProfileAutoload = new QAction(i18n("Enable profile"), this);
    connect(enableProfileAutoload, SIGNAL(triggered(bool)), this,
            SLOT(enableProfileAutoloadSlot()));
    menuActions.append(enableProfileAutoload);
}


// events
void Netctl::sendNotification(const QString eventId, const QString message)
{
    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_netctl"));
    notification->setTitle(eventId);
    notification->setText(message);
    notification->sendEvent();
    delete notification;
}


void Netctl::showGui()
{
    QProcess command;
    command.startDetached(paths[QString("gui")]);
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
            status = true;
            iconWidget->setIcon(paths[QString("active")]);
        }
        else {
            iconWidget->setIcon(paths[QString("inactive")]);
            status = false;
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
        uiConfig.lineEdit_activeIcon->setText(url.path());
}


void Netctl::selectGuiExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiConfig.lineEdit_gui->setText(url.path());
}


void Netctl::selectInactiveIcon()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiConfig.lineEdit_inactiveIcon->setText(url.path());
}


void Netctl::selectNetctlExe()
{
    KUrl url = KFileDialog::getOpenUrl(KUrl(), "*");
    if (!url.isEmpty())
        uiConfig.lineEdit_netctl->setText(url.path());
}


void Netctl::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *configwin = new QWidget;
    uiConfig.setupUi(configwin);
    QString text = QString(NAME) + " - " + QString(VERSION) + "\n" + "(c) " +
            QString(DATE) + " " + QString(AUTHOR);
    uiConfig.label_info->setText(text);

    uiConfig.spinBox_autoUpdate->setValue(autoUpdateInterval);
    uiConfig.lineEdit_gui->setText(paths[QString("gui")]);
    uiConfig.lineEdit_netctl->setText(paths[QString("netctl")]);
    if (useSudo)
        uiConfig.checkBox_sudo->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_sudo->setCheckState(Qt::Unchecked);
    uiConfig.lineEdit_sudo->setText(paths[QString("sudo")]);
    if (bigInterface[QString("main")])
        uiConfig.checkBox_showBigInterface->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showBigInterface->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("netDev")])
        uiConfig.checkBox_showNetDev->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showNetDev->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("extIp")])
        uiConfig.checkBox_showExtIp->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showExtIp->setCheckState(Qt::Unchecked);
    if (bigInterface[QString("intIp")])
        uiConfig.checkBox_showIntIp->setCheckState(Qt::Checked);
    else
        uiConfig.checkBox_showIntIp->setCheckState(Qt::Unchecked);
    setBigInterface();

    KConfigGroup cg = config();
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    QFont font = QFont(fontFamily, 12, 400, FALSE);
    uiConfig.fontComboBox_font->setCurrentFont(font);
    uiConfig.spinBox_fontSize->setValue(fontSize);
    uiConfig.kcolorcombo_fontColor->setColor(fontColor);
    uiConfig.spinBox_fontWeight->setValue(fontWeight);
    if (fontStyle == "normal")
        uiConfig.comboBox_fontStyle->setCurrentIndex(0);
    else if (fontStyle == "italic")
        uiConfig.comboBox_fontStyle->setCurrentIndex(1);
    uiConfig.lineEdit_activeIcon->setText(paths[QString("active")]);
    uiConfig.lineEdit_inactiveIcon->setText(paths[QString("inactive")]);

    parent->addPage(configwin, i18n("Netctl plasmoid"), Applet::icon());

    connect(uiConfig.checkBox_showBigInterface, SIGNAL(stateChanged(int)), this,
            SLOT(setBigInterface()));
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
    paths[QString("gui")] = cg.readEntry("guiPath", "/usr/bin/netctl-gui");
    paths[QString("netctl")] = cg.readEntry("netctlPath", "/usr/bin/netctl");
    useSudo = cg.readEntry("useSudo", true);
    paths[QString("sudo")] = cg.readEntry("sudoPath", "/usr/bin/kdesu -c");
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
