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

#include "version.h"


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

    ctrlDir = settings.value(QString("CTRL_DIR"), QString(CTRL_DIR));
    ctrlGroup = settings.value(QString("CTRL_GROUP"), QString(CTRL_GROUP));
    pidFile = settings.value(QString("PID_FILE"), QString(PID_FILE));
    sudoCommand = settings.value(QString("SUDO_PATH"), QString(SUDO_PATH));
    wpaCliPath = settings.value(QString("WPACLI_PATH"), QString(WPACLI_PATH));
    wpaDrivers = settings.value(QString("WPA_DRIVERS"), QString(WPA_DRIVERS));
    wpaSupPath = settings.value(QString("WPASUP_PATH"), QString(WPASUP_PATH));
    useSuid = (settings.value(QString("FORCE_SUDO"), QString("true")) != QString("true"));

    if (useSuid) sudoCommand = QString("");
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
QString WpaSup::existentProfile(const QString essid) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QString();
    }
    if (netctlProfile == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return QString();
    }

    QString profileFile;
    QList<netctlProfileInfo> profileList = netctlCommand->getProfileList();
    foreach(netctlProfileInfo profile, profileList) {
        if (essid != profile.essid) continue;
        profileFile = profile.name;
        break;
    }

    return profileFile;
}


/**
 * @fn getRecommendedConfiguration
 */
QMap<QString, QString> WpaSup::getRecommendedConfiguration()
{
    QMap<QString, QString> settings;
    int size = 99;
    TaskResult process;
    QStringList recommended;
    // ctrl directory
    // nothing to do
    settings[QString("CTRL_DIR")] = QString("/run/wpa_supplicant");
    // ctrl group
    // check group list and find out 'network', 'users', 'root'
    settings[QString("CTRL_GROUP")] = QString("");
    gid_t gtpList[size];
    int grpSize = getgroups(size, gtpList);
    recommended.clear();
    recommended.append("network");
    recommended.append("users");
    recommended.append("root");
    foreach(QString rec, recommended) {
        for (int i=0; i<grpSize; i++)
            if (rec == QString(getgrgid(gtpList[i])->gr_name)) {
                settings[QString("CTRL_GROUP")] = rec;
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
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("FORCE_SUDO")] = QString("false");
            break;
        }
    }
    // pid file
    // nothing to do
    settings[QString("PID_FILE")] = QString("/run/wpa_supplicant_$i.pid");
    // sudo path
    // find out sudo, kdesu, gksu exes
    settings[QString("SUDO_PATH")] = QString("");
    recommended.clear();
    recommended.append("sudo");
    recommended.append("kdesu");
    recommended.append("gksu");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("SUDO_PATH")] = process.output.trimmed();
            break;
        }
    }
    // wpa_cli path
    // find out wpa_cli exe
    settings[QString("WPACLI_PATH")] = QString("true");
    recommended.clear();
    recommended.append("wpa_cli");
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
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
    foreach(QString rec, recommended) {
        process = runTask(QString("which %1").arg(rec), false);
        if (process.status()) {
            settings[QString("WPASUP_PATH")] = process.output.trimmed();
            break;
        }
    }

    return settings;
}


/**
 * @fn isProfileActive
 */
bool WpaSup::isProfileActive(const QString essid) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "ESSID" << essid;
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    if (netctlProfile == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }

    return netctlCommand->isProfileActive(existentProfile(essid));
}


/**
 * @fn current
 */
