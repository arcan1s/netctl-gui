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
#include <unistd.h>

#include <netctlgui/netctlgui.h>
#include <pdebug/pdebug.h>

#include "controladaptor.h"
#include "netctladaptor.h"
#include "version.h"


NetctlHelper::NetctlHelper(QObject *parent, QMap<QString, QVariant> args)
    : QObject(parent),
      configPath(args[QString("config")].toString()),
      debug(args[QString("debug")].toBool()),
      system(args[QString("system")].toBool())
{
    updateConfiguration();
    if (!args[QString("nodaemon")].toBool())
        daemon(0, 0);
}


NetctlHelper::~NetctlHelper()
{
    if (debug) qDebug() << PDEBUG;

    deleteInterface();
}


QStringList NetctlHelper::printSettings()
{
    if (debug) qDebug() << PDEBUG;

    QStringList settingsList;
    for (int i=0; i<configuration.keys().count(); i++)
        settingsList.append(configuration.keys()[i] + QString("==") +
                            configuration[configuration.keys()[i]]);

    return settingsList;
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
}


void NetctlHelper::deleteInterface()
{
    if (debug) qDebug() << PDEBUG;

    QDBusConnection::systemBus().unregisterObject(DBUS_LIB_PATH);
    QDBusConnection::systemBus().unregisterObject(DBUS_CTRL_PATH);
    QDBusConnection::systemBus().unregisterService(DBUS_HELPER_SERVICE);
}


QMap<QString, QString> NetctlHelper::getDefault()
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    settings[QString("CLOSE_HELPER")] = QString("false");
    settings[QString("CLOSETOTRAY")] = QString("true");
    settings[QString("CTRL_DIR")] = QString("/run/wpa_supplicant_netctl-gui");
    settings[QString("CTRL_GROUP")] = QString("users");
    settings[QString("FORCE_SUDO")] = QString("false");
    settings[QString("HELPER_PATH")] = QString("/usr/bin/netctlgui-helper");
    settings[QString("HELPER_SERVICE")] = QString("netctlgui-helper.service");
    settings[QString("IFACE_DIR")] = QString("/sys/class/net/");
    settings[QString("LANGUAGE")] = QString("en");
    settings[QString("NETCTL_PATH")] = QString("/usr/bin/netctl");
    settings[QString("NETCTLAUTO_PATH")] = QString("/usr/bin/netctl-auto");
    settings[QString("NETCTLAUTO_SERVICE")] = QString("netctl-auto");
    settings[QString("PID_FILE")] = QString("/run/wpa_supplicant_netctl-gui.pid");
    settings[QString("PREFERED_IFACE")] = QString("");
    settings[QString("PROFILE_DIR")] = QString("/etc/netctl/");
    settings[QString("RFKILL_DIR")] = QString("/sys/class/rfkill/");
    settings[QString("STARTTOTRAY")] = QString("false");
    settings[QString("SUDO_PATH")] = QString("/usr/bin/kdesu");
    settings[QString("SYSTEMCTL_PATH")] = QString("/usr/bin/systemctl");
    settings[QString("SYSTRAY")] = QString("true");
    settings[QString("USE_HELPER")] = QString("true");
    settings[QString("WPACLI_PATH")] = QString("/usr/bin/wpa_cli");
    settings[QString("WPASUP_PATH")] = QString("/usr/bin/wpa_supplicant");
    settings[QString("WPA_DRIVERS")] = QString("nl80211,wext");
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


QMap<QString, QString> NetctlHelper::getSettings(const QString file, const QMap<QString, QString> existing)
{
    if (debug) qDebug() << PDEBUG;

    QMap<QString, QString> settings;
    if (existing.isEmpty())
        settings = getDefault();
    else
        settings = existing;
    QFile configFile(file);
    QString fileStr;
    if (!configFile.open(QIODevice::ReadOnly))
        return settings;
    while (true) {
        fileStr = QString(configFile.readLine()).trimmed();
        if ((fileStr.isEmpty()) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar('#')) && (!configFile.atEnd())) continue;
        if ((fileStr[0] == QChar(';')) && (!configFile.atEnd())) continue;
        if (fileStr.contains(QChar('=')))
            settings[fileStr.split(QChar('='))[0]] = fileStr.split(QChar('='))[1];
        if (configFile.atEnd()) break;
    }
    configFile.close();
    for (int i=0; i<settings.keys().count(); i++)
        if (debug) qDebug() << PDEBUG << ":" << settings.keys()[i] + QString("=") + settings[settings.keys()[i]];

    return settings;
}


void NetctlHelper::updateConfiguration()
{
    if (debug) qDebug() << PDEBUG;

    deleteInterface();
    configuration = getSettings(QString("/etc/netctlgui-helper.conf"));
    if (!system)
        configuration = getSettings(configPath, configuration);
    createInterface();
}


void NetctlHelper::quitHelper()
{
    if (debug) qDebug() << PDEBUG;

    QCoreApplication::instance()->quit();
}
