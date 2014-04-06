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

#ifndef PPPOEWIDGET_H
#define PPPOEWIDGET_H

#include <QWidget>


namespace Ui {
class PppoeWidget;
}

class PppoeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PppoeWidget(QWidget *parent = 0);
    ~PppoeWidget();
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(const QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(const bool state);

private slots:
    void changeMode(const QString currentText);
    void selectOptionsFile();
    void showAdvanced();

private:
    Ui::PppoeWidget *ui;
    void createActions();
    void createFilter();
};


#endif /* PPPOEWIDGET_H */
