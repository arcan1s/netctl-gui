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
 * @file wpasupinteract.cpp
 * Source code of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#include <QDebug>

#include "netctlgui.h"
#include "taskadds.h"


/**
 * @class WpaSup
 */
/**
 * @fn WpaSup
 */
WpaSup::WpaSup(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
    netctlCommand = new Netctl(debug, settings);
    netctlProfile = new NetctlProfile(debug, settings);

    if (settings.contains(QString("CTRL_DIR")))
        ctrlDir = settings[QString("CTRL_DIR")];
    if (settings.contains(QString("CTRL_GROUP")))
        ctrlGroup = settings[QString("CTRL_GROUP")];
    if (settings.contains(QString("PID_FILE")))
        pidFile = settings[QString("PID_FILE")];
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    if (settings.contains(QString("WPACLI_PATH")))
        wpaCliPath = settings[QString("WPACLI_PATH")];
    if (settings.contains(QString("WPA_DRIVERS")))
        wpaDrivers = settings[QString("WPA_DRIVERS")];
    if (settings.contains(QString("WPASUP_PATH")))
        wpaSupPath = settings[QString("WPASUP_PATH")];
    if (settings.contains(QString("FORCE_SUDO")))
        if (settings[QString("FORCE_SUDO")] == QString("true"))
            useSuid = false;

    if (useSuid)
        sudoCommand = QString("");
}


/**
 * @fn ~WpaSup
 */
WpaSup::~WpaSup()
{
    if (debug) qDebug() << "[WpaSup]" << "[~WpaSup]";

    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
}


// general information
/**
 * @fn existentProfile
 */
QString WpaSup::existentProfile(const QString essid)
{
    if (debug) qDebug() << "[WpaSup]" << "[existentProfile]";
    if (debug) qDebug() << "[WpaSup]" << "[existentProfile]" << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[existentProfile]" << ":" << "Could not find library";
        return QString();
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[existentProfile]" << ":" << "Could not find library";
        return QString();
    }

    QString profileFile = QString("");
    QList<netctlProfileInfo> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlProfile->getValueFromProfile(profileList[i].name, QString("ESSID")))
            profileFile = profileList[i].name;

    return profileFile;
}


/**
 * @fn isProfileActive
 */
bool WpaSup::isProfileActive(const QString essid)
{
    if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]";
    if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]" << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]" << ":" << "Could not find library";
        return false;
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]" << ":" << "Could not find library";
        return false;
    }

    QString profileFile;
    QList<netctlProfileInfo> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlProfile->getValueFromProfile(profileList[i].name, QString("ESSID"))) {
            profileFile = profileList[i].name;
            break;
        }

    return netctlCommand->isProfileActive(profileFile);
}


/**
 * @fn isProfileExists
 */
bool WpaSup::isProfileExists(const QString essid)
{
    if (debug) qDebug() << "[WpaSup]" << "[isProfileExists]";
    if (debug) qDebug() << "[WpaSup]" << "[isProfileExists]" << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[isProfileExists]" << ":" << "Could not find library";
        return false;
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[isProfileExists]" << ":" << "Could not find library";
        return false;
    }

    bool exists = false;
    QList<netctlProfileInfo> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlProfile->getValueFromProfile(profileList[i].name, QString("ESSID"))) {
            exists = true;
            break;
        }

    return exists;
}


/**
 * @fn scanWifi
 */
