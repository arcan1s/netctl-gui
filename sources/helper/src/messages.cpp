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


#include <QCoreApplication>
#include <QDir>

#include "messages.h"
#include "version.h"


QString errorMessage()
{
    return QCoreApplication::translate("NetctlHelper", "Unknown flag\n");
}


QMap<QString, QVariant> getArgs()
{
    QMap<QString, QVariant> args;
    args[QString("config")] = QString("%1/.config/netctl-gui.conf").arg(QDir::homePath());
    args[QString("debug")] = false;
    args[QString("nodaemon")] = false;
    args[QString("session")] = false;
    args[QString("state")] = (int) 0;
    args[QString("system")] = false;
    args[QString("help")] = false;
    args[QString("info")] = false;
    args[QString("version")] = false;
    args[QString("error")] = false;

    return args;
}


QString helpMessage()
{
    QString helpMessage = QString("");
    helpMessage += QString("%1\n").arg(QCoreApplication::translate("NetctlHelper", "Usage:"));
    helpMessage += QString("netctlgui-helper [ options ]\n");
    helpMessage += QString("%1\n").arg(QCoreApplication::translate("NetctlHelper", "Options:"));
    // windows
    helpMessage += QString("   -c, --config <arg>    - %1\n").arg(QCoreApplication::translate("NetctlHelper", "read configuration from this file"));
    helpMessage += QString("   -d, --debug           - %1\n").arg(QCoreApplication::translate("NetctlHelper", "print debug information"));
    helpMessage += QString("       --nodaemon        - %1\n").arg(QCoreApplication::translate("NetctlHelper", "do not start as daemon"));
    helpMessage += QString("       --replace         - %1\n").arg(QCoreApplication::translate("NetctlHelper", "force replace the existing session"));
    helpMessage += QString("       --restore         - %1\n").arg(QCoreApplication::translate("NetctlHelper", "force restore the existing session"));
    helpMessage += QString("       --session         - %1\n").arg(QCoreApplication::translate("NetctlHelper", "force create user DBus session"));
    helpMessage += QString("       --system          - %1\n").arg(QCoreApplication::translate("NetctlHelper", "do not read user configuration, system-wide only"));
    helpMessage += QString(" %1\n").arg(QCoreApplication::translate("NetctlHelper", "Show messages:"));
    helpMessage += QString("   -v, --version         - %1\n").arg(QCoreApplication::translate("NetctlHelper", "show version and exit"));
    helpMessage += QString("   -i, --info            - %1\n").arg(QCoreApplication::translate("NetctlHelper", "show build information and exit"));
    helpMessage += QString("   -h, --help            - %1\n").arg(QCoreApplication::translate("NetctlHelper", "show this help and exit"));

    return helpMessage;
}


QString infoMessage()
{
    QString infoMessage = QString("");
    // build information
    infoMessage += QCoreApplication::translate("NetctlHelper", "Build date: %1").
            arg(QString(BUILD_DATE));
    // cmake
    infoMessage += QString("\n%1:\n").arg(QCoreApplication::translate("NetctlHelper", "cmake flags"));
    // cmake properties
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "cmake properties"));
    infoMessage += QString("\t-DCMAKE_BUILD_TYPE=%1\n").arg(QString(CMAKE_BUILD_TYPE));
    infoMessage += QString("\t-DCMAKE_INSTALL_PREFIX=%1\n").arg(QString(CMAKE_INSTALL_PREFIX));
    // components
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "Components"));
    infoMessage += QString("\t-DBUILD_DATAENGINE=%1\n").arg(QString(PROJECT_BUILD_DATAENGINE));
    infoMessage += QString("\t-DBUILD_GUI=%1\n").arg(QString(PROJECT_BUILD_GUI));
    infoMessage += QString("\t-DBUILD_HELPER=%1\n").arg(QString(PROJECT_BUILD_HELPER));
    infoMessage += QString("\t-DBUILD_LIBRARY=%1\n").arg(QString(PROJECT_BUILD_LIBRARY));
    infoMessage += QString("\t-DBUILD_PLASMOID=%1\n").arg(QString(PROJECT_BUILD_PLASMOID));
    // additional components
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "Additional components"));
    infoMessage += QString("\t-DBUILD_DOCS=%1\n").arg(QString(PROJECT_BUILD_DOCS));
    infoMessage += QString("\t-DBUILD_TEST=%1\n").arg(QString(PROJECT_BUILD_TEST));
    // project properties
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "Project properties"));
    infoMessage += QString("\t-DDBUS_SYSTEMCONF_PATH=%1\n").arg(QString(PROJECT_DBUS_SYSTEMCONF_PATH));
    infoMessage += QString("\t-DSYSTEMD_SERVICE_PATH=%1\n").arg(QString(PROJECT_SYSTEMD_SERVICE_PATH));
    infoMessage += QString("\t-DUSE_CAPABILITIES=%1\n").arg(QString(PROJECT_USE_CAPABILITIES));
    infoMessage += QString("\t-DUSE_KDE4=%1\n").arg(QString(PROJECT_USE_KDE4));
    infoMessage += QString("\t-DUSE_QT5=%1\n").arg(QString(PROJECT_USE_QT5));
    // transport information
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "DBus configuration"));
    infoMessage += QString("\tDBUS_SERVICE=%1\n").arg(QString(DBUS_SERVICE));
    infoMessage += QString("\tDBUS_INTERFACE=%1\n").arg(QString(DBUS_INTERFACE));
    infoMessage += QString("\tDBUS_OBJECT_PATH=%1\n").arg(QString(DBUS_OBJECT_PATH));
    infoMessage += QString("\tDBUS_HELPER_SERVICE=%1\n").arg(QString(DBUS_HELPER_SERVICE));
    infoMessage += QString("\tDBUS_HELPER_INTERFACE=%1\n").arg(QString(DBUS_HELPER_INTERFACE));
    infoMessage += QString("\tDBUS_CTRL_PATH=%1\n").arg(QString(DBUS_CTRL_PATH));
    infoMessage += QString("\tDBUS_LIB_PATH=%1\n").arg(QString(DBUS_LIB_PATH));
    // docs path
    infoMessage += QString("%1:\n").arg(QCoreApplication::translate("NetctlHelper", "Documentation"));
    infoMessage += QString("\tDOCS_PATH=%1\n").arg(QString(DOCS_PATH));

    return infoMessage;
}


QString versionMessage()
{
    QString versionMessage = QString("");
    versionMessage += QString("%1\n").arg(QString(NAME));
    versionMessage += QString("%1 : %2\n").arg(QCoreApplication::translate("NetctlHelper", "Version")).arg(QString(VERSION));
    versionMessage += QString("%1 : %2\n").arg(QCoreApplication::translate("NetctlHelper", "Author")).arg(QString(AUTHOR));
    versionMessage += QString("%1 : %2\n").arg(QCoreApplication::translate("NetctlHelper", "License")).arg(QString(LICENSE));

    return versionMessage;
}
