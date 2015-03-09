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

#include <QDBusConnection>
#include <QDBusMessage>
#include <QtTest>

#include <netctlgui/netctlgui.h>

#include "version.h"


Netctl *TestNetctl::createNetctlObj()
{
    QMap<QString, QString> settings = Netctl::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    settings[QString("PREFERED_IFACE")] = QString("ngtest");
    Netctl *netctl = new Netctl(false, settings);

    return netctl;
}


NetctlProfile *TestNetctl::createNetctlProfileObj()
{
    QMap<QString, QString> settings = NetctlProfile::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    NetctlProfile *netctl = new NetctlProfile(false, settings);

    return netctl;
}


void TestNetctl::createTestProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QMap<QString, QString> profileSettings;
    profileSettings["Connection"] = QString("dummy");
    profileSettings["Description"] = QString("\"Simple test profile\"");
    profileSettings["IP"] = QString("no");
    profileSettings["IP6"] = QString("no");
    profileSettings["Interface"] = QString("ngtest");
    netctl->copyProfile(netctl->createProfile(QString("netctlgui-test-dummy"), profileSettings));
    delete netctl;
}


void TestNetctl::removeTestProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    netctl->removeProfile(QString("netctlgui-test-dummy"));
    delete netctl;
}


QList<QVariant> TestNetctl::sendDBusRequest(const QString path, const QString cmd, const QList<QVariant> args)
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, path,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty())
        request.setArguments(args);
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();

    return arguments;
}


void TestNetctl::initTestCase()
{
    qDebug() << "netctlgui library tests";
    qDebug() << "Netctl class tests (netctl functions)";
    qDebug() << "TODO: unfortunately, some functions which is required to work";
    qDebug() << "with the working profile isn't tested here (including netctl-auto)";
    QWARN("Some functions requires root privileges");
    createTestProfile();
    if (sendDBusRequest(QString("/ctrl"), QString("Active")).isEmpty()) {
        helper = false;
        QWARN("Helper isn't active. DBus tests will be ignored");
    } else
        helper = true;
}


void TestNetctl::cleanupTestCase()
{
    removeTestProfile();
}


void TestNetctl::test_getRecommendedConfiguration()
{
    QStringList original;
    original.append(QString("FORCE_SUDO==false"));
    original.append(QString("IFACE_DIR==/sys/class/net"));
    original.append(QString("NETCTLAUTO_PATH==/usr/bin/netctl-auto"));
    original.append(QString("NETCTLAUTO_SERVICE==netctl-auto"));
    original.append(QString("NETCTL_PATH==/usr/bin/netctl"));
    original.append(QString("PREFERED_IFACE==wifi0"));
    original.append(QString("PROFILE_DIR==/etc/netctl"));
    original.append(QString("SUDO_PATH==/usr/bin/sudo"));
    original.append(QString("SYSTEMCTL_PATH==/usr/bin/systemctl"));
    QMap<QString, QString> resultMap = Netctl::getRecommendedConfiguration();
    QStringList result;
    for (int i=0; i<resultMap.keys().count(); i++)
        result.append(QString("%1==%2").arg(resultMap.keys()[i]).arg(resultMap[resultMap.keys()[i]]));

    QWARN("This test may fail on other configuration");
    QCOMPARE(result, original);
}


void TestNetctl::test_getActiveProfile()
{
    Netctl *netctl = createNetctlObj();
    netctl->startProfile(QString("netctlgui-test-dummy"));
    QStringList result = netctl->getActiveProfile();
    QStringList dbus;
    if (helper)
        dbus = sendDBusRequest(QString("/netctl"), QString("netctlActiveProfile"))[0].toString().split(QChar('|'));
    netctl->startProfile(QString("netctlgui-test-dummy"));
    delete netctl;

    QVERIFY(result.contains(QString("netctlgui-test-dummy")));
    if (helper) QCOMPARE(dbus, result);
}


void TestNetctl::test_getProfileDescription()
{
    Netctl *netctl = createNetctlObj();
    QString original = QString("Simple test profile");
    QString result = netctl->getProfileDescription(QString("netctlgui-test-dummy"));
    QString dbus;
    if (helper) {
        QList<QVariant> args;
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("Description"));
        dbus = sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)
                [0].toString();
    }
    delete netctl;

    QCOMPARE(result, original);
    if (helper) QCOMPARE(dbus, result);
}


