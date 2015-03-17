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

#include "wifimenuwidget.h"
#include "ui_wifimenuwidget.h"

#include <QDebug>
#include <QMenu>
#include <QMessageBox>

#include <pdebug/pdebug.h>

#include "calls.h"
#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "mainwindow.h"
#include "passwdwidget.h"


WiFiMenuWidget::WiFiMenuWidget(QWidget *parent, const QMap<QString, QString> settings, const bool debugCmd)
    : QMainWindow(parent),
      debug(debugCmd),
      configuration(settings)
{
    mainWindow = dynamic_cast<MainWindow *>(parent);
    useHelper = (configuration[QString("USE_HELPER")] == QString("true"));

    // windows
    ui = new Ui::WiFiMenuWidget;
    ui->setupUi(this);
    ui->tableWidget_wifi->setColumnHidden(5, true);
    ui->tableWidget_wifi->setColumnHidden(6, true);
    updateToolBarState(static_cast<Qt::ToolBarArea>(configuration[QString("WIFI_TOOLBAR")].toInt()));

    createActions();
}


WiFiMenuWidget::~WiFiMenuWidget()
{
    if (debug) qDebug() << PDEBUG;

    if (ui != nullptr) delete ui;
}


Qt::ToolBarArea WiFiMenuWidget::getToolBarArea()
{
    if (debug) qDebug() << PDEBUG;

    return toolBarArea(ui->toolBar);
}


void WiFiMenuWidget::update()
{
    if (debug) qDebug() << PDEBUG;

    updateWifiTab();
    updateMenuWifi();
}


void WiFiMenuWidget::updateToolBarState(const Qt::ToolBarArea area)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Toolbar area" << area;

    removeToolBar(ui->toolBar);
    if (area != Qt::NoToolBarArea) {
        addToolBar(area, ui->toolBar);
        ui->toolBar->show();
    }
}


bool WiFiMenuWidget::wifiTabSelectEssidSlot(const QString essid)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Essid" << essid;

    for (int i=0; i<ui->tableWidget_wifi->rowCount(); i++) {
        if (ui->tableWidget_wifi->item(i, 0)->text() != essid) continue;
        ui->tableWidget_wifi->setCurrentCell(i, 0);
    }
    return (ui->tableWidget_wifi->currentItem() != nullptr);
}


void WiFiMenuWidget::connectToUnknownEssid(const QString passwd)
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_wifi->currentItem() == nullptr) return;
    if (passwdWid != nullptr) delete passwdWid;

    QMap<QString, QString> settings;
    QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 2)->text();
    if (security.contains(QString("WPA")))
        settings[QString("Security")] = QString("wpa");
    else if (security.contains(QString("WEP")))
        settings[QString("Security")] = QString("wep");
    else
        settings[QString("Security")] = QString("none");
    if (!passwd.isEmpty())
        settings[QString("Key")] = QString("'%1'").arg(passwd);
    if (hiddenNetwork)
        settings[QString("Hidden")] = QString("yes");
    QString essid = QString("'%1'").arg(ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text());
    InterfaceAnswer answer = connectToEssid(essid, settings, mainWindow->netctlInterface,
                                            useHelper, debug);

    QString profile = QString("netctl-gui-%1").arg(essid);
    profile.remove(QChar('"')).remove(QChar('\''));
    QString message;
    if (answer == InterfaceAnswer::True) {
        message = QApplication::translate("MainWindow", "Connection is successfully");
        mainWindow->showMessage(true);
    } else {
        message = QApplication::translate("MainWindow", "Connection failed");
        mainWindow->showMessage(false);
    }
    message += QString("\n");
    message += QApplication::translate("MainWindow", "Do you want to save profile %1?").arg(profile);
    int select = QMessageBox::question(this, QApplication::translate("MainWindow", "WiFi menu"),
                                       message, QMessageBox::Save | QMessageBox::Discard, QMessageBox::Save);

    switch (select) {
    case QMessageBox::Save:
        break;
    case QMessageBox::Discard:
    default:
        removeProfileSlot(profile, mainWindow->netctlInterface, useHelper, debug);
        break;
    }

    updateWifiTab();
}


