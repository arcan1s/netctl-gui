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
#include <QTranslator>
#include <iostream>

#include "language.h"
#include "mainwindow.h"
#include "version.h"


using namespace std;


QChar isParametrEnable(const bool parametr)
{
    if (parametr)
        return QChar('*');
    else
        return QChar(' ');
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // translation
    QString configPath = QDir::homePath() + QDir::separator() + QString(".config") +
            QDir::separator() + QString("netctl-gui.conf");
    QString language = Language::defineLanguage(configPath);
    QTranslator translator;
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    // reading command line flags
    bool error = false;
    // windows
    bool showAbout = false;
    bool showNetctlAuto = false;
    bool showSettings = false;
    // main functions
    QString selectEssid = QString("ESSID");
    QString openProfile = QString("PROFILE");
    QString selectProfile = QString("PROFILE");
    // additional functions
    bool debug = false;
    bool defaultSettings = false;
    QString options = QString("OPTIONS");
    int tabNumber = 1;
    // messages
    bool showVersion = false;
    bool showInfo = false;
    bool showHelp = false;
    // reading
    for (int i=1; i<argc; i++) {
        // windows
        // about
        if (QString(argv[i]) == QString("--about")) {
            showAbout = true;
        }
        // netctl-auto
        else if (QString(argv[i]) == QString("--netctl-auto")) {
            showNetctlAuto = true;
        }
        // settings
        else if (QString(argv[i]) == QString("--settings")) {
            showSettings = true;
        }
        // main functions
        // select ESSID
        else if ((QString(argv[i]) == QString("-e")) || (QString(argv[i]) == QString("--essid"))) {
            selectEssid = QString(argv[i+1]);
            i++;
        }
        // open profile
        else if ((QString(argv[i]) == QString("-o")) || (QString(argv[i]) == QString("--open"))) {
            openProfile = QString(argv[i+1]);
            i++;
        }
        // select profile
        else if ((QString(argv[i]) == QString("-s")) || (QString(argv[i]) == QString("--select"))) {
            selectProfile = QString(argv[i+1]);
            i++;
        }
        // additional functions
        // debug
        else if ((QString(argv[i]) == QString("-d")) || (QString(argv[i]) == QString("--debug"))) {
            debug = true;
        }
        // default settings
        else if (QString(argv[i]) == QString("--default")) {
            defaultSettings = true;
        }
        // options
        else if (QString(argv[i]) == QString("--set-opts")) {
            options = QString(argv[i+1]);
            i++;
        }
        // tab number
        else if ((QString(argv[i]) == QString("-t")) || (QString(argv[i]) == QString("--tab"))) {
            if (atoi(argv[i+1]) > 3)
                tabNumber = 3;
            else if (atoi(argv[i+1]) < 1)
                tabNumber = 1;
            else
                tabNumber = atoi(argv[i+1]);
            i++;
        }
        // messages
        // version message
        else if ((QString(argv[i]) == QString("-v")) || (QString(argv[i]) == QString("--version"))) {
            showVersion = true;
        }
        // info message
        else if ((QString(argv[i]) == QString("-i")) || (QString(argv[i]) == QString("--info"))) {
            showInfo = true;
        }
        // help message
        else if ((QString(argv[i]) == QString("-h")) || (QString(argv[i]) == QString("--help"))) {
            showHelp = true;
        }
        else {
            error = true;
        }
    }
    if (selectEssid != QString("ESSID"))
        tabNumber = 3;
    if (openProfile != QString("PROFILE"))
        tabNumber = 2;
    if (selectProfile != QString("PROFILE"))
        tabNumber = 1;

    // messages
    QString errorMessage = QApplication::translate("MainWindow", "Unknown flag\n");

    QString helpMessage = QString("");
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Usage:"));
    helpMessage += QString("netctl-gui [ --about ] [ --netctl-auto ] [ --settings ]\n");
    helpMessage += QString("           [ -e ESSID | --essid ESSID ] [ -o PROFILE | --open PROFILE ]\n");
    helpMessage += QString("           [ -s PROFILE | --select PROFILE ]\n");
    helpMessage += QString("           [ -d | --debug ] [ --default ] [ --set-opts OPTIONS ]\n");
    helpMessage += QString("           [ -t NUM | --tab NUM ]\n");
    helpMessage += QString("           [ -v | --version ] [ -i | --info ] [ -h | --help]\n\n");
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Parametrs:"));
    // windows
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Open window:"));
    helpMessage += QString("%1                  --about               - %2\n")
            .arg(isParametrEnable(showAbout))
            .arg(QApplication::translate("MainWindow", "show about window"));
    helpMessage += QString("%1                  --netctl-auto         - %2\n")
            .arg(isParametrEnable(showNetctlAuto))
            .arg(QApplication::translate("MainWindow", "show netctl-auto window"));
    helpMessage += QString("%1                  --settings            - %2\n")
            .arg(isParametrEnable(showSettings))
            .arg(QApplication::translate("MainWindow", "show settings window"));
    // main functions
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Functions:"));
    helpMessage += QString("   -e %1   --essid %1    - %2\n")
            .arg(selectEssid, -10)
            .arg(QApplication::translate("MainWindow", "select ESSID %1").arg(selectEssid));
    helpMessage += QString("   -o %1   --open %1     - %2\n")
            .arg(openProfile, -10)
            .arg(QApplication::translate("MainWindow", "open profile %1").arg(openProfile));
    helpMessage += QString("   -s %1   --select %1   - %2\n")
            .arg(selectProfile, -10)
            .arg(QApplication::translate("MainWindow", "select profile %1").arg(selectProfile));
    // additional functions
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Additional flags:"));
    helpMessage += QString("%1  -d              --debug               - %2\n")
            .arg(isParametrEnable(debug))
            .arg(QApplication::translate("MainWindow", "print debug information"));
    helpMessage += QString("%1                  --default             - %2\n")
            .arg(isParametrEnable(defaultSettings))
            .arg(QApplication::translate("MainWindow", "start with default settings"));
    helpMessage += QString("                   --set-opts %1\n")
            .arg(options, -10);
    helpMessage += QString("                                         - %1\n")
            .arg(QApplication::translate("MainWindow", "set options for this run, comma separated"));
    helpMessage += QString("   -t %1          --tab %1             - %2\n")
            .arg(QString::number(tabNumber), -3)
            .arg(QApplication::translate("MainWindow", "open a tab with number %1").arg(QString::number(tabNumber)));
    // messages
    helpMessage += QString("%1\n").arg(QApplication::translate("MainWindow", "Show messages:"));
    helpMessage += QString("   -v           --version                - %1\n")
            .arg(QApplication::translate("MainWindow", "show version and exit"));
    helpMessage += QString("   -i           --info                   - %1\n")
            .arg(QApplication::translate("MainWindow", "show build information and exit"));
    helpMessage += QString("   -h           --help                   - %1\n")
            .arg(QApplication::translate("MainWindow", "show this help and exit"));

    QString infoMessage = QString("");
    infoMessage += QApplication::translate("MainWindow", "Build date: %1").
            arg(QString(BUILD_DATE));
    infoMessage += QString("\n%1\n").arg(QApplication::translate("MainWindow", "cmake flags:"));
    infoMessage += QString("\t-DCMAKE_BUILD_TYPE=%1 \\\n").arg(QString(CMAKE_BUILD_TYPE));
    infoMessage += QString("\t-DCMAKE_INSTALL_PREFIX=%1 \\\n").arg(QString(CMAKE_INSTALL_PREFIX));
    infoMessage += QString("\t-DBUILD_DOCS=%1 \\\n").arg(QString(PROJECT_BUILD_DOCS));
    infoMessage += QString("\t-DBUILD_LIBRARY=%1 \\\n").arg(QString(PROJECT_BUILD_LIBRARY));
    infoMessage += QString("\t-DBUILD_GUI=%1 \\\n").arg(QString(PROJECT_BUILD_GUI));
    infoMessage += QString("\t-DUSE_QT5=%1 \\\n").arg(QString(PROJECT_USE_QT5));
    infoMessage += QString("\t-DBUILD_DATAENGINE=%1 \\\n").arg(QString(PROJECT_BUILD_DATAENGINE));
    infoMessage += QString("\t-DBUILD_PLASMOID=%1\n").arg(QString(PROJECT_BUILD_PLASMOID));

    QString versionMessage = QString("");
    versionMessage += QString("%1\n").arg(QString(NAME));
    versionMessage += QApplication::translate("MainWindow", "Version : %1\n").arg(QString(VERSION));
    versionMessage += QApplication::translate("MainWindow", "Author : %1\n").arg(QString(AUTHOR));
    versionMessage += QApplication::translate("MainWindow", "License : %1\n").arg(QString(LICENSE));

    // reread translations
    a.removeTranslator(&translator);
    language = Language::defineLanguage(configPath, options);
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    // running
    if (error) {
        cout << errorMessage.toUtf8().data() << endl;
        cout << helpMessage.toUtf8().data();
        return 127;
    }
    if (showInfo) {
        cout << versionMessage.toUtf8().data() << endl;
        cout << infoMessage.toUtf8().data();
        return 0;
    }
    if (showHelp) {
        cout << helpMessage.toUtf8().data();
        return 0;
    }
    if (showVersion) {
        cout << versionMessage.toUtf8().data();
        return 0;
    }
    MainWindow w(0,
                 showAbout, showNetctlAuto, showSettings,
                 selectEssid, openProfile, selectProfile,
                 debug, defaultSettings, options, tabNumber);
    w.show();
    return a.exec();
}
