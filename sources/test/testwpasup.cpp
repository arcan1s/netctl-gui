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

#include <QtTest>

#include <netctlgui/netctlgui.h>


WpaSup *TestWpaSup::createWpaSupObj()
{
    QMap<QString, QString> settings = WpaSup::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    WpaSup *wpasup = new WpaSup(false, settings);

    return wpasup;
}


void TestWpaSup::initTestCase()
{
    qDebug() << "netctlgui library tests";
    qDebug() << "WpaSup class tests";
    qDebug() << "TODO: unfortunately, some functions which is required to work";
    qDebug() << "with the working profile isn't tested here (including netctl-auto)";
    QWARN("Some functions requires root privileges");
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
        result.append(resultMap.keys()[i] + QString("==") + resultMap[resultMap.keys()[i]]);

    QWARN("This test may fail on other configuration");
    QCOMPARE(result, original);
}


QTEST_MAIN(TestWpaSup);
