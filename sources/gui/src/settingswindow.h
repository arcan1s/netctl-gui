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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>


class MainWindow;

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(MainWindow *wid = 0,
                            const QString configFile = QString(""));
    ~SettingsWindow();
    QMap<QString, QString> getDefault();
    QMap<QString, QString> getSettings();

public slots:
    void setDefault();
    void showWindow();

private slots:
    void addLanguages();
    void saveSettings();
    // buttons
    void selectIfaceDir();
    void selectNetctlPath();
    void selectProfileDir();
    void selectRfkillDir();
    void selectSudoPath();
    void selectWpaCliPath();
    void selectWpaSupPath();

private:
    MainWindow *parent;
    QString file;
    Ui::SettingsWindow *ui;
    void createActions();
    // ESC pressed event
    void keyPressEvent(QKeyEvent *pressedKey);
    QMap<QString, QString> readSettings();
    void setSettings(const QMap<QString, QString> settings);
};


#endif /* SETTINGSWINDOW_H */
