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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QProcessEnvironment>
#include <QSettings>
#include <QTranslator>
#include <QUrl>

#include <language/language.h>
#include <listmap/listmap.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "aboutwindow.h"
#include "calls.h"
#include "commonfunctions.h"
#include "dbusoperation.h"
#include "errorwindow.h"
#include "mainwidget.h"
#include "netctlautowindow.h"
#include "netctlguiadaptor.h"
#include "newprofilewidget.h"
#include "passwdwidget.h"
#include "settingswindow.h"
#include "trayicon.h"
#include "version.h"
#include "wifimenuwidget.h"


MainWindow::MainWindow(QWidget *parent, const QVariantMap args,
                       QTranslator *qtAppTranslator, QTranslator *appTranslator)
    : QMainWindow(parent),
      configPath(args[QString("config")].toString()),
      debug(args[QString("debug")].toBool()),
      qtTranslator(qtAppTranslator),
      translator(appTranslator)
{
    setWindowIcon(QIcon(":icon"));
    if (debug) qDebug() << PDEBUG << ":" << "about" << args[QString("about")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "auto" << args[QString("auto")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "config" << configPath;
    if (debug) qDebug() << PDEBUG << ":" << "debug" << debug;
    if (debug) qDebug() << PDEBUG << ":" << "defaults" << args[QString("defaults")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "essid" << args[QString("essid")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "minimized" << args[QString("minimized")].toInt();
    if (debug) qDebug() << PDEBUG << ":" << "open" << args[QString("open")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "options" << args[QString("options")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "select" << args[QString("select")].toString();
    if (debug) qDebug() << PDEBUG << ":" << "settings" << args[QString("settings")].toBool();
    if (debug) qDebug() << PDEBUG << ":" << "tab" << args[QString("tab")].toInt();

    ui = new Ui::MainWindow;
    ui->setupUi(this);
    updateConfiguration(args);

    // main actions
    if (args[QString("essid")].toString() != QString("ESSID")) {
        if (!wifiMenuWidget->wifiTabSelectEssidSlot(args[QString("essid")].toString()))
            ErrorWindow::showWindow(18, QString(PDEBUG), debug);
    } else if (args[QString("open")].toString() != QString("PROFILE")) {
        newProfileWidget->profileTabOpenProfileSlot(args[QString("open")].toString());
    } else if (args[QString("select")].toString() != QString("PROFILE")) {
        if (!mainWidget->mainTabSelectProfileSlot(args[QString("select")].toString()))
            ErrorWindow::showWindow(17, QString(PDEBUG), debug);
    }

    // show windows
    if (args[QString("about")].toBool())
        showAboutWindow();
    if (args[QString("auto")].toBool())
        showNetctlAutoWindow();
    if (args[QString("settings")].toBool())
        showSettingsWindow();

    ui->statusBar->showMessage(QApplication::translate("MainWindow", "Ready"));
}


MainWindow::~MainWindow()
{
    if (debug) qDebug() << PDEBUG;

    if (configuration[QString("CLOSE_HELPER")] == QString("true"))
        forceStopHelper();
    deleteObjects();
    delete ui;
}


int MainWindow::currentTab()
{
    if (debug) qDebug() << PDEBUG;

    return ui->stackedWidget->currentIndex();
}


Qt::ToolBarArea MainWindow::getToolBarArea()
{
    if (debug) qDebug() << PDEBUG;

    return toolBarArea(ui->toolBar);
}


void MainWindow::emitNeedToBeConfigured()
{
    if (debug) qDebug() << PDEBUG;

    emit(needToBeConfigured());
}


QStringList MainWindow::printSettings()
{
    if (debug) qDebug() << PDEBUG;

    return mapToList(configuration);
}


netctlCurrent MainWindow::printTrayInformation()
{
    if (debug) qDebug() << PDEBUG;

    return trayInformation(netctlInterface, useHelper, debug);
}


bool MainWindow::isHelperActive()
{
    if (debug) qDebug() << PDEBUG;

    QList<QVariant> responce = sendRequestToCtrl(QString("Active"), debug);

    return (!responce.isEmpty());
}


bool MainWindow::isHelperServiceActive()
{
    if (debug) qDebug() << PDEBUG;

    QString cmd = QString("%1 is-active %2").arg(configuration[QString("SYSTEMCTL_PATH")])
                                            .arg(configuration[QString("HELPER_SERVICE")]);
    if (debug) qDebug() << PDEBUG << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (debug) qDebug() << PDEBUG;

    if ((QSystemTrayIcon::isSystemTrayAvailable()) && (configuration[QString("SYSTRAY")] == QString("true"))) {
        hide();
        trayIcon->showMessage(QApplication::translate("MainWindow", "Information"),
                              QApplication::translate("MainWindow", "Application has been hidden to tray"));
        event->ignore();
    } else
        closeMainWindow();
}


// tray icon
void MainWindow::closeMainWindow()
{
    if (debug) qDebug() << PDEBUG;

    storeToolBars();
    qApp->quit();
}


void MainWindow::openProfileSlot(const QString profile)
{
    if (debug) qDebug() << PDEBUG;

    newProfileWidget->profileTabOpenProfileSlot(profile);
}


void MainWindow::showAboutWindow()
{
    if (debug) qDebug() << PDEBUG;

    aboutWin->show();
}


void MainWindow::showMainWindow()
{
    if (debug) qDebug() << PDEBUG;

    if (isHidden()) {
        setTab(ui->stackedWidget->currentIndex());
        show();
    } else
        hide();
}


void MainWindow::showNetctlAutoWindow()
{
    if (debug) qDebug() << PDEBUG;

    netctlAutoWin->showWindow();
}


void MainWindow::showSettingsWindow()
{
    if (debug) qDebug() << PDEBUG;

    storeToolBars();
    settingsWin->showWindow();
}


void MainWindow::showApi()
{
    if (debug) qDebug() << PDEBUG;

    showMessage(QDesktopServices::openUrl(QUrl(QString("%1netctl-gui-dbus-api.html").arg(QString(DOCS_PATH)))));
}


void MainWindow::showLibrary()
{
    if (debug) qDebug() << PDEBUG;

    showMessage(QDesktopServices::openUrl(QUrl(QString("%1html/index.html").arg(QString(DOCS_PATH)))));
}


void MainWindow::showSecurityNotes()
{
    if (debug) qDebug() << PDEBUG;

    showMessage(QDesktopServices::openUrl(QUrl(QString("%1netctl-gui-security-notes.html").arg(QString(DOCS_PATH)))));
}


bool MainWindow::forceStartHelper()
{
    if (debug) qDebug() << PDEBUG;
    if (!checkExternalApps(QString("helper"), configuration, debug)) {
        ErrorWindow::showWindow(1, externalApps(QString("helper"), configuration).join(QChar('\n')), debug);
        emit(needToBeConfigured());
        return false;
    }

    QString cmd = QString("%1 -c %2").arg(configuration[QString("HELPER_PATH")]).arg(configPath);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


bool MainWindow::forceStopHelper()
{
    if (debug) qDebug() << PDEBUG;

    QList<QVariant> responce = sendRequestToCtrl(QString("Close"), debug);

    return (!responce.isEmpty());
}


bool MainWindow::startHelper()
{
    if (debug) qDebug() << PDEBUG;

    if (isHelperActive())
        return forceStopHelper();
    else
        return forceStartHelper();
}


void MainWindow::setDisabled(const bool disabled)
{
    if (debug) qDebug() << PDEBUG;

    ui->toolBar->setDisabled(disabled);
    ui->stackedWidget->setDisabled(disabled);
}


void MainWindow::setTab(int tab)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Set tab" << tab;
    if (ui->stackedWidget->currentWidget() == nullptr) return;

    if ((tab > 2) || (tab < 0)) tab = 0;
    if (tab != ui->stackedWidget->currentIndex())
        return ui->stackedWidget->setCurrentIndex(tab);

    switch (tab) {
    case 1:
        newProfileWidget->update();
        break;
    case 2:
        wifiMenuWidget->update();
        break;
    case 0:
    default:
        mainWidget->update();
        break;
    }
}


void MainWindow::showMessage(const bool status)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Status" << status;

    if (status)
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Done"));
    else
        ui->statusBar->showMessage(QApplication::translate("MainWindow", "Error"));
}


void MainWindow::storeToolBars()
{
    if (debug) qDebug() << PDEBUG;

    QSettings settings(configPath, QSettings::IniFormat);

    settings.beginGroup(QString("Toolbars"));
    settings.setValue(QString("MAIN_TOOLBAR"), QString::number(getToolBarArea()));
    settings.setValue(QString("NETCTL_TOOLBAR"), QString::number(mainWidget->getToolBarArea()));
    settings.setValue(QString("NETCTLAUTO_TOOLBAR"), QString::number(netctlAutoWin->getToolBarArea()));
    settings.setValue(QString("PROFILE_TOOLBAR"), QString::number(newProfileWidget->getToolBarArea()));
    settings.setValue(QString("WIFI_TOOLBAR"), QString::number(wifiMenuWidget->getToolBarArea()));
    settings.endGroup();

    settings.sync();
}


void MainWindow::updateConfiguration(const QVariantMap args)
{
    if (debug) qDebug() << PDEBUG;

    deleteObjects();

    QString actualConfigPath = QFile(configPath).exists() ? configPath : QString("/etc/netctl-gui.conf");
    settingsWin = new SettingsWindow(this, debug, actualConfigPath);
    if (args[QString("default")].toBool())
        settingsWin->setDefault();
    configuration = settingsWin->getSettings();
    delete settingsWin;
    QMap<QString, QString> optionsDict = parseOptions(args[QString("options")].toString());
    for (int i=0; i<optionsDict.keys().count(); i++)
        configuration[optionsDict.keys()[i]] = optionsDict[optionsDict.keys()[i]];
    useHelper = ((configuration[QString("USE_HELPER")] == QString("true")) &&
                 (checkExternalApps(QString("helper"), configuration, debug)));

    // update translation
    qApp->removeTranslator(translator);
    QString language = Language::defineLanguage(actualConfigPath, args[QString("options")].toString());
    if (debug) qDebug() << PDEBUG << ":" << "Language is" << language;
    qtTranslator->load(QString("qt_%1").arg(language), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(qtTranslator);
    translator->load(QString(":/translations/%1").arg(language));
    qApp->installTranslator(translator);

    createObjects();
    createActions();

    // tray
    trayIcon->setVisible(QSystemTrayIcon::isSystemTrayAvailable() && (configuration[QString("SYSTRAY")] == QString("true")));
    if (trayIcon->isVisible()) {
        setHidden(configuration[QString("STARTTOTRAY")] == QString("true"));
        if (args[QString("minimized")].toInt() == 1)
            show();
        else if (args[QString("minimized")].toInt() == 2)
            hide();
    }
    else
        show();

    // update ui
    setTab(args[QString("tab")].toInt() - 1);
}


void MainWindow::updateToolBarState(const Qt::ToolBarArea area)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Toolbar area" << area;

    removeToolBar(ui->toolBar);
    if (area != Qt::NoToolBarArea) {
        addToolBar(area, ui->toolBar);
        ui->toolBar->show();
    }
}


// private slots
void MainWindow::setTabByAction(QAction *action)
{
    if (debug) qDebug() << PDEBUG;
    if (action == nullptr) return;

    if (action == ui->actionNetctl)
        setTab(0);
    else if (action == ui->actionProfiles)
        setTab(1);
    else if (action == ui->actionWiFi_menu)
        setTab(2);
}


void MainWindow::reportABug()
{
    if (debug) qDebug() << PDEBUG;

    showMessage(QDesktopServices::openUrl(QUrl(QString(BUGTRACKER))));
}


// window signals
bool MainWindow::checkHelperStatus()
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper) useHelper = isHelperActive();
    if (!useHelper) {
        configuration[QString("CLOSE_HELPER")] = QString("false");
        configuration[QString("FORCE_SUDO")] = QString("true");
        configuration[QString("USE_HELPER")] = QString("false");
    }
    if (isHelperServiceActive())
        configuration[QString("CLOSE_HELPER")] = QString("false");

    return useHelper;
}


void MainWindow::createActions()
{
    if (debug) qDebug() << PDEBUG;

    connect(ui->toolBar, SIGNAL(actionTriggered(QAction *)), this, SLOT(setTabByAction(QAction *)));
    connect(ui->actionNetctl_auto, SIGNAL(triggered()), this, SLOT(showNetctlAutoWindow()));
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(setTab(int)));
    connect(this, SIGNAL(needToBeConfigured()), this, SLOT(showSettingsWindow()));

    // menu
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(showSettingsWindow()));
    connect(ui->actionSecurity, SIGNAL(triggered()), this, SLOT(showSecurityNotes()));
    connect(ui->actionDBus_API, SIGNAL(triggered()), this, SLOT(showApi()));
    connect(ui->actionLibrary, SIGNAL(triggered()), this, SLOT(showLibrary()));
    connect(ui->actionReport_a_bug, SIGNAL(triggered()), this, SLOT(reportABug()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutWindow()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(closeMainWindow()));
}


void MainWindow::createDBusSession()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(DBUS_SERVICE)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register service";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
    }
    if (!bus.registerObject(DBUS_OBJECT_PATH,
                            new NetctlGuiAdaptor(this),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register GUI object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
    }
}


void MainWindow::createObjects()
{
    if (debug) qDebug() << PDEBUG;

    // backend
    createDBusSession();
    if ((useHelper) && (!forceStartHelper())) {
        ErrorWindow::showWindow(19, QString(PDEBUG), debug);
        useHelper = false;
    }
    checkHelperStatus();

    netctlCommand = new Netctl(debug, configuration);
    netctlInterface = new NetctlInterface(debug, configuration);
    wpaCommand = new WpaSup(debug, configuration);
    // frontend
    mainWidget = new MainWidget(this, configuration, debug);
    netctlAutoWin = new NetctlAutoWindow(this, configuration, debug);
    newProfileWidget = new NewProfileWidget(this, configuration, debug);
    wifiMenuWidget = new WiFiMenuWidget(this, configuration, debug);
    trayIcon = new TrayIcon(this, configuration, debug);
    // windows
    ui->retranslateUi(this);
    ui->stackedWidget->addWidget(mainWidget);
    ui->stackedWidget->addWidget(newProfileWidget);
    ui->stackedWidget->addWidget(wifiMenuWidget);
    updateToolBarState(static_cast<Qt::ToolBarArea>(configuration[QString("MAIN_TOOLBAR")].toInt()));
    aboutWin = new AboutWindow(this, debug);
    settingsWin = new SettingsWindow(this, debug, configPath);
}


void MainWindow::deleteObjects()
{
    if (debug) qDebug() << PDEBUG;

    // workaround to avoid crash on window closing
    disconnect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(setTab(int)));

    QDBusConnection::sessionBus().unregisterObject(DBUS_OBJECT_PATH);
    QDBusConnection::sessionBus().unregisterService(DBUS_SERVICE);
    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlInterface != nullptr) delete netctlInterface;
    if (wpaCommand != nullptr) delete wpaCommand;

    if (aboutWin != nullptr) delete aboutWin;
    if (settingsWin != nullptr) delete settingsWin;
    if (trayIcon != nullptr) delete trayIcon;
    if (mainWidget != nullptr) delete mainWidget;
    if (netctlAutoWin != nullptr) delete netctlAutoWin;
    if (newProfileWidget != nullptr) delete newProfileWidget;
    if (wifiMenuWidget != nullptr) delete wifiMenuWidget;
}


QMap<QString, QString> MainWindow::parseOptions(const QString options)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    for (int i=0; i<options.split(QChar(',')).count(); i++) {
        QStringList option = options.split(QChar(','))[i].split(QChar('='));
        if (option.count() != 2) continue;
        settings[option[0]] = option[1];
    }
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << QString("%1=%2").arg(settings.keys()[i]).arg(settings[settings.keys()[i]]);

    return settings;
}
