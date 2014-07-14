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
    bool debug = false;
    bool defaultSettings = false;
    bool error = false;
    bool showAbout = false;
    bool showHelp = false;
    bool showInfo = false;
    bool showNetctlAuto = false;
    bool showSettings = false;
    bool showVersion = false;
    int tabNumber = 1;
    for (int i=1; i<argc; i++) {
        // about
        if (QString(argv[i]) == QString("--about")) {
            showAbout = true;
        }
        // debug
        else if ((QString(argv[i]) == QString("-d")) || (QString(argv[i]) == QString("--debug"))) {
            debug = true;
        }
        // default settings
        else if (QString(argv[i]) == QString("--default")) {
            defaultSettings = true;
        }
        // netctl-auto
        else if (QString(argv[i]) == QString("--netctl-auto")) {
            showNetctlAuto = true;
        }
        // settings
        else if (QString(argv[i]) == QString("--settings")) {
            showSettings = true;
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

    // messages
    QString errorMessage = QApplication::translate("MainWindow", "Unknown flag\n");

    QString helpMessage = QString("");
    helpMessage += QApplication::translate("MainWindow", "Usage:\n");
    helpMessage += QString("netctl-gui [ --about ] [ -d | --debug ] [ --default ] [ --netctl-auto ]\n");
    helpMessage += QString("           [ --settings ] [ -t NUM | --tab NUM ] [ -v | --version ]\n");
    helpMessage += QString("           [ -i | --info ] [ -h | --help]\n\n");
    helpMessage += QApplication::translate("MainWindow", "Parametrs:\n");
    helpMessage += QString("%1           --about         - %2\n")
            .arg(isParametrEnable(showAbout))
            .arg(QApplication::translate("MainWindow", "show about window"));
    helpMessage += QString("%1  -d       --debug         - %2\n")
            .arg(isParametrEnable(debug))
            .arg(QApplication::translate("MainWindow", "print debug information"));
    helpMessage += QString("%1           --default       - %2\n")
            .arg(isParametrEnable(defaultSettings))
            .arg(QApplication::translate("MainWindow", "start with default settings"));
    helpMessage += QString("%1           --netctl-auto   - %2\n")
            .arg(isParametrEnable(showNetctlAuto))
            .arg(QApplication::translate("MainWindow", "show netctl-auto window"));
    helpMessage += QString("%1           --settings      - %2\n")
            .arg(isParametrEnable(showSettings))
            .arg(QApplication::translate("MainWindow", "show settings window"));
    helpMessage += QString("   -t %1     --tab %1         - %2\n")
            .arg(QString::number(tabNumber))
            .arg(QApplication::translate("MainWindow", "open a tab with number %1").arg(QString::number(tabNumber)));
    helpMessage += QString("   -v       --version       - %1\n")
            .arg(QApplication::translate("MainWindow", "show version and exit"));
    helpMessage += QString("   -i       --info          - %1\n")
            .arg(QApplication::translate("MainWindow", "show build information and exit"));
    helpMessage += QString("   -h       --help          - %1\n")
            .arg(QApplication::translate("MainWindow", "show this help and exit"));

    QString infoMessage = QString("");
    infoMessage += QApplication::translate("MainWindow", "Build date: %1").
            arg(QString(BUILD_DATE));
    infoMessage += QString("\n%1\n").arg(QApplication::translate("MainWindow", "cmake flags:"));
    infoMessage += QString("\t-DCMAKE_BUILD_TYPE=%1 \\\n").arg(QString(CMAKE_BUILD_TYPE));
    infoMessage += QString("\t-DCMAKE_INSTALL_PREFIX=%1 \\\n").arg(QString(CMAKE_INSTALL_PREFIX));
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
        cout << versionMessage.toUtf8().data() << endl;
        cout << helpMessage.toUtf8().data();
        return 0;
    }
    if (showVersion) {
        cout << versionMessage.toUtf8().data();
        return 0;
    }
    MainWindow w(0, debug, defaultSettings, showAbout, showNetctlAuto, showSettings, tabNumber);
    w.show();
    return a.exec();
}
