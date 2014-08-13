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

#include <KConfigDialog>
#include <KFileDialog>
#include <KGlobal>
#include <KNotification>
#include <KStandardDirs>
#include <KUrl>
#include <plasma/theme.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QMenu>
#include <QProcessEnvironment>

#include "netctl.h"
#include "pdebug.h"
#include "ui_about.h"
#include "ui_appearance.h"
#include "ui_dataengine.h"
#include "ui_widget.h"
#include "version.h"


IconLabel::IconLabel(Netctl *wid, const bool debugCmd)
    : QLabel(),
      debug(debugCmd),
      widget(wid)
{
}


IconLabel::~IconLabel()
{
}


void IconLabel::mousePressEvent(QMouseEvent *event)
{
    if (debug) qDebug() << PDEBUG;

    if (event->button() == Qt::LeftButton)
        widget->showGui();
}


Netctl::Netctl(QObject *parent, const QVariantList &args)
    : Plasma::PopupApplet(parent, args)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("NETCTLGUI_DEBUG"), QString("no"));
    if (debugEnv == QString("yes"))
        debug = true;
    else
        debug = false;

    setBackgroundHints(DefaultBackground);
    setAspectRatioMode(Plasma::IgnoreAspectRatio);
    setHasConfigurationInterface(true);
    // text format init
    formatLine.append(QString(""));
    formatLine.append(QString(""));
}


Netctl::~Netctl()
{
    if (debug) qDebug() << PDEBUG;

//    // actions
//    delete startProfileMenu;
//    delete switchToProfileMenu;
//    // dataengine
//    disconnectFromEngine();
//    delete netctlEngine;
//    // ui
//    delete iconWidget;
//    delete textLabel;
//    delete layout;
//    delete graphicsWidget;
}


void Netctl::init()
{
    if (debug) qDebug() << PDEBUG;

    info[QString("current")] = QString("N\\A");
    info[QString("extip4")] = QString("N\\A");
    info[QString("extip6")] = QString("N\\A");
    info[QString("interfaces")] = QString("N\\A");
    info[QString("intip4")] = QString("N\\A");
    info[QString("intip6")] = QString("N\\A");
    info[QString("profiles")] = QString("N\\A");
    info[QString("status")] = QString("N\\A");

    netctlEngine = dataEngine(QString("netctl"));
    createActions();
    // generate ui
    graphicsWidget = new QWidget();
    graphicsWidget->setAttribute(Qt::WA_TranslucentBackground, true);
    setWidget(graphicsWidget);
    // layouts
    layout = new QHBoxLayout(graphicsWidget);
    layout->setContentsMargins(1, 1, 1, 1);
    iconLabel = new IconLabel(this, debug);
    layout->addWidget(iconLabel);
    textLabel = new QLabel(graphicsWidget);
    layout->addWidget(textLabel);
    // read variables
    configChanged();
}


QString Netctl::parsePattern(const QString rawLine)
{
    if (debug) qDebug() << PDEBUG;

    QString line = rawLine;
    for (int i=0; i<info.keys().count(); i++)
        if (line.contains(QString("$") + info.keys()[i]))
            line = line.split(QString("$") + info.keys()[i])[0] +
                    info[info.keys()[i]] +
                    line.split(QString("$") + info.keys()[i])[1];

    return line;
}


QMap<QString, QString> Netctl::readDataEngineConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> rawConfig;
    rawConfig[QString("EXTIP4")] = QString("false");
    rawConfig[QString("EXTIP4CMD")] = QString("curl ip4.telize.com");
    rawConfig[QString("EXTIP6")] = QString("false");
    rawConfig[QString("EXTIP6CMD")] = QString("curl ip6.telize.com");
    rawConfig[QString("NETCTLCMD")] = QString("/usr/bin/netctl");
    rawConfig[QString("NETCTLAUTOCMD")] = QString("/usr/bin/netctl-auto");

    QString fileName = KGlobal::dirs()->findResource("config", "netctl.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::ReadOnly))
        return updateDataEngineConfiguration(rawConfig);
    QString fileStr;
    QStringList value;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('='))) {
            value.clear();
            for (int i=1; i<fileStr.split(QChar('=')).count(); i++)
                value.append(fileStr.split(QChar('='))[i]);
            rawConfig[fileStr.split(QChar('='))[0]] = value.join(QChar('='));
        }
        if (configFile.atEnd()) break;
    }
    configFile.close();

    return updateDataEngineConfiguration(rawConfig);
}


