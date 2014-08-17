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


#include "testnetctlauto.h"

#include <QtTest>

#include <netctlgui/netctlgui.h>


Netctl *TestNetctlAuto::createNetctlObj()
{
    QMap<QString, QString> settings = Netctl::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    // to test netctl-auto with dummy profiles
    settings[QString("PREFERED_IFACE")] = QString("ngtest");
    Netctl *netctl = new Netctl(false, settings);

    return netctl;
}


NetctlProfile *TestNetctlAuto::createNetctlProfileObj()
{
    QMap<QString, QString> settings = NetctlProfile::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    NetctlProfile *netctl = new NetctlProfile(false, settings);

    return netctl;
}


void TestNetctlAuto::createTestProfiles()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QMap<QString, QString> profileSettings;
    profileSettings["Connection"] = QString("dummy");
    profileSettings["Description"] = QString("\"Simple test profile\"");
    profileSettings["IP"] = QString("no");
    profileSettings["IP6"] = QString("no");
    profileSettings["Interface"] = QString("ngtest");
    netctl->copyProfile(netctl->createProfile(QString("netctlgui-test-dummy"), profileSettings));
    profileSettings["Connection"] = QString("dummy");
    profileSettings["Description"] = QString("\"Second simple test profile\"");
    profileSettings["IP"] = QString("no");
    profileSettings["IP6"] = QString("no");
    profileSettings["Interface"] = QString("ngtest");
    netctl->copyProfile(netctl->createProfile(QString("netctlgui-test-dummy-snd"), profileSettings));
    delete netctl;
}


void TestNetctlAuto::removeTestProfiles()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    netctl->removeProfile(QString("netctlgui-test-dummy"));
    netctl->removeProfile(QString("netctlgui-test-dummy-snd"));
    delete netctl;
}


void TestNetctlAuto::initTestCase()
{
    qDebug() << "netctlgui library tests";
    qDebug() << "TODO: unfortunately, some functions which is required to work";
    qDebug() << "with the working profile isn't tested here (including netctl-auto)";
    QWARN("Some functions requires root privileges");
}


QTEST_MAIN(TestNetctlAuto);