QList<netctlWifiInfo> WpaSup::scanWifi()
{
    if (debug) qDebug() << "[WpaSup]" << "[scanWifi]";

    QList<netctlWifiInfo> scanResults;
    if (!startWpaSupplicant()) {
        stopWpaSupplicant();
        return scanResults;
    }
    if (!wpaCliCall(QString("scan")))
        return scanResults;
    waitForProcess(3);

    QStringList rawOutput = getWpaCliOutput(QString("scan_results")).split(QChar('\n'), QString::SkipEmptyParts);
    // remove table header
    rawOutput.removeFirst();
    // remove duplicates
    QStringList rawList;
    for (int i=0; i<rawOutput.count(); i++) {
        bool exist = false;
        if (rawOutput[i].split(QChar('\t'), QString::SkipEmptyParts).count() > 4)
            for (int j=0; j<rawList.count(); j++)
                if (rawList[j].split(QChar('\t'), QString::SkipEmptyParts).count() > 4)
                    if (rawOutput[i].split(QChar('\t'), QString::SkipEmptyParts)[4] ==
                            rawList[j].split(QChar('\t'), QString::SkipEmptyParts)[4])
                        exist = true;
        if (!exist)
            rawList.append(rawOutput[i]);
    }

    for (int i=0; i<rawList.count(); i++) {
        netctlWifiInfo wifiPoint;
        // point name
        if (rawList[i].split(QChar('\t'), QString::SkipEmptyParts).count() > 4)
            wifiPoint.name = rawList[i].split(QChar('\t'), QString::SkipEmptyParts)[4];
        else
            wifiPoint.name = QString("<hidden>");
        // profile status
        wifiPoint.active = isProfileActive(wifiPoint.name);
        wifiPoint.exists = isProfileExists(wifiPoint.name);
        // point signal
        wifiPoint.signal = rawList[i].split(QChar('\t'), QString::SkipEmptyParts)[2];
        // point security
        QString security = rawList[i].split(QChar('\t'), QString::SkipEmptyParts)[3];
        if (security.contains(QString("WPA2")))
            security = QString("WPA2");
        else if (security.contains(QString("WPA")))
            security = QString("WPA");
        else if (security.contains(QString("WEP")))
            security = QString("WEP");
        else
            security = QString("none");
        wifiPoint.security = security;
        scanResults.append(wifiPoint);
    }
    stopWpaSupplicant();

    return scanResults;
}


/**
 * @fn startWpaSupplicant
 */
bool WpaSup::startWpaSupplicant()
{
    if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]";
    if (ctrlDir == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find directory";
        return false;
    }
    if (ctrlGroup == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find group";
        return false;
    }
    if (pidFile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find PID file";
        return false;
    }
    if (wpaDrivers == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find drivers";
        return false;
    }
    if (wpaSupPath == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find wpa_supplicant";
        return false;
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find library";
        return false;
    }
    if (netctlCommand->getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find interfaces";
        return false;
    }

    if (QFile(pidFile).exists())
        return true;
    QString interface = netctlCommand->getWirelessInterfaceList()[0];
    QString cmd = sudoCommand + QString(" ") + wpaSupPath + QString(" -B -P ") + pidFile +
            QString(" -i ") + interface + QString(" -D ") + wpaDrivers +
            QString(" -C \"DIR=") + ctrlDir + QString(" GROUP=") + ctrlGroup + QString("\"");
    if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    waitForProcess(1);
    if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}


/**
 * @fn stopWpaSupplicant
 */
bool WpaSup::stopWpaSupplicant()
{
    if (debug) qDebug() << "[WpaSup]" << "[stopWpaSupplicant]";

    return wpaCliCall(QString("terminate"));
}


// functions
/**
 * @fn getWpaCliOutput
 */
QString WpaSup::getWpaCliOutput(const QString commandLine)
{
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]";
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Command" << commandLine;
    if (ctrlDir == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Could not find directory";
        return QString();
    }
    if (pidFile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Could not find PID file";
        return QString();
    }
    if (wpaCliPath == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Could not find wpa_cli";
        return QString();
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Could not find library";
        return QString();
    }
    if (netctlCommand->getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Could not find interfaces";
        return QString();
    }

    QString interface = netctlCommand->getWirelessInterfaceList()[0];
    QString cmd = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Cmd returns" << process.exitCode;

    return process.output;
}


/**
 * @fn waitForProcess
 */
bool WpaSup::waitForProcess(const int sec)
{
    if (debug) qDebug() << "[WpaSup]" << "[waitForProcess]";
    if (debug) qDebug() << "[WpaSup]" << "[waitForProcess]" << ":" << "Interval" << sec;

    QString cmd = QString("sleep %1").arg(QString::number(sec));
    runTask(cmd);

    return true;
}


/**
 * @fn wpaCliCall
 */
bool WpaSup::wpaCliCall(const QString commandLine)
{
    if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]";
    if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Command" << commandLine;
    if (ctrlDir == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Could not find directory";
        return false;
    }
    if (pidFile == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Could not find PID file";
        return false;
    }
    if (wpaCliPath == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Could not find wpa_cli";
        return false;
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Could not find library";
        return false;
    }
    if (netctlCommand->getWirelessInterfaceList().isEmpty()) {
        if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Could not find interfaces";
        return false;
    }

    QString interface = netctlCommand->getWirelessInterfaceList()[0];
    QString cmd = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    waitForProcess(1);
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Cmd returns" << process.exitCode;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}