void Netctl::writeDataEngineConfiguration(const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config = updateDataEngineConfiguration(settings);
    QString fileName = KGlobal::dirs()->locateLocal("config", "netctl.conf");
    if (debug) qDebug() << PDEBUG << ":" << "Configuration file" << fileName;
    QFile configFile(fileName);
    if (!configFile.open(QIODevice::WriteOnly)) return;
    for (int i=0; i<config.keys().count(); i++) {
        QByteArray string = (config.keys()[i] + QString("=") + config[config.keys()[i]] + QString("\n")).toUtf8();
        configFile.write(string);
    }
    configFile.close();
}


QMap<QString, QString> Netctl::updateDataEngineConfiguration(const QMap<QString, QString> rawConfig)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QString key, value;
    // remove spaces and copy source map
    for (int i=0; i<rawConfig.keys().count(); i++) {
        key = rawConfig.keys()[i];
        value = rawConfig[key];
        key.remove(QChar(' '));
        if ((key != QString("EXTIP4CMD")) &&
            (key != QString("EXTIP6CMD")) &&
            (key != QString("NETCTLCMD")) &&
            (key != QString("NETCTLAUTOCMD")))
            value.remove(QChar(' '));
        config[key] = value;
    }

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << config.keys()[i] + QString("=") + config[config.keys()[i]];

    return config;
}


void Netctl::updateIcon()
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Status" << status;

    QString icon;
    if (status)
        icon = paths[QString("active")];
    else
        icon = paths[QString("inactive")];

    setPopupIcon(KIcon(icon));
    QPixmap iconPixmap;
    iconPixmap.load(icon);
    iconLabel->setPixmap(iconPixmap);
}


void Netctl::updateInterface(bool setShown)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "State" << setShown;

    if (setShown)
        layout->addWidget(textLabel);
    else
        layout->removeWidget(textLabel);
    graphicsWidget->adjustSize();
    resize(1, 1);
}


// context menu
void Netctl::enableProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    QString enableStatus = QString("");
    if (info[QString("status")].contains(QString("enabled"))) {
        enableStatus = QString(" disable ");
        sendNotification(QString("Info"), i18n("Set profile %1 disabled", info[QString("current")]));
    } else {
        enableStatus = QString(" enable ");
        sendNotification(QString("Info"), i18n("Set profile %1 enabled", info[QString("current")]));
    }
    if (useHelper) {
        QList<QVariant> args;
        args.append(info[QString("current")]);
        sendDBusRequest(QString("Enable"), args);
    } else {
        QProcess command;
        QString commandLine = QString("");
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ");
        commandLine += paths[QString("netctl")] + enableStatus + info[QString("current")];
        command.startDetached(commandLine);
    }
}


void Netctl::restartProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Restart profile %1", info[QString("current")]));
    if (useHelper) {
        QList<QVariant> args;
        args.append(info[QString("current")]);
        sendDBusRequest(QString("Restart"), args);
    } else {
        QProcess command;
        QString commandLine = QString("");
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ");
        commandLine += paths[QString("netctl")] + QString(" restart ") + info[QString("current")];
        command.startDetached(commandLine);
    }
}


void Netctl::startProfileSlot(QAction *profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile->text().remove(QChar('&'));

    sendNotification(QString("Info"), i18n("Start profile %1", profile->text().remove(QChar('&'))));
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile->text().remove(QChar('&')));
        if (status)
            sendDBusRequest(QString("SwitchTo"), args);
        else
            sendDBusRequest(QString("Start"), args);
    } else {
        QProcess command;
        QString commandLine = QString("");
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ");
        if (status)
            commandLine += paths[QString("netctl")] + QString(" switch-to ") + profile->text().remove(QChar('&'));
        else
            commandLine += paths[QString("netctl")] + QString(" start ") + profile->text().remove(QChar('&'));
        command.startDetached(commandLine);
    }
}


