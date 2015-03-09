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


#include "testnetctlprofile.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QtTest>

#include <netctlgui/netctlgui.h>

#include "version.h"


NetctlProfile *TestNetctlProfile::createNetctlProfileObj()
{
    QMap<QString, QString> settings = NetctlProfile::getRecommendedConfiguration();
    settings[QString("FORCE_SUDO")] = QString("true");
    NetctlProfile *netctl = new NetctlProfile(false, settings);

    return netctl;
}


bool TestNetctlProfile::createTestProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QMap<QString, QString> profileSettings;
    profileSettings["Connection"] = QString("dummy");
    profileSettings["Description"] = QString("\"Simple test profile\"");
    profileSettings["IP"] = QString("no");
    profileSettings["IP6"] = QString("no");
    profileSettings["Interface"] = QString("ngtest");
    bool status = netctl->copyProfile(netctl->createProfile(QString("netctlgui-test-dummy"), profileSettings));
    delete netctl;

    return status;
}


bool TestNetctlProfile::removeTestProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    bool status = netctl->removeProfile(QString("netctlgui-test-dummy"));
    delete netctl;

    return status;
}


QList<QVariant> TestNetctlProfile::sendDBusRequest(const QString path, const QString cmd, const QList<QVariant> args)
{
    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, path,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty()) request.setArguments(args);
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();

    return arguments;
}


void TestNetctlProfile::initTestCase()
{
    qDebug() << "netctlgui library tests";
    qDebug() << "NetctlProfile class tests";
    qDebug() << "TODO: unfortunately, some functions which is required to work";
    qDebug() << "with the working profile isn't tested here (including netctl-auto)";
    QWARN("Some functions requires root privileges");
    // arent needed
//    createTestProfile();
    if (sendDBusRequest(QString("/ctrl"), QString("Active")).isEmpty()) {
        helper = false;
        QWARN("Helper isn't active. DBus tests will be ignored");
    } else
        helper = true;
}


void TestNetctlProfile::cleanupTestCase()
{
    // arent needed
//    removeTestProfile();
}


void TestNetctlProfile::test_getRecommendedConfiguration()
{
    QStringList original;
    original.append(QString("FORCE_SUDO==false"));
    original.append(QString("PROFILE_DIR==/etc/netctl"));
    original.append(QString("SUDO_PATH==/usr/bin/sudo"));
    QMap<QString, QString> resultMap = NetctlProfile::getRecommendedConfiguration();
    QStringList result;
    for (int i=0; i<resultMap.keys().count(); i++)
        result.append(QString("%1==%2").arg(resultMap.keys()[i]).arg(resultMap[resultMap.keys()[i]]));

    QWARN("This test may fail on other configuration");
    QCOMPARE(result, original);
}


void TestNetctlProfile::test_copyProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QVERIFY(createTestProfile());
    if (helper) {
        QList<QVariant> args;
        args.append(QString("netctlgui-test-dummy"));
        QStringList profileSettings;
        profileSettings.append(QString("Connection==dummy"));
        profileSettings.append(QString("Description==\"Simple test profile\""));
        profileSettings.append(QString("IP==no"));
        profileSettings.append(QString("IP6==no"));
        profileSettings.append(QString("Interface==ngtest"));
        args.append(profileSettings);
        QVERIFY(sendDBusRequest(QString("/ctrl"), QString("Create"), args)[0].toBool());
    }
    delete netctl;
}


void TestNetctlProfile::test_getValueFromProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    // more specific test will be with profile examples
    QStringList original;
    original.append(QString("dummy"));
    original.append(QString("Simple test profile"));
    original.append(QString("no"));
    original.append(QString("no"));
    original.append(QString("ngtest"));
    QStringList result, dbus;
    result.append(netctl->getValueFromProfile(QString("netctlgui-test-dummy"),
                                              QString("Connection")));
    result.append(netctl->getValueFromProfile(QString("netctlgui-test-dummy"),
                                              QString("Description")));
    result.append(netctl->getValueFromProfile(QString("netctlgui-test-dummy"),
                                              QString("IP")));
    result.append(netctl->getValueFromProfile(QString("netctlgui-test-dummy"),
                                              QString("IP6")));
    result.append(netctl->getValueFromProfile(QString("netctlgui-test-dummy"),
                                              QString("Interface")));
    if (helper) {
        QList<QVariant> args;
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("Connection"));
        dbus.append(sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)[0].toString());
        args.clear();
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("Description"));
        dbus.append(sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)[0].toString());
        args.clear();
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("IP"));
        dbus.append(sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)[0].toString());
        args.clear();
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("IP6"));
        dbus.append(sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)[0].toString());
        args.clear();
        args.append(QString("netctlgui-test-dummy"));
        args.append(QString("Interface"));
        dbus.append(sendDBusRequest(QString("/netctl"), QString("ProfileValue"), args)[0].toString());
        QCOMPARE(dbus, result);
    }
    delete netctl;

    QCOMPARE(result, original);
}


