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
#include <QLibraryInfo>
#include <QTranslator>
#include <iostream>
#include <unistd.h>

#include "language.h"
#include "mainwindow.h"
#include "messages.h"
#include "version.h"


using namespace std;


bool existingSessionOperation(const QString operation)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_SERVICE, DBUS_OBJECT_PATH,
                                                          DBUS_INTERFACE, operation);
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();

    return !arguments.isEmpty();
}


unsigned int getUidFromSession(const int type = 0)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage request = QDBusMessage::createMethodCall(DBUS_SERVICE, DBUS_OBJECT_PATH,
                                                          DBUS_INTERFACE, QString("UIDs"));
    QDBusMessage response = bus.call(request);
    QList<QVariant> arguments = response.arguments();

    return arguments[0].toStringList()[type].toUInt();
}


int main(int argc, char *argv[])
{
    QMap<QString, QVariant> args = getArgs();
    // reading
    for (int i=1; i<argc; i++) {
        if (QString(argv[i]) == QString("--detached")) {
            // detached
            args[QString("detached")] = true;
        } else if (QString(argv[i]) == QString("--maximized")) {
            // maximized
            args[QString("minimized")] = (int) 1;
        } else if (QString(argv[i]) == QString("--minimized")) {
            // minimized
            args[QString("minimized")] = (int) 2;
        } else if (QString(argv[i]) == QString("--about")) {
            // about
            args[QString("about")] = true;
        } else if (QString(argv[i]) == QString("--netctl-auto")) {
            // netctl-auto
            args[QString("auto")] = true;
        } else if (QString(argv[i]) == QString("--settings")) {
            // settings
            args[QString("settings")] = true;
        } else if ((QString(argv[i]) == QString("-e")) || (QString(argv[i]) == QString("--essid"))) {
            // select ESSID
            args[QString("essid")] = QString(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-o")) || (QString(argv[i]) == QString("--open"))) {
            // open profile
            args[QString("open")] = QString(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-s")) || (QString(argv[i]) == QString("--select"))) {
            // select profile
            args[QString("select")] = QString(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-c")) || (QString(argv[i]) == QString("--config"))) {
            // config path
            args[QString("config")] = QDir().absoluteFilePath(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-d")) || (QString(argv[i]) == QString("--debug"))) {
            // debug
            args[QString("debug")] = true;
        } else if (QString(argv[i]) == QString("--default")) {
            // default settings
            args[QString("default")] = true;
        } else if (QString(argv[i]) == QString("--set-opts")) {
            // options
            args[QString("options")] = QString(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-t")) || (QString(argv[i]) == QString("--tab"))) {
            // tab number
            if (atoi(argv[i+1]) > 3)
                args[QString("tab")] = (int) 3;
            else if (atoi(argv[i+1]) < 1)
                args[QString("tab")] = (int) 1;
            else
                args[QString("tab")] = atoi(argv[i+1]);
            i++;
        } else if ((QString(argv[i]) == QString("-h")) || (QString(argv[i]) == QString("--help"))) {
            // help message
            args[QString("help")] = true;
        } else if ((QString(argv[i]) == QString("-i")) || (QString(argv[i]) == QString("--info"))) {
            // info message
            args[QString("info")] = true;
        } else if ((QString(argv[i]) == QString("-v")) || (QString(argv[i]) == QString("--version"))) {
            // version message
            args[QString("version")] = true;
        } else {
            args[QString("error")] = true;
        }
    }
    if ((args[QString("debug")].toBool()) ||
        (args[QString("help")].toBool()) ||
        (args[QString("info")].toBool()) ||
        (args[QString("version")].toBool()) ||
        (args[QString("error")].toBool()))
        args[QString("detached")] = false;
    if (args[QString("essid")].toString() != QString("ESSID"))
        args[QString("tab")] = (int) 3;
    if (args[QString("open")].toString() != QString("PROFILE"))
        args[QString("tab")] = (int) 2;
    if (args[QString("select")].toString() != QString("PROFILE"))
        args[QString("tab")] = (int) 1;

    // detach from console
    if (args[QString("detached")].toBool())
        daemon(0, 0);
    QApplication a(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);
    // reread translations according to flags
    QString language = Language::defineLanguage(args[QString("config")].toString(),
            args[QString("options")].toString());
    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_") + language, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);
    QTranslator translator;
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    // running
    if (args[QString("error")].toBool()) {
        cout << errorMessage().toUtf8().data() << endl;
        cout << helpMessage().toUtf8().data();
        return 127;
    } else if (args[QString("help")].toBool()) {
        cout << helpMessage().toUtf8().data();
        return 0;
    } else if (args[QString("info")].toBool()) {
        cout << versionMessage().toUtf8().data() << endl;
        cout << infoMessage().toUtf8().data();
        return 0;
    } else if (args[QString("version")].toBool()) {
        cout << versionMessage().toUtf8().data();
        return 0;
    }

    // check if exists
    if (existingSessionOperation(QString("Active"))) {
        if ((getuid() == getUidFromSession(0)) && (geteuid() == getUidFromSession(1))) {
            // restore session
            cout << QCoreApplication::translate("MainWindow", "Restore existing session.")
                    .toUtf8().data() << endl;
            existingSessionOperation(QString("Restore"));
            return 0;
        } else if ((getuid() == getUidFromSession(0)) && (geteuid() != getUidFromSession(1))) {
            cout << QCoreApplication::translate("MainWindow", "Close existing session.")
                    .toUtf8().data() << endl;
            existingSessionOperation(QString("Close"));
        }
    }
    MainWindow w(0, args, &qtTranslator, &translator);
    return a.exec();
}
