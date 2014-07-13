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
    bool showHelp = false;
    bool showNetctlAuto = false;
    bool showSettings = false;
    bool showVersion = false;
    int tabNumber = 1;
    for (int i=1; i<argc; i++) {
        // debug
        if ((QString(argv[i]) == QString("-d")) || (QString(argv[i]) == QString("--debug"))) {
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
    helpMessage += QApplication::translate("MainWindow", "netctl-gui [ -d | --debug ] [ --default ] [ --netctl-auto ] [ --settings ]\n");
    helpMessage += QApplication::translate("MainWindow", "           [ -t NUM | --tab NUM ] [ -v | --version ] [ -h | --help]\n\n");
    helpMessage += QApplication::translate("MainWindow", "Parametrs:\n");
    helpMessage += QApplication::translate("MainWindow", "%1  -d       --debug         - print debug information\n")
            .arg(isParametrEnable(debug));
    helpMessage += QApplication::translate("MainWindow", "%1           --default       - start with default settings\n")
            .arg(isParametrEnable(defaultSettings));
    helpMessage += QApplication::translate("MainWindow", "%1           --netctl-auto   - show netctl-auto window\n")
            .arg(isParametrEnable(showNetctlAuto));
    helpMessage += QApplication::translate("MainWindow", "%1           --settings      - show settings window\n")
            .arg(isParametrEnable(showSettings));
    helpMessage += QApplication::translate("MainWindow", "   -t %1     --tab %1         - open a tab with number %1\n")
            .arg(QString::number(tabNumber));
    helpMessage += QApplication::translate("MainWindow", "   -v       --version       - show version and exit\n");
    helpMessage += QApplication::translate("MainWindow", "   -h       --help          - show this help and exit\n");
    QString versionMessage = QString("");
    versionMessage += QApplication::translate("MainWindow", "                                    Netctl GUI\n");
    versionMessage += QApplication::translate("MainWindow", "Version : %1                                                  License : GPLv3\n")
            .arg(QString(VERSION));
    versionMessage += QApplication::translate("MainWindow", "                                                    Evgeniy Alekseev aka arcanis\n");
    versionMessage += QApplication::translate("MainWindow", "                                                    E-mail : esalexeev@gmail.com\n");
    // running
    if (error) {
        cout << errorMessage.toUtf8().data();
        cout << helpMessage.toUtf8().data();
        return 127;
    }
    if (showHelp) {
        cout << versionMessage.toUtf8().data();
        cout << helpMessage.toUtf8().data();
        return 0;
    }
    if (showVersion) {
        cout << versionMessage.toUtf8().data();
        return 0;
    }
    MainWindow w(0, defaultSettings, debug, showNetctlAuto, showSettings, tabNumber);
    w.show();
    return a.exec();
}