void Netctl::stopProfileSlot()
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Stop profile %1", info[QString("current")]));
    if (useHelper) {
        QList<QVariant> args;
        args.append(info[QString("current")]);
        sendDBusRequest(QString("Start"), args);
    } else {
        QProcess command;
        QString commandLine = QString("");
        if (useSudo)
            commandLine = paths[QString("sudo")] + QString(" ");
        commandLine += paths[QString("netctl")] + QString(" stop ") + info[QString("current")];
        command.startDetached(commandLine);
    }
}


void Netctl::switchToProfileSlot(QAction *profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile->text().remove(QChar('&'));


    sendNotification(QString("Info"), i18n("Switch to profile %1", profile->text().remove(QChar('&'))));
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile->text().remove(QChar('&')));
        sendDBusRequest(QString("autoStart"), args);
    } else {
        QProcess command;
        QString commandLine = paths[QString("netctlAuto")] + QString(" switch-to ") +
                profile->text().remove(QChar('&'));
        command.startDetached(commandLine);
    }
}


void Netctl::startHelper()
{
    if (debug) qDebug() << PDEBUG;

    QProcess command;
    QString commandLine = paths[QString("helper")];

    command.startDetached(commandLine);
}


void Netctl::checkHelperStatus()
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper)
        useHelper = !sendDBusRequest(QString("Active"), QList<QVariant>()).isEmpty();
}


QList<QAction*> Netctl::contextualActions()
{
    if (debug) qDebug() << PDEBUG;

    if (status)
        contextMenu[QString("title")]->setIcon(QIcon(paths[QString("active")]));
    else
        contextMenu[QString("title")]->setIcon(QIcon(paths[QString("inactive")]));
    contextMenu[QString("title")]->setText(info[QString("current")] + QString(" ") + info[QString("status")]);

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
    } else {
        contextMenu[QString("start")]->setVisible(true);
        contextMenu[QString("stop")]->setVisible(status);
        contextMenu[QString("switch")]->setVisible(false);
        contextMenu[QString("restart")]->setVisible(status);
        contextMenu[QString("enable")]->setVisible(status);
        if (status) {
            contextMenu[QString("start")]->setText(i18n("Start another profile"));
            contextMenu[QString("stop")]->setText(i18n("Stop %1", info[QString("current")]));
            contextMenu[QString("restart")]->setText(i18n("Restart %1", info[QString("current")]));
            if (info[QString("status")].contains(QString("enabled")))
                contextMenu[QString("enable")]->setText(i18n("Disable %1", info[QString("current")]));
            else
                contextMenu[QString("enable")]->setText(i18n("Enable %1", info[QString("current")]));
        }
        else
            contextMenu[QString("start")]->setText(i18n("Start profile"));
        startProfileMenu->clear();
        for (int i=0; i<profileList.count(); i++) {
            QAction *profile = new QAction(profileList[i], this);
            startProfileMenu->addAction(profile);
        }
    }

    contextMenu[QString("wifi")]->setVisible(useWifi);

    return menuActions;
}


void Netctl::createActions()
{
    if (debug) qDebug() << PDEBUG;

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

    contextMenu[QString("gui")] = new QAction(i18n("Show netctl-gui"), this);
    contextMenu[QString("gui")]->setIcon(QIcon(":icon"));
    connect(contextMenu[QString("gui")], SIGNAL(triggered(bool)), this, SLOT(showGui()));
    menuActions.append(contextMenu[QString("gui")]);

    contextMenu[QString("wifi")] = new QAction(i18n("Show WiFi menu"), this);
    contextMenu[QString("wifi")]->setIcon(QIcon(":wifi"));
    connect(contextMenu[QString("wifi")], SIGNAL(triggered(bool)), this, SLOT(showWifi()));
    menuActions.append(contextMenu[QString("wifi")]);
}


// events
void Netctl::sendNotification(const QString eventId, const QString message)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << eventId;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << message;

    KNotification *notification = new KNotification(eventId);
    notification->setComponentData(KComponentData("plasma_applet_netctl"));
    notification->setTitle(QString("Netctl ::: ") + eventId);
    notification->setText(message);
    notification->sendEvent();
    delete notification;
}


