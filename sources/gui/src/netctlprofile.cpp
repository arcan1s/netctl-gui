/***************************************************************************
 *   This file is part of netctl-plasmoid                                  *
 *                                                                         *
 *   netctl-plasmoid is free software: you can redistribute it and/or      *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   netctl-plasmoid is distributed in the hope that it will be useful,    *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with netctl-plasmoid. If not, see http://www.gnu.org/licenses/  *
 ***************************************************************************/

#include "netctlprofile.h"

#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

#include "mainwindow.h"
#include <cstdio>


NetctlProfile::NetctlProfile(MainWindow *wid, QMap<QString, QString> settings)
    : parent(wid)
{
    profileDirectory = new QDir(settings[QString("PROFILE_DIR")]);
    sudoCommand = settings[QString("SUDO_PATH")];
}


NetctlProfile::~NetctlProfile()
{
    delete profileDirectory;
}


bool NetctlProfile::copyProfile(const QString oldPath)
{
    QString newPath = profileDirectory->absolutePath() + QDir::separator() + QFileInfo(oldPath).fileName();
    QProcess command;
    command.start(sudoCommand + QString(" /usr/bin/mv ") + oldPath + QString(" ") + newPath);
    command.waitForFinished(-1);
    if (command.exitCode() == 0)
        return true;
    else
        return false;
}


QString NetctlProfile::createProfile(const QString profile, const QMap<QString, QString> settings)
{

    QString profileTempName = QDir::homePath() + QDir::separator() +
            QString(".cache") + QDir::separator() + QFileInfo(profile).fileName();
    QFile profileFile(profileTempName);

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
                (settings.keys()[i] == QString("WPAConfigSection")) ||
                (settings.keys()[i] == QString("WPAConfigSection")))
            out << QString("(") + settings[settings.keys()[i]] << QString(")\n");
        else
            out << settings[settings.keys()[i]] << QString("\n");
    }
    profileFile.close();

    return profileTempName;
}


QMap<QString, QString> NetctlProfile::getSettingsFromProfile(const QString profile)
{
    QMap<QString, QString> settings;
    QFile profileFile;
    QString fileStr;

    if (profile[0] == QDir::separator())
        profileFile.setFileName(profile);
    else
        profileFile.setFileName(profileDirectory->absolutePath() + QDir::separator() + profile);
    if (!profileFile.open(QIODevice::ReadOnly))
        return settings;
    while (true) {
        fileStr = QString(profileFile.readLine());
        if (profileFile.atEnd())
            break;
        else if (fileStr[0] != '#') {
            if (fileStr.split(QString("="), QString::SkipEmptyParts).count() == 2) {
                if ((fileStr.split(QString("="))[1][0] == QChar('(')) &&
                        (fileStr.split(QString("="))[1][fileStr.split(QString("="))[1].size()-2] == QChar(')')))
                    settings[fileStr.split(QString("="))[0]] = fileStr.split(QString("="))[1]
                            .remove(QString("("))
                            .remove(QString(")"))
                            .trimmed();
                else if (fileStr.split(QString("="))[1][0] == QChar('(')) {
                    QString parameterName = fileStr.split(QString("="))[0];
                    QStringList parameter;
                    if (!fileStr.split(QString("="))[1]
                            .remove(QString("("))
                            .remove(QString(")"))
                            .trimmed()
                            .isEmpty())
                        parameter.append(fileStr.split(QString("="))[1]
                                .remove(QString("("))
                                .remove(QString(")"))
                                .trimmed());
                    while(true) {
                        fileStr = QString(profileFile.readLine());
                        if ((profileFile.atEnd()) ||
                                (fileStr[fileStr.size()-2] == QChar(')')))
                            break;
                        if (!fileStr.remove(QString("("))
                                .remove(QString(")"))
                                .trimmed()
                                .isEmpty())
                            parameter.append(fileStr.remove(QString("("))
                                    .remove(QString(")"))
                                    .trimmed());
                    }
                    settings[parameterName] = parameter.join(QString("\n"));
                }
                else
                    settings[fileStr.split(QString("="))[0]] = fileStr.split(QString("="))[1]
                            .trimmed();
            }

        }
    }

    profileFile.close();
    return settings;
}
