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

#include "passwdwidget.h"
#include "ui_passwdwidget.h"

#include <QLineEdit>

#include "mainwindow.h"


PasswdWidget::PasswdWidget(MainWindow *wid)
    : QWidget(wid),
      parent(wid),
      ui(new Ui::PasswdWidget)
{
    ui->setupUi(this);
    createActions();
}


PasswdWidget::~PasswdWidget()
{
    delete ui;
}


// ESC press event
void PasswdWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Escape)
        cancel();
}


void PasswdWidget::createActions()
{
    connect(ui->checkBox_showSymbols, SIGNAL(stateChanged(int)), this, SLOT(setEchoMode(int)));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(passwdApply()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked(bool)), this, SLOT(cancel()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked(bool)), this, SLOT(passwdApply()));
}


void PasswdWidget::setFocusToLineEdit()
{
    ui->lineEdit->setFocus(Qt::ActiveWindowFocusReason);
}


void PasswdWidget::setPassword(const bool mode)
{
    if (mode) {
        ui->checkBox_showSymbols->setCheckState(Qt::Checked);
        setEchoMode(ui->checkBox_showSymbols->checkState());
        ui->label->setText(QApplication::translate("PasswdWidget", "Password"));
    } else {
        ui->lineEdit->setEchoMode(QLineEdit::Normal);
        ui->label->setText(QApplication::translate("PasswdWidget", "ESSID"));
    }
    passwdMode = mode;
}


void PasswdWidget::cancel()
{
    hide();
    parent->updateTabs(2);
    this->~PasswdWidget();
}


void PasswdWidget::passwdApply()
{
    hide();
    if (passwdMode)
        return parent->connectToUnknownEssid(ui->lineEdit->text());
    else
        return parent->setHiddenName(ui->lineEdit->text());
}


void PasswdWidget::setEchoMode(const int mode)
{
    if (mode == 0)
        ui->lineEdit->setEchoMode(QLineEdit::Password);
    else
        ui->lineEdit->setEchoMode(QLineEdit::Normal);
}
