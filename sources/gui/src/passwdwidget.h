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

#ifndef PASSWDWIDGET_H
#define PASSWDWIDGET_H

#include <QKeyEvent>
#include <QPushButton>
#include <QWidget>


class MainWindow;

namespace Ui {
class PasswdWidget;
}

class PasswdWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PasswdWidget(MainWindow *wid = 0);
    ~PasswdWidget();

public slots:
    void setFocusToLineEdit();
    void setPassword(const bool mode = true);

private slots:
    void cancel();
    void passwdApply();
    void setEchoMode(const int mode);

private:
    MainWindow *parent;
    Ui::PasswdWidget *ui;
    // ESC pressed event
    void keyPressEvent(QKeyEvent *pressedKey);
    void createActions();
    bool passwdMode = true;
};


#endif /* PASSWDWIDGET_H */
