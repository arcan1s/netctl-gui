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

#ifndef TUNTAPWIDGET_H
#define TUNTAPWIDGET_H

#include <QWidget>


namespace Ui {
class TuntapWidget;
}

class TuntapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TuntapWidget(QWidget *parent = 0);
    ~TuntapWidget();
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(const QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(const bool state);

private:
    Ui::TuntapWidget *ui;
};


#endif /* TUNTAPWIDGET_H */
