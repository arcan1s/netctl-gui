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

#include "newprofilewidget.h"
#include "ui_newprofilewidget.h"

#include <QDebug>
#include <QLineEdit>
#include <QMenu>

#include <pdebug/pdebug.h>

#include "calls.h"
#include "bridgewidget.h"
#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "ethernetwidget.h"
#include "generalwidget.h"
#include "ipwidget.h"
#include "mainwindow.h"
#include "macvlanwidget.h"
#include "mobilewidget.h"
#include "pppoewidget.h"
#include "tunnelwidget.h"
#include "tuntapwidget.h"
#include "vlanwidget.h"
#include "wirelesswidget.h"


NewProfileWidget::NewProfileWidget(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QMainWindow(parent),
      debug(debugCmd),
      configuration(settings)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (configuration[QString("USE_HELPER")] == QString("true"));

    // windows
    ui = new Ui::NewProfileWidget;
    ui->setupUi(this);
    updateToolBarState(static_cast<Qt::ToolBarArea>(configuration[QString("PROFILE_TOOLBAR")].toInt()));
    // profile widgets
    generalWid = new GeneralWidget(this, configuration);
    ui->scrollAreaWidgetContents->layout()->addWidget(generalWid);
    ipWid = new IpWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ipWid);
    bridgeWid = new BridgeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(bridgeWid);
    ethernetWid = new EthernetWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(ethernetWid);
    macvlanWid = new MacvlanWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(macvlanWid);
    mobileWid = new MobileWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(mobileWid);
    pppoeWid = new PppoeWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(pppoeWid);
    tunnelWid = new TunnelWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(tunnelWid);
    tuntapWid = new TuntapWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(tuntapWid);
    vlanWid = new VlanWidget(this);
    ui->scrollAreaWidgetContents->layout()->addWidget(vlanWid);
    wirelessWid = new WirelessWidget(this, configuration);
    ui->scrollAreaWidgetContents->layout()->addWidget(wirelessWid);

    createActions();
}


NewProfileWidget::~NewProfileWidget()
{
    if (debug) qDebug() << PDEBUG;

    if (bridgeWid != nullptr) delete bridgeWid;
    if (ethernetWid != nullptr) delete ethernetWid;
    if (generalWid != nullptr) delete generalWid;
    if (ipWid != nullptr) delete ipWid;
    if (macvlanWid != nullptr) delete macvlanWid;
    if (mobileWid != nullptr) delete mobileWid;
    if (pppoeWid != nullptr) delete pppoeWid;
    if (tunnelWid != nullptr) delete tunnelWid;
    if (tuntapWid != nullptr) delete tuntapWid;
    if (vlanWid != nullptr) delete vlanWid;
    if (wirelessWid != nullptr) delete wirelessWid;

    if (ui != nullptr) delete ui;
}


Qt::ToolBarArea NewProfileWidget::getToolBarArea()
{
    if (debug) qDebug() << PDEBUG;

    return toolBarArea(ui->toolBar);
}


void NewProfileWidget::profileTabOpenProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;

    ui->comboBox_profile->addItem(profile);
    ui->comboBox_profile->setCurrentIndex(ui->comboBox_profile->count() - 1);
}


void NewProfileWidget::update()
{
    if (debug) qDebug() << PDEBUG;

    updateProfileTab();
    updateMenuProfile();
}


void NewProfileWidget::updateToolBarState(const Qt::ToolBarArea area)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Toolbar area" << area;

    removeToolBar(ui->toolBar);
    if (area != Qt::NoToolBarArea) {
        addToolBar(area, ui->toolBar);
        ui->toolBar->show();
    }
}


void NewProfileWidget::updateMenuProfile()
{
    if (debug) qDebug() << PDEBUG;

    bool selected = !ui->comboBox_profile->currentText().isEmpty();
    ui->actionLoad->setEnabled(selected);
    ui->actionRemove->setEnabled(selected);
    ui->actionSave->setEnabled(selected);
}


