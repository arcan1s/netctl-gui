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

#ifndef NETCTLAUTOWINDOW_H
#define NETCTLAUTOWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>


class MainWindow;

namespace Ui {
class NetctlAutoWindow;
}

class NetctlAutoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetctlAutoWindow(QWidget *parent = 0,
                              const QMap<QString, QString> settings = QMap<QString, QString>(),
                              const bool debugCmd = false);
    ~NetctlAutoWindow();
    Qt::ToolBarArea getToolBarArea();

public slots:
    void showWindow();
    void updateToolBarState(const Qt::ToolBarArea area = Qt::TopToolBarArea);

private slots:
    // table
    void netctlAutoContextualMenu(const QPoint &pos);
    void netctlAutoUpdateTable();
    // netctl-auto
    void netctlAutoDisableAllProfiles();
    void netctlAutoEnableProfile();
    void netctlAutoEnableAllProfiles();
    void netctlAutoStartProfile();
    void netctlAutoRefreshButtons(QTableWidgetItem *current, QTableWidgetItem *previous);
    // service
    void netctlAutoEnableService();
    void netctlAutoRestartService();
    void netctlAutoStartService();

private:
    // ui
    Ui::NetctlAutoWindow *ui = nullptr;
    MainWindow *mainWindow = nullptr;
    // backend
    QString checkStatus(const bool statusBool, const bool nullFalse = false);
    void createActions();
    bool debug = false;
    bool useHelper = true;
};


#endif /* NETCTLAUTOWINDOW_H */
