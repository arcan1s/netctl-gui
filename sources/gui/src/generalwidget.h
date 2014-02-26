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

#ifndef GENERALWIDGET_H
#define GENERALWIDGET_H

#include <QComboBox>
#include <QDir>
#include <QMap>
#include <QWidget>


namespace Ui {
class GeneralWidget;
}

class GeneralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralWidget(QWidget *parent = 0,
                           QString ifaceDir = QString(""),
                           QString profileDir = QString(""));
    ~GeneralWidget();
    QComboBox *connectionType;
    QMap<QString, QString> getSettings();
    int isOk();
    void setSettings(QMap<QString, QString> settings);

public slots:
    void clear();
    void setShown(bool state);

private slots:
    void addAfter();
    void addBindTo();
    void showAdvanced();

private:
    QDir *ifaceDirectory;
    QDir *profileDirectory;
    Ui::GeneralWidget *ui;
    void createActions();
    void keyPressEvent(QKeyEvent *pressedKey);
};


#endif /* GENERALWIDGET_H */