void TestNetctlProfile::test_createProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QMap<QString, QString> profileSettings;
    // cat /etc/netctl/examples/* | sort | uniq | grep -v '^#' | less
    profileSettings["AccessPointName"] = QString("apn");
    profileSettings["Address6"] = QString("'2001:470:1f08:d87::2/64'");
    profileSettings["Address"] = QString("'192.168.1.23/24' '192.168.1.87/24'");
    profileSettings["BindsToInterfaces"] = QString("eth0 eth1 tap0");
    profileSettings["Connection"] = QString("bond");
    profileSettings["DNS"] = QString("'192.168.1.1'");
    profileSettings["DNSDomain"] = QString("\"mydomain.com\"");
    profileSettings["DNSSearch"] = QString("\"mydomain.com\"");
    profileSettings["Description"] = QString("\"netctlgui full test profile\"");
    profileSettings["ESSID"] = QString("'MyNetwork'");
    profileSettings["Gateway"] = QString("'192.168.1.1'");
    profileSettings["Group"] = QString("'nobody'");
    profileSettings["Hostname"] = QString("\"ponyhost\"");
    profileSettings["IP6"] = QString("static");
    profileSettings["IP"] = QString("dhcp");
    profileSettings["IPCustom"] = QString("'addr add dev eth0 192.168.1.23/24 brd +' 'route add default via 192.168.1.1'");
    profileSettings["Interface"] = QString("eth0");
    profileSettings["Key"] = QString("\\\"1234567890abcdef");
    profileSettings["MACAddress"] = QString("\"12:34:56:78:9a:bc\"");
    profileSettings["Mode"] = QString("'sit'");
    profileSettings["Password"] = QString("'very secret'");
    profileSettings["Remote"] = QString("'216.66.80.26'");
    profileSettings["Routes6"] = QString("'::/0'");
    profileSettings["Security"] = QString("none");
    profileSettings["User"] = QString("'example@yourprovider.com'");
    profileSettings["VLANID"] = QString("11");
    profileSettings["WPAConfigFile"] = QString("'/etc/wpa_supplicant/wpa_supplicant.conf'");
    profileSettings["WPAConfigSection"] = QString("\n\
'ssid=\"University\"'\n\
'key_mgmt=WPA-EAP'\n\
'eap=TTLS'\n\
'group=TKIP'\n\
'pairwise=TKIP CCMP'\n\
'anonymous_identity=\"anonymous\"'\n\
'identity=\"myusername\"'\n\
'password=\"mypassword\"'\n\
'priority=1'\n\
'phase2=\"auth=PAP\"'\n\
");
    QVERIFY(netctl->copyProfile(netctl->createProfile(QString("netctlgui-test-full"), profileSettings)));
    if (helper) {
        QList<QVariant> args;
        args.append(QString("netctlgui-test-full"));
        QStringList profileSettingsList;
        for (int i=0; i<profileSettings.keys().count(); i++)
            profileSettingsList.append(QString("%1==%2").arg(profileSettings.keys()[i]).arg(profileSettings[profileSettings.keys()[i]]));
        args.append(profileSettingsList);
        QVERIFY(sendDBusRequest(QString("/ctrl"), QString("Create"), args)[0].toBool());
    }
    delete netctl;
}


void TestNetctlProfile::test_getSettingsFromProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QStringList original;
    // cat /etc/netctl/examples/* | sort | uniq | grep -v '^#' | less
    original.append("AccessPointName==apn");
    original.append("Address==192.168.1.23/24\n\
192.168.1.87/24");
    original.append("Address6==2001:470:1f08:d87::2/64");
    original.append("BindsToInterfaces==eth0\n\
eth1\n\
tap0");
    original.append("Connection==bond");
    original.append("DNS==192.168.1.1");
    original.append("DNSDomain==mydomain.com");
    original.append("DNSSearch==mydomain.com");
    original.append("Description==netctlgui full test profile");
    original.append("ESSID==MyNetwork");
    original.append("Gateway==192.168.1.1");
    original.append("Group==nobody");
    original.append("Hostname==ponyhost");
    original.append("IP==dhcp");
    original.append("IP6==static");
    original.append("IPCustom==addr add dev eth0 192.168.1.23/24 brd +\n\
route add default via 192.168.1.1");
    original.append("Interface==eth0");
    original.append("Key==\"1234567890abcdef");
    original.append("MACAddress==12:34:56:78:9a:bc");
    original.append("Mode==sit");
    original.append("Password==very secret");
    original.append("Remote==216.66.80.26");
    original.append("Routes6==::/0");
    original.append("Security==none");
    original.append("User==example@yourprovider.com");
    original.append("VLANID==11");
    original.append("WPAConfigFile==/etc/wpa_supplicant/wpa_supplicant.conf");
    original.append("WPAConfigSection==ssid=\"University\"\n\
key_mgmt=WPA-EAP\n\
eap=TTLS\n\
group=TKIP\n\
pairwise=TKIP CCMP\n\
anonymous_identity=\"anonymous\"\n\
identity=\"myusername\"\n\
password=\"mypassword\"\n\
priority=1\n\
phase2=\"auth=PAP\"\
");
    QStringList result, dbus;
    if (helper) {
        QList<QVariant> args;
        args.append(QString("netctlgui-test-full"));
        dbus = sendDBusRequest(QString("/netctl"), QString("Profile"), args)[0].toStringList();
    }
    QMap<QString, QString> resultMap = netctl->getSettingsFromProfile(QString("netctlgui-test-full"));
    for (int i=0; i<resultMap.keys().count(); i++)
        result.append(QString("%1==%2").arg(resultMap.keys()[i]).arg(resultMap[resultMap.keys()[i]]));
    netctl->removeProfile(QString("netctlgui-test-full"));
    delete netctl;

    QCOMPARE(result, original);
    if (helper) QCOMPARE(dbus, result);
}


void TestNetctlProfile::test_removeProfile()
{
    NetctlProfile *netctl = createNetctlProfileObj();
    QVERIFY(removeTestProfile());
    delete netctl;
}


QTEST_MAIN(TestNetctlProfile);
