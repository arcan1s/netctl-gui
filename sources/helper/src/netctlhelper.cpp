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

#include "netctlhelper.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>
#include <QSettings>
#include <unistd.h>

#include <listmap/listmap.h>
#include <netctlgui/netctlgui.h>
#include <pdebug/pdebug.h>

#include "controladaptor.h"
#include "interfaceadaptor.h"
#include "netctladaptor.h"
#include "version.h"


NetctlHelper::NetctlHelper(QObject *parent, QMap<QString, QVariant> args)
    : QObject(parent),
      configPath(args[QString("config")].toString()),
      debug(args[QString("debug")].toBool()),
      session(args[QString("session")].toBool()),
      system(args[QString("system")].toBool())
{
    updateConfiguration();
    if (!args[QString("nodaemon")].toBool()) daemon(0, 0);
}


NetctlHelper::~NetctlHelper()
{
    if (debug) qDebug() << PDEBUG;

    deleteInterface();
}


QStringList NetctlHelper::printSettings()
{
    if (debug) qDebug() << PDEBUG;

    return mapToList(configuration);
}


void NetctlHelper::createInterface()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.registerService(DBUS_HELPER_SERVICE)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register service";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
        return quitHelper();
    }
    if (!bus.registerObject(DBUS_LIB_PATH,
                            new NetctlAdaptor(this, debug, configuration),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register library object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
        return quitHelper();
    }
    if (!bus.registerObject(DBUS_CTRL_PATH,
                            new ControlAdaptor(this, debug, configuration),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register control object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
        return quitHelper();
    }
    if (!bus.registerObject(DBUS_INTERFACE_PATH,
                            new InterfaceAdaptor(this, debug, configuration),
                            QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register interface object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
        return quitHelper();
    }
    // session bus
    if (!session) return;
    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.registerService(DBUS_HELPER_SERVICE)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register session service";
        if (debug) qDebug() << PDEBUG << ":" << sessionBus.lastError().message();
    }
    if (!sessionBus.registerObject(DBUS_LIB_PATH,
                                   new NetctlAdaptor(this, debug, configuration),
                                   QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register session library object";
        if (debug) qDebug() << PDEBUG << ":" << sessionBus.lastError().message();
    }
    if (!sessionBus.registerObject(DBUS_CTRL_PATH,
                                   new ControlAdaptor(this, debug, configuration),
                                   QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register session control object";
        if (debug) qDebug() << PDEBUG << ":" << sessionBus.lastError().message();
    }
    if (!sessionBus.registerObject(DBUS_INTERFACE_PATH,
                                   new InterfaceAdaptor(this, debug, configuration),
                                   QDBusConnection::ExportAllContents)) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not register session interface object";
        if (debug) qDebug() << PDEBUG << ":" << bus.lastError().message();
        return quitHelper();
    }
}


void NetctlHelper::deleteInterface()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection::systemBus().unregisterObject(DBUS_LIB_PATH);
    QDBusConnection::systemBus().unregisterObject(DBUS_CTRL_PATH);
    QDBusConnection::systemBus().unregisterObject(DBUS_INTERFACE_PATH);
    QDBusConnection::systemBus().unregisterService(DBUS_HELPER_SERVICE);
    // session bus
    if (!session) return;
    QDBusConnection::sessionBus().unregisterObject(DBUS_LIB_PATH);
    QDBusConnection::sessionBus().unregisterObject(DBUS_CTRL_PATH);
    QDBusConnection::sessionBus().unregisterObject(DBUS_INTERFACE_PATH);
    QDBusConnection::sessionBus().unregisterService(DBUS_HELPER_SERVICE);
}


QMap<QString, QString> NetctlHelper::getSettings(const QString file)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> config;
    QSettings settings(file, QSettings::IniFormat);

    settings.beginGroup(QString("Common"));
    config[QString("LANGUAGE")] = settings.value(QString("LANGUAGE"), QString("en")).toString();
    config[QString("SYSTRAY")] = settings.value(QString("SYSTRAY"), QString("true")).toString();
    config[QString("CLOSETOTRAY")] = settings.value(QString("CLOSETOTRAY"), QString("true")).toString();
    config[QString("STARTTOTRAY")] = settings.value(QString("STARTTOTRAY"), QString("false")).toString();
    config[QString("SKIPCOMPONENTS")] = settings.value(QString("SKIPCOMPONENTS"), QString("false")).toString();
    settings.endGroup();

    settings.beginGroup(QString("Helper"));
    config[QString("USE_HELPER")] = settings.value(QString("USE_HELPER"), QString("true")).toString();
    config[QString("FORCE_SUDO")] = settings.value(QString("FORCE_SUDO"), QString("false")).toString();
    config[QString("CLOSE_HELPER")] = settings.value(QString("CLOSE_HELPER"), QString("false")).toString();
    config[QString("HELPER_PATH")] = settings.value(QString("HELPER_PATH"), QString("/usr/bin/netctlgui-helper")).toString();
    config[QString("HELPER_SERVICE")] = settings.value(QString("HELPER_SERVICE"), QString("netctlgui-helper.service")).toString();
    settings.endGroup();

    settings.beginGroup(QString("netctl"));
    config[QString("SYSTEMCTL_PATH")] = settings.value(QString("SYSTEMCTL_PATH"), QString("/usr/bin/systemctl")).toString();
    config[QString("NETCTL_PATH")] = settings.value(QString("NETCTL_PATH"), QString("/usr/bin/netctl")).toString();
    config[QString("NETCTLAUTO_PATH")] = settings.value(QString("NETCTLAUTO_PATH"), QString("/usr/bin/netctl-auto")).toString();
    config[QString("NETCTLAUTO_SERVICE")] = settings.value(QString("NETCTLAUTO_SERVICE"), QString("netctl-auto")).toString();
    config[QString("PROFILE_DIR")] = settings.value(QString("PROFILE_DIR"), QString("/etc/netctl")).toString();
    settings.endGroup();

    settings.beginGroup(QString("sudo"));
    config[QString("SUDO_PATH")] = settings.value(QString("SUDO_PATH"), QString("/usr/bin/kdesu")).toString();
    settings.endGroup();

    settings.beginGroup(QString("wpa_supplicant"));
    config[QString("WPASUP_PATH")] = settings.value(QString("WPASUP_PATH"), QString("/usr/bin/wpa_supplicant")).toString();
    config[QString("WPACLI_PATH")] = settings.value(QString("WPACLI_PATH"), QString("/usr/bin/wpa_cli")).toString();
    config[QString("PID_FILE")] = settings.value(QString("PID_FILE"), QString("/run/wpa_supplicant_$i.pid")).toString();
    config[QString("WPA_DRIVERS")] = settings.value(QString("WPA_DRIVERS"), QString("nl80211,wext")).toString();
    config[QString("CTRL_DIR")] = settings.value(QString("CTRL_DIR"), QString("/run/wpa_supplicant")).toString();
    config[QString("CTRL_GROUP")] = settings.value(QString("CTRL_GROUP"), QString("users")).toString();
    settings.endGroup();

    settings.beginGroup(QString("Other"));
    config[QString("EDITOR_PATH")] = settings.value(QString("EDITOR_PATH"), QString("/usr/bin/gvim")).toString();
    config[QString("IFACE_DIR")] = settings.value(QString("IFACE_DIR"), QString("/sys/class/net/")).toString();
    config[QString("RFKILL_DIR")] = settings.value(QString("RFKILL_DIR"), QString("/sys/class/rfkill/")).toString();
    config[QString("PREFERED_IFACE")] = settings.value(QString("PREFERED_IFACE"), QString("")).toString();
    settings.endGroup();

    settings.beginGroup(QString("Toolbars"));
    config[QString("MAIN_TOOLBAR")] = settings.value(QString("MAIN_TOOLBAR"), Qt::TopToolBarArea).toString();
    config[QString("NETCTL_TOOLBAR")] = settings.value(QString("NETCTL_TOOLBAR"), Qt::TopToolBarArea).toString();
    config[QString("NETCTLAUTO_TOOLBAR")] = settings.value(QString("NETCTLAUTO_TOOLBAR"), Qt::TopToolBarArea).toString();
    config[QString("PROFILE_TOOLBAR")] = settings.value(QString("PROFILE_TOOLBAR"), Qt::TopToolBarArea).toString();
    config[QString("WIFI_TOOLBAR")] = settings.value(QString("WIFI_TOOLBAR"), Qt::TopToolBarArea).toString();
    settings.endGroup();

    for (int i=0; i<config.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << QString("%1=%2").arg(config.keys()[i]).arg(config[config.keys()[i]]);

    return config;
}


void NetctlHelper::updateConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    deleteInterface();
    if (system)
        configuration = getSettings(QString("/etc/netctl-gui.conf"));
    else
        configuration = getSettings(configPath);
    createInterface();
}


void NetctlHelper::quitHelper()
{
    if (debug) qDebug() << PDEBUG;

    QCoreApplication::instance()->quit();
}
