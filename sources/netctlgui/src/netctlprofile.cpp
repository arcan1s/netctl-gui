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


/**
 * @class NetctlProfile
 */
/**
 * @fn NetctlProfile
 */
NetctlProfile::NetctlProfile(const bool debugCmd, const QMap<QString, QString> settings)
    : debug(debugCmd)
{
    if (settings.contains(QString("PROFILE_DIR")))
        profileDirectory = new QDir(settings[QString("PROFILE_DIR")]);
    else
        profileDirectory = new QDir(QString(PROFILE_DIR));
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    if (settings.contains(QString("FORCE_SUDO")))
        useSuid = (settings[QString("FORCE_SUDO")] != QString("true"));

    if (useSuid)
        sudoCommand = QString("");
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
bool NetctlProfile::copyProfile(const QString oldPath)
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
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}


/**
 * @fn createProfile
 */
QString NetctlProfile::createProfile(const QString profile, const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString profileTempName = QString("%1/%2").arg(QDir::tempPath()).arg(QFileInfo(profile).fileName());
    QFile profileFile(profileTempName);
    if (debug) qDebug() << PDEBUG << ":" << "Save to" << profileTempName;
    if (!profileFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return profileTempName;
    QTextStream out(&profileFile);
    for (int i=0; i<settings.keys().count(); i++) {
        out << settings.keys()[i] << QString("=");
        if ((settings.keys()[i] == QString("BindsToInterfaces")) ||
            (settings.keys()[i] == QString("After")) ||
            (settings.keys()[i] == QString("Address")) ||
            (settings.keys()[i] == QString("Routes")) ||
            (settings.keys()[i] == QString("Address6")) ||
            (settings.keys()[i] == QString("Routes6")) ||
            (settings.keys()[i] == QString("IPCustom")) ||
            (settings.keys()[i] == QString("DNS")) ||
            (settings.keys()[i] == QString("DNSOptions")) ||
            (settings.keys()[i] == QString("ScanFrequencies")) ||
            (settings.keys()[i] == QString("WPAConfigSection")))
            out << QString("(%1)").arg(settings[settings.keys()[i]]) << endl;
        else
            out << settings[settings.keys()[i]] << endl;
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
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
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
    for (int i=0; i<recommended.count(); i++) {
        process = runTask(QString("which %1").arg(recommended[i]), false);
        if (process.exitCode == 0) {
            settings[QString("SUDO_PATH")] = process.output.trimmed();
            break;
        }
    }

    return settings;
}


/**
 * @fn getSettingsFromProfile
 */
QMap<QString, QString> NetctlProfile::getSettingsFromProfile(const QString profile)
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
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    QStringList output = QString(process.output).trimmed().split(QChar('\n'));
    QStringList systemVariables;
    systemVariables.append(QString("PIPESTATUS"));
    for (int i=0; i<output.count(); i++)
        systemVariables.append(output[i].split(QChar('='))[0]);
    // profile variables
    QMap<QString, QString> settings;
    QString profileUrl = QString("%1/%2").arg(profileDirectory->absolutePath()).arg(QFileInfo(profile).fileName());
    cmd = QString("env -i bash -c \"source '%1'; set\"").arg(profileUrl);
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    output = QString(process.output).trimmed().split(QChar('\n'));

    // read variables
    QStringList keys;
    for (int i=0; i<output.count(); i++)
        if (!systemVariables.contains(output[i].split(QChar('='))[0]))
            keys.append(output[i].split(QChar('='))[0]);
    for (int i=0; i<keys.count(); i++){
        cmd = QString("env -i bash -c \"source '%1'; for i in ${!%2[@]}; do echo ${%2[$i]}; done\"").arg(profileUrl).arg(keys[i]);
        process = runTask(cmd, false);
        settings[keys[i]] = process.output.trimmed();
        if (debug) qDebug() << PDEBUG << ":" << keys[i] << "=" << settings[keys[i]];
    }

    return settings;
}


/**
 * @fn getValueFromProfile
 */
QString NetctlProfile::getValueFromProfile(const QString profile, const QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (debug) qDebug() << PDEBUG << ":" << "Key" << key;

    return getValuesFromProfile(profile, QStringList() << key)[0];
}


/**
 * @fn getValuesFromProfile
 */
QStringList NetctlProfile::getValuesFromProfile(const QString profile, const QStringList keys)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (debug) qDebug() << PDEBUG << ":" << "Keys" << keys;

    QMap<QString, QString> settings = getSettingsFromProfile(profile);
    QStringList values;
    for (int i=0; i<keys.count(); i++)
        values.append(settings[keys[i]]);

    return values;
}


/**
 * @fn removeProfile
 */
bool NetctlProfile::removeProfile(const QString profile)
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
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    return (process.exitCode == 0);
}
