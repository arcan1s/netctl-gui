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

#ifndef VLANWIDGET_H
#define VLANWIDGET_H

#include <QWidget>


namespace Ui {
class VlanWidget;
}

class VlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VlanWidget(QWidget *parent = 0);
    ~VlanWidget();
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(bool state);

private:
    Ui::VlanWidget *ui;
};


#endif /* VLANWIDGET_H */
