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

#include <KI18n/KLocalizedString>
#include <KNotifications/KNotification>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>
#include <QProcessEnvironment>

#include <pdebug/pdebug.h>

#include "netctladds.h"
#include "version.h"


NetctlAdds::NetctlAdds(QObject *parent)
    : QObject(parent)
{
    // debug
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString debugEnv = environment.value(QString("NETCTLGUI_DEBUG"), QString("no"));
    debug = (debugEnv == QString("yes"));
}


NetctlAdds::~NetctlAdds()
{
    if (debug) qDebug() << PDEBUG;
}


bool NetctlAdds::checkHelperStatus(const bool useHelper)
{
    if (debug) qDebug() << PDEBUG;

    if (useHelper)
        return !sendDBusRequest(QString("Active"), QList<QVariant>()).isEmpty();
    else
        return useHelper;
}


void NetctlAdds::startHelper(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;

    QProcess command;

    command.startDetached(cmd);
}


QList<QVariant> NetctlAdds::sendDBusRequest(const QString cmd, const QList<QVariant> args)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "cmd" << cmd;
    if (debug) qDebug() << PDEBUG << ":" << "args" << args;

    QDBusConnection bus = QDBusConnection::systemBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_HELPER_SERVICE, DBUS_CTRL_PATH,
                                                          DBUS_HELPER_INTERFACE, cmd);
    if (!args.isEmpty())
        request.setArguments(args);
    QDBusMessage response = bus.call(request, QDBus::BlockWithGui);
    QList<QVariant> arguments = response.arguments();
    if (arguments.size() == 0)
        if (debug) qDebug() << PDEBUG << ":" << "Error message" << response.errorMessage();

    return arguments;
}


QString NetctlAdds::getAboutText(const QString type)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Type" << type;

    QString text;
    if (type == QString("header"))
        text = QString(NAME);
    else if (type == QString("description"))
        text = i18n("KDE widget which interacts with netctl.");
    else if (type == QString("links"))
        text = i18n("Links:") + QString("<br>") +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(HOMEPAGE)).arg(i18n("Homepage")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(REPOSITORY)).arg(i18n("Repository")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(BUGTRACKER)).arg(i18n("Bugtracker")) +
               QString("<a href=\"%1\">%2</a><br>").arg(QString(TRANSLATION)).arg(i18n("Translation issue")) +
               QString("<a href=\"%1\">%2</a>").arg(QString(AUR_PACKAGES)).arg(i18n("AUR packages"));
    else if (type == QString("copy"))
        text = QString("<small>&copy; %1 <a href=\"mailto:%2\">%3</a><br>").arg(QString(DATE)).arg(QString(EMAIL)).arg(QString(AUTHOR)) +
               i18n("This software is licensed under %1", QString(LICENSE)) + QString("</small>");
    else if (type == QString("translators"))
        text = i18n("Translators: %1", QString(TRANSLATORS));
    else if (type == QString("3rdparty")) {
        QStringList trdPartyList = QString(TRDPARTY_LICENSE).split(QChar(';'), QString::SkipEmptyParts);
        for (int i=0; i<trdPartyList.count(); i++)
            trdPartyList[i] = QString("<a href=\"%3\">%1</a> (%2 license)")
                    .arg(trdPartyList[i].split(QChar(','))[0])
                    .arg(trdPartyList[i].split(QChar(','))[1])
                    .arg(trdPartyList[i].split(QChar(','))[2]);
        text = i18n("This software uses: %1", trdPartyList.join(QString(", ")));
    }

    return text;
}


QString NetctlAdds::getInfo(const QString current, const QString status)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Current profiles" << current;
    if (debug) qDebug() << PDEBUG << ":" << "Statuses" << status;

    QStringList profiles;
    for (int i=0; i<current.split(QChar('|')).count(); i++)
        profiles.append(current.split(QChar('|'))[i] +
                QString(" (") + status.split(QChar('|'))[i] + QString(")"));

    return profiles.join(QString(" | "));
}


QString NetctlAdds::parsePattern(const QString pattern, const QMap<QString, QVariant> dict)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Dictionary" << dict;

    QString parsed = pattern;
    for (int i=0; i<dict.keys().count(); i++)
        parsed.replace(QString("$") + dict.keys()[i], dict[dict.keys()[i]].toString());
    // fix newline
    parsed.replace(QString("\n"), QString("<br>"));

    return parsed;
}


