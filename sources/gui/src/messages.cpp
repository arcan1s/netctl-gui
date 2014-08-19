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
#include <QDir>

#include "messages.h"
#include "version.h"


QString errorMessage()
{
    QString errorMessage = QApplication::translate("MainWindow", "Unknown flag\n");

    return errorMessage;
}


QMap<QString, QVariant> getArgs()
{
    QMap<QString, QVariant> args;
    // windows
    args[QString("detached")] = false;
    args[QString("minimized")] = (int) 0;
    args[QString("about")] = false;
    args[QString("auto")] = false;
    args[QString("settings")] = false;
    // main functions
    args[QString("essid")] = QString("ESSID");
    args[QString("open")] = QString("PROFILE");
    args[QString("select")] = QString("PROFILE");
    // additional functions
    args[QString("config")] = QString(QDir::homePath() + QString("/.config/netctl-gui.conf"));
    args[QString("debug")] = false;
    args[QString("defaults")] = false;
    args[QString("options")] = QString("OPTIONS");
    args[QString("tab")] = (int) 1;
    // messages
    args[QString("error")] = false;
    args[QString("help")] = false;
    args[QString("info")] = false;
    args[QString("version")] = false;

    return args;
}


QString helpMessage()
{
    QString helpMessage = QString("");
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Usage:"));
    helpMessage += QString("netctl-gui [ options ]\n");
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Options:"));
    // windows
    helpMessage += QString(" %1\n").arg(QApplication::translate("MainWindow", "Open window:"));
    helpMessage += QString("       --detached        - %1\n")
            .arg(QApplication::translate("MainWindow", "start detached from console"));
    helpMessage += QString("       --maximized       - %1\n")
            .arg(QApplication::translate("MainWindow", "start maximized"));
    helpMessage += QString("       --minimized       - %1\n")
            .arg(QApplication::translate("MainWindow", "start minimized to tray"));
    helpMessage += QString("       --about           - %1\n")
            .arg(QApplication::translate("MainWindow", "show about window"));
    helpMessage += QString("       --netctl-auto     - %1\n")
            .arg(QApplication::translate("MainWindow", "show netctl-auto window"));
    helpMessage += QString("       --settings        - %1\n")
            .arg(QApplication::translate("MainWindow", "show settings window"));
    // main functions
    helpMessage += QString(" %1\n").arg(QApplication::translate("MainWindow", "Functions:"));
    helpMessage += QString("   -e, --essid <arg>     - %1\n")
            .arg(QApplication::translate("MainWindow", "select this ESSID"));
    helpMessage += QString("   -o, --open <arg>      - %1\n")
            .arg(QApplication::translate("MainWindow", "open this profile"));
    helpMessage += QString("   -s, --select <arg>    - %1\n")
            .arg(QApplication::translate("MainWindow", "select this profile"));
    // additional functions
    helpMessage += QString(" %1\n").arg(QApplication::translate("MainWindow", "Additional flags:"));
    helpMessage += QString("   -c, --config <arg>    - %1\n")
            .arg(QApplication::translate("MainWindow", "read configuration from this file"));
    helpMessage += QString("   -d, --debug           - %1\n")
            .arg(QApplication::translate("MainWindow", "print debug information"));
    helpMessage += QString("       --default         - %1\n")
            .arg(QApplication::translate("MainWindow", "start with default settings"));
    helpMessage += QString("       --set-opts <arg>  - %1\n")
            .arg(QApplication::translate("MainWindow", "set options for this run, comma separated"));
    helpMessage += QString("   -t, --tab <arg>       - %1\n")
            .arg(QApplication::translate("MainWindow", "open a tab with this number"));
    // messages
    helpMessage += QString(" %1\n").arg(QApplication::translate("MainWindow", "Show messages:"));
    helpMessage += QString("   -v, --version         - %1\n")
            .arg(QApplication::translate("MainWindow", "show version and exit"));
    helpMessage += QString("   -i, --info            - %1\n")
            .arg(QApplication::translate("MainWindow", "show build information and exit"));
    helpMessage += QString("   -h, --help            - %1\n")
            .arg(QApplication::translate("MainWindow", "show this help and exit"));

    return helpMessage;
}


QString infoMessage()
{
    QString infoMessage = QString("");
    // build information
    infoMessage += QCoreApplication::translate("MainWindow", "Build date: %1").
            arg(QString(BUILD_DATE));
    infoMessage += QString("\n%1:\n").arg(QCoreApplication::translate("MainWindow", "cmake flags"));
    infoMessage += QString("\t-DCMAKE_BUILD_TYPE=%1 \\\n").arg(QString(CMAKE_BUILD_TYPE));
    infoMessage += QString("\t-DCMAKE_INSTALL_PREFIX=%1 \\\n").arg(QString(CMAKE_INSTALL_PREFIX));
    infoMessage += QString("\t-DBUILD_DOCS=%1 \\\n").arg(QString(PROJECT_BUILD_DOCS));
    infoMessage += QString("\t-DBUILD_LIBRARY=%1 \\\n").arg(QString(PROJECT_BUILD_LIBRARY));
    infoMessage += QString("\t-DBUILD_GUI=%1 \\\n").arg(QString(PROJECT_BUILD_GUI));
    infoMessage += QString("\t-DUSE_QT5=%1 \\\n").arg(QString(PROJECT_USE_QT5));
    infoMessage += QString("\t-DBUILD_DATAENGINE=%1 \\\n").arg(QString(PROJECT_BUILD_DATAENGINE));
    infoMessage += QString("\t-DBUILD_PLASMOID=%1\n").arg(QString(PROJECT_BUILD_PLASMOID));
    // transport information
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("MainWindow", "DBus configuration"));
    infoMessage += QString("\tDBUS_SERVICE=%1\n").arg(QString(DBUS_SERVICE));
    infoMessage += QString("\tDBUS_INTERFACE=%1\n").arg(QString(DBUS_INTERFACE));
    infoMessage += QString("\tDBUS_OBJECT_PATH=%1\n").arg(QString(DBUS_OBJECT_PATH));
    infoMessage += QString("\tDBUS_HELPER_SERVICE=%1\n").arg(QString(DBUS_HELPER_SERVICE));
    infoMessage += QString("\tDBUS_HELPER_INTERFACE=%1\n").arg(QString(DBUS_HELPER_INTERFACE));
    infoMessage += QString("\tDBUS_CTRL_PATH=%1\n").arg(QString(DBUS_CTRL_PATH));
    infoMessage += QString("\tDBUS_LIB_PATH=%1\n").arg(QString(DBUS_LIB_PATH));
    // docs path
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("MainWindow", "Documentation"));
    infoMessage += QString("\tDOCS_PATH=%1\n").arg(QString(DOCS_PATH));
    QStringList docs = QDir(QString(DOCS_PATH)).entryList(QDir::NoDotAndDotDot);
    for (int i=0; i<docs.count(); i++)
        infoMessage += QString("\t%1\n").arg(docs[i]);

    return infoMessage;
}


QString versionMessage()
{
    QString versionMessage = QString("");
    versionMessage += QString("%1\n").arg(QString(NAME));
    versionMessage += QString("%1 : %2\n")
            .arg(QApplication::translate("MainWindow", "Version"))
            .arg(QString(VERSION));
    versionMessage += QString("%1 : %2\n")
            .arg(QApplication::translate("MainWindow", "Author"))
            .arg(QString(AUTHOR));
    versionMessage += QString("%1 : %2\n")
            .arg(QApplication::translate("MainWindow", "License"))
            .arg(QString(LICENSE));

    return versionMessage;
}
