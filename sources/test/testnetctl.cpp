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


#include "testnetctl.h"

#include <QtTest>

#include <netctlgui/netctlgui.h>


void TestNetctl::test_getRecommendedConfiguration()
{
    QStringList original;
//    original.append(QString("CTRL_DIR==/run/wpa_supplicant_netctl-gui"));
//    original.append(QString("CTRL_GROUP==network"));
    original.append(QString("FORCE_SUDO==false"));
    original.append(QString("IFACE_DIR==/sys/class/net"));
    original.append(QString("NETCTLAUTO_PATH==/usr/bin/netctl-auto"));
    original.append(QString("NETCTLAUTO_SERVICE==netctl-auto"));
    original.append(QString("NETCTL_PATH==/usr/bin/netctl"));
//    original.append(QString("PID_FILE==/run/wpa_supplicant_netctl-gui.pid"));
    original.append(QString("PREFERED_IFACE==wifi0"));
    original.append(QString("PROFILE_DIR==/etc/netctl"));
    original.append(QString("SUDO_PATH==/usr/bin/sudo"));
    original.append(QString("SYSTEMCTL_PATH==/usr/bin/systemctl"));
//    original.append(QString("WPACLI_PATH==/usr/bin/wpa_cli"));
//    original.append(QString("WPASUP_PATH==/usr/bin/wpa_supplicant"));
//    original.append(QString("WPA_DRIVERS==nl80211,wext"));
    QMap<QString, QString> resultMap = Netctl::getRecommendedConfiguration();
    QStringList result;
    for (int i=0; i<resultMap.keys().count(); i++)
        result.append(resultMap.keys()[i] + QString("==") + resultMap[resultMap.keys()[i]]);

    QCOMPARE(result, original);
}


void TestNetctl::test_getActiveProfile()
{
    QMap<QString, QString> settings = Netctl::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    Netctl *netctl = new Netctl(false, settings);

    delete netctl;
}


QTEST_MAIN(TestNetctl)
