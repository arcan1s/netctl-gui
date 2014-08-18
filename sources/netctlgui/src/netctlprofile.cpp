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

#include "netctlgui.h"
#include "pdebug.h"
#include "taskadds.h"


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
        profileDirectory = new QDir(QString("/etc/netctl/"));
    if (settings.contains(QString("SUDO_PATH")))
        sudoCommand = settings[QString("SUDO_PATH")];
    if (settings.contains(QString("FORCE_SUDO")))
        if (settings[QString("FORCE_SUDO")] == QString("true"))
            useSuid = false;

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
    if (profileDirectory == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }

    QString newPath = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(oldPath).fileName();
    QString cmd = sudoCommand + QString(" /usr/bin/mv \"") + oldPath + QString("\" \"") + newPath + QString("\"");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}


/**
 * @fn createProfile
 */
QString NetctlProfile::createProfile(const QString profile, const QMap<QString, QString> settings)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    QString profileTempName = QDir::tempPath() + QDir::separator() + QFileInfo(profile).fileName();
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
            out << QString("(") + settings[settings.keys()[i]] << QString(")") << endl;
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
    QString cmd;
    TaskResult process;
    QStringList recommended;
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
        cmd = QString("which ") + recommended[i];
        process = runTask(cmd, false);
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
    if (profileDirectory == 0) {
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
    QString profileUrl = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(profile).fileName();
    cmd = QString("env -i bash -c \"source '") + profileUrl + QString("'; set\"");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    process = runTask(cmd, false);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;
    output = QString(process.output).trimmed().split(QChar('\n'));

    // gettings variables
    QStringList keys;
    for (int i=0; i<output.count(); i++)
        if (!systemVariables.contains(output[i].split(QChar('='))[0]))
            keys.append(output[i].split(QChar('='))[0]);
    for (int i=0; i<keys.count(); i++){
        cmd = QString("env -i bash -c \"source '") + profileUrl +
                QString("'; for i in ${!") + keys[i] + QString("[@]}; do echo ${") +
                keys[i] + QString("[$i]}; done\"");
        process = runTask(cmd, false);
        settings[keys[i]] = process.output.trimmed();
        if (debug) qDebug() << PDEBUG << ":" << keys[i] << "=" << settings[keys[i]];
    }

    return settings;
}


/**
 * @fn ValueFromProfile
 */
QString NetctlProfile::getValueFromProfile(const QString profile, const QString key)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (debug) qDebug() << PDEBUG << ":" << "Key" << key;

    QMap<QString, QString> settings = getSettingsFromProfile(profile);

    if (settings.contains(key))
        return settings[key];
    else
        return QString();
}


/**
 * @fn removeProfile
 */
bool NetctlProfile::removeProfile(const QString profile)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;
    if (profileDirectory == 0) {
        if (debug) qDebug() << PDEBUG << ":" << "Could not find directory";
        return false;
    }

    QString profilePath = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(profile).fileName();
    QString cmd = sudoCommand + QString(" /usr/bin/rm \"") + profilePath + QString("\"");
    if (debug) qDebug() << PDEBUG << ":" << "Run cmd" << cmd;
    TaskResult process = runTask(cmd, useSuid);
    if (debug) qDebug() << PDEBUG << ":" << "Cmd returns" << process.exitCode;
    if (process.exitCode != 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error" << process.error;

    if (process.exitCode == 0)
        return true;
    else
        return false;
}