void Netctl::showGui()
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Start GUI"));
    QProcess command;

    command.startDetached(paths[QString("gui")]);
}


void Netctl::showWifi()
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Start WiFi menu"));
    QProcess command;

    command.startDetached(paths[QString("wifi")]);
}


// data engine interaction
void Netctl::connectToEngine()
{
    if (debug) qDebug() << PDEBUG;

    netctlEngine->connectSource(QString("active"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("current"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("extip4"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("extip6"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("interfaces"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("intip4"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("intip6"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("profiles"), this, autoUpdateInterval);
    netctlEngine->connectSource(QString("status"), this, autoUpdateInterval);
    updateInterface(bigInterface);
}


void Netctl::dataUpdated(const QString &sourceName, const Plasma::DataEngine::Data &data)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Source" << sourceName;

    if (isPopupShowing()) return;
    if (isUserConfiguring()) return;
    if (data.isEmpty()) return;
    QString value = data[QString("value")].toString();
    if (value.isEmpty())
        value = QString("N\\A");

    if (sourceName == QString("active")) {
        if (value == QString("true")) {
            if (!status)
                sendNotification(QString("Info"), i18n("Network is up"));
            status = true;
        } else {
            if (status)
                sendNotification(QString("Info"), i18n("Network is down"));
            status = false;
        }
        updateIcon();
    } else if (sourceName == QString("current")) {
        info[QString("current")] = value;
        // update text
        if (bigInterface)
            textLabel->setText(formatLine[0] + parsePattern(textPattern) + formatLine[1]);
    } else if (sourceName == QString("extip4")) {
        info[QString("extip4")] = value;
    } else if (sourceName == QString("extip6")) {
        info[QString("extip6")] = value;
    } else if (sourceName == QString("interfaces")) {
        info[QString("interfaces")] = value;
    } else if (sourceName == QString("intip4")) {
        info[QString("intip4")] = value;
    } else if (sourceName == QString("intip6")) {
        info[QString("intip6")] = value;
    } else if (sourceName == QString("profiles")) {
        profileList = value.split(QChar(','));
        info[QString("profiles")] = profileList.join(QChar(','));
    } else if (sourceName == QString("status")) {
        info[QString("status")] = QString("(") + value + QString(")");
    }

    update();
}


void Netctl::disconnectFromEngine()
{
    if (debug) qDebug() << PDEBUG;

    netctlEngine->disconnectSource(QString("currentProfile"), this);
    netctlEngine->disconnectSource(QString("extIp4"), this);
    netctlEngine->disconnectSource(QString("extIp6"), this);
    netctlEngine->disconnectSource(QString("interfaces"), this);
    netctlEngine->disconnectSource(QString("intIp4"), this);
    netctlEngine->disconnectSource(QString("intIp6"), this);
    netctlEngine->disconnectSource(QString("profiles"), this);
    netctlEngine->disconnectSource(QString("statusBool"), this);
    netctlEngine->disconnectSource(QString("statusString"), this);

    updateInterface(false);
}


QList<QVariant> Netctl::sendDBusRequest(const QString cmd, const QList<QVariant> args)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;

    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty())
        request.setArguments(args);
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();
    if (arguments.size() == 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();

    return arguments;
}


//  configuration interface
void Netctl::selectAbstractSomething()
{
    if (debug) qDebug() << PDEBUG;

    QString path = QString("/usr/bin");
    QLineEdit *lineEdit = uiWidConfig.lineEdit_gui;
    if (sender() == uiAppConfig.pushButton_activeIcon) {
        path = QString("/usr/share/icons");
        lineEdit = uiAppConfig.lineEdit_activeIcon;
    } else if (sender() == uiAppConfig.pushButton_inactiveIcon) {
        path = QString("/usr/share/icons");
        lineEdit = uiAppConfig.lineEdit_inactiveIcon;
    } else if (sender() == uiWidConfig.pushButton_gui)
        lineEdit = uiWidConfig.lineEdit_gui;
    else if (sender() == uiWidConfig.pushButton_helper)
        lineEdit = uiWidConfig.lineEdit_helper;
    else if (sender() == uiWidConfig.pushButton_netctl)
        lineEdit = uiWidConfig.lineEdit_netctl;
    else if (sender() == uiWidConfig.pushButton_netctlAuto)
        lineEdit = uiWidConfig.lineEdit_netctlAuto;
    else if (sender() == uiWidConfig.pushButton_sudo)
        lineEdit = uiWidConfig.lineEdit_sudo;
    else if (sender() == uiWidConfig.pushButton_wifi)
        lineEdit = uiWidConfig.lineEdit_wifi;
    else if (sender() == uiDEConfig.pushButton_extIp4)
        lineEdit = uiDEConfig.lineEdit_extIp4;
    else if (sender() == uiDEConfig.pushButton_extIp6)
        lineEdit = uiDEConfig.lineEdit_extIp6;
    else if (sender() == uiDEConfig.pushButton_netctl)
        lineEdit = uiDEConfig.lineEdit_netctl;
    else if (sender() == uiDEConfig.pushButton_netctlAuto)
        lineEdit = uiDEConfig.lineEdit_netctlAuto;

    KUrl url = KFileDialog::getOpenUrl(KUrl(path), "*");
    lineEdit->setText(url.path());
}


void Netctl::createConfigurationInterface(KConfigDialog *parent)
{
    if (debug) qDebug() << PDEBUG;

    QWidget *configWidget = new QWidget;
    uiWidConfig.setupUi(configWidget);
    QWidget *appWidget = new QWidget;
    uiAppConfig.setupUi(appWidget);
    QWidget *deWidget = new QWidget;
    uiDEConfig.setupUi(deWidget);
    QWidget *aboutWidget = new QWidget;
    uiAboutConfig.setupUi(aboutWidget);

    uiWidConfig.spinBox_autoUpdate->setValue(autoUpdateInterval);
    uiWidConfig.lineEdit_gui->setText(paths[QString("gui")]);
    if (useHelper)
        uiWidConfig.checkBox_helper->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_helper->setCheckState(Qt::Unchecked);
    uiWidConfig.lineEdit_helper->setText(paths[QString("helper")]);
    setHelper();
    uiWidConfig.lineEdit_netctl->setText(paths[QString("netctl")]);
    uiWidConfig.lineEdit_netctlAuto->setText(paths[QString("netctlAuto")]);
    if (useSudo)
        uiWidConfig.checkBox_sudo->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_sudo->setCheckState(Qt::Unchecked);
    uiWidConfig.lineEdit_sudo->setText(paths[QString("sudo")]);
    setSudo();
    if (useWifi)
        uiWidConfig.checkBox_wifi->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_wifi->setCheckState(Qt::Unchecked);
    uiWidConfig.lineEdit_wifi->setText(paths[QString("wifi")]);
    setWifi();
    if (bigInterface)
        uiWidConfig.checkBox_showBigInterface->setCheckState(Qt::Checked);
    else
        uiWidConfig.checkBox_showBigInterface->setCheckState(Qt::Unchecked);
    QString pattern = textPattern;
    uiWidConfig.textEdit->setPlainText(pattern.replace(QString("<br>"), QString("\n")));
    setBigInterface();

    KConfigGroup cg = config();
    QString textAlign = cg.readEntry("textAlign", "center");
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    QFont font = QFont(fontFamily, 12, 400, FALSE);
    uiAppConfig.comboBox_textAlign->setCurrentIndex(uiAppConfig.comboBox_textAlign->findText(textAlign));
    uiAppConfig.fontComboBox_font->setCurrentFont(font);
    uiAppConfig.spinBox_fontSize->setValue(fontSize);
    uiAppConfig.kcolorcombo_fontColor->setColor(fontColor);
    uiAppConfig.spinBox_fontWeight->setValue(fontWeight);
    uiAppConfig.comboBox_fontStyle->setCurrentIndex(uiAppConfig.comboBox_fontStyle->findText(fontStyle));
    uiAppConfig.lineEdit_activeIcon->setText(paths[QString("active")]);
    uiAppConfig.lineEdit_inactiveIcon->setText(paths[QString("inactive")]);

    QMap<QString, QString> deSettings = readDataEngineConfiguration();
    uiDEConfig.lineEdit_netctl->setText(deSettings[QString("NETCTLCMD")]);
    uiDEConfig.lineEdit_netctlAuto->setText(deSettings[QString("NETCTLAUTOCMD")]);
    if (deSettings[QString("EXTIP4")] == QString("true"))
        uiDEConfig.checkBox_extIp4->setCheckState(Qt::Checked);
    else
        uiDEConfig.checkBox_extIp4->setCheckState(Qt::Unchecked);
    uiDEConfig.lineEdit_extIp4->setText(deSettings[QString("EXTIP4CMD")]);
    setDataEngineExternalIp4();
    if (deSettings[QString("EXTIP6")] == QString("true"))
        uiDEConfig.checkBox_extIp6->setCheckState(Qt::Checked);
    else
        uiDEConfig.checkBox_extIp6->setCheckState(Qt::Unchecked);
    uiDEConfig.lineEdit_extIp6->setText(deSettings[QString("EXTIP6CMD")]);
    setDataEngineExternalIp6();

    QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
    for (int i=0; i<trdPartyList.count(); i++)
        trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                .arg(trdPartyList[i].split(QChar(','))[0])
                .arg(trdPartyList[i].split(QChar(','))[1])
                .arg(trdPartyList[i].split(QChar(','))[2]);
    uiAboutConfig.label_name->setText(QString(NAME));
    uiAboutConfig.label_version->setText(i18n("Version %1\n(build date %2)", QString(VERSION), QString(BUILD_DATE)));
    uiAboutConfig.label_description->setText(i18n("KDE widget which interacts with netctl."));
    uiAboutConfig.label_translators->setText(i18n("Translators: %1", QString(TRANSLATORS)));
    uiAboutConfig.label_links->setText(i18n("Links:") + QString("<br>") +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(i18n("Homepage")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(i18n("Repository")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(i18n("Bugtracker")) +
                                       QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(i18n("Translation issue")) +
                                       QString("<a href=\"%1\">%2</a>").arg(QString(AUR_PACKAGES)).arg(i18n("AUR packages")));
    uiAboutConfig.label_license->setText(QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR)) +
                                         i18n("This software is licensed under %1", QString(LICENSE)) + QString("<br>") +
                                         i18n("This software uses: %1", trdPartyList.join(QString(", "))) + QString("</small>"));

    parent->addPage(configWidget, i18n("Netctl plasmoid"), Applet::icon());
    parent->addPage(appWidget, i18n("Appearance"), QString("preferences-desktop-theme"));
    parent->addPage(deWidget, i18n("DataEngine"), Applet::icon());
    parent->addPage(aboutWidget, i18n("About"), QString("help-about"));

    connect(uiWidConfig.checkBox_helper, SIGNAL(stateChanged(int)), this, SLOT(setHelper()));
    connect(uiWidConfig.checkBox_showBigInterface, SIGNAL(stateChanged(int)), this,
            SLOT(setBigInterface()));
    connect(uiWidConfig.checkBox_sudo, SIGNAL(stateChanged(int)), this, SLOT(setSudo()));
    connect(uiWidConfig.checkBox_wifi, SIGNAL(stateChanged(int)), this, SLOT(setWifi()));
    connect(uiDEConfig.checkBox_extIp4, SIGNAL(stateChanged(int)), this, SLOT(setDataEngineExternalIp4()));
    connect(uiDEConfig.checkBox_extIp6, SIGNAL(stateChanged(int)), this, SLOT(setDataEngineExternalIp6()));

    connect(uiWidConfig.pushButton_gui, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiWidConfig.pushButton_helper, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiWidConfig.pushButton_netctl, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiWidConfig.pushButton_netctlAuto, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiWidConfig.pushButton_sudo, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiWidConfig.pushButton_wifi, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiAppConfig.pushButton_activeIcon, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiAppConfig.pushButton_inactiveIcon, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiDEConfig.pushButton_extIp4, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiDEConfig.pushButton_extIp6, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiDEConfig.pushButton_netctl, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));
    connect(uiDEConfig.pushButton_netctlAuto, SIGNAL(clicked()), this, SLOT(selectAbstractSomething()));

    connect(parent, SIGNAL(okClicked()), this, SLOT(configAccepted()));
    connect(parent, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
}


void Netctl::configAccepted()
{
    if (debug) qDebug() << PDEBUG;

    disconnectFromEngine();
    KConfigGroup cg = config();

    cg.writeEntry("autoUpdateInterval", uiWidConfig.spinBox_autoUpdate->value());
    cg.writeEntry("guiPath", uiWidConfig.lineEdit_gui->text());
    if (uiWidConfig.checkBox_helper->checkState() == 0)
        cg.writeEntry("useHelper", false);
    else
        cg.writeEntry("useHelper", true);
    cg.writeEntry("helperPath", uiWidConfig.lineEdit_helper->text());
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
    QString pattern = uiWidConfig.textEdit->toPlainText();
    pattern.replace(QString("\n"), QString("<br>"));
    cg.writeEntry("textPattern", pattern);

    cg.writeEntry("textAlign", uiAppConfig.comboBox_textAlign->currentText());
    cg.writeEntry("fontFamily", uiAppConfig.fontComboBox_font->currentFont().family());
    cg.writeEntry("fontSize", uiAppConfig.spinBox_fontSize->value());
    cg.writeEntry("fontColor", uiAppConfig.kcolorcombo_fontColor->color().name());
    cg.writeEntry("fontWeight", uiAppConfig.spinBox_fontWeight->value());
    cg.writeEntry("fontStyle", uiAppConfig.comboBox_fontStyle->currentText());
    cg.writeEntry("activeIconPath", uiAppConfig.lineEdit_activeIcon->text());
    cg.writeEntry("inactiveIconPath", uiAppConfig.lineEdit_inactiveIcon->text());

    QMap<QString, QString> deSettings;
    deSettings[QString("NETCTLCMD")] = uiDEConfig.lineEdit_netctl->text();
    deSettings[QString("NETCTLAUTOCMD")] = uiDEConfig.lineEdit_netctlAuto->text();
    if (uiDEConfig.checkBox_extIp4->checkState() == 0)
        deSettings[QString("EXTIP4")] = QString("false");
    else
        deSettings[QString("EXTIP4")] = QString("true");
    deSettings[QString("EXTIP4CMD")] = uiDEConfig.lineEdit_extIp4->text();
    if (uiDEConfig.checkBox_extIp6->checkState() == 0)
        deSettings[QString("EXTIP6")] = QString("false");
    else
        deSettings[QString("EXTIP6")] = QString("true");
    deSettings[QString("EXTIP6CMD")] = uiDEConfig.lineEdit_extIp6->text();
    writeDataEngineConfiguration(deSettings);
}


void Netctl::configChanged()
{
    if (debug) qDebug() << PDEBUG;

    KConfigGroup cg = config();

    autoUpdateInterval = cg.readEntry("autoUpdateInterval", 1000);
    paths[QString("gui")] = cg.readEntry("guiPath", "/usr/bin/netctl-gui");
    paths[QString("helper")] = cg.readEntry("helperPath", "/usr/bin/netctlgui-helper");
    paths[QString("netctl")] = cg.readEntry("netctlPath", "/usr/bin/netctl");
    paths[QString("netctlAuto")] = cg.readEntry("netctlAutoPath", "/usr/bin/netctl-auto");
    paths[QString("sudo")] = cg.readEntry("sudoPath", "/usr/bin/kdesu");
    paths[QString("wifi")] = cg.readEntry("wifiPath", "/usr/bin/netctl-gui -t 3");
    useSudo = cg.readEntry("useSudo", true);
    useWifi = cg.readEntry("useWifi", false);
    bigInterface = cg.readEntry("showBigInterface", true);
    useHelper = cg.readEntry("useHelper", true);
    textPattern = cg.readEntry("textPattern", "$current $status<br>IPv4: $intip4<br>IPv6: $intip6");

    QString textAlign = cg.readEntry("textAlign", "center");
    QString fontFamily = cg.readEntry("fontFamily", "Terminus");
    int fontSize = cg.readEntry("fontSize", 10);
    QString fontColor = cg.readEntry("fontColor", "#000000");
    int fontWeight = cg.readEntry("fontWeight", 400);
    QString fontStyle = cg.readEntry("fontStyle", "normal");
    paths[QString("active")] = cg.readEntry("activeIconPath",
                                            "/usr/share/icons/hicolor/64x64/apps/netctl-idle.png");
    paths[QString("inactive")] = cg.readEntry("inactiveIconPath",
                                              "/usr/share/icons/hicolor/64x64/apps/netctl-offline.png");

    formatLine[0] = QString("<html><head><meta name=\"qrichtext\" content=\"1\" />\
<style type=\"text/css\">p, li { white-space: pre-wrap; }</style>\
</head><body style=\"font-family:'%2'; font-size:%3pt; font-weight:%4; font-style:%5; color:%6;\"><p align=%1>")
            .arg(textAlign)
            .arg(fontFamily)
            .arg(QString::number(fontSize))
            .arg(QString::number(fontWeight))
            .arg(fontStyle)
            .arg(fontColor);
    formatLine[1] = QString("</p></body></html>");

    if (useHelper) startHelper();
    checkHelperStatus();
    connectToEngine();
}


void Netctl::setBigInterface()
{
    if (debug) qDebug() << PDEBUG;

    if (uiWidConfig.checkBox_showBigInterface->checkState() == 0)
        uiWidConfig.textEdit->setDisabled(true);
    else if (uiWidConfig.checkBox_showBigInterface->checkState() == 2)
        uiWidConfig.textEdit->setDisabled(false);
}


void Netctl::setDataEngineExternalIp4()
{
    if (debug) qDebug() << PDEBUG;

    if (uiDEConfig.checkBox_extIp4->checkState() == 0) {
        uiDEConfig.lineEdit_extIp4->setDisabled(true);
        uiDEConfig.pushButton_extIp4->setDisabled(true);
    } else if (uiDEConfig.checkBox_extIp4->checkState() == 2) {
        uiDEConfig.lineEdit_extIp4->setEnabled(true);
        uiDEConfig.pushButton_extIp4->setEnabled(true);
    }
}


void Netctl::setDataEngineExternalIp6()
{
    if (debug) qDebug() << PDEBUG;

    if (uiDEConfig.checkBox_extIp6->checkState() == 0) {
        uiDEConfig.lineEdit_extIp6->setDisabled(true);
        uiDEConfig.pushButton_extIp6->setDisabled(true);
    } else if (uiDEConfig.checkBox_extIp6->checkState() == 2) {
        uiDEConfig.lineEdit_extIp6->setEnabled(true);
        uiDEConfig.pushButton_extIp6->setEnabled(true);
    }
}


void Netctl::setHelper()
{
    if (uiWidConfig.checkBox_helper->checkState() == 0) {
        uiWidConfig.lineEdit_helper->setDisabled(true);
        uiWidConfig.pushButton_helper->setDisabled(true);
    } else if (uiWidConfig.checkBox_helper->checkState() == 2) {
        uiWidConfig.lineEdit_helper->setEnabled(true);
        uiWidConfig.pushButton_helper->setEnabled(true);
    }
}


void Netctl::setSudo()
{
    if (debug) qDebug() << PDEBUG;

    if (uiWidConfig.checkBox_sudo->checkState() == 0) {
        uiWidConfig.lineEdit_sudo->setDisabled(true);
        uiWidConfig.pushButton_sudo->setDisabled(true);
    } else if (uiWidConfig.checkBox_sudo->checkState() == 2) {
        uiWidConfig.lineEdit_sudo->setEnabled(true);
        uiWidConfig.pushButton_sudo->setEnabled(true);
    }
}


void Netctl::setWifi()
{
    if (debug) qDebug() << PDEBUG;

    if (uiWidConfig.checkBox_wifi->checkState() == 0) {
        uiWidConfig.lineEdit_wifi->setDisabled(true);
        uiWidConfig.pushButton_wifi->setDisabled(true);
    } else if (uiWidConfig.checkBox_wifi->checkState() == 2) {
        uiWidConfig.lineEdit_wifi->setEnabled(true);
        uiWidConfig.pushButton_wifi->setEnabled(true);
    }
}


#include "netctl.moc"
