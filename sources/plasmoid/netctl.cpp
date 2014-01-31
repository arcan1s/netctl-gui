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

#include "netctl.h"
#include "ui_configwindow.h"
#include <version.h>

#include <KConfigDialog>
#include <KFileDialog>
#include <KNotification>
#include <KUrl>
#include <Plasma/DataEngine>
#include <plasma/theme.h>

#include <QGraphicsLinearLayout>
#include <QProcess>

// temporary
#include <stdio.h>


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
    delete iconWidget;
    delete iconFrame;
    delete textFrame;
}


void Netctl::init()
{
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
    resize(150,64);
}


void Netctl::createActions()
{
    //    helpAction = new QAction( KIcon( "help-about" ), i18n( "Help" ), this );
    //    helpAction->setMenu( ( QMenu* ) (new KHelpMenu( NULL, about, false ) )->menu() );
}


QList<QAction*> Netctl::contextualActions()
{
    QList<QAction*> menuActions;

    return menuActions;
}


int Netctl::showGui()
{
    QProcess command;
    command.startDetached(guiPath);
    return 0;
}


int Netctl::sendNotification(QString eventId, int num)
{
    // send notification
    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_netctl"));
    notification->setTitle(QString(i18n("Netctl plasmoid")));
    notification->setText("test");
    notification->sendEvent();
    delete notification;
    return 0;
}


// data engine interaction
void Netctl::connectToEngine()
{
    Plasma::DataEngine *netctlEngine = dataEngine(QString("netctl"));
    netctlEngine->connectSource(QString("profiles"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusBool"), this, autoUpdateInterval);
    if (showBigInterface) {
        netctlEngine->connectSource(QString("currentProfile"), this, autoUpdateInterval);
        netctlEngine->connectSource(QString("statusString"), this, autoUpdateInterval);
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
    Plasma::DataEngine *netctlEngine = dataEngine(QString("netctl"));
    netctlEngine->disconnectSource(QString("profiles"), this);
    netctlEngine->disconnectSource(QString("statusBool"), this);
    if (showBigInterface) {
        netctlEngine->disconnectSource(QString("currentProfile"), this);
        netctlEngine->disconnectSource(QString("statusString"), this);
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

    if (sourceName == QString("currentProfile")) {
        if (value.isEmpty())
            value = QString("N\\A");
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
        if (value.isEmpty())
            value = QString("N\\A");
        extIp = value;
    }
    else if (sourceName == QString("intIp")) {
        if (value.isEmpty())
            value = QString("N\\A");
        intIp = value;
    }
    else if (sourceName == QString("interfaces")) {
        if (value.isEmpty())
            value = QString("N\\A");
        interfaces = value;
    }
    else if (sourceName == QString("profiles")) {
        return;
    }
    else if (sourceName == QString("statusBool")) {
        if (value == QString("true"))
            iconWidget->setIcon(activeIconPath);
        else
            iconWidget->setIcon(inactiveIconPath);
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
    showBigInterface = cg.readEntry("showBigInterface", true);
    showNetDev = cg.readEntry("showNetDev", true);
    showExtIp = cg.readEntry("showExtIp", true);
    showIntIp = cg.readEntry("showIntIp", true);

    fontFamily = cg.readEntry("fontFamily", "Terminus");
    fontSize = cg.readEntry("fontSize", 12);
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


int Netctl::setBigInterface()
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
    else
        return 1;
    return 0;
}


#include "netctl.moc"
