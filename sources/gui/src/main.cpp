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


#include <QApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDir>
#include <QTranslator>
#include <iostream>
#include <unistd.h>

#include "language.h"
#include "mainwindow.h"
#include "messages.h"
#include "version.h"


using namespace std;


bool restoreExistSession()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage request = QDBusMessage::createMethodCall(QString(DBUS_SERVICE),
                                                          QString(DBUS_OBJECT_PATH),
                                                          QString(DBUS_INTERFACE),
                                                          QString("Restore"));
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();
    return ((arguments.size() == 1) && arguments[0].toBool());
}


int main(int argc, char *argv[])
{
    // detach from console
    for (int i=0; i<argc; i++)
        if (QString(argv[i]) == QString("--daemon")) {
            daemon(0, 0);
            break;
        }
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    // check if exists
    if (restoreExistSession())
        return 0;

    // config path
    QMap<QString, QVariant> args = getArgs();
    // translation
    QString language = Language::defineLanguage(args[QString("config")].toString());
    QTranslator translator;
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    // reading
    for (int i=1; i<argc; i++) {
        // windows
        // daemonized
        if (QString(argv[i]) == QString("--daemon")) {
            args[QString("minimized")] = (int) 1;
        }
        // maximized
        else if (QString(argv[i]) == QString("--maximized")) {
            args[QString("minimized")] = (int) 2;
        }
        // minimized
        else if (QString(argv[i]) == QString("--minimized")) {
            args[QString("minimized")] = (int) 3;
        }
        // about
        else if (QString(argv[i]) == QString("--about")) {
            args[QString("about")] = true;
        }
        // netctl-auto
        else if (QString(argv[i]) == QString("--netctl-auto")) {
            args[QString("auto")] = true;
        }
        // settings
        else if (QString(argv[i]) == QString("--settings")) {
            args[QString("settings")] = true;
        }
        // main functions
        // select ESSID
        else if ((QString(argv[i]) == QString("-e")) || (QString(argv[i]) == QString("--essid"))) {
            args[QString("essid")] = QString(argv[i+1]);
            i++;
        }
        // open profile
        else if ((QString(argv[i]) == QString("-o")) || (QString(argv[i]) == QString("--open"))) {
            args[QString("open")] = QString(argv[i+1]);
            i++;
        }
        // select profile
        else if ((QString(argv[i]) == QString("-s")) || (QString(argv[i]) == QString("--select"))) {
            args[QString("select")] = QString(argv[i+1]);
            i++;
        }
        // additional functions
        // config path
        else if ((QString(argv[i]) == QString("-c")) || (QString(argv[i]) == QString("--config"))) {
            args[QString("config")] = QDir().absoluteFilePath(argv[i+1]);
            i++;
        }
        // debug
        else if ((QString(argv[i]) == QString("-d")) || (QString(argv[i]) == QString("--debug"))) {
            args[QString("debug")] = true;
        }
        // default settings
        else if (QString(argv[i]) == QString("--default")) {
            args[QString("defaults")] = true;
        }
        // options
        else if (QString(argv[i]) == QString("--set-opts")) {
            args[QString("options")] = QString(argv[i+1]);
            i++;
        }
        // tab number
        else if ((QString(argv[i]) == QString("-t")) || (QString(argv[i]) == QString("--tab"))) {
            if (atoi(argv[i+1]) > 3)
                args[QString("tab")] = (int) 3;
            else if (atoi(argv[i+1]) < 1)
                args[QString("tab")] = (int) 1;
            else
                args[QString("tab")] = atoi(argv[i+1]);
            i++;
        }
        // messages
        // help message
        else if ((QString(argv[i]) == QString("-h")) || (QString(argv[i]) == QString("--help"))) {
            args[QString("help")] = true;
        }
        // info message
        else if ((QString(argv[i]) == QString("-i")) || (QString(argv[i]) == QString("--info"))) {
            args[QString("info")] = true;
        }
        // version message
        else if ((QString(argv[i]) == QString("-v")) || (QString(argv[i]) == QString("--version"))) {
            args[QString("version")] = true;
        }
        else {
            args[QString("error")] = true;
        }
    }
    if (args[QString("essid")].toString() != QString("ESSID"))
        args[QString("tab")] = (int) 3;
    if (args[QString("open")].toString() != QString("PROFILE"))
        args[QString("tab")] = (int) 2;
    if (args[QString("select")].toString() != QString("PROFILE"))
        args[QString("tab")] = (int) 1;

    // reread translations
    a.removeTranslator(&translator);
    language = Language::defineLanguage(args[QString("config")].toString(),
            args[QString("options")].toString());
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    // running
    if (args[QString("error")].toBool()) {
        cout << errorMessage().toUtf8().data() << endl;
        cout << helpMessage().toUtf8().data();
        return 127;
    }
    if (args[QString("help")].toBool()) {
        cout << helpMessage().toUtf8().data();
        return 0;
    }
    if (args[QString("info")].toBool()) {
        cout << versionMessage().toUtf8().data() << endl;
        cout << infoMessage().toUtf8().data();
        return 0;
    }
    if (args[QString("version")].toBool()) {
        cout << versionMessage().toUtf8().data();
        return 0;
    }
    MainWindow w(0, args);
    return a.exec();
}