netctlWifiInfo WpaSup::current() const
{
    if (debug) qDebug() << PDEBUG;
    if (pidFile.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return netctlWifiInfo();
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return netctlWifiInfo();
    }
    QString _pidFile = pidFile;
    _pidFile.replace(QString("$i"), interfaces.first());
    if (debug) qDebug() << PDEBUG << ":" << "PID file" << _pidFile << QFile(_pidFile).exists();

    netctlWifiInfo current;
    if (!QFile(_pidFile).exists()) return current;
    QString rawText = getWpaCliOutput(QString("status"));
    if (!rawText.contains(QString("wpa_state=COMPLETED\n"))) return current;

    QStringList rawList = rawText.split(QChar('\n'), QString::SkipEmptyParts);
    foreach(QString element, rawList) {
        QStringList line = element.split(QChar('='));
        if (line.count() != 2) continue;
        if (line.at(0) == QString("bssid"))
            current.macs.append(line.at(1));
        else if (line.at(0) == QString("freq")) {
            if ((line.at(1).toInt() >= 5000) && (line.at(1).toInt() < 6000))
                current.type = PointType::FiveG;
            else if ((line.at(1).toInt() < 5000) && (line.at(1).toInt() > 2000))
                current.type = PointType::TwoG;
            current.frequencies.append(line.at(1).toInt());
        } else if (line.at(0) == QString("ssid"))
            current.name = line.at(1);
        else if (line.at(0) == QString("key_mgmt")) {
            QString security = line.at(1);
            if (security.contains(QString("WPA2")))
                security = QString("WPA2");
            else if (security.contains(QString("WPA")))
                security = QString("WPA");
            else if (security.contains(QString("WEP")))
                security = QString("WEP");
            else
                security = QString("none");
            current.security = security;
        }
    }

    // status
    current.active = true;
    current.exists = (!existentProfile(current.name).isEmpty());

    return current;
}


/**
 * @fn scanWifi
 */
QList<netctlWifiInfo> WpaSup::scanWifi() const
{
    if (debug) qDebug() << PDEBUG;
    if (pidFile.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return QList<netctlWifiInfo>();
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return QList<netctlWifiInfo>();
    }
    QString _pidFile = pidFile;
    _pidFile.replace(QString("$i"), interfaces.first());
    if (debug) qDebug() << PDEBUG << ":" << "PID file" << _pidFile << QFile(_pidFile).exists();

    bool terminateOnExit = (!QFile(_pidFile).exists());
    QList<netctlWifiInfo> scanResults;
    if (!startWpaSupplicant()) {
        if (terminateOnExit) stopWpaSupplicant();
        return scanResults;
    }
    if (!wpaCliCall(QString("scan"))) return scanResults;
    waitForProcess(3);

    QStringList rawList = getWpaCliOutput(QString("scan_results")).split(QChar('\n'), QString::SkipEmptyParts);
    // remove table header
    rawList.removeFirst();
    QStringList names;

    // init profile list
    QList<netctlProfileInfo> profiles;
    if (netctlCommand->isNetctlAutoRunning())
        profiles = netctlCommand->getProfileListFromNetctlAuto();
    else
        profiles = netctlCommand->getProfileList();
    // iterate by wifi output
    foreach(QString element, rawList) {
        QStringList line = element.split(QChar('\t'));
        if (line.count() != 5) continue;
        QString name = line.at(4);
        if (name.isEmpty()) name = QString("<hidden>");
        // append mac and frequency if exists
        int index = names.indexOf(name);
        if ((name != QString("<hidden>")) && (index > -1)) {
            scanResults[index].frequencies.append(line.at(1).toInt());
            scanResults[index].macs.append(line.at(0));
            if (scanResults[index].signal < line.at(2).toInt())
                scanResults[index].signal = line.at(2).toInt();
            // check type
            if ((line.at(1).toInt() >= 5000) && (line.at(1).toInt() < 6000)) {
                if (scanResults[index].type == PointType::None)
                    scanResults[index].type = PointType::FiveG;
                else if (scanResults[index].type == PointType::TwoG)
                    scanResults[index].type = PointType::TwoAndFiveG;
            } else if ((line.at(1).toInt() < 5000) && (line.at(1).toInt() > 2000)) {
                if (scanResults[index].type == PointType::None)
                    scanResults[index].type = PointType::TwoG;
                else if (scanResults[index].type == PointType::FiveG)
                    scanResults[index].type = PointType::TwoAndFiveG;
            }

            continue;
        }

        // point name
        netctlWifiInfo wifiPoint;
        wifiPoint.name = name;
        // profile status
        netctlProfileInfo profile;
        profile.name = QString("");
        profile.active = false;
        foreach(netctlProfileInfo pr, profiles) {
            if (wifiPoint.name != pr.essid) continue;
            profile = pr;
            break;
        }
        wifiPoint.active = profile.active;
        wifiPoint.exists = (!profile.name.isEmpty());
        // mac
        wifiPoint.macs.append(line.at(0));
        // frequencies
        wifiPoint.frequencies.append(line.at(1).toInt());
        // type
        // check type
        if ((line.at(1).toInt() >= 5000) && (line.at(1).toInt() < 6000)) {
            wifiPoint.type = PointType::FiveG;
        } else if ((line.at(1).toInt() < 5000) && (line.at(1).toInt() > 2000)) {
            wifiPoint.type = PointType::TwoG;
        }
        // point signal
        wifiPoint.signal = line.at(2).toInt();
        // point security
        QString security = line.at(3);
        if (security.contains(QString("WPA2")))
            security = QString("WPA2");
        else if (security.contains(QString("WPA")))
            security = QString("WPA");
        else if (security.contains(QString("WEP")))
            security = QString("WEP");
        else
            security = QString("none");
        wifiPoint.security = security;

        // append
        names.append(name);
        scanResults.append(wifiPoint);
    }
    if (terminateOnExit) stopWpaSupplicant();

    return scanResults;
}


