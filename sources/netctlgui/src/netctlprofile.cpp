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
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

#include "netctlgui.h"


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
    else
        sudoCommand = QString("/usr/bin/kdesu");
}


/**
 * @fn ~NetctlProfile
 */
NetctlProfile::~NetctlProfile()
{
    if (debug) qDebug() << "[NetctlProfile]" << "[~NetctlProfile]";

    if (profileDirectory != 0)
        delete profileDirectory;
}


/**
 * @fn copyProfile
 */
bool NetctlProfile::copyProfile(const QString oldPath)
{
    if (debug) qDebug() << "[NetctlProfile]" << "[copyProfile]";
    if (debug) qDebug() << "[NetctlProfile]" << "[copyProfile]" << ":" << "Path" << oldPath;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[NetctlProfile]" << "[profileDirectory]" << ":" << "Could not find directory";
        return false;
    }
    if (sudoCommand == 0) {
        if (debug) qDebug() << "[NetctlProfile]" << "[profileDirectory]" << ":" << "Could not find sudo";
        return false;
    }

    QProcess command;
    QString newPath = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(oldPath).fileName();
    QString commandText = sudoCommand + QString(" /usr/bin/mv ") + oldPath + QString(" ") + newPath;
    if (debug) qDebug() << "[NetctlProfile]" << "[copyProfile]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


/**
 * @fn createProfile
 */
QString NetctlProfile::createProfile(const QString profile, const QMap<QString, QString> settings)
{
    if (debug) qDebug() << "[NetctlProfile]" << "[createProfile]";
    if (debug) qDebug() << "[NetctlProfile]" << "[createProfile]" << ":" << "Profile" << profile;

    QString profileTempName = QDir::homePath() + QDir::separator() +
            QString(".cache") + QDir::separator() + QFileInfo(profile).fileName();
    QFile profileFile(profileTempName);
    if (debug) qDebug() << "[NetctlProfile]" << "[createProfile]" << ":" << "Save to" << profileTempName;
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
 * @fn getSettingsFromProfile
 */
QMap<QString, QString> NetctlProfile::getSettingsFromProfile(const QString profile)
{
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]";
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Profile" << profile;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Could not find directory";
        return QMap<QString, QString>();
    }

    // getting variables list
    // system variables
    QProcess shell;
    QString cmd = QString("env -i bash -c \"set\"");
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Run cmd" << cmd;
    shell.start(cmd);
    shell.waitForFinished(-1);
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Cmd returns" << shell.exitCode();
    QStringList output = QString(shell.readAllStandardOutput()).trimmed().split(QChar('\n'));
    QStringList systemVariables;
    systemVariables.append(QString("PIPESTATUS"));
    for (int i=0; i<output.count(); i++)
        systemVariables.append(output[i].split(QChar('='))[0]);
    // profile variables
    QMap<QString, QString> settings;
    QString profileUrl = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(profile).fileName();
    cmd = QString("env -i bash -c \"source ") + profileUrl + QString("; set\"");
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Run cmd" << cmd;
    shell.start(cmd);
    shell.waitForFinished(-1);
    if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << "Cmd returns" << shell.exitCode();
    output = QString(shell.readAllStandardOutput()).trimmed().split(QChar('\n'));

    // gettings variables
    QStringList keys;
    for (int i=0; i<output.count(); i++)
        if (!systemVariables.contains(output[i].split(QChar('='))[0]))
            keys.append(output[i].split(QChar('='))[0]);
    for (int i=0; i<keys.count(); i++){
        cmd = QString("env -i bash -c \"source ") + profileUrl +
                QString("; for i in ${!") + keys[i] + QString("[@]}; do echo ${") +
                keys[i] + QString("[$i]}; done\"");
        shell.start(cmd);
        shell.waitForFinished(-1);
        settings[keys[i]] = shell.readAllStandardOutput().trimmed();
        if (debug) qDebug() << "[NetctlProfile]" << "[getSettingsFromProfile]" << ":" << keys[i] << "=" << settings[keys[i]];
    }

    return settings;
}


/**
 * @fn ValueFromProfile
 */
QString NetctlProfile::getValueFromProfile(const QString profile, const QString key)
{
    if (debug) qDebug() << "[NetctlProfile]" << "[getValueFromProfile]";
    if (debug) qDebug() << "[NetctlProfile]" << "[getValueFromProfile]" << ":" << "Profile" << profile;
    if (debug) qDebug() << "[NetctlProfile]" << "[getValueFromProfile]" << ":" << "Key" << key;

    QMap<QString, QString> settings = getSettingsFromProfile(profile);

    if (settings.contains(key))
        return settings[key];
    else
        return QString("");
}


/**
 * @fn removeProfile
 */
bool NetctlProfile::removeProfile(const QString profile)
{
    if (debug) qDebug() << "[NetctlProfile]" << "[removeProfile]";
    if (debug) qDebug() << "[NetctlProfile]" << "[removeProfile]" << ":" << "Profile" << profile;
    if (profileDirectory == 0) {
        if (debug) qDebug() << "[NetctlProfile]" << "[removeProfile]" << ":" << "Could not find directory";
        return false;
    }
    if (sudoCommand == 0) {
        if (debug) qDebug() << "[NetctlProfile]" << "[removeProfile]" << ":" << "Could not find sudo";
        return false;
    }

    QProcess command;
    QString profilePath = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(profile).fileName();
    QString commandText = sudoCommand + QString(" /usr/bin/rm ") + profilePath;
    if (debug) qDebug() << "[NetctlProfile]" << "[removeProfile]" << ":" << "Run cmd" << commandText;
    command.start(commandText);
    command.waitForFinished(-1);

    if (command.exitCode() == 0)
        return true;
    else
        return false;
}