void NetctlAdds::runCmd(const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Cmd" << cmd;

    QProcess command;

    command.startDetached(cmd);
}


void NetctlAdds::sendNotification(const QString eventId, const QString message)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Event" << eventId;
    if (debug) qDebug() << PDEBUG << ":" << "Message" << message;

    KNotification::event(eventId, QString("Netctl ::: ") + eventId, message);
}


// context menu
void NetctlAdds::enableProfileSlot(const QMap<QString, QVariant> dict, const bool useHelper,
                                   const QString cmd, const QString sudoCmd)
{
    if (debug) qDebug() << PDEBUG;

    QString enableStatus = QString("");
    if (dict[QString("status")].toString().contains(QString("enabled"))) {
        enableStatus = QString(" disable ");
        sendNotification(QString("Info"), i18n("Set profile %1 disabled", dict[QString("current")].toString()));
    } else {
        enableStatus = QString(" enable ");
        sendNotification(QString("Info"), i18n("Set profile %1 enabled", dict[QString("current")].toString()));
    }
    if (useHelper) {
        QList<QVariant> args;
        args.append(dict[QString("current")].toString());
        sendDBusRequest(QString("Enable"), args);
    } else {
        QProcess command;
        QString commandLine = sudoCmd + QString(" ") + cmd + enableStatus + dict[QString("current")].toString();
        command.startDetached(commandLine);
    }
}


void NetctlAdds::restartProfileSlot(const QMap<QString, QVariant> dict, const bool useHelper,
                                    const QString cmd, const QString sudoCmd)
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Restart profile %1", dict[QString("current")].toString()));
    if (useHelper) {
        QList<QVariant> args;
        args.append(dict[QString("current")].toString());
        sendDBusRequest(QString("Restart"), args);
    } else {
        QProcess command;
        QString commandLine = sudoCmd + QString(" ") + cmd + QString(" restart ") + dict[QString("current")].toString();
        command.startDetached(commandLine);
    }
}


void NetctlAdds::startProfileSlot(QString profile, const bool status,
                                  const bool useHelper, const QString cmd, const QString sudoCmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    profile.remove(QChar('&'));
    sendNotification(QString("Info"), i18n("Start profile %1", profile));
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        if (status)
            sendDBusRequest(QString("SwitchTo"), args);
        else
            sendDBusRequest(QString("Start"), args);
    } else {
        QProcess command;
        QString commandLine = sudoCmd + QString(" ") + cmd;
        if (status)
            commandLine += QString(" switch-to ") + profile;
        else
            commandLine += QString(" start ") + profile;
        command.startDetached(commandLine);
    }
}


void NetctlAdds::stopProfileSlot(const QMap<QString, QVariant> dict, const bool useHelper,
                                 const QString cmd, const QString sudoCmd)
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Stop profile %1", dict[QString("current")].toString()));
    if (useHelper) {
        QList<QVariant> args;
        args.append(dict[QString("current")].toString());
        sendDBusRequest(QString("Start"), args);
    } else {
        QProcess command;
        QString commandLine = sudoCmd + QString(" ") + cmd + QString(" stop ") + dict[QString("current")].toString();
        command.startDetached(commandLine);
    }
}


void NetctlAdds::stopAllProfilesSlot(const bool useHelper, const QString cmd, const QString sudoCmd)
{
    if (debug) qDebug() << PDEBUG;

    sendNotification(QString("Info"), i18n("Stop all profiles"));
    if (useHelper)
        sendDBusRequest(QString("StopAll"), QList<QVariant>());
    else {
        QProcess command;
        QString commandLine = sudoCmd + QString(" ") + cmd + QString(" stop-all");
        command.startDetached(commandLine);
    }
}


void NetctlAdds::switchToProfileSlot(QString profile, const bool useHelper,
                                     const QString cmd)
{
    if (debug) qDebug() << PDEBUG;
    if (debug) qDebug() << PDEBUG << ":" << "Profile" << profile;

    profile.remove(QChar('&'));
    sendNotification(QString("Info"), i18n("Switch to profile %1", profile));
    if (useHelper) {
        QList<QVariant> args;
        args.append(profile);
        sendDBusRequest(QString("autoStart"), args);
    } else {
        QProcess command;
        QString commandLine = cmd + QString(" switch-to ") + profile;
        command.startDetached(commandLine);
    }
}
