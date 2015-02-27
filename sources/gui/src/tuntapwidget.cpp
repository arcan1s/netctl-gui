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


QMap<QString, QString> TuntapWidget::getSettings()
{
    QMap<QString, QString> settings;

    if (isOk() != 0) return settings;

    settings[QString("Mode")] = QString("'%1'").arg(ui->comboBox_mode->currentText());
    settings[QString("User")] = QString("'%1'").arg(ui->lineEdit_user->text());
    settings[QString("Group")] = QString("'%1'").arg(ui->lineEdit_group->text());

    return settings;
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


void TuntapWidget::setSettings(const QMap<QString, QString> settings)
{
    clear();

    if (settings.contains(QString("Mode"))) {
        int index = ui->comboBox_mode->findText(settings[QString("Mode")]);
        ui->comboBox_mode->setCurrentIndex(index);
    }
    if (settings.contains(QString("User")))
        ui->lineEdit_user->setText(settings[QString("User")]);
    if (settings.contains(QString("Group")))
        ui->lineEdit_group->setText(settings[QString("Group")]);
}
