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
#include <grp.h>

#include <netctlgui/netctlgui.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>


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
    if (debug) qDebug() << PDEBUG;

    if (netctlCommand != nullptr) delete netctlCommand;
    if (netctlProfile != nullptr) delete netctlProfile;
}


// general information
/**
 * @fn existentProfile
 */
QString WpaSup::existentProfile(const QString essid)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QString();
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
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
 * @fn getRecommendedConfiguration
 */
QMap<QString, QString> WpaSup::getRecommendedConfiguration()
{
    QMap<QString, QString> settings;
    int size = 99;
    QString cmd;
    TaskResult process;
    QStringList recommended;
    // ctrl directory
    // nothing to do
    settings[QString("CTRL_DIR")] = QString("/run/wpa_supplicant_netctl-gui");
    // ctrl group
    // check group list and find out 'network', 'users', 'root'
    settings[QString("CTRL_GROUP")] = QString("");
    gid_t gtpList[99];
    int grpSize = getgroups(size, gtpList);
    recommended.clear();
    recommended.append("network");
    recommended.append("users");
    recommended.append("root");
    for (int i=0; i<recommended.count(); i++) {
        for (int j=0; j<grpSize; j++)
            if (recommended[i] == QString(getgrgid(gtpList[j])->gr_name)) {
                settings[QString("CTRL_GROUP")] = recommended[i];
                break;
            }
        if (!settings[QString("CTRL_GROUP")].isEmpty()) break;
    }
    // force sudo
    // find out helper exe
    settings[QString("FORCE_SUDO")] = QString("true");
    recommended.clear();
    recommended.append(QString("netctlgui-helper"));
    recommended.append(QString("netctlgui-helper-suid"));
    for (int i=0; i<recommended.count(); i++) {
        cmd = QString("which ") + recommended[i];
        process = runTask(cmd, false);
        if (process.exitCode == 0) {
            settings[QString("FORCE_SUDO")] = QString("false");
            break;
        }
    }
    // pid file
    // nothing to do
    settings[QString("PID_FILE")] = QString("/run/wpa_supplicant_netctl-gui.pid");
    // sudo path
    // find out sudo, kdesu, gksu exes
    settings[QString("SUDO_PATH")] = QString("");
    recommended.clear();
    recommended.append("sudo");
    recommended.append("kdesu");
    recommended.append("gksu");
    for (int i=0; i<recommended.count(); i++) {
        cmd = QString("which ") + recommended[i];
        process = runTask(cmd, false);
        if (process.exitCode == 0) {
            settings[QString("SUDO_PATH")] = process.output.trimmed();
            break;
        }
    }
    // wpa_cli path
    // find out wpa_cli exe
    settings[QString("WPACLI_PATH")] = QString("true");
    recommended.clear();
    recommended.append("wpa_cli");
    for (int i=0; i<recommended.count(); i++) {
        cmd = QString("which ") + recommended[i];
        process = runTask(cmd, false);
        if (process.exitCode == 0) {
            settings[QString("WPACLI_PATH")] = process.output.trimmed();
            break;
        }
    }
    // wpa drivers
    // nothing to do
    settings[QString("WPA_DRIVERS")] = QString("nl80211,wext");
    // wpa_supplicant path
    // find out wpa_supplicant exe
    settings[QString("WPASUP_PATH")] = QString("true");
    recommended.clear();
    recommended.append("wpa_supplicant");
    for (int i=0; i<recommended.count(); i++) {
        cmd = QString("which ") + recommended[i];
        process = runTask(cmd, false);
        if (process.exitCode == 0) {
            settings[QString("WPASUP_PATH")] = process.output.trimmed();
            break;
        }
    }

    return settings;
}


/**
 * @fn isProfileActive
 */
bool WpaSup::isProfileActive(const QString essid)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
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
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    if (netctlProfile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
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
    if (debug) qDebug() << PDEBUG;

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
    if (debug) qDebug() << PDEBUG;
    if (ctrlDir == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }
    if (ctrlGroup == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find group";
        return false;
    }
    if (pidFile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return false;
    }
    if (wpaDrivers == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find drivers";
        return false;
    }
    if (wpaSupPath == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_supplicant";
        return false;
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return false;
    }

    if (QFile(pidFile).exists())
        return true;
    QString interface = interfaces[0];
    QString cmd = sudoCommand + QString(" ") + wpaSupPath + QString(" -B -P ") + pidFile +
            QString(" -i ") + interface + QString(" -D ") + wpaDrivers +
            QString(" -C \"DIR=") + ctrlDir + QString(" GROUP=") + ctrlGroup + QString("\"");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    waitForProcess(1);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

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
    if (debug) qDebug() << PDEBUG;

    return wpaCliCall(QString("terminate"));
}


// functions
/**
 * @fn getWpaCliOutput
 */
QString WpaSup::getWpaCliOutput(const QString commandLine)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << commandLine;
    if (ctrlDir == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return QString();
    }
    if (pidFile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return QString();
    }
    if (wpaCliPath == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_cli";
        return QString();
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QString();
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return QString();
    }

    QString interface = interfaces[0];
    QString cmd = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.output;
}


/**
 * @fn waitForProcess
 */
bool WpaSup::waitForProcess(const int sec)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << sec;

    QString cmd = QString("sleep %1").arg(QString::number(sec));
    runTask(cmd);

    return true;
}


/**
 * @fn wpaCliCall
 */
bool WpaSup::wpaCliCall(const QString commandLine)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << commandLine;
    if (ctrlDir == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }
    if (pidFile == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return false;
    }
    if (wpaCliPath == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_cli";
        return false;
    }
    if (netctlCommand == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return false;
    }

    QString interface = interfaces[0];
    QString cmd = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    waitForProcess(1);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}