/**
 * @fn startWpaSupplicant
 */
bool WpaSup::startWpaSupplicant() const
{
    if (debug) qDebug() << PDEBUG;
    if (ctrlDir.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }
    if (ctrlGroup.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find group";
        return false;
    }
    if (pidFile.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find PID file";
        return false;
    }
    if (wpaDrivers.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find drivers";
        return false;
    }
    if (wpaSupPath.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_supplicant";
        return false;
    }
    if (netctlCommand == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find library";
        return false;
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return false;
    }
    QString _pidFile = pidFile;
    _pidFile.replace(QString("$i"), interfaces.first());
    if (debug) qDebug() << PDEBUG << ":" << "PID file" << _pidFile << QFile(_pidFile).exists();

    if (QFile(_pidFile).exists()) return (QFileInfo(ctrlDir).group() == ctrlGroup);
    QString cmd = QString("%1 %2 -B -P \"%3\" -i %4 -D %5 -C \"DIR=%6 GROUP=%7\"")
                    .arg(sudoCommand).arg(wpaSupPath).arg(_pidFile).arg(interfaces.first())
                    .arg(wpaDrivers).arg(ctrlDir).arg(ctrlGroup);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    waitForProcess(1);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.status();
}


/**
 * @fn stopWpaSupplicant
 */
bool WpaSup::stopWpaSupplicant() const
{
    if (debug) qDebug() << PDEBUG;

    return wpaCliCall(QString("terminate"));
}


// functions
/**
 * @fn getWpaCliOutput
 */
QString WpaSup::getWpaCliOutput(const QString commandLine) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << commandLine;
    if (ctrlDir.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return QString();
    }
    if (wpaCliPath.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_cli";
        return QString();
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return QString();
    }

    QString cmd = QString("%1 -i %2 -p %3 %4").arg(wpaCliPath).arg(interfaces.first()).arg(ctrlDir).arg(commandLine);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.output;
}


/**
 * @fn waitForProcess
 */
bool WpaSup::waitForProcess(const int sec) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Interval" << sec;

    QString cmd = QString("sleep %1").arg(sec);
    runTask(cmd);

    return true;
}


/**
 * @fn wpaCliCall
 */
bool WpaSup::wpaCliCall(const QString commandLine) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Command" << commandLine;
    if (ctrlDir.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }
    if (wpaCliPath.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find wpa_cli";
        return false;
    }
    QStringList interfaces = netctlCommand->getWirelessInterfaceList();
    if (interfaces.isEmpty()) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find interfaces";
        return false;
    }

    QString cmd = QString("%1 -i %2 -p %3 %4").arg(wpaCliPath).arg(interfaces.first()).arg(ctrlDir).arg(commandLine);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd);
    waitForProcess(1);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.status();
}