void NewProfileWidget::updateProfileTab()
{
    if (debug) qDebug() << PDEBUG;

    mainWindow->setDisabled(true);
    ui->comboBox_profile->clear();
    QList<netctlProfileInfo> profiles = generalInformation(mainWindow->netctlInterface,
                                                           useHelper, debug).netctlProfiles;
    for (int i=0; i<profiles.count(); i++)
        ui->comboBox_profile->addItem(profiles[i].name);
    ui->comboBox_profile->setCurrentIndex(-1);

    generalWid->clear();
    ipWid->clear();
    bridgeWid->clear();
    ethernetWid->clear();
    macvlanWid->clear();
    mobileWid->clear();
    pppoeWid->clear();
    tunnelWid->clear();
    tuntapWid->clear();
    vlanWid->clear();
    wirelessWid->clear();

    profileTabChangeState(generalWid->connectionType->currentText());
    mainWindow->setDisabled(false);
    mainWindow->showMessage(true);

}


void NewProfileWidget::profileTabChangeState(const QString current)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Current type" << current;

    generalWid->setVisible(true);
    ipWid->setVisible((current != QString("pppoe")) && (current != QString("mobile_ppp")));
    bridgeWid->setVisible(current == QString("bridge"));
    ethernetWid->setVisible((current == QString("ethernet")) ||
                            (current == QString("vlan")) ||
                            (current == QString("macvlan")));
    macvlanWid->setVisible(current == QString("macvlan"));
    mobileWid->setVisible(current == QString("mobile_ppp"));
    pppoeWid->setVisible(current == QString("pppoe"));
    tunnelWid->setVisible(current == QString("tunnel"));
    tuntapWid->setVisible(current == QString("tuntap"));
    vlanWid->setVisible(current == QString("vlan"));
    wirelessWid->setVisible(current == QString("wireless"));
}


void NewProfileWidget::profileTabCreateProfile()
{
    if (debug) qDebug() << PDEBUG;

    // error checking
    if (ui->comboBox_profile->currentText().isEmpty())
        return ErrorWindow::showWindow(3, QString(PDEBUG), debug);
    if (generalWid->isOk() == 1)
        return ErrorWindow::showWindow(4, QString(PDEBUG), debug);
    else if (generalWid->isOk() == 2)
        return ErrorWindow::showWindow(5, QString(PDEBUG), debug);
    if ((generalWid->connectionType->currentText() == QString("ethernet")) ||
            (generalWid->connectionType->currentText() == QString("wireless")) ||
            (generalWid->connectionType->currentText() == QString("bond")) ||
            (generalWid->connectionType->currentText() == QString("dummy")) ||
            (generalWid->connectionType->currentText() == QString("bridge")) ||
            (generalWid->connectionType->currentText() == QString("tunnel")) ||
            (generalWid->connectionType->currentText() == QString("tuntap")) ||
            (generalWid->connectionType->currentText() == QString("vlan")) ||
            (generalWid->connectionType->currentText() == QString("macvlan")) ||
            (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        if (ipWid->isOk() == 1)
            return ErrorWindow::showWindow(6, QString(PDEBUG), debug);
        else if (ipWid->isOk() == 2)
            return ErrorWindow::showWindow(6, QString(PDEBUG), debug);
    }
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        if (ethernetWid->isOk() == 1)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        if (wirelessWid->isOk() == 1)
            return ErrorWindow::showWindow(8, QString(PDEBUG), debug);
        else if (wirelessWid->isOk() == 2)
            return ErrorWindow::showWindow(9, QString(PDEBUG), debug);
        else if (wirelessWid->isOk() == 3)
            return ErrorWindow::showWindow(10, QString(PDEBUG), debug);
        else if (wirelessWid->isOk() == 4)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
        else if (wirelessWid->isOk() == 5)
            return ErrorWindow::showWindow(11, QString(PDEBUG), debug);
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        if (pppoeWid->isOk() == 1)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
        else if (pppoeWid->isOk() == 2)
            return ErrorWindow::showWindow(12, QString(PDEBUG), debug);
        else if (pppoeWid->isOk() == 3)
            return ErrorWindow::showWindow(13, QString(PDEBUG), debug);
        else if (pppoeWid->isOk() == 4)
            return ErrorWindow::showWindow(12, QString(PDEBUG), debug);
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        if (mobileWid->isOk() == 1)
            return ErrorWindow::showWindow(15, QString(PDEBUG), debug);
        if (mobileWid->isOk() == 2)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
    }
    else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        if (tunnelWid->isOk() == 1)
            return ErrorWindow::showWindow(20, QString(PDEBUG), debug);
    }
    else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        if (tuntapWid->isOk() == 1)
            return ErrorWindow::showWindow(15, QString(PDEBUG), debug);
        if (tuntapWid->isOk() == 2)
            return ErrorWindow::showWindow(15, QString(PDEBUG), debug);
    }
    else if (generalWid->connectionType->currentText() == QString("vlan")) {
        if (ethernetWid->isOk() == 1)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
    }
    else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        if (ethernetWid->isOk() == 1)
            return ErrorWindow::showWindow(7, QString(PDEBUG), debug);
    }

    mainWindow->setDisabled(true);
    // read settings
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    QMap<QString, QString> settings;
    settings = generalWid->getSettings();
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = wirelessWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if ((generalWid->connectionType->currentText() == QString("bond")) ||
               (generalWid->connectionType->currentText() == QString("dummy")) ||
               (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings =  bridgeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        QMap<QString, QString> addSettings = pppoeWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        QMap<QString, QString> addSettings = mobileWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tunnelWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = tuntapWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("vlan")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = vlanWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    } else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        QMap<QString, QString> addSettings = ipWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = ethernetWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
        addSettings = macvlanWid->getSettings();
        for (int i=0; i<addSettings.keys().count(); i++)
            settings.insert(addSettings.keys()[i], addSettings[addSettings.keys()[i]]);
    }

    // call netctlprofile
    InterfaceAnswer answer = createProfileSlot(profile, settings, mainWindow->netctlInterface,
                                               useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateProfileTab();
}