void TestNetctl::test_getProfileStatus()
{
    Netctl *netctl = createNetctlObj();
    QStringList original;
    original.append(QString("inactive (static)"));
    original.append(QString("active (static)"));
    original.append(QString("active (enabled)"));
    original.append(QString("inactive (enabled)"));
    original.append(QString("inactive (static)"));
    QStringList result;
    result.append(netctl->getProfileStatus(QString("netctlgui-test-dummy")));
    netctl->startProfile(QString("netctlgui-test-dummy"));
    result.append(netctl->getProfileStatus(QString("netctlgui-test-dummy")));
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    result.append(netctl->getProfileStatus(QString("netctlgui-test-dummy")));
    netctl->startProfile(QString("netctlgui-test-dummy"));
    result.append(netctl->getProfileStatus(QString("netctlgui-test-dummy")));
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    result.append(netctl->getProfileStatus(QString("netctlgui-test-dummy")));
    delete netctl;

    QCOMPARE(result, original);
}


void TestNetctl::test_isProfileActive()
{
    Netctl *netctl = createNetctlObj();
    QList<QVariant> args;
    args.append(QString("netctlgui-test-dummy"));
    QVERIFY(!netctl->isProfileActive(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(!sendDBusRequest(QString("/netctl"), QString("isProfileActive"), args)[0].toBool());
    netctl->startProfile(QString("netctlgui-test-dummy"));
    QVERIFY(netctl->isProfileActive(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileActive"), args)[0].toBool());
    netctl->startProfile(QString("netctlgui-test-dummy"));
    delete netctl;
}


void TestNetctl::test_isProfileEnabled()
{
    Netctl *netctl = createNetctlObj();
    QList<QVariant> args;
    args.append(QString("netctlgui-test-dummy"));
    QVERIFY(!netctl->isProfileEnabled(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(!sendDBusRequest(QString("/netctl"), QString("isProfileEnabled"), args)[0].toBool());
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    QVERIFY(netctl->isProfileEnabled(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileEnabled"), args)[0].toBool());
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    delete netctl;
}


void TestNetctl::test_reenableProfile()
{
    Netctl *netctl = createNetctlObj();
    QList<QVariant> args;
    args.append(QString("netctlgui-test-dummy"));
    QVERIFY(!netctl->isProfileEnabled(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(!sendDBusRequest(QString("/netctl"), QString("isProfileEnabled"), args)[0].toBool());
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    QVERIFY(netctl->isProfileEnabled(QString("netctlgui-test-dummy")));
    QVERIFY(netctl->reenableProfile(QString("netctlgui-test-dummy")));
    QVERIFY(netctl->isProfileEnabled(QString("netctlgui-test-dummy")));
    if (helper) {
        QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileEnabled"), args)[0].toBool());
        QVERIFY(sendDBusRequest(QString("/ctrl"), QString("Reenable"), args)[0].toBool());
        QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileEnabled"), args)[0].toBool());
    }
    netctl->enableProfile(QString("netctlgui-test-dummy"));
    delete netctl;
}


void TestNetctl::test_restartProfile()
{
    Netctl *netctl = createNetctlObj();
    QList<QVariant> args;
    args.append(QString("netctlgui-test-dummy"));
    QVERIFY(!netctl->isProfileActive(QString("netctlgui-test-dummy")));
    if (helper) QVERIFY(!sendDBusRequest(QString("/netctl"), QString("isProfileActive"), args)[0].toBool());
    netctl->startProfile(QString("netctlgui-test-dummy"));
    QVERIFY(netctl->isProfileActive(QString("netctlgui-test-dummy")));
    QVERIFY(netctl->restartProfile(QString("netctlgui-test-dummy")));
    QVERIFY(netctl->isProfileActive(QString("netctlgui-test-dummy")));
    if (helper) {
        QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileActive"), args)[0].toBool());
        QVERIFY(sendDBusRequest(QString("/ctrl"), QString("Restart"), args)[0].toBool());
        QVERIFY(sendDBusRequest(QString("/netctl"), QString("isProfileActive"), args)[0].toBool());
    }
    netctl->startProfile(QString("netctlgui-test-dummy"));
    delete netctl;
}


QTEST_MAIN(TestNetctl);
