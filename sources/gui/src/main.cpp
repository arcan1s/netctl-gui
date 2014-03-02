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


#include <QApplication>

#include <QDir>
#include <QTextStream>
#include <QTranslator>
#include <iostream>

#include "mainwindow.h"
#include "version.h"


using namespace std;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // translation
    QString configPath = QDir::homePath() + QDir::separator() + QString(".config") +
            QDir::separator() + QString("netctl-gui.conf");
    QFile configFile(configPath);
    QString fileStr;
    QString language = QString("english");
    if (configFile.open(QIODevice::ReadOnly))
        while (true) {
            fileStr = QString(configFile.readLine());
            if (fileStr[0] != '#') {
                if (fileStr.contains(QString("LANGUAGE=")))
                    language = fileStr.split(QString("="))[1]
                            .remove(QString(" "))
                            .trimmed();
            }
            if (configFile.atEnd())
                break;
        }
    configFile.close();
    QTranslator translator;
    translator.load(QString(":/translations/") + language);
    a.installTranslator(&translator);

    QString helpMessage = QString("");
    helpMessage += QApplication::translate("MainWindow", "                                    Netctl GUI\n");
    helpMessage += QApplication::translate("MainWindow", "Version : %1                                                  License : GPLv3\n")
            .arg(QString(VERSION));
    helpMessage += QApplication::translate("MainWindow", "                                                    Evgeniy Alekseev aka arcanis\n");
    helpMessage += QApplication::translate("MainWindow", "                                                    E-mail : esalexeev@gmail.com\n\n");
    helpMessage += QApplication::translate("MainWindow", "Usage:\n");
    helpMessage += QApplication::translate("MainWindow", "netctl-gui [ --default ] [ -t NUM | --tab NUM ] [ -h | --help]\n\n");
    helpMessage += QApplication::translate("MainWindow", "Parametrs:\n");
    helpMessage += QApplication::translate("MainWindow", "           --default    - start with default settings\n");
    helpMessage += QApplication::translate("MainWindow", "  -t NUM   --tab NUM    - open a tab with number NUM\n");
    helpMessage += QApplication::translate("MainWindow", "  -h       --help       - show this help and exit\n");
    bool defaultSettings = false;
    int tabNumber = 0;

    for (int i=1; i<argc; i++) {
        // help message
        if (((argv[i][0] == '-') && (argv[i][1] == 'h') && (argv[i][2] == '\0')) ||
                ((argv[i][0] == '-') && (argv[i][1] == '-') && (argv[i][2] == 'h') && (argv[i][3] == 'e') &&
                 (argv[i][4] == 'l') && (argv[i][5] == 'p') && (argv[i][6] == '\0'))) {
            cout << helpMessage.toUtf8().data();
            return 0;
        }
        // default settings
        else if ((argv[i][0] == '-') && (argv[i][1] == '-') && (argv[i][2] == 'd') && (argv[i][3] == 'e') &&
                 (argv[i][4] == 'f') && (argv[i][5] == 'a') && (argv[i][6] == 'u') && (argv[i][7] == 'l') &&
                 (argv[i][8] == 't') && (argv[i][9] == '\0')) {
            defaultSettings = true;
        }
        // tab number
        else if (((argv[i][0] == '-') && (argv[i][1] == 't') && (argv[i][2] == '\0')) ||
                 ((argv[i][0] == '-') && (argv[i][1] == '-') && (argv[i][2] == 't') &&
                  (argv[i][3] == 'a') && (argv[i][4] == 'b') && (argv[i][5] == '\0'))) {
            if (atoi(argv[i+1]) > 3)
                tabNumber = 3;
            else if (atoi(argv[i+1]) < 1)
                tabNumber = 1;
            else
                tabNumber = atoi(argv[i+1]);
            i++;
        }
    }

    MainWindow w(0, defaultSettings, tabNumber);
    w.show();
    return a.exec();
}
