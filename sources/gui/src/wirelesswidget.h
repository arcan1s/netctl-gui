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

#ifndef WIRELESSWIDGET_H
#define WIRELESSWIDGET_H

#include <QDir>
#include <QMap>
#include <QWidget>


namespace Ui {
class WirelessWidget;
}

class WirelessWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WirelessWidget(QWidget *parent = 0,
                            QMap<QString, QString> settings = QMap<QString, QString>());
    ~WirelessWidget();
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(const QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(const bool state);

private slots:
    void addDriver();
    void addFreq();
    void addOption();
    void changeSecurity(const QString currentText);
    void showAdvanced();
    void selectWpaConfig();

private:
    QDir *rfkillDirectory;
    Ui::WirelessWidget *ui;
    void createActions();
    void keyPressEvent(const QKeyEvent *pressedKey);
};


#endif /* WIRELESSWIDGET_H */
