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
 * @file netctlprofile.cpp
 * Source code of netctlgui library
 * @author Evgeniy Alekseev
 * @copyright GPLv3
 * @bug https://github.com/arcan1s/netctl-gui/issues
 */


#include <QDebug>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <netctlgui/netctlgui.h>
#include <pdebug/pdebug.h>
#include <task/taskadds.h>

#include "version.h"


/**
 * @class NetctlProfile
 */
/**
 * @fn NetctlProfile
 */
NetctlProfile::NetctlProfile(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
    profileDirectory = new QDir(settings.value(QString("PROFILE_DIR"), QString(PROFILE_DIR)));
    sudoCommand = settings.value(QString("SUDO_PATH"), QString(SUDO_PATH));
    useSuid = (settings.value(QString("FORCE_SUDO"), QString("true")) != QString("true"));

    if (useSuid) sudoCommand = QString("");
}


/**
 * @fn ~NetctlProfile
 */
NetctlProfile::~NetctlProfile()
{
    if (debug) qDebug() << PDEBUG;

    if (profileDirectory != nullptr) delete profileDirectory;
}


/**
 * @fn copyProfile
 */
bool NetctlProfile::copyProfile(const QString oldPath) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Path" << oldPath;
    if (profileDirectory == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }

    QString newPath = QString("%1/%2").arg(profileDirectory->absolutePath()).arg(QFileInfo(oldPath).fileName());
    QString cmd = QString("%1 /usr/bin/mv \"%2\" \"%3\"").arg(sudoCommand)
                                                         .arg(oldPath)
                                                         .arg(newPath);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.status();
}


/**
 * @fn createProfile
 */
QString NetctlProfile::createProfile(const QString profile, const QMap<QString, QString> settings) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString profileTempName = QString("%1/%2").arg(QDir::tempPath()).arg(QFileInfo(profile).fileName());
    QFile profileFile(profileTempName);
    if (debug) qDebug() << PDEBUG << ":" << "Save to" << profileTempName;
    if (!profileFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return profileTempName;
    QTextStream out(&profileFile);
    foreach(QString key, settings.keys()) {
        out << key << QString("=");
        if ((key == QString("BindsToInterfaces")) ||
            (key == QString("After")) ||
            (key == QString("Address")) ||
            (key == QString("Routes")) ||
            (key == QString("Address6")) ||
            (key == QString("Routes6")) ||
            (key == QString("IPCustom")) ||
            (key == QString("DNS")) ||
            (key == QString("DNSOptions")) ||
            (key == QString("ScanFrequencies")) ||
            (key == QString("WPAConfigSection")))
            out << QString("(%1)").arg(settings[key]) << endl;
        else
            out << settings[key] << endl;
    }
    profileFile.close();

    return profileTempName;
}


/**
 * @fn getRecommendedConfiguration
 */
QMap<QString, QString> NetctlProfile::getRecommendedConfiguration()
{
    QMap<QString, QString> settings;
    TaskResult process;
    QStringList recommended;
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
    // profile path
    // find out netctl directory into /etc
    settings[QString("PROFILE_DIR")] = QString("");
    QDirIterator iterator(QDir("/etc"), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        if (!iterator.fileInfo().isDir()) continue;
        QString name = iterator.filePath();
        if (name.contains(QString("netctl"))) {
            settings[QString("PROFILE_DIR")] = name;
            break;
        }
    }
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

    return settings;
}


/**
 * @fn getSettingsFromProfile
 */
QMap<QString, QString> NetctlProfile::getSettingsFromProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (profileDirectory == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return QMap<QString, QString>();
    }

    // getting variables list
    // system variables
    QString cmd = QString("env -i bash -c \"set\"");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QStringList output = QString(process.output).trimmed().split(QChar('\n'));
    QStringList systemVariables;
    systemVariables.append(QString("PIPESTATUS"));
    foreach(QString str, output)
        systemVariables.append(str.split(QChar('=')).first());
    // profile variables
    QMap<QString, QString> settings;
    QString profileUrl = QString("%1/%2").arg(profileDirectory->absolutePath()).arg(QFileInfo(profile).fileName());
    cmd = QString("env -i bash -c \"source '%1'; set\"").arg(profileUrl);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    output = QString(process.output).trimmed().split(QChar('\n'));

    // read variables
    QStringList keys;
    foreach(QString str, output)
        if (!systemVariables.contains(str.split(QChar('=')).first()))
            keys.append(str.split(QChar('=')).first());
    foreach(QString key, keys) {
        cmd = QString("env -i bash -c \"source '%1'; for i in ${!%2[@]}; do echo ${%2[$i]}; done\"").arg(profileUrl).arg(key);
        process = runTask(cmd, false);
        settings[key] = process.output.trimmed();
        if (debug) qDebug() << PDEBUG << ":" << key << "=" << settings[key];
    }

    return settings;
}


/**
 * @fn getValueFromProfile
 */
QString NetctlProfile::getValueFromProfile(const QString profile, const QString key) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (debug) qDebug() << PDEBUG << ":" << "Key" << key;

    return getValuesFromProfile(profile, QStringList() << key).first();
}


/**
 * @fn getValuesFromProfile
 */
QStringList NetctlProfile::getValuesFromProfile(const QString profile, const QStringList keys) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (debug) qDebug() << PDEBUG << ":" << "Keys" << keys;

    QMap<QString, QString> settings = getSettingsFromProfile(profile);
    QStringList values;
    foreach(QString key, keys)
        values.append(settings[key]);

    return values;
}


/**
 * @fn removeProfile
 */
bool NetctlProfile::removeProfile(const QString profile) const
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (profileDirectory == nullptr) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }

    QString profilePath = QString("%1/%2").arg(profileDirectory->absolutePath()).arg(QFileInfo(profile).fileName());
    QString cmd = QString("%1 /usr/bin/rm \"%2\"").arg(sudoCommand).arg(profilePath);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return process.status();
}