void WiFiMenuWidget::setHiddenName(const QString name)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Set name" << name;

    ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->setText(name);

    wifiTabStart();
}


void WiFiMenuWidget::updateMenuWifi()
{
    if (debug) qDebug() << PDEBUG;

    bool selected = (ui->tableWidget_wifi->currentItem() != nullptr);
    ui->actionStart->setEnabled(selected);
    if (selected && ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 5)->text().isEmpty()) {
        ui->actionStart->setText(QApplication::translate("WiFiMenuWidget", "Start"));
        ui->actionStart->setIcon(QIcon::fromTheme("system-run"));
    } else {
        ui->actionStart->setText(QApplication::translate("WiFiMenuWidget", "Stop"));
        ui->actionStart->setIcon(QIcon::fromTheme("process-stop"));
    }
}


void WiFiMenuWidget::updateText(const netctlWifiInfo current)
{
    if (debug) qDebug() << PDEBUG;
    if (!wifiTabSetEnabled(checkExternalApps(QString("wpasup-only"), configuration, debug))) return;
    if (!checkExternalApps(QString("wpasup"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    ui->label_wifi->setText(QApplication::translate("WiFiMenuWidget", "Processing..."));

    QString text = QString("");
    text += QString("%1 - %2 - %3 ").arg(current.name).arg(current.security).arg(current.macs[0]);
    text += QString("(%1 %2)").arg(current.frequencies[0]).arg(QApplication::translate("WiFiMenuWidget", "MHz"));

    ui->label_wifi->setText(text);
}


void WiFiMenuWidget::updateWifiTab()
{
    if (debug) qDebug() << PDEBUG;
    if (!wifiTabSetEnabled(checkExternalApps(QString("wpasup-only"), configuration, debug))) return;
    if (!checkExternalApps(QString("wpasup"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        return mainWindow->emitNeedToBeConfigured();
    }

    mainWindow->setDisabled(true);
    QList<netctlWifiInfo> scanResults;
    if (useHelper)
        scanResults = parseOutputWifi(sendRequestToCtrl(QString("VerboseWiFi"), debug));
    else
        scanResults = mainWindow->wpaCommand->scanWifi();

    ui->tableWidget_wifi->setSortingEnabled(false);
    ui->tableWidget_wifi->selectRow(-1);
    ui->tableWidget_wifi->sortByColumn(3, Qt::AscendingOrder);
    ui->tableWidget_wifi->clear();
    // -1 because the first is always current point
    ui->tableWidget_wifi->setRowCount(scanResults.count() - 1);

    // create header
    QStringList headerList;
    headerList.append(QApplication::translate("WiFiMenuWidget", "Name"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "Type"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "# of points"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "Signal"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "Security"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "Active"));
    headerList.append(QApplication::translate("WiFiMenuWidget", "Exists"));
    ui->tableWidget_wifi->setHorizontalHeaderLabels(headerList);
    // create items
    netctlWifiInfo current = scanResults.isEmpty() ? netctlWifiInfo() : scanResults.takeFirst();
    for (int i=0; i<scanResults.count(); i++) {
        // font
        QFont font;
        font.setBold(scanResults[i].active);
        font.setItalic(scanResults[i].exists);
        // type
        QString type;
        switch (scanResults[i].type) {
        case PointType::TwoG:
            type = QApplication::translate("WiFiMenuWidget", "2GHz");
            break;
        case PointType::FiveG:
            type = QApplication::translate("WiFiMenuWidget", "5GHz");
            break;
        case PointType::TwoAndFiveG:
            type = QApplication::translate("WiFiMenuWidget", "2GHz and 5GHz");
            break;
        case PointType::None:
        default:
            type = QApplication::translate("WiFiMenuWidget", "N\\A");
            break;
        }
        // tooltip
        QString toolTip = QString("");
        for (int j=0; j<scanResults[i].macs.count(); j++)
            toolTip += QString("%1 %2: %3 (%4 %5)\n").arg(QApplication::translate("WiFiMenuWidget", "Point"))
                                                     .arg(j + 1)
                                                     .arg(scanResults[i].macs[j])
                                                     .arg(scanResults[i].frequencies[j])
                                                     .arg(QApplication::translate("WiFiMenuWidget", "MHz"));
        toolTip += QString("%1: %2\n").arg(QApplication::translate("WiFiMenuWidget", "Type"))
                                      .arg(type);
        toolTip += QString("%1: %2\n").arg(QApplication::translate("WiFiMenuWidget", "Active"))
                                      .arg(checkStatus(scanResults[i].active));
        toolTip += QString("%1: %2").arg(QApplication::translate("WiFiMenuWidget", "Exists"))
                                    .arg(checkStatus(scanResults[i].exists));
        // name
        ui->tableWidget_wifi->setItem(i, 0, new QTableWidgetItem(scanResults[i].name));
        ui->tableWidget_wifi->item(i, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 0)->setToolTip(toolTip);
        ui->tableWidget_wifi->item(i, 0)->setFont(font);
        // type
        ui->tableWidget_wifi->setItem(i, 1, new QTableWidgetItem(type));
        ui->tableWidget_wifi->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 1)->setToolTip(toolTip);
        // count
        ui->tableWidget_wifi->setItem(i, 2, new QTableWidgetItem(QString::number(scanResults[i].frequencies.count())));
        ui->tableWidget_wifi->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 2)->setToolTip(toolTip);
        // signal
        ui->tableWidget_wifi->setItem(i, 3, new QTableWidgetItem(QString::number(scanResults[i].signal)));
        ui->tableWidget_wifi->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 3)->setToolTip(toolTip);
        // security
        ui->tableWidget_wifi->setItem(i, 4, new QTableWidgetItem(scanResults[i].security));
        ui->tableWidget_wifi->item(i, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget_wifi->item(i, 4)->setToolTip(toolTip);
        // active
        ui->tableWidget_wifi->setItem(i, 5, new QTableWidgetItem(checkStatus(scanResults[i].active, true)));
        ui->tableWidget_wifi->item(i, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        // exists
        ui->tableWidget_wifi->setItem(i, 6, new QTableWidgetItem(checkStatus(scanResults[i].exists, true)));
        ui->tableWidget_wifi->item(i, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

    ui->tableWidget_wifi->setSortingEnabled(true);

    ui->tableWidget_wifi->resizeRowsToContents();
    ui->tableWidget_wifi->resizeColumnsToContents();
    ui->tableWidget_wifi->resizeRowsToContents();
#if QT_VERSION >= 0x050000
    ui->tableWidget_wifi->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
    ui->tableWidget_wifi->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif

    updateText(current);
    mainWindow->setDisabled(false);
    mainWindow->showMessage(true);
}


void WiFiMenuWidget::wifiTabContextualMenu(const QPoint &pos)
{
    if (debug) qDebug() << PDEBUG;
    if (ui->tableWidget_wifi->currentItem() == nullptr) return;

    // create menu
    QMenu menu(this);
    QAction *refreshTable = menu.addAction(QApplication::translate("WiFiMenuWidget", "Refresh"));
    refreshTable->setIcon(QIcon::fromTheme("view-refresh"));
    menu.addSeparator();
    QAction *startWifi = menu.addAction(QApplication::translate("WiFiMenuWidget", "Start WiFi"));

    // set text
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 5)->text().isEmpty()) {
        startWifi->setText(QApplication::translate("WiFiMenuWidget", "Start WiFi"));
        startWifi->setIcon(QIcon::fromTheme("system-run"));

    } else {
        startWifi->setText(QApplication::translate("WiFiMenuWidget", "Stop WiFi"));
        startWifi->setIcon(QIcon::fromTheme("process-stop"));
    }

    // actions
    QAction *action = menu.exec(ui->tableWidget_wifi->viewport()->mapToGlobal(pos));
    if (action == refreshTable) {
        if (debug) qDebug() << PDEBUG << ":" << "Refresh WiFi";
        updateWifiTab();
    } else if (action == startWifi) {
        if (debug) qDebug() << PDEBUG << ":" << "Start WiFi";
        wifiTabStart();
    }
}


bool WiFiMenuWidget::wifiTabSetEnabled(const bool state)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << state;

    ui->actionFunc->setEnabled(!state);
    ui->tableWidget_wifi->setEnabled(state);
    if (!state) ui->label_wifi->setText(QApplication::translate("WiFiMenuWidget", "Please install 'wpa_supplicant' before use it"));

    return state;
}


int WiFiMenuWidget::wifiTabShowInfo()
{
    if (debug) qDebug() << PDEBUG;

    return QMessageBox::information(this, QApplication::translate("WiFiMenuWidget", "Information"),
                                    QApplication::translate("WiFiMenuWidget", "This isn't the functionality you're looking for"));
}


void WiFiMenuWidget::wifiTabStart()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("wpasup"), configuration, debug)) {
        ErrorWindow::showWindow(1, QString(PDEBUG), debug);
        return mainWindow->emitNeedToBeConfigured();
    }
    if (ui->tableWidget_wifi->currentItem() == nullptr) return;

    mainWindow->setDisabled(true);

    // name is hidden
    if (ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text() == QString("<hidden>")) {
        hiddenNetwork = true;
        passwdWid = new PasswdWidget(this);
        passwdWid->setPassword(false);
        int widgetWidth = 2 * width() / 3;
        int widgetHeight = 110;
        int x = (width() - widgetWidth) / 2;
        int y = (height() - widgetHeight) / 2;
        passwdWid->setGeometry(x, y, widgetWidth, widgetHeight);
        passwdWid->show();
        passwdWid->setFocusToLineEdit();
        return;
    }

    // name isn't hidden
    hiddenNetwork = false;
    QString profile = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 0)->text();
    if (!ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 6)->text().isEmpty()) {
        InterfaceAnswer answer = startProfileSlot(profile, mainWindow->netctlInterface,
                                                  useHelper, debug);
        mainWindow->showMessage(answer == InterfaceAnswer::True);
    } else {
        QString security = ui->tableWidget_wifi->item(ui->tableWidget_wifi->currentItem()->row(), 1)->text();
        if (security == QString("none")) return connectToUnknownEssid(QString(""));
        else {
            passwdWid = new PasswdWidget(this);
            passwdWid->setPassword(true);
            int widgetWidth = 2 * width() / 3;
            int widgetHeight = 110;
            int x = (width() - widgetWidth) / 2;
            int y = (height() - widgetHeight) / 2;
            passwdWid->setGeometry(x, y, widgetWidth, widgetHeight);
            passwdWid->show();
            passwdWid->setFocusToLineEdit();
            return;
        }
    }

    updateWifiTab();
}


void WiFiMenuWidget::createActions()
{
    if (debug) qDebug() << PDEBUG;

    // menu actions
    connect(ui->actionFunc, SIGNAL(triggered(bool)), this, SLOT(wifiTabShowInfo()));
    connect(ui->actionRefresh, SIGNAL(triggered(bool)), this, SLOT(updateWifiTab()));
    connect(ui->actionStart, SIGNAL(triggered(bool)), this, SLOT(wifiTabStart()));
    // wifi tab events
    connect(ui->tableWidget_wifi, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(wifiTabStart()));
    connect(ui->tableWidget_wifi, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
            this, SLOT(updateMenuWifi()));
    connect(ui->tableWidget_wifi, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(wifiTabContextualMenu(QPoint)));
}
