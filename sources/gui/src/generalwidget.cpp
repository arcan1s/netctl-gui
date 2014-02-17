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

#include "generalwidget.h"
#include "ui_generalwidget.h"

#include <QKeyEvent>


GeneralWidget::GeneralWidget(QWidget *parent, QString ifaceDir, QString profileDir)
    : QWidget(parent),
      ifaceDirectory(new QDir(ifaceDir)),
      profileDirectory(new QDir(profileDir)),
      ui(new Ui::GeneralWidget)
{
    ui->setupUi(this);
    createActions();
    clear();
    showAdvanced();
}


GeneralWidget::~GeneralWidget()
{
    delete ifaceDirectory;
    delete profileDirectory;
    delete ui;
}


void GeneralWidget::clear()
{
    ui->lineEdit_description->clear();
    ui->comboBox_connection->setCurrentIndex(0);
    ui->comboBox_interface->clear();
    ui->comboBox_interface->addItems(ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    ui->comboBox_bindto->clear();
    ui->comboBox_bindto->addItems(ifaceDirectory->entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    ui->listWidget_bindto->clear();
    ui->comboBox_after->clear();
    ui->comboBox_after->addItems(profileDirectory->entryList(QDir::Files));
    ui->listWidget_after->clear();
    ui->lineEdit_execUpPost->clear();
    ui->lineEdit_execDownPre->clear();
    ui->checkBox_forceConnect->setCheckState(Qt::Unchecked);
}


void GeneralWidget::createActions()
{
    connect(ui->pushButton_generalAdvanced, SIGNAL(clicked(bool)), this, SLOT(showAdvanced()));
    connect(ui->pushButton_after, SIGNAL(clicked(bool)), this, SLOT(addAfter()));
    connect(ui->pushButton_bindto, SIGNAL(clicked(bool)), this, SLOT(addBindTo()));
}


void GeneralWidget::addAfter()
{
    ui->listWidget_after->addItem(ui->comboBox_after->currentText());
}


void GeneralWidget::addBindTo()
{
    ui->listWidget_bindto->addItem(ui->comboBox_bindto->currentText());
}


void GeneralWidget::keyPressEvent(QKeyEvent *pressedKey)
{
    if (pressedKey->key() == Qt::Key_Delete) {
        if (ui->listWidget_bindto->hasFocus() &&
                (ui->listWidget_bindto->currentItem() != 0))
            delete ui->listWidget_bindto->currentItem();
        else if (ui->listWidget_after->hasFocus() &&
                 (ui->listWidget_after->currentItem() != 0))
            delete ui->listWidget_after->currentItem();
    }
}


void GeneralWidget::showAdvanced()
{
    if (ui->pushButton_generalAdvanced->text().indexOf(QString("Show")) > -1) {
        ui->widget_generalAdvanced->setShown(true);
        ui->pushButton_generalAdvanced->setText(QApplication::translate("GeneralWidget", "Hide advanced"));
    }
    else {
        ui->widget_generalAdvanced->setHidden(true);
        ui->pushButton_generalAdvanced->setText(QApplication::translate("GeneralWidget", "Show advanced"));
    }
}


QHash<QString, QString> GeneralWidget::getSettings()
{

}


int GeneralWidget::isOk()
{
    return 0;
}
