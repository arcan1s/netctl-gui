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

#include "tuntapwidget.h"
#include "ui_tuntapwidget.h"


TuntapWidget::TuntapWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TuntapWidget)
{
    ui->setupUi(this);
    clear();
}


TuntapWidget::~TuntapWidget()
{
    delete ui;
}


void TuntapWidget::clear()
{
    ui->comboBox_mode->setCurrentIndex(0);
    ui->lineEdit_user->setText(QString("nobody"));
    ui->lineEdit_group->setText(QString("nobody"));
}


QHash<QString, QString> TuntapWidget::getSettings()
{
    QHash<QString, QString> tuntapSettings;

    if (isOk() == 0) {
        tuntapSettings[QString("Mode")] = QString("'") + ui->comboBox_mode->currentText() + QString("'");
        tuntapSettings[QString("User")] = QString("'") + ui->lineEdit_user->text() + QString("'");
        tuntapSettings[QString("Group")] = QString("'") + ui->lineEdit_group->text() + QString("'");
        clear();
    }

    return tuntapSettings;
}


int TuntapWidget::isOk()
{
    // empty username
    if (ui->lineEdit_user->text().isEmpty())
        return 1;
    // empty group name
    if (ui->lineEdit_group->text().isEmpty())
        return 2;
    // all fine
    return 0;
}


void TuntapWidget::setSettings(QHash<QString, QString> settings)
{
    if (settings.contains(QString("Mode")))
        for (int i=0; i<ui->comboBox_mode->count(); i++)
            if (settings[QString("Mode")].remove(QString("'")) == ui->comboBox_mode->itemText(i))
                ui->comboBox_mode->setCurrentIndex(i);
    if (settings.contains(QString("User")))
        ui->lineEdit_user->setText(settings[QString("User")].remove(QString("'")));
    if (settings.contains(QString("Group")))
        ui->lineEdit_group->setText(settings[QString("Group")].remove(QString("'")));
}
