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
#include <QProcess>

#include <netctlgui/netctlinteract.h>
#include <netctlgui/sleepthread.h>
#include <netctlgui/wpasupinteract.h>


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

    if (settings.contains(QString("CTRL_DIR")))
        ctrlDir = settings[QString("CTRL_DIR")];
    else
        ctrlDir = QString("/run/wpa_supplicant_netctl-gui");
    if (settings.contains(QString("CTRL_GROUP")))
        ctrlGroup = settings[QString("CTRL_GROUP")];
    else
        ctrlGroup = QString("users");
    if (settings.contains(QString("IFACE_DIR")))
        ifaceDirectory = new QDir(settings[QString("IFACE_DIR")]);
    else
        ifaceDirectory = new QDir(QString("/sys/class/net/"));
    if (settings.contains(QString("PREFERED_IFACE")))
        mainInterface = settings[QString("PREFERED_IFACE")];
    else
        mainInterface = QString("");
    if (settings.contains(QString("PID_FILE")))
        pidFile = settings[QString("PID_FILE")];
    else
        pidFile = QString("/run/wpa_supplicant_netctl-gui.pid");
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    else
        sudoCommand = QString("/usr/bin/kdesu");
    if (settings.contains(QString("WPACLI_PATH")))
        wpaCliPath = settings[QString("WPACLI_PATH")];
    else
        wpaCliPath = QString("/usr/bin/wpa_cli");
    if (settings.contains(QString("WPA_DRIVERS")))
        wpaDrivers = settings[QString("WPA_DRIVERS")];
    else
        wpaDrivers = QString("nl80211,wext");
    if (settings.contains(QString("WPASUP_PATH")))
        wpaSupPath = settings[QString("WPASUP_PATH")];
    else
        wpaSupPath = QString("/usr/bin/wpa_supplicant");

    // terminate old loaded profile
    if (QFile(pidFile).exists() || QDir(ctrlDir).exists())
        stopWpaSupplicant();
}


/**
 * @fn ~WpaSup
 */
WpaSup::~WpaSup()
{
    if (debug) qDebug() << "[WpaSup]" << "[~WpaSup]";

    delete netctlCommand;
    if (ifaceDirectory != 0)
        delete ifaceDirectory;
}


// general information
/**
 * @fn existentProfile
 */
QString WpaSup::existentProfile(const QString essid)
{
    if (debug) qDebug() << "[WpaSup]" << "[existentProfile]";
    if (debug) qDebug() << "[WpaSup]" << "[existentProfile]" << ":" << "ESSID" << essid;

    QString profileFile = QString("");
    QList<QStringList> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlCommand->getSsidFromProfile(profileList[i][0]))
            profileFile = profileList[i][0];

    return profileFile;
}


/**
 * @fn getInterfaceList
 */
QStringList WpaSup::getInterfaceList()
{
    if (debug) qDebug() << "[WpaSup]" << "[getInterfaceList]";
    if (ifaceDirectory == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[getInterfaceList]" << ":" << "Could not find directory";
        return QStringList();
    }

    QStringList interfaces;
    if (!mainInterface.isEmpty())
        interfaces.append(mainInterface);
    QStringList allInterfaces = ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (int i=0; i<allInterfaces.count(); i++) {
        if (debug) qDebug() << "[WpaSup]" << "[getInterfaceList]" << ":" << "Check directory"
                 << ifaceDirectory->path() + QDir::separator() + allInterfaces[i] + QDir::separator() + QString("wireless");
        if (QDir(ifaceDirectory->path() + QDir::separator() + allInterfaces[i] +
                 QDir::separator() + QString("wireless")).exists())
            interfaces.append(allInterfaces[i]);
    }

    return interfaces;
}


/**
 * @fn isProfileActive
 */
bool WpaSup::isProfileActive(const QString essid)
{
    if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]";
    if (debug) qDebug() << "[WpaSup]" << "[isProfileActive]" << ":" << "ESSID" << essid;

    QString profileFile;
    QList<QStringList> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlCommand->getSsidFromProfile(profileList[i][0])) {
            profileFile = profileList[i][0];
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

    bool exists = false;
    QList<QStringList> profileList = netctlCommand->getProfileList();
    for (int i=0; i<profileList.count(); i++)
        if (essid == netctlCommand->getSsidFromProfile(profileList[i][0])) {
            exists = true;
            break;
        }

    return exists;
}


/**
 * @fn scanWifi
 */
QList<QStringList> WpaSup::scanWifi()
{
    if (debug) qDebug() << "[WpaSup]" << "[scanWifi]";

    QList<QStringList> scanResults;
    if (!startWpaSupplicant()) {
        stopWpaSupplicant();
        return scanResults;
    }
    if (!wpaCliCall(QString("scan")))
        return scanResults;
    SleepThread::sleep(3);

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
        QStringList wifiPoint;
        // point name
        if (rawList[i].split(QChar('\t'), QString::SkipEmptyParts).count() > 4)
            wifiPoint.append(rawList[i].split(QChar('\t'), QString::SkipEmptyParts)[4]);
        else
            wifiPoint.append(QString("<hidden>"));
        // profile status
        QString status;
        if (isProfileExists(wifiPoint[0])) {
            status = QString("exists");
            if (isProfileActive(wifiPoint[0]))
                status = status + QString(" (active)");
            else
                status = status + QString(" (inactive)");
        }
        else
            status = QString("new");
        wifiPoint.append(status);
        // point signal
        wifiPoint.append(rawList[i].split(QChar('\t'), QString::SkipEmptyParts)[2]);
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
        wifiPoint.append(security);
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
    if (sudoCommand == 0) {
        if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Could not find sudo";
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

    if (QFile(pidFile).exists())
        return true;
    QProcess command;
    QString interface = getInterfaceList()[0];
    QString commandText = sudoCommand + QString(" ") + wpaSupPath + QString(" -B -P ") + pidFile +
            QString(" -i ") + interface + QString(" -D ") + wpaDrivers +
            QString(" -C \"DIR=") + ctrlDir + QString(" GROUP=") + ctrlGroup + QString("\"");
    if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    SleepThread::sleep(1);
    if (debug) qDebug() << "[WpaSup]" << "[startWpaSupplicant]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() == 0)
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

    QProcess command;
    QString interface = getInterfaceList()[0];
    QString commandText = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << "[WpaSup]" << "[getWpaCliOutput]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);

    return command.readAllStandardOutput();
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

    QProcess command;
    QString interface = getInterfaceList()[0];
    QString commandText = wpaCliPath + QString(" -i ") + interface + QString(" -p ") + ctrlDir +
            QString(" -P ") + pidFile + QString(" ") + commandLine;
    if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);
    SleepThread::sleep(1);
    if (debug) qDebug() << "[WpaSup]" << "[wpaCliCall]" << ":" << "Cmd returns" << command.exitCode();

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}
