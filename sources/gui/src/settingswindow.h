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

#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <QTreeWidgetItem>


class MainWindow;

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0,
                            const bool debugCmd = false,
                            const QString configFile = QString(""));
    ~SettingsWindow();
    QMap<QString, QString> getDefault();
    QMap<QString, QString> getSettings();

public slots:
    void closeWindow();
    void restoreSettings();
    void setDefault();
    void showWindow();

private slots:
    void addLanguages();
    void changePage(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void saveSettings();
    void setTray();
    void updateHelper();
    // buttons
    void selectAbstractSomething();
    void startHelper();

private:
    bool debug;
    QString file;
    Ui::SettingsWindow *ui;
    void createActions();
    // ESC pressed event
    void keyPressEvent(QKeyEvent *pressedKey);
    QMap<QString, QString> readSettings();
    void setSettings(const QMap<QString, QString> settings);
};


#endif /* SETTINGSWINDOW_H */
