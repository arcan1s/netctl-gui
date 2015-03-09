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


#include "testwpasup.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QtTest>

#include <netctlgui/netctlgui.h>

#include "version.h"


WpaSup *TestWpaSup::createWpaSupObj()
{
    QMap<QString, QString> settings = WpaSup::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    settings[QString("PREFERED_IFACE")] = QString("ngtest");
    WpaSup *wpasup = new WpaSup(false, settings);

    return wpasup;
}


QList<QVariant> TestWpaSup::sendDBusRequest(const QString path, const QString cmd, const QList<QVariant> args)
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, path,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty()) request.setArguments(args);
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();

    return arguments;
}


void TestWpaSup::initTestCase()
{
    qDebug() << "netctlgui library tests";
    qDebug() << "WpaSup class tests";
    qDebug() << "TODO: unfortunately, some functions which is required to work";
    qDebug() << "with the working profile isn't tested here (including netctl-auto)";
    QWARN("Some functions requires root privileges");
    if (sendDBusRequest(QString("/ctrl"), QString("Active")).isEmpty()) {
        helper = false;
        QWARN("Helper isn't active. DBus tests will be ignored");
    } else
        helper = true;
}


void TestWpaSup::cleanupTestCase()
{
}


void TestWpaSup::test_getRecommendedConfiguration()
{
    QStringList original;
    original.append(QString("CTRL_DIR==/run/wpa_supplicant_netctl-gui"));
    original.append(QString("CTRL_GROUP==network"));
    original.append(QString("FORCE_SUDO==false"));
    original.append(QString("PID_FILE==/run/wpa_supplicant_netctl-gui.pid"));
    original.append(QString("SUDO_PATH==/usr/bin/sudo"));
    original.append(QString("WPACLI_PATH==/usr/bin/wpa_cli"));
    original.append(QString("WPASUP_PATH==/usr/bin/wpa_supplicant"));
    original.append(QString("WPA_DRIVERS==nl80211,wext"));
    QMap<QString, QString> resultMap = WpaSup::getRecommendedConfiguration();
    QStringList result;
    for (int i=0; i<resultMap.keys().count(); i++)
        result.append(QString("%1==%2").arg(resultMap.keys()[i]).arg(resultMap[resultMap.keys()[i]]));

    QWARN("This test may fail on other configuration");
    QCOMPARE(result, original);
}


QTEST_MAIN(TestWpaSup);
