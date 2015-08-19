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
/**
 * @file netctlinterface.cpp
 * Source code of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#include <QDebug>

#include <netctlgui/netctlinterface.h>
#include <pdebug/pdebug.h>


/**
 * @class NetctlInterface
 */
/**
 * @fn NetctlInterface
 */
NetctlInterface::NetctlInterface(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
    netctlCommand = new Netctl(debug, settings);
    netctlProfile = new NetctlProfile(debug, settings);
    wpaCommand = new WpaSup(debug, settings);
}


/**
 * @fn ~NetctlInterface
 */
NetctlInterface::~NetctlInterface()
{
    if (debug) qDebug() << PDEBUG;

    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
    if (wpaCommand != nullptr) delete wpaCommand;
}


/**
 * @fn connectToEssid
 */
InterfaceAnswer NetctlInterface::connectToEssid(const QString essid,
                                                QMap<QString, QString> settings) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }
    if (wpaCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    QString profile = wpaCommand->existentProfile(essid);
    if (profile.isEmpty())
        return connectToUnknownEssid(essid, settings);
    else
        return startProfile(profile);
}


/**
 * @fn connectToKnownEssid
 */
InterfaceAnswer NetctlInterface::connectToKnownEssid(const QString essid) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }
    if (wpaCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    QString profile = wpaCommand->existentProfile(essid);
    if (profile.isEmpty()) return InterfaceAnswer::Error;

    return startProfile(profile);
}


/**
 * @fn connectToUnknownEssid
 */
InterfaceAnswer NetctlInterface::connectToUnknownEssid(const QString essid,
                                                       QMap<QString, QString> settings) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }
    if (wpaCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    // append settings
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) return InterfaceAnswer::Error;
    settings[QString("Description")] = QString("'Automatically generated profile by Netctl GUI'");
    settings[QString("Interface")] = interfaces.first();
    settings[QString("Connection")] = QString("wireless");
    settings[QString("ESSID")] = QString("'%1'").arg(essid);
    settings[QString("IP")] = QString("dhcp");

    // save profile
    QString profile = QString("netctl-gui-%1").arg(essid);
    profile.remove(QChar('"')).remove(QChar('\''));
    if (createProfile(profile, settings) != InterfaceAnswer::True) return InterfaceAnswer::Error;

    // start it
    return startProfile(profile);
}


/**
 * @fn createProfile
 */
InterfaceAnswer NetctlInterface::createProfile(const QString profile,
                                               const QMap<QString, QString> settings) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    QString profileTempName = netctlProfile->createProfile(profile, settings);
    if (profileTempName.isEmpty()) return InterfaceAnswer::Error;

    return static_cast<InterfaceAnswer>(netctlProfile->copyProfile(profileTempName));
}


/**
 * @fn enableProfile
 */
InterfaceAnswer NetctlInterface::enableProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    netctlCommand->enableProfile(profile);

    return static_cast<InterfaceAnswer>(netctlCommand->isProfileEnabled(profile));
}


/**
 * @fn removeProfile
 */
InterfaceAnswer NetctlInterface::removeProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlProfile == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    return static_cast<InterfaceAnswer>(netctlProfile->removeProfile(profile));
}


/**
 * @fn restartProfile
 */
InterfaceAnswer NetctlInterface::restartProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    netctlCommand->restartProfile(profile);

    return static_cast<InterfaceAnswer>(netctlCommand->isProfileActive(profile));
}


/**
 * @fn startProfile
 */
InterfaceAnswer NetctlInterface::startProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    QStringList current = netctlCommand->getActiveProfile();
    if ((current.isEmpty()) || (current.contains(profile)))
        netctlCommand->startProfile(profile);
    else
        netctlCommand->switchToProfile(profile);

    return static_cast<InterfaceAnswer>(netctlCommand->isProfileActive(profile));
}


/**
 * @fn stopAllProfiles
 */
InterfaceAnswer NetctlInterface::stopAllProfiles() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    return static_cast<InterfaceAnswer>(netctlCommand->stopAllProfiles());
}


/**
 * @fn switchToProfile
 */
InterfaceAnswer NetctlInterface::switchToProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return InterfaceAnswer::Error;
    }

    InterfaceAnswer status = InterfaceAnswer::Error;
    bool netctlAutoStatus = netctlCommand->isNetctlAutoRunning();
    if (netctlAutoStatus) {
        netctlCommand->autoStartProfile(profile);
        status = static_cast<InterfaceAnswer>(netctlCommand->autoIsProfileActive(profile));
    } else {
        netctlCommand->switchToProfile(profile);
        status = static_cast<InterfaceAnswer>(netctlCommand->isProfileActive(profile));
    }

    return status;
}


/**
 * @fn information
 */
netctlInformation NetctlInterface::information() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return netctlInformation();
    }

    netctlInformation info;
    info.netctlAuto = netctlCommand->isNetctlAutoRunning();
    info.netctlAutoEnabled = netctlCommand->isNetctlAutoEnabled();
    info.netctlProfiles = netctlCommand->getProfileList();
    if (info.netctlAuto) info.netctlAutoProfiles = netctlCommand->getProfileListFromNetctlAuto();

    return info;
}


/**
 * @fn profileSettings
 */
QMap<QString, QString> NetctlInterface::profileSettings(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlProfile == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QMap<QString, QString>();
    }

    return netctlProfile->getSettingsFromProfile(profile);
}


/**
 * @fn status
 */
netctlCurrent NetctlInterface::status() const
{
    if (debug) qDebug() << PDEBUG;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return netctlCurrent();
    }

    netctlCurrent current;
    current.netctlAuto = netctlCommand->isNetctlAutoRunning();
    QList<netctlProfileInfo> profiles;
    if (current.netctlAuto)
        profiles = netctlCommand->getProfileListFromNetctlAuto();
    else
        profiles = netctlCommand->getProfileList();
    foreach(netctlProfileInfo profile, profiles) {
        current.profiles.append(profile.name);
        if (!profile.active) continue;
        current.current.append(profile.name);
        current.enables.append(profile.enabled);
    }

    return current;
}
