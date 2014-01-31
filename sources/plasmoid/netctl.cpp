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

#include <cstdlib>
#include <ctime>
#include <KConfigDialog>
#include <KNotification>
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneMouseEvent>
#include <Plasma/DataEngine>
#include <Plasma/Frame>
#include <plasma/theme.h>
#include <stdio.h>


Netctl::Netctl(QObject *parent, const QVariantList &args) :
    Plasma::Applet(parent, args)
{
    setBackgroundHints(DefaultBackground);
    setHasConfigurationInterface(true);
    // text format init
    formatLine.append("");
    formatLine.append("");
}


Netctl::~Netctl()
{
    delete iconWidget;
    delete textLabel;
}


void Netctl::init()
{
    // generate ui
    // main layout
    QGraphicsLinearLayout *fullSpaceLayout = new QGraphicsLinearLayout();
    fullSpaceLayout->setContentsMargins(0,0,0,0);
    setLayout(fullSpaceLayout);

    // frame
    Plasma::Frame *frame = new Plasma::Frame();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout();
    layout->setContentsMargins(0,0,0,0);
    frame->setLayout(layout);
    fullSpaceLayout->addItem(frame);

    // icon
    iconWidget = new Plasma::IconWidget(KIcon("/home/arcanis/Documents/github/netctlplasmoid/sources/icons/network-idle-64x64.png"), QString(), this);
    iconWidget->setIcon(KIcon("/home/arcanis/Documents/github/netctlplasmoid/sources/icons/network-idle-64x64.png"));
    //    connect(shutdownIcon.iconWidget, SIGNAL(clicked()), this, SLOT(onShutdown()));
    layout->addItem(iconWidget);

    // label
    textLabel = new Plasma::Label(this);
    textLabel->setText("test1\ntest2\ntest3");
    fullSpaceLayout->addItem(textLabel);

    // read variables
    configChanged();
    this->resize(100,48);

    // connect to dataengine
    connectToEngine();
    printf ("done init\n");
}


void Netctl::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // mouse double click event
    // add gui support
    return;
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
    printf ("source\n");
    Plasma::DataEngine *netctlEngine = dataEngine(QString("netctl"));
    netctlEngine->connectSource(QString("currentProfile"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("extIp"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("interfaces"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("intIp"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("profiles"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusBool"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("statusString"), this, autoUpdateInterval);
}


void Netctl::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (data.keys().count() == 0)
        return;
    QString value;
    printf ("dataupdate\n");

    if (sourceName == QString("currentProfile"))
    {
        value = data[QString("value")].toString();
        if (value == QString(""))
            value = QString("N\\A");
        textLabel->setText(value);
    }
    update();
}


//  configuration interface
void Netctl::createConfigurationInterface(KConfigDialog *parent)
{
    QWidget *configwin = new QWidget;
    uiConfig.setupUi(configwin);

    uiConfig.spinBox_autoUpdate->setValue(autoUpdateInterval);
    uiConfig.lineEdit_gui->setText(guiPath);
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
    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(uiConfig.checkBox_showBigInterface, SIGNAL(stateChanged(int)), this, SLOT(setBigInterface()));
}


void Netctl::configAccepted()
{
    KConfigGroup cg = config();

    cg.writeEntry("autoUpdateInterval", uiConfig.spinBox_autoUpdate->value());
    cg.writeEntry("guiPath", uiConfig.lineEdit_gui->text());
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

    formatLine[0] = ("<p align=\"justify\"><span style=\" font-family:'" + fontFamily + \
                     "'; font-style:" + fontStyle + \
                     "; font-size:" + QString::number(fontSize) + \
                     "pt; font-weight:" + QString::number(fontWeight) + \
                     "; color:" + fontColor + \
                     ";\">");
    formatLine[1] = ("</span></p>");
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