void NewProfileWidget::profileTabLoadProfile()
{
    if (debug) qDebug() << PDEBUG;

    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    if (profile.isEmpty()) return;
    QMap<QString, QString> settings = profileInformation(profile, mainWindow->netctlInterface,
                                                         useHelper, debug);

    if (settings.isEmpty()) return ErrorWindow::showWindow(17, QString(PDEBUG), debug);

    generalWid->setSettings(settings);
    if (generalWid->connectionType->currentText() == QString("ethernet")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("wireless")) {
        ipWid->setSettings(settings);
        wirelessWid->setSettings(settings);
    } else if ((generalWid->connectionType->currentText() == QString("bond")) ||
               (generalWid->connectionType->currentText() == QString("dummy")) ||
               (generalWid->connectionType->currentText() == QString("openvswitch"))) {
        ipWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("bridge")) {
        ipWid->setSettings(settings);
        bridgeWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("pppoe")) {
        pppoeWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("mobile_ppp")) {
        mobileWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("tunnel")) {
        ipWid->setSettings(settings);
        tunnelWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("tuntap")) {
        ipWid->setSettings(settings);
        tuntapWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("vlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        vlanWid->setSettings(settings);
    } else if (generalWid->connectionType->currentText() == QString("macvlan")) {
        ipWid->setSettings(settings);
        ethernetWid->setSettings(settings);
        macvlanWid->setSettings(settings);
    }
}


void NewProfileWidget::profileTabRemoveProfile()
{
    if (debug) qDebug() << PDEBUG;

    mainWindow->setDisabled(true);
    QString profile = QFileInfo(ui->comboBox_profile->currentText()).fileName();
    InterfaceAnswer answer = removeProfileSlot(profile, mainWindow->netctlInterface,
                                               useHelper, debug);
    mainWindow->showMessage(answer == InterfaceAnswer::True);

    updateProfileTab();
}


void NewProfileWidget::createActions()
{
    if (debug) qDebug() << PDEBUG;

    // menu actions
    connect(ui->actionClear, SIGNAL(triggered(bool)), this, SLOT(updateProfileTab()));
    connect(ui->actionLoad, SIGNAL(triggered(bool)), this, SLOT(profileTabLoadProfile()));
    connect(ui->actionRemove, SIGNAL(triggered(bool)), this, SLOT(profileTabRemoveProfile()));
    connect(ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(profileTabCreateProfile()));
    // main tab events
    connect(ui->comboBox_profile, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileTabLoadProfile()));
    connect(ui->comboBox_profile, SIGNAL(editTextChanged(QString)), this, SLOT(updateMenuProfile()));
    connect(ui->comboBox_profile->lineEdit(), SIGNAL(returnPressed()), this, SLOT(profileTabLoadProfile()));
    connect(generalWid->connectionType, SIGNAL(currentIndexChanged(QString)), this, SLOT(profileTabChangeState(QString)));
}
