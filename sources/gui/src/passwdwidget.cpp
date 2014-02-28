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

#include "passwdwidget.h"
#include "ui_passwdwidget.h"

#include "mainwindow.h"


PasswdWidget::PasswdWidget(MainWindow *wid)
    : QWidget(wid),
      parent(wid),
      ui(new Ui::PasswdWidget)
{
    ui->setupUi(this);
    cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
    okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    createActions();
}


PasswdWidget::~PasswdWidget()
{
    delete cancelButton;
    delete okButton;
    delete ui;
}


// ESC press event
void PasswdWidget::keyPressEvent(const QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Escape)
        cancel();
}


void PasswdWidget::createActions()
{
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(passwdApply()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(passwdApply()));
}


void PasswdWidget::setFocusToLineEdit()
{
    ui->lineEdit->setFocus(Qt::ActiveWindowFocusReason);
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
    return parent->connectToUnknownEssid(ui->lineEdit->text());
}
